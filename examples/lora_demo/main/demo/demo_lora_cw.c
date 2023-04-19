
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "radio.h"
#include "bsp_board.h"

#define RF_FREQUENCY                                862000000 // Hz
#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         5         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false

static RadioEvents_t RadioEvents;

static const char *TAG = "app_main";

void demo_lora_cw(void)
{
    int cnt = 0;
    ESP_LOGI(TAG, "system start");
    ESP_LOGI(TAG, "demo lora Continuous Wave");
    bsp_sx126x_init();
    Radio.Init( &RadioEvents );
    
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetTxConfig( MODEM_LORA,22, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
    Radio.SetTxContinuousWave( RF_FREQUENCY, 22, 10000 );

    while(1) {
        printf("cnt: %d\n", cnt++);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}