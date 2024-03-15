// Copyright 2015-2022 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "bsp_board.h"
#include "esp_event.h"
#include "esp_event_base.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lv_port.h"
#include "ui/ui.h"

#include "esp32_rp2040.h"
#include "main.h"

#include "cJSON.h"

#include "mbedtls/base64.h"
#include "string.h"

#define VERSION "v1.0.0"
#define LOG_MEM_INFO 0
#define SENSECAP \
    "\n\
   _____                      _________    ____         \n\
  / ___/___  ____  ________  / ____/   |  / __ \\       \n\
  \\__ \\/ _ \\/ __ \\/ ___/ _ \\/ /   / /| | / /_/ /   \n\
 ___/ /  __/ / / (__  )  __/ /___/ ___ |/ ____/         \n\
/____/\\___/_/ /_/____/\\___/\\____/_/  |_/_/           \n\
--------------------------------------------------------\n\
 Version: %s %s %s\n\
--------------------------------------------------------\n\
"

#include "app_boxes.h"
#include "app_image.h"
#include "app_keypoints.h"

static const char* TAG = "app_main";

ESP_EVENT_DEFINE_BASE(VIEW_EVENT_BASE);
esp_event_loop_handle_t view_event_handle;

void JsonQueue_processing_task(void* pvParameters);
void parse_model_name(cJSON* json, lv_obj_t* ui_Label) {
    cJSON* model_name = cJSON_GetObjectItem(json, "model_name");
    if (cJSON_IsString(model_name) && model_name->valuestring != NULL) {
        // ESP_LOGI(TAG, "model_name: %s", model_name->valuestring);
        lv_port_sem_take();
        lv_label_set_text(ui_Label, model_name->valuestring);
        lv_port_sem_give();
    }
    // cJSON* model_name = cJSON_GetObjectItem(receivedJson, "model_name");
    // if (cJSON_IsString(model_name)) {
    //     ESP_LOGI(TAG, "model_name: %s", model_name->valuestring);
    //     lv_port_sem_take();
    //     lv_label_set_text(ui_Label1, model_name->valuestring);
    //     lv_port_sem_give();
    // }
}
// lv_obj_t* canvas;
lv_obj_t* canvas_left;
lv_obj_t* canvas_right;

uint8_t* cbuf_left;
uint8_t* cbuf_right;

