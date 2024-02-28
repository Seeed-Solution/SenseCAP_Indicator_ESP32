#include <Wire.h>
#include <ArduinoJson.h>
#include <Seeed_Arduino_SSCMA.h>
#include <PacketSerial.h>
#include <Base64.h>


// #define ARDUINO_ARCH_RP2040 1
#define pcSerial Serial
#ifdef ARDUINO_ARCH_RP2040  /////////////////////////
#include <SoftwareSerial.h>
#define ESP_rxPin 17
#define ESP_txPin 16
#define espSerial Serial1
#define ESP32_COMM_BAUD_RATE (921600)
// #define ESP32_COMM_BAUD_RATE              (115200)
// Right Grove -> Check https://files.seeedstudio.com/wiki/SenseCAP/SenseCAP_Indicator/grove.png
#define txPin 20
#define rxPin 21
//SoftwareSerial atSerial = SoftwareSerial(rxPin, txPin);
#define atSerial Serial2
#define _SDA 26
#define _SCL 27
#elif ESP32  // XIAO /////////////////////////
#include <HardwareSerial.h>
HardwareSerial atSerial(0);
#else  /////////////////////////
#define atSerial Serial2
#endif
const char compile_date[] = __DATE__ " " __TIME__;
SSCMA AI;
// SSCMA AI_I2C;

PacketSerial myPacketSerial;
#define PKT_TYPE_CMD_BEEP_ON 0xA1
#define PKT_TYPE_CMD_SHUTDOWN 0xA3
#define PKT_TYPE_CMD_POWER_ON 0xA4
#define PKT_TYPE_CMD_MODEL_TITLE 0xA5
/************************ recv cmd from esp32  ****************************/
static bool shutdown_flag = false;

static volatile bool get_title = false;

void onPacketReceived(const uint8_t *buffer, size_t size) {
  if (size < 1) {
    return;
  }
  switch (buffer[0]) {
    case PKT_TYPE_CMD_SHUTDOWN:
      {
        Serial.println("cmd shutdown");
        shutdown_flag = true;
        break;
      }
    case PKT_TYPE_CMD_BEEP_ON:
      {
        // Serial.println("cmd shutdown");
        // shutdown_flag = true;
        // watchdog_reboot()
        beep_on();
        break;
      }
    case PKT_TYPE_CMD_MODEL_TITLE:
      {
        get_title = true;
        break;
      }
    default:
      break;
  }
}
/************************ beep ****************************/

#define Buzzer 19  //Buzzer GPIO

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

void setup() {
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
#endif

  pcSerial.printf("AI _serialType: %d\n", AI._serialType);
}

void loop() {
  if (!AI.invoke(1, false, true)) {
    JsonDocument doc;  // Adjust size as needed

    // Performance metrics
    int index = 0;
    doc["preprocess"] = AI.perf().prepocess;
    doc["inference"] = AI.perf().inference;
    doc["postprocess"] = AI.perf().postprocess;

    // Last image
    if (AI.last_image().length()) {
      doc["img"] = AI.last_image();
    }
    // Boxes
    index = 0;
    for (auto &box : AI.boxes()) {
      int currentBox[6] = { box.x, box.y, box.w, box.h, box.score, box.target };
      copyArray(currentBox, doc["boxes"][index++]);
    }

    // Classes
    index = 0;
    for (auto &classObj : AI.classes()) {
      int currentClass[2] = { classObj.target, classObj.score };
      copyArray(currentClass, doc["classes"][index++]);
    }
    // Points
    index = 0;
    for (auto &point : AI.points()) {
      int currtentPoint[4] = { point.target, point.score, point.x, point.y };
      copyArray(currtentPoint, doc["points"][index++]);
    }

    // Keypoints
    index = 0;
    for (auto &keypoint : AI.keypoints()) {
      int currentKeypoint[6] = { keypoint.box.target, keypoint.box.score, keypoint.box.x, keypoint.box.y, keypoint.box.w, keypoint.box.h };
      copyArray(currentKeypoint, doc["keypoints"][index]);
      int j = 0;
      for (auto &point : keypoint.points) {
        int arrayKeypoints[2] = { point.x, point.y };
        copyArray(arrayKeypoints, doc["keypoints"][index]["points"][j++]);
      }
      index++;
    }
    // serializeJsonPretty(doc, pcSerial); // Serialize and print the JSON document
#ifdef espSerial
    serializeJson(doc, espSerial);  // Serialize and print the JSON document
#endif
  }


  if (get_title) {
    get_title = false;
    pcSerial.println("Recieved get title event");
    if (!AI.get_info()) {
      // base64 解码成 JSON
      String base64String = AI.info();
      // pcSerial.printf("AI.info(): %s \n", base64String.c_str());

      int inputStringLength = base64String.length() + 1;
      char inputString[inputStringLength];
      base64String.toCharArray(inputString, inputStringLength);

      int decodedLength = Base64.decodedLength(inputString, inputStringLength - 1);  // 计算解码长度
      char decodedString[decodedLength + 1];                                         // 分配额外的空间用于空终止符

      Base64.decode(decodedString, inputString, inputStringLength - 1);  // 解码Base64字符串
      decodedString[decodedLength] = '\0';                               // 手动添加空终止符确保字符串正确终止

      // pcSerial.print("Decoded string is:\t");
      // pcSerial.println(decodedString);

      JsonDocument doc;                                                  // 创建一个足够大的JsonDocument
      DeserializationError error = deserializeJson(doc, decodedString);  // 将解码后的字符串反序列化到JSON文档

      if (error) {  // 检查反序列化是否成功
        // pcSerial.print("deserializeJson() failed: ");
        // pcSerial.println(error.c_str());
        return;
      }
      // 获取JSON的info 信息
      JsonDocument send_doc;
      send_doc["model_name"] = doc["name"];
      serializeJson(send_doc, espSerial);
#ifdef espSerial
      serializeJson(send_doc, espSerial);  // Serialize and print the JSON document
#endif
    }
  }
}
void setup1() {
  beep_init();
  delay(500);
  // beep_on();
  // Serial.println("Beep init");
}

