#include <Seeed_Arduino_SSCMA.h>
#include <ArduinoJson.h> 
#ifdef ESP32
#include <HardwareSerial.h>

// Define two Serial devices mapped to the two internal UARTs
HardwareSerial atSerial(0);

#else
#define atSerial Serial1
#endif

SSCMA AI;
void setup()
{
    Serial.begin(9600);
    AI.begin(&atSerial);
    // Serial.println("Start!");
}

void loop() {
    JsonDocument doc; // Adjust size as needed
    if (!AI.invoke(1,false,true)) {
        // Performance metrics
        // doc["preprocess"] = AI.perf().prepocess; 
        // doc["inference"]= AI.perf().inference; 
        // doc["postprocess"]= AI.perf().postprocess;
        
        // // Boxes
        // JsonArray boxes = doc.createNestedArray("boxes");
        for (int i = 0; i < AI.boxes().size(); i++) {
          // JsonObject box = boxes.createNestedObject();
          doc["boxes"]["target"] =  AI.boxes()[i].target; 
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

        // // Classes
        // JsonArray classes = doc.createNestedArray("classes");
        // for (int i = 0; i < AI.classes().size(); i++) {
        //     classObj["target"] = AI.classes()[i].target;
        //     classObj["score"] = AI.classes()[i].score;
        // }

        // Points
        // JsonArray points = doc.createNestedArray("points");
        // for (int i = 0; i < AI.points().size(); i++) {
        //     point["target"] = AI.points()[i].target;
        //     point["score"] = AI.points()[i].score;
        //     point["x"] = AI.points()[i].x;
        //     point["y"] = AI.points()[i].y;
        // }

        // // Keypoints
        // JsonArray keypoints = doc.createNestedArray("keypoints");
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

        // Last image
        if (!AI.last_image().isEmpty()) {
            doc["img"] = AI.last_image().c_str();
        }

        serializeJsonPretty(doc, Serial); // Serialize and print the JSON document
        Serial.println(); // Ensure there's a newline at the end
    }
    delay(1000);
}