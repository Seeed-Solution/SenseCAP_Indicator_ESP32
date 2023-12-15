#include "indicator_enabler.h"

int indicator_model_init(void)
{
#ifdef INDICATOR_STORAGE_SPIFFS_H
    indicator_spiffs_init();
#endif
#ifdef INDICATOR_STORAGE_NVS_H
    indicator_nvs_init();
#else
    #error "Please inplement storage model"
#endif

#ifdef INDICATOR_BTN_H
    indicator_btn_init();
#endif

#ifdef INDICATOR_DISPLAY_H
    indicator_display_init(); // lcd bl on
#endif

#ifdef INDICATOR_WIFI_H
    indicator_wifi_init();
    #ifdef INDICATOR_TIME_H
    time_model_init();
    #endif
    #ifdef INDICATOR_CITY_H
    indicator_city_init();
    #endif
#endif

#ifdef ESP32_RP2040_H
    esp32_rp2040_init();
    #ifdef SENSOR_H
    indicator_sensor_init();
        #ifdef SENSOR_CHARTS_H &&TIME_MODEL_H
    sensor_charts_init();
        #endif
    #endif
#endif


#ifdef INDICATOR_LORAWAN_H
    indicator_lorawan_init();
    #ifdef indicator_CMD_H
    indicator_cmd_init();
    #endif
#endif

#ifdef INDICATOR_TERMINAL_H
    indicator_terminal_init();
#endif
}