/**
 * @file esp32_rp2040.h
 * @date  12 December 2023

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2023, Seeed Studio
 */

#ifndef ESP32_RP2040_H
#define ESP32_RP2040_H

#ifdef __cplusplus
extern "C" {
#endif

// enum  pkt_type {

//     PKT_TYPE_CMD_COLLECT_INTERVAL = 0xA0, //uin32_t 
//     PKT_TYPE_CMD_BEEP_ON  = 0xA1,  //uin32_t  ms: on time 
//     PKT_TYPE_CMD_BEEP_OFF = 0xA2,
//     PKT_TYPE_CMD_SHUTDOWN = 0xA3, //uin32_t 
//     PKT_TYPE_CMD_POWER_ON = 0xA4,

//     PKT_TYPE_SENSOR_SCD41_TEMP  = 0xB0, // float
//     PKT_TYPE_SENSOR_SCD41_HUMIDITY = 0xB1, // float
//     PKT_TYPE_SENSOR_SCD41_CO2 = 0xB2, // float

//     PKT_TYPE_SENSOR_SHT41_TEMP = 0xB3, // float
//     PKT_TYPE_SENSOR_SHT41_HUMIDITY = 0xB4, // float

//     PKT_TYPE_SENSOR_TVOC_INDEX = 0xB5, // float

//     //todo
// };

void esp32_rp2040_init(void);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*ESP32_RP2040_H*/