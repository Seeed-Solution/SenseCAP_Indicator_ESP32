#include <Wire.h>
#include <ArduinoJson.h>
#include <Seeed_Arduino_SSCMA.h>

// #define ARDUINO_ARCH_RP2040 1
#define pcSerial Serial
#ifdef  ARDUINO_ARCH_RP2040 /////////////////////////
  #include <SoftwareSerial.h>
  #define ESP_rxPin 17
  #define ESP_txPin 16
  #define espSerial Serial1
  #define ESP32_COMM_BAUD_RATE              (115200)
  // Right Grove -> Check https://files.seeedstudio.com/wiki/SenseCAP/SenseCAP_Indicator/grove.png
  #define rxPin 26
  #define txPin 27
  SoftwareSerial atSerial = SoftwareSerial(rxPin, txPin);
  #define _SDA 20
  #define _SCL 21
#elif ESP32 // XIAO /////////////////////////
  #include <HardwareSerial.h>
  HardwareSerial atSerial(0);
#else /////////////////////////
  #define atSerial Serial1
#endif
const char compile_date[] = __DATE__ " " __TIME__;
SSCMA AI;
SSCMA AI_I2C;

void setup()
{
  #ifdef pcSerial
    pcSerial.begin(9600);
    // while(!pcSerial) ;
    pcSerial.println(compile_date);
  #endif

  #ifdef ARDUINO_ARCH_RP2040 
    #ifdef pcSerial
      pcSerial.println("Setting ESP32 UART For inner RP2040");
    #endif
    espSerial.setRX(ESP_rxPin);
    espSerial.setTX(ESP_txPin);
    espSerial.begin(ESP32_COMM_BAUD_RATE);
    
    #ifdef pcSerial
      pcSerial.println("Using UART OF AI");
    #endif
      AI.begin(&atSerial);
  #endif

  Serial.printf("AI _serialType: %d\n", AI._serialType);
}

void setup1()
{
  #ifdef pcSerial
    pcSerial.println("Setting Wire for AI_I2C");
  #endif
    Wire.setSCL(_SCL);
    Wire.setSDA(_SDA);
    AI_I2C.begin();
    Serial.printf("AI_I2C _serialType: %d\n", AI_I2C._serialType);
}

void loop() {
    if (!AI.invoke(1, false, true))
    {
      JsonDocument doc; // Adjust size as needed

        // Performance metrics
        doc["preprocess"] = AI.perf().prepocess;
        doc["inference"]= AI.perf().inference;
        doc["postprocess"]= AI.perf().postprocess;

        // Last image
        if (AI.last_image().length())
        {
            doc["img"] = AI.last_image();
        }
        // // Boxes
        // JsonArray boxes = doc.createNestedArray("boxes");
        for (int i = 0; i < AI.boxes().size(); i++)
        {
            // JsonObject box = boxes.createNestedObject();
            doc["boxes"]["target"] = AI.boxes()[i].target;
            //     box["score"] =
            doc["boxes"]["score"] = AI.boxes()[i].score;
            //     box["x"] =
            doc["boxes"]["x"] = AI.boxes()[i].x;
            //     box["y"] =
            doc["boxes"]["y"] = AI.boxes()[i].y;
            //     box["w"] =
            doc["boxes"]["w"] = AI.boxes()[i].w;
            //     box["h"] =
            doc["boxes"]["h"] = AI.boxes()[i].h;
        }

        // Classes
        // for (int i = 0; i < AI.classes().size(); i++) {
        //     classObj["target"] = AI.classes()[i].target;
        //     classObj["score"] = AI.classes()[i].score;
        // }

        // // Points
        // for (int i = 0; i < AI.points().size(); i++) {
        //     point["target"] = AI.points()[i].target;
        //     point["score"] = AI.points()[i].score;
        //     point["x"] = AI.points()[i].x;
        //     point["y"] = AI.points()[i].y;
        // }

        // // Keypoints
        // for (int i = 0; i < AI.keypoints().size(); i++) {
        //     JsonObject keypoint = keypoints.createNestedObject();
        //     keypoint["target"] = AI.keypoints()[i].box.target;
        //     keypoint["score"] = AI.keypoints()[i].box.score;
        //     JsonObject box = keypoint.createNestedObject("box");
        //     box["x"] = AI.keypoints()[i].box.x;
        //     box["y"] = AI.keypoints()[i].box.y;
        //     box["w"] = AI.keypoints()[i].box.w;
        //     box["h"] = AI.keypoints()[i].box.h;
        //     JsonArray pointsArray = keypoint.createNestedArray("points");
        //     for (int j = 0; j < AI.keypoints()[i].points.size(); j++) {
        //         pointsArray.add(AI.keypoints()[i].points[j].x);
        //         pointsArray.add(AI.keypoints()[i].points[j].y);
        //     }

        //     Serial.print("keypoint[");
        //     Serial.print(i);
        //     Serial.print("] target=");
        //     Serial.print(AI.keypoints()[i].box.target);
        //     Serial.print(", score=");
        //     Serial.print(AI.keypoints()[i].box.score);
        //     Serial.print(", box:[x=");
        //     Serial.print(AI.keypoints()[i].box.x);
        //     Serial.print(", y=");
        //     Serial.print(AI.keypoints()[i].box.y);
        //     Serial.print(", w=");
        //     Serial.print(AI.keypoints()[i].box.w);
        //     Serial.print(", h=");
        //     Serial.print(AI.keypoints()[i].box.h);
        //     Serial.print("], points:[");
        //     for (int j = 0; j < AI.keypoints()[i].points.size(); j++) {
        //         Serial.print("[");
        //         Serial.print(AI.keypoints()[i].points[j].x);
        //         Serial.print(",");
        //         Serial.print(AI.keypoints()[i].points[j].y);
        //         Serial.print("],");
        //     }
        //     Serial.println("]");
        // }

        serializeJsonPretty(doc, pcSerial); // Serialize and print the JSON document
        #ifdef espSerial
        serializeJson(doc, espSerial); // Serialize and print the JSON document
        #endif
    }
}

