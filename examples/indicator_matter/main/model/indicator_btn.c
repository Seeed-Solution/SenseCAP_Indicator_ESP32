#include "indicator_btn.h"
#include "indicator_display.h"
#include "bsp_btn.h"

static uint32_t hold_cnt=0;
static bool sleep_flag=false;
static bool sleep_start_flag=false;

static esp_timer_handle_t   factory_reset_timer_handle;

static void __factory_reset_callback(void* arg)
{
    ESP_ERROR_CHECK(nvs_flash_erase());
    fflush(stdout);
    esp_restart();
}

static void __btn_click_callback(void* arg)
{
    bool st=0;
    if( sleep_flag ) {
        ESP_LOGI("btn", "click, cur st: sleep mode, restart!");
        fflush(stdout);
        esp_restart();
        return;
    }
    if( indicator_display_st_get()) {
        ESP_LOGI("btn", "click, off");
        indicator_display_off();

        st = 0;
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SCREEN_CTRL, &st, sizeof(st), portMAX_DELAY);
    } else {
        ESP_LOGI("btn", "click, on");
        
        st = 1;
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SCREEN_CTRL, &st, sizeof(st), portMAX_DELAY);

        indicator_display_on();
    }
}

static void __btn_double_click_callback(void* arg)
{
    ESP_LOGI("btn", "double click");
}

static void __btn_press_start_callback(void* arg)
{
    ESP_LOGI("btn", "press start");
    hold_cnt = 1500;
    sleep_start_flag = false;
}

static void __btn_long_press_hold_callback(void* arg)
{
    static bool factory_reset_flag = false;
    //ESP_LOGI("btn", "long press hold");
    // default CONFIG_BUTTON_PERIOD_TIME_MS=5ms 
    hold_cnt += CONFIG_BUTTON_PERIOD_TIME_MS;

    if( hold_cnt >= 3000  && hold_cnt < 10000) {
        if( sleep_flag ) {
            ESP_LOGI("btn", "wake, restart");
            fflush(stdout);
            esp_restart();
        } else if( !sleep_start_flag) {
            sleep_start_flag = true;
            ESP_LOGI("btn", "entry sleep mode");
            indicator_display_off();
            bool st = 0;
            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SCREEN_CTRL, &st, sizeof(st), portMAX_DELAY);
        }

    } else if( hold_cnt >= 10000  && !factory_reset_flag) {
        ESP_LOGI("btn", "factory reset");
        factory_reset_flag = true;
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_FACTORY_RESET, NULL, 0, portMAX_DELAY);
        
        bool st = 1;
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SCREEN_CTRL, &st, sizeof(st), portMAX_DELAY);
        indicator_display_on();

        const esp_timer_create_args_t timer_args = {
            .callback = &__factory_reset_callback,
            /* argument specified here will be passed to timer callback function */
            .arg = (void*) factory_reset_timer_handle,
            .name = "factory_reset"
        };
        ESP_ERROR_CHECK( esp_timer_create(&timer_args, &factory_reset_timer_handle));
        ESP_ERROR_CHECK(esp_timer_start_once(factory_reset_timer_handle, 1000000*3)); //3s
        
    }
}

static void __btn_press_up_callback(void* arg)
{
    if( hold_cnt >= 3000  && hold_cnt < 15000) {
        //sleep
        ESP_LOGI("btn", "entry sleep mode");
        sleep_flag = true;
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SHUTDOWN, NULL, 0, portMAX_DELAY);
        //to sleep , close sensor and wifi
    }
}


int indicator_btn_init(void)
{
    bsp_btn_register_callback( BOARD_BTN_ID_USER, BUTTON_SINGLE_CLICK,     __btn_click_callback, NULL);
    bsp_btn_register_callback( BOARD_BTN_ID_USER, BUTTON_DOUBLE_CLICK, __btn_double_click_callback, NULL);
    bsp_btn_register_callback( BOARD_BTN_ID_USER, BUTTON_LONG_PRESS_START, __btn_press_start_callback, NULL);
    bsp_btn_register_callback( BOARD_BTN_ID_USER, BUTTON_LONG_PRESS_HOLD,  __btn_long_press_hold_callback, NULL);
    bsp_btn_register_callback( BOARD_BTN_ID_USER, BUTTON_PRESS_UP,  __btn_press_up_callback, NULL);
}
