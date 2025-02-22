// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.3.3
// Project name: indicator_lorawan

#include "../ui.h"


// COMPONENT date time1

lv_obj_t * ui_date_time1_create(lv_obj_t * comp_parent)
{

    lv_obj_t * cui_date_time1;
    cui_date_time1 = lv_obj_create(comp_parent);
    lv_obj_set_width(cui_date_time1, 400);
    lv_obj_set_height(cui_date_time1, 224);
    lv_obj_set_x(cui_date_time1, 0);
    lv_obj_set_y(cui_date_time1, 88);
    lv_obj_set_align(cui_date_time1, LV_ALIGN_CENTER);
    lv_obj_add_flag(cui_date_time1, LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_SCROLL_ONE);     /// Flags
    lv_obj_clear_flag(cui_date_time1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_t * cui_PnlTimeZone;
    cui_PnlTimeZone = lv_obj_create(cui_date_time1);
    lv_obj_set_width(cui_PnlTimeZone, 400);
    lv_obj_set_height(cui_PnlTimeZone, 50);
    lv_obj_set_align(cui_PnlTimeZone, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(cui_PnlTimeZone, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_border_color(cui_PnlTimeZone, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(cui_PnlTimeZone, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * cui_time_zone_title1;
    cui_time_zone_title1 = lv_label_create(cui_PnlTimeZone);
    lv_obj_set_width(cui_time_zone_title1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(cui_time_zone_title1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(cui_time_zone_title1, LV_ALIGN_LEFT_MID);
    lv_label_set_text(cui_time_zone_title1, "Time Zone");

    lv_obj_t * cui_time_zone_num_cfg1;
    cui_time_zone_num_cfg1 = lv_dropdown_create(cui_PnlTimeZone);
    lv_dropdown_set_dir(cui_time_zone_num_cfg1, LV_DIR_TOP);
    lv_dropdown_set_options(cui_time_zone_num_cfg1, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12");
    lv_obj_set_width(cui_time_zone_num_cfg1, 69);
    lv_obj_set_height(cui_time_zone_num_cfg1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(cui_time_zone_num_cfg1, LV_ALIGN_RIGHT_MID);
    lv_obj_add_flag(cui_time_zone_num_cfg1, LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_set_style_bg_color(cui_time_zone_num_cfg1, lv_color_hex(0x6F6F6F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_time_zone_num_cfg1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * cui_utc_tile1;
    cui_utc_tile1 = lv_label_create(cui_PnlTimeZone);
    lv_obj_set_width(cui_utc_tile1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(cui_utc_tile1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(cui_utc_tile1, -150);
    lv_obj_set_y(cui_utc_tile1, 0);
    lv_obj_set_align(cui_utc_tile1, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(cui_utc_tile1, "UTC");

    lv_obj_t * cui_time_zone_sign_cfg_1;
    cui_time_zone_sign_cfg_1 = lv_dropdown_create(cui_PnlTimeZone);
    lv_dropdown_set_dir(cui_time_zone_sign_cfg_1, LV_DIR_TOP);
    lv_dropdown_set_options(cui_time_zone_sign_cfg_1, "+\n-");
    lv_obj_set_width(cui_time_zone_sign_cfg_1, 50);
    lv_obj_set_height(cui_time_zone_sign_cfg_1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(cui_time_zone_sign_cfg_1, -80);
    lv_obj_set_y(cui_time_zone_sign_cfg_1, 0);
    lv_obj_set_align(cui_time_zone_sign_cfg_1, LV_ALIGN_RIGHT_MID);
    lv_obj_add_flag(cui_time_zone_sign_cfg_1, LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_set_style_bg_color(cui_time_zone_sign_cfg_1, lv_color_hex(0x6F6F6F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_time_zone_sign_cfg_1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * cui_PnlSavingTime;
    cui_PnlSavingTime = lv_obj_create(cui_date_time1);
    lv_obj_set_width(cui_PnlSavingTime, 400);
    lv_obj_set_height(cui_PnlSavingTime, 50);
    lv_obj_set_x(cui_PnlSavingTime, 0);
    lv_obj_set_y(cui_PnlSavingTime, 50);
    lv_obj_set_align(cui_PnlSavingTime, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(cui_PnlSavingTime, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_border_color(cui_PnlSavingTime, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(cui_PnlSavingTime, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * cui_daylight_title1;
    cui_daylight_title1 = lv_label_create(cui_PnlSavingTime);
    lv_obj_set_width(cui_daylight_title1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(cui_daylight_title1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(cui_daylight_title1, LV_ALIGN_LEFT_MID);
    lv_label_set_text(cui_daylight_title1, "Daylight Saving Time");

    lv_obj_t * cui_daylight_cfg1;
    cui_daylight_cfg1 = lv_switch_create(cui_PnlSavingTime);
    lv_obj_set_width(cui_daylight_cfg1, 50);
    lv_obj_set_height(cui_daylight_cfg1, 25);
    lv_obj_set_align(cui_daylight_cfg1, LV_ALIGN_RIGHT_MID);
    lv_obj_add_state(cui_daylight_cfg1, LV_STATE_CHECKED);       /// States
    lv_obj_set_style_bg_color(cui_daylight_cfg1, lv_color_hex(0x363636), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_daylight_cfg1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_daylight_cfg1, lv_color_hex(0xFFFFFF),
                              LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(cui_daylight_cfg1, 255, LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);

    lv_obj_set_style_bg_color(cui_daylight_cfg1, lv_color_hex(0x529D53), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(cui_daylight_cfg1, 255, LV_PART_INDICATOR | LV_STATE_CHECKED);

    lv_obj_t * cui_PnlManualSetting;
    cui_PnlManualSetting = lv_obj_create(cui_date_time1);
    lv_obj_set_width(cui_PnlManualSetting, 400);
    lv_obj_set_height(cui_PnlManualSetting, 100);
    lv_obj_set_x(cui_PnlManualSetting, 1);
    lv_obj_set_y(cui_PnlManualSetting, 100);
    lv_obj_set_align(cui_PnlManualSetting, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(cui_PnlManualSetting, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_border_color(cui_PnlManualSetting, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(cui_PnlManualSetting, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * cui_manual_setting_title1;
    cui_manual_setting_title1 = lv_label_create(cui_PnlManualSetting);
    lv_obj_set_width(cui_manual_setting_title1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(cui_manual_setting_title1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(cui_manual_setting_title1, 0);
    lv_obj_set_y(cui_manual_setting_title1, -5);
    lv_label_set_text(cui_manual_setting_title1, "Manual Setting");

    lv_obj_t * cui_PnlTimeContainer;
    cui_PnlTimeContainer = lv_obj_create(cui_PnlManualSetting);
    lv_obj_set_width(cui_PnlTimeContainer, 400);
    lv_obj_set_height(cui_PnlTimeContainer, 60);
    lv_obj_set_x(cui_PnlTimeContainer, 0);
    lv_obj_set_y(cui_PnlTimeContainer, 10);
    lv_obj_set_align(cui_PnlTimeContainer, LV_ALIGN_BOTTOM_MID);
    lv_obj_clear_flag(cui_PnlTimeContainer, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_border_color(cui_PnlTimeContainer, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(cui_PnlTimeContainer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * cui_date_cfg1;
    cui_date_cfg1 = lv_textarea_create(cui_PnlTimeContainer);
    lv_obj_set_width(cui_date_cfg1, 115);
    lv_obj_set_height(cui_date_cfg1, LV_SIZE_CONTENT);    /// 40
    lv_obj_set_align(cui_date_cfg1, LV_ALIGN_LEFT_MID);
    lv_textarea_set_max_length(cui_date_cfg1, 12);
    lv_textarea_set_placeholder_text(cui_date_cfg1, "01/01/2023");
    lv_textarea_set_one_line(cui_date_cfg1, true);
    lv_obj_set_scrollbar_mode(cui_date_cfg1, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_text_align(cui_date_cfg1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_date_cfg1, &ui_font_SmallTitle, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_date_cfg1, lv_color_hex(0x6F6F6F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_date_cfg1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cui_date_cfg1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cui_date_cfg1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cui_date_cfg1, 7, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cui_date_cfg1, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * cui_PnlTimeSet;
    cui_PnlTimeSet = lv_obj_create(cui_PnlTimeContainer);
    lv_obj_set_width(cui_PnlTimeSet, 200);
    lv_obj_set_height(cui_PnlTimeSet, 60);
    lv_obj_set_x(cui_PnlTimeSet, -50);
    lv_obj_set_y(cui_PnlTimeSet, 0);
    lv_obj_set_align(cui_PnlTimeSet, LV_ALIGN_RIGHT_MID);
    lv_obj_clear_flag(cui_PnlTimeSet, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_border_color(cui_PnlTimeSet, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(cui_PnlTimeSet, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * cui_hour_cfg2;
    cui_hour_cfg2 = lv_roller_create(cui_PnlTimeSet);
    lv_roller_set_options(cui_hour_cfg2,
                          "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23",
                          LV_ROLLER_MODE_INFINITE);
    lv_obj_set_height(cui_hour_cfg2, 100);
    lv_obj_set_width(cui_hour_cfg2, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_align(cui_hour_cfg2, LV_ALIGN_LEFT_MID);

    lv_obj_set_style_bg_color(cui_hour_cfg2, lv_color_hex(0x529D52), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_hour_cfg2, 255, LV_PART_SELECTED | LV_STATE_DEFAULT);

    lv_obj_t * cui_time_label3;
    cui_time_label3 = lv_label_create(cui_PnlTimeSet);
    lv_obj_set_width(cui_time_label3, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(cui_time_label3, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(cui_time_label3, -25);
    lv_obj_set_y(cui_time_label3, 0);
    lv_obj_set_align(cui_time_label3, LV_ALIGN_CENTER);
    lv_label_set_text(cui_time_label3, ":");

    lv_obj_t * cui_min_cfg2;
    cui_min_cfg2 = lv_roller_create(cui_PnlTimeSet);
    lv_roller_set_options(cui_min_cfg2,
                          "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59",
                          LV_ROLLER_MODE_INFINITE);
    lv_obj_set_height(cui_min_cfg2, 100);
    lv_obj_set_width(cui_min_cfg2, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x(cui_min_cfg2, 5);
    lv_obj_set_y(cui_min_cfg2, 0);
    lv_obj_set_align(cui_min_cfg2, LV_ALIGN_CENTER);

    lv_obj_set_style_bg_color(cui_min_cfg2, lv_color_hex(0x529D52), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_min_cfg2, 255, LV_PART_SELECTED | LV_STATE_DEFAULT);

    lv_obj_t * cui_time_label4;
    cui_time_label4 = lv_label_create(cui_PnlTimeSet);
    lv_obj_set_width(cui_time_label4, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(cui_time_label4, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(cui_time_label4, -45);
    lv_obj_set_y(cui_time_label4, 0);
    lv_obj_set_align(cui_time_label4, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(cui_time_label4, ":");

    lv_obj_t * cui_sec_cfg1;
    cui_sec_cfg1 = lv_roller_create(cui_PnlTimeSet);
    lv_roller_set_options(cui_sec_cfg1,
                          "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59",
                          LV_ROLLER_MODE_INFINITE);
    lv_obj_set_height(cui_sec_cfg1, 100);
    lv_obj_set_width(cui_sec_cfg1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x(cui_sec_cfg1, 10);
    lv_obj_set_y(cui_sec_cfg1, 0);
    lv_obj_set_align(cui_sec_cfg1, LV_ALIGN_RIGHT_MID);

    lv_obj_set_style_bg_color(cui_sec_cfg1, lv_color_hex(0x529D52), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_sec_cfg1, 255, LV_PART_SELECTED | LV_STATE_DEFAULT);

    lv_obj_t * cui_Button1;
    cui_Button1 = lv_btn_create(cui_PnlTimeContainer);
    lv_obj_set_width(cui_Button1, 45);
    lv_obj_set_height(cui_Button1, 40);
    lv_obj_set_align(cui_Button1, LV_ALIGN_RIGHT_MID);
    lv_obj_add_flag(cui_Button1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(cui_Button1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(cui_Button1, lv_color_hex(0x529D52), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_Button1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * cui_Label1;
    cui_Label1 = lv_label_create(cui_Button1);
    lv_obj_set_width(cui_Label1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(cui_Label1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(cui_Label1, LV_ALIGN_CENTER);
    lv_label_set_text(cui_Label1, "Save");

    lv_obj_t ** children = lv_mem_alloc(sizeof(lv_obj_t *) * _UI_COMP_DATE_TIME1_NUM);
    children[UI_COMP_DATE_TIME1_DATE_TIME1] = cui_date_time1;
    children[UI_COMP_DATE_TIME1_PNLTIMEZONE] = cui_PnlTimeZone;
    children[UI_COMP_DATE_TIME1_PNLTIMEZONE_TIME_ZONE_TITLE1] = cui_time_zone_title1;
    children[UI_COMP_DATE_TIME1_PNLTIMEZONE_TIME_ZONE_NUM_CFG1] = cui_time_zone_num_cfg1;
    children[UI_COMP_DATE_TIME1_PNLTIMEZONE_UTC_TILE1] = cui_utc_tile1;
    children[UI_COMP_DATE_TIME1_PNLTIMEZONE_TIME_ZONE_SIGN_CFG_1] = cui_time_zone_sign_cfg_1;
    children[UI_COMP_DATE_TIME1_PNLSAVINGTIME] = cui_PnlSavingTime;
    children[UI_COMP_DATE_TIME1_PNLSAVINGTIME_DAYLIGHT_TITLE1] = cui_daylight_title1;
    children[UI_COMP_DATE_TIME1_PNLSAVINGTIME_DAYLIGHT_CFG1] = cui_daylight_cfg1;
    children[UI_COMP_DATE_TIME1_PNLMANUALSETTING] = cui_PnlManualSetting;
    children[UI_COMP_DATE_TIME1_PNLMANUALSETTING_MANUAL_SETTING_TITLE1] = cui_manual_setting_title1;
    children[UI_COMP_DATE_TIME1_PNLMANUALSETTING_PNLTIMECONTAINER] = cui_PnlTimeContainer;
    children[UI_COMP_DATE_TIME1_PNLMANUALSETTING_PNLTIMECONTAINER_DATE_CFG1] = cui_date_cfg1;
    children[UI_COMP_DATE_TIME1_PNLMANUALSETTING_PNLTIMECONTAINER_PNLTIMESET] = cui_PnlTimeSet;
    children[UI_COMP_DATE_TIME1_PNLMANUALSETTING_PNLTIMECONTAINER_PNLTIMESET_HOUR_CFG2] = cui_hour_cfg2;
    children[UI_COMP_DATE_TIME1_PNLMANUALSETTING_PNLTIMECONTAINER_PNLTIMESET_TIME_LABEL3] = cui_time_label3;
    children[UI_COMP_DATE_TIME1_PNLMANUALSETTING_PNLTIMECONTAINER_PNLTIMESET_MIN_CFG2] = cui_min_cfg2;
    children[UI_COMP_DATE_TIME1_PNLMANUALSETTING_PNLTIMECONTAINER_PNLTIMESET_TIME_LABEL4] = cui_time_label4;
    children[UI_COMP_DATE_TIME1_PNLMANUALSETTING_PNLTIMECONTAINER_PNLTIMESET_SEC_CFG1] = cui_sec_cfg1;
    children[UI_COMP_DATE_TIME1_PNLMANUALSETTING_PNLTIMECONTAINER_BUTTON1] = cui_Button1;
    children[UI_COMP_DATE_TIME1_PNLMANUALSETTING_PNLTIMECONTAINER_BUTTON1_LABEL1] = cui_Label1;
    lv_obj_add_event_cb(cui_date_time1, get_component_child_event_cb, LV_EVENT_GET_COMP_CHILD, children);
    lv_obj_add_event_cb(cui_date_time1, del_component_child_event_cb, LV_EVENT_DELETE, children);
    ui_comp_date_time1_create_hook(cui_date_time1);
    return cui_date_time1;
}

