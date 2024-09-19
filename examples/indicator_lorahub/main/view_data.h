#ifndef VIEW_DATA_H
#define VIEW_DATA_H

#include <stdbool.h>
#include <string.h> 
#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_event_base.h"
#include "bsp_board.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "RegionCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

ESP_EVENT_DECLARE_BASE(VIEW_EVENT_BASE);
extern esp_event_loop_handle_t view_event_handle;

enum start_screen{
    SCREEN_SENSECAP_LOG, //todo
    SCREEN_WIFI_CONFIG,
};


#define WIFI_SCAN_LIST_SIZE  15

struct view_data_wifi_st
{
    bool   is_connected;
    bool   is_connecting;
    bool   is_network;  //is connect network
    char   ssid[32];
    int8_t rssi;
};


struct view_data_wifi_config
{
    char    ssid[32];
    uint8_t password[64];
    bool    have_password;
};

struct view_data_wifi_item
{
    char   ssid[32];
    bool   auth_mode;
    int8_t rssi;
};

struct view_data_wifi_list
{
    bool  is_connect;
    struct view_data_wifi_item  connect;
    uint16_t cnt;
    struct view_data_wifi_item aps[WIFI_SCAN_LIST_SIZE];
};

struct view_data_wifi_connet_ret_msg 
{
    uint8_t ret; //0:successfull , 1: failure
    char    msg[64];
};

struct view_data_display
{
    int   brightness; //0~100
    bool  sleep_mode_en;       //Turn Off Screen
    int   sleep_mode_time_min;  
};

struct view_data_time_cfg
{
    bool    time_format_24;

    bool    auto_update; //time auto update
    time_t  time;       // use when auto_update is true
    bool    set_time; 

    bool    auto_update_zone;  // use when  auto_update  is true
    int8_t  zone;       // use when auto_update_zone is true
    
    bool    daylight;   // use when auto_update is true  
}__attribute__((packed));

struct sensor_data_average
{
    float   data;  //Average over the past hour
    time_t  timestamp;
    bool    valid;
};

struct sensor_data_minmax
{
    float   max;
    float   min;
    time_t  timestamp;
    bool    valid;
};

enum sensor_data_type{
    SENSOR_DATA_CO2,
    SENSOR_DATA_TVOC,
    SENSOR_DATA_TEMP,
    SENSOR_DATA_HUMIDITY,
};

struct view_data_sensor_data
{
    enum sensor_data_type sensor_type;
    float  value;
};

struct view_data_sensor_history_data
{
    enum sensor_data_type sensor_type;
    struct sensor_data_average data_day[24];
    struct sensor_data_minmax data_week[7];
    uint8_t resolution;
    
    float day_min;
    float day_max;
    
    float week_min;
    float week_max;
};

enum view_data_lorawan_join{
    LORAWAN_JOIN_ST_FAIL,
    LORAWAN_JOIN_ST_OK,
    LORAWAN_JOIN_ST_PROCEED,
};

struct view_data_lorawan_basic_cfg {
    DeviceClass_t  class; 
    LoRaMacRegion_t region;
    uint32_t  uplink_interval_min;
    bool IsOtaaActivation;
};

struct view_data_lorawan_network_info{
    uint8_t eui[8];
    uint8_t join_eui[8];
    uint8_t app_key[16];
    uint8_t dev_addr[4];
    uint8_t apps_key[16];
    uint8_t nwks_key[16];
};

#define LORAHUB_LOG_LEVEL_INFO   0
#define LORAHUB_LOG_LEVEL_WARN   1
#define LORAHUB_LOG_LEVEL_ERROR  2

struct view_data_lorahub_log {
    uint8_t level; // 0: info ,  1: warn , 2: error 
    uint8_t data[128];
};

struct lorahub{
    char     web_cfg_lns_address[64];
    char     web_cfg_lns_port_str[6];
    char     web_cfg_chan_freq_mhz_str[12];
    char     web_cfg_chan_datarate_str[4];
    char     web_cfg_chan_bandwidth_khz_str[4];
    char     web_cfg_sntp_address[64];
};

enum {
    VIEW_EVENT_SCREEN_START = 0,  // uint8_t, enum start_screen, which screen when start
    
    VIEW_EVENT_SENSOR_DATA, // struct view_data_sensor_data

    VIEW_EVENT_SENSOR_DATA_CHART,

    VIEW_EVENT_WIFI_ST,   //view_data_wifi_st_t
    
    VIEW_EVENT_WIFI_LIST,       //view_data_wifi_list_t
    VIEW_EVENT_WIFI_LIST_REQ,   // NULL
    VIEW_EVENT_WIFI_CONNECT,    // struct view_data_wifi_config
    VIEW_EVENT_WIFI_CONNECT_RET,   // struct view_data_wifi_connet_ret_msg
    VIEW_EVENT_WIFI_CFG_DELETE,

    VIEW_EVENT_CITY,      // char city[32], max display 24 char

    VIEW_EVENT_TIME,  //  bool time_format_24
    VIEW_EVENT_TIME_CFG_UPDATE,  //  struct view_data_time_cfg
    VIEW_EVENT_TIME_CFG_APPLY,   //  struct view_data_time_cfg

    VIEW_EVENT_DISPLAY_CFG,         // struct view_data_display
    VIEW_EVENT_BRIGHTNESS_UPDATE,   // uint8_t brightness
    VIEW_EVENT_DISPLAY_CFG_APPLY,   // struct view_data_display. will save

    VIEW_EVENT_LORAHUB_CONFIGURE,
    VIEW_EVENT_LORAHUB_REBOOT,
    VIEW_EVENT_LORAHUB_MONITOR,

    VIEW_EVENT_LORAHUB_DATA_UPDATE,

    VIEW_EVENT_SHUTDOWN,      //NULL
    VIEW_EVENT_FACTORY_RESET, //NULL
    VIEW_EVENT_SCREEN_CTRL,   // bool  0:disable , 1:enable

    VIEW_EVENT_ALL,
};



#ifdef __cplusplus
}
#endif

#endif
