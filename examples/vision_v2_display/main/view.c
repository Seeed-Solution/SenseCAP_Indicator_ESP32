#include "cJSON.h"
#include "image.h"
#include "lvgl.h"
#include "main.h"
#include "mbedtls/base64.h"
#include "rectangle.h"
#include "string.h"
#include "ui/ui.h"

const char *TAG = "indicator_view";

lv_obj_t *ui_v2_image;



void JsonQueue_processing_task(void *pvParameters);

void app_view_init(void)
{
    ui_v2_image = lv_img_create(lv_scr_act());
    lv_obj_set_align(ui_v2_image, LV_ALIGN_CENTER);

    xTaskCreatePinnedToCore(
        JsonQueue_processing_task, // 任务函数
        "JsonQueueProcessingTask", // 任务名称
        1024 * 30,                 // 堆栈大小
        NULL,                      // 传递给任务的参数
        5,                         // 任务优先级
        NULL,                      // 任务句柄
        1                          // 任务固定在核心1
    );
    ESP_LOGI(TAG, "Queue create success");
}

static unsigned char img_buf[DECODED_STR_MAX_SIZE] = {0};

void JsonQueue_processing_task(void *pvParameters)
{
    char *receivedStr = NULL;
    for (;;) {
        if (xQueueReceive(JsonQueue, &receivedStr, portMAX_DELAY) == pdPASS && receivedStr != NULL) {
            
            cJSON *receivedJson = cJSON_Parse(receivedStr);
            if (receivedJson != NULL) {
                cJSON *img = cJSON_GetObjectItem(receivedJson, "img");
                if (cJSON_IsString(img) && (img->valuestring != NULL)) {
                    memset(img_buf, 0, DECODED_STR_MAX_SIZE);
                    strncpy(img_buf, img->valuestring, DECODED_STR_MAX_SIZE - 1);
                    lv_port_sem_take();
                    lv_obj_clean(ui_v2_image); // must be ahead
                    display_one_image(ui_v2_image, &img_buf);
                    lv_port_sem_give();
                    vTaskDelay(1);
                } 
                cJSON *boxes = cJSON_GetObjectItem(receivedJson, "boxes");
                if (cJSON_IsArray(boxes)) {

                    cJSON *row;
                    cJSON *col;
                    ESP_LOGI(TAG, "sizeArray: %d", cJSON_GetArraySize(boxes));
                    lv_port_sem_take();
                    cJSON_ArrayForEach(row, boxes)
                    {
                        int _        = 0;
                        int array[6] = {0};
                        cJSON_ArrayForEach(col, row)
                        {
                            array[_++] = (int)col->valueint;
                        }
                        ESP_LOGI(TAG, "%d %d %d %d %d %d", array[0], array[1], array[2], array[3], array[4], array[5]);
                        draw_one_rectangle(ui_v2_image, array[0], array[1], array[2], array[3], lv_color_make(113, 235, 52), array[4], array[5]);
                        ESP_LOGI(TAG, "Drawn rectangle %d", _);
                        vTaskDelay(1);
                    }
                    lv_port_sem_give();
                }

                cJSON *model_name = cJSON_GetObjectItem(receivedJson,"model_name");
                if(cJSON_IsString(model_name)){
                    ESP_LOGI(TAG, "model_name: %s", model_name->valuestring);
                    lv_port_sem_take();
                    lv_label_set_text(ui_Label1, model_name->valuestring);
                    lv_port_sem_give();
                }
                cJSON_Delete(receivedJson);
            }
            free(receivedStr); // Free the received string after processing.
            receivedStr = NULL;
        }
    }
}