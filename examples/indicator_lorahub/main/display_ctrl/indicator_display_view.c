#include "indicator_display.h"
#include "ui.h"
#include "view_data.h"

// static void __brighness_cfg_event_cb(lv_event_t * e)
void brighness_cfg_event_cb(lv_event_t *e) // Value changed
{
    lv_obj_t *slider = lv_event_get_target(e);
    int32_t   vaule  = lv_slider_get_value(slider);
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_BRIGHTNESS_UPDATE, &vaule, sizeof(vaule), portMAX_DELAY);
}

// static void __display_cfg_apply_event_cb(lv_event_t * e)
void display_cfg_apply_event_cb(lv_event_t *e) // defocused the textarea
{
    lv_obj_t *target = lv_event_get_target(e);
    bool      dis    = lv_obj_has_state(ui_sleep_mode_cfg1, LV_STATE_CHECKED); // true always on

    struct view_data_display cfg;
    memset(&cfg, 0, sizeof(cfg));
    if (target == ui_sleep_mode_cfg1) {
        if (!dis) {
            cfg.sleep_mode_time_min = 0;
        }
        cfg.sleep_mode_en = false;
    } else if (target == ui_sleep_mode_time_cfg1) { // set a time, now it must be not always on
        cfg.sleep_mode_en = true;
    }
    char *p_time = lv_textarea_get_text(ui_sleep_mode_time_cfg1);
    if (dis || !p_time) {
        cfg.sleep_mode_time_min = 0;
    } else {
        cfg.sleep_mode_time_min = atoi(p_time);
    }
    cfg.brightness = lv_slider_get_value(ui_brighness_cfg1);
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_DISPLAY_CFG_APPLY, &cfg, sizeof(cfg), portMAX_DELAY);
}


void brighness_update_callback(lv_event_t *e)
{
    struct view_data_display cfg;
    __display_cfg_get(&cfg);
    lv_slider_set_value(ui_brighness_cfg1, cfg.brightness, LV_ANIM_OFF);
    if (cfg.sleep_mode_en) {
        lv_obj_clear_state(ui_sleep_mode_cfg1, LV_STATE_CHECKED);
        lv_obj_clear_flag(ui_PnlSleepModeTime, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_state(ui_sleep_mode_cfg1, LV_STATE_CHECKED);
    }
    char str[16] = {0};
    sprintf(str, "%d", cfg.sleep_mode_time_min);
    lv_textarea_set_text(ui_sleep_mode_time_cfg1, str);
}
