#include "pico/sync.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Base64.h>
#include <PacketSerial.h>
#include <Seeed_Arduino_SSCMA.h>
#include <Wire.h>

#define pcSerial_ENABLE 0
#if pcSerial_ENABLE
#define _LOG 1  // Disable for high frequency
#endif

#define USING_480 0
#define USING_UART 1

#if USING_UART
SSCMA AI;
#define ai_instance AI
#else
SSCMA AI_I2C;
#define ai_instance AI_I2C  // For I2C Port
#endif

#define ARDUINO_ARCH_RP2040 1

#if pcSerial_ENABLE
#define pcSerial Serial
#endif
#ifdef ARDUINO_ARCH_RP2040  /////////////////////////
#include <SoftwareSerial.h>
#define ESP_rxPin 17
#define ESP_txPin 16
#define espSerial Serial1
#define ESP32_COMM_BAUD_RATE (921600)
// #define ESP32_COMM_BAUD_RATE              (115200)
// Right Grove -> Check https://files.seeedstudio.com/wiki/SenseCAP/SenseCAP_Indicator/grove.png
#define txPin 20  // Write
#define rxPin 21  // Yellow
// SoftwareSerial atSerial = SoftwareSerial(rxPin, txPin);
#define atSerial Serial2
#define _SDA 26  // Write
#define _SCL 27  // Yellow
#elif ESP32      // XIAO /////////////////////////
#include <HardwareSerial.h>
HardwareSerial atSerial(0);
#else  /////////////////////////
#define atSerial Serial2
#endif

static inline void AI_func(SSCMA& instance);
static inline void send_model_title(SSCMA& instance);

const char compile_date[] = __DATE__ " " __TIME__;

PacketSerial myPacketSerial;
#define PKT_TYPE_CMD_BEEP_ON 0xA1
#define PKT_TYPE_CMD_SHUTDOWN 0xA3
#define PKT_TYPE_CMD_POWER_ON 0xA4
#define PKT_TYPE_CMD_MODEL_TITLE 0xA5
/************************ recv cmd from esp32  ****************************/
static bool shutdown_flag = false;

// static bool get_title = false;

void onPacketReceived(const uint8_t* buffer, size_t size) {
  if (size < 1)
    return;

  switch (buffer[0]) {
    case PKT_TYPE_CMD_SHUTDOWN:
      {
        Serial.println("cmd shutdown");
        shutdown_flag = true;
        break;
      }
    case PKT_TYPE_CMD_BEEP_ON:
      {
        beep_on();
        break;
      }
    case PKT_TYPE_CMD_MODEL_TITLE:
      {
        send_model_title(ai_instance);
        // get_title = true;
        break;
      }
    default:
      break;
  }
}
/************************ beep ****************************/

#define Buzzer 19  // Buzzer GPIO

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

/************************ setup ****************************/
mutex_t myMutex;
void setup() {
  mutex_init(&myMutex);
  beep_init();
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
  Wire1.setSCL(_SCL);
  Wire1.setSDA(_SDA);
  AI_I2C.begin(&Wire1);  // For I2C Port
#ifdef pcSerial
  pcSerial.println("Setting Wire1 for AI_I2C");
#endif
#if USING_480
  AI_I2C.fetch480();
#endif
#endif
}

void setup1() {
#ifdef pcSerial
  pcSerial.begin(ESP32_COMM_BAUD_RATE);
  // pcSerial.begin(115200);
  // while (!pcSerial)
  //     ;
  // pcSerial.println(compile_date);
  pcSerial.println("Setting ESP32 UART For inner RP2040");
#endif
  // Activate the communication inner device
  espSerial.setRX(ESP_rxPin);
  espSerial.setTX(ESP_txPin);
  espSerial.begin(ESP32_COMM_BAUD_RATE);

  myPacketSerial.setStream(&espSerial);
  myPacketSerial.setPacketHandler(&onPacketReceived);

  // Serial.println("Beep init");
  // delay(500);
  // beep_on();
}

void loop() {  // For UART Port
  AI_func(ai_instance);
}

