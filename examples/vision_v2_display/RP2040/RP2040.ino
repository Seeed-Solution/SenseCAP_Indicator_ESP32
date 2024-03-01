#include <ArduinoJson.h>
#include <Base64.h>
#include <PacketSerial.h>
#include <Seeed_Arduino_SSCMA.h>
#include <Wire.h>


#define ARDUINO_ARCH_RP2040 1
#define pcSerial            Serial
#ifdef ARDUINO_ARCH_RP2040 /////////////////////////
    #include <SoftwareSerial.h>
    #define ESP_rxPin            17
    #define ESP_txPin            16
    #define espSerial            Serial1
    #define ESP32_COMM_BAUD_RATE (921600)
    // #define ESP32_COMM_BAUD_RATE              (115200)
    // Right Grove -> Check https://files.seeedstudio.com/wiki/SenseCAP/SenseCAP_Indicator/grove.png
    #define txPin                20
    #define rxPin                21
    // SoftwareSerial atSerial = SoftwareSerial(rxPin, txPin);
    #define atSerial             Serial2
    #define _SDA                 26
    #define _SCL                 27
#elif ESP32 // XIAO /////////////////////////
    #include <HardwareSerial.h>
HardwareSerial atSerial(0);
#else /////////////////////////
    #define atSerial Serial2
#endif

#define _LOG 0 // Disable for high frequency

const char compile_date[] = __DATE__ " " __TIME__;
SSCMA      AI;
SSCMA      AI_I2C;

PacketSerial myPacketSerial;
#define PKT_TYPE_CMD_BEEP_ON     0xA1
#define PKT_TYPE_CMD_SHUTDOWN    0xA3
#define PKT_TYPE_CMD_POWER_ON    0xA4
#define PKT_TYPE_CMD_MODEL_TITLE 0xA5
/************************ recv cmd from esp32  ****************************/
static bool shutdown_flag      = false;

static volatile bool get_title = false;

void onPacketReceived(const uint8_t *buffer, size_t size)
{
    if (size < 1) {
        return;
    }
    switch (buffer[0]) {
        case PKT_TYPE_CMD_SHUTDOWN: {
            Serial.println("cmd shutdown");
            shutdown_flag = true;
            break;
        }
        case PKT_TYPE_CMD_BEEP_ON: {
            beep_on();
            break;
        }
        case PKT_TYPE_CMD_MODEL_TITLE: {
            get_title = true;
            break;
        }
        default:
            break;
    }
}
/************************ beep ****************************/

#define Buzzer 19 // Buzzer GPIO

void beep_init(void)
{
    pinMode(Buzzer, OUTPUT);
}
void beep_off(void)
{
    digitalWrite(19, LOW);
}
void beep_on(void)
{
    analogWrite(Buzzer, 127);
    delay(50);
    analogWrite(Buzzer, 0);
}

void setup()
{
#ifdef pcSerial
    pcSerial.begin(9600);
    // while (!pcSerial)
    //   ;
    pcSerial.println(compile_date);
#endif

#ifdef ARDUINO_ARCH_RP2040
    #ifdef pcSerial
    pcSerial.println("Setting ESP32 UART For inner RP2040");
    #endif
    espSerial.setRX(ESP_rxPin);
    espSerial.setTX(ESP_txPin);
    espSerial.begin(ESP32_COMM_BAUD_RATE);

    myPacketSerial.setStream(&espSerial);
    myPacketSerial.setPacketHandler(&onPacketReceived);
    #ifdef pcSerial
    pcSerial.println("Using UART OF AI");
    #endif
    atSerial.setFIFOSize(16 * 1024);
    AI.begin(&atSerial);
    // AI_I2C.begin();// For I2C Port
#endif
}

