#include "main.h"

#include "lvgl.h"
#include "ui/ui.h"
#include "mbedtls/base64.h"
#include "cJSON.h"
const char *TAG = "indicator_view";

#define IMG_WIDTH 240
#define IMG_HEIGHT 240

lv_obj_t *ui_v2_image;

QueueHandle_t JsonQueue;

static unsigned char *base64_jpeg;

static lv_img_dsc_t img_dsc = {
    .header.always_zero = 0,
    .header.w = IMG_WIDTH,
    .header.h = IMG_HEIGHT,
    .data_size = 0,
    .header.cf = LV_IMG_CF_RAW_ALPHA,
    .data = NULL,
};

static unsigned char img_buf[1024 * 5] = {0};

static inline void display_one_page(const unsigned char *p_data);

void JsonQueue_processing_task(void *pvParameters);

void app_view_init(void)
{
    ui_v2_image = lv_img_create(lv_scr_act());
    lv_obj_set_align(ui_v2_image, LV_ALIGN_CENTER);

    JsonQueue = xQueueCreate(JsonQueue_SIZE, sizeof(char *)); // Image Queue
    if (JsonQueue == NULL)
    {
        ESP_LOGE(TAG, "Queue create failed");
        return;
    }
    xTaskCreatePinnedToCore(
        JsonQueue_processing_task, // 任务函数
        "JsonQueueProcessingTask", // 任务名称
        1024 * 5,                  // 堆栈大小
        NULL,                      // 传递给任务的参数
        5,                         // 任务优先级
        NULL,                      // 任务句柄
        1                          // 任务固定在核心1
    );
    // xTaskCreate(JsonQueue_processing_task, "json_task", 1024 * 10, NULL, 10, NULL);
}

void JsonQueue_processing_task(void *pvParameters) {
    char *receivedStr = NULL;
    for (;;) {
        if (xQueueReceive(JsonQueue, &receivedStr, portMAX_DELAY) == pdPASS && receivedStr != NULL) {
            cJSON *receivedJson = cJSON_Parse(receivedStr);
            if (receivedJson != NULL) {
                cJSON *img = cJSON_GetObjectItem(receivedJson, "img");
                if (cJSON_IsString(img) && (img->valuestring != NULL)) {
                    display_one_page((const unsigned char *)img->valuestring);
                }
                cJSON_Delete(receivedJson);
            }
            free(receivedStr); // Free the received string after processing.
            receivedStr = NULL;
        }
    }
}

#define DECODED_STR_MAX_SIZE (8 * 1024)
static unsigned char decoded_str[DECODED_STR_MAX_SIZE + 1]; // 静态分配
static inline void display_one_page(const unsigned char *p_data) {
    if (!p_data) return;

    size_t str_len = strlen((const char *)p_data);
    size_t output_len = 0;
    // 首先获取解码后的长度
    int decode_ret = mbedtls_base64_decode(NULL, 0, &output_len, p_data, str_len);
    if (decode_ret == MBEDTLS_ERR_BASE64_INVALID_CHARACTER) {
        ESP_LOGE(TAG, "Invalid character in Base64 string");
        return;
    }else if(decode_ret == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL){
        ESP_LOGE(TAG, "Buffer too small for decoding");
        return;
    }

    // 检查解码后的长度是否超出预期
    if (output_len == 0 || output_len > DECODED_STR_MAX_SIZE) {
        ESP_LOGE(TAG, "Base64 decode output size is too large or zero.");
        return;
    }

    // 执行实际的解码操作
    decode_ret = mbedtls_base64_decode(decoded_str, DECODED_STR_MAX_SIZE, &output_len, p_data, str_len);
    if (decode_ret == 0) {
        img_dsc.data = decoded_str;
        img_dsc.data_size = output_len; // 确保设置了正确的data_size
        lv_img_set_src(ui_v2_image, &img_dsc);
        // ESP_LOGI(TAG, "displayed successfully.");
    } else {
        ESP_LOGE(TAG, "Failed to decode Base64 string, error: %d", decode_ret);
    }
}