void loop1() {
  myPacketSerial.update();
  if (myPacketSerial.overflow()) {
  }
}

// void setup1()
// {
//   #ifdef pcSerial
//     pcSerial.println("Setting Wire1 for AI_I2C");
//   #endif
//     Wire1.setSCL(_SCL);
//     Wire1.setSDA(_SDA);
//     AI_I2C.begin();
//     Serial.printf("AI_I2C _serialType: %d\n", AI_I2C._serialType);
// }

// void loop1() {
//     if (!AI_I2C.invoke(1, false, true))
//     {
//       JsonDocument doc_i2c; // Adjust size as needed

//         // Performance metrics
//         int index = 0;
//         doc_i2c["preprocess"] = AI_I2C.perf().prepocess;
//         doc_i2c["inference"]= AI_I2C.perf().inference;
//         doc_i2c["postprocess"]= AI_I2C.perf().postprocess;

//         // Last image
//         if (AI_I2C.last_image().length())
//         {
//             doc_i2c["img"] = AI_I2C.last_image();
//         }
//         // Boxes
//         index = 0;
//         for(auto &box : AI_I2C.boxes())
//         {
//             // doc_i2c["boxes"]["target"] = box.target;
//             // doc_i2c["boxes"]["score"] = box.score;
//             // doc_i2c["boxes"]["x"] = box.x;
//             // doc_i2c["boxes"]["y"] = box.y;
//             // doc_i2c["boxes"]["w"] = box.w;
//             // doc_i2c["boxes"]["h"] = box.h;
//             int currentBox[6] = {box.target, box.score, box.x, box.y, box.w, box.h};
//             copyArray(currentBox, doc_i2c["boxes"][index++]);
//         }

//         // Classes
//         index = 0;
//         for(auto &classObj : AI_I2C.classes())
//         {
//             // doc_i2c["classes"]["target"] = classObj.target;
//             // doc_i2c["classes"]["score"] = classObj.score;
//             int currentClass[2] = {classObj.target, classObj.score};
//             copyArray(currentClass, doc_i2c["classes"][index++]);
//         }
//         // Points
//         index = 0;
//         for(auto &point : AI_I2C.points())
//         {
//             // doc_i2c["points"]["target"] = point.target;
//             // doc_i2c["points"]["score"] = point.score;
//             // doc_i2c["points"]["x"] = point.x;
//             // doc_i2c["points"]["y"] = point.y;
//             int currtentPoint[4] = {point.target, point.score, point.x, point.y};
//             copyArray(currtentPoint, doc_i2c["points"][index++]);
//         }

//         // Keypoints
//         index = 0;
//         for(auto &keypoint : AI_I2C.keypoints()){
//             // doc_i2c["keypoints"]["target"] = keypoint.box.target;
//             // doc_i2c["keypoints"]["score"] = keypoint.box.score;
//             // doc_i2c["keypoints"]["x"] = keypoint.box.x;
//             // doc_i2c["keypoints"]["y"] = keypoint.box.y;
//             // doc_i2c["keypoints"]["w"] = keypoint.box.w;
//             // doc_i2c["keypoints"]["h"] = AI_I2C.keypoints()[i].box.h;
//             int currentKeypoint[6] = {keypoint.box.target, keypoint.box.score, keypoint.box.x, keypoint.box.y, keypoint.box.w, keypoint.box.h};
//                 copyArray(currentKeypoint, doc_i2c["keypoints"][index]);
//             int j=0;
//             for(auto &point: keypoint.points) {
//                 // Serial.print(AI_I2C.keypoints()[i].points[j].x);
//                 // Serial.print(AI_I2C.keypoints()[i].points[j].y);
//                 int arrayKeypoints[2] = {point.x, point.y};
//                 copyArray(arrayKeypoints, doc_i2c["keypoints"][index]["points"][j++]);
//             }
//             index++;
//         }

//         serializeJsonPretty(doc_i2c, pcSerial); // Serialize and print the JSON doc_i2cument
//         #ifdef espSerial
//         serializeJson(doc_i2c, espSerial); // Serialize and print the JSON doc_i2cument
//         #endif
//     }
// }
