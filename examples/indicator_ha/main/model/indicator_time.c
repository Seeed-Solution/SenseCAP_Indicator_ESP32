#include "indicator_time.h"
#include "esp_sntp.h"
#include "freertos/semphr.h"
#include<stdlib.h>
#include "nvs.h"


#define TIME_CFG_STORAGE  "time-cfg"

struct indicator_time
{
    struct view_data_time_cfg cfg;
    char net_zone[64];
};

static const char *TAG = "time";

static struct indicator_time __g_time_model;
static SemaphoreHandle_t       __g_data_mutex;

static esp_timer_handle_t   view_update_timer_handle;

static void __time_cfg_set(struct view_data_time_cfg *p_cfg )
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    memcpy( &__g_time_model.cfg, p_cfg, sizeof(struct view_data_time_cfg));
    xSemaphoreGive(__g_data_mutex);
}

static void __time_cfg_get(struct view_data_time_cfg *p_cfg )
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    memcpy(p_cfg, &__g_time_model.cfg, sizeof(struct view_data_time_cfg));
    xSemaphoreGive(__g_data_mutex);
}


static void __time_cfg_save(struct view_data_time_cfg *p_cfg )
{
    esp_err_t ret = 0;
    ret = indicator_storage_write(TIME_CFG_STORAGE, (void *)p_cfg, sizeof(struct view_data_time_cfg));
    if( ret != ESP_OK ) {
        ESP_LOGI(TAG, "cfg write err:%d", ret);
    } else {
        ESP_LOGI(TAG, "cfg write successful");
    }
}

static void __time_cfg_print(struct view_data_time_cfg *p_cfg )
{
    printf( "time_format_24:%d, auto_update:%d, time:%d, auto_update_zone:%d, zone:%d, daylight:%d\n",  \
      (bool) p_cfg->time_format_24, (bool)p_cfg->auto_update, (long)p_cfg->time, (bool)p_cfg->auto_update_zone, (int8_t)p_cfg->zone, (bool)p_cfg->daylight);
}


static void __time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI("ntp", "Notification of a time synchronization event");
    struct view_data_time_cfg cfg;
    __time_cfg_get(&cfg);
    bool time_format_24 = cfg.time_format_24;
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_TIME, &time_format_24, sizeof(time_format_24), portMAX_DELAY);
}

static void __time_set(time_t time)
{
    struct tm tm = {4, 14, 3, 19, 0, 138, 0, 0, 0};
    struct timeval timestamp = { time, 0 };
    settimeofday(&timestamp, NULL);
}

static void __time_sync_enable(void)
{
    sntp_init();
}

static void __time_sync_stop(void)
{
    sntp_stop();
}

static void __time_zone_set(struct view_data_time_cfg *p_cfg)
{
    if ( !p_cfg->auto_update_zone) {
        
        int8_t zone = p_cfg->zone;
        char zone_str[32];

        if( p_cfg->daylight) {
            zone -=1; //todo
        }
        if( zone >= 0) {
            snprintf(zone_str, sizeof(zone_str) - 1, "UTC-%d", zone);
        } else {
            snprintf(zone_str, sizeof(zone_str) - 1, "UTC+%d", 0 - zone);
        }
        setenv("TZ", zone_str, 1);
    } else {

        char net_zone[64] = {0};
        xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
        memcpy(net_zone, &__g_time_model.net_zone, sizeof(net_zone));
        xSemaphoreGive(__g_data_mutex);

        if( strlen(net_zone) > 0 ) {
            setenv("TZ", net_zone, 1);
        }
    }
}

static void __time_cfg(struct view_data_time_cfg *p_cfg, bool set_time)
{
    if( p_cfg->auto_update ) {
        __time_sync_enable();
        __time_zone_set(p_cfg); 
    } else {
        __time_sync_stop();
        struct timeval timestamp = { p_cfg->time, 0 };
        if( set_time ) {
            settimeofday(&timestamp, NULL);
        }
    }
}