void app_main(void) {
    ESP_LOGI(TAG, "system start");

    ESP_LOGI("", SENSECAP, VERSION, __DATE__, __TIME__);

    ESP_ERROR_CHECK(bsp_board_init());
    lv_port_init();

    esp_event_loop_args_t view_event_task_args = {.queue_size = 10,
                                                  .task_name = "view_event_task",
                                                  .task_priority = uxTaskPriorityGet(NULL),
                                                  .task_stack_size = 1024 * 5,
                                                  .task_core_id = tskNO_AFFINITY};
    ESP_ERROR_CHECK(esp_event_loop_create(&view_event_task_args, &view_event_handle));

#if CONFIG_LCD_AVOID_TEAR
    ESP_LOGI(TAG, "Avoid lcd tearing effect");
#if CONFIG_LCD_LVGL_FULL_REFRESH
    ESP_LOGI(TAG, "LVGL full-refresh");
#elif CONFIG_LCD_LVGL_DIRECT_MODE
    ESP_LOGI(TAG, "LVGL direct-mode");
#endif
#endif

    lv_port_sem_take();
    /* (must be 480*800, set LCD_EVB_SCREEN_ROTATION_90 in menuconfig)*/
    ui_init();

    lv_port_sem_give();

    ESP_LOGI(TAG, "Out of ui_init()");

    esp32_rp2040_init(); // another task to handle the RP2040 Serial data
    ESP_LOGI(TAG, "RP2040 init success");

    lv_port_sem_take();

    cbuf_left = (uint8_t*)heap_caps_malloc(LV_IMG_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT), MALLOC_CAP_SPIRAM);
    cbuf_right = (uint8_t*)heap_caps_malloc(LV_IMG_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT), MALLOC_CAP_SPIRAM);
    canvas_left = lv_canvas_create(lv_scr_act());
    canvas_right = lv_canvas_create(lv_scr_act());
    if (cbuf_left != NULL) {
        lv_canvas_set_buffer(canvas_left, cbuf_left, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
    } else {
        ESP_LOGI(TAG, "Memory for canvas buffer allocated");
    }
    if (cbuf_right != NULL) {
        lv_canvas_set_buffer(canvas_right, cbuf_right, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
    } else {
        ESP_LOGI(TAG, "Memory for canvas buffer allocated");
    }
    // lv_obj_center(canvas);
    // lv_obj_align(canvas, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(canvas_left, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_align(canvas_right, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_canvas_fill_bg(canvas_left, lv_palette_darken(LV_PALETTE_NONE, 3), LV_OPA_COVER);
    lv_canvas_fill_bg(canvas_right, lv_palette_main(LV_PALETTE_GREY), LV_OPA_COVER);

    // lv_draw_label_dsc_t label_dsc;
    // lv_draw_label_dsc_init(&label_dsc);
    // label_dsc.color = lv_palette_main(LV_PALETTE_ORANGE);
    // lv_canvas_draw_text(canvas, 40, 20, 100, &label_dsc, "Some text on text canvas");
    init_image();
    init_keypoints_app();
    init_boxes_app();
    lv_port_sem_give();

    xTaskCreatePinnedToCore(JsonQueue_processing_task, // 任务函数
                            "JsonQueueTask",           // 任务名称
                            1024 * 8,                  // 堆栈大小
                            NULL,                      // 传递给任务的参数
                            5,                         // 任务优先级
                            NULL,                      // 任务句柄
                            1                          // 任务固定在核心1
    );
    ESP_LOGI(TAG, "Queue create success");
    // while (1) {
    //     vTaskDelay(pdMS_TO_TICKS(10000));
    // }
}

void JsonQueue_processing_task(void* pvParameters) {
    char* receivedStr = NULL;
    for (;;) {
        if (xQueueReceive(JsonQueue, &receivedStr, portMAX_DELAY) == pdPASS && receivedStr != NULL) {

            cJSON* receivedJson = cJSON_Parse(receivedStr);
            if (receivedJson != NULL) {
                // Get the model name
                parse_model_name(receivedJson, ui_Model_Name);

                // // Display the image
                display_image_from_json(receivedJson, canvas_left);

                // ///// 画框的函数调用
                process_and_draw_boxes(receivedJson, canvas_left);

                /// Keypoints 的处理
                lv_port_sem_take();
                lv_canvas_fill_bg(canvas_right, lv_palette_main(LV_PALETTE_NONE), LV_OPA_COVER);
                lv_port_sem_give();
                vTaskDelay(1);
                keypoints_t* keypoints_array;
                int keypoints_count;
                if (get_keypoints(receivedJson, &keypoints_array, &keypoints_count)) {

                    if (keypoints_array == NULL) {
                        ESP_LOGE(TAG, "keypoints_array is NULL");
                        continue;
                    }
                    lv_port_sem_take();
                    draw_one_box(canvas_left, keypoints_array->box, lv_color_make(113, 235, 52));
                    // draw_one_box(canvas_right, keypoints_array[0].box, lv_color_make(113, 235, 52));
                    draw_keypoints(canvas_left, keypoints_array, keypoints_count);  // boxes and keypoints
                    draw_keypoints(canvas_right, keypoints_array, keypoints_count); // boxes and keypoints
                    lv_port_sem_give();
                    vTaskDelay(1);
                    for (int i = 0; i < keypoints_count; i++) {
                        free_keypoints_array(keypoints_array[i].points);
                    }
                    free(keypoints_array);
                }

                cJSON_Delete(receivedJson);
            }
            free(receivedStr); // Free the received string after processing.
            vTaskDelay(1);
            receivedStr = NULL;
        }
    }
}
