#include "indicator_display.h"
#include "freertos/semphr.h"

#include "driver/ledc.h"
#include "esp_timer.h"
#include "nvs.h"

#define DISPLAY_CFG_STORAGE  "display"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (45) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_FREQUENCY          (5000) // Frequency in Hertz. Set frequency at 5 kHz



struct indicator_display
{
    struct view_data_display cfg;
    bool timer_st;  // true, running , false:  stop
    bool display_st;
};

static const char *TAG = "display";

static struct indicator_display __g_display_model;
static SemaphoreHandle_t       __g_data_mutex;

static esp_timer_handle_t      sleep_timer_handle;
static SemaphoreHandle_t       __g_timer_mutex;

static bool init_done_flag = false;
static void __display_cfg_set(struct view_data_display *p_data )
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    memcpy( &__g_display_model.cfg, p_data, sizeof(struct view_data_display));
    xSemaphoreGive(__g_data_mutex);
}

static void __display_cfg_get(struct view_data_display *p_data )
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    memcpy(p_data, &__g_display_model.cfg, sizeof(struct view_data_display));
    xSemaphoreGive(__g_data_mutex);
}

static void __timer_st_set( bool st )
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    __g_display_model.timer_st = st;
    xSemaphoreGive(__g_data_mutex);
}

static bool __timer_st_get(void)
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    bool flag =  __g_display_model.timer_st;
    xSemaphoreGive(__g_data_mutex);
    return flag;
}

static void __display_st_set( bool st )
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    __g_display_model.display_st = st;
    xSemaphoreGive(__g_data_mutex);
}

static bool __display_st_get(void)
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    bool st =  __g_display_model.display_st;
    xSemaphoreGive(__g_data_mutex);
    return st;
}


static void __display_cfg_print(struct view_data_display *p_data )
{
    ESP_LOGI(TAG, "brightnes:%d, sleep_mode:%d, time:%d min",p_data->brightness, p_data->sleep_mode_en, p_data->sleep_mode_time_min );

}



