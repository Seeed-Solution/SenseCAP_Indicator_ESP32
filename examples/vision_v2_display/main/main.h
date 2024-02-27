/**
 * @file main.h
 * @date  23 February 2024

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 */

#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>

#include "esp_timer.h"
#include "esp_err.h"
#include "esp_log.h"
#include "bsp_board.h"

#include "esp_event_base.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char *img_base64; // 存储Base64编码的图像数据
    int img_size;
} img_event_data_t;

ESP_EVENT_DECLARE_BASE(VIEW_EVENT_BASE);
extern esp_event_loop_handle_t view_event_handle;

enum {
    VIEW_EVENT_IMG = 0,      
    VIEW_EVENT_ALL,
};
#define BUF_SIZE (15 * 1024)
#define MAX_JSON_SIZE (BUF_SIZE * 2)

#define JsonQueue_SIZE (30)
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*MAIN_H*/