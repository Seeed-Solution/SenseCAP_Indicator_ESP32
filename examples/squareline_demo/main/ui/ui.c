// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.2.1
// LVGL VERSION: 8.3.4
// PROJECT: SquareLine_Project

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t *ui_scren_time;
lv_obj_t *ui_background;
lv_obj_t *ui_hour;
lv_obj_t *ui_hour_cfg;
lv_obj_t *ui_min;
lv_obj_t *ui_min_cfg;
lv_obj_t *ui_location;
lv_obj_t *ui_city;
lv_obj_t *ui_location_Icon;
lv_obj_t *ui_date_panel;
lv_obj_t *ui_date;
lv_obj_t *ui_Label1;
lv_obj_t *ui_adorn;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////

///////////////////// SCREENS ////////////////////
void ui_scren_time_screen_init(void)
{
ui_scren_time = lv_obj_create(NULL);
lv_obj_clear_flag( ui_scren_time, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_background = lv_img_create(ui_scren_time);
lv_img_set_src(ui_background, &ui_img_background_png);
lv_obj_set_width( ui_background, lv_pct(100));
lv_obj_set_height( ui_background, lv_pct(100));
lv_obj_set_align( ui_background, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_background, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_background, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_hour = lv_obj_create(ui_background);
lv_obj_set_width( ui_hour, 200);
lv_obj_set_height( ui_hour, 170);
lv_obj_set_x( ui_hour, -110 );
lv_obj_set_y( ui_hour, 0 );
lv_obj_set_align( ui_hour, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_hour, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_hour, lv_color_hex(0x2E2E2E), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_hour, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_hour_cfg = lv_label_create(ui_hour);
lv_obj_set_width( ui_hour_cfg, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_hour_cfg, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_hour_cfg, LV_ALIGN_CENTER );
lv_label_set_text(ui_hour_cfg,"20");
lv_obj_set_style_text_font(ui_hour_cfg, &ui_font_Font1, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_min = lv_obj_create(ui_background);
lv_obj_set_width( ui_min, 200);
lv_obj_set_height( ui_min, 170);
lv_obj_set_x( ui_min, 110 );
lv_obj_set_y( ui_min, 0 );
lv_obj_set_align( ui_min, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_min, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_min, lv_color_hex(0x2E2E2E), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_min, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_min_cfg = lv_label_create(ui_min);
lv_obj_set_width( ui_min_cfg, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_min_cfg, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_min_cfg, LV_ALIGN_CENTER );
lv_label_set_text(ui_min_cfg,"23");
lv_obj_set_style_text_font(ui_min_cfg, &ui_font_Font1, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_location = lv_obj_create(ui_scren_time);
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
lv_label_set_text(ui_city,"Shen Zhen");
lv_obj_set_style_text_font(ui_city, &lv_font_montserrat_16, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_location_Icon = lv_img_create(ui_scren_time);
lv_img_set_src(ui_location_Icon, &ui_img_location2_png);
lv_obj_set_width( ui_location_Icon, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_location_Icon, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_location_Icon, 199 );
lv_obj_set_y( ui_location_Icon, 103 );
lv_obj_set_align( ui_location_Icon, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_location_Icon, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_location_Icon, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_date_panel = lv_obj_create(ui_scren_time);
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
lv_label_set_text(ui_date,"Monday, 06 / 03 /  2023");
lv_label_set_recolor(ui_date,"true");
lv_obj_set_style_text_font(ui_date, &lv_font_montserrat_16, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Label1 = lv_label_create(ui_scren_time);
lv_obj_set_width( ui_Label1, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label1, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_Label1, LV_ALIGN_CENTER );
lv_label_set_text(ui_Label1,":");
lv_obj_set_style_text_font(ui_Label1, &lv_font_montserrat_48, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_adorn = lv_obj_create(ui_scren_time);
lv_obj_set_width( ui_adorn, 480);
lv_obj_set_height( ui_adorn, 6);
lv_obj_set_align( ui_adorn, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_adorn, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_adorn, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_adorn, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_grad_color(ui_adorn, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_color(ui_adorn, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_adorn, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

}

void ui_init( void )
{
lv_disp_t *dispp = lv_disp_get_default();
lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
lv_disp_set_theme(dispp, theme);
ui_scren_time_screen_init();
lv_disp_load_scr( ui_scren_time);
}