void loop1() {
  myPacketSerial.update();
  if (myPacketSerial.overflow()) {
  }
}
JsonDocument doc_info;  // Adjust size as needed
JsonDocument doc_image;  // Adjust size as needed
/**
 * @brief
 *
 * @param instance
 * @attention
 * {"keypoints":[{"box":[0,95,120,120,239,240],"points":[[127,77],[153,62],[98,59],[182,91],[61,88],[222,209],[10,198],[241,249],[0,234],[193,220],[25,183],[179,293],[61,285],[135,256],[72,220],[116,271],[197,216]]}]}
 */
static inline void AI_func(SSCMA& instance) {
  if (!mutex_enter_timeout_ms(&myMutex, 1000))
    return;

  if (!instance.invoke(1, false, true)) {

    /* Performance metrics */
    // JsonDocument doc_fet;   // Adjust size as needed
    // auto& perf = instance.perf();
    // doc_fet["pre"] = perf.prepocess;
    // doc_fet["inf"] = perf.inference;
    // doc_fet["post"] = perf.postprocess;

    // serializeJson(doc_fet, espSerial);
    // espSerial.println();
    // serializeJson(doc_fet, pcSerial);  // Serialize and print the JSON document
    // pcSerial.println();
    doc_info.clear();
    /* Boxes */
    auto& boxes = instance.boxes();
    int index = 0;
    for (auto& box : boxes) {
      int currentBox[6] = { box.x, box.y, box.w, box.h, box.score, box.target };
      copyArray(currentBox, doc_info["boxes"][index++]);  // Assuming copyArray directly modifies the doc
    }

    auto& classes = instance.classes();
    index = 0;
    for (auto& classObj : classes) {
      int currentClass[2] = { classObj.score, classObj.target };
      copyArray(currentClass, doc_info["classes"][index++]);
    }

    /* Points */
    auto& points = instance.points();
    index = 0;
    for (auto& point : points) {
      int currentPoint[4] = { point.x, point.y, point.score, point.target };
      copyArray(currentPoint, doc_info["points"][index++]);
    }

    /* Keypoints */
    auto& keypoints = instance.keypoints();
    index = 0;
    for (auto& keypoint : keypoints) {
      int currentKeypoint[6] = { keypoint.box.x, keypoint.box.y, keypoint.box.w,
                                 keypoint.box.h, keypoint.box.score, keypoint.box.target };
      copyArray(currentKeypoint, doc_info["keypoints"][index]["box"]);
      int j = 0;
      for (auto& point : keypoint.points) {
        // int arrayKeypoints[] = { point.x, point.y, point.score, point.target};
        int arrayKeypoints[] = { point.x, point.y, point.score};
        // int arrayKeypoints[] = { point.x, point.y};
        copyArray(arrayKeypoints, doc_info["keypoints"][index]["points"][j++]);
      }
      index++;
    }
    /* Last image*/
    auto lastImage = instance.last_image();
    if (lastImage.length()) {
      doc_image.clear();
      doc_image["img"] = lastImage;
      serializeJson(doc_image, espSerial);
      espSerial.println();
#if _LOG
      // serializeJson(doc_image, pcSerial);  // Serialize and print the JSON document
      // pcSerial.println();
#endif
    }
    // delay(1);
    if (!doc_info.isNull()) {
      serializeJson(doc_info, espSerial);
      espSerial.println();
#if _LOG
      serializeJson(doc_info, pcSerial);  // Serialize and print the JSON document
      pcSerial.println();
#endif
    }
  }
  mutex_exit(&myMutex);
}

static inline void send_model_title(SSCMA& instance) {
  mutex_enter_blocking(&myMutex);

  bool fetchSuccess = instance.fetch_info();
  String base64String;
  if (!fetchSuccess) {
    base64String = instance.info();
  }
  mutex_exit(&myMutex);

  if (!fetchSuccess) {
    int inputStringLength = base64String.length() + 1;
    char inputString[inputStringLength];
    base64String.toCharArray(inputString, inputStringLength);

    int decodedLength = Base64.decodedLength(inputString, inputStringLength - 1);
    char decodedString[decodedLength + 1];
    Base64.decode(decodedString, inputString, inputStringLength - 1);
    decodedString[decodedLength] = '\0';

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, decodedString);
    if (error) {
      return;
    }

    JsonDocument send_doc;
    send_doc["name"] = doc["name"];
#ifdef espSerial
    serializeJson(send_doc, espSerial);
    espSerial.println();
#endif
#if _LOG
    serializeJson(send_doc, pcSerial);
    pcSerial.println();
#endif
  }
}