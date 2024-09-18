/**
 * @file esp32_rp2040.c
 * @date  29 November 2023

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2023, Seeed Studio
 */
#include "esp32_rp2040.h"
#include "cobs.h"
#include "driver/uart.h"
#include "esp_timer.h"
#include "nvs.h"
#include "time.h"
#include <stdlib.h>

static const char *TAG = "esp32_rp2040";

#define ESP32_RP2040_TXD                  (19)
#define ESP32_RP2040_RXD                  (20)
#define ESP32_RP2040_RTS                  (UART_PIN_NO_CHANGE)
#define ESP32_RP2040_CTS                  (UART_PIN_NO_CHANGE)

#define ESP32_COMM_PORT_NUM               (2)
#define ESP32_COMM_BAUD_RATE              (115200)
#define ESP32_RP2040_COMM_TASK_STACK_SIZE (1024 * 4)
#define BUF_SIZE                          (512)

uint8_t buf[BUF_SIZE];  // recv
uint8_t data[BUF_SIZE]; // decode

static void __commu_event_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);
__attribute__((weak)) int __sensor_data_parse_handle(uint8_t *p_data, ssize_t len){
    ESP_LOGI(TAG, "__sensor_data_parse_handle");
}

static int __cmd_send(uint8_t cmd, void *p_data, uint8_t len)
{
    uint8_t buf[32]  = {0};
    uint8_t data[32] = {0};

    if (len > 31) {
        return -1;
    }

    uint8_t index = 1;

    data[0]       = cmd;

    if (len > 0 && p_data != NULL) {
        memcpy(&data[1], p_data, len);
        index += len;
    }
    cobs_encode_result ret = cobs_encode(buf, sizeof(buf), data, index);
#if 1 // SENSOR_COMM_DEBUG
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

    __cmd_send(PKT_TYPE_CMD_POWER_ON, NULL, 0);
    cobs_decode_result ret;

    while (1) {
        int len = uart_read_bytes(ESP32_COMM_PORT_NUM, buf, (BUF_SIZE - 1), 1 / portTICK_PERIOD_MS);
#if SENSOR_COMM_DEBUG
        ESP_LOGI(TAG, "len:%d", len);
#endif
        int      index       = 0;
        uint8_t *p_buf_start = buf;
        uint8_t *p_buf_end   = buf;
        if (len > 0) {

#if SENSOR_COMM_DEBUG
            printf("recv:  ");
            for (int i = 0; i < len; i++) {
                printf("0x%x ", buf[i]);
            }
            printf("\r\n");
#endif
            while (p_buf_start < (buf + len)) {
                uint8_t *p_buf_end = p_buf_start;
                while (p_buf_end < (buf + len)) {
                    if (*p_buf_end == 0x00) {
                        break;
                    }
                    p_buf_end++;
                }
                // decode buf
                memset(data, 0, sizeof(data));
                ret = cobs_decode(data, sizeof(data), p_buf_start, p_buf_end - p_buf_start);

#if SENSOR_COMM_DEBUG
                ESP_LOGI(TAG, "decode status:%d, len:%d, type:0x%x  ", ret.status, ret.out_len, data[0]);
                printf("decode: ");
                for (int i = 0; i < ret.out_len; i++) {
                    printf("0x%x ", data[i]);
                }
                printf("\r\n");
#endif
                if (ret.out_len > 1 && ret.status == COBS_DECODE_OK) {
                    // todo  ret.status
                    __sensor_data_parse_handle((uint8_t *)data, ret.out_len); // parse data HERE
                }

                p_buf_start = p_buf_end + 1; // next message
            }
        }
    }
}

void esp32_rp2040_init(void)
{
    xTaskCreate(esp32_rp2040_comm_task, "esp32_rp2040_comm_task", ESP32_RP2040_COMM_TASK_STACK_SIZE, NULL, 5, NULL);
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_SHUTDOWN,
                                                             __commu_event_handler, NULL, NULL));
}

static void __sensor_shutdown(void)
{
    int ret = 0;
    ret     = __cmd_send(PKT_TYPE_CMD_SHUTDOWN, NULL, 0);
    if (ret <= 0) {
        ESP_LOGI(TAG, "sensor shutdown fail!. %d", ret);
    }
}
static void __commu_event_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
    switch (id) {
        case VIEW_EVENT_SHUTDOWN:
            ESP_LOGI(TAG, "event: VIEW_EVENT_SHUTDOWN");
            __sensor_shutdown();
            break;
        default:
            break;
    }
}