// static inline void display_one_page(const unsigned char *p_data)
// {
//     if (p_data == NULL)
//     {
//         printf("Input data is NULL\n");
//         return;
//     }
//     size_t str_len = strlen((const char *)p_data);
//     if (str_len == 0)
//     {
//         printf("Input data is empty\n");
//         return;
//     }
//     size_t output_len = 0;
//     // 第一次调用，获取解码后的长度
//     mbedtls_base64_decode(NULL, 0, &output_len, p_data, str_len);

//     // // 检查输出长度是否合理
//     if (output_len == 0 || output_len > 1000000)
//     { // 假定1000000为最大合理长度
//         printf("Decoded length is unreasonable: %zu\n", output_len);
//         return;
//     }
//     unsigned char *decoded_str = (unsigned char)malloc(output_len + 1); // 分配足够的内存
//     if (decoded_str == NULL)
//     {
//         printf("Failed to allocate memory for decoded string\n");
//         return;
//     }
//     // memset(decoded_str, 0, output_len + 1);
//     // 实际的解码操作
//     int ret = mbedtls_base64_decode(&decoded_str, output_len, &output_len, (const unsigned char *)p_data, str_len);
//     if (ret == 0)
//     {
//         // decoded_str[output_len] = '\0'; // 确保字符串以null结尾

//         // 使用解码的数据
//         img_dsc.header.always_zero = 0;
//         img_dsc.header.w = 240; // 设置宽度
//         img_dsc.header.h = 240; // 设置高度
//         img_dsc.data_size = output_len;
//         img_dsc.header.cf = LV_IMG_CF_RAW;
//         img_dsc.data = decoded_str;

//         lv_img_set_src(ui_v2_image, &img_dsc);
//         printf("Decoded image set to display\n");
//     }
//     else if (ret == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL)
//     {
//         ESP_LOGE(TAG, "MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL");
//     }
//     else
//     {
//         ESP_LOGE(TAG, "Failed to decode Base64 string\n");
//     }

//     free(decoded_str); // 释放内存
// }
// int ret = mbedtls_base64_decode(NULL, 0, &output_len, (const unsigned char *)p_data, str_len);
// decoded_str = malloc(output_len + 1); // 分配足够的内存
// if (decoded_str != NULL)
// {
//     // 进行实际的解码操作
//     ret = mbedtls_base64_decode(decoded_str, output_len, &output_len, (const unsigned char *)p_data, str_len);
//     if (ret == 0)
//     {
//         decoded_str[output_len] = '\0'; // 确保字符串以null结尾
//         // printf("Decoded string: %s\n", decoded_str);
//         img_dsc.header.always_zero = 0;
//         img_dsc.header.w = 240;
//         img_dsc.header.h = 240;
//         img_dsc.data_size = output_len;
//         img_dsc.header.cf = LV_IMG_CF_RAW;
//         img_dsc.data = decoded_str;
//         lv_img_set_src(ui_v2_image, &img_dsc);

//         // ESP_LOGI(TAG, "Decoded image size: %zu", output_len);
//     }
//     else
//     {
//         printf("Failed to decode Base64 string\n");
//     }
// }
// else
// {
//     printf("Failed to allocate memory for decoded string\n");
// }

