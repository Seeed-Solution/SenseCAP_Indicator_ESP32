#include "indicator_time.h"
#include "esp_log.h"
#include "esp_sntp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "time.h"
#include "esp_timer.h"
#include "view_data.h"

static const char *TAG = "time_model";

#define UPDATE_TIME_MS 1000 * 5000 // 3s

void __datetime_triger_callback(void *arg)
{
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_TIME, NULL, 0, portMAX_DELAY);
}

void _create_datetime_update_timer()
{
    const esp_timer_create_args_t timer_args = {
        .callback = &__datetime_triger_callback,
        .name     = "datatime_trigger_timer"};

    esp_timer_handle_t datatime_trigger_timer_handle;
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &datatime_trigger_timer_handle));
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_TIME, NULL, 0, portMAX_DELAY);
    ESP_ERROR_CHECK(esp_timer_start_periodic(datatime_trigger_timer_handle, UPDATE_TIME_MS)); // 定义SAMPLING_INTERVAL为抽样间隔，单位为微秒
}

__attribute__((weak)) void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Time synchronized");
    esp_sntp_set_sync_interval(3600000); // default CONFIG_LWIP_SNTP_UPDATE_DELAY
    _create_datetime_update_timer();
}


static void __view_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_id == VIEW_EVENT_TIME_CFG_UPDATE) {
        
        struct view_data_time_cfg *cfg = (struct view_data_time_cfg *)event_data; 
        if(!cfg->auto_update){
           // 禁用时间同步

        }else{
            // 如果时间未同步，则根据 zone 同步
        }
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_TIME_CFG_APPLY, &cfg, sizeof(cfg), portMAX_DELAY);
    }
}

/* Public functions */
bool isTimeValid(void)
{
    // June 1, 2023 00:00:00 in seconds since Unix epoch
    const time_t targetTime = 1685584800;
    return time(NULL) > targetTime;
}

uint64_t getTimestampSec(void) // second
{
    return (uint64_t)time(NULL);
}

uint64_t getTimestampMs(void) // millisecond
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000LL + (uint64_t)tv.tv_usec / 1000LL;
}

void set_time_zone(const char *timeZone)
{
    setenv("TZ", timeZone, 1); // 设置环境变量 TZ 为指定的时区
    tzset();                   // 更新时区设置
}

void printTimeMs(void)
{
    time_t    now_sec = getTimestampSec();
    struct tm timeinfo;
    localtime_r(&now_sec, &timeinfo);

    char strftime_buf[64];
    strftime(strftime_buf, sizeof(strftime_buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    ESP_LOGI(TAG, "%s", strftime_buf); // print time
}

void time_model_init(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_setservername(1, "cn.ntp.org.cn");
    esp_sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    esp_sntp_set_sync_interval(1000 * 15); // must be > 15000, 15000 means 15 seconds, default CONFIG_LWIP_SNTP_UPDATE_DELAY
    esp_sntp_init();
    set_time_zone("CST-8");
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_TIME, NULL, 0, portMAX_DELAY); // Default 1900-01-01
}