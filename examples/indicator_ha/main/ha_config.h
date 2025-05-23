

#ifndef HA_CONFIG_H
#define HA_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_BROKER_URL                        "mqtt://homeassistant.local" /* domain */
// #define CONFIG_BROKER_URL                        "mqtt://192.168.1.102" /* IP */
#define CONFIG_MQTT_CLIENT_ID                    "indicator-id"
#define CONFIG_MQTT_USERNAME                     "indicator-usr"
#define CONFIG_MQTT_PASSWORD                     "indicator-password"

#define CONFIG_HA_SENSOR_ENTITY_NUM              6
#define CONFIG_HA_SWITCH_ENTITY_NUM              8

// topic
#define CONFIG_TOPIC_SENSOR_DATA                 "indicator/sensor"
#define CONFIG_TOPIC_SENSOR_DATA_QOS             0

#define CONFIG_TOPIC_SWITCH_STATE                "indicator/switch/state"
#define CONFIG_TOPIC_SWITCH_SET                  "indicator/switch/set"
#define CONFIG_TOPIC_SWITCH_QOS                  1

// buildin sensor
#define CONFIG_SENSOR_BUILDIN_CO2_VALUE_KEY      "co2"
#define CONFIG_SENSOR_BUILDIN_TVOC_VALUE_KEY     "tvoc"
#define CONFIG_SENSOR_BUILDIN_TEMP_VALUE_KEY     "temp"
#define CONFIG_SENSOR_BUILDIN_HUMIDITY_VALUE_KEY "humidity"
#define CONFIG_SENSOR_BUILDIN_TOPIC_DATA         CONFIG_TOPIC_SENSOR_DATA

// sensor1
#define CONFIG_SENSOR1_VALUE_KEY                 "temp"
#define CONFIG_SENSOR1_UI_UNIT                   "°C"
#define CONFIG_SENSOR1_UI_NAME                   "Temp"

// sensor2
#define CONFIG_SENSOR2_VALUE_KEY                 "humidity"
#define CONFIG_SENSOR2_UI_UNIT                   "%"
#define CONFIG_SENSOR2_UI_NAME                   "Humidity"

// sensor3
#define CONFIG_SENSOR3_VALUE_KEY                 "tvoc"
#define CONFIG_SENSOR3_UI_UNIT                   ""
#define CONFIG_SENSOR3_UI_NAME                   "tVOC"

// sensor4
#define CONFIG_SENSOR4_VALUE_KEY                 "co2"
#define CONFIG_SENSOR4_UI_UNIT                   "ppm"
#define CONFIG_SENSOR4_UI_NAME                   "CO2"

// sensor5
#define CONFIG_SENSOR5_VALUE_KEY                 "temp"
#define CONFIG_SENSOR5_UI_UNIT                   "°C"
#define CONFIG_SENSOR5_UI_NAME                   "Temp"

// sensor6
#define CONFIG_SENSOR6_VALUE_KEY                 "humidity"
#define CONFIG_SENSOR6_UI_UNIT                   "%"
#define CONFIG_SENSOR6_UI_NAME                   "Humidity"

// switch1
#define CONFIG_SWITCH1_VALUE_KEY                 "switch1"
#define CONFIG_SWITCH1_UI_NAME                   "Switch1"

// switch2
#define CONFIG_SWITCH2_VALUE_KEY                 "switch2"
#define CONFIG_SWITCH2_UI_NAME                   "Switch2"

// switch3
#define CONFIG_SWITCH3_VALUE_KEY                 "switch3"
#define CONFIG_SWITCH3_UI_NAME                   "Switch3"

// switch4
#define CONFIG_SWITCH4_VALUE_KEY                 "switch4"
#define CONFIG_SWITCH4_UI_NAME                   "Switch4"

// switch5
#define CONFIG_SWITCH5_VALUE_KEY                 "switch5"
#define CONFIG_SWITCH5_UI_NAME                   "Switch5"

// switch6
#define CONFIG_SWITCH6_VALUE_KEY                 "switch6"
#define CONFIG_SWITCH6_UI_NAME                   "Switch6"

// switch7
#define CONFIG_SWITCH7_VALUE_KEY                 "switch7"
#define CONFIG_SWITCH7_UI_NAME                   "Switch7"

// switch8
#define CONFIG_SWITCH8_VALUE_KEY                 "switch8"
#define CONFIG_SWITCH8_UI_NAME                   "Switch8"

#ifdef __cplusplus
}
#endif

#endif