void process_json_data(cJSON *json)
{
    cJSON *img = cJSON_GetObjectItem(json, "img");
    cJSON *boxes = cJSON_GetObjectItem(json, "boxes");

    if (cJSON_IsString(img) && (img->valuestring != NULL)) // 图片数据的处理
    {
        // 动态分配内存来存储事件数据
        img_event_data_t *event_data = (img_event_data_t *)malloc(sizeof(img_event_data_t));
        if (event_data != NULL)
        {
            event_data->img_base64 = strdup(img->valuestring); // 复制字符串以确保它在事件处理时仍然有效
            event_data->img_size = strlen(img->valuestring);
            // ESP_LOGI(TAG, "allocated");
            if (event_data->img_base64 != NULL)
            {
#if RP2040_ESP_COMM_DEBUG
                ESP_LOGI(TAG, "\nImage data: %s", event_data->img_base64); // 正常
#endif                                                                     // RP2040_ESP_COMM_DEBUG
                if (xQueueSend(JsonQueue, &event_data, portMAX_DELAY) != pdPASS)
                {
                    // 处理队列发送失败的情况，例如可以释放data的内存
                    free(event_data->img_base64);
                    free(event_data);
                }
            }
            else
            {
                // 如果字符串复制失败，则释放之前分配的内存以避免内存泄漏
                free(event_data);
            }
        }
    }

    if (cJSON_IsObject(boxes)) // 框框的处理
    {
        cJSON *target = cJSON_GetObjectItem(boxes, "target");
        cJSON *score = cJSON_GetObjectItem(boxes, "score");
        cJSON *x = cJSON_GetObjectItem(boxes, "x");
        cJSON *y = cJSON_GetObjectItem(boxes, "y");
        cJSON *w = cJSON_GetObjectItem(boxes, "w");
        cJSON *h = cJSON_GetObjectItem(boxes, "h");

        ESP_LOGI(TAG, "\nBoxes data - Target: %d, Score: %d, X: %d, Y: %d, W: %d, H: %d",
                 target->valueint, score->valueint, x->valueint, y->valueint, w->valueint, h->valueint);
    }
}

// static void __view_event_handler(void *handler_args, esp_event_base_t base,
//                                  int32_t id, void *event_data)
// {
//     switch (id)
//     {
//     case VIEW_EVENT_IMG:
//     {

//         ESP_LOGI(TAG, "event: VIEW_EVENT_IMG");
//         // struct view_data_openai_request *p_req = (struct view_data_openai_request *)event_data;
//         // request_st_update(0, "ready");
//         // xSemaphoreGive(__g_dalle_com_sem);
//         img_event_data_t *event_data = (img_event_data_t *)event_data;
//         memcpy(&img_buf, event_data->img_base64, event_data->img_size);
//         ESP_LOGI(TAG, "event: VIEW_EVENT_IMG %d", event_data->img_size);
//         if (event_data != NULL)
//         {
//             // 使用event_data->img_base64进行图像解码和显示...
//             ESP_LOGI(TAG, "\nImage data: %s", img_buf);
//             // display_one_page(&img_buf);
//             // 完成后释放内存
//             // free(event_data->img_base64);
//             // free(event_data);
//         }
//         break;
//     }

//     default:
//         break;
//     }
// }
// static void __indicator_video_task(void *p_arg)
// {

//     // lv_obj_t *ui_v2_image;
//     // ui_v2_image = lv_img_create(lv_scr_act());
//     // lv_obj_set_align(ui_v2_image, LV_ALIGN_CENTER);

//     while (true)
//     {
//         /* code */
//         size_t str_len = strlen(base64_jpeg);
//         size_t output_len = 0;

//         unsigned char *decoded_str = NULL;

//         // 计算解码后的长度
//         int ret = mbedtls_base64_decode(NULL, 0, &output_len, (const unsigned char *)base64_jpeg, str_len);
//         if (1)
//         {
//             decoded_str = malloc(output_len + 1); // 分配足够的内存

//             if (decoded_str != NULL)
//             {
//                 // 进行实际的解码操作
//                 ret = mbedtls_base64_decode(decoded_str, output_len, &output_len, (const unsigned char *)base64_jpeg, str_len);
//                 if (ret == 0)
//                 {
//                     decoded_str[output_len] = '\0'; // 确保字符串以null结尾
//                     // printf("Decoded string: %s\n", decoded_str);
//                     img_png.header.always_zero = 0;
//                     img_png.header.w = 240;
//                     img_png.header.h = 240;
//                     img_png.data_size = output_len;
//                     img_png.header.cf = LV_IMG_CF_RAW_ALPHA;
//                     img_png.data = decoded_str;
//                     lv_img_set_src(ui_v2_image, &img_png);

//                     ESP_LOGI(TAG, "Decoded image size: %zu", output_len);
//                 }
//                 else
//                 {
//                     printf("Failed to decode Base64 string\n");
//                 }

//                 // free(decoded_str);
//             }
//             else
//             {
//                 printf("Failed to allocate memory for decoded string\n");
//             }
//         }
//         else
//         {
//             printf("Failed to calculate buffer length\n");
//         }
//     }
// }
