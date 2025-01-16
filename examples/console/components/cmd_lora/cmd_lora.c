/* Console example — various lora commands

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "esp_log.h"
#include "esp_console.h"
#include "esp_chip_info.h"
// #include "driver/rtc_io.h"
#include "driver/uart.h"
#include "argtable3/argtable3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cmd_lora.h"
#include "sdkconfig.h"
#include "esp_random.h"

#include "radio.h"

#ifdef CONFIG_FREERTOS_USE_STATS_FORMATTING_FUNCTIONS
#define WITH_TASKS_INFO 1
#endif

static const char *TAG = "cmd_lora";

static void register_version(void);
static void register_restart(void);
static void register_log_level(void);


static uint8_t __g_tx_done_flag=true; 
static RadioEvents_t RadioEvents;
static void OnTxDone( void )
{
    ESP_LOGI(TAG, "OnTxDone");
    __g_tx_done_flag=true;
}
static void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    int i = 0;
    ESP_LOGI(TAG, "rssi:%d dBm, snr:%d dB, len:%d, payload:", rssi, snr, size);
    for( i = 0; i < size; i++) {
        printf("0x%x ",payload[i] );
    }
    printf("\n");
}
static void OnRxError()
{
    printf("OnRxError \n");
}


static void register_lora_tx_test();
static void register_lora_rx_test();
static void register_lora_cw_test();
static void register_lora_fcc_fhss_test();

void register_lora_test(void)
{
    register_lora_tx_test();
    register_lora_rx_test();
    register_lora_cw_test();
    register_lora_fcc_fhss_test();
}


void register_lora(void)
{
    register_lora_test();

    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.RxError = OnRxError;
    Radio.Init( &RadioEvents );

    Radio.SetChannel( 868000000 );
    Radio.SetTxConfig( MODEM_LORA, 10, 0, 0, 7, 1, 8, false, true, 0,0, false, 3000);
}

/* ---------------- lora ---------------------- */
/**  lora tx cw */

static struct {
    struct arg_int *freq;
    struct arg_int *power;
    struct arg_end *end;
} lora_tx_cw_args;



static int lora_cw_test(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &lora_tx_cw_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, lora_tx_cw_args.end, argv[0]);
        return 1;
    }

    int      power  = 10;
    uint32_t freq   = 868000000;

    if (lora_tx_cw_args.power->count) {
        power = lora_tx_cw_args.power->ival[0];
    }
    if (lora_tx_cw_args.freq->count) {
        freq = lora_tx_cw_args.freq->ival[0];
    }

    ESP_LOGI(TAG,  "Power          = %d dB", power);
    ESP_LOGI(TAG,  "Frequency      = %u Hz", freq);
    ESP_LOGI(TAG,  "Start Tx Continuous Wave");

    Radio.SetChannel( freq );
    Radio.SetTxContinuousWave( freq, power, -1 );
    return 0;
}

static void register_lora_cw_test(void)
{
    lora_tx_cw_args.freq =
        arg_int0("f", "freq", "<f>", "Set the radio frequency in Hz, range: 415000000 ~ 940000000 Hz, default: 868000000");
    lora_tx_cw_args.power =
        arg_int0("p", "power", "", "Set the radio power, LPA range: -17 ~ +14 dB, HPA range: -9 ~ +22 dB, default: 10");

    lora_tx_cw_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "lora_cw",
        .help = "Tx Continuous Wave",
        .hint = NULL,
        .func = &lora_cw_test,
        .argtable = &lora_tx_cw_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}


/**  lora tx */

static struct {
    struct arg_int *freq;
    struct arg_int *sf;
    struct arg_int *bw;
    struct arg_int *cr;
    struct arg_int *power;
    struct arg_int *crc;
    struct arg_int *iq;
    struct arg_int *public;
    struct arg_int *interval;
    struct arg_str *txt;
    struct arg_int *cnt;
    struct arg_end *end;
} lora_tx_args;


