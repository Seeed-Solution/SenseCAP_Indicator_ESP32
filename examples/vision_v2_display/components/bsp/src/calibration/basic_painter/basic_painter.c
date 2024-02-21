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
# include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "basic_painter.h"

static const char *TAG = "basic painter";
static painter_config_t g_config;

#define PAINTER_CHECK(a, str)  if(!(a)) {                               \
        ESP_LOGE(TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);   \
        return ;                                                           \
    }

static uint16_t g_point_color = COLOR_BLACK;
static uint16_t g_back_color  = COLOR_WHITE;

esp_err_t painter_init(painter_config_t *config)
{
    g_config = *config;
    return ESP_OK;
}

void painter_set_point_color(uint16_t color)
{
    g_point_color = color;
}

uint16_t painter_get_point_color(void)
{
    return g_point_color;
}

void painter_set_back_color(uint16_t color)
{
    g_back_color = color;
}

uint16_t painter_get_back_color(void)
{
    return g_back_color;
}

void painter_clear(uint16_t color)
{
    uint16_t *buffer = malloc(g_config.info.width * sizeof(uint16_t));
    PAINTER_CHECK(NULL != buffer, "Memory not enough");

    for (size_t i = 0; i <g_config.info.width; i++) {
        buffer[i] = color;
    }

    for (int y = 0; y < g_config.info.height; y++) {
        g_config.draw_bitmap(0, y, g_config.info.width, 1, buffer);
    }

    free(buffer);
}

void painter_draw_char(int x, int y, char ascii_char, const font_t *font, uint16_t color)
{
    PAINTER_CHECK(ascii_char >= ' ', "ACSII code invalid");
    PAINTER_CHECK(NULL != font, "Font pointer invalid");
    int i, j;
    uint16_t char_size = font->Height * (font->Width / 8 + (font->Width % 8 ? 1 : 0));
    unsigned int char_offset = (ascii_char - ' ') * char_size;
    const unsigned char *ptr = &font->table[char_offset];
    uint16_t buf[18 * 25];
    PAINTER_CHECK(font->Height * font->Width * sizeof(uint16_t) <= sizeof(buf), "Font size is too large");
    int ox = 0;
    int oy = 0;

    for (i = 0; i < font->Width * font->Height; i++) {
        buf[i] = g_back_color;
    }
    for (j = 0; j < char_size; j++) {
        uint8_t temp = ptr[j];
        for (i = 0; i < 8; i++) {
            if (temp & 0x80) {
                buf[ox + (font->Width * oy)] = g_point_color;
            }
            temp <<= 1;
            ox++;
            if (ox == font->Width) {
                ox = 0;
                oy++;
                break;
            }
        }
    }
    g_config.draw_bitmap(x, y, font->Width, font->Height, buf);         // Draw NxN char
}

void painter_draw_string(int x, int y, const char *text, const font_t *font, uint16_t color)
{
    PAINTER_CHECK(NULL != text, "string pointer invalid");
    PAINTER_CHECK(NULL != font, "Font pointer invalid");
    const char *p_text = text;
    uint16_t x0 = x;
    uint16_t y0 = y;

    while (*p_text != 0) {
        if (x > (x0 + g_config.info.width - font->Width)) {
            y += font->Height;
            x = x0;
        }
        if (y > (y0 + g_config.info.height - font->Height)) {
            break;
        }
        if (*p_text == '\n') {
            y += font->Height;
            x = x0;
        } else {
            painter_draw_char(x, y, *p_text, font, color);
        }
        x += font->Width;
        p_text++;
    }
}

void painter_draw_num(int x, int y, uint32_t num, uint8_t len, const font_t *font, uint16_t color)
{
    PAINTER_CHECK(len < 10, "The length of the number is too long");
    PAINTER_CHECK(NULL != font, "Font pointer invalid");
    char buf[10] = {0};
    int8_t num_len;

    itoa(num, buf, 10);
    num_len = strlen(buf);
    x += (font->Width * (len - 1));

    for (size_t i = 0; i < len; i++) {
        if (i < num_len) {
            painter_draw_char(x, y, buf[i], font, color);
        } else {
            painter_draw_char(x, y, '0', font, color);
        }

        x -= font->Width;
    }
}

void painter_draw_image(int x, int y, int width, int height, uint16_t *img)
{
    PAINTER_CHECK(NULL != img, "Image pointer invalid");
    g_config.draw_bitmap(x, y, width, height, img);
}

