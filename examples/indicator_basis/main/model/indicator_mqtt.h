#ifndef INDICATOR_MQTT_H
#define INDICATOR_MQTT_H

#include "config.h"
#include "view_data.h"

#ifdef __cplusplus
extern "C" {
#endif

// ubidots config 
#define CONFIG_BROKER_URL              "mqtt://industrial.api.ubidots.com"
#define CONFIG_TOPIC_INDICATORE        "/v2.0/devices/indicator"
#define CONFIG_MQTT_USERNAME           "<your ubidots tokens>"  // your tokens


// sensor
#define CONFIG_SENSOR_CO2_VALUE_KEY       "co2"
#define CONFIG_SENSOR_TVOC_VALUE_KEY      "tvoc"
#define CONFIG_SENSOR_TEMP_VALUE_KEY      "temp"
#define CONFIG_SENSOR_HUMIDITY_VALUE_KEY  "humidity"
#define CONFIG_SENSOR_TOPIC_DATA          CONFIG_TOPIC_INDICATORE


int indicator_mqtt_init(void);

#ifdef __cplusplus
}
#endif

#endif
