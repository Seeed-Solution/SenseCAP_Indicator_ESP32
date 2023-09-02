#ifndef VIEW_DATA_H
#define VIEW_DATA_H

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

enum start_screen{
    SCREEN_SENSECAP_LOG, //todo
    SCREEN_MATTER_CONFIG,
    SCREEN_DASHBOARD,
};

struct view_data_wifi_st
{
    bool   is_connected;
    int8_t rssi;
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


enum dashboard_data_type{
    DASHBOARD_DATA_ARC,
    DASHBOARD_DATA_SWITCH,
    DASHBOARD_DATA_SLIDER,
    DASHBOARD_DATA_BUTTON1,
    DASHBOARD_DATA_BUTTON2,
};

struct view_data_matter_dashboard_data
{
    enum dashboard_data_type dashboard_data_type;
    int  value;
};


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

enum {
    VIEW_EVENT_SCREEN_START = 0,  // uint8_t, enum start_screen, which screen when start

    VIEW_EVENT_TIME,  //  bool time_format_24
    
    VIEW_EVENT_WIFI_ST,   //view_data_wifi_st_t
    VIEW_EVENT_CITY,      // char city[32], max display 24 char

    VIEW_EVENT_MATTER_SET_DASHBOARD_DATA, // struct view_data_matter_dashboard_data
    VIEW_EVENT_MATTER_DASHBOARD_DATA, // struct view_data_matter_dashboard_data
    VIEW_EVENT_SENSOR_DATA, // struct view_data_sensor_data

    VIEW_EVENT_SENSOR_TEMP,  
    VIEW_EVENT_SENSOR_HUMIDITY,
    VIEW_EVENT_SENSOR_TVOC,
    VIEW_EVENT_SENSOR_CO2,

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
