// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.0
// LVGL version: 8.3.3
// Project name: sensecap

#include "../ui.h"

void ui_screen_openai_screen_init(void)
{
ui_screen_openai = lv_obj_create(NULL);
lv_obj_clear_flag( ui_screen_openai, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_time4 = lv_label_create(ui_screen_openai);
lv_obj_set_width( ui_time4, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_time4, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_time4, 30 );
lv_obj_set_y( ui_time4, 20 );
lv_label_set_text(ui_time4,"21:20");
lv_obj_set_style_text_font(ui_time4, &ui_font_font1, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_wifi__st_btn_7 = lv_btn_create(ui_screen_openai);
lv_obj_set_width( ui_wifi__st_btn_7, 60);
lv_obj_set_height( ui_wifi__st_btn_7, 60);
lv_obj_set_x( ui_wifi__st_btn_7, -10 );
lv_obj_set_y( ui_wifi__st_btn_7, 10 );
lv_obj_set_align( ui_wifi__st_btn_7, LV_ALIGN_TOP_RIGHT );
lv_obj_add_flag( ui_wifi__st_btn_7, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_wifi__st_btn_7, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_wifi__st_btn_7, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_wifi__st_btn_7, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_grad_color(ui_wifi__st_btn_7, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );

ui_wifi_st_7 = lv_img_create(ui_wifi__st_btn_7);
lv_img_set_src(ui_wifi_st_7, &ui_img_wifi_disconet_png);
lv_obj_set_width( ui_wifi_st_7, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_wifi_st_7, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_wifi_st_7, LV_ALIGN_TOP_RIGHT );
lv_obj_add_flag( ui_wifi_st_7, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_wifi_st_7, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_openai_log = lv_img_create(ui_screen_openai);
lv_img_set_src(ui_openai_log, &ui_img_openai_log_png);
lv_obj_set_width( ui_openai_log, LV_SIZE_CONTENT);  /// 263
lv_obj_set_height( ui_openai_log, LV_SIZE_CONTENT);   /// 65
lv_obj_set_x( ui_openai_log, 108 );
lv_obj_set_y( ui_openai_log, 88 );
lv_obj_add_flag( ui_openai_log, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_openai_log, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_chatgpt_btn = lv_btn_create(ui_screen_openai);
lv_obj_set_width( ui_chatgpt_btn, 300);
lv_obj_set_height( ui_chatgpt_btn, 60);
lv_obj_set_x( ui_chatgpt_btn, 90 );
lv_obj_set_y( ui_chatgpt_btn, 270 );
lv_obj_add_flag( ui_chatgpt_btn, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_chatgpt_btn, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_chatgpt_btn, lv_color_hex(0x10A37F), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_chatgpt_btn, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_chatgpt__title = lv_label_create(ui_chatgpt_btn);
lv_obj_set_width( ui_chatgpt__title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_chatgpt__title, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_chatgpt__title, LV_ALIGN_CENTER );
lv_label_set_text(ui_chatgpt__title,"ChatGPT");
lv_obj_set_style_text_align(ui_chatgpt__title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_chatgpt__title, &ui_font_font1, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_set_api_key_btn = lv_btn_create(ui_screen_openai);
lv_obj_set_width( ui_set_api_key_btn, 300);
lv_obj_set_height( ui_set_api_key_btn, 60);
lv_obj_set_x( ui_set_api_key_btn, 90 );
lv_obj_set_y( ui_set_api_key_btn, 194 );
lv_obj_add_flag( ui_set_api_key_btn, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_set_api_key_btn, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_set_api_key_btn, lv_color_hex(0x10A37F), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_set_api_key_btn, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_set_api_key_title = lv_label_create(ui_set_api_key_btn);
lv_obj_set_width( ui_set_api_key_title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_set_api_key_title, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_set_api_key_title, LV_ALIGN_CENTER );
lv_label_set_text(ui_set_api_key_title,"Set API Key");
lv_obj_set_style_text_align(ui_set_api_key_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_set_api_key_title, &ui_font_font1, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_dalle_btn = lv_btn_create(ui_screen_openai);
lv_obj_set_width( ui_dalle_btn, 300);
lv_obj_set_height( ui_dalle_btn, 60);
lv_obj_set_x( ui_dalle_btn, 90 );
lv_obj_set_y( ui_dalle_btn, 346 );
lv_obj_add_flag( ui_dalle_btn, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_dalle_btn, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_dalle_btn, lv_color_hex(0x10A37F), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_dalle_btn, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_dalle_title = lv_label_create(ui_dalle_btn);
lv_obj_set_width( ui_dalle_title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_dalle_title, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_dalle_title, 0 );
lv_obj_set_y( ui_dalle_title, -2 );
lv_obj_set_align( ui_dalle_title, LV_ALIGN_CENTER );
lv_label_set_text(ui_dalle_title,"DALL•E");
lv_obj_set_style_text_font(ui_dalle_title, &lv_font_montserrat_26, LV_PART_MAIN| LV_STATE_DEFAULT);


lv_obj_t *ui_scrolldots4 = lv_obj_create(ui_screen_openai);
lv_obj_set_width( ui_scrolldots4, 200);
lv_obj_set_height( ui_scrolldots4, 20);
lv_obj_set_x( ui_scrolldots4, 0 );
lv_obj_set_y( ui_scrolldots4, -20 );
lv_obj_set_align( ui_scrolldots4, LV_ALIGN_BOTTOM_MID );
lv_obj_clear_flag( ui_scrolldots4, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_scrolldots4, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_scrolldots4, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_color(ui_scrolldots4, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_scrolldots4, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_t * ui_scrolldots41 = lv_obj_create(ui_scrolldots4);
lv_obj_set_width( ui_scrolldots41, 7);
lv_obj_set_height( ui_scrolldots41, 7);
lv_obj_set_x( ui_scrolldots41, -45);
lv_obj_set_align( ui_scrolldots41, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_scrolldots41, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

lv_obj_t * ui_scrolldots42 = lv_obj_create(ui_scrolldots4);
lv_obj_set_width( ui_scrolldots42, 7);
lv_obj_set_height( ui_scrolldots42, 7);
lv_obj_set_x( ui_scrolldots42, -15);
lv_obj_set_align( ui_scrolldots42, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_scrolldots42, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

lv_obj_t * ui_scrolldots43 = lv_obj_create(ui_scrolldots4);
lv_obj_set_width( ui_scrolldots43, 7);
lv_obj_set_height( ui_scrolldots43, 7);
lv_obj_set_x( ui_scrolldots43, 15);
lv_obj_set_align( ui_scrolldots43, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_scrolldots43, LV_OBJ_FLAG_SCROLLABLE );    /// Flags


lv_obj_t *ui_scrolldots44 = lv_obj_create(ui_scrolldots4);
lv_obj_set_width( ui_scrolldots44, 7);
lv_obj_set_height( ui_scrolldots44, 7);
lv_obj_set_x( ui_scrolldots44, 45);
lv_obj_set_align( ui_scrolldots44, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_scrolldots44, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

lv_obj_set_style_bg_color(ui_scrolldots43, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_scrolldots43, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_color(ui_scrolldots43, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_scrolldots43, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

}
