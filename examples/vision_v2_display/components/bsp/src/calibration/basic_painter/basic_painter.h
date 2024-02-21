// Copyright 2020 Espressif Systems (Shanghai) Co. Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _BASIC_PAINTER_H
#define _BASIC_PAINTER_H

#include "painter_fonts.h"

/* Color definitions, RGB565 format */
#define COLOR_BLACK       0x0000
#define COLOR_NAVY        0x000F
#define COLOR_DARKGREEN   0x03E0
#define COLOR_DARKCYAN    0x03EF
#define COLOR_MAROON      0x7800
#define COLOR_PURPLE      0x780F
#define COLOR_OLIVE       0x7BE0
#define COLOR_LIGHTGREY   0xC618
#define COLOR_DARKGREY    0x7BEF
#define COLOR_BLUE        0x001F
#define COLOR_GREEN       0x07E0
#define COLOR_CYAN        0x07FF
#define COLOR_RED         0xF800
#define COLOR_MAGENTA     0xF81F
#define COLOR_YELLOW      0xFFE0
#define COLOR_WHITE       0xFFFF
#define COLOR_ORANGE      0xFD20
#define COLOR_GREENYELLOW 0xAFE5
#define COLOR_PINK        0xF81F
#define COLOR_SILVER      0xC618
#define COLOR_GRAY        0x8410
#define COLOR_LIME        0x07E0
#define COLOR_TEAL        0x0410
#define COLOR_FUCHSIA     0xF81F
#define COLOR_ESP_BKGD    0xD185

#ifdef __cplusplus
extern "C" {
#endif

typedef esp_err_t (*painter_draw_bitmap_t)(int x1, int y1, int w, int h, const void *p_data);
typedef esp_err_t (*painter_draw_pixel_t)(int x1, int y1, const void *p_data);

typedef struct {
    struct {
        int width;
        int height;
    } info;
    painter_draw_bitmap_t draw_bitmap;
} painter_config_t;

/**
 * @brief LCD paint initial
 *
 * @param driver Output a lcd_driver_fun_t structure.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL Failed
 */
esp_err_t painter_init(painter_config_t *config);

/**
 * @brief Set point color
 *
 * @param color point color
 *
 */
void painter_set_point_color(uint16_t color);

/**
 * @brief Get point color
 *
 * @return point color
 *
 */
uint16_t painter_get_point_color(void);

/**
 * @brief Set background color
 *
 * @param color background color
 *
 */
void painter_set_back_color(uint16_t color);

/**
 * @brief Get background color
 *
 * @return background color
 *
 */
uint16_t painter_get_back_color(void);

/**
 * @brief Clear screen to one color
 *
 * @param color Set the screen to this color
 */
void painter_clear(uint16_t color);

/**
 * @brief Draw a character on screen
 *
 * @param x Starting point in X direction
 * @param y Starting point in Y direction
 * @param ascii_char ASCII characters to display
 * @param font Pointer to a font
 * @param color Color to display
 */
void painter_draw_char(int x, int y, char ascii_char, const font_t* font, uint16_t color);

/**
 * @brief Draw a string on screen
 *
 * @param x Starting point in X direction
 * @param y Starting point in Y direction
 * @param text ASCII string to display
 * @param font Pointer to a font
 * @param color Color to display
 */
void painter_draw_string(int x, int y, const char* text, const font_t* font, uint16_t color);

/**
 * @brief Draw a number on screen
 *
 * @param x Starting point in X direction
 * @param y Starting point in Y direction
 * @param num Number to display
 * @param len The length of the number
 * @param font Pointer to a font
 * @param color Color to display
 */
void painter_draw_num(int x, int y, uint32_t num, uint8_t len, const font_t *font, uint16_t color);

/**
 * @brief Draw a float number on screen
 *
 * @param x Starting point in X direction
 * @param y Starting point in Y direction
 * @param num Number to display
 * @param len The length of the number
 * @param point Position of decimal point
 * @param font Pointer to a font
 * @param color Color to display
 */
void painter_draw_float(int x, int y, uint32_t num, uint8_t len, uint8_t point, const font_t *font, uint16_t color);

/**
 * @brief Draw a image on screen
 *
 * @param x Starting point in X direction
 * @param y Starting point in Y direction
 * @param width width of image
 * @param height height of image
 * @param img pointer to bitmap array
 */
void painter_draw_image(int x, int y, int width, int height, uint16_t *img);

/**
 * @brief Draw a horizontal line on screen
 *
 * @param x Starting point in X direction
 * @param y Starting point in Y direction
 * @param line_length length of line
 * @param color Color to display
 */
void painter_draw_horizontal_line(int x, int y, int line_length, uint16_t color);

/**
 * @brief Draw a vertical line on screen
 *
 * @param x Starting point in X direction
 * @param y Starting point in Y direction
 * @param line_length length of line
 * @param color Color to display
 */
void painter_draw_vertical_line(int x, int y, int line_length, uint16_t color);

/**
 * @brief draw a line on sccreen
 *
 * @param x0 Starting point in X direction
 * @param y0 Starting point in Y direction
 * @param x1 End point in X direction
 * @param y1 End point in Y direction
 * @param color Color to display
 */
void painter_draw_line(int x0, int y0, int x1, int y1, uint16_t color);

/**
 * @brief draw a rectangle on sccreen
 *
 * @param x0 Starting point in X direction
 * @param y0 Starting point in Y direction
 * @param x1 End point in X direction
 * @param y1 End point in Y direction
 * @param color Color to display
 */
void painter_draw_rectangle(int x0, int y0, int x1, int y1, uint16_t color);

/**
 * @brief draw a filled rectangle on sccreen
 *
 * @param x0 Starting point in X direction
 * @param y0 Starting point in Y direction
 * @param x1 End point in X direction
 * @param y1 End point in Y direction
 * @param color Color to display
 */
void painter_draw_filled_rectangle(int x0, int y0, int x1, int y1, uint16_t color);

/**
 * @brief Draw a hollow circle on screen
 *
 * @param x Starting point in X direction
 * @param y Starting point in Y direction
 * @param radius radius of circle
 * @param color Color to display
 */
void painter_draw_circle(int x, int y, int radius, uint16_t color);

/**
 * @brief Draw a filled circle on screen
 *
 * @param x Starting point in X direction
 * @param y Starting point in Y direction
 * @param radius radius of circle
 * @param color Color to display
 */
void painter_draw_filled_circle(int x, int y, int radius, uint16_t color);


#ifdef __cplusplus
}
#endif

#endif
