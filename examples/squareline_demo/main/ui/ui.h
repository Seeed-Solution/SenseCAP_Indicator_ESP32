// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.2.1
// LVGL VERSION: 8.3.4
// PROJECT: SquareLine_Project

#ifndef _SQUARELINE_PROJECT_UI_H
#define _SQUARELINE_PROJECT_UI_H

#ifdef __cplusplus
extern "C" {
#endif

    #include "lvgl/lvgl.h"

extern lv_obj_t *ui_scren_time;
extern lv_obj_t *ui_background;
extern lv_obj_t *ui_hour;
extern lv_obj_t *ui_hour_cfg;
extern lv_obj_t *ui_min;
extern lv_obj_t *ui_min_cfg;
extern lv_obj_t *ui_location;
extern lv_obj_t *ui_city;
extern lv_obj_t *ui_location_Icon;
extern lv_obj_t *ui_date_panel;
extern lv_obj_t *ui_date;
extern lv_obj_t *ui_Label1;
extern lv_obj_t *ui_adorn;


LV_IMG_DECLARE( ui_img_background_png);   // assets/background.png
LV_IMG_DECLARE( ui_img_location2_png);   // assets/location2.png


LV_FONT_DECLARE( ui_font_Font1);


void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
