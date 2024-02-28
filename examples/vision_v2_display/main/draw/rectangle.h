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

// #define MAX_RECTS 10 // 假设我们最多同时需要10个矩形

// typedef struct {
//     lv_obj_t *rects[MAX_RECTS];
//     bool      is_used[MAX_RECTS];
// } RectPool;


// void      init_rect_pool(lv_obj_t *parent);
// lv_obj_t *alloc_rect();
// void      free_rect(lv_obj_t *rect);
void      delete_all_rects();

// void draw_one_rectangle(lv_obj_t *parent, int x, int y, int w, int h, lv_color_t color, int tarStr, int score);
void draw_one_rectangle(lv_obj_t *parent, int x, int y, int w, int h, lv_color_t color, int tarStr, int score);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*RACTANGLE_H*/