static void __lcd_bl_init(uint8_t brightness)
{
    if(brightness > 99) {
        brightness=99;
    } else if( brightness < 1) {
        brightness=1;
    }

    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           =  (8192 - 1) * brightness/ 100, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

static void __lcd_bl_set(int brightness )
{   
    if(brightness > 99) {
        brightness=99;
    } else if( brightness < 1) {
        brightness=1;
    }

    uint32_t duty =(uint32_t) (8192 - 1) * brightness / 100.0;

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

static void __lcd_bl_on(void)
{
    struct view_data_display cfg;
    __display_cfg_get(&cfg);
    __lcd_bl_set(cfg.brightness);
    __display_st_set(true);
}

static void __lcd_bl_off(void)
{
    ledc_stop( LEDC_MODE, LEDC_CHANNEL, 0);
    __display_st_set(false);
}

static void __sleep_mode_timer_callback(void* arg)
{
    ESP_LOGI(TAG, "sleep mode, lcd bl off");
    __lcd_bl_off();
    __timer_st_set(false);

    bool st=0;
    st = 0;
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SCREEN_CTRL, &st, sizeof(st), portMAX_DELAY);
}

static void __timer_stop(void)
{
    if(  __timer_st_get()) {
        xSemaphoreTake(__g_timer_mutex, portMAX_DELAY);
        ESP_ERROR_CHECK(esp_timer_stop(sleep_timer_handle));
        xSemaphoreGive(__g_timer_mutex);
        __timer_st_set(false);
    }
}

static void __sleep_mode_restart(bool en, int min)
{
    __timer_stop();   
    if( ! en  || min ==0) {
        return;
    }
    __timer_st_set(true);
    xSemaphoreTake(__g_timer_mutex, portMAX_DELAY);
    ESP_ERROR_CHECK(esp_timer_start_once(sleep_timer_handle, (uint64_t) min *60 * 1000000 ));
    xSemaphoreGive(__g_timer_mutex);
}

static void __sleep_mode_init( bool  sleep_mode_en, int sleep_mode_time_min)
{
    const esp_timer_create_args_t timer_args = {
            .callback = &__sleep_mode_timer_callback,
            /* argument specified here will be passed to timer callback function */
            .arg = (void*) sleep_timer_handle,
            .name = "sleep mode"
    };
    ESP_ERROR_CHECK( esp_timer_create(&timer_args, &sleep_timer_handle));
    
    __timer_st_set(false);
    __sleep_mode_restart(sleep_mode_en, sleep_mode_time_min);
}


static void __display_cfg_save(struct view_data_display *p_data ) 
{
    esp_err_t ret = 0;
    ret = indicator_storage_write(DISPLAY_CFG_STORAGE, (void *)p_data, sizeof(struct view_data_display));
    if( ret != ESP_OK ) {
        ESP_LOGI(TAG, "cfg write err:%d", ret);
    } else {
        ESP_LOGI(TAG, "cfg write successful");
    }
}


static void __display_cfg_restore(void)
{
    esp_err_t ret = 0;
    struct view_data_display cfg;
    
    size_t len = sizeof(cfg);
    
    ret = indicator_storage_read(DISPLAY_CFG_STORAGE, (void *)&cfg, &len);
    if( ret == ESP_OK  && len== (sizeof(cfg)) ) {
        ESP_LOGI(TAG, "cfg read successful");
        __display_cfg_set(&cfg);
    } else {
        // err or not find
        if( ret == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGI(TAG, "cfg not find");
        }else {
            ESP_LOGI(TAG, "cfg read err:%d", ret);
        } 
        
        cfg.brightness = 80;
        cfg.sleep_mode_en = false;
        cfg.sleep_mode_time_min = 0;
        __display_cfg_set(&cfg);
    }
}

static void __view_event_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    switch (id)
    {
        case VIEW_EVENT_BRIGHTNESS_UPDATE: {
            int *p_brightness= (int *)event_data;
            struct view_data_display cfg;

            ESP_LOGI(TAG, "event: VIEW_EVENT_BRIGHTNESS_UPDATE, brightnes:%d", *p_brightness);
         
            __lcd_bl_set(*p_brightness);
           
            __display_cfg_get(&cfg);
            cfg.brightness=*p_brightness;
            __display_cfg_set(&cfg);
            break;
        }
        case VIEW_EVENT_DISPLAY_CFG_APPLY: {

            struct view_data_display * p_cfg = (struct view_data_display *)event_data;
            ESP_LOGI(TAG, "event: VIEW_EVENT_DISPLAY_CFG_APPLY");
            __display_cfg_print(p_cfg);

            __display_cfg_set(p_cfg);
            __display_cfg_save(p_cfg);
            __sleep_mode_restart(p_cfg->sleep_mode_en, p_cfg->sleep_mode_time_min);
            break;
        }
    
    default:
        break;
    }


}
int indicator_display_init(void)
{
    struct view_data_display cfg;
    __g_data_mutex  =  xSemaphoreCreateMutex();
    __g_timer_mutex = xSemaphoreCreateMutex();

    __display_cfg_restore();

    __display_cfg_get(&cfg);

    __lcd_bl_init(cfg.brightness);
    __display_st_set(true);

    __sleep_mode_init(cfg.sleep_mode_en, cfg.sleep_mode_time_min);

    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_DISPLAY_CFG, &cfg, sizeof(cfg), portMAX_DELAY);

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_BRIGHTNESS_UPDATE, 
                                                            __view_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_DISPLAY_CFG_APPLY, 
                                                            __view_event_handler, NULL, NULL));
    init_done_flag = true;
    return 0;
}

int indicator_display_sleep_restart(void)
{
    if( !init_done_flag ) {
        return 0;
    }
    struct view_data_display cfg;
    __display_cfg_get(&cfg);
    __sleep_mode_restart(cfg.sleep_mode_en, cfg.sleep_mode_time_min);
    return 0;
}

bool indicator_display_st_get(void)
{
    return __display_st_get();
}

int indicator_display_on(void)
{
   __lcd_bl_on();
   indicator_display_sleep_restart();
   return 0; 
}

int indicator_display_off(void)
{
    __lcd_bl_off();
    __timer_stop();
}
