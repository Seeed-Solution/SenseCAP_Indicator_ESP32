#include <Arduino.h>
#include <ArduinoJson.h>
#include <Base64.h>
#include <PacketSerial.h>
#include <Seeed_Arduino_SSCMA.h>
#include <Wire.h>

#define USING_UART 1

#if USING_UART
SSCMA AI;
#else
SSCMA AI_I2C;
#endif

#define ARDUINO_ARCH_RP2040 1
#define pcSerial Serial
#ifdef ARDUINO_ARCH_RP2040 /////////////////////////
#include <SoftwareSerial.h>
#define ESP_rxPin 17
#define ESP_txPin 16
#define espSerial Serial1
#define ESP32_COMM_BAUD_RATE (921600)
// #define ESP32_COMM_BAUD_RATE              (115200)
// Right Grove -> Check https://files.seeedstudio.com/wiki/SenseCAP/SenseCAP_Indicator/grove.png
#define txPin 20 // Write
#define rxPin 21 // Yellow
// SoftwareSerial atSerial = SoftwareSerial(rxPin, txPin);
#define atSerial Serial2
#define _SDA 26 // Write
#define _SCL 27 // Yellow
#elif ESP32     // XIAO /////////////////////////
#include <HardwareSerial.h>
HardwareSerial atSerial(0);
#else /////////////////////////
#define atSerial Serial2
#endif

#define _LOG 0 // Disable for high frequency
#define USING_480 0
const char compile_date[] = __DATE__ " " __TIME__;

PacketSerial myPacketSerial;
#define PKT_TYPE_CMD_BEEP_ON 0xA1
#define PKT_TYPE_CMD_SHUTDOWN 0xA3
#define PKT_TYPE_CMD_POWER_ON 0xA4
#define PKT_TYPE_CMD_MODEL_TITLE 0xA5
/************************ recv cmd from esp32  ****************************/
static bool shutdown_flag = false;

static volatile bool get_title = false;

