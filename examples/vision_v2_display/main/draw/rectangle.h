/**
 * @file ractangle.h
 * @date  27 February 2024

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 */

#ifndef RACTANGLE_H
#define RACTANGLE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "esp_log.h"
#include "lvgl.h"

void draw_one_rectangle(lv_obj_t *parent, int x, int y, int w, int h, lv_color_t color, int tarStr, int score);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*RACTANGLE_H*/