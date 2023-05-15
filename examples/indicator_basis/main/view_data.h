#ifndef VIEW_DATA_H
#define VIEW_DATA_H

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

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
    float  vaule;
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

enum {
    VIEW_EVENT_SCREEN_START = 0,  // uint8_t, enum start_screen, which screen when start

    VIEW_EVENT_TIME,  //  bool time_format_24
    
    VIEW_EVENT_WIFI_ST,   //view_data_wifi_st_t
    VIEW_EVENT_CITY,      // char city[32], max display 24 char

    VIEW_EVENT_SENSOR_DATA, // struct view_data_sensor_data

    VIEW_EVENT_SENSOR_TEMP,  
    VIEW_EVENT_SENSOR_HUMIDITY,
    VIEW_EVENT_SENSOR_TVOC,
    VIEW_EVENT_SENSOR_CO2,

    VIEW_EVENT_SENSOR_TEMP_HISTORY,
    VIEW_EVENT_SENSOR_HUMIDITY_HISTORY,
    VIEW_EVENT_SENSOR_TVOC_HISTORY,
    VIEW_EVENT_SENSOR_CO2_HISTORY,

    VIEW_EVENT_SENSOR_DATA_HISTORY, //struct view_data_sensor_history_data


    VIEW_EVENT_WIFI_LIST,       //view_data_wifi_list_t
    VIEW_EVENT_WIFI_LIST_REQ,   // NULL
    VIEW_EVENT_WIFI_CONNECT,    // struct view_data_wifi_config

    VIEW_EVENT_WIFI_CONNECT_RET,   // struct view_data_wifi_connet_ret_msg


    VIEW_EVENT_WIFI_CFG_DELETE,


    VIEW_EVENT_TIME_CFG_UPDATE,  //  struct view_data_time_cfg
    VIEW_EVENT_TIME_CFG_APPLY,   //  struct view_data_time_cfg

    VIEW_EVENT_DISPLAY_CFG,         // struct view_data_display
    VIEW_EVENT_BRIGHTNESS_UPDATE,   // uint8_t brightness
    VIEW_EVENT_DISPLAY_CFG_APPLY,   // struct view_data_display. will save


    VIEW_EVENT_SHUTDOWN,      //NULL
    VIEW_EVENT_FACTORY_RESET, //NULL
    VIEW_EVENT_SCREEN_CTRL,   // bool  0:disable , 1:enable

    VIEW_EVENT_ALL,
};



#ifdef __cplusplus
}
#endif

#endif