void painter_draw_horizontal_line(int x, int y, int line_length, uint16_t color)
{
    int i;

    for (i = x; i < x + line_length; i++) {
        g_config.draw_bitmap(i, y, 1, 1, &color);
    }
}

void painter_draw_vertical_line(int x, int y, int line_length, uint16_t color)
{
    int i;

    for (i = y; i < y + line_length; i++) {
        g_config.draw_bitmap(x, i, 1, 1, &color);
    }
}

void painter_draw_line(int x1, int y1, int x2, int y2, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;

    if (delta_x > 0) {
        incx = 1;    //set direction
    } else if (delta_x == 0) {
        incx = 0;    //vertical line
    } else {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0) {
        incy = 1;
    } else if (delta_y == 0) {
        incy = 0;    //horizontal line
    } else {
        incy = -1;
        delta_y = -delta_y;
    }

    if (delta_x > delta_y) {
        distance = delta_x;
    } else {
        distance = delta_y;
    }

    for (t = 0; t <= distance + 1; t++) {
        g_config.draw_bitmap(uRow, uCol, 1, 1, &color);
        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance) {
            xerr -= distance;
            uRow += incx;
        }

        if (yerr > distance) {
            yerr -= distance;
            uCol += incy;
        }
    }
}

void painter_draw_rectangle(int x0, int y0, int x1, int y1, uint16_t color)
{
    int min_x, min_y, max_x, max_y;
    min_x = x1 > x0 ? x0 : x1;
    max_x = x1 > x0 ? x1 : x0;
    min_y = y1 > y0 ? y0 : y1;
    max_y = y1 > y0 ? y1 : y0;

    painter_draw_horizontal_line(min_x, min_y, max_x - min_x + 1, color);
    painter_draw_horizontal_line(min_x, max_y, max_x - min_x + 1, color);
    painter_draw_vertical_line(min_x, min_y, max_y - min_y + 1, color);
    painter_draw_vertical_line(max_x, min_y, max_y - min_y + 1, color);
}

void painter_draw_filled_rectangle(int x0, int y0, int x1, int y1, uint16_t color)
{
    int min_x, min_y, max_x, max_y;
    int i;
    min_x = x1 > x0 ? x0 : x1;
    max_x = x1 > x0 ? x1 : x0;
    min_y = y1 > y0 ? y0 : y1;
    max_y = y1 > y0 ? y1 : y0;

    for (i = min_x; i <= max_x; i++) {
        painter_draw_vertical_line(i, min_y, max_y - min_y + 1, color);
    }
}

void painter_draw_circle(int x, int y, int radius, uint16_t color)
{
    /* Bresenham algorithm */
    int x_pos = -radius;
    int y_pos = 0;
    int err = 2 - 2 * radius;
    int e2;

    do {
        g_config.draw_bitmap(x - x_pos, y + y_pos, 1, 1, &color);
        g_config.draw_bitmap(x + x_pos, y + y_pos, 1, 1, &color);
        g_config.draw_bitmap(x + x_pos, y - y_pos, 1, 1, &color);
        g_config.draw_bitmap(x - x_pos, y - y_pos, 1, 1, &color);
        e2 = err;

        if (e2 <= y_pos) {
            err += ++y_pos * 2 + 1;

            if (-x_pos == y_pos && e2 <= x_pos) {
                e2 = 0;
            }
        }

        if (e2 > x_pos) {
            err += ++x_pos * 2 + 1;
        }
    } while (x_pos <= 0);
}

void painter_draw_filled_circle(int x, int y, int radius, uint16_t color)
{
    /* Bresenham algorithm */
    int x_pos = -radius;
    int y_pos = 0;
    int err = 2 - 2 * radius;
    int e2;

    do {
        g_config.draw_bitmap(x - x_pos, y + y_pos, 1, 1, &color);
        g_config.draw_bitmap(x + x_pos, y + y_pos, 1, 1, &color);
        g_config.draw_bitmap(x + x_pos, y - y_pos, 1, 1, &color);
        g_config.draw_bitmap(x - x_pos, y - y_pos, 1, 1, &color);
        painter_draw_horizontal_line(x + x_pos, y + y_pos, 2 * (-x_pos) + 1, color);
        painter_draw_horizontal_line(x + x_pos, y - y_pos, 2 * (-x_pos) + 1, color);
        e2 = err;

        if (e2 <= y_pos) {
            err += ++y_pos * 2 + 1;

            if (-x_pos == y_pos && e2 <= x_pos) {
                e2 = 0;
            }
        }

        if (e2 > x_pos) {
            err += ++x_pos * 2 + 1;
        }
    } while (x_pos <= 0);
}

