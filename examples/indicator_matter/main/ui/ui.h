// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.3.3
// PROJECT: sensecap

#ifndef _SENSECAP_UI_H
#define _SENSECAP_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

void up_Animation( lv_obj_t *TargetObject, int delay);
void ui_event_screen_time( lv_event_t * e);
extern lv_obj_t *ui_screen_time;
extern lv_obj_t *ui_background;
extern lv_obj_t *ui_qrcode_background;
extern lv_obj_t *ui_hour;
extern lv_obj_t *ui_hour_dis;
extern lv_obj_t *ui_min;
extern lv_obj_t *ui_min_dis;
extern lv_obj_t *ui_colon;
extern lv_obj_t *ui_adorn;
extern lv_obj_t *ui_wifi_st_1;
extern lv_obj_t *ui_date_panel;
extern lv_obj_t *ui_date;
extern lv_obj_t *ui_location;
extern lv_obj_t *ui_location_Icon;
extern lv_obj_t *ui_city;
void ui_event_screen_sensor( lv_event_t * e);
extern lv_obj_t *ui_screen_sensor;
extern lv_obj_t *ui_wifi__st_button_2;
extern lv_obj_t *ui_wifi_st_2;
extern lv_obj_t *ui_time2;
extern lv_obj_t *ui_co2;
extern lv_obj_t *ui_co2_icon;
extern lv_obj_t *ui_co2_title;
extern lv_obj_t *ui_co2_data;
extern lv_obj_t *ui_co2_unit;
extern lv_obj_t *ui_tvoc_2;
extern lv_obj_t *ui_tvoc_icon_2;
extern lv_obj_t *ui_tvoc_title_2;
extern lv_obj_t *ui_tvoc_data;
extern lv_obj_t *ui_tvoc_unit_2;
extern lv_obj_t *ui_temp2;
extern lv_obj_t *ui_temp_icon_2;
extern lv_obj_t *ui_temp_title_2;
extern lv_obj_t *ui_temp_data_2;
extern lv_obj_t *ui_temp_unit_2;
extern lv_obj_t *ui_humidity2;
extern lv_obj_t *ui_humidity_icon_2;
extern lv_obj_t *ui_humidity_title_2;
extern lv_obj_t *ui_humidity_data_2;
extern lv_obj_t *ui_humidity_unit_2;
extern lv_obj_t *ui_scrolldots2;
extern lv_obj_t *ui_scrolldots21;
extern lv_obj_t *ui_scrolldots22;
extern lv_obj_t *ui_scrolldots23;
void ui_event_screen_setting( lv_event_t * e);
extern lv_obj_t *ui_screen_setting;
extern lv_obj_t *ui_wifi__st_button_3;
extern lv_obj_t *ui_wifi_st_3;
extern lv_obj_t *ui_time3;
extern lv_obj_t *ui_setting_icon;
extern lv_obj_t *ui_setting_title;
extern lv_obj_t *ui_setting_display;
extern lv_obj_t *ui_setting_display_icon;
extern lv_obj_t *ui_setting_display_title;
void ui_event_setting_time( lv_event_t * e);
extern lv_obj_t *ui_setting_time;
extern lv_obj_t *ui_setting_time_icon;
extern lv_obj_t *ui_setting_time_title;
extern lv_obj_t *ui_scrolldots3;
extern lv_obj_t *ui_scrolldots31;
extern lv_obj_t *ui_scrolldots32;
extern lv_obj_t *ui_scrolldots33;
extern lv_obj_t *ui_screen_display;
extern lv_obj_t *ui_wifi_st_4;
void ui_event_back1( lv_event_t * e);
extern lv_obj_t *ui_back1;
extern lv_obj_t *ui_display_title;
extern lv_obj_t *ui_brighness;
extern lv_obj_t *ui_brighness_cfg;
extern lv_obj_t *ui_brighness_title;
extern lv_obj_t *ui_brighness_icon_1;
extern lv_obj_t *ui_brighness_icon_2;
extern lv_obj_t *ui_screen_always_on;
extern lv_obj_t *ui_screen_always_on_title;
void ui_event_screen_always_on_cfg( lv_event_t * e);
extern lv_obj_t *ui_screen_always_on_cfg;
extern lv_obj_t *ui_turn_off_after_time;
extern lv_obj_t *ui_after;
void ui_event_sleep_mode_time_cfg( lv_event_t * e);
extern lv_obj_t *ui_turn_off_after_time_cfg;
extern lv_obj_t *ui_min;
void ui_event_display_keyboard( lv_event_t * e);
void ui_event_screen_matter( lv_event_t * e);
void ui_event_slider_value_update( lv_event_t * e);
extern lv_obj_t *ui_display_keyboard;
extern lv_obj_t *ui_screen_date_time;
extern lv_obj_t *ui_wifi_st_5;
void ui_event_back2( lv_event_t * e);
extern lv_obj_t *ui_back2;
extern lv_obj_t *ui_date_time_title;
extern lv_obj_t *ui_screen_matter_settings_title;
extern lv_obj_t *ui_time_format;
extern lv_obj_t *ui_time_format_title;
extern lv_obj_t *ui_time_format_cfg;
extern lv_obj_t *ui_auto_update;
extern lv_obj_t *ui_auto_update_title;
void ui_event_auto_update_cfg( lv_event_t * e);
extern lv_obj_t *ui_auto_update_cfg;
extern lv_obj_t *ui_date_time;
extern lv_obj_t * ui_time_zone;
extern lv_obj_t *  ui_zone_auto_update_cfg;
extern lv_obj_t *ui_time_zone_title;
extern lv_obj_t *ui_time_zone_num_cfg;
extern lv_obj_t *ui_utc_tile;
extern lv_obj_t *ui_time_zone_sign_cfg_;
extern lv_obj_t *ui_daylight_title;
extern lv_obj_t *ui_daylight_cfg;
extern lv_obj_t *ui_manual_setting_title;
extern lv_obj_t *ui_date_cfg;
extern lv_obj_t *ui_hour_cfg;
extern lv_obj_t *ui_min_cfg;
extern lv_obj_t *ui_sec_cfg;
extern lv_obj_t *ui_time_label1;
extern lv_obj_t *ui_time_label2;
extern lv_obj_t *ui_wifi_st_6;
extern lv_obj_t * ui_time_save;
extern lv_obj_t *ui_back3;

