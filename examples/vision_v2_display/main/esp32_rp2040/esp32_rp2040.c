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
#include "cobs.h"
#include "driver/uart.h"
#include "esp32_rp2040.h"
static const char* TAG = "esp32_rp2040";

#define ESP32_RP2040_TXD (19)
#define ESP32_RP2040_RXD (20)
#define ESP32_RP2040_RTS (UART_PIN_NO_CHANGE)
#define ESP32_RP2040_CTS (UART_PIN_NO_CHANGE)

#define ESP32_COMM_PORT_NUM (2)
#define ESP32_COMM_BAUD_RATE (921600)
#define ESP32_RP2040_COMM_TASK_STACK_SIZE (15 * 1024)

#define UART_BUF_SIZE (10 * 1024)
#define UART_WAITING_SIZE (UART_BUF_SIZE * 2)
#define JSON_OBJECT_POOL_SIZE (20)
#define MAX_JSON_BUFFER_SIZE (8 * 1024)


// 定义JSON对象池
static char jsonObjectPool[JSON_OBJECT_POOL_SIZE][MAX_JSON_BUFFER_SIZE];
static int poolIndex = 0;

static char* getJsonObjectFromPool() {
    char* obj = jsonObjectPool[poolIndex];
    poolIndex = (poolIndex + 1) % JSON_OBJECT_POOL_SIZE; // 循环使用对象池
    return obj;
}

static void __commu_event_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data);

QueueHandle_t JsonQueue;
static char waitingBuffer[MAX_JSON_BUFFER_SIZE];
static int waitingBufferSize = 0;
uint8_t rev_buf[UART_BUF_SIZE]; // 临时接收缓冲区
static void esp32_rp2040_comm_task(void* arg) {
    uart_config_t uart_config = {
        .baud_rate = ESP32_COMM_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install(ESP32_COMM_PORT_NUM, UART_BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(ESP32_COMM_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(
        uart_set_pin(ESP32_COMM_PORT_NUM, ESP32_RP2040_TXD, ESP32_RP2040_RXD, ESP32_RP2040_RTS, ESP32_RP2040_CTS));
    
    vTaskDelay(50 / portTICK_PERIOD_MS);
    __cmd_send(PKT_TYPE_CMD_MODEL_TITLE, NULL, 0);
    
    while (1) {
        int len = uart_read_bytes(ESP32_COMM_PORT_NUM, rev_buf, (UART_BUF_SIZE - 1), 10 / portTICK_PERIOD_MS);
        if (len > 0) {
            // check if the buffer is full
            if (waitingBufferSize + len < MAX_JSON_BUFFER_SIZE) {
                memcpy(waitingBuffer + waitingBufferSize, rev_buf, len);
                waitingBufferSize += len;
                waitingBuffer[waitingBufferSize] = '\0';
            } else {
                ESP_LOGE(TAG, "Waiting buffer overflow, reset.");
                waitingBufferSize = 0;
                continue;
            }
            // Attempts to extract and process all the complete JSON objects
            char* start_ptr = waitingBuffer;
            char* end_ptr;
            while ((end_ptr = strchr(start_ptr, '\n')) != NULL) { // Use line breaks as end markers
                int json_len = end_ptr - start_ptr;

                if (json_len > 0 && json_len < MAX_JSON_BUFFER_SIZE) {
                    char* jsonObject = getJsonObjectFromPool();
                    memcpy(jsonObject, start_ptr, json_len);
                    jsonObject[json_len] = '\0';

                    // Directly send object pointers to the queue to avoid duplication
                    if (xQueueSend(JsonQueue, &jsonObject, 0) != pdPASS) {
                        ESP_LOGE(TAG, "Failed to send JSON object to queue.");
                    }
                }

                start_ptr = end_ptr + 1; // Move to the start position of the next message
            }

            // Move the remaining data to the beginning of the buffer
            waitingBufferSize = strlen(start_ptr);
            memmove(waitingBuffer, start_ptr, waitingBufferSize + 1); // Including the ending null character
        }
    }
}

uint8_t JsonQueueBuffer[JSON_OBJECT_POOL_SIZE * sizeof(char*)];
StaticQueue_t JsonQueueBufferStruct;

void esp32_rp2040_init(void) {
    JsonQueue = xQueueCreateStatic(JSON_OBJECT_POOL_SIZE, sizeof(char*), JsonQueueBuffer, &JsonQueueBufferStruct);
    if (JsonQueue == NULL) {
        ESP_LOGE(TAG, "Queue create failed");
        return;
    }

    xTaskCreatePinnedToCore(esp32_rp2040_comm_task,            // 任务函数
                            "esp32_task",                      // 任务名称
                            ESP32_RP2040_COMM_TASK_STACK_SIZE, // 堆栈大小
                            NULL,                              // 传递给任务的参数
                            10,                                // 任务优先级
                            NULL,                              // 任务句柄
                            1                                  // 任务固定在核心0
    );
}

int __cmd_send(uint8_t cmd, void* p_data, uint8_t len) {
    uint8_t buf[32] = {0};
    uint8_t data[32] = {0};

    if (len > 31) {
        return -1;
    }

    uint8_t index = 1;

    data[0] = cmd;

    if (len > 0 && p_data != NULL) {
        memcpy(&data[1], p_data, len);
        index += len;
    }
    cobs_encode_result ret = cobs_encode(buf, sizeof(buf), data, index);
#if 0 // SENSOR_COMM_DEBUG
    ESP_LOGI(TAG, "encode status:%d, len:%d", ret.status, ret.out_len);
    for (int i = 0; i < ret.out_len; i++) {
        printf("0x%x ", buf[i]);
    }
    printf("\r\n");
#endif

    if (ret.status == COBS_ENCODE_OK) {
        return uart_write_bytes(ESP32_COMM_PORT_NUM, buf, ret.out_len + 1);
    }
    return -1;
}