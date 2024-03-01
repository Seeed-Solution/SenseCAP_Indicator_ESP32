/**
 * @file image.h
 * @date  27 February 2024

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 */

#ifndef IMAGE_H
#define IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "esp_log.h"
#define DECODED_STR_MAX_SIZE (7 * 1024)
void display_one_image(lv_obj_t *image, const unsigned char *p_data);
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*IMAGE_H*/