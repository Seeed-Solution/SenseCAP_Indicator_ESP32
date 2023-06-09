// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.0
// LVGL version: 8.3.6
// Project name: ASIS

#include "../ui.h"

void ui_screen_ha_ctrl_screen_init(void)
{
ui_screen_ha_ctrl = lv_obj_create(NULL);
lv_obj_clear_flag( ui_screen_ha_ctrl, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_screen_ha_ctrl, lv_color_hex(0x1C1C1C), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_screen_ha_ctrl, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_ha_ctrl_wifi_btn = lv_btn_create(ui_screen_ha_ctrl);
lv_obj_set_width( ui_ha_ctrl_wifi_btn, 60);
lv_obj_set_height( ui_ha_ctrl_wifi_btn, 60);
lv_obj_set_x( ui_ha_ctrl_wifi_btn, -10 );
lv_obj_set_y( ui_ha_ctrl_wifi_btn, 10 );
lv_obj_set_align( ui_ha_ctrl_wifi_btn, LV_ALIGN_TOP_RIGHT );
lv_obj_add_flag( ui_ha_ctrl_wifi_btn, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_ha_ctrl_wifi_btn, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_ha_ctrl_wifi_btn, lv_color_hex(0x1C1C1C), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_ha_ctrl_wifi_btn, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_grad_color(ui_ha_ctrl_wifi_btn, lv_color_hex(0x1C1C1C), LV_PART_MAIN | LV_STATE_DEFAULT );

ui_ha_ctrl_wifi_st = lv_img_create(ui_ha_ctrl_wifi_btn);
lv_img_set_src(ui_ha_ctrl_wifi_st, &ui_img_wifi_disconet_png);
lv_obj_set_width( ui_ha_ctrl_wifi_st, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_ha_ctrl_wifi_st, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_ha_ctrl_wifi_st, LV_ALIGN_TOP_RIGHT );
lv_obj_add_flag( ui_ha_ctrl_wifi_st, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_ha_ctrl_wifi_st, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_ha_ctrl_time = lv_label_create(ui_screen_ha_ctrl);
lv_obj_set_width( ui_ha_ctrl_time, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_ha_ctrl_time, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_ha_ctrl_time, 30 );
lv_obj_set_y( ui_ha_ctrl_time, 20 );
lv_label_set_text(ui_ha_ctrl_time,"21:20");
lv_obj_set_style_text_font(ui_ha_ctrl_time, &ui_font_font1, LV_PART_MAIN| LV_STATE_DEFAULT);


ui_screen_home_ctrl_lable = lv_label_create(ui_screen_ha_ctrl);
lv_obj_set_width( ui_screen_home_ctrl_lable, 316);
lv_obj_set_height( ui_screen_home_ctrl_lable, 38);
lv_obj_set_x( ui_screen_home_ctrl_lable, 82 );
lv_obj_set_y( ui_screen_home_ctrl_lable, 58 );
lv_label_set_text(ui_screen_home_ctrl_lable,"Home Assistant Control");
lv_obj_set_style_text_color(ui_screen_home_ctrl_lable, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_screen_home_ctrl_lable, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_screen_home_ctrl_lable, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_screen_home_ctrl_lable, &ui_font_font0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_switch4 = lv_btn_create(ui_screen_ha_ctrl);
lv_obj_set_width( ui_switch4, 214);
lv_obj_set_height( ui_switch4, 164);
lv_obj_set_x( ui_switch4, 22 );
lv_obj_set_y( ui_switch4, 96 );
lv_obj_add_flag( ui_switch4, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_switch4, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_switch4, 15, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_radius(ui_switch4, 20, LV_PART_MAIN| LV_STATE_CHECKED);
lv_obj_set_style_bg_color(ui_switch4, lv_color_hex(0x282828), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_switch4, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_switch4, lv_color_hex(0x3E3E3E), LV_PART_MAIN | LV_STATE_CHECKED );
lv_obj_set_style_bg_opa(ui_switch4, 255, LV_PART_MAIN| LV_STATE_CHECKED);
lv_obj_add_flag(ui_switch4, LV_OBJ_FLAG_CHECKABLE);

ui_switch4_logo = lv_img_create(ui_switch4);
lv_img_set_src(ui_switch4_logo, &ui_img_ic_switch2_off_png);
lv_obj_set_width( ui_switch4_logo, LV_SIZE_CONTENT);  /// 45
lv_obj_set_height( ui_switch4_logo, LV_SIZE_CONTENT);   /// 45
lv_obj_set_x( ui_switch4_logo, 0 );
lv_obj_set_y( ui_switch4_logo, -10 );
lv_obj_set_align( ui_switch4_logo, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_switch4_logo, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_switch4_logo, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_switch4_label = lv_label_create(ui_switch4);
lv_obj_set_width( ui_switch4_label, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_switch4_label, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_switch4_label, LV_ALIGN_BOTTOM_MID );
lv_label_set_text(ui_switch4_label,CONFIG_SWITCH4_UI_NAME);
lv_obj_set_style_text_color(ui_switch4_label, lv_color_hex(0x9E9E9E), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_switch4_label, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_switch4_label, &lv_font_montserrat_18, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_switch5 = lv_obj_create(ui_screen_ha_ctrl);
lv_obj_set_width( ui_switch5, 214);
lv_obj_set_height( ui_switch5, 164);
lv_obj_set_x( ui_switch5, 244 );
lv_obj_set_y( ui_switch5, 96 );
lv_obj_clear_flag( ui_switch5, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_switch5, 12, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_switch5, lv_color_hex(0x282828), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_switch5, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_color(ui_switch5, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_switch5, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_switch5, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_switch5_arc = lv_arc_create(ui_switch5);
lv_obj_set_width( ui_switch5_arc, 130);
lv_obj_set_height( ui_switch5_arc, 125);
lv_obj_set_align( ui_switch5_arc, LV_ALIGN_CENTER );
lv_arc_set_value(ui_switch5_arc, 0);
lv_arc_set_bg_angles(ui_switch5_arc,140,40);
lv_obj_set_style_arc_color(ui_switch5_arc, lv_color_hex(0x1C1C1C), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_arc_opa(ui_switch5_arc, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_arc_width(ui_switch5_arc, 8, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_set_style_pad_left(ui_switch5_arc, 0, LV_PART_INDICATOR| LV_STATE_DEFAULT);
lv_obj_set_style_pad_right(ui_switch5_arc, 0, LV_PART_INDICATOR| LV_STATE_DEFAULT);
lv_obj_set_style_pad_top(ui_switch5_arc, 0, LV_PART_INDICATOR| LV_STATE_DEFAULT);
lv_obj_set_style_pad_bottom(ui_switch5_arc, 0, LV_PART_INDICATOR| LV_STATE_DEFAULT);
lv_obj_set_style_arc_color(ui_switch5_arc, lv_color_hex(0x529D53), LV_PART_INDICATOR | LV_STATE_DEFAULT );
lv_obj_set_style_arc_opa(ui_switch5_arc, 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);
lv_obj_set_style_arc_width(ui_switch5_arc, 8, LV_PART_INDICATOR| LV_STATE_DEFAULT);
lv_obj_set_style_arc_rounded(ui_switch5_arc, true, LV_PART_INDICATOR| LV_STATE_DEFAULT);

lv_obj_set_style_bg_color(ui_switch5_arc, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_switch5_arc, 255, LV_PART_KNOB| LV_STATE_DEFAULT);

ui_switch5_arc_data = lv_label_create(ui_switch5_arc);
lv_obj_set_width( ui_switch5_arc_data, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_switch5_arc_data, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_switch5_arc_data, 0 );
lv_obj_set_y( ui_switch5_arc_data, -50 );
lv_obj_set_align( ui_switch5_arc_data, LV_ALIGN_BOTTOM_MID );
lv_label_set_text(ui_switch5_arc_data,"0");
lv_obj_set_style_text_color(ui_switch5_arc_data, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_switch5_arc_data, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_switch5_arc_data, &lv_font_montserrat_24, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_switch5_label = lv_label_create(ui_switch5);
lv_obj_set_width( ui_switch5_label, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_switch5_label, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_switch5_label, 0 );
lv_obj_set_y( ui_switch5_label, -5 );
lv_obj_set_align( ui_switch5_label, LV_ALIGN_BOTTOM_MID );
lv_label_set_text(ui_switch5_label, CONFIG_SWITCH5_UI_NAME);
lv_obj_set_style_text_color(ui_switch5_label, lv_color_hex(0x9E9E9E), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_switch5_label, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_switch5_label, &lv_font_montserrat_18, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_switch6 = lv_btn_create(ui_screen_ha_ctrl);
lv_obj_set_width( ui_switch6, 214);
lv_obj_set_height( ui_switch6, 78);
lv_obj_set_x( ui_switch6, 22 );
lv_obj_set_y( ui_switch6, 268 );
lv_obj_add_flag( ui_switch6, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_switch6, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_switch6, 15, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_radius(ui_switch6, 20, LV_PART_MAIN| LV_STATE_CHECKED);
lv_obj_set_style_bg_color(ui_switch6, lv_color_hex(0x282828), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_switch6, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_switch6, lv_color_hex(0x3E3E3E), LV_PART_MAIN | LV_STATE_CHECKED );
lv_obj_set_style_bg_opa(ui_switch6, 255, LV_PART_MAIN| LV_STATE_CHECKED);
lv_obj_add_flag(ui_switch6, LV_OBJ_FLAG_CHECKABLE);

ui_switch6_logo = lv_img_create(ui_switch6);
lv_img_set_src(ui_switch6_logo, &ui_img_ic_switch1_off_png);
lv_obj_set_x( ui_switch6_logo, 120 );
lv_obj_set_y( ui_switch6_logo, 2 );
lv_obj_add_flag( ui_switch6_logo, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_switch6_logo, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_switch6_label = lv_label_create(ui_switch6);
lv_obj_set_width( ui_switch6_label, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_switch6_label, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_switch6_label, 16 );
lv_obj_set_y( ui_switch6_label, 22 );
lv_label_set_text(ui_switch6_label, CONFIG_SWITCH6_UI_NAME);
lv_obj_set_style_text_color(ui_switch6_label, lv_color_hex(0x9E9E9E), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_switch6_label, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_switch6_label, &lv_font_montserrat_18, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_switch7 = lv_btn_create(ui_screen_ha_ctrl);
lv_obj_set_width( ui_switch7, 214);
lv_obj_set_height( ui_switch7, 78);
lv_obj_set_x( ui_switch7, 244 );
lv_obj_set_y( ui_switch7, 268 );
lv_obj_add_flag( ui_switch7, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_switch7, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_switch7, 15, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_radius(ui_switch7, 20, LV_PART_MAIN| LV_STATE_CHECKED);
lv_obj_set_style_bg_color(ui_switch7, lv_color_hex(0x282828), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_switch7, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_switch7, lv_color_hex(0x3E3E3E), LV_PART_MAIN | LV_STATE_CHECKED );
lv_obj_set_style_bg_opa(ui_switch7, 255, LV_PART_MAIN| LV_STATE_CHECKED);
lv_obj_add_flag(ui_switch7, LV_OBJ_FLAG_CHECKABLE);

ui_switch7_label = lv_label_create(ui_switch7);
lv_obj_set_width( ui_switch7_label, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_switch7_label, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_switch7_label, 16 );
lv_obj_set_y( ui_switch7_label, 22 );
lv_label_set_text(ui_switch7_label,CONFIG_SWITCH7_UI_NAME);
lv_obj_set_style_text_color(ui_switch7_label, lv_color_hex(0x9E9E9E), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_switch7_label, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_switch7_label, &lv_font_montserrat_18, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_switch7_switch = lv_switch_create(ui_switch7);
lv_obj_set_width( ui_switch7_switch, 80);
lv_obj_set_height( ui_switch7_switch, 30);
lv_obj_set_x( ui_switch7_switch, 102 );
lv_obj_set_y( ui_switch7_switch, 18 );
// lv_obj_add_state( ui_switch7_switch, LV_STATE_CHECKED );     /// States
lv_obj_set_style_radius(ui_switch7_switch, 40, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_set_style_radius(ui_switch7_switch, 40, LV_PART_INDICATOR| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_switch7_switch, lv_color_hex(0x4F4F4F), LV_PART_INDICATOR | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_switch7_switch, 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_switch7_switch, lv_color_hex(0x529D53), LV_PART_INDICATOR | LV_STATE_CHECKED );
lv_obj_set_style_bg_opa(ui_switch7_switch, 255, LV_PART_INDICATOR| LV_STATE_CHECKED);

ui_switch8 = lv_btn_create(ui_screen_ha_ctrl);
lv_obj_set_width( ui_switch8, 440);
lv_obj_set_height( ui_switch8, 78);
lv_obj_set_x( ui_switch8, 22 );
lv_obj_set_y( ui_switch8, 358 );
lv_obj_add_flag( ui_switch8, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_switch8, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_switch8, 12, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_switch8, lv_color_hex(0x282828), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_switch8, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_switch8_label = lv_label_create(ui_switch8);
lv_obj_set_width( ui_switch8_label, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_switch8_label, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_switch8_label, LV_ALIGN_BOTTOM_MID );
lv_label_set_text(ui_switch8_label, CONFIG_SWITCH8_UI_NAME);
lv_obj_set_style_text_color(ui_switch8_label, lv_color_hex(0x9E9E9E), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_switch8_label, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_switch8_label, &lv_font_montserrat_18, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_switch8_slider = lv_slider_create(ui_switch8);
lv_slider_set_value( ui_switch8_slider, 0, LV_ANIM_OFF);
if (lv_slider_get_mode(ui_switch8_slider)==LV_SLIDER_MODE_RANGE ) lv_slider_set_left_value( ui_switch8_slider, 0, LV_ANIM_OFF);
lv_obj_set_width( ui_switch8_slider, 385);
lv_obj_set_height( ui_switch8_slider, 20);
lv_obj_set_x( ui_switch8_slider, 12 );
lv_obj_set_y( ui_switch8_slider, 12 );
lv_obj_set_style_radius(ui_switch8_slider, 15, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_switch8_slider, lv_color_hex(0x1C1C1C), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_switch8_slider, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_set_style_bg_color(ui_switch8_slider, lv_color_hex(0x529D53), LV_PART_INDICATOR | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_switch8_slider, 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);

lv_obj_set_style_bg_color(ui_switch8_slider, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_switch8_slider, 255, LV_PART_KNOB| LV_STATE_DEFAULT);

creat_scrolldots(ui_screen_ha_ctrl, 4);
}
