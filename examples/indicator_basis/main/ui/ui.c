// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.3.3
// PROJECT: sensecap

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
void up_Animation( lv_obj_t *TargetObject, int delay);
void ui_event_screen_time( lv_event_t * e);
lv_obj_t *ui_screen_time;
lv_obj_t *ui_background;
lv_obj_t *ui_hour;
lv_obj_t *ui_hour_dis;
lv_obj_t *ui_min;
lv_obj_t *ui_min_dis;
lv_obj_t *ui_colon;
lv_obj_t *ui_adorn;
lv_obj_t *ui_wifi_st_1;
lv_obj_t *ui_date_panel;
lv_obj_t *ui_date;
lv_obj_t *ui_location;
lv_obj_t *ui_location_Icon;
lv_obj_t *ui_city;
void ui_event_screen_sensor( lv_event_t * e);
lv_obj_t *ui_screen_sensor;
void ui_event_wifi__st_button_2( lv_event_t * e);
lv_obj_t *ui_wifi__st_button_2;
lv_obj_t *ui_wifi_st_2;
lv_obj_t *ui_time2;
lv_obj_t *ui_co2;
lv_obj_t *ui_co2_icon;
lv_obj_t *ui_co2_title;
lv_obj_t *ui_co2_data;
lv_obj_t *ui_co2_unit;
lv_obj_t *ui_tvoc_2;
lv_obj_t *ui_tvoc_icon_2;
lv_obj_t *ui_tvoc_title_2;
lv_obj_t *ui_tvoc_data;
lv_obj_t *ui_tvoc_unit_2;
lv_obj_t *ui_temp2;
lv_obj_t *ui_temp_icon_2;
lv_obj_t *ui_temp_title_2;
lv_obj_t *ui_temp_data_2;
lv_obj_t *ui_temp_unit_2;
lv_obj_t *ui_humidity2;
lv_obj_t *ui_humidity_icon_2;
lv_obj_t *ui_humidity_title_2;
lv_obj_t *ui_humidity_data_2;
lv_obj_t *ui_humidity_unit_2;
lv_obj_t *ui_scrolldots2;
lv_obj_t *ui_scrolldots21;
lv_obj_t *ui_scrolldots22;
lv_obj_t *ui_scrolldots23;
void ui_event_screen_setting( lv_event_t * e);
lv_obj_t *ui_screen_setting;
void ui_event_wifi__st_button_3( lv_event_t * e);
lv_obj_t *ui_wifi__st_button_3;
lv_obj_t *ui_wifi_st_3;
lv_obj_t *ui_time3;
lv_obj_t *ui_setting_icon;
lv_obj_t *ui_setting_title;
void ui_event_setting_wifi( lv_event_t * e);
lv_obj_t *ui_setting_wifi;
lv_obj_t *ui_setting_wifi_icon;
lv_obj_t *ui_setting_wifi_title;
void ui_event_setting_display( lv_event_t * e);
lv_obj_t *ui_setting_display;
lv_obj_t *ui_setting_display_icon;
lv_obj_t *ui_setting_display_title;
void ui_event_setting_time( lv_event_t * e);
lv_obj_t *ui_setting_time;
lv_obj_t *ui_setting_time_icon;
lv_obj_t *ui_setting_time_title;
lv_obj_t *ui_scrolldots3;
lv_obj_t *ui_scrolldots31;
lv_obj_t *ui_scrolldots32;
lv_obj_t *ui_scrolldots33;
lv_obj_t *ui_screen_display;
lv_obj_t *ui_wifi_st_4;
void ui_event_back1( lv_event_t * e);
lv_obj_t *ui_back1;
lv_obj_t *ui_display_title;
lv_obj_t *ui_brighness;
lv_obj_t *ui_brighness_cfg;
lv_obj_t *ui_brighness_title;
lv_obj_t *ui_brighness_icon_1;
lv_obj_t *ui_brighness_icon_2;
lv_obj_t *ui_screen_always_on;
lv_obj_t *ui_screen_always_on_title;
void ui_event_screen_always_on_cfg( lv_event_t * e);
lv_obj_t *ui_screen_always_on_cfg;
lv_obj_t *ui_turn_off_after_time;
lv_obj_t *ui_after;
void ui_event_sleep_mode_time_cfg( lv_event_t * e);
lv_obj_t *ui_turn_off_after_time_cfg;
lv_obj_t *ui_min;
void ui_event_display_keyboard( lv_event_t * e);
lv_obj_t *ui_display_keyboard;
lv_obj_t *ui_screen_date_time;
lv_obj_t *ui_wifi_st_5;
void ui_event_back2( lv_event_t * e);
lv_obj_t *ui_back2;
lv_obj_t *ui_date_time_title;
lv_obj_t *ui_time_format;
lv_obj_t *ui_time_format_title;
lv_obj_t *ui_time_format_cfg;
lv_obj_t *ui_auto_update;
lv_obj_t *ui_auto_update_title;
void ui_event_auto_update_cfg( lv_event_t * e);
lv_obj_t *ui_auto_update_cfg;
lv_obj_t *ui_date_time;
lv_obj_t * ui_time_zone;
lv_obj_t *  ui_zone_auto_update_cfg;
lv_obj_t *ui_time_zone_title;
lv_obj_t *ui_time_zone_num_cfg;
lv_obj_t *ui_utc_tile;
lv_obj_t *ui_time_zone_sign_cfg_;
lv_obj_t *ui_daylight_title;
lv_obj_t *ui_daylight_cfg;
lv_obj_t *ui_manual_setting_title;
lv_obj_t *ui_date_cfg;
lv_obj_t *ui_hour_cfg;
lv_obj_t *ui_min_cfg;
lv_obj_t *ui_sec_cfg;
lv_obj_t *ui_time_label1;
lv_obj_t *ui_time_label2;
lv_obj_t *ui_screen_wifi;
lv_obj_t *ui_wifi_st_6;
lv_obj_t *ui_wifi_title;
lv_obj_t * ui_time_save;
void ui_event_back3( lv_event_t * e);
lv_obj_t *ui_back3;

lv_obj_t *ui_screen_factory;
lv_obj_t *ui_factory_resetting_title;

lv_obj_t *ui_screen_sensor_chart;
lv_obj_t *ui_wifi_st_7;
lv_obj_t *ui_back4;
lv_obj_t *ui_sensor_data_title;
lv_obj_t * ui_sensor_chart_day;
lv_chart_series_t * ui_sensor_chart_day_series;

lv_obj_t * ui_sensor_chart_week;
lv_chart_series_t * ui_sensor_chart_week_series_hight;
lv_chart_series_t * ui_sensor_chart_week_series_low;


static lv_obj_t *ui_screen_last;
///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////
void up_Animation( lv_obj_t *TargetObject, int delay)
{
lv_anim_t PropertyAnimation_0;
lv_anim_init(&PropertyAnimation_0);
lv_anim_set_time(&PropertyAnimation_0, 200);
lv_anim_set_user_data(&PropertyAnimation_0,TargetObject);
lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_y );
lv_anim_set_values(&PropertyAnimation_0, -50, 0 );
lv_anim_set_path_cb( &PropertyAnimation_0, lv_anim_path_ease_out);
lv_anim_set_delay( &PropertyAnimation_0, delay + 0 );
lv_anim_set_playback_time(&PropertyAnimation_0, 0);
lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
lv_anim_set_early_apply( &PropertyAnimation_0, false );
lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_y );
lv_anim_start(&PropertyAnimation_0);
lv_anim_t PropertyAnimation_1;
lv_anim_init(&PropertyAnimation_1);
lv_anim_set_time(&PropertyAnimation_1, 100);
lv_anim_set_user_data(&PropertyAnimation_1,TargetObject);
lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_opacity );
lv_anim_set_values(&PropertyAnimation_1, 0, 255 );
lv_anim_set_path_cb( &PropertyAnimation_1, lv_anim_path_linear);
lv_anim_set_delay( &PropertyAnimation_1, delay + 0 );
lv_anim_set_playback_time(&PropertyAnimation_1, 0);
lv_anim_set_playback_delay(&PropertyAnimation_1, 0);
lv_anim_set_repeat_count(&PropertyAnimation_1, 0);
lv_anim_set_repeat_delay(&PropertyAnimation_1, 0);
lv_anim_set_early_apply( &PropertyAnimation_1, true );
lv_anim_set_get_value_cb(&PropertyAnimation_1, &_ui_anim_callback_get_opacity );
lv_anim_start(&PropertyAnimation_1);

}

///////////////////// FUNCTIONS ////////////////////

