#include "indicator_controller.h"
#include "lvgl.h"
#include "ui.h"

#include "indicator_view.h"

#include "ui_helpers.h"
#include <time.h>
#include <sys/time.h>

/**********************
 *  VARIABLES alias
 **********************/



/**********************
 *  STATIC VARIABLES
 **********************/

static const char *TAG = "controller";


static lv_obj_t * calendar;
static lv_calendar_date_t  _g_date_cfg;


/**********************  time cfg **********************/

static void __btn_save_time_enable(void)
{
    lv_obj_add_flag(ui_time_save, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(ui_time_save, lv_color_hex(0x529d53), LV_PART_MAIN | LV_STATE_DEFAULT );
}

static void calendar_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_user_data(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        lv_calendar_date_t d;
        lv_calendar_get_pressed_date(obj, &d);
        
        memcpy(&_g_date_cfg, &d, sizeof(lv_calendar_date_t));

        char buf[32];
        lv_snprintf(buf, sizeof(buf), "%02d/%02d/%d", d.day, d.month, d.year);
        lv_textarea_set_text(ta, buf);

        lv_obj_del(calendar);
        calendar = NULL;
        lv_obj_clear_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_TRANSP, 0);

        __btn_save_time_enable();
    }
}

static void data_cfg_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);

    if(code == LV_EVENT_FOCUSED) {
        if(lv_indev_get_type(lv_indev_get_act()) == LV_INDEV_TYPE_POINTER) {
            if(calendar == NULL) {
                lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
                calendar = lv_calendar_create(lv_layer_top());
                lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_50, 0);
                lv_obj_set_style_bg_color(lv_layer_top(), lv_palette_main(LV_PALETTE_GREY), 0);
                lv_obj_set_size(calendar, 300, 330);

                int year = 2023;
                int mon  = 1;
                int day  = 1;
                char *p_date = lv_textarea_get_text(ui_date_cfg);
                if( p_date !=NULL) {
                    sscanf(p_date, "%02d/%02d/%d", &day, &mon, &year );
                    ESP_LOGI(TAG, "calendar: %d/%d/%d", p_date, year,mon, day);
                }
                lv_calendar_set_today_date(calendar, year, mon, day);
                lv_calendar_set_showed_date(calendar, year, mon);
                lv_obj_align(calendar, LV_ALIGN_CENTER, 0, 30);
                lv_obj_add_event_cb(calendar, calendar_event_cb, LV_EVENT_ALL, ta);

                lv_calendar_header_dropdown_create(calendar);
            }
        }
    }
}

static void __time_cfg_apply(bool set_time)
{
    struct view_data_time_cfg cfg;
    char buf[32];

    lv_dropdown_get_selected_str( ui_time_format_cfg, buf, sizeof(buf));
    if( strcmp(buf, "12H") == 0 ) {
        cfg.time_format_24 = false;
    } else {
        cfg.time_format_24 = true;
    }

    if( lv_obj_has_state( ui_auto_update_cfg, LV_STATE_CHECKED) ) {
        cfg.auto_update = true;
    } else {
        cfg.auto_update = false;
    }

    //time
    struct tm tm;
    memset(&tm, 0 , sizeof(struct tm));

    tm.tm_year = _g_date_cfg.year > 1900 ? (_g_date_cfg.year- 1900): 0;
    tm.tm_mon  = _g_date_cfg.month - 1; //todo
    tm.tm_mday = _g_date_cfg.day;
    //ESP_LOGI(TAG, "date: %02d/%02d/%d", _g_date_cfg.month, _g_date_cfg.day, _g_date_cfg.year);


    lv_roller_get_selected_str(ui_hour_cfg, buf, sizeof(buf));
    //ESP_LOGI( TAG,"hour: %s\n", buf);
    tm.tm_hour = atoi(buf);

    lv_roller_get_selected_str(ui_min_cfg, buf, sizeof(buf));
    //ESP_LOGI( TAG,"min: %s\n", buf);
    tm.tm_min = atoi(buf);

    lv_roller_get_selected_str(ui_sec_cfg, buf, sizeof(buf));
    //ESP_LOGI( TAG,"sec: %s\n", buf);
    tm.tm_sec = atoi(buf);

    cfg.time = mktime(&tm);
    cfg.set_time = set_time;

    // zone
    int8_t zone;
    if( lv_obj_has_state( ui_zone_auto_update_cfg, LV_STATE_CHECKED) ) {
        cfg.auto_update_zone = true;
    } else {
        cfg.auto_update_zone = false;
    }

    lv_dropdown_get_selected_str(ui_time_zone_sign_cfg_, buf, sizeof(buf));
    if( strcmp(buf, "-") == 0 ) {
        lv_dropdown_get_selected_str(ui_time_zone_num_cfg, buf, sizeof(buf));
        zone = 0 - atoi(buf);
    } else {
        lv_dropdown_get_selected_str(ui_time_zone_num_cfg, buf, sizeof(buf));
        zone = atoi(buf);
    }
    cfg.zone = zone;

    if( lv_obj_has_state( ui_daylight_cfg, LV_STATE_CHECKED) ) {
        cfg.daylight = true;
    } else {
        cfg.daylight = false;
    }
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_TIME_CFG_APPLY, &cfg, sizeof(cfg), portMAX_DELAY);
}

