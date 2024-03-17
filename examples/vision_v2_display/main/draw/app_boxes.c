/**
 * @file app_boxes.c
 * @date  12 March 2024

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 */

#include "app_boxes.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
static const char* TAG = "app_boxes";

static lv_draw_rect_dsc_t rect_dsc;

void init_boxes_app() {
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_opa = LV_OPA_TRANSP;

    rect_dsc.border_color = lv_palette_main(LV_PALETTE_GREEN);
    rect_dsc.border_opa = LV_OPA_COVER;
    rect_dsc.border_width = 2;
    // rect_dsc.border_opa = LV_OPA_COVER, rect_dsc.border_width = 2, rect_dsc.border_side = LV_BORDER_SIDE_FULL,
    // rect_dsc.shadow_color = lv_palette_main(LV_PALETTE_GREY), rect_dsc.shadow_opa = LV_OPA_COVER,
    // rect_dsc.shadow_width = 1;
}
void draw_one_box(lv_obj_t* canvas, const boxes_t box, lv_color_t color) {
    int w = box.w;
    int h = box.h;
    int x = box.x - box.w / 2;
    int y = box.y - box.h / 2;

    // ESP_LOGI(TAG, "Drawing box at x: %d, y: %d, w: %d, h: %d", x, y, w, h);
    rect_dsc.border_color = color;
    lv_canvas_draw_rect(canvas, x, y, w, h, &rect_dsc); 

    // lv_obj_t* rect = lv_obj_create(parent);
    // if (rect == NULL) {
    //     ESP_LOGE(TAG, "Failed to create rectangle");
    //     return;
    // }
    // lv_obj_set_size(rect, w, h);
    // lv_obj_set_pos(rect, x, y);
    // lv_obj_set_style_border_color(rect, color, 0);
    // lv_obj_set_style_border_width(rect, 3, 0);
    // lv_obj_set_style_bg_opa(rect, LV_OPA_TRANSP, 0); // Make background transparent
}

void parse_box_from_cjson(cJSON* boxJson, boxes_t* box) {
    if (!cJSON_IsArray(boxJson) || box == NULL) {
        ESP_LOGE(TAG, "Invalid input to parse_box_from_cjson");
        return;
    }

    // 请确保按照正确的顺序从你的JSON中读取值
    // box->boxArray[0] = cJSON_GetArrayItem(boxJson, 0)->valueint; // x
    // box->boxArray[1] = cJSON_GetArrayItem(boxJson, 1)->valueint; // y
    // box->boxArray[2] = cJSON_GetArrayItem(boxJson, 2)->valueint; // w
    // box->boxArray[3] = cJSON_GetArrayItem(boxJson, 3)->valueint; // h
    // box->boxArray[4] = cJSON_GetArrayItem(boxJson, 4)->valueint; // score
    // box->boxArray[5] = cJSON_GetArrayItem(boxJson, 5)->valueint; // target
    for (int i = 0; i < cJSON_GetArraySize(boxJson); i++) {
        box->boxArray[i] = cJSON_GetArrayItem(boxJson, i)->valueint;
    }
}

/**
 * @brief
 * process_and_draw_boxes
 */
// void process_and_draw_boxes(cJSON* receivedJson, lv_obj_t* image_obj) {
//     cJSON* boxesJson = cJSON_GetObjectItem(receivedJson, "boxes");
//     if (!cJSON_IsArray(boxesJson)) {
//         // ESP_LOGE(TAG, "boxes is not an array");
//         return;
//     }

//     cJSON* boxJson;
//     lv_port_sem_take();
//     cJSON_ArrayForEach(boxJson, boxesJson) {
//         boxes_t box;
//         parse_box_from_cjson(boxJson, &box);
//         draw_one_box(image_obj, box, lv_color_make(113, 235, 52));
//         vTaskDelay(1);
//     }
//     lv_port_sem_give();
// }
void process_and_draw_boxes(cJSON* receivedJson, lv_obj_t* canvas) {
    cJSON* boxesJson = cJSON_GetObjectItem(receivedJson, "boxes");
    if (!cJSON_IsArray(boxesJson)) {
        return;
    }

    lv_port_sem_take();
    cJSON* boxJson = NULL;
    cJSON_ArrayForEach(boxJson, boxesJson) {
        boxes_t box;
        parse_box_from_cjson(boxJson, &box);
        draw_one_box(canvas, box, lv_color_make(113, 235, 52));
        vTaskDelay(1);
    }
    lv_port_sem_give();
}

// cJSON *boxes = cJSON_GetObjectItem(receivedJson, "boxes");
// if (cJSON_IsArray(boxes)) {
//     cJSON *box_json;
//     ESP_LOGI(TAG, "sizeArray: %d", cJSON_GetArraySize(boxes));
//     lv_port_sem_take();
//     cJSON_ArrayForEach(box_json, boxes)
//     {
//         boxes_t one_box;
//         parse_box(box_json, &one_box);
//         draw_one_box(ui_v2_image, one_box, lv_color_make(113, 235, 52));

//         // ESP_LOGI(TAG, "Drawn rectangle %d", _);
//         vTaskDelay(1);
//     }
//     lv_port_sem_give();
// }