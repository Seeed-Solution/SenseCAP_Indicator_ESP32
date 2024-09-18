/**
 * @file indicator_time_view.c
 * @date  14 December 2023

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2023, Seeed Studio
 */

#include "indicator_time.h"
#include "ui.h"
#include "view_data.h"
#include "nvs.h"
#include "esp_timer.h"
#include "freertos/semphr.h"

static const char *TAG = "timedate_view";

static void __view_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id) {
        case VIEW_EVENT_TIME: { // time display
            bool time_format_24 = true;
            if (event_data) {
                time_format_24 = *(bool *)event_data;
            }

            time_t    now      = 0;
            struct tm timeinfo = {0};
            char     *p_wday_str;

            time(&now);
            localtime_r(&now, &timeinfo);
            char buf_h[3];
            char buf_m[3];
            char buf[6];
            int  _format_hour = timeinfo.tm_hour;
            if (!time_format_24) {
                if (_format_hour >= 13 && _format_hour <= 23) {
                    _format_hour = _format_hour - 12;
                }
            }
            lv_snprintf(buf_h, sizeof(buf_h), "%02d", _format_hour);
            lv_label_set_text(ui_LblHour1, buf_h);
            lv_snprintf(buf_m, sizeof(buf_m), "%02d", timeinfo.tm_min);
            lv_label_set_text(ui_LblMin1, buf_m);

            lv_snprintf(buf, sizeof(buf), "%02d:%02d", _format_hour, timeinfo.tm_min);
            lv_label_set_text(ui_LblTime1, buf);
            lv_label_set_text(ui_LblTime2, buf);
            lv_label_set_text(ui_LblTime3, buf);

            switch (timeinfo.tm_wday) {
                case 0: p_wday_str = "Sunday"; break;
                case 1: p_wday_str = "Monday"; break;
                case 2: p_wday_str = "Tuesday"; break;
                case 3: p_wday_str = "Wednesday"; break;
                case 4: p_wday_str = "Thursday"; break;
                case 5: p_wday_str = "Friday"; break;
                case 6: p_wday_str = "Sunday"; break;
                default: p_wday_str = ""; break;
            }
            char buf1[32];
            lv_snprintf(buf1, sizeof(buf1), "%s, %02d / %02d / %04d", p_wday_str, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_year + 1900);
            lv_label_set_text(ui_LblDate1, buf1);
        } break;
        // case VIEW_EVENT_TIME_CFG_APPLY:
            // struct view_data_time_cfg *p_cfg = (struct view_data_time_cfg *)event_data;
            // ESP_LOGI(TAG, "event: VIEW_EVENT_TIME_CFG_APPLY");
            // __time_cfg_print(p_cfg);
            // __time_cfg_set(p_cfg);
            // __time_cfg_save(p_cfg);
            // __time_cfg(p_cfg, p_cfg->set_time); // config;

            // bool time_format_24 = p_cfg->time_format_24;
            // esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_TIME, &time_format_24, sizeof(time_format_24), portMAX_DELAY);
            // break;
        default:
            break;
    }
}
//  struct view_data_time_cfg
//  struct view_data_time_cfg

    void view_datetime_init(void)
{
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_TIME,
                                                             __view_event_handler,
                                                             NULL, NULL));
    // ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
    //                                                          VIEW_EVENT_BASE, VIEW_EVENT_TIME_CFG_UPDATE,
    //                                                          __view_event_handler,
    //                                                          NULL, NULL));
    // ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
    //                                                          VIEW_EVENT_BASE, VIEW_EVENT_TIME_CFG_APPLY,
    //                                                          __view_event_handler,
    //                                                          NULL, NULL));
}