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

static inline void display_one_page(lv_obj_t *image, const unsigned char *p_data);
void draw_one_rectangle(lv_obj_t *parent, int x, int y, int w, int h, lv_color_t color, const char *tarStr, int score);

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
        1024 * 10,                 // 堆栈大小
        NULL,                      // 传递给任务的参数
        5,                         // 任务优先级
        NULL,                      // 任务句柄
        1                          // 任务固定在核心1
    );
}

#define MAX_BOXES 5                         // 假设最多同时显示5个矩形
static lv_obj_t *rects[MAX_BOXES] = {NULL}; // 用于跟踪所有矩形对象的数组
static int rect_count = 0;                  // 当前矩形数量
// 删除所有旧的矩形对象
void delete_all_rects()
{
    for (int i = 0; i < rect_count; i++)
    {
        if (rects[i] != NULL)
        {
            lv_obj_del(rects[i]); // 删除对象
            rects[i] = NULL;      // 清除指针，防止野指针
        }
    }
    rect_count = 0; // 重置计数器
}
void draw_one_rectangle(lv_obj_t *parent, int x, int y, int w, int h, lv_color_t color, const char *tarStr, int score)
{
    // Adjust coordinates to center the rectangle
    x = x - w / 2;
    y = y - h / 2;

    // Create the main rectangle (border only, no fill)
    lv_obj_t *rect = lv_obj_create(parent);
    lv_obj_set_size(rect, w, h);
    lv_obj_set_pos(rect, x, y);
    lv_obj_set_style_border_color(rect, color, 0);
    lv_obj_set_style_border_width(rect, 4, 0);
    lv_obj_set_style_bg_opa(rect, LV_OPA_TRANSP, 0); // Make background transparent

    if (rect_count < MAX_BOXES)
    {                               // 确保不会超出数组界限
        rects[rect_count++] = rect; // 跟踪新创建的矩形对象
    }
}

static int box_count = 0;
static boxes_t boxes_array[5] = {0};
void JsonQueue_processing_task(void *pvParameters)
{
    char *receivedStr = NULL;
    for (;;)
    {
        if (xQueueReceive(JsonQueue, &receivedStr, portMAX_DELAY) == pdPASS && receivedStr != NULL)
        {
            cJSON *receivedJson = cJSON_Parse(receivedStr);
            if (receivedJson != NULL)
            {
                cJSON *img = cJSON_GetObjectItem(receivedJson, "img");
                if (cJSON_IsString(img) && (img->valuestring != NULL))
                {
                    lv_port_sem_take();
                    delete_all_rects();
                    // lv_obj_t *child = lv_obj_get_child(ui_v2_image, 0);
                    // if(child != NULL){
                    //     lv_obj_del(child);
                    // }
                    // lv_obj_clean(ui_v2_image);
                    display_one_page(ui_v2_image, (const unsigned char *)img->valuestring);
                    lv_port_sem_give();
                }

                cJSON *boxes = cJSON_GetObjectItem(receivedJson, "boxes");
                if (cJSON_IsArray(boxes))
                {
                    cJSON *row;
                    box_count = 0;
                    cJSON_ArrayForEach(row, boxes)
                    {
                        cJSON *col;
                        int _ = 0, array[6] = {0};
                        cJSON_ArrayForEach(col, row)
                        {
                            array[_++] = col->valueint;
                        }
                        boxes_array[box_count].x = array[0];
                        boxes_array[box_count].y = array[1];
                        boxes_array[box_count].w = array[2];
                        boxes_array[box_count].h = array[3];
                        boxes_array[box_count].score = array[4];
                        boxes_array[box_count].target = array[5];
                        ESP_LOGI(TAG, "\nBoxes data[%d] - X: %d, Y: %d, W: %d, H: %d, Score: %d, Target: %d",
                                 box_count,
                                 boxes_array[box_count].x,
                                 boxes_array[box_count].y,
                                 boxes_array[box_count].w,
                                 boxes_array[box_count].h,
                                 boxes_array[box_count].score,
                                 boxes_array[box_count].target);
                        lv_port_sem_take();
                        draw_one_rectangle(ui_v2_image, array[0], array[1], array[2], array[3], lv_color_make(113, 235, 52), array[4], array[5]);
                        lv_port_sem_give();
                        box_count++;
                    }
                }
                cJSON_Delete(receivedJson);
            }
            free(receivedStr); // Free the received string after processing.
            receivedStr = NULL;
        }
    }
}

#define DECODED_STR_MAX_SIZE (5 * 1024)
static unsigned char decoded_str[DECODED_STR_MAX_SIZE + 1]; // 静态分配
static inline void display_one_page(lv_obj_t *image, const unsigned char *p_data)
{
    if (!p_data)
        return;

    size_t str_len = strlen((const char *)p_data);
    size_t output_len = 0;
    // 首先获取解码后的长度
    int decode_ret = mbedtls_base64_decode(NULL, 0, &output_len, p_data, str_len);
    if (decode_ret == MBEDTLS_ERR_BASE64_INVALID_CHARACTER)
    {
        ESP_LOGE(TAG, "Invalid character in Base64 string");
        return;
    }

    // 检查解码后的长度是否超出预期
    if (output_len == 0 || output_len > DECODED_STR_MAX_SIZE)
    {
        ESP_LOGE(TAG, "Base64 decode output size is too large or zero.");
        return;
    }

    // 执行实际的解码操作
    decode_ret = mbedtls_base64_decode(decoded_str, DECODED_STR_MAX_SIZE, &output_len, p_data, str_len);
    if (decode_ret == 0)
    {
        img_dsc.data = decoded_str;
        img_dsc.data_size = output_len; // 确保设置了正确的data_size
        lv_img_set_src(image, &img_dsc);
        ESP_LOGI(TAG, "displayed successfully.");
    }
    else if (decode_ret == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL)
    {
        ESP_LOGE(TAG, "Buffer too small for decoding %d bytes %d output", str_len, output_len);
        return;
    }
    else
    {
        ESP_LOGE(TAG, "Failed to decode Base64 string, error: %d", decode_ret);
    }
}
