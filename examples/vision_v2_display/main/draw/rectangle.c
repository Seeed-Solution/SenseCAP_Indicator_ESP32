#include "rectangle.h"

static const char *TAG = "view_rect";

void draw_one_rectangle(lv_obj_t *parent, int x, int y, int w, int h, lv_color_t color, int tarStr, int score)
{
    // Adjust coordinates to center the rectangle
    x = x - w / 2;
    y = y - h / 2;

    lv_obj_t *rect = lv_obj_create(parent);
    if (rect == NULL) {
        ESP_LOGE(TAG, "Failed to create rectangle");
        return;
    }
    lv_obj_set_size(rect, w, h);
    lv_obj_set_pos(rect, x, y);
    lv_obj_set_style_border_color(rect, color, 0);
    lv_obj_set_style_border_width(rect, 4, 0);
    lv_obj_set_style_bg_opa(rect, LV_OPA_TRANSP, 0); // Make background transparent
}