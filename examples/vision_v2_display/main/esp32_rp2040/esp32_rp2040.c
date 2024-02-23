/**
 * @file esp32_rp2040.c
 * @date  29 November 2023

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2023, Seeed Studio
 */
// #include "nvs.h"
#include "time.h"
// #include <stdlib.h>
#include "driver/uart.h"
#include "cJSON.h"
#include "esp32_rp2040.h"
#include "../main.h"

static const char *TAG = "esp32_rp2040";

#define ESP32_RP2040_TXD (19)
#define ESP32_RP2040_RXD (20)
#define ESP32_RP2040_RTS (UART_PIN_NO_CHANGE)
#define ESP32_RP2040_CTS (UART_PIN_NO_CHANGE)

#define ESP32_COMM_PORT_NUM (2)
#define ESP32_COMM_BAUD_RATE (115200)
#define ESP32_RP2040_COMM_TASK_STACK_SIZE (1024 * 5)
#define BUF_SIZE (4 * 1024)

uint8_t rev_buf[BUF_SIZE]; // 临时接收缓冲区

#define MAX_JSON_SIZE BUF_SIZE * 2
static uint8_t json_buffer[MAX_JSON_SIZE];
static json_buffer_len = 0;

static void __commu_event_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);
__attribute__((weak)) int __sensor_data_parse_handle(uint8_t *p_data, ssize_t len)
{
    ESP_LOGI(TAG, "__sensor_data_parse_handle");
}

#define RP2040_ESP_COMM_DEBUG 0
extern QueueHandle_t img_Queue;
void process_json_data(cJSON *json)
{
    cJSON *img = cJSON_GetObjectItem(json, "img");
    cJSON *boxes = cJSON_GetObjectItem(json, "boxes");

    if (cJSON_IsString(img) && (img->valuestring != NULL))
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
                // ESP_LOGI(TAG, "\nImage data: %s", event_data->img_base64); // 正常

                // esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_IMG, event_data, sizeof(img_event_data_t), portMAX_DELAY);
                if (xQueueSend(img_Queue, &event_data, portMAX_DELAY) != pdPASS)
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

    if (cJSON_IsObject(boxes))
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

static void esp32_rp2040_comm_task(void *arg)
{
    uart_config_t uart_config = {
        .baud_rate = ESP32_COMM_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install(ESP32_COMM_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(ESP32_COMM_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(ESP32_COMM_PORT_NUM, ESP32_RP2040_TXD, ESP32_RP2040_RXD, ESP32_RP2040_RTS, ESP32_RP2040_CTS));

    while (1)
    {

        int len = uart_read_bytes(ESP32_COMM_PORT_NUM, rev_buf, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);

        if (len > 0)
        {
            // 确保缓冲区不会溢出
            if ((json_buffer_len + len) <= MAX_JSON_SIZE)
            {
                memcpy(json_buffer + json_buffer_len, rev_buf, len);
                json_buffer_len += len;

                cJSON *json = cJSON_ParseWithLength((const char *)json_buffer, (size_t)json_buffer_len);
                if (json != NULL)
                {
#if RP2040_ESP_COMM_DEBUG
                    char *my_json_string = cJSON_Print(json);
                    ESP_LOGI(TAG, "my_json_string\n%s", my_json_string);
                    cJSON_free(my_json_string);
#endif
                    process_json_data(json);
                    cJSON_Delete(json);  // 完成处理后释放cJSON对象
                    json_buffer_len = 0; // 清空缓冲区，准备接收下一个消息
                }
#if RP2040_ESP_COMM_DEBUG
                else
                {
                    const char *error_ptr = cJSON_GetErrorPtr(); // 解析错误处理
                    if (error_ptr != NULL)
                    {
                        ESP_LOGE(stderr, "Error before: %s\n", error_ptr);
                    }
                    // JSON数据可能不完整，等待更多数据
                }
#endif
            }
            else
            {
                // 缓冲区溢出处理
                ESP_LOGE(TAG, "Buffer overflow, resetting buffer\n");
                json_buffer_len = 0; // 重置缓冲区
            }
        }
    }
}

void esp32_rp2040_init(void)
{
    xTaskCreate(esp32_rp2040_comm_task, "esp32_rp2040_comm_task", ESP32_RP2040_COMM_TASK_STACK_SIZE, NULL, 10, NULL);
}
