/**
 * @file app_keypoints.h
 * @date  12 March 2024

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 */

#ifndef APP_KEYPOINTS_H
#define APP_KEYPOINTS_H

#ifdef __cplusplus
extern "C" {
#endif
#include "esp_log.h"
#include "lvgl.h"
#include "app_boxes.h"

typedef union {
    struct {
        uint16_t x;
        uint16_t y;
    };
    uint16_t position[2];
} keypoint_t;

typedef struct {
    boxes_t box; // Assuming two points: top-left and bottom-right for the box
    keypoint_t* points; // Array of points
    size_t points_count; // Number of points
} keypoints_t;

keypoint_t* create_keypoints_array(size_t count);
// 释放关键点数组
void free_keypoints_array(keypoint_t* array);
/**
 * @brief
 * {"keypoints":
*    [ // the first person : the first keypoint
*       {
            "box":[0,95,120,120,239,240],
            "points":[[127,77],[197,216]]
        }
*    ]
 * }
 */
void init_keypoints_app();
bool get_keypoints(cJSON* receivedJson, keypoints_t** keypoints_array, int* keypoints_count);
void draw_one_point(lv_obj_t *parent, const keypoint_t point, lv_color_t color);
void draw_keypoints(lv_obj_t* canvas, const keypoints_t* keypoints);
void draw_keypoints_array(lv_obj_t* canvas, const keypoints_t* keypoints, size_t count);
// typedef struct
// {
//     boxes_t box;
//     std::vector<point_t> points;
// } keypoints_t;


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*APP_KEYPOINTS_H*/