#include "indicator_enabler.h"

#include "ui.h"
#include "view_data.h"

#include "view_pages.h"
static void __view_event_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);

int indicator_view_init(void)
{
#ifndef _indicator_lorahub_UI_H
    #error "Please include ui.h in your project"
#else
    lv_port_sem_take();
    ui_init(); /* (must be 480*800, set LCD_EVB_SCREEN_ROTATION_90 in menuconfig)*/
    view_pages_init();
    #ifdef INDICATOR_WIFI_H
    wifi_list_event_init();
    #endif
    lv_port_sem_give();
#endif
#ifdef SENSOR_H
    view_sensor_init();
    #ifdef SENSOR_CHARTS_H
    view_sensor_chart_init();
    #endif

#endif
#ifdef indicator_lorahub_H
    view_lorawan_init();
#endif

#ifdef INDICATOR_TIME_H
    view_datetime_init();
#endif

    // sensor_chart_event_init();

    for (int i = 0; i < VIEW_EVENT_ALL; i++) {
        ESP_ERROR_CHECK(
            esp_event_handler_instance_register_with(view_event_handle,
                                                     VIEW_EVENT_BASE, i,
                                                     __view_event_handler,
                                                     NULL, NULL));
    }
}

static void __view_event_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
    // lv_port_sem_take();
    switch (id) {
#ifdef INDICATOR_WIFI_H

        case VIEW_EVENT_SCREEN_START: { // if now wifi connected before
            uint8_t screen = *(uint8_t *)event_data;
            // g_prev_scr     = lv_scr_act();
            if (screen == SCREEN_WIFI_CONFIG) {
                lv_disp_load_scr(ui_ScreenWIFI);
            }
            break;
        }
        case VIEW_EVENT_WIFI_ST: {
            view_event_wifi_st(event_data);
            break;
        }
        case VIEW_EVENT_WIFI_LIST: {
            view_event_wifi_list(event_data);
            break;
        }
        case VIEW_EVENT_WIFI_CONNECT_RET: {
            view_event_wifi_connet_ret(event_data);
            break;
        }
#endif
        default:
            break;
    }
    // lv_port_sem_give();
}