static void __time_view_update_callback(void* arg)
{
    static int last_min = 60;
    time_t now = 0;
    struct tm timeinfo = { 0 };
    time(&now);
    localtime_r(&now, &timeinfo);
    if( timeinfo.tm_min != last_min) {
        last_min = timeinfo.tm_min;
        
        struct view_data_time_cfg cfg;
        __time_cfg_get(&cfg);
        bool time_format_24 = cfg.time_format_24;
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_TIME, &time_format_24, sizeof(time_format_24), portMAX_DELAY);
        ESP_LOGI(TAG, "need update time view");
        char strftime_buf[64];
        strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
        ESP_LOGI(TAG, "%s", strftime_buf);
    }
}

static __time_view_update_init(void)
{
    const esp_timer_create_args_t timer_args = {
            .callback = &__time_view_update_callback,
            /* argument specified here will be passed to timer callback function */
            .arg = (void*) view_update_timer_handle,
            .name = "time update"
    };
    ESP_ERROR_CHECK( esp_timer_create(&timer_args, &view_update_timer_handle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(view_update_timer_handle, 1000000)); //1s
}


static void __view_event_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    switch (id)
    {
        case VIEW_EVENT_TIME_CFG_APPLY: {
            struct view_data_time_cfg * p_cfg = (struct view_data_time_cfg *)event_data;
            ESP_LOGI(TAG, "event: VIEW_EVENT_TIME_CFG_APPLY");
            __time_cfg_print(p_cfg);
            __time_cfg_set(p_cfg);
            __time_cfg_save(p_cfg);
            __time_cfg(p_cfg, p_cfg->set_time);  //config;

            bool time_format_24 = p_cfg->time_format_24;
            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_TIME, &time_format_24, sizeof(time_format_24), portMAX_DELAY);
            break;
        }
        case VIEW_EVENT_WIFI_ST: {
            static bool fist = true;
            ESP_LOGI(TAG, "event: VIEW_EVENT_WIFI_ST");
            struct view_data_wifi_st *p_st = ( struct view_data_wifi_st *)event_data;
            if( p_st->is_network) {
                
                if( !fist) {
                    break;
                }
                fist = false;
                struct view_data_time_cfg cfg;
                __time_cfg_get(&cfg);
                if( cfg.auto_update ) {
                    __time_sync_stop();
                    __time_sync_enable();
                }
            } 
        }
    default:
        break;
    }
}


static void __time_cfg_restore(void)
{

    esp_err_t ret = 0;
    struct view_data_time_cfg cfg;
    
    size_t len = sizeof(cfg);
    
    ret = indicator_storage_read(TIME_CFG_STORAGE, (void *)&cfg, &len);
    if( ret == ESP_OK  && len== (sizeof(cfg)) ) {
        ESP_LOGI(TAG, "cfg read successful");
        __time_cfg_set(&cfg);
    } else {
        // err or not find
        if( ret == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGI(TAG, "cfg not find");
        }else {
            ESP_LOGI(TAG, "cfg read err:%d", ret);
        }

        cfg.auto_update = true;
        cfg.auto_update_zone = true;
        cfg.daylight = true;
        cfg.time_format_24 = true;
        cfg.zone = 0;
        cfg.time = 0;
        __time_cfg_set(&cfg);
    }
}

int indicator_time_init(void)
{
    __g_data_mutex  =  xSemaphoreCreateMutex();

    memset(__g_time_model.net_zone, 0 , sizeof(__g_time_model.net_zone));

    __time_cfg_restore();

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_setservername(1, "cn.ntp.org.cn");
    sntp_set_time_sync_notification_cb(__time_sync_notification_cb);
    
    struct view_data_time_cfg cfg;
    __time_cfg_get(&cfg);
    __time_cfg(&cfg, true);
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_TIME_CFG_UPDATE, &cfg, sizeof(cfg), portMAX_DELAY);

    __time_view_update_init();

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_TIME_CFG_APPLY, 
                                                            __view_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_WIFI_ST, 
                                                            __view_event_handler, NULL, NULL));
}

int indicator_time_net_zone_set( char *p)
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    strcpy(__g_time_model.net_zone, p);
    xSemaphoreGive(__g_data_mutex);

    struct view_data_time_cfg cfg;
    __time_cfg_get(&cfg);

    if( cfg.auto_update ) {
        __time_zone_set(&cfg); 
    }
    bool time_format_24 = cfg.time_format_24;
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_TIME, &time_format_24, sizeof(time_format_24), portMAX_DELAY);
}