extern lv_obj_t *ui_screen_factory;
extern lv_obj_t *ui_factory_resetting_title;

extern lv_obj_t *ui_screen_matter_setup;
extern lv_obj_t *ui_screen_matter_settings;
extern lv_obj_t *ui_screen_matter;
extern lv_obj_t *ui_button_panel1;
extern lv_obj_t *ui_toggle_button1;
extern lv_obj_t *ui_button_panel_label1;
extern lv_obj_t *ui_slider_with_value_panel;
extern lv_obj_t *ui_arc;
extern lv_obj_t *ui_slider_value_label;
extern lv_obj_t *ui_slider_with_value_panel_label;
extern lv_obj_t *ui_button_panel2;
extern lv_obj_t *ui_toggle_button2;
extern lv_obj_t *ui_toggle_button2_label;
extern lv_obj_t *ui_button_panel_label2;
extern lv_obj_t *ui_switch_panel;
extern lv_obj_t *ui_switch1;
extern lv_obj_t *ui_switch_panel_label;
extern lv_obj_t *ui_slider_panel;
extern lv_obj_t *ui_slider1;
extern lv_obj_t *ui_slider_panel_label;
extern lv_obj_t *ui_header;
extern lv_obj_t *ui_qrcode_img;
extern lv_obj_t *ui_qrcode_panel;

LV_IMG_DECLARE( ui_img_wifi_disconet_png);   // assets/wifi_disconet.png
LV_IMG_DECLARE( ui_img_location_png);   // assets/location.png
LV_IMG_DECLARE( ui_img_temp_1_png);   // assets/temp_1.png
LV_IMG_DECLARE( ui_img_humidity_1_png);   // assets/humidity_1.png
LV_IMG_DECLARE( ui_img_co2_png);   // assets/co2.png
LV_IMG_DECLARE( ui_img_tvoc_png);   // assets/tvoc.png
LV_IMG_DECLARE( ui_img_temp_2_png);   // assets/temp_2.png
LV_IMG_DECLARE( ui_img_humidity_2_png);   // assets/humidity_2.png
LV_IMG_DECLARE( ui_img_setting_png);   // assets/setting.png
LV_IMG_DECLARE( ui_img_display_png);   // assets/display.png
LV_IMG_DECLARE( ui_img_time_png);   // assets/time.png
LV_IMG_DECLARE( ui_img_back_png);   // assets/back.png
LV_IMG_DECLARE( ui_img_high_light_png);   // assets/high_light.png
LV_IMG_DECLARE( ui_img_low_light_png);   // assets/low_light.png
LV_IMG_DECLARE( ui_img_background_png);
LV_IMG_DECLARE( ui_img_location2_png);
LV_IMG_DECLARE( ui_img_matter_qrcode_png );

LV_FONT_DECLARE( ui_font_font0);
LV_FONT_DECLARE( ui_font_font1);
LV_FONT_DECLARE( ui_font_font2);
LV_FONT_DECLARE( ui_font_font3);
LV_FONT_DECLARE( ui_font_font4);

void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
