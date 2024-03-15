/**
 * @file boxes.h
 * @date  12 March 2024

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 */

#ifndef BOXES_H
#define BOXES_H

#ifdef __cplusplus
extern "C" {
#endif
#include "esp_log.h"
#include "lvgl.h"
#include <cJSON.h>
typedef union {
    struct {
        uint16_t x;
        uint16_t y;
        uint16_t w;
        uint16_t h;
        uint16_t score;
        uint16_t target;
    };
    uint16_t boxArray[6];
} boxes_t;

// void draw_one_box(lv_obj_t* parent, const boxes_t box, lv_color_t color);
void draw_one_box(lv_obj_t* canvas, const boxes_t box, lv_color_t color);
// void process_and_draw_boxes(cJSON* receivedJson, lv_obj_t* image_obj);
void process_and_draw_boxes(cJSON* receivedJson, lv_obj_t* canvas);
void init_boxes_app();

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*BOXES_H*/