void loop1() { 
    if (!AI_I2C.invoke(1, false, true))
    {
      JsonDocument doc_i2c; // Adjust size as needed

        // Performance metrics
        int index = 0;
        doc_i2c["preprocess"] = AI_I2C.perf().prepocess;
        doc_i2c["inference"]= AI_I2C.perf().inference;
        doc_i2c["postprocess"]= AI_I2C.perf().postprocess;

        // Last image
        if (AI_I2C.last_image().length())
        {
            doc_i2c["img"] = AI_I2C.last_image();
        }
        // Boxes
        index = 0;
        for(auto &box : AI_I2C.boxes())
        {
            // doc_i2c["boxes"]["target"] = box.target;
            // doc_i2c["boxes"]["score"] = box.score;
            // doc_i2c["boxes"]["x"] = box.x;
            // doc_i2c["boxes"]["y"] = box.y;
            // doc_i2c["boxes"]["w"] = box.w;
            // doc_i2c["boxes"]["h"] = box.h; 
            int currentBox[6] = {box.target, box.score, box.x, box.y, box.w, box.h}; 
            copyArray(currentBox, doc_i2c["boxes"][index++]);
        }

        // Classes
        index = 0;
        for(auto &classObj : AI_I2C.classes())
        {
            // doc_i2c["classes"]["target"] = classObj.target;
            // doc_i2c["classes"]["score"] = classObj.score;
            int currentClass[2] = {classObj.target, classObj.score};
            copyArray(currentClass, doc_i2c["classes"][index++]);
        }
        // Points
        index = 0;
        for(auto &point : AI_I2C.points())
        {
            // doc_i2c["points"]["target"] = point.target;
            // doc_i2c["points"]["score"] = point.score;
            // doc_i2c["points"]["x"] = point.x;
            // doc_i2c["points"]["y"] = point.y;
            int currtentPoint[4] = {point.target, point.score, point.x, point.y};
            copyArray(currtentPoint, doc_i2c["points"][index++]);
        }

        // Keypoints
        index = 0;
        for(auto &keypoint : AI_I2C.keypoints()){
            // doc_i2c["keypoints"]["target"] = keypoint.box.target;
            // doc_i2c["keypoints"]["score"] = keypoint.box.score;
            // doc_i2c["keypoints"]["x"] = keypoint.box.x;
            // doc_i2c["keypoints"]["y"] = keypoint.box.y;
            // doc_i2c["keypoints"]["w"] = keypoint.box.w;
            // doc_i2c["keypoints"]["h"] = AI_I2C.keypoints()[i].box.h;
            int currentKeypoint[6] = {keypoint.box.target, keypoint.box.score, keypoint.box.x, keypoint.box.y, keypoint.box.w, keypoint.box.h}; 
                copyArray(currentKeypoint, doc_i2c["keypoints"][index]);
            int j=0;
            for(auto &point: keypoint.points) {
                // Serial.print(AI_I2C.keypoints()[i].points[j].x);
                // Serial.print(AI_I2C.keypoints()[i].points[j].y);
                int arrayKeypoints[2] = {point.x, point.y};
                copyArray(arrayKeypoints, doc_i2c["keypoints"][index]["points"][j++]);
            }
            index++;
        }

        serializeJsonPretty(doc_i2c, pcSerial); // Serialize and print the JSON doc_i2cument
        #ifdef espSerial
        serializeJson(doc_i2c, espSerial); // Serialize and print the JSON doc_i2cument
        #endif
    }
}