void ui_event_screen_time( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT  ) {
      _ui_screen_change( ui_screen_sensor, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0);
}
if ( event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT  ) {
      _ui_screen_change( ui_screen_setting, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0);
}
if ( event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_BOTTOM  ) {
      ui_screen_last = ui_screen_time;
      _ui_screen_change( ui_screen_wifi, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 200, 0);
}
}
void ui_event_wifi__st_button_1( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_CLICKED) {
      ui_screen_last = ui_screen_time;
      _ui_screen_change( ui_screen_wifi, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 200, 0);
}
}
void ui_event_screen_sensor( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT  ) {
      _ui_screen_change( ui_screen_time, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0);
}
if ( event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT  ) {
      _ui_screen_change( ui_screen_setting, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0);
}
if ( event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_TOP  ) {
      _ui_screen_change( ui_screen_time, LV_SCR_LOAD_ANIM_MOVE_TOP, 200, 0);
}
}
void ui_event_wifi__st_button_2( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_CLICKED) {
      ui_screen_last = ui_screen_sensor;
      _ui_screen_change( ui_screen_wifi, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 200, 0);
}
}
void ui_event_screen_setting( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT  ) {
      _ui_screen_change( ui_screen_time, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0);
}
if ( event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT  ) {
      _ui_screen_change( ui_screen_sensor, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0);
}
if ( event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_TOP  ) {
      _ui_screen_change( ui_screen_time, LV_SCR_LOAD_ANIM_MOVE_TOP, 200, 0);
}
}
void ui_event_wifi__st_button_3( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_CLICKED) {
      ui_screen_last = ui_screen_setting;
      _ui_screen_change( ui_screen_wifi, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 200, 0);
}
}
void ui_event_setting_wifi( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    lv_obj_t * cur_screen = lv_scr_act();
if ( event_code == LV_EVENT_CLICKED &&  cur_screen == ui_screen_setting ) {
      ui_screen_last = ui_screen_setting;
      _ui_screen_change( ui_screen_wifi, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 200, 0);
}
}
void ui_event_setting_display( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    lv_obj_t * cur_screen = lv_scr_act();
if ( event_code == LV_EVENT_CLICKED &&  cur_screen == ui_screen_setting ) {
      _ui_screen_change( ui_screen_display, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 200, 0);
}
}
void ui_event_setting_time( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    lv_obj_t * cur_screen = lv_scr_act();
if ( event_code == LV_EVENT_CLICKED &&  cur_screen == ui_screen_setting ) {
      _ui_screen_change( ui_screen_date_time, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 200, 0);
}
}
void ui_event_back1( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_CLICKED) {
      _ui_screen_change( ui_screen_setting, LV_SCR_LOAD_ANIM_OVER_TOP, 200, 0);
}
}
void ui_event_screen_always_on_cfg( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_VALUE_CHANGED &&  lv_obj_has_state(target,LV_STATE_CHECKED)  ) {
      _ui_flag_modify( ui_display_keyboard, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
      _ui_flag_modify( ui_turn_off_after_time, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
}
if ( event_code == LV_EVENT_VALUE_CHANGED &&  !lv_obj_has_state(target,LV_STATE_CHECKED)  ) {
      _ui_flag_modify( ui_turn_off_after_time, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
}
}
void ui_event_sleep_mode_time_cfg( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_CLICKED) {
      _ui_flag_modify( ui_display_keyboard, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
}
}
void ui_event_display_keyboard( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_READY) {
      _ui_flag_modify( ui_display_keyboard, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
}
if ( event_code == LV_EVENT_CANCEL) {
      _ui_flag_modify( ui_display_keyboard, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
}
if ( event_code == LV_EVENT_DEFOCUSED) {
      _ui_flag_modify( ui_display_keyboard, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
}
}
void ui_event_back2( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_CLICKED) {
      _ui_screen_change( ui_screen_setting, LV_SCR_LOAD_ANIM_OVER_TOP, 200, 0);
}
}
void ui_event_auto_update_cfg( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_VALUE_CHANGED &&  lv_obj_has_state(target,LV_STATE_CHECKED)  ) {
      _ui_flag_modify( ui_date_time, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
}
if ( event_code == LV_EVENT_VALUE_CHANGED &&  !lv_obj_has_state(target,LV_STATE_CHECKED)  ) {
      _ui_flag_modify( ui_date_time, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
}
}
void ui_event_back3( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_CLICKED) {
      
      if( ui_screen_last == NULL ) {
            ui_screen_last = ui_screen_time;
      }
      _ui_screen_change( ui_screen_last, LV_SCR_LOAD_ANIM_OVER_TOP, 200, 0);
}
}

void ui_event_back4( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_CLICKED) {
      lv_disp_t *dispp = lv_disp_get_default();
      lv_theme_t *theme = lv_theme_default_init(dispp, lv_color_hex(0x529d53), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
      lv_disp_set_theme(dispp, theme);
      _ui_screen_change( ui_screen_sensor, LV_SCR_LOAD_ANIM_OVER_RIGHT, 200, 0);
}
}


///////////////////// SCREENS ////////////////////
void ui_screen_time_screen_init(void)
{
ui_screen_time = lv_obj_create(NULL);
lv_obj_clear_flag( ui_screen_time, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_background = lv_img_create(ui_screen_time);
lv_img_set_src(ui_background, &ui_img_background_png);
lv_obj_set_width( ui_background, lv_pct(100));
lv_obj_set_height( ui_background, lv_pct(100));
lv_obj_set_align( ui_background, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_background, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_background, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

// ui_wifi__st_button_1 = lv_btn_create(ui_background);
// lv_obj_set_width( ui_wifi__st_button_1, 60);
// lv_obj_set_height( ui_wifi__st_button_1, 60);
// lv_obj_set_x( ui_wifi__st_button_1, -10 );
// lv_obj_set_y( ui_wifi__st_button_1, 10 );
// lv_obj_set_align( ui_wifi__st_button_1, LV_ALIGN_TOP_RIGHT );
// lv_obj_add_flag( ui_wifi__st_button_1, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
// lv_obj_clear_flag( ui_wifi__st_button_1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
// lv_obj_set_style_bg_color(ui_wifi__st_button_1, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
// lv_obj_set_style_bg_opa(ui_wifi__st_button_1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
// lv_obj_set_style_bg_grad_color(ui_wifi__st_button_1, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );

ui_wifi_st_1 = lv_img_create(ui_background);
lv_img_set_src(ui_wifi_st_1, &ui_img_wifi_disconet_png);
lv_obj_set_width( ui_wifi_st_1, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_wifi_st_1, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_wifi_st_1, -20 );
lv_obj_set_y( ui_wifi_st_1, 20 );
lv_obj_set_align( ui_wifi_st_1, LV_ALIGN_TOP_RIGHT );
lv_obj_add_flag( ui_wifi_st_1, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_wifi_st_1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_add_flag( ui_wifi_st_1, LV_OBJ_FLAG_CLICKABLE );    /// Flags
lv_obj_add_event_cb(ui_wifi_st_1, ui_event_wifi__st_button_1, LV_EVENT_ALL, NULL);



ui_hour = lv_obj_create(ui_background);
lv_obj_set_width( ui_hour, 200);
lv_obj_set_height( ui_hour, 170);
lv_obj_set_x( ui_hour, -110 );
lv_obj_set_y( ui_hour, 0 );
lv_obj_set_align( ui_hour, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_hour, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_hour, lv_color_hex(0x2E2E2E), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_hour, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_hour_dis = lv_label_create(ui_hour);
lv_obj_set_width( ui_hour_dis, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_hour_dis, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_hour_dis, LV_ALIGN_CENTER );
lv_label_set_text(ui_hour_dis,"00");
lv_obj_set_style_text_font(ui_hour_dis, &ui_font_font4, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_min = lv_obj_create(ui_background);
lv_obj_set_width( ui_min, 200);
lv_obj_set_height( ui_min, 170);
lv_obj_set_x( ui_min, 110 );
lv_obj_set_y( ui_min, 0 );
lv_obj_set_align( ui_min, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_min, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_min, lv_color_hex(0x2E2E2E), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_min, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_min_dis = lv_label_create(ui_min);
lv_obj_set_width( ui_min_dis, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_min_dis, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_min_dis, LV_ALIGN_CENTER );
lv_label_set_text(ui_min_dis,"00");
lv_obj_set_style_text_font(ui_min_dis, &ui_font_font4, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_location = lv_obj_create(ui_background);
lv_obj_set_width( ui_location, 399);
lv_obj_set_height( ui_location, 30);
lv_obj_set_x( ui_location, 6 );
lv_obj_set_y( ui_location, 103 );
lv_obj_set_align( ui_location, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_location, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_location, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_location, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_grad_color(ui_location, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_color(ui_location, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_location, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_city = lv_label_create(ui_location);
lv_obj_set_width( ui_city, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_city, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_city, LV_ALIGN_RIGHT_MID );
lv_label_set_text(ui_city," -- ");
lv_obj_set_style_text_font(ui_city, &lv_font_montserrat_16, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_location_Icon = lv_img_create(ui_background);
lv_img_set_src(ui_location_Icon, &ui_img_location2_png);
lv_obj_set_width( ui_location_Icon, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_location_Icon, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_location_Icon, 199 );
lv_obj_set_y( ui_location_Icon, 103 );
lv_obj_set_align( ui_location_Icon, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_location_Icon, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_location_Icon, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_date_panel = lv_obj_create(ui_background);
lv_obj_set_width( ui_date_panel, 235);
lv_obj_set_height( ui_date_panel, 30);
lv_obj_set_x( ui_date_panel, -103 );
lv_obj_set_y( ui_date_panel, -108 );
lv_obj_set_align( ui_date_panel, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_date_panel, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_date_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_date_panel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_grad_color(ui_date_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_color(ui_date_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_date_panel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_date = lv_label_create(ui_date_panel);
lv_obj_set_width( ui_date, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_date, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_date, LV_ALIGN_LEFT_MID );
lv_label_set_text(ui_date,"Monday, 01 / 01 / 1970");
lv_label_set_recolor(ui_date,"true");
lv_obj_set_style_text_font(ui_date, &lv_font_montserrat_16, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_colon = lv_label_create(ui_background);
lv_obj_set_width( ui_colon, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_colon, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_colon, LV_ALIGN_CENTER );
lv_label_set_text(ui_colon,":");
lv_obj_set_style_text_font(ui_colon, &ui_font_font3, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_adorn = lv_obj_create(ui_background);
lv_obj_set_width( ui_adorn, 480);
lv_obj_set_height( ui_adorn, 6);
lv_obj_set_align( ui_adorn, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_adorn, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_adorn, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_adorn, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_grad_color(ui_adorn, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_color(ui_adorn, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_adorn, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_add_event_cb(ui_screen_time, ui_event_screen_time, LV_EVENT_ALL, NULL);
}
void ui_screen_sensor_screen_init(void)
{
ui_screen_sensor = lv_obj_create(NULL);
lv_obj_clear_flag( ui_screen_sensor, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_wifi__st_button_2 = lv_btn_create(ui_screen_sensor);
lv_obj_set_width( ui_wifi__st_button_2, 60);
lv_obj_set_height( ui_wifi__st_button_2, 60);
lv_obj_set_x( ui_wifi__st_button_2, -10 );
lv_obj_set_y( ui_wifi__st_button_2, 10 );
lv_obj_set_align( ui_wifi__st_button_2, LV_ALIGN_TOP_RIGHT );
lv_obj_add_flag( ui_wifi__st_button_2, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_wifi__st_button_2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_wifi__st_button_2, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_wifi__st_button_2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_grad_color(ui_wifi__st_button_2, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );

ui_wifi_st_2 = lv_img_create(ui_wifi__st_button_2);
lv_img_set_src(ui_wifi_st_2, &ui_img_wifi_disconet_png);
lv_obj_set_width( ui_wifi_st_2, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_wifi_st_2, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_wifi_st_2, LV_ALIGN_TOP_RIGHT );
lv_obj_add_flag( ui_wifi_st_2, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_wifi_st_2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_time2 = lv_label_create(ui_screen_sensor);
lv_obj_set_width( ui_time2, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_time2, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_time2, 30 );
lv_obj_set_y( ui_time2, 20 );
lv_label_set_text(ui_time2,"21:20");
lv_obj_set_style_text_font(ui_time2, &ui_font_font1, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_co2 = lv_btn_create(ui_screen_sensor);
lv_obj_set_width( ui_co2, 436);
lv_obj_set_height( ui_co2, 140);
lv_obj_set_x( ui_co2, 0 );
lv_obj_set_y( ui_co2, -100 );
lv_obj_set_align( ui_co2, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_co2, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_co2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_co2, lv_color_hex(0x529D53), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_co2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_co2_icon = lv_img_create(ui_co2);
lv_img_set_src(ui_co2_icon, &ui_img_co2_png);
lv_obj_set_width( ui_co2_icon, LV_SIZE_CONTENT);  /// 44
lv_obj_set_height( ui_co2_icon, LV_SIZE_CONTENT);   /// 53
lv_obj_set_x( ui_co2_icon, -184 );
lv_obj_set_y( ui_co2_icon, -40 );
lv_obj_set_align( ui_co2_icon, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_co2_icon, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_co2_icon, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_co2_title = lv_label_create(ui_co2);
lv_obj_set_width( ui_co2_title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_co2_title, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_co2_title, -134 );
lv_obj_set_y( ui_co2_title, -40 );
lv_obj_set_align( ui_co2_title, LV_ALIGN_CENTER );
lv_label_set_text(ui_co2_title,"CO2");
lv_obj_set_style_text_font(ui_co2_title, &ui_font_font0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_co2_data = lv_label_create(ui_co2);
lv_obj_set_width( ui_co2_data, 200);
lv_obj_set_height( ui_co2_data, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_co2_data, -90 );
lv_obj_set_y( ui_co2_data, -1 );
lv_obj_set_align( ui_co2_data, LV_ALIGN_CENTER );
lv_label_set_text(ui_co2_data,"N/A ");
lv_obj_set_style_text_align(ui_co2_data, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_co2_data, &ui_font_font3, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_co2_unit = lv_label_create(ui_co2);
lv_obj_set_width( ui_co2_unit, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_co2_unit, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_co2_unit, 35 );
lv_obj_set_y( ui_co2_unit, 0 );
lv_obj_set_align( ui_co2_unit, LV_ALIGN_CENTER );
lv_label_set_text(ui_co2_unit,"ppm");
lv_obj_set_style_text_font(ui_co2_unit, &ui_font_font0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_tvoc_2 = lv_btn_create(ui_screen_sensor);
lv_obj_set_width( ui_tvoc_2, 140);
lv_obj_set_height( ui_tvoc_2, 200);
lv_obj_set_x( ui_tvoc_2, -148 );
lv_obj_set_y( ui_tvoc_2, 80 );
lv_obj_set_align( ui_tvoc_2, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_tvoc_2, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_tvoc_2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_tvoc_2, lv_color_hex(0xE06D3D), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_tvoc_2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_tvoc_icon_2 = lv_img_create(ui_tvoc_2);
lv_img_set_src(ui_tvoc_icon_2, &ui_img_tvoc_png);
lv_obj_set_width( ui_tvoc_icon_2, LV_SIZE_CONTENT);  /// 44
lv_obj_set_height( ui_tvoc_icon_2, LV_SIZE_CONTENT);   /// 53
lv_obj_set_x( ui_tvoc_icon_2, -40 );
lv_obj_set_y( ui_tvoc_icon_2, -70 );
lv_obj_set_align( ui_tvoc_icon_2, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_tvoc_icon_2, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_tvoc_icon_2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_tvoc_title_2 = lv_label_create(ui_tvoc_2);
lv_obj_set_width( ui_tvoc_title_2, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_tvoc_title_2, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_tvoc_title_2, 15 );
lv_obj_set_y( ui_tvoc_title_2, -70 );
lv_obj_set_align( ui_tvoc_title_2, LV_ALIGN_CENTER );
lv_label_set_text(ui_tvoc_title_2,"tVOC");
lv_obj_set_style_text_font(ui_tvoc_title_2, &ui_font_font0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_tvoc_data = lv_label_create(ui_tvoc_2);
lv_obj_set_width( ui_tvoc_data, 100);
lv_obj_set_height( ui_tvoc_data, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_tvoc_data, -21 );
lv_obj_set_y( ui_tvoc_data, 0 );
lv_obj_set_align( ui_tvoc_data, LV_ALIGN_CENTER );
lv_label_set_text(ui_tvoc_data,"N/A");
lv_obj_set_style_text_align(ui_tvoc_data, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_tvoc_data, &ui_font_font2, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_tvoc_unit_2 = lv_label_create(ui_tvoc_2);
lv_obj_set_width( ui_tvoc_unit_2, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_tvoc_unit_2, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_tvoc_unit_2, 44 );
lv_obj_set_y( ui_tvoc_unit_2, 4 );
lv_obj_set_align( ui_tvoc_unit_2, LV_ALIGN_CENTER );
lv_label_set_text(ui_tvoc_unit_2,"");
lv_obj_set_style_text_font(ui_tvoc_unit_2, &lv_font_montserrat_16, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_temp2 = lv_btn_create(ui_screen_sensor);
lv_obj_set_width( ui_temp2, 140);
lv_obj_set_height( ui_temp2, 200);
lv_obj_set_x( ui_temp2, 0 );
lv_obj_set_y( ui_temp2, 80 );
lv_obj_set_align( ui_temp2, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_temp2, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_temp2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_temp2, lv_color_hex(0xEEBF41), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_temp2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_temp_icon_2 = lv_img_create(ui_temp2);
lv_img_set_src(ui_temp_icon_2, &ui_img_temp_2_png);
lv_obj_set_width( ui_temp_icon_2, LV_SIZE_CONTENT);  /// 44
lv_obj_set_height( ui_temp_icon_2, LV_SIZE_CONTENT);   /// 53
lv_obj_set_x( ui_temp_icon_2, -40 );
lv_obj_set_y( ui_temp_icon_2, -70 );
lv_obj_set_align( ui_temp_icon_2, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_temp_icon_2, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_temp_icon_2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_temp_title_2 = lv_label_create(ui_temp2);
lv_obj_set_width( ui_temp_title_2, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_temp_title_2, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_temp_title_2, 15 );
lv_obj_set_y( ui_temp_title_2, -70 );
lv_obj_set_align( ui_temp_title_2, LV_ALIGN_CENTER );
lv_label_set_text(ui_temp_title_2,"Temp");
lv_obj_set_style_text_font(ui_temp_title_2, &ui_font_font0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_temp_data_2 = lv_label_create(ui_temp2);
lv_obj_set_width( ui_temp_data_2, 80);
lv_obj_set_height( ui_temp_data_2, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_temp_data_2, -20 );
lv_obj_set_y( ui_temp_data_2, 0 );
lv_obj_set_align( ui_temp_data_2, LV_ALIGN_CENTER );
lv_label_set_text(ui_temp_data_2,"N/A");
lv_obj_set_style_text_align(ui_temp_data_2, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_temp_data_2, &ui_font_font2, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_temp_unit_2 = lv_label_create(ui_temp2);
lv_obj_set_width( ui_temp_unit_2, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_temp_unit_2, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_temp_unit_2, 35 );
lv_obj_set_y( ui_temp_unit_2, 0 );
lv_obj_set_align( ui_temp_unit_2, LV_ALIGN_CENTER );
lv_label_set_text(ui_temp_unit_2,"°C");
lv_obj_set_style_text_font(ui_temp_unit_2, &lv_font_montserrat_20, LV_PART_MAIN| LV_STATE_DEFAULT);
//lv_obj_set_style_text_font(ui_temp_unit_2, &ui_font_font0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_humidity2 = lv_btn_create(ui_screen_sensor);
lv_obj_set_width( ui_humidity2, 140);
lv_obj_set_height( ui_humidity2, 200);
lv_obj_set_x( ui_humidity2, 148 );
lv_obj_set_y( ui_humidity2, 80 );
lv_obj_set_align( ui_humidity2, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_humidity2, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_humidity2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_humidity2, lv_color_hex(0x4EACE4), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_humidity2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_humidity_icon_2 = lv_img_create(ui_humidity2);
lv_img_set_src(ui_humidity_icon_2, &ui_img_humidity_2_png);
lv_obj_set_width( ui_humidity_icon_2, LV_SIZE_CONTENT);  /// 44
lv_obj_set_height( ui_humidity_icon_2, LV_SIZE_CONTENT);   /// 53
lv_obj_set_x( ui_humidity_icon_2, -45 );
lv_obj_set_y( ui_humidity_icon_2, -70 );
lv_obj_set_align( ui_humidity_icon_2, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_humidity_icon_2, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_humidity_icon_2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_humidity_title_2 = lv_label_create(ui_humidity2);
lv_obj_set_width( ui_humidity_title_2, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_humidity_title_2, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_humidity_title_2, 15 );
lv_obj_set_y( ui_humidity_title_2, -70 );
lv_obj_set_align( ui_humidity_title_2, LV_ALIGN_CENTER );
lv_label_set_text(ui_humidity_title_2,"Humidity");
lv_obj_set_style_text_font(ui_humidity_title_2, &ui_font_font0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_humidity_data_2 = lv_label_create(ui_humidity2);
lv_obj_set_width( ui_humidity_data_2, 80);
lv_obj_set_height( ui_humidity_data_2, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_humidity_data_2, -30 );
lv_obj_set_y( ui_humidity_data_2, 0 );
lv_obj_set_align( ui_humidity_data_2, LV_ALIGN_CENTER );
lv_label_set_text(ui_humidity_data_2,"N/A");
lv_obj_set_style_text_align(ui_humidity_data_2, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_humidity_data_2, &ui_font_font2, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_humidity_unit_2 = lv_label_create(ui_humidity2);
lv_obj_set_width( ui_humidity_unit_2, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_humidity_unit_2, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_humidity_unit_2, 25 );
lv_obj_set_y( ui_humidity_unit_2, 0 );
lv_obj_set_align( ui_humidity_unit_2, LV_ALIGN_CENTER );
lv_label_set_text(ui_humidity_unit_2,"%");
lv_obj_set_style_text_font(ui_humidity_unit_2, &ui_font_font0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_scrolldots2 = lv_obj_create(ui_screen_sensor);
lv_obj_set_width( ui_scrolldots2, 100);
lv_obj_set_height( ui_scrolldots2, 20);
lv_obj_set_x( ui_scrolldots2, 0 );
lv_obj_set_y( ui_scrolldots2, -20 );
lv_obj_set_align( ui_scrolldots2, LV_ALIGN_BOTTOM_MID );
lv_obj_clear_flag( ui_scrolldots2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_scrolldots2, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_scrolldots2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_color(ui_scrolldots2, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_scrolldots2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_scrolldots21 = lv_obj_create(ui_scrolldots2);
lv_obj_set_width( ui_scrolldots21, 7);
lv_obj_set_height( ui_scrolldots21, 7);
lv_obj_set_align( ui_scrolldots21, LV_ALIGN_LEFT_MID );
lv_obj_clear_flag( ui_scrolldots21, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_scrolldots22 = lv_obj_create(ui_scrolldots2);
lv_obj_set_width( ui_scrolldots22, 7);
lv_obj_set_height( ui_scrolldots22, 7);
lv_obj_set_align( ui_scrolldots22, LV_ALIGN_RIGHT_MID );
lv_obj_clear_flag( ui_scrolldots22, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_scrolldots23 = lv_obj_create(ui_scrolldots2);
lv_obj_set_width( ui_scrolldots23, 7);
lv_obj_set_height( ui_scrolldots23, 7);
lv_obj_set_align( ui_scrolldots23, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_scrolldots23, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_scrolldots23, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_scrolldots23, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_color(ui_scrolldots23, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_scrolldots23, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_add_event_cb(ui_wifi__st_button_2, ui_event_wifi__st_button_2, LV_EVENT_ALL, NULL);
lv_obj_add_event_cb(ui_screen_sensor, ui_event_screen_sensor, LV_EVENT_ALL, NULL);

}
void ui_screen_setting_screen_init(void)
{
ui_screen_setting = lv_obj_create(NULL);
lv_obj_clear_flag( ui_screen_setting, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_wifi__st_button_3 = lv_btn_create(ui_screen_setting);
lv_obj_set_width( ui_wifi__st_button_3, 60);
lv_obj_set_height( ui_wifi__st_button_3, 60);
lv_obj_set_x( ui_wifi__st_button_3, -10 );
lv_obj_set_y( ui_wifi__st_button_3, 10 );
lv_obj_set_align( ui_wifi__st_button_3, LV_ALIGN_TOP_RIGHT );
lv_obj_add_flag( ui_wifi__st_button_3, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_wifi__st_button_3, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_wifi__st_button_3, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_wifi__st_button_3, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_grad_color(ui_wifi__st_button_3, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );

ui_wifi_st_3 = lv_img_create(ui_wifi__st_button_3);
lv_img_set_src(ui_wifi_st_3, &ui_img_wifi_disconet_png);
lv_obj_set_width( ui_wifi_st_3, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_wifi_st_3, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_wifi_st_3, LV_ALIGN_TOP_RIGHT );
lv_obj_add_flag( ui_wifi_st_3, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_wifi_st_3, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_time3 = lv_label_create(ui_screen_setting);
lv_obj_set_width( ui_time3, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_time3, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_time3, 30 );
lv_obj_set_y( ui_time3, 20 );
lv_label_set_text(ui_time3,"21:20");
lv_obj_set_style_text_font(ui_time3, &ui_font_font1, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_setting_icon = lv_img_create(ui_screen_setting);
lv_img_set_src(ui_setting_icon, &ui_img_setting_png);
lv_obj_set_width( ui_setting_icon, LV_SIZE_CONTENT);  /// 21
lv_obj_set_height( ui_setting_icon, LV_SIZE_CONTENT);   /// 21
lv_obj_set_x( ui_setting_icon, 28 );
lv_obj_set_y( ui_setting_icon, 86 );
lv_obj_add_flag( ui_setting_icon, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_setting_icon, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_setting_title = lv_label_create(ui_screen_setting);
lv_obj_set_width( ui_setting_title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_setting_title, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_setting_title, 58 );
lv_obj_set_y( ui_setting_title, 86 );
lv_label_set_text(ui_setting_title,"Setting");
lv_obj_set_style_text_font(ui_setting_title, &ui_font_font1, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_setting_wifi = lv_btn_create(ui_screen_setting);
lv_obj_set_width( ui_setting_wifi, 140);
lv_obj_set_height( ui_setting_wifi, 200);
lv_obj_set_x( ui_setting_wifi, -148 );
lv_obj_set_y( ui_setting_wifi, 10 );
lv_obj_set_align( ui_setting_wifi, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_setting_wifi, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_setting_wifi, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_setting_wifi, lv_color_hex(0x4EACE4), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_setting_wifi, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_setting_wifi_icon = lv_img_create(ui_setting_wifi);
lv_img_set_src(ui_setting_wifi_icon, &ui_img_wifi_setting_png);
lv_obj_set_width( ui_setting_wifi_icon, LV_SIZE_CONTENT);  /// 63
lv_obj_set_height( ui_setting_wifi_icon, LV_SIZE_CONTENT);   /// 48
lv_obj_set_x( ui_setting_wifi_icon, 0 );
lv_obj_set_y( ui_setting_wifi_icon, -20 );
lv_obj_set_align( ui_setting_wifi_icon, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_setting_wifi_icon, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_setting_wifi_icon, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_setting_wifi_title = lv_label_create(ui_setting_wifi);
lv_obj_set_width( ui_setting_wifi_title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_setting_wifi_title, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_setting_wifi_title, 0 );
lv_obj_set_y( ui_setting_wifi_title, 25 );
lv_obj_set_align( ui_setting_wifi_title, LV_ALIGN_CENTER );
lv_label_set_text(ui_setting_wifi_title,"WiFi");
lv_obj_set_style_text_font(ui_setting_wifi_title, &ui_font_font1, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_setting_display = lv_btn_create(ui_screen_setting);
lv_obj_set_width( ui_setting_display, 140);
lv_obj_set_height( ui_setting_display, 200);
lv_obj_set_x( ui_setting_display, 0 );
lv_obj_set_y( ui_setting_display, 10 );
lv_obj_set_align( ui_setting_display, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_setting_display, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_setting_display, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_setting_display, lv_color_hex(0xEEBF41), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_setting_display, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_setting_display_icon = lv_img_create(ui_setting_display);
lv_img_set_src(ui_setting_display_icon, &ui_img_display_png);
lv_obj_set_width( ui_setting_display_icon, LV_SIZE_CONTENT);  /// 44
lv_obj_set_height( ui_setting_display_icon, LV_SIZE_CONTENT);   /// 53
lv_obj_set_x( ui_setting_display_icon, 0 );
lv_obj_set_y( ui_setting_display_icon, -20 );
lv_obj_set_align( ui_setting_display_icon, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_setting_display_icon, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_setting_display_icon, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_setting_display_title = lv_label_create(ui_setting_display);
lv_obj_set_width( ui_setting_display_title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_setting_display_title, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_setting_display_title, 0 );
lv_obj_set_y( ui_setting_display_title, 25 );
lv_obj_set_align( ui_setting_display_title, LV_ALIGN_CENTER );
lv_label_set_text(ui_setting_display_title,"Display");
lv_obj_set_style_text_font(ui_setting_display_title, &ui_font_font1, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_setting_time = lv_btn_create(ui_screen_setting);
lv_obj_set_width( ui_setting_time, 140);
lv_obj_set_height( ui_setting_time, 200);
lv_obj_set_x( ui_setting_time, 148 );
lv_obj_set_y( ui_setting_time, 10 );
lv_obj_set_align( ui_setting_time, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_setting_time, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_setting_time, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_setting_time, lv_color_hex(0x529D53), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_setting_time, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_setting_time_icon = lv_img_create(ui_setting_time);
lv_img_set_src(ui_setting_time_icon, &ui_img_time_png);
lv_obj_set_width( ui_setting_time_icon, LV_SIZE_CONTENT);  /// 56
lv_obj_set_height( ui_setting_time_icon, LV_SIZE_CONTENT);   /// 56
lv_obj_set_x( ui_setting_time_icon, 0 );
lv_obj_set_y( ui_setting_time_icon, -20 );
lv_obj_set_align( ui_setting_time_icon, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_setting_time_icon, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_setting_time_icon, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_setting_time_title = lv_label_create(ui_setting_time);
lv_obj_set_width( ui_setting_time_title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_setting_time_title, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_setting_time_title, 0 );
lv_obj_set_y( ui_setting_time_title, 25 );
lv_obj_set_align( ui_setting_time_title, LV_ALIGN_CENTER );
lv_label_set_text(ui_setting_time_title,"Date&Time");
lv_obj_set_style_text_font(ui_setting_time_title, &ui_font_font1, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_scrolldots3 = lv_obj_create(ui_screen_setting);
lv_obj_set_width( ui_scrolldots3, 100);
lv_obj_set_height( ui_scrolldots3, 20);
lv_obj_set_x( ui_scrolldots3, 0 );
lv_obj_set_y( ui_scrolldots3, -20 );
lv_obj_set_align( ui_scrolldots3, LV_ALIGN_BOTTOM_MID );
lv_obj_clear_flag( ui_scrolldots3, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_scrolldots3, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_scrolldots3, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_color(ui_scrolldots3, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_scrolldots3, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_scrolldots31 = lv_obj_create(ui_scrolldots3);
lv_obj_set_width( ui_scrolldots31, 7);
lv_obj_set_height( ui_scrolldots31, 7);
lv_obj_set_align( ui_scrolldots31, LV_ALIGN_LEFT_MID );
lv_obj_clear_flag( ui_scrolldots31, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_scrolldots32 = lv_obj_create(ui_scrolldots3);
lv_obj_set_width( ui_scrolldots32, 7);
lv_obj_set_height( ui_scrolldots32, 7);
lv_obj_set_align( ui_scrolldots32, LV_ALIGN_RIGHT_MID );
lv_obj_clear_flag( ui_scrolldots32, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_scrolldots32, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_scrolldots32, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_color(ui_scrolldots32, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_scrolldots32, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_scrolldots33 = lv_obj_create(ui_scrolldots3);
lv_obj_set_width( ui_scrolldots33, 7);
lv_obj_set_height( ui_scrolldots33, 7);
lv_obj_set_align( ui_scrolldots33, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_scrolldots33, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

lv_obj_add_event_cb(ui_wifi__st_button_3, ui_event_wifi__st_button_3, LV_EVENT_ALL, NULL);
lv_obj_add_event_cb(ui_setting_wifi, ui_event_setting_wifi, LV_EVENT_ALL, NULL);
lv_obj_add_event_cb(ui_setting_display, ui_event_setting_display, LV_EVENT_ALL, NULL);
lv_obj_add_event_cb(ui_setting_time, ui_event_setting_time, LV_EVENT_ALL, NULL);
lv_obj_add_event_cb(ui_screen_setting, ui_event_screen_setting, LV_EVENT_ALL, NULL);

}
void ui_screen_display_screen_init(void)
{
ui_screen_display = lv_obj_create(NULL);
lv_obj_clear_flag( ui_screen_display, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_wifi_st_4 = lv_img_create(ui_screen_display);
lv_img_set_src(ui_wifi_st_4, &ui_img_wifi_disconet_png);
lv_obj_set_width( ui_wifi_st_4, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_wifi_st_4, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_wifi_st_4, -20 );
lv_obj_set_y( ui_wifi_st_4, 20 );
lv_obj_set_align( ui_wifi_st_4, LV_ALIGN_TOP_RIGHT );
lv_obj_add_flag( ui_wifi_st_4, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_wifi_st_4, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_back1 = lv_btn_create(ui_screen_display);
lv_obj_set_width( ui_back1, 100);
lv_obj_set_height( ui_back1, 50);
lv_obj_set_x( ui_back1, 10 );
lv_obj_set_y( ui_back1, 30 );
lv_obj_add_flag( ui_back1, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_back1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_back1, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_back1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_img_src( ui_back1, &ui_img_back_png, LV_PART_MAIN | LV_STATE_DEFAULT );

ui_display_title = lv_label_create(ui_screen_display);
lv_obj_set_width( ui_display_title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_display_title, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_display_title, 1 );
lv_obj_set_y( ui_display_title, 50 );
lv_obj_set_align( ui_display_title, LV_ALIGN_TOP_MID );
lv_label_set_text(ui_display_title,"Display");
lv_obj_set_style_text_font(ui_display_title, &ui_font_font1, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_brighness = lv_obj_create(ui_screen_display);
lv_obj_set_width( ui_brighness, 400);
lv_obj_set_height( ui_brighness, 100);
lv_obj_set_x( ui_brighness, 0 );
lv_obj_set_y( ui_brighness, -80 );
lv_obj_set_align( ui_brighness, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_brighness, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_brighness_cfg = lv_slider_create(ui_brighness);
lv_obj_set_width( ui_brighness_cfg, 250);
lv_obj_set_height( ui_brighness_cfg, 15);
lv_obj_set_x( ui_brighness_cfg, 0 );
lv_obj_set_y( ui_brighness_cfg, 10 );
lv_obj_set_align( ui_brighness_cfg, LV_ALIGN_CENTER );
lv_obj_set_style_bg_color(ui_brighness_cfg, lv_color_hex(0x363636), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_brighness_cfg, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_set_style_bg_color(ui_brighness_cfg, lv_color_hex(0x529D53), LV_PART_INDICATOR | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_brighness_cfg, 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);

lv_obj_set_style_bg_color(ui_brighness_cfg, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_brighness_cfg, 255, LV_PART_KNOB| LV_STATE_DEFAULT);

ui_brighness_title = lv_label_create(ui_brighness);
lv_obj_set_width( ui_brighness_title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_brighness_title, LV_SIZE_CONTENT);   /// 1
lv_label_set_text(ui_brighness_title,"Brightness");

ui_brighness_icon_1 = lv_img_create(ui_brighness);
lv_img_set_src(ui_brighness_icon_1, &ui_img_high_light_png);
lv_obj_set_width( ui_brighness_icon_1, LV_SIZE_CONTENT);  /// 22
lv_obj_set_height( ui_brighness_icon_1, LV_SIZE_CONTENT);   /// 22
lv_obj_set_x( ui_brighness_icon_1, 0 );
lv_obj_set_y( ui_brighness_icon_1, 10 );
lv_obj_set_align( ui_brighness_icon_1, LV_ALIGN_RIGHT_MID );
lv_obj_add_flag( ui_brighness_icon_1, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_brighness_icon_1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_brighness_icon_2 = lv_img_create(ui_brighness);
lv_img_set_src(ui_brighness_icon_2, &ui_img_low_light_png);
lv_obj_set_width( ui_brighness_icon_2, LV_SIZE_CONTENT);  /// 18
lv_obj_set_height( ui_brighness_icon_2, LV_SIZE_CONTENT);   /// 18
lv_obj_set_x( ui_brighness_icon_2, 0 );
lv_obj_set_y( ui_brighness_icon_2, 10 );
lv_obj_set_align( ui_brighness_icon_2, LV_ALIGN_LEFT_MID );
lv_obj_add_flag( ui_brighness_icon_2, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_brighness_icon_2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_screen_always_on = lv_obj_create(ui_screen_display);
lv_obj_set_width( ui_screen_always_on, 400);
lv_obj_set_height( ui_screen_always_on, 50);
lv_obj_set_x( ui_screen_always_on, 0 );
lv_obj_set_y( ui_screen_always_on, 5 );
lv_obj_set_align( ui_screen_always_on, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_screen_always_on, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_screen_always_on_title = lv_label_create(ui_screen_always_on);
lv_obj_set_width( ui_screen_always_on_title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_screen_always_on_title, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_screen_always_on_title, LV_ALIGN_LEFT_MID );
lv_label_set_text(ui_screen_always_on_title,"Always-on");

ui_screen_always_on_cfg = lv_switch_create(ui_screen_always_on);
lv_obj_set_width( ui_screen_always_on_cfg, 50);
lv_obj_set_height( ui_screen_always_on_cfg, 25);
lv_obj_set_align( ui_screen_always_on_cfg, LV_ALIGN_RIGHT_MID );
lv_obj_set_style_bg_color(ui_screen_always_on_cfg, lv_color_hex(0x363636), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_screen_always_on_cfg, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_screen_always_on_cfg, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED|LV_STATE_PRESSED );
lv_obj_set_style_bg_opa(ui_screen_always_on_cfg, 255, LV_PART_MAIN| LV_STATE_CHECKED|LV_STATE_PRESSED);

lv_obj_set_style_bg_color(ui_screen_always_on_cfg, lv_color_hex(0x529D53), LV_PART_INDICATOR | LV_STATE_CHECKED );
lv_obj_set_style_bg_opa(ui_screen_always_on_cfg, 255, LV_PART_INDICATOR| LV_STATE_CHECKED);

ui_turn_off_after_time = lv_obj_create(ui_screen_display);
lv_obj_set_width( ui_turn_off_after_time, 400);
lv_obj_set_height( ui_turn_off_after_time, 50);
lv_obj_set_x( ui_turn_off_after_time, 0 );
lv_obj_set_y( ui_turn_off_after_time, 65 );
lv_obj_set_align( ui_turn_off_after_time, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_turn_off_after_time, LV_OBJ_FLAG_HIDDEN );   /// Flags
lv_obj_clear_flag( ui_turn_off_after_time, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_after = lv_label_create(ui_turn_off_after_time);
lv_obj_set_width( ui_after, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_after, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_after, LV_ALIGN_LEFT_MID );
lv_label_set_text(ui_after,"Turn Off Screen After");

ui_turn_off_after_time_cfg = lv_textarea_create(ui_turn_off_after_time);
lv_obj_set_width( ui_turn_off_after_time_cfg, 60);
lv_obj_set_height( ui_turn_off_after_time_cfg, 40);
lv_obj_set_x( ui_turn_off_after_time_cfg, -50 );
lv_obj_set_y( ui_turn_off_after_time_cfg, 0 );
lv_obj_set_align( ui_turn_off_after_time_cfg, LV_ALIGN_RIGHT_MID );
lv_textarea_set_placeholder_text(ui_turn_off_after_time_cfg,"1");
lv_obj_set_style_bg_color(ui_turn_off_after_time_cfg, lv_color_hex(0x6F6F6F), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_turn_off_after_time_cfg, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_min = lv_label_create(ui_turn_off_after_time);
lv_obj_set_width( ui_min, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_min, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_min, LV_ALIGN_RIGHT_MID );
lv_label_set_text(ui_min,"min");

ui_display_keyboard = lv_keyboard_create(ui_screen_display);
lv_keyboard_set_mode(ui_display_keyboard,LV_KEYBOARD_MODE_NUMBER);
lv_obj_set_width( ui_display_keyboard, 200);
lv_obj_set_height( ui_display_keyboard, 120);
lv_obj_set_x( ui_display_keyboard, 8 );
lv_obj_set_y( ui_display_keyboard, 150 );
lv_obj_set_align( ui_display_keyboard, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_display_keyboard, LV_OBJ_FLAG_HIDDEN );   /// Flags

lv_obj_add_event_cb(ui_back1, ui_event_back1, LV_EVENT_ALL, NULL);
lv_obj_add_event_cb(ui_screen_always_on_cfg, ui_event_screen_always_on_cfg, LV_EVENT_ALL, NULL);
lv_obj_add_event_cb(ui_turn_off_after_time_cfg, ui_event_sleep_mode_time_cfg, LV_EVENT_ALL, NULL);
lv_keyboard_set_textarea(ui_display_keyboard,ui_turn_off_after_time_cfg);
lv_obj_add_event_cb(ui_display_keyboard, ui_event_display_keyboard, LV_EVENT_ALL, NULL);

}
void ui_screen_date_time_screen_init(void)
{
ui_screen_date_time = lv_obj_create(NULL);
lv_obj_clear_flag( ui_screen_date_time, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_wifi_st_5 = lv_img_create(ui_screen_date_time);
lv_img_set_src(ui_wifi_st_5, &ui_img_wifi_disconet_png);
lv_obj_set_width( ui_wifi_st_5, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_wifi_st_5, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_wifi_st_5, -20 );
lv_obj_set_y( ui_wifi_st_5, 20 );
lv_obj_set_align( ui_wifi_st_5, LV_ALIGN_TOP_RIGHT );
lv_obj_add_flag( ui_wifi_st_5, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_wifi_st_5, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_back2 = lv_btn_create(ui_screen_date_time);
lv_obj_set_width( ui_back2, 100);
lv_obj_set_height( ui_back2, 50);
lv_obj_set_x( ui_back2, 10 );
lv_obj_set_y( ui_back2, 30 );
lv_obj_add_flag( ui_back2, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_back2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_back2, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_back2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_img_src( ui_back2, &ui_img_back_png, LV_PART_MAIN | LV_STATE_DEFAULT );

ui_date_time_title = lv_label_create(ui_screen_date_time);
lv_obj_set_width( ui_date_time_title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_date_time_title, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_date_time_title, 0 );
lv_obj_set_y( ui_date_time_title, 50 );
lv_obj_set_align( ui_date_time_title, LV_ALIGN_TOP_MID );
lv_label_set_text(ui_date_time_title,"Date & Time");
lv_obj_set_style_text_font(ui_date_time_title, &ui_font_font1, LV_PART_MAIN| LV_STATE_DEFAULT);


lv_obj_t * config_list = lv_list_create(ui_screen_date_time);
lv_obj_clear_flag( config_list, LV_OBJ_FLAG_SCROLLABLE );

lv_obj_set_style_pad_row(config_list, 8, 0);

lv_obj_set_align( config_list, LV_ALIGN_CENTER );
lv_obj_set_width( config_list, 420);
lv_obj_set_height( config_list, 350);
lv_obj_set_x( config_list, 0 );
lv_obj_set_y( config_list, 30 );

lv_obj_set_style_bg_color(config_list, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_grad_color(config_list, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_color(config_list, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );


ui_time_format = lv_obj_create(config_list);
lv_obj_set_width( ui_time_format, 400);
lv_obj_set_height( ui_time_format, 50);
lv_obj_set_align( ui_time_format, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_time_format, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_time_format_title = lv_label_create(ui_time_format);
lv_obj_set_width( ui_time_format_title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_time_format_title, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_time_format_title, LV_ALIGN_LEFT_MID );
lv_label_set_text(ui_time_format_title,"Time Format");

ui_time_format_cfg = lv_dropdown_create(ui_time_format);
lv_dropdown_set_options( ui_time_format_cfg, "24H\n12H" );
lv_obj_set_width( ui_time_format_cfg, 100);
lv_obj_set_height( ui_time_format_cfg, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_time_format_cfg, LV_ALIGN_RIGHT_MID );
lv_obj_add_flag( ui_time_format_cfg, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_set_style_bg_color(ui_time_format_cfg, lv_color_hex(0x6F6F6F), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_time_format_cfg, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

//--------------------------- time  ui_auto_update-------------------

ui_auto_update = lv_obj_create(config_list);
lv_obj_set_width( ui_auto_update, 400);
lv_obj_set_height( ui_auto_update, 50);
//lv_obj_set_x( ui_auto_update, 0 );
//lv_obj_set_y( ui_auto_update, -60 );
lv_obj_set_align( ui_auto_update, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_auto_update, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_auto_update_title = lv_label_create(ui_auto_update);
lv_obj_set_width( ui_auto_update_title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_auto_update_title, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_auto_update_title, LV_ALIGN_LEFT_MID );
lv_label_set_text(ui_auto_update_title,"Time Auto Update");

ui_auto_update_cfg = lv_switch_create(ui_auto_update);
lv_obj_set_width( ui_auto_update_cfg, 50);
lv_obj_set_height( ui_auto_update_cfg, 25);
lv_obj_set_align( ui_auto_update_cfg, LV_ALIGN_RIGHT_MID );
lv_obj_add_state( ui_auto_update_cfg, LV_STATE_CHECKED );     /// States
lv_obj_set_style_bg_color(ui_auto_update_cfg, lv_color_hex(0x363636), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_auto_update_cfg, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_auto_update_cfg, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED|LV_STATE_PRESSED );
lv_obj_set_style_bg_opa(ui_auto_update_cfg, 255, LV_PART_MAIN| LV_STATE_CHECKED|LV_STATE_PRESSED);

lv_obj_set_style_bg_color(ui_auto_update_cfg, lv_color_hex(0x529D53), LV_PART_INDICATOR | LV_STATE_CHECKED );
lv_obj_set_style_bg_opa(ui_auto_update_cfg, 255, LV_PART_INDICATOR| LV_STATE_CHECKED);




//--------------------------- time  -------------------
ui_date_time = lv_obj_create(config_list);
lv_obj_set_width( ui_date_time, 400);
lv_obj_set_height( ui_date_time, 100);
//lv_obj_set_x( ui_time_zone, 0 );
//lv_obj_set_y( ui_time_zone, 88 );
lv_obj_set_align( ui_date_time, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_date_time, LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_SCROLL_ONE );   /// Flags
lv_obj_clear_flag( ui_date_time, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_date_cfg = lv_textarea_create(ui_date_time);
lv_obj_set_width( ui_date_cfg, 120);
lv_obj_set_height( ui_date_cfg, LV_SIZE_CONTENT);   /// 40
lv_obj_set_x( ui_date_cfg, -5);
//lv_obj_set_y( ui_date_cfg, 60 );
lv_obj_set_align( ui_date_cfg, LV_ALIGN_LEFT_MID );
lv_textarea_set_max_length(ui_date_cfg,12);
lv_textarea_set_placeholder_text(ui_date_cfg,"01/01/2023");
lv_textarea_set_one_line(ui_date_cfg,true);
lv_obj_set_scrollbar_mode(ui_date_cfg, LV_SCROLLBAR_MODE_OFF);
lv_obj_set_style_text_align(ui_date_cfg, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_date_cfg, &ui_font_font0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_date_cfg, lv_color_hex(0x6F6F6F), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_date_cfg, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

int x_offset = 70;
ui_hour_cfg = lv_roller_create(ui_date_time);
lv_roller_set_options( ui_hour_cfg, "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23", LV_ROLLER_MODE_NORMAL );
lv_obj_set_height( ui_hour_cfg, 100);
lv_obj_set_width( ui_hour_cfg, LV_SIZE_CONTENT);  /// 1
lv_obj_set_x( ui_hour_cfg, 40 - x_offset );
//lv_obj_set_y( ui_hour_cfg, 60 );
lv_obj_set_align( ui_hour_cfg, LV_ALIGN_CENTER );

// lv_obj_set_style_bg_color(ui_hour_cfg, lv_color_hex(0x4EACE4), LV_PART_SELECTED | LV_STATE_DEFAULT );
// lv_obj_set_style_bg_opa(ui_hour_cfg, 255, LV_PART_SELECTED| LV_STATE_DEFAULT);

ui_min_cfg = lv_roller_create(ui_date_time);
lv_roller_set_options( ui_min_cfg, "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59", LV_ROLLER_MODE_NORMAL );
lv_obj_set_height( ui_min_cfg, 100);
lv_obj_set_width( ui_min_cfg, LV_SIZE_CONTENT);  /// 1
lv_obj_set_x( ui_min_cfg, 100 - x_offset);
//lv_obj_set_y( ui_min_cfg, 60 );
lv_obj_set_align( ui_min_cfg, LV_ALIGN_CENTER );

// lv_obj_set_style_bg_color(ui_min_cfg, lv_color_hex(0x4EACE4), LV_PART_SELECTED | LV_STATE_DEFAULT );
// lv_obj_set_style_bg_opa(ui_min_cfg, 255, LV_PART_SELECTED| LV_STATE_DEFAULT);

ui_sec_cfg = lv_roller_create(ui_date_time);
lv_roller_set_options( ui_sec_cfg, "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59", LV_ROLLER_MODE_NORMAL );
lv_obj_set_height( ui_sec_cfg, 100);
lv_obj_set_width( ui_sec_cfg, LV_SIZE_CONTENT);  /// 1
lv_obj_set_x( ui_sec_cfg, 160 - x_offset);
//lv_obj_set_y( ui_sec_cfg, 60 );
lv_obj_set_align( ui_sec_cfg, LV_ALIGN_CENTER );

// lv_obj_set_style_bg_color(ui_sec_cfg, lv_color_hex(0x4EACE4), LV_PART_SELECTED | LV_STATE_DEFAULT );
// lv_obj_set_style_bg_opa(ui_sec_cfg, 255, LV_PART_SELECTED| LV_STATE_DEFAULT);

ui_time_label1 = lv_label_create(ui_date_time);
lv_obj_set_width( ui_time_label1, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_time_label1, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_time_label1, 70 - x_offset);
//lv_obj_set_y( ui_time_label1, 60 );
lv_obj_set_align( ui_time_label1, LV_ALIGN_CENTER );
lv_label_set_text(ui_time_label1,":");

ui_time_label2 = lv_label_create(ui_date_time);
lv_obj_set_width( ui_time_label2, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_time_label2, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_time_label2, 130 - x_offset);
//lv_obj_set_y( ui_time_label2, 60 );
lv_obj_set_align( ui_time_label2, LV_ALIGN_CENTER );
lv_label_set_text(ui_time_label2,":");

ui_time_save = lv_btn_create( ui_date_time);
lv_obj_set_x( ui_time_save, 230 - x_offset);
lv_obj_set_align(ui_time_save , LV_ALIGN_CENTER );
lv_obj_set_width( ui_time_save, 50);
lv_obj_set_height( ui_time_save, 40);
lv_obj_clear_flag(ui_time_save, LV_OBJ_FLAG_CLICKABLE);
lv_obj_set_style_bg_color(ui_time_save, lv_color_hex(0x6F6F6F), LV_PART_MAIN | LV_STATE_DEFAULT );

lv_obj_t *  ui_time_save_label = lv_label_create(ui_time_save);
lv_obj_set_align( ui_time_save_label, LV_ALIGN_CENTER );
lv_label_set_text(ui_time_save_label,"Save");

//--------------------------- zone  ui_auto_update-------------------

lv_obj_t *ui_zone_auto_update = lv_obj_create(config_list);
lv_obj_set_width( ui_zone_auto_update, 400);
lv_obj_set_height( ui_zone_auto_update, 50);
//lv_obj_set_x( ui_auto_update, 0 );
//lv_obj_set_y( ui_auto_update, -60 );
lv_obj_set_align( ui_zone_auto_update, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_zone_auto_update, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_add_flag( ui_zone_auto_update, LV_OBJ_FLAG_HIDDEN );

lv_obj_t * ui_zone_auto_update_title = lv_label_create(ui_zone_auto_update);
lv_obj_set_width( ui_zone_auto_update_title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_zone_auto_update_title, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_zone_auto_update_title, LV_ALIGN_LEFT_MID );
lv_label_set_text(ui_zone_auto_update_title,"Zone Auto Update");

ui_zone_auto_update_cfg = lv_switch_create(ui_zone_auto_update);
lv_obj_set_width( ui_zone_auto_update_cfg, 50);
lv_obj_set_height( ui_zone_auto_update_cfg, 25);
lv_obj_set_align( ui_zone_auto_update_cfg, LV_ALIGN_RIGHT_MID );
lv_obj_add_state( ui_zone_auto_update_cfg, LV_STATE_CHECKED );     /// States
lv_obj_set_style_bg_color(ui_zone_auto_update_cfg, lv_color_hex(0x363636), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_zone_auto_update_cfg, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_zone_auto_update_cfg, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED|LV_STATE_PRESSED );
lv_obj_set_style_bg_opa(ui_zone_auto_update_cfg, 255, LV_PART_MAIN| LV_STATE_CHECKED|LV_STATE_PRESSED);

lv_obj_set_style_bg_color(ui_zone_auto_update_cfg, lv_color_hex(0x529D53), LV_PART_INDICATOR | LV_STATE_CHECKED );
lv_obj_set_style_bg_opa(ui_zone_auto_update_cfg, 255, LV_PART_INDICATOR| LV_STATE_CHECKED);

//--------------------------- zone -------------------

ui_time_zone = lv_obj_create(config_list);
lv_obj_set_width( ui_time_zone, 400);
lv_obj_set_height( ui_time_zone, 100);
//lv_obj_set_x( ui_time_zone, 0 );
//lv_obj_set_y( ui_time_zone, 88 );
lv_obj_set_align( ui_time_zone, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_time_zone, LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_SCROLL_ONE );   /// Flags
lv_obj_clear_flag( ui_time_zone, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_time_zone_title = lv_label_create(ui_time_zone);
lv_obj_set_width( ui_time_zone_title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_time_zone_title, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_time_zone_title, 0 );
lv_obj_set_y( ui_time_zone_title, -20 );
lv_obj_set_align( ui_time_zone_title, LV_ALIGN_LEFT_MID );
lv_label_set_text(ui_time_zone_title,"Time Zone");

ui_time_zone_num_cfg = lv_dropdown_create(ui_time_zone);
lv_dropdown_set_options( ui_time_zone_num_cfg, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12" );
lv_obj_set_width( ui_time_zone_num_cfg, 69);
lv_obj_set_height( ui_time_zone_num_cfg, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_time_zone_num_cfg, 150 );
lv_obj_set_y( ui_time_zone_num_cfg, -20 );
lv_obj_set_align( ui_time_zone_num_cfg, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_time_zone_num_cfg, LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_set_style_bg_color(ui_time_zone_num_cfg, lv_color_hex(0x6F6F6F), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_time_zone_num_cfg, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_utc_tile = lv_label_create(ui_time_zone);
lv_obj_set_width( ui_utc_tile, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_utc_tile, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_utc_tile, 30 );
lv_obj_set_y( ui_utc_tile, -20 );
lv_obj_set_align( ui_utc_tile, LV_ALIGN_CENTER );
lv_label_set_text(ui_utc_tile,"UTC");

ui_time_zone_sign_cfg_ = lv_dropdown_create(ui_time_zone);
lv_dropdown_set_options( ui_time_zone_sign_cfg_, "+\n-" );
lv_obj_set_width( ui_time_zone_sign_cfg_, 50);
lv_obj_set_height( ui_time_zone_sign_cfg_, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_time_zone_sign_cfg_, 84 );
lv_obj_set_y( ui_time_zone_sign_cfg_, -20 );
lv_obj_set_align( ui_time_zone_sign_cfg_, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_time_zone_sign_cfg_, LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_set_style_bg_color(ui_time_zone_sign_cfg_, lv_color_hex(0x6F6F6F), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_time_zone_sign_cfg_, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_daylight_title = lv_label_create(ui_time_zone);
lv_obj_set_width( ui_daylight_title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_daylight_title, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_daylight_title, 0 );
lv_obj_set_y( ui_daylight_title, 20 );
lv_obj_set_align( ui_daylight_title, LV_ALIGN_LEFT_MID );
lv_label_set_text(ui_daylight_title,"Daylight Saving Time");

ui_daylight_cfg = lv_switch_create(ui_time_zone);
lv_obj_set_width( ui_daylight_cfg, 50);
lv_obj_set_height( ui_daylight_cfg, 25);
lv_obj_set_x( ui_daylight_cfg, 0 );
lv_obj_set_y( ui_daylight_cfg, 20 );
lv_obj_set_align( ui_daylight_cfg, LV_ALIGN_RIGHT_MID );
lv_obj_add_state( ui_daylight_cfg, LV_STATE_CHECKED );     /// States
lv_obj_set_style_bg_color(ui_daylight_cfg, lv_color_hex(0x363636), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_daylight_cfg, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_daylight_cfg, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED|LV_STATE_PRESSED );
lv_obj_set_style_bg_opa(ui_daylight_cfg, 255, LV_PART_MAIN| LV_STATE_CHECKED|LV_STATE_PRESSED);

lv_obj_set_style_bg_color(ui_daylight_cfg, lv_color_hex(0x529D53), LV_PART_INDICATOR | LV_STATE_CHECKED );
lv_obj_set_style_bg_opa(ui_daylight_cfg, 255, LV_PART_INDICATOR| LV_STATE_CHECKED);

lv_obj_add_event_cb(ui_back2, ui_event_back2, LV_EVENT_ALL, NULL);

}
void ui_screen_wifi_screen_init(void)
{
ui_screen_wifi = lv_obj_create(NULL);
lv_obj_clear_flag( ui_screen_wifi, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_wifi_st_6 = lv_img_create(ui_screen_wifi);
lv_img_set_src(ui_wifi_st_6, &ui_img_wifi_disconet_png);
lv_obj_set_width( ui_wifi_st_6, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_wifi_st_6, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_wifi_st_6, -20 );
lv_obj_set_y( ui_wifi_st_6, 20 );
lv_obj_set_align( ui_wifi_st_6, LV_ALIGN_TOP_RIGHT );
lv_obj_add_flag( ui_wifi_st_6, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_wifi_st_6, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_wifi_title = lv_label_create(ui_screen_wifi);
lv_obj_set_width( ui_wifi_title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_wifi_title, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_wifi_title, 1 );
lv_obj_set_y( ui_wifi_title, 50 );
lv_obj_set_align( ui_wifi_title, LV_ALIGN_TOP_MID );
lv_label_set_text(ui_wifi_title,"WiFi");
lv_obj_set_style_text_font(ui_wifi_title, &ui_font_font1, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_back3 = lv_btn_create(ui_screen_wifi);
lv_obj_set_width( ui_back3, 100);
lv_obj_set_height( ui_back3, 50);
lv_obj_set_x( ui_back3, 10 );
lv_obj_set_y( ui_back3, 30 );
lv_obj_add_flag( ui_back3, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_back3, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_back3, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_back3, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_img_src( ui_back3, &ui_img_back_png, LV_PART_MAIN | LV_STATE_DEFAULT );

lv_obj_add_event_cb(ui_back3, ui_event_back3, LV_EVENT_ALL, NULL);

}

void ui_screen_factory_screen_init()
{
ui_screen_factory = lv_obj_create(NULL);
lv_obj_clear_flag( ui_screen_factory, LV_OBJ_FLAG_SCROLLABLE );

ui_factory_resetting_title = lv_label_create(ui_screen_factory);
lv_obj_set_width( ui_factory_resetting_title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_factory_resetting_title, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_factory_resetting_title, LV_ALIGN_CENTER );
lv_label_set_text(ui_factory_resetting_title,"Factory Resetting...");
lv_obj_set_style_text_font(ui_factory_resetting_title, &ui_font_font2, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_color(ui_factory_resetting_title, lv_color_hex(0xCD3700), LV_PART_MAIN | LV_STATE_DEFAULT );
}


void ui_screen_sensor_chart_screen_init()
{
	ui_screen_sensor_chart = lv_obj_create(NULL);
	//lv_obj_clear_flag( ui_screen_sensor_chart, LV_OBJ_FLAG_SCROLLABLE );

	ui_wifi_st_7 = lv_img_create(ui_screen_sensor_chart);
	lv_img_set_src(ui_wifi_st_7, &ui_img_wifi_disconet_png);
	lv_obj_set_width( ui_wifi_st_7, LV_SIZE_CONTENT);  /// 1
	lv_obj_set_height( ui_wifi_st_7, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_x( ui_wifi_st_7, -20 );
	lv_obj_set_y( ui_wifi_st_7, 20 );
	lv_obj_set_align( ui_wifi_st_7, LV_ALIGN_TOP_RIGHT );
	lv_obj_add_flag( ui_wifi_st_7, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
	lv_obj_clear_flag( ui_wifi_st_7, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

	ui_back4 = lv_btn_create(ui_screen_sensor_chart);
	lv_obj_set_width( ui_back4, 100);
	lv_obj_set_height( ui_back4, 50);
	lv_obj_set_x( ui_back4, 10 );
	lv_obj_set_y( ui_back4, 30 );
	lv_obj_add_flag( ui_back4, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
	lv_obj_clear_flag( ui_back4, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
	lv_obj_set_style_bg_color(ui_back4, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_opa(ui_back4, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_bg_img_src( ui_back4, &ui_img_back_png, LV_PART_MAIN | LV_STATE_DEFAULT );

      lv_obj_add_event_cb(ui_back4, ui_event_back4, LV_EVENT_ALL, NULL);

	ui_sensor_data_title = lv_label_create(ui_screen_sensor_chart);
	lv_obj_set_width( ui_sensor_data_title, LV_SIZE_CONTENT);  /// 1
	lv_obj_set_height( ui_sensor_data_title, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_x( ui_sensor_data_title, 0 );
	lv_obj_set_y( ui_sensor_data_title, 50 );
	lv_obj_set_align( ui_sensor_data_title, LV_ALIGN_TOP_MID );
	lv_label_set_text(ui_sensor_data_title,"Temp"); //modify
	lv_obj_set_style_text_font(ui_sensor_data_title, &ui_font_font1, LV_PART_MAIN| LV_STATE_DEFAULT);


	lv_obj_t * sensor_chat_panel = lv_obj_create(ui_screen_sensor_chart);
	lv_obj_set_align( sensor_chat_panel, LV_ALIGN_TOP_MID );
	lv_obj_set_width( sensor_chat_panel, 480);
	lv_obj_set_height( sensor_chat_panel, 400);
	lv_obj_set_x( sensor_chat_panel, 0 );
	lv_obj_set_y( sensor_chat_panel, 75 );

	lv_obj_set_style_bg_color( sensor_chat_panel, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_grad_color( sensor_chat_panel, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_border_color( sensor_chat_panel, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );


	lv_obj_t * tabview = lv_tabview_create( sensor_chat_panel, LV_DIR_TOP, 40);
    lv_obj_t * ui_tab_day = lv_tabview_add_tab(tabview, "Day");
    lv_obj_t * ui_tab_week = lv_tabview_add_tab(tabview, "Week");

	lv_obj_set_style_bg_color(tabview, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_grad_color(tabview, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_border_color(tabview, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );


	lv_obj_set_style_bg_color(ui_tab_day, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_grad_color(ui_tab_day, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_border_color(ui_tab_day, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );

	lv_obj_set_style_bg_color(ui_tab_week, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_grad_color(ui_tab_week, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_border_color(ui_tab_week, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );


    // day chart
    ui_sensor_chart_day = lv_chart_create( ui_tab_day );

	lv_obj_set_style_bg_color(ui_sensor_chart_day, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_grad_color(ui_sensor_chart_day, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_border_color(ui_sensor_chart_day, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );

    lv_obj_refresh_ext_draw_size(ui_sensor_chart_day);
    lv_chart_set_zoom_x(ui_sensor_chart_day, 800);


    //lv_chart_set_axis_tick(ui_sensor_chart_day, LV_CHART_AXIS_PRIMARY_Y, 0, 0, 5, 1, true, 80);
    lv_chart_set_axis_tick(ui_sensor_chart_day, LV_CHART_AXIS_PRIMARY_X, 0, 0, 24, 1, true, 50);
    //lv_chart_set_range(ui_sensor_chart_day, LV_CHART_AXIS_PRIMARY_X, 0, 200);
    lv_chart_set_range(ui_sensor_chart_day, LV_CHART_AXIS_PRIMARY_Y, -200, 600); //modify

    lv_chart_set_div_line_count(ui_sensor_chart_day, 0, 24);
    lv_chart_set_point_count(ui_sensor_chart_day, 24);


    lv_obj_set_style_border_side(ui_sensor_chart_day, LV_BORDER_SIDE_RIGHT | LV_BORDER_SIDE_BOTTOM, 0);


    ui_sensor_chart_day_series = lv_chart_add_series(ui_sensor_chart_day, lv_palette_main(LV_PALETTE_YELLOW), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_series_color(ui_sensor_chart_day, ui_sensor_chart_day_series, lv_palette_main(LV_PALETTE_GREEN)); //modify


    // week chart
    ui_sensor_chart_week = lv_chart_create( ui_tab_week );

	lv_obj_set_style_bg_color(ui_sensor_chart_week, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_grad_color(ui_sensor_chart_week, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_border_color(ui_sensor_chart_week, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );


    lv_obj_refresh_ext_draw_size(ui_sensor_chart_week);
    lv_chart_set_zoom_x(ui_sensor_chart_week, 256);


    //lv_chart_set_axis_tick(ui_sensor_chart_week, LV_CHART_AXIS_PRIMARY_Y, 0, 0, 5, 1, true, 80);
    lv_chart_set_axis_tick(ui_sensor_chart_week, LV_CHART_AXIS_PRIMARY_X, 0, 0, 7, 1, true, 50);
    //lv_chart_set_range(ui_sensor_chart_week, LV_CHART_AXIS_PRIMARY_X, 0, 200);
    lv_chart_set_range(ui_sensor_chart_week, LV_CHART_AXIS_PRIMARY_Y, -200, 600);

    lv_chart_set_div_line_count(ui_sensor_chart_week, 0, 7);
    lv_chart_set_point_count(ui_sensor_chart_week, 7);



      ui_sensor_chart_week_series_hight = lv_chart_add_series(ui_sensor_chart_week, lv_palette_main(LV_PALETTE_YELLOW), LV_CHART_AXIS_PRIMARY_Y);
      ui_sensor_chart_week_series_low = lv_chart_add_series(ui_sensor_chart_week, lv_palette_main(LV_PALETTE_YELLOW), LV_CHART_AXIS_PRIMARY_Y);

}

void ui_init( void )
{
lv_disp_t *dispp = lv_disp_get_default();
lv_theme_t *theme = lv_theme_default_init(dispp, lv_color_hex(0x529d53), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
lv_disp_set_theme(dispp, theme);
ui_screen_time_screen_init();
ui_screen_sensor_screen_init();
ui_screen_setting_screen_init();
ui_screen_display_screen_init();
ui_screen_date_time_screen_init();
ui_screen_wifi_screen_init();
ui_screen_factory_screen_init();
ui_screen_sensor_chart_screen_init();
lv_disp_load_scr( ui_screen_time);
}
