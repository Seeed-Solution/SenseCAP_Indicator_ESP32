/**
 * @file beep.c
 * @brief get cmd via LoRa and beep
 * @version 1.0
 * @date 2023-08-09
 * @author @Love4yzp
 */
#include "bsp_board.h"
#include "cobs.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "radio.h"

static const char *TAG = "app_beep";

#define RF_FREQUENCY 868000000         // Hz

#define TX_OUTPUT_POWER 14             // dBm

#define LORA_BANDWIDTH             0   // [0: 125 kHz,
                                       //  1: 250 kHz,
                                       //  2: 500 kHz,
                                       //  3: Reserved]
#define LORA_SPREADING_FACTOR      12  // [SF7..SF12]
#define LORA_CODINGRATE            1   // [1: 4/5,
                                       //  2: 4/6,
                                       //  3: 4/7,
                                       //  4: 4/8]
#define LORA_PREAMBLE_LENGTH       8   // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT        5   // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON       false

#define RX_TIMEOUT_VALUE 1000
#define BUFFER_SIZE      10  // Define the payload size here

uint16_t BufferSize = BUFFER_SIZE;
uint8_t Buffer[BUFFER_SIZE];

static RadioEvents_t RadioEvents;

/**
 * @brief packet type With RP2040
 */
#define ESP32_RP2040_TXD ( 19 )
#define ESP32_RP2040_RXD ( 20 )
#define ESP32_RP2040_RTS ( UART_PIN_NO_CHANGE )
#define ESP32_RP2040_CTS ( UART_PIN_NO_CHANGE )

#define ESP32_COMM_PORT_NUM               ( 2 )
#define ESP32_COMM_BAUD_RATE              ( 115200 )
#define ESP32_RP2040_COMM_TASK_STACK_SIZE ( 1024 * 4 )
#define BUF_SIZE                          ( 512 )
uint8_t buf[BUF_SIZE];   // recv
uint8_t data[BUF_SIZE];  // decode

enum pkt_type {
    PKT_TYPE_ACK          = 0x00,  // uin32_t
    PKT_TYPE_CMD_BEEP_ON  = 0xA1,  // uin32_t  ms: on time
    PKT_TYPE_CMD_BEEP_OFF = 0xA2,
};
static int __cmd_send( uint8_t cmd, void *p_data, uint8_t len ) {
    uint8_t buf[32]  = { 0 };
    uint8_t data[32] = { 0 };

    if ( len > 31 ) {
        return -1;
    }

    uint8_t index = 1;

    data[0] = cmd;

    if ( len > 0 && p_data != NULL ) {
        memcpy( &data[1], p_data, len );
        index += len;
    }
    cobs_encode_result ret = cobs_encode( buf, sizeof( buf ), data, index );
#if SENSOR_COMM_DEBUG
    ESP_LOGI( TAG, "encode status:%d, len:%d", ret.status, ret.out_len );
    for ( int i = 0; i < ret.out_len; i++ ) {
        printf( "0x%x ", buf[i] );
    }
    printf( "\r\n" );
#endif

    if ( ret.status == COBS_ENCODE_OK ) {
        return uart_write_bytes( ESP32_COMM_PORT_NUM, buf, ret.out_len + 1 );
    }

    return -1;
}

/**
 * @brief prase payload and beep
 * @attention RP2040 should be also configured, the file is as BEEP_RP2040.ino
 * @link https://github.com/Seeed-Solution/SenseCAP_Indicator_RP2040/
 */
void beep_act( uint8_t *payload, uint8_t size ) {
    int ret;
    if ( size > 0 ) {
        ESP_LOGI( TAG, "Received: %s", payload );
        if ( strcmp( (char *)payload, "on" ) || strcmp( (char *)payload, "ON" ) == 0 ) {
            ESP_LOGI( TAG, "BEEP ON" );
            ret = __cmd_send( PKT_TYPE_CMD_BEEP_ON, NULL, 0 );
        } else if ( strcmp( (char *)payload, "off" ) || strcmp( (char *)payload, "OFF" ) == 0 ) {
            ESP_LOGI( TAG, "BEEP OFF" );
            ret = __cmd_send( PKT_TYPE_CMD_BEEP_OFF, NULL, 0 );
        } else {
            ESP_LOGI( TAG, "Unknown command" );
        }
    }

    if ( ret <= 0 ) {
        ESP_LOGI( TAG, "operator failed %d", ret );
    }
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ) {
    int i = 0;
    ESP_LOGI( TAG, "rssi:%d dBm, snr:%d dB, len:%d, payload:", rssi, snr, size );
    for ( i = 0; i < size; i++ ) {
        printf( "0x%x ", payload[i] );
    }
    printf( "\n" );

    BufferSize = size;
    memset( Buffer, 0, BUFFER_SIZE );
    if ( BufferSize > BUFFER_SIZE ) {
        BufferSize = BUFFER_SIZE;
    }
    memcpy( Buffer, payload, BufferSize );

    beep_act( Buffer, BufferSize );
}

void func_beep( void ) {
    ESP_LOGI( TAG, "board initialization" );
    ESP_ERROR_CHECK( bsp_board_init() );

    RadioEvents.RxDone = OnRxDone;

    Radio.Init( &RadioEvents );

    Radio.SetChannel( RF_FREQUENCY );

    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                       LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                       LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                       0, true, 0, 0, LORA_IQ_INVERSION_ON, true );

    Radio.SetMaxPayloadLength( MODEM_LORA, BUFFER_SIZE );

    Radio.Rx( RX_TIMEOUT_VALUE );

    uart_config_t uart_config = {
        .baud_rate  = ESP32_COMM_BAUD_RATE,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK( uart_driver_install( ESP32_COMM_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags ) );
    ESP_ERROR_CHECK( uart_param_config( ESP32_COMM_PORT_NUM, &uart_config ) );
    ESP_ERROR_CHECK( uart_set_pin( ESP32_COMM_PORT_NUM, ESP32_RP2040_TXD, ESP32_RP2040_RXD, ESP32_RP2040_RTS, ESP32_RP2040_CTS ) );

    for ( ;; ) {
        vTaskDelay( 10000 / portTICK_PERIOD_MS );
    }
}
