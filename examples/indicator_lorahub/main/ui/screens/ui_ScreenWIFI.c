// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.3.3
// Project name: indicator_lorahub

#include "../ui.h"

void ui_ScreenWIFI_screen_init(void)
{
    ui_ScreenWIFI = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_ScreenWIFI, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_BtnBackSWifi = lv_btn_create(ui_ScreenWIFI);
    lv_obj_set_width(ui_BtnBackSWifi, 100);
    lv_obj_set_height(ui_BtnBackSWifi, 50);
    lv_obj_set_x(ui_BtnBackSWifi, 10);
    lv_obj_set_y(ui_BtnBackSWifi, 30);
    lv_obj_add_flag(ui_BtnBackSWifi, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_BtnBackSWifi, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_BtnBackSWifi, lv_color_hex(0x101418), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BtnBackSWifi, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui_BtnBackSWifi, &ui_img_back_png, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblTitleWifi = lv_label_create(ui_ScreenWIFI);
    lv_obj_set_width(ui_LblTitleWifi, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_LblTitleWifi, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_LblTitleWifi, 1);
    lv_obj_set_y(ui_LblTitleWifi, 50);
    lv_obj_set_align(ui_LblTitleWifi, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_LblTitleWifi, "WiFi");
    lv_obj_set_style_text_font(ui_LblTitleWifi, &ui_font_HeadTitle, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_BtnBackSWifi, ui_event_BtnBackSWifi, LV_EVENT_ALL, NULL);

}
