#include "indicator_cmd.h"
#include "indicator_storage_nvs.h"
#include "esp_log.h"
#include "esp_console.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "nvs_flash.h"
#include "argtable3/argtable3.h"
#include "RegionCommon.h"
#include "indicator_lorawan.h"

static const char *TAG = "cmd";

#define PROMPT_STR "Indicator"

static struct {
    struct arg_str *eui;
    struct arg_str *join_eui;
    struct arg_str *app_key;
    struct arg_str *dev_addr;
    struct arg_str *apps_key;
    struct arg_str *nwks_key;
    struct arg_end *end;
} lorawan_args;


static struct {
    struct arg_str *class;
    struct arg_str *region;
    struct arg_int *interval;
    struct arg_int *isotaa;
    struct arg_int *startup;
    struct arg_end *end;
} lorawan_test_args;


static int hexCharToInt(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        return 10 + (c - 'A');
    } else if (c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    }
    return -1;
}

static int hexStringToBytes(const char *hexStr, unsigned char *byteArray, size_t byteArraySize) {
    size_t strLen = strlen(hexStr);
    if (strLen % 2 != 0 || strLen / 2 != byteArraySize) {
        return 0;
    }
    for (size_t i = 0; i < strLen / 2; i++) {
        int highNibble = hexCharToInt(hexStr[i * 2]);
        int lowNibble = hexCharToInt(hexStr[i * 2 + 1]);

        if (highNibble == -1 || lowNibble == -1) {
            return 0;
        }
        byteArray[i] = (unsigned char)((highNibble << 4) | lowNibble);
    }
    return strLen / 2; 
}

static int lorawan_test(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &lorawan_test_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, lorawan_test_args.end, argv[0]);
        return 1;
    }
    
    struct view_data_lorawan_basic_cfg  cfg = {0};
    DeviceClass_t class = CLASS_A;
    LoRaMacRegion_t region = LORAMAC_REGION_US915;

    bool update_flag = false;
    uint8_t buf[32] = {0,};

    size_t len = sizeof(cfg);
    
    esp_err_t ret;
    ret = indicator_nvs_read( LORAWAN_BASIC_CFG_STORAGE, (void *)&cfg, &len);
    if( ret != ESP_OK ) {
        cfg.class = CLASS_A;
        cfg.IsOtaaActivation = true;
        cfg.region =  LORAMAC_REGION_US915;
        cfg.uplink_interval_min = 5;
    } else {
        lorawan_basic_cfg_printf(&cfg);
    }

    if (lorawan_test_args.class->count) {

        int len = strlen(lorawan_test_args.class->sval[0]);
        if( len != 1 ) {
            ESP_LOGE(TAG,  "Incorrect input parameters :%s", lorawan_args.eui->sval[0]);
            return -1;
        }
        char c = * (char * ) lorawan_test_args.class->sval[0];
        if( c == 'A'){
            class = CLASS_A;
        } else if( c == 'B') {
            class = CLASS_B;
        } else if( c == 'C') {
            class = CLASS_C;
        } else {
            ESP_LOGE(TAG,  "Incorrect input parameters :%s", lorawan_args.eui->sval[0]);
            return -1;
        }
        cfg.class = class;
        update_flag =  true;
    }

    if (lorawan_test_args.region->count) {

        int len = strlen(lorawan_test_args.region->sval[0]);
        if( len != 5 ) {
            ESP_LOGE(TAG,  "Incorrect input parameters :%s", lorawan_args.eui->sval[0]);
            return -1;
        }

        if( strcmp(lorawan_test_args.region->sval[0], "US915")== 0){
            region = LORAMAC_REGION_US915;
        } else if( strcmp(lorawan_test_args.region->sval[0], "EU868") == 0 ) {
            region = LORAMAC_REGION_EU868;
        } else if( strcmp(lorawan_test_args.region->sval[0], "AS923") == 0 ) {
            region = LORAMAC_REGION_AS923;
        } else if( strcmp(lorawan_test_args.region->sval[0], "IN865")== 0 ) {
            region = LORAMAC_REGION_IN865;
        } else if( strcmp(lorawan_test_args.region->sval[0], "AU915")== 0 ) {
            region = LORAMAC_REGION_AU915;
        } else if( strcmp(lorawan_test_args.region->sval[0], "CN470")== 0 ) {
            region = LORAMAC_REGION_CN470;
        } else if( strcmp(lorawan_test_args.region->sval[0], "CN779")== 0 ) {
            region = LORAMAC_REGION_CN779;
        } else if( strcmp(lorawan_test_args.region->sval[0], "EU433")== 0 ) {
            region = LORAMAC_REGION_EU433;
        } else if( strcmp(lorawan_test_args.region->sval[0], "KR920")== 0 ) {
            region = LORAMAC_REGION_KR920;
        } else if( strcmp(lorawan_test_args.region->sval[0], "RU864")== 0 ) {
            region = LORAMAC_REGION_RU864;
        } else {
            ESP_LOGE(TAG,  "Incorrect input parameters :%s", lorawan_args.eui->sval[0]);
            return -1;
        }

        cfg.region = region;
        update_flag =  true;
    }

    if (lorawan_test_args.interval->count) {
        update_flag =  true;
        cfg.uplink_interval_min = lorawan_test_args.interval->ival[0];
    }

    if (lorawan_test_args.isotaa->count) {
        update_flag =  true;
        cfg.IsOtaaActivation = lorawan_test_args.isotaa->ival[0];
    }

    if( update_flag ) {
        printf("\r\nUpdate lorawan basic cfg...\r\n\r\n");
        lorawan_basic_cfg_printf(&cfg);
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_CFG_APPLY, (void *)&cfg, sizeof(cfg), portMAX_DELAY);
    }

    if( lorawan_test_args.startup->count ) {
        bool startup = lorawan_test_args.startup->ival[0];
        if( startup) {
            printf("start lorawan\r\n");
        } else {
            printf("stop lorawan\r\n");
        }
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_JOIN_ACTION, (void *)&startup, sizeof(startup), portMAX_DELAY);
    }

    return 0;
}