static void __time_cfg_apply_event_cb(lv_event_t * e)
{
    __time_cfg_apply(false);
}

static void __time_save_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        __time_cfg_apply(true);
        lv_obj_clear_flag(ui_time_save, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_color(ui_time_save, lv_color_hex(0x6F6F6F), LV_PART_MAIN | LV_STATE_DEFAULT );
    }
}
static void __hour_min_sec_cfg_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        __btn_save_time_enable();
    }
}

void ui_event_zone_auto_update_cfg( lv_event_t * e) {
lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_VALUE_CHANGED &&  lv_obj_has_state(target,LV_STATE_CHECKED)  ) {
      _ui_flag_modify( ui_time_zone, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
}
if ( event_code == LV_EVENT_VALUE_CHANGED &&  !lv_obj_has_state(target,LV_STATE_CHECKED)  ) {
      _ui_flag_modify( ui_time_zone, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
}
}

void ui_event_min( lv_event_t * e) {
    char buf[32];
    lv_roller_get_selected_str(ui_min_cfg, buf, sizeof(buf));
    int tm_min = atoi(buf);
    ESP_LOGI( TAG, "min: %s (%d)\n", buf, tm_min);
}

static void __time_cfg_envent_init(void)
{
    _g_date_cfg.year=2023;
    _g_date_cfg.month=1;
    _g_date_cfg.day=1;

    lv_obj_add_event_cb(ui_zone_auto_update_cfg, ui_event_zone_auto_update_cfg, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui_auto_update_cfg, ui_event_auto_update_cfg, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_add_event_cb(ui_date_cfg, data_cfg_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_back2, __time_cfg_apply_event_cb, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(ui_time_save, __time_save_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_add_event_cb(ui_hour_cfg, __hour_min_sec_cfg_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui_min_cfg, __hour_min_sec_cfg_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui_sec_cfg, __hour_min_sec_cfg_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
}


/**********************  display cfg **********************/
static void __brighness_cfg_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    int32_t vaule = lv_slider_get_value(slider);
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_BRIGHTNESS_UPDATE, &vaule, sizeof(vaule), portMAX_DELAY);
}

static void __display_cfg_apply_event_cb(lv_event_t * e)
{
    struct view_data_display cfg;
    cfg.brightness = lv_slider_get_value(ui_brighness_cfg);
    if( ! lv_obj_has_state( ui_screen_always_on_cfg, LV_STATE_CHECKED) ) {
        cfg.sleep_mode_en = true;
        char *p_time =  lv_textarea_get_text(ui_turn_off_after_time_cfg);
        if( p_time) {
            cfg.sleep_mode_time_min = atoi(p_time);
        } else {
            cfg.sleep_mode_time_min = 0;
        }
    } else {
        cfg.sleep_mode_en = false;
    }
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_DISPLAY_CFG_APPLY, &cfg, sizeof(cfg), portMAX_DELAY);
}

static void __display_cfg_event_init(void)
{
    lv_obj_add_event_cb(ui_brighness_cfg, __brighness_cfg_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui_back1, __display_cfg_apply_event_cb, LV_EVENT_PRESSED, NULL);
}


/**********************  sensor chart **********************/

static void ui_event_sensor_co2_chart( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    lv_obj_t * cur_screen = lv_scr_act();
if ( event_code == LV_EVENT_CLICKED &&  cur_screen == ui_screen_sensor ) {
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_CO2_HISTORY, NULL, 0, portMAX_DELAY);

    _ui_screen_change( ui_screen_sensor_chart, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 0);
}
}

static void ui_event_sensor_tvoc_chart( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    lv_obj_t * cur_screen = lv_scr_act();
if ( event_code == LV_EVENT_CLICKED &&  cur_screen == ui_screen_sensor ) {
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_TVOC_HISTORY, NULL, 0, portMAX_DELAY);
      _ui_screen_change( ui_screen_sensor_chart, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 0);
}
}
static void ui_event_sensor_temp_chart( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    lv_obj_t * cur_screen = lv_scr_act();
if ( event_code == LV_EVENT_CLICKED &&  cur_screen == ui_screen_sensor ) {
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_TEMP_HISTORY, NULL, 0, portMAX_DELAY);
      _ui_screen_change( ui_screen_sensor_chart, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 0);
}
}
static void ui_event_sensor_humidity_chart( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    lv_obj_t * cur_screen = lv_scr_act();
if ( event_code == LV_EVENT_CLICKED &&  cur_screen == ui_screen_sensor ) {
     esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_HUMIDITY_HISTORY, NULL, 0, portMAX_DELAY);
      _ui_screen_change( ui_screen_sensor_chart, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 0);
}
}

static void __sensor_chart_event_init(void)
{
    lv_obj_add_event_cb(ui_co2, ui_event_sensor_co2_chart, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_tvoc_2, ui_event_sensor_tvoc_chart, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_temp2, ui_event_sensor_temp_chart, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_humidity2, ui_event_sensor_humidity_chart, LV_EVENT_ALL, NULL);
}

int indicator_controller_init(void)
{
    __time_cfg_envent_init();
    __display_cfg_event_init();
    __sensor_chart_event_init();

    return 0;
}