void onPacketReceived(const uint8_t* buffer, size_t size) {
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

void beep_init(void) {
    pinMode(Buzzer, OUTPUT);
}
void beep_off(void) {
    digitalWrite(19, LOW);
}
void beep_on(void) {
    analogWrite(Buzzer, 127);
    delay(50);
    analogWrite(Buzzer, 0);
}

/**
 * @brief
 *
 * @param instance
 * @attention
 * {"keypoints":[{"box":[0,95,120,120,239,240],"points":[[127,77],[153,62],[98,59],[182,91],[61,88],[222,209],[10,198],[241,249],[0,234],[193,220],[25,183],[179,293],[61,285],[135,256],[72,220],[116,271],[197,216]]}]}
 */
inline void AI_func(SSCMA& instance) {
    if (!instance.invoke(1, false, true)) {
        JsonDocument doc; // Adjust size as needed

        // Performance metrics
        // auto& perf = instance.perf();
        // doc["preprocess"] = perf.prepocess;
        // doc["inference"] = perf.inference;
        // doc["postprocess"] = perf.postprocess;
        // doc["preprocess"] = instance.perf().prepocess;
        // doc["inference"] = instance.perf().inference;
        // doc["postprocess"] = instance.perf().postprocess;

        // if (instance.last_image().length()) {
        //     doc["img"] = instance.last_image();
        // }
        // Boxes
        auto& boxes = instance.boxes();
        int index = 0;
        for (auto& box : boxes) {
            int currentBox[6] = {box.x, box.y, box.w, box.h, box.score, box.target};
            copyArray(currentBox, doc["boxes"][index++]); // Assuming copyArray directly modifies the doc
        }
        // for (auto& box : instance.boxes()) {
        //     int currentBox[6] = {box.x, box.y, box.w, box.h, box.score, box.target};
        //     copyArray(currentBox, doc["boxes"][index++]);
        // }

        // Classes
        // index = 0;
        // for (auto& classObj : instance.classes()) {
        //     int currentClass[2] = {classObj.score, classObj.target};
        //     copyArray(currentClass, doc["classes"][index++]);
        // }
        auto& classes = instance.classes();
        index = 0;
        for (auto& classObj : classes) {
            int currentClass[2] = {classObj.score, classObj.target};
            copyArray(currentClass, doc["classes"][index++]);
        }
        // Points
        // index = 0;
        // for (auto& point : instance.points()) {
        //     int currtentPoint[4] = {point.x, point.y, point.score, point.target};
        //     copyArray(currtentPoint, doc["points"][index++]);
        // }
        auto& points = instance.points();
        index = 0;
        for (auto& point : points) {
            int currentPoint[4] = {point.x, point.y, point.score, point.target};
            copyArray(currentPoint, doc["points"][index++]);
        }

        // Keypoints
        // index = 0;
        // for (auto& keypoint : instance.keypoints()) { // how many people
        //     int currentKeypoint[6] = {keypoint.box.x, keypoint.box.y,     keypoint.box.w,
        //                               keypoint.box.h, keypoint.box.score, keypoint.box.target};
        //     copyArray(currentKeypoint, doc["keypoints"][index]["box"]);
        //     int j = 0;
        //     for (auto& point : keypoint.points) { // 一个人的所有点
        //         // pcSerial.printf("point.x: %d, point.y: %d\n", point.x, point.y);
        //         int arrayKeypoints[2] = {point.x, point.y};
        //         copyArray(arrayKeypoints, doc["keypoints"][index]["points"][j++]);
        //     }
        //     index++;
        // }
        auto& keypoints = instance.keypoints();
        index = 0;
        for (auto& keypoint : keypoints) {
            int currentKeypoint[6] = {keypoint.box.x, keypoint.box.y,     keypoint.box.w,
                                      keypoint.box.h, keypoint.box.score, keypoint.box.target};
            copyArray(currentKeypoint, doc["keypoints"][index]["box"]);
            int j = 0;
            for (auto& point : keypoint.points) {
                int arrayKeypoints[2] = {point.x, point.y};
                copyArray(arrayKeypoints, doc["keypoints"][index]["points"][j++]);
            }
            index++;
        }

                // Last image
        auto lastImage = instance.last_image();
        if (lastImage.length()) {
            doc["img"] = lastImage;
        }
#if _LOG
        // serializeJsonPretty(doc, pcSerial); // Serialize and print the JSON document
        serializeJson(doc, pcSerial); // Serialize and print the JSON document
        pcSerial.println();
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
            String base64String = instance.info();
            // pcSerial.printf("instance.info(): %s \n", base64String.c_str());

            int inputStringLength = base64String.length() + 1;
            char inputString[inputStringLength];
            base64String.toCharArray(inputString, inputStringLength);

            int decodedLength = Base64.decodedLength(inputString, inputStringLength - 1); // 计算解码长度
            char decodedString[decodedLength + 1]; // 分配额外的空间用于空终止符

            Base64.decode(decodedString, inputString, inputStringLength - 1); // 解码Base64字符串
            decodedString[decodedLength] = '\0'; // 手动添加空终止符确保字符串正确终止

            // pcSerial.print("Decoded string is:\t");
            // pcSerial.println(decodedString);

            JsonDocument doc;                                                 // 创建一个足够大的JsonDocument
            DeserializationError error = deserializeJson(doc, decodedString); // 将解码后的字符串反序列化到JSON文档

            if (error) { // 检查反序列化是否成功
                // pcSerial.print("deserializeJson() failed: ");
                // pcSerial.println(error.c_str());
                return;
            }
            // 获取JSON的info 信息
            JsonDocument send_doc;
            send_doc["model_name"] = doc["name"];
#ifdef espSerial
            serializeJson(send_doc, espSerial); // Serialize and print the JSON document
#endif
#if _LOG
            serializeJson(send_doc, pcSerial);
#endif
        }
    }
}

void setup() {
#ifdef pcSerial
    pcSerial.begin(ESP32_COMM_BAUD_RATE);
    // pcSerial.begin(115200);
    // while (!pcSerial)
    //     ;
    pcSerial.println(compile_date);
#endif

#ifdef pcSerial
    pcSerial.println("Setting ESP32 UART For inner RP2040");
#endif
    espSerial.setRX(ESP_rxPin);
    espSerial.setTX(ESP_txPin);
    espSerial.begin(ESP32_COMM_BAUD_RATE);

    myPacketSerial.setStream(&espSerial);
    myPacketSerial.setPacketHandler(&onPacketReceived);

#if USING_UART
    atSerial.setFIFOSize(16 * 1024);
#ifdef pcSerial
    pcSerial.println("Using UART OF AI");
#endif
    AI.begin(&atSerial);
    #if USING_480
    AI.fetch480();
    #endif 
#else
#ifdef pcSerial
    pcSerial.println("Setting Wire1 for AI_I2C");
#endif
    Wire1.setSCL(_SCL);
    Wire1.setSDA(_SDA);
    AI_I2C.begin(&Wire1); // For I2C Port
    #if USING_480
    AI_I2C.fetch480();
    #endif
#endif
}

void setup1() { // For I2C Port
    beep_init();
    // Serial.println("Beep init");
    delay(500);
    // beep_on();
}

void loop() { // For UART Port
#if USING_UART
    AI_func(AI);
#else
    AI_func(AI_I2C); // For I2C Port
#endif
}

void loop1() {
    myPacketSerial.update();
    if (myPacketSerial.overflow()) {
    }
}