static int lorawan_cfg(int argc, char **argv)
{
    
    int nerrors = arg_parse(argc, argv, (void **) &lorawan_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, lorawan_args.end, argv[0]);
        return 1;
    }

    struct view_data_lorawan_network_info  info = {0};

    uint8_t buf[32] = {0,};
    int len = 0;
    bool update_flag = false;
    esp_err_t ret;

    len = sizeof(info);
    memset(&info, 0, sizeof(info));
    ret = indicator_nvs_read(LORAWAN_NETWORK_INFO_STORAGE, (void *)&info, &len);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "lorawan info read fail!");
    } else {
        lorawan_network_info_printf( &info);
    }

    if (lorawan_args.eui->count) {
        int len = hexStringToBytes(lorawan_args.eui->sval[0], buf, 8 );
        if( len == 0  ) {
            ESP_LOGE(TAG,  "Incorrect input parameters :%s", lorawan_args.eui->sval[0]);
            return -1;
        }
        printf("New EUI: %s\r\n", lorawan_args.eui->sval[0]);
        update_flag =  true;
        memcpy( info.eui, buf, len );
    }

    if (lorawan_args.join_eui->count) {
        int len = hexStringToBytes(lorawan_args.join_eui->sval[0], buf, 8 );
        if( len == 0  ) {
            ESP_LOGE(TAG,  "Incorrect input parameters :%s", lorawan_args.join_eui->sval[0]);
            return -1;
        }
        printf("New JOIN EUI: %s\r\n", lorawan_args.join_eui->sval[0]);
        update_flag =  true;
        memcpy( info.join_eui, buf, len );
    }

    if (lorawan_args.app_key->count) {
        int len = hexStringToBytes(lorawan_args.app_key->sval[0], buf, 16 );
        if( len == 0  ) {
            ESP_LOGE(TAG,  "Incorrect input parameters :%s", lorawan_args.app_key->sval[0]);
            return -1;
        }
        printf("New APP KEY: %s\r\n", lorawan_args.app_key->sval[0]);
        update_flag =  true;
        memcpy( info.app_key, buf, len );
    }

    if (lorawan_args.dev_addr->count) {
        int len = hexStringToBytes(lorawan_args.dev_addr->sval[0], buf, 4 );
        if( len == 0  ) {
            ESP_LOGE(TAG,  "Incorrect input parameters :%s", lorawan_args.dev_addr->sval[0]);
            return -1;
        }
        printf("New DEV ADDR: %s\r\n", lorawan_args.dev_addr->sval[0]);
        update_flag =  true;
        memcpy( info.dev_addr, buf, len );
    }

    if (lorawan_args.apps_key->count) {
        int len = hexStringToBytes(lorawan_args.apps_key->sval[0], buf, 16 );
        if( len == 0  ) {
            ESP_LOGE(TAG,  "Incorrect input parameters :%s", lorawan_args.apps_key->sval[0]);
            return -1;
        }
        printf("New APPS KEY: %s\r\n", lorawan_args.apps_key->sval[0]);
        update_flag =  true;
        memcpy( info.apps_key, buf, len );
    }

    if (lorawan_args.nwks_key->count) {
        int len = hexStringToBytes(lorawan_args.nwks_key->sval[0], buf, 16 );
        if( len == 0  ) {
            ESP_LOGE(TAG,  "Incorrect input parameters :%s", lorawan_args.nwks_key->sval[0]);
            return -1;
        }
        printf("New NWKS KEY: %s\r\n", lorawan_args.nwks_key->sval[0]);
        update_flag =  true;
        memcpy( info.nwks_key, buf, len );
    }

    if(  update_flag ) {
        printf("\r\nUpdate lorawan network info...\r\n\r\n");
        lorawan_network_info_printf( &info);
        indicator_nvs_write(LORAWAN_NETWORK_INFO_STORAGE, (void *)&info, sizeof(info));
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_NETWORK_INFO_UPDATE, (void *)&info, sizeof(info), portMAX_DELAY);
    }
    return 0;
}

