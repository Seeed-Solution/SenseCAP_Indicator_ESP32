#include "indicator_controller_ha.h"
#include "lvgl.h"
#include "ui.h"

#include "indicator_view.h"

#include "ui_helpers.h"
#include <time.h>
#include <sys/time.h>

static const char *TAG = "controller-ha";

void ui_event_screen_ha_data(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT)
    {
        _ui_screen_change(ui_screen_ha, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0);
    }
    if (event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT)
    {
        _ui_screen_change(ui_screen_sensor, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0);
    }
    if ( event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_TOP  ) {
        _ui_screen_change( ui_screen_time, LV_SCR_LOAD_ANIM_MOVE_TOP, 200, 0);
    }
}

void ui_event_screen_ha(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT)
    {
        _ui_screen_change(ui_screen_ha_ctrl, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0);
    }
    if (event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT)
    {
        _ui_screen_change(ui_screen_ha_data, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0);
    }
    if ( event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_TOP  ) {
        _ui_screen_change( ui_screen_time, LV_SCR_LOAD_ANIM_MOVE_TOP, 200, 0);
    }
}

void ui_event_screen_ha_ctrl(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);

    if( lv_slider_is_dragged(ui_switch8_slider)){
        // ESP_LOGI(TAG,"ui_switch8_slider dragging");
        return ;
    }
    
    if ( ( (lv_arc_t *)ui_switch5_arc)->dragging ){
        // ESP_LOGI(TAG,"ui_switch5_arc dragging");
        return;
    }

    if (event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT)
    {
        _ui_screen_change(ui_screen_setting, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0);
    }
    if (event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT)
    {
        _ui_screen_change(ui_screen_ha, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0);
    }
    // if ( event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_TOP  ) {
    //     _ui_screen_change( ui_screen_time, LV_SCR_LOAD_ANIM_MOVE_TOP, 200, 0);
    // }
}

void ui_event_switch1(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_CLICKED) {
        struct view_data_ha_switch_data switch_data;
        switch_data.index = 0;
        if( lv_obj_has_state(ui_switch1, LV_STATE_CHECKED) ) {
            switch_data.value = 1;
            lv_img_set_src(ui_switch1_logo, &ui_img_ic_switch2_on_png);
        } else {
            switch_data.value = 0;
            lv_img_set_src(ui_switch1_logo, &ui_img_ic_switch2_off_png);
        }
        ESP_LOGI(TAG, " switch%d: %d", switch_data.index+1, switch_data.value );
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_SWITCH_ST,  &switch_data, sizeof(switch_data), portMAX_DELAY);
    }
}

void ui_event_switch2(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_CLICKED) {
        struct view_data_ha_switch_data switch_data;
        switch_data.index = 1;
        if( lv_obj_has_state(ui_switch2, LV_STATE_CHECKED) ) {
            switch_data.value = 1;
            lv_img_set_src(ui_switch2_logo, &ui_img_ic_switch1_on_png);
        } else {
            switch_data.value = 0;
            lv_img_set_src(ui_switch2_logo, &ui_img_ic_switch1_off_png);
        }
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_SWITCH_ST,  &switch_data, sizeof(switch_data), portMAX_DELAY);
    }
}
void ui_event_switch3(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_CLICKED) {
        struct view_data_ha_switch_data switch_data;
        switch_data.index = 2;
        if( lv_obj_has_state(target, LV_STATE_CHECKED) ) {
            switch_data.value = 1;
            lv_obj_add_state(ui_switch3_switch, LV_STATE_CHECKED);
            lv_obj_add_state(ui_switch3, LV_STATE_CHECKED);
        } else {
            switch_data.value = 0;
            lv_obj_clear_state(ui_switch3_switch, LV_STATE_CHECKED);
            lv_obj_clear_state(ui_switch3, LV_STATE_CHECKED);
        }
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_SWITCH_ST,  &switch_data, sizeof(switch_data), portMAX_DELAY);
    }
}

void ui_event_switch4(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_CLICKED) {
        struct view_data_ha_switch_data switch_data;
        switch_data.index = 3;
        if( lv_obj_has_state(ui_switch4, LV_STATE_CHECKED) ) {
            switch_data.value = 1;
            lv_img_set_src(ui_switch4_logo, &ui_img_ic_switch2_on_png);
        } else {
            switch_data.value = 0;
            lv_img_set_src(ui_switch4_logo, &ui_img_ic_switch2_off_png);
        }
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_SWITCH_ST,  &switch_data, sizeof(switch_data), portMAX_DELAY);
    }
}

void ui_event_switch5_arc(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_VALUE_CHANGED) {
        int32_t vaule = lv_arc_get_value(ui_switch5_arc);
        char buf[16];
        sprintf(buf,"%d",vaule );
        lv_label_set_text(ui_switch5_arc_data,buf);

        struct view_data_ha_switch_data switch_data;
        switch_data.index = 4;
        switch_data.value = vaule;
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_SWITCH_ST,  &switch_data, sizeof(switch_data), portMAX_DELAY);
    }
}


void ui_event_switch6(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_CLICKED) {
        struct view_data_ha_switch_data switch_data;
        switch_data.index = 5;
        if( lv_obj_has_state(ui_switch6, LV_STATE_CHECKED) ) {
            switch_data.value = 1;
            lv_img_set_src(ui_switch6_logo, &ui_img_ic_switch1_on_png);
        } else {
            switch_data.value = 0;
            lv_img_set_src(ui_switch6_logo, &ui_img_ic_switch1_off_png);
        }
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_SWITCH_ST,  &switch_data, sizeof(switch_data), portMAX_DELAY);
    }
}

void ui_event_switch7(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_CLICKED) {
        struct view_data_ha_switch_data switch_data;
        switch_data.index = 6;
        if( lv_obj_has_state(target, LV_STATE_CHECKED) ) {
            switch_data.value = 1;
            lv_obj_add_state(ui_switch7_switch, LV_STATE_CHECKED);
            lv_obj_add_state(ui_switch7, LV_STATE_CHECKED);
        } else {
            switch_data.value = 0;
            lv_obj_clear_state(ui_switch7_switch, LV_STATE_CHECKED);
            lv_obj_clear_state(ui_switch7, LV_STATE_CHECKED);
        }
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_SWITCH_ST,  &switch_data, sizeof(switch_data), portMAX_DELAY);
    }
}

void ui_event_switch8_slider(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_VALUE_CHANGED) {  
        int32_t vaule = lv_slider_get_value(ui_switch8_slider);
        struct view_data_ha_switch_data switch_data;
        switch_data.index = 7;
        switch_data.value = vaule;
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_SWITCH_ST,  &switch_data, sizeof(switch_data), portMAX_DELAY);
    }
}

void ui_event_ha_init(void)
{
    lv_obj_add_event_cb(ui_screen_ha_data, ui_event_screen_ha_data, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_screen_ha, ui_event_screen_ha, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_screen_ha_ctrl, ui_event_screen_ha_ctrl, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_switch1, ui_event_switch1, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_switch2, ui_event_switch2, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_switch3, ui_event_switch3, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_switch3_switch, ui_event_switch3, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_switch4, ui_event_switch4, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_switch5_arc, ui_event_switch5_arc, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_switch6, ui_event_switch6, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_switch7, ui_event_switch7, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_switch7_switch, ui_event_switch7, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_switch8_slider, ui_event_switch8_slider, LV_EVENT_VALUE_CHANGED, NULL);
}

int indicator_controller_ha_init(void)
{
    ui_event_ha_init();
    return 0;
}