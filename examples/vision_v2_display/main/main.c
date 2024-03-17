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
#include "indicator_btn.h"

static const char* TAG = "app_main";

ESP_EVENT_DEFINE_BASE(VIEW_EVENT_BASE);
esp_event_loop_handle_t view_event_handle;

void JsonQueue_processing_task(void* pvParameters);

// lv_obj_t* canvas;
lv_obj_t* canvas_left;
lv_obj_t* canvas_right;

uint8_t* cbuf_left;
uint8_t* cbuf_right;

#define BASE64_IMAGE_MAX_SIZE (18 * 1024)
static unsigned char imageData[BASE64_IMAGE_MAX_SIZE] = {0};
#define DECODED_IMAGE_MAX_SIZE (15 * 1024)
static unsigned char jpegImage[DECODED_IMAGE_MAX_SIZE + 1]; // 静态分配解码后的图片数据缓冲区
static void __json_event_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data) {
    lv_port_sem_take();
    switch (id) {
    case VIEW_EVENT_IMG: {
        char* img_data = (char*)event_data;
        size_t jpegImageSize = decode_base64_image(img_data, jpegImage);
        if (jpegImageSize) { // !=0
        } else {
            ESP_LOGE(TAG, "Failed to decode image");
        }

        update_canvas_with_image(canvas_left, jpegImage, jpegImageSize);
        break;
    }
    case VIEW_EVENT_MODEL_NAME: {
        char* p_data = ((char*)event_data); // Only one time, don't need to care about the memory mutex
        lv_label_set_text(ui_Model_Name, p_data);
        break;
    }
    case VIEW_EVENT_BOXES: {
        boxes_t* box = (boxes_t*)event_data;
        draw_one_box(canvas_left, *box, lv_color_make(113, 235, 52));
        break;
    }
    case VIEW_EVENT_KEYPOINTS: {
        keypoints_t* keypoints_array = (keypoints_t*)event_data;

        lv_canvas_fill_bg(canvas_right, lv_palette_main(LV_PALETTE_NONE), LV_OPA_COVER);

        draw_keypoints(canvas_left, keypoints_array);  // boxes and keypoints
        draw_keypoints(canvas_right, keypoints_array); // boxes and keypoints

        draw_one_box(canvas_left, keypoints_array->box, lv_color_make(113, 235, 52));

        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_BOXES, &keypoints_array->box,
                          sizeof(keypoints_array->box), portMAX_DELAY);

        break;
    }
    default:
        break;
    }
    lv_port_sem_give();
}

void app_main(void) {
    // ESP_LOGI(TAG, "system start");

    // ESP_LOGI("", SENSECAP, VERSION, __DATE__, __TIME__);

    ESP_ERROR_CHECK(bsp_board_init());
    lv_port_init();
    indicator_btn_init();
    esp_event_loop_args_t view_event_task_args = {.queue_size = 10, // json_event_task_args
                                                  .task_name = "view_event_task",
                                                  .task_priority = uxTaskPriorityGet(NULL),
                                                  .task_stack_size = 1024 * 5,
                                                  .task_core_id = tskNO_AFFINITY};
    ESP_ERROR_CHECK(esp_event_loop_create(&view_event_task_args, &view_event_handle));

    lv_port_sem_take();
    /* (must be 480*800, set LCD_EVB_SCREEN_ROTATION_90 in menuconfig)*/
    ui_init();

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

    ESP_LOGI(TAG, "Out of ui_init()");

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_MODEL_NAME,
                                                             __json_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_IMG,
                                                             __json_event_handler, NULL, NULL));

    esp32_rp2040_init(); // another task to handle the RP2040 Serial data
    ESP_LOGI(TAG, "RP2040 init success");
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

bool is_name_geted = false;

void JsonQueue_processing_task(void* pvParameters) {
    char* receivedStr = NULL;
    static char modelTitle[64] = {0};

    for (;;) {
        if (xQueueReceive(JsonQueue, &receivedStr, portMAX_DELAY) == pdPASS && receivedStr != NULL) {

            cJSON* receivedJson = cJSON_Parse(receivedStr);
            if (receivedJson != NULL) { // 负责解析，并发送给对应的 event 处理

                /* Get the model name */
                if (!is_name_geted) {
                    cJSON* model_name = cJSON_GetObjectItem(receivedJson, "name");
                    if (cJSON_IsString(model_name) && model_name->valuestring) {
                        // ESP_LOGI(TAG, "Model name: %s", model_name->valuestring);
                        strncpy(modelTitle, model_name->valuestring, sizeof(modelTitle) - 1);
                        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_MODEL_NAME, &modelTitle[0],
                                          sizeof(modelTitle), portMAX_DELAY);
                        is_name_geted = true;
                    }
                }

                // Display the image
                cJSON* jsonImage = cJSON_GetObjectItem(receivedJson, "img");
                if (cJSON_IsString(jsonImage) && jsonImage->valuestring) {
                    lv_memset_00(imageData, sizeof(imageData));
                    strncpy(imageData, jsonImage->valuestring, sizeof(imageData) - 1);
                    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_IMG, &imageData[0],
                                      sizeof(imageData), portMAX_DELAY);
                } else {
                    ESP_LOGE(TAG, "Invalid img string");
                }

                // ///// 画框的函数调用
                cJSON* jsonBoxes = cJSON_GetObjectItem(receivedJson, "boxes");
                if (cJSON_IsArray(jsonBoxes)) {
                    ESP_LOGI(TAG, "sizeArray: %d", cJSON_GetArraySize(jsonBoxes));
                    cJSON* boxJson = NULL;
                    cJSON_ArrayForEach(boxJson, jsonBoxes) {
                        boxes_t box;
                        for (int i = 0; i < cJSON_GetArraySize(boxJson); i++) {
                            box.boxArray[i] = cJSON_GetArrayItem(boxJson, i)->valueint;
                        }
                        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_BOXES, (void*)&box,
                                          sizeof(box), portMAX_DELAY);
                    }
                }

                /// Keypoints 的处理
                cJSON* jsonKeypoints = cJSON_GetObjectItem(receivedJson, "keypoints");
                if (cJSON_IsArray(jsonKeypoints)) {
                    keypoints_t* keypoints_array;
                    int keypoints_count;
                    if (get_keypoints(jsonKeypoints, &keypoints_array, &keypoints_count)) {

                        if (keypoints_array == NULL) {
                            ESP_LOGE(TAG, "keypoints_array is NULL");
                            continue;
                        }
                        for (int i = 0; i < keypoints_count; i++) {
                            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_KEYPOINTS,
                                              (void*)&keypoints_array[i], sizeof(keypoints_array[i]), portMAX_DELAY);
                        }

                        for (int i = 0; i < keypoints_count; i++) {
                            free_keypoints_array(keypoints_array[i].points);
                        }
                        free(keypoints_array);
                    }
                }

                cJSON_Delete(receivedJson);
            }
            // free(receivedStr); // Free the received string after processing.
            cJSON_free(receivedStr); // Free the received string after processing.
            vTaskDelay(1);
            // receivedStr = NULL;
        }
    }
}
