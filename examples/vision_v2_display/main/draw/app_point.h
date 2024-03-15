/**
 * @file app_point.h
 * @date  12 March 2024

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 */

#ifndef APP_POINT_H
#define APP_POINT_H

#ifdef __cplusplus
extern "C" {
#endif
#include "esp_log.h"
#include "lvgl.h"
#include "cJSON.h"
typedef union {
    struct
    {
        uint16_t x;
        uint16_t y;
        // uint16_t z; // Not used
        uint16_t score;
        uint16_t target;
    };
    uint16_t pointArray[4];
} point_t;

bool get_point(cJSON *json, point_t *point);
void draw_one_point(lv_obj_t *parent, const point_t point, lv_color_t color);

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
typedef struct {

} keypoint_t;


// typedef struct
// {
//     boxes_t box;
//     std::vector<point_t> points;
// } keypoints_t;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*APP_POINT_H*/