static void register_lorawan(void)
{
    lorawan_args.eui =  arg_str0(NULL, "eui", "", "set EUI, 8 hexadecimal data, MSB");
    lorawan_args.join_eui =  arg_str0(NULL, "join_eui", "", "set JOIN EUI or APP EUI, 8 hexadecimal data, MSB"); 
    lorawan_args.app_key =  arg_str0(NULL, "app_key", "", "set APP KEY, 16 hexadecimal data, MSB"); 
    lorawan_args.dev_addr =  arg_str0(NULL, "dev_addr", "", "set Device addr of ABP,  4 hexadecimal data, MSB"); 
    lorawan_args.apps_key =  arg_str0(NULL, "apps_key", "", "set APPS KEY of ABP,  16 hexadecimal data, MSB");
    lorawan_args.nwks_key =  arg_str0(NULL, "nwks_key", "", "set NWKS KEY of ABP,  16 hexadecimal data, MSB"); 
    lorawan_args.end = arg_end(6);

    const esp_console_cmd_t cmd = {
        .command = "lorawan",
        .help = "Set lorawan network info.",
        .hint = NULL,
        .func = &lorawan_cfg,
        .argtable = &lorawan_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static void register_lorawan_test(void)
{
    lorawan_test_args.class    = arg_str0("c", "CLASS",  "<A|B|C>", "Set the CLASS, default: A");
    lorawan_test_args.region   = arg_str0("r", "REGION", "<AS923|AU915|CN470|CN779|EU433|EU868|KR920|IN865|US915|RU864>", "Set the CLASS, default: US915");
    lorawan_test_args.interval = arg_int0("i", NULL, "", "Set the uplink interval(min), default: 5min");
    lorawan_test_args.isotaa   = arg_int0("j", NULL, "<0|1>", "Is Otaa Activation, default: 1");
    lorawan_test_args.startup  = arg_int0("s", NULL, "<0|1>", "Is startup lorawan, default: 0");
    lorawan_test_args.end = arg_end(5);

    const esp_console_cmd_t cmd = {
        .command = "lorawan_test",
        .help = "lorawan test",
        .hint = NULL,
        .func = &lorawan_test,
        .argtable = &lorawan_test_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}


int indicator_cmd_init(void)
{
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    repl_config.prompt = PROMPT_STR ">";
    repl_config.max_cmdline_length = 1024;

    register_lorawan();
    register_lorawan_test();

    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
    ESP_ERROR_CHECK(esp_console_start_repl(repl));

}