void AI_func(SSCMA &instance)
{
    if (!instance.invoke(1, false, true)) {
        JsonDocument doc; // Adjust size as needed

        // Performance metrics
        int index          = 0;
        doc["preprocess"]  = instance.perf().prepocess;
        doc["inference"]   = instance.perf().inference;
        doc["postprocess"] = instance.perf().postprocess;

        // Last image
        if (instance.last_image().length()) {
            doc["img"] = instance.last_image();
        }
        // Boxes
        index = 0;
        for (auto &box : instance.boxes()) {
            int currentBox[6] = {box.x, box.y, box.w, box.h, box.score, box.target};
            copyArray(currentBox, doc["boxes"][index++]);
        }

        // Classes
        index = 0;
        for (auto &classObj : instance.classes()) {
            int currentClass[2] = {classObj.target, classObj.score};
            copyArray(currentClass, doc["classes"][index++]);
        }
        // Points
        index = 0;
        for (auto &point : instance.points()) {
            int currtentPoint[4] = {point.target, point.score, point.x, point.y};
            copyArray(currtentPoint, doc["points"][index++]);
        }

        // Keypoints
        index = 0;
        for (auto &keypoint : instance.keypoints()) {
            int currentKeypoint[6] = {keypoint.box.target, keypoint.box.score, keypoint.box.x, keypoint.box.y, keypoint.box.w, keypoint.box.h};
            copyArray(currentKeypoint, doc["keypoints"][index]);
            int j = 0;
            for (auto &point : keypoint.points) {
                int arrayKeypoints[2] = {point.x, point.y};
                copyArray(arrayKeypoints, doc["keypoints"][index]["points"][j++]);
            }
            index++;
        }
        #if _LOG
        serializeJsonPretty(doc, pcSerial); // Serialize and print the JSON document
        #endif
#ifdef espSerial
        serializeJson(doc, espSerial); // Serialize and print the JSON document
#endif
    }

    if (get_title) {
        get_title = false;
        #if _LOG
        pcSerial.println("Recieved get title event");
        #endif
        if (!instance.fetch_info()) {
            // base64 解码成 JSON
            String base64String    = instance.info();
            // pcSerial.printf("instance.info(): %s \n", base64String.c_str());

            int  inputStringLength = base64String.length() + 1;
            char inputString[inputStringLength];
            base64String.toCharArray(inputString, inputStringLength);

            int  decodedLength = Base64.decodedLength(inputString, inputStringLength - 1); // 计算解码长度
            char decodedString[decodedLength + 1];                                         // 分配额外的空间用于空终止符

            Base64.decode(decodedString, inputString, inputStringLength - 1); // 解码Base64字符串
            decodedString[decodedLength] = '\0';                              // 手动添加空终止符确保字符串正确终止

            // pcSerial.print("Decoded string is:\t");
            // pcSerial.println(decodedString);

            JsonDocument         doc;                                         // 创建一个足够大的JsonDocument
            DeserializationError error = deserializeJson(doc, decodedString); // 将解码后的字符串反序列化到JSON文档

            if (error) { // 检查反序列化是否成功
                // pcSerial.print("deserializeJson() failed: ");
                // pcSerial.println(error.c_str());
                return;
            }
            // 获取JSON的info 信息
            JsonDocument send_doc;
            send_doc["model_name"] = doc["name"];
            #if _LOG
            serializeJson(send_doc, pcSerial);
            #endif
#ifdef espSerial
            serializeJson(send_doc, espSerial); // Serialize and print the JSON document
#endif
        }
    }
}

void loop()
{ // For UART Port
    AI_func(AI);
    // AI_func(AI_I2C); // For I2C Port 
}

void setup1()
{ // For I2C Port
    beep_init();
    // Serial.println("Beep init");
    delay(500);
    // beep_on();

#ifdef pcSerial
    pcSerial.println("Setting Wire1 for AI_I2C");
#endif
    Wire1.setSCL(_SCL);
    Wire1.setSDA(_SDA);
}

void loop1()
{
    myPacketSerial.update();
    if (myPacketSerial.overflow()) {
    }
}