#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"
#include "bsp_board.h"
#include "cmd_sys.h"
#include "cmd_lora.h"
#include "cmd_wifi.h"
#include "nvs_flash.h"
#include "lv_port.h"
#include "ui.h"
#include "bsp_btn.h"
#include "driver/uart.h"
#include "cobs.h"

#define VERSION   "v1.0.2-test"

#define SENSECAP  "\n\
   _____                      _________    ____         \n\
  / ___/___  ____  ________  / ____/   |  / __ \\       \n\
  \\__ \\/ _ \\/ __ \\/ ___/ _ \\/ /   / /| | / /_/ /   \n\
 ___/ /  __/ / / (__  )  __/ /___/ ___ |/ ____/         \n\
/____/\\___/_/ /_/____/\\___/\\____/_/  |_/_/           \n\
--------------------------------------------------------\n\
 Version: %s %s %s\n\
--------------------------------------------------------\n\
"

// #ifndef CONFIG_LCD_BOARD_SENSECAP_TERMINAL_WXM
//     #error "Must be a SenseCAP Terminal WXM Board"
// #endif

static const char* TAG = "TEST";
#define PROMPT_STR CONFIG_IDF_TARGET

#define TEST_MSG  "TEST MSG: "


#define SENSOR_COMM_DEBUG    0


#define ESP32_RP2040_TXD (19)
#define ESP32_RP2040_RXD (20)
#define ESP32_RP2040_RTS (UART_PIN_NO_CHANGE)
#define ESP32_RP2040_CTS (UART_PIN_NO_CHANGE)

#define ESP32_COMM_PORT_NUM      (2)
#define ESP32_COMM_BAUD_RATE     (115200)
#define BUF_SIZE (512)

uint8_t buf[BUF_SIZE];   //recv 
uint8_t data[BUF_SIZE];  //decode

enum  pkt_type {

    PKT_TYPE_CMD_COLLECT_INTERVAL = 0xA0, //uin32_t 
    PKT_TYPE_CMD_BEEP_ON  = 0xA1,  //uin32_t  ms: on time 
    PKT_TYPE_CMD_BEEP_OFF = 0xA2,
    PKT_TYPE_CMD_SHUTDOWN = 0xA3, //uin32_t 
    PKT_TYPE_CMD_POWER_ON = 0xA4,

    PKT_TYPE_SENSOR_SCD41_TEMP  = 0xB0, // float
    PKT_TYPE_SENSOR_SCD41_HUMIDITY = 0xB1, // float
    PKT_TYPE_SENSOR_SCD41_CO2 = 0xB2, // float

    PKT_TYPE_SENSOR_SHT41_TEMP = 0xB3, // float
    PKT_TYPE_SENSOR_SHT41_HUMIDITY = 0xB4, // float

    PKT_TYPE_SENSOR_TVOC_INDEX = 0xB5, // float

    PKT_TYPE_CMD_TEST = 0xF0, //
    //todo
};

static int __cmd_send(uint8_t cmd, void *p_data, uint8_t len)
{
    uint8_t buf[32] = {0};
    uint8_t data[32] = {0};

    if( len > 31) {
        return -1;
    }
    
    uint8_t index =1;

    data[0] = cmd;

    if( len > 0 && p_data != NULL) {
        memcpy(&data[1], p_data, len);
        index += len;
    }
    cobs_encode_result ret = cobs_encode(buf, sizeof(buf),  data, index);
#if SENSOR_COMM_DEBUG
    ESP_LOGI(TAG, "encode status:%d, len:%d",  ret.status,  ret.out_len);
    for(int i=0; i < ret.out_len; i++ ) {
        printf( "0x%x ", buf[i] );
    }
    printf("\r\n");
#endif

    if( ret.status == COBS_ENCODE_OK ) {
        return uart_write_bytes(ESP32_COMM_PORT_NUM,  buf, ret.out_len+1);
    }
    return -1;
}


static int __data_parse_handle(uint8_t *p_data, ssize_t len)
{
    char data_buf[32];
    memset(data_buf, 0, sizeof(data_buf));
    float  vaule;
    uint8_t pkt_type = p_data[0];
    switch ( pkt_type)
    {
        case PKT_TYPE_SENSOR_SCD41_CO2: {
           
            if( len < (sizeof(vaule) +1)) {
                break;
            }

            memcpy(&vaule, &p_data[1], sizeof(vaule));

            snprintf(data_buf, sizeof(data_buf), "%.1f", vaule);
            // ESP_LOGI(TAG, "CO2: %s", data_buf);
            break; 
        }
        
        case PKT_TYPE_SENSOR_SHT41_TEMP: {
            if( len < (sizeof(vaule) +1)) {
                break;
            }
            memcpy(&vaule, &p_data[1], sizeof(vaule));

            snprintf(data_buf, sizeof(data_buf), "%.1f",vaule );
            // ESP_LOGI(TAG, "temp:%s", data_buf);

            break;
        } 

        case PKT_TYPE_SENSOR_SHT41_HUMIDITY: {
            if( len < (sizeof(vaule) +1)) {
                break;
            }
            memcpy(&vaule, &p_data[1], sizeof(vaule));

            snprintf(data_buf, sizeof(data_buf), "%.1f", vaule);
            // ESP_LOGI(TAG, "HUMIDITY: %s", data_buf);
            break; 
        } 

        case PKT_TYPE_SENSOR_TVOC_INDEX: {
            if( len < (sizeof(vaule) +1)) {
                break;
            }
            memcpy(&vaule, &p_data[1], sizeof(vaule));

            snprintf(data_buf, sizeof(data_buf), "%.1f", vaule);
            // ESP_LOGI(TAG, "TVOC: %s", data_buf);
            break; 
        } 
        case  PKT_TYPE_CMD_TEST : {
            uint32_t cnt=0;
            if( len < (sizeof(cnt) +1)) {
                break;
            }
            memcpy(&cnt, &p_data[1], sizeof(vaule));
            cnt +=1;

            __cmd_send(PKT_TYPE_CMD_TEST, &cnt, sizeof(cnt));
            break;
        }
        default:
            break;
    }
    return 0;
}


static void esp32_rp2040_comm_task(void *arg)
{
    uart_config_t uart_config = {
        .baud_rate = ESP32_COMM_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install(ESP32_COMM_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(ESP32_COMM_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(ESP32_COMM_PORT_NUM, ESP32_RP2040_TXD, ESP32_RP2040_RXD, ESP32_RP2040_RTS, ESP32_RP2040_CTS));

    cobs_decode_result ret;
    
    while (1) {
        int len = uart_read_bytes(ESP32_COMM_PORT_NUM, buf, (BUF_SIZE - 1), 1 / portTICK_PERIOD_MS);
#if SENSOR_COMM_DEBUG
        ESP_LOGI(TAG, "len:%d",  len);
#endif 
        int index  = 0;
        uint8_t *p_buf_start =  buf;
        uint8_t *p_buf_end = buf;
        if( len > 0 ) {

#if SENSOR_COMM_DEBUG
            printf("recv:  ");
            for(int i=0; i < len; i++ ) {
                printf( "0x%x ", buf[i] );
            }
            printf("\r\n");
#endif 
            while ( p_buf_start < (buf + len)) {
                uint8_t *p_buf_end = p_buf_start;
                while( p_buf_end < (buf + len) ) {
                    if( *p_buf_end == 0x00) {
                        break;
                    }
                    p_buf_end++;
                }
                // decode buf 
                memset(data, 0, sizeof(data));
                ret = cobs_decode(data, sizeof(data),  p_buf_start, p_buf_end - p_buf_start);

#if SENSOR_COMM_DEBUG
                ESP_LOGI(TAG, "decode status:%d, len:%d, type:0x%x  ",  ret.status,  ret.out_len, data[0]);
                printf("decode: ");
                for(int i=0; i < ret.out_len; i++ ) {
                    printf( "0x%x ", data[i] );
                }
                printf("\r\n");
#endif 
                if( ret.out_len > 1  &&  ret.status == COBS_DECODE_OK ) { //todo  ret.status
                    __data_parse_handle((uint8_t *)data, ret.out_len);
                }

                p_buf_start = p_buf_end + 1; // next message
            }
        }
    }
}

static void console_task(void *arg)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    initialise_wifi();

    /* Register commands */
    esp_console_register_help_command();
    register_sys();
    register_lora();
    register_wifi();
    register_ping();

    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    repl_config.prompt = PROMPT_STR ">";
    repl_config.max_cmdline_length = 1024;

    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

static void __btn_click_callback(void* arg)
{
    static bool st=1;
    ESP_LOGI("btn",  TEST_MSG "button click ok");
    st=!st;
    bsp_lcd_set_backlight(st);
}

void app_main(void)
{
    ESP_LOGI("", SENSECAP, VERSION, __DATE__, __TIME__);
    ESP_ERROR_CHECK(bsp_board_init());

    lv_port_init();

    lv_port_sem_take();
    ui_init();
    lv_port_sem_give();

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );


    xTaskCreate(console_task, "console_task", 1024*4, NULL, 6, NULL);
    xTaskCreate(esp32_rp2040_comm_task, "esp32_rp2040_comm_task", 1024*4, NULL, 5, NULL);
    bsp_btn_register_callback( BOARD_BTN_ID_USER, BUTTON_SINGLE_CLICK,  __btn_click_callback, NULL);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }

}
