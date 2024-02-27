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
#include "../main.h"
#include "cJSON.h"
#include "driver/uart.h"
#include "esp32_rp2040.h"

static const char *TAG = "esp32_rp2040";

#define ESP32_RP2040_TXD                  (19)
#define ESP32_RP2040_RXD                  (20)
#define ESP32_RP2040_RTS                  (UART_PIN_NO_CHANGE)
#define ESP32_RP2040_CTS                  (UART_PIN_NO_CHANGE)

#define ESP32_COMM_PORT_NUM               (2)
#define ESP32_COMM_BAUD_RATE              (921600)
#define ESP32_RP2040_COMM_TASK_STACK_SIZE (1024 * 15)


uint8_t buf[BUF_SIZE];  // recv
uint8_t data[BUF_SIZE]; // decode

static void               __commu_event_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);

// static int __cmd_send(uint8_t cmd, void *p_data, uint8_t len)
// {
//     uint8_t buf[32]  = {0};
//     uint8_t data[32] = {0};

//     if (len > 31) {
//         return -1;
//     }

//     uint8_t index = 1;

//     data[0]       = cmd;

//     if (len > 0 && p_data != NULL) {
//         memcpy(&data[1], p_data, len);
//         index += len;
//     }
//     cobs_encode_result ret = cobs_encode(buf, sizeof(buf), data, index);
// #if 1 // SENSOR_COMM_DEBUG
//     ESP_LOGI(TAG, "encode status:%d, len:%d", ret.status, ret.out_len);
//     for (int i = 0; i < ret.out_len; i++) {
//         printf("0x%x ", buf[i]);
//     }
//     printf("\r\n");
// #endif

//     if (ret.status == COBS_ENCODE_OK) {
//         return uart_write_bytes(ESP32_COMM_PORT_NUM, buf, ret.out_len + 1);
//     }
//     return -1;
// }


uint8_t     rev_buf[BUF_SIZE]; // 临时接收缓冲区
static char json_buffer[MAX_JSON_SIZE];
static json_buffer_len = 0;

#define RP2040_ESP_COMM_DEBUG 0

extern QueueHandle_t JsonQueue;

static void esp32_rp2040_comm_task(void *arg)
{
    uart_config_t uart_config = {
        .baud_rate  = ESP32_COMM_BAUD_RATE,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install(ESP32_COMM_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(ESP32_COMM_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(ESP32_COMM_PORT_NUM, ESP32_RP2040_TXD, ESP32_RP2040_RXD, ESP32_RP2040_RTS, ESP32_RP2040_CTS));
    // __cmd_send(PKT_TYPE_CMD_POWER_ON, NULL, 0);
    while (1) {

        int len = uart_read_bytes(ESP32_COMM_PORT_NUM, rev_buf, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);

        if (len > 0) {
            // 确保缓冲区不会溢出
            if ((json_buffer_len + len) <= MAX_JSON_SIZE) {
                memcpy(json_buffer + json_buffer_len, rev_buf, len);
                json_buffer_len += len;

                cJSON *json = cJSON_ParseWithLength((const char *)json_buffer, (size_t)json_buffer_len);
                if (json != NULL) {
#if RP2040_ESP_COMM_DEBUG
                    char *my_json_string = cJSON_Print(json);
                    ESP_LOGI(TAG, "my_json_string\n%s", my_json_string);
                    cJSON_free(my_json_string);
#endif
                    char *strToSend = pvPortMalloc(json_buffer_len);
                    if (strToSend != NULL) {

                        memset(strToSend, 0, json_buffer_len);
                        memcpy(strToSend, json_buffer, json_buffer_len);
                        if (xQueueSend(JsonQueue, &strToSend, portMAX_DELAY) != pdPASS) {
                            // 队列发送失败，需要释放 cJSON 对象以避免内存泄露
                            vPortFree(strToSend);
                            ESP_LOGI(TAG, "Failed to send JSON object to queue");
                        }
                    } else {
                        ESP_LOGE(TAG, "Failed to allocate memory for JSON object");
                    }
                    cJSON_Delete(json);
                    json_buffer_len = 0; // 清空缓冲区，准备接收下一个消息
                }
#if RP2040_ESP_COMM_DEBUG
                else {
                    const char *error_ptr = cJSON_GetErrorPtr(); // 解析错误处理
                    if (error_ptr != NULL) {
                        ESP_LOGE(stderr, "Error before: %s\n", error_ptr);
                    }
                    // JSON数据可能不完整，等待更多数据
                }
#endif
            } else {
                // 缓冲区溢出处理
                ESP_LOGE(TAG, "Buffer overflow, resetting buffer\n");
                json_buffer_len = 0; // 重置缓冲区
            }
        }
    }
}

void esp32_rp2040_init(void)
{
    // xTaskCreate(esp32_rp2040_comm_task, "esp32_rp2040_comm_task", ESP32_RP2040_COMM_TASK_STACK_SIZE, NULL, 10, NULL);
    xTaskCreatePinnedToCore(
        esp32_rp2040_comm_task,   // 任务函数
        "esp32_rp2040_comm_task", // 任务名称
        1024 * 50,                // 堆栈大小
        NULL,                     // 传递给任务的参数
        10,                       // 任务优先级
        NULL,                     // 任务句柄
        0                         // 任务固定在核心1
    );
}
