#include "main.h"

#include "lvgl.h"
#include "ui/ui.h"
#include "mbedtls/base64.h"

const char *TAG = "indicator_view";

#define IMG_WIDTH 240
#define IMG_HEIGHT 240

lv_obj_t *ui_v2_image;

QueueHandle_t img_Queue;

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

void img_queue_processing_task(void *pvParameters);

void app_view_init(void)
{
    // ui_v2_image = lv_img_create(lv_scr_act());
    // lv_obj_set_align(ui_v2_image, LV_ALIGN_CENTER);

    // ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
    //                                                          VIEW_EVENT_BASE, VIEW_EVENT_IMG,
    //                                                          __view_event_handler, NULL, NULL));
    ui_v2_image = lv_img_create(lv_scr_act());
    lv_obj_set_align(ui_v2_image, LV_ALIGN_CENTER);

    // 初始化队列
    img_Queue = xQueueCreate(IMG_QUEUE_SIZE, sizeof(img_event_data_t *));
    if (img_Queue == NULL)
    {
        ESP_LOGE(TAG, "Queue create failed");
        return;
    }

    // 创建队列处理任务
    xTaskCreate(img_queue_processing_task, "img_task", 1024 * 10, NULL, 10, NULL);
}

void img_queue_processing_task(void *pvParameters)
{
    img_event_data_t *data;
    while (1)
    {
        if (xQueueReceive(img_Queue, &data, portMAX_DELAY) == pdPASS)
        {
            // 处理图像数据
            // ESP_LOGI(TAG, "\n Queue Image: %s", data->img_base64);
            display_one_page(data->img_base64);
            // 释放内存
            free(data->img_base64);
            free(data);
        }
    }
}

static inline void display_one_page(const unsigned char *p_data)
{
    size_t str_len = strlen(p_data);
    size_t output_len = 0;

    unsigned char *decoded_str = NULL;

    int ret = mbedtls_base64_decode(NULL, 0, &output_len, (const unsigned char *)p_data, str_len);
    decoded_str = malloc(output_len + 1); // 分配足够的内存
    if (decoded_str != NULL)
    {
        // 进行实际的解码操作
        ret = mbedtls_base64_decode(decoded_str, output_len, &output_len, (const unsigned char *)p_data, str_len);
        if (ret == 0)
        {
            decoded_str[output_len] = '\0'; // 确保字符串以null结尾
            // printf("Decoded string: %s\n", decoded_str);
            img_dsc.header.always_zero = 0;
            img_dsc.header.w = 240;
            img_dsc.header.h = 240;
            img_dsc.data_size = output_len;
            img_dsc.header.cf = LV_IMG_CF_RAW_ALPHA;
            img_dsc.data = decoded_str;
            lv_img_set_src(ui_v2_image, &img_dsc);

            ESP_LOGI(TAG, "Decoded image size: %zu", output_len);
        }
        else
        {
            printf("Failed to decode Base64 string\n");
        }
    }
    else
    {
        printf("Failed to allocate memory for decoded string\n");
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