static int lora_tx_test(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &lora_tx_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, lora_tx_args.end, argv[0]);
        return 1;
    }

    int      power  = 10;
    uint32_t freq   = 868000000;
    uint32_t sf     = 7;
    uint32_t bw     = 0;
    uint32_t cr     = 1;
    uint32_t crc    = 1;
    uint32_t iq     = 0;
    uint32_t public = 0;
    uint32_t interval = 0;
    uint32_t cnt      = 1;
    char buf[255];
    uint32_t len =4;

    strcpy( buf, "hello");
    len= strlen("hello");

    if (lora_tx_args.power->count) {
        power = lora_tx_args.power->ival[0];
    }
    if (lora_tx_args.freq->count) {
        freq = lora_tx_args.freq->ival[0];
    }
    if (lora_tx_args.sf->count) {
        sf = lora_tx_args.sf->ival[0];
    }
    if (lora_tx_args.bw->count) {
        bw = lora_tx_args.bw->ival[0];
    }
    if (lora_tx_args.cr->count) {
        cr = lora_tx_args.cr->ival[0];
    }
    if (lora_tx_args.crc->count) {
        crc = lora_tx_args.crc->ival[0];
    }
    if (lora_tx_args.iq->count) {
        iq = lora_tx_args.iq->ival[0];
    }
    if (lora_tx_args.public->count) {
        public = lora_tx_args.public->ival[0];
    }
    if (lora_tx_args.interval->count) {
        interval = lora_tx_args.interval->ival[0];
    }
    if (lora_tx_args.cnt->count) {
        cnt = lora_tx_args.cnt->ival[0];
    }
    if (lora_tx_args.txt->count) {
        strncpy( buf, lora_tx_args.txt->sval[0],  sizeof(buf));
        len = strlen(lora_tx_args.txt->sval[0]);
    }

    ESP_LOGI(TAG,  "Power          = %d dB", power);
    ESP_LOGI(TAG,  "Frequency   = %u Hz", freq);
    ESP_LOGI(TAG,  "SF             = %d", sf);
    ESP_LOGI(TAG,  "BandWidth      = %d", bw);
    ESP_LOGI(TAG,  "CodingRate     = %d", cr);
    ESP_LOGI(TAG,  "CRC            = %d", crc);
    ESP_LOGI(TAG,  "IQ             = %d", iq);
    ESP_LOGI(TAG,  "Public         = %d", public);
    ESP_LOGI(TAG,  "Interval       = %d", interval);
    ESP_LOGI(TAG,  "Num            = %d", cnt);
    ESP_LOGI(TAG,  "tx len %d, payload: %s", len, buf);
    
    Radio.SetChannel( freq );
    Radio.SetTxConfig( MODEM_LORA, power, 0, bw,sf,cr, 8, false, crc, 0,0, iq, 3000);
    Radio.SetPublicNetwork(public);

    uint8_t quit=0;
    int recv_len = 0;
    uint32_t num = 0;
    __g_tx_done_flag = true;
    while(1) {
        ESP_LOGI(TAG, "send...%d", num++);
        recv_len = uart_read_bytes(0, &quit, 1,  1 / portTICK_PERIOD_MS);
        if( recv_len > 0) {
            if( quit==0x3) { //Ctrl+C
                break;
            }
        }

        while(__g_tx_done_flag == false) {
            vTaskDelay(20 / portTICK_PERIOD_MS);
            recv_len = uart_read_bytes(0, &quit, 1,  1 / portTICK_PERIOD_MS);
            if( recv_len > 0) {
                if( quit==0x3) { //Ctrl+C
                    break;
                }
            }
        };
        if( recv_len > 0) {
            if( quit==0x3) { //Ctrl+C
                break;
            }
        }


        __g_tx_done_flag = false;
        Radio.Send((uint8_t *)buf, len );

        if (interval) {
            vTaskDelay( interval / portTICK_PERIOD_MS);
        }

        if( cnt !=0  && num >= cnt) {
            break;
        }
    }
    
    //wait last tx done
    while(__g_tx_done_flag == false) {
        vTaskDelay(20 / portTICK_PERIOD_MS);
        recv_len = uart_read_bytes(0, &quit, 1,  1 / portTICK_PERIOD_MS);
        if( recv_len > 0) {
            if( quit==0x3) { //Ctrl+C
                break;
            }
        }
    };
    vTaskDelay( 1000 / portTICK_PERIOD_MS);

    Radio.Standby();
    ESP_LOGI(TAG, "End of send!");
    return 0;
}
static void register_lora_tx_test(void)
{
    lora_tx_args.freq =
        arg_int0("f", "freq", "<f>", "Set the radio frequency in Hz, range: 415000000 ~ 940000000 Hz, default: 868000000");
    lora_tx_args.sf =
        arg_int0("s", "sf", "<6~12>", "Set Lora SF, range: 6 ~ 12, default: 7");
    lora_tx_args.bw =
        arg_int0("b", "bw", "<0|1|2>", "Set Lora Bandwidth, 0:125KHz  1:250KHz  2:500KHz, default: 0");
    lora_tx_args.cr =
        arg_int0("c", "cr", "<1|2|3|4>", "Set Lora CodingRate, 1:CR_4_5  2:CR_4_6  3:CR_4_7  4:CR_4_8 , default: 1");
    lora_tx_args.power =
        arg_int0("p", "power", "", "Set the radio power, LPA range: -17 ~ +14 dB, HPA range: -9 ~ +22 dB, default: 10");
    lora_tx_args.crc =
        arg_int0(NULL, "crc", "<0|1>", "Set Lora CRC, 0:DISABLE  1:ENABLE, default: 1");
    lora_tx_args.iq =
        arg_int0(NULL, "iq", "<0|1>", "Set Lora IQ mode, 0:STANDARD  1:INVERTED, default: 0");
    lora_tx_args.public =
        arg_int0(NULL, "net", "<0|1>", "Set Public Network, 0: Private Network, 1: Public Network, default: 0");
    lora_tx_args.interval =
        arg_int0("i", "interval", "<t>", "Set the tx interval ms, default: 0");
    lora_tx_args.txt =
        arg_str0("d", "txt", "<d>", "Set the text data to send, default: hello");
    lora_tx_args.cnt =
        arg_int0("n", "num", "<n>", "Set Number of packets sent, 0: Keep sending, default: 1");

    lora_tx_args.end = arg_end(11);

    const esp_console_cmd_t cmd = {
        .command = "lora_tx",
        .help = "lora tx data",
        .hint = NULL,
        .func = &lora_tx_test,
        .argtable = &lora_tx_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/**  lora tx fhss */

#define FHSS_125K_MODE  0
#define FHSS_500K_MODE  1

#define BW125K_FREQ_INTERVAL        200000
#define BW500K_FREQ_INTERVAL_1      1600000
#define BW500K_FREQ_INTERVAL_2      600000

#define CH1_FREQ                    902300000
#define CH65_FREQ                   903000000
#define CH73_FREQ                   923300000

static struct {
    struct arg_int *mode;
    struct arg_int *sf;
    struct arg_int *cr;
    struct arg_int *power;
    struct arg_int *crc;
    struct arg_int *iq;
    struct arg_int *public;
    struct arg_int *interval;
    struct arg_str *txt;
    struct arg_end *end;
} lora_tx_fhss_args;

static uint32_t rand_freq[64] = {0};

static void rand_generate(uint8_t* buf, uint8_t count, uint8_t fhss_mode)
{
    uint8_t randTmp = 0;

    for (int i = 0; i < count; i++) {
LOOP:
        if(fhss_mode == FHSS_125K_MODE)
        {
            randTmp = (uint8_t) (esp_random() % 64 + 1) ;
        }
        else
        {
            randTmp = (uint8_t)  (esp_random() % 16 + 65); // (65, 80)
        }

        for (int j = i; j >= 0; j--)
        {
            if (buf[j] == randTmp)
            {
                goto LOOP;
            }
        }

        buf[i] = randTmp;
        //printf("%d ", randTmp);
    }
}
static int  freq_buf_generate(uint8_t  fhss_mode)
{
    uint8_t  rand_buff[64] = {0};
    uint32_t freq = 0;
    int freq_index = 0;
    int rand_index = 0;
    if( fhss_mode == FHSS_125K_MODE) {
        rand_generate(rand_buff, 64, fhss_mode);
        
        for( rand_index = 0; rand_index < 64; rand_index++) {
            freq = CH1_FREQ + (rand_buff[rand_index] - 1) * BW125K_FREQ_INTERVAL;
            rand_freq[freq_index++] = freq;
        }
        return freq_index;

    } else {
        rand_generate(rand_buff, 16, fhss_mode);

        for( rand_index = 0; rand_index < 16; rand_index++) {

            if(rand_buff[rand_index] < 73)
            {
                freq = CH65_FREQ + (rand_buff[rand_index] - 65 ) * BW500K_FREQ_INTERVAL_1;
            }
            else
            {
                freq = CH73_FREQ + (rand_buff[rand_index] - 73 ) * BW500K_FREQ_INTERVAL_2;
            }
            rand_freq[freq_index++] = freq;
        }
        return freq_index;
    }
}

static int lora_fcc_fhss_tx_test(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &lora_tx_fhss_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, lora_tx_fhss_args.end, argv[0]);
        return 1;
    }

    int      power  = 14;
    uint32_t freq   = CH1_FREQ;
    uint32_t sf     = 10;
    uint32_t mode   = FHSS_125K_MODE;
    uint32_t bw     = 0;
    uint32_t cr     = 1;
    uint32_t crc    = 1;
    uint32_t iq     = 0;
    uint32_t public = 0;
    uint32_t interval = 0;
    uint32_t cnt = 0;
    char buf[255];
    uint32_t len =4;

    strcpy( buf, "hello seeed! 1234567");
    len= strlen("hello seeed! 1234567");

    if (lora_tx_fhss_args.power->count) {
        power = lora_tx_fhss_args.power->ival[0];
    }

    if (lora_tx_fhss_args.mode->count) {
        mode = lora_tx_fhss_args.mode->ival[0];
    }
    if (lora_tx_fhss_args.sf->count) {
        sf = lora_tx_fhss_args.sf->ival[0];
    }
    if (lora_tx_fhss_args.cr->count) {
        cr = lora_tx_fhss_args.cr->ival[0];
    }
    if (lora_tx_fhss_args.crc->count) {
        crc = lora_tx_fhss_args.crc->ival[0];
    }
    if (lora_tx_fhss_args.iq->count) {
        iq = lora_tx_fhss_args.iq->ival[0];
    }
    if (lora_tx_fhss_args.public->count) {
        public = lora_tx_fhss_args.public->ival[0];
    }
    if (lora_tx_fhss_args.interval->count) {
        interval = lora_tx_fhss_args.interval->ival[0];
    }
    if (lora_tx_fhss_args.txt->count) {
        strncpy( buf, lora_tx_fhss_args.txt->sval[0],  sizeof(buf));
        len = strlen(lora_tx_fhss_args.txt->sval[0]);
    }

    if( mode == FHSS_125K_MODE) {
        bw = 0;
    } else {
        bw = 2;
    }
    memset(rand_freq, 0, sizeof(rand_freq));
    cnt = freq_buf_generate(mode);

    ESP_LOGI(TAG,  "Frequency  count: %d", cnt);
    for(int i = 0; i < cnt; i++) {
        printf("%d ", rand_freq[i]);
    }
    printf("Hz\r\n");
    ESP_LOGI(TAG,  "Power          = %d dB", power);
    ESP_LOGI(TAG,  "SF             = %d", sf);
    ESP_LOGI(TAG,  "BandWidth      = %d", bw);
    ESP_LOGI(TAG,  "CodingRate     = %d", cr);
    ESP_LOGI(TAG,  "CRC            = %d", crc);
    ESP_LOGI(TAG,  "IQ             = %d", iq);
    ESP_LOGI(TAG,  "Public         = %d", public);
    ESP_LOGI(TAG,  "Interval       = %d", interval);
    ESP_LOGI(TAG,  "Num            = %d", cnt);
    ESP_LOGI(TAG,  "tx len %d, payload: %s", len, buf);
    
    Radio.SetChannel( rand_freq[0] );
    Radio.SetTxConfig( MODEM_LORA, power, 0, bw,sf,cr, 8, false, crc, 0,0, iq, 3000);
    Radio.SetPublicNetwork(public);

    uint8_t quit=0;
    int recv_len = 0;
    uint32_t num = 0;
    __g_tx_done_flag = true;


    int index =0;

    while(1) {
        
        recv_len = uart_read_bytes(0, &quit, 1,  1 / portTICK_PERIOD_MS);
        if( recv_len > 0) {
            if( quit==0x3) { //Ctrl+C
                break;
            }
        }
        while(__g_tx_done_flag == false) {
            vTaskDelay(20 / portTICK_PERIOD_MS);
            recv_len = uart_read_bytes(0, &quit, 1,  1 / portTICK_PERIOD_MS);
            if( recv_len > 0) {
                if( quit==0x3) { //Ctrl+C
                    break;
                }
            }
        };
        if( recv_len > 0) {
            if( quit==0x3) { //Ctrl+C
                break;
            }
        }

        if( index >= cnt) {
            break;
        }

        
        
        __g_tx_done_flag = false;
        freq = rand_freq[index];
        
        ESP_LOGI(TAG, "freq:%d, send...%d", freq, index);

        Radio.SetChannel( freq );
        Radio.Send((uint8_t *)buf, len );

        if (interval) {
            vTaskDelay( interval / portTICK_PERIOD_MS);
        }
        index++;
    }

    Radio.Standby();
    ESP_LOGI(TAG, "End of send!");
    return 0;
}

static void register_lora_fcc_fhss_test(void)
{
    lora_tx_fhss_args.mode =
        arg_int0("m", "mode", "<0|1>", "Set the fhss mode, 0: FHSS_125K_MODE, 1: FHSS_500K_MODE, default: 0");
    lora_tx_fhss_args.sf =
        arg_int0("s", "sf", "<6~12>", "Set Lora SF, range: 6 ~ 12, default: 10");
    lora_tx_fhss_args.cr =
        arg_int0("c", "cr", "<1|2|3|4>", "Set Lora CodingRate, 1:CR_4_5  2:CR_4_6  3:CR_4_7  4:CR_4_8 , default: 1");
    lora_tx_fhss_args.power =
        arg_int0("p", "power", "", "Set the radio power, LPA range: -17 ~ +14 dB, HPA range: -9 ~ +22 dB, default: 14");
    lora_tx_fhss_args.crc =
        arg_int0(NULL, "crc", "<0|1>", "Set Lora CRC, 0:DISABLE  1:ENABLE, default: 1");
    lora_tx_fhss_args.iq =
        arg_int0(NULL, "iq", "<0|1>", "Set Lora IQ mode, 0:STANDARD  1:INVERTED, default: 0");
    lora_tx_fhss_args.public =
        arg_int0(NULL, "net", "<0|1>", "Set Public Network, 0: Private Network, 1: Public Network, default: 0");
    lora_tx_fhss_args.interval =
        arg_int0("i", "interval", "<t>", "Set the tx interval ms, default: 0");
    lora_tx_fhss_args.txt =
        arg_str0("d", "txt", "<d>", "Set the text data to send, default: hello seeed! 1234567");
    lora_tx_fhss_args.end = arg_end(10);

    const esp_console_cmd_t cmd = {
        .command = "lora_fcc_fhss",
        .help = "lora fcc fhss tx test",
        .hint = NULL,
        .func = &lora_fcc_fhss_tx_test,
        .argtable = &lora_tx_fhss_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}


/**  lora rx */

static struct {
    struct arg_int *freq;
    struct arg_int *sf;
    struct arg_int *bw;
    struct arg_int *cr;
    struct arg_int *crc;
    struct arg_int *iq;
    struct arg_int *public;
    struct arg_int *boosted;
    struct arg_end *end;
} lora_rx_args;


static int lora_rx_test(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &lora_rx_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, lora_rx_args.end, argv[0]);
        return 1;
    }

    uint32_t freq   = 868000000;
    uint32_t sf     = 7;
    uint32_t bw     = 0;
    uint32_t cr     = 1;
    uint32_t crc    = 1;
    uint32_t iq     = 0;
    uint32_t public = 0;
    uint32_t boosted= 0;
    
    if (lora_rx_args.freq->count) {
        freq = lora_rx_args.freq->ival[0];
    }
    if (lora_rx_args.sf->count) {
        sf = lora_rx_args.sf->ival[0];
    }
    if (lora_rx_args.bw->count) {
        bw = lora_rx_args.bw->ival[0];
    }
    if (lora_rx_args.cr->count) {
        cr = lora_rx_args.cr->ival[0];
    }
    if (lora_rx_args.crc->count) {
        crc = lora_rx_args.crc->ival[0];
    }
    if (lora_rx_args.iq->count) {
        iq = lora_rx_args.iq->ival[0];
    }
    if (lora_rx_args.public->count) {
        public = lora_rx_args.public->ival[0];
    }
    if (lora_rx_args.boosted->count) {
        boosted = lora_rx_args.boosted->ival[0];
    }

    ESP_LOGI(TAG,  "Frequency   = %u Hz", freq);
    ESP_LOGI(TAG,  "SF             = %d", sf);
    ESP_LOGI(TAG,  "BandWidth      = %d", bw);
    ESP_LOGI(TAG,  "CodingRate     = %d", cr);
    ESP_LOGI(TAG,  "CRC            = %d", crc);
    ESP_LOGI(TAG,  "IQ             = %d", iq);
    ESP_LOGI(TAG,  "Public         = %d", public);
    
    Radio.SetChannel( freq );
    Radio.SetRxConfig( MODEM_LORA, bw, sf,cr, 0, 8,
                                   5, false,
                                   0, crc, 0, 0, iq, true);
    Radio.SetMaxPayloadLength( MODEM_LORA, 255 );
    Radio.SetPublicNetwork(public);

    ESP_LOGI(TAG,  "Start Recv data...");
    if( boosted ) {
        ESP_LOGI(TAG,  "rx boosted...");
        Radio.RxBoosted( 0 );
    } else {
        Radio.Rx( 0 );
    }

    uint8_t quit=0;
    int len = 0;
    while(1) {
        len = uart_read_bytes(0, &quit, 1,  10 / portTICK_PERIOD_MS);
        if( len > 0) {
            if( quit==0x3) { //Ctrl+C
                break;
            }
        }
    }
    Radio.Standby();
    return 0;
}

static void register_lora_rx_test(void)
{
    lora_rx_args.freq =
        arg_int0("f", "freq", "<f>", "Set the radio frequency in Hz, range: 415000000 ~ 940000000 Hz, default: 868000000");
    lora_rx_args.sf =
        arg_int0("s", "sf", "<6~12>", "Set Lora SF, range: 6 ~ 12, default: 7");
    lora_rx_args.bw =
        arg_int0("b", "bw", "<0|1|2>", "Set Lora Bandwidth, 0:125KHz  1:250KHz  2:500KHz, default: 0");
    lora_rx_args.cr =
        arg_int0("c", "cr", "<1|2|3|4>", "Set Lora CodingRate, 1:CR_4_5  2:CR_4_6  3:CR_4_7  4:CR_4_8 , default: 1");
    lora_rx_args.crc =
        arg_int0(NULL, "crc", "<0|1>", "Set Lora CRC, 0:DISABLE  1:ENABLE, default: 1");
    lora_rx_args.iq =
        arg_int0(NULL, "iq", "<0|1>", "Set Lora IQ mode, 0:STANDARD  1:INVERTED, default: 0");
    lora_rx_args.public =
        arg_int0(NULL, "net", "<0|1>", "Set Public Network, 0: Private Network, 1: Public Network, default: 0");
    lora_rx_args.boosted =
        arg_int0(NULL, "boosted", "<0|1>", "1: Boosted RX, 0: Normal RX, default: 0");

    lora_rx_args.end = arg_end(8);

    const esp_console_cmd_t cmd = {
        .command = "lora_rx",
        .help = "lora rx data",
        .hint = NULL,
        .func = &lora_rx_test,
        .argtable = &lora_rx_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}



