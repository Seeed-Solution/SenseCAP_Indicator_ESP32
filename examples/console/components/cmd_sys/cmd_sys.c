/* Console example — various sys commands

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
#include "esp_flash.h"
#include "argtable3/argtable3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cmd_sys.h"
#include "sdkconfig.h"
#include "bsp_board.h"
#include "esp_mac.h"
#include "bmp3xx.h"

static const char *TAG = "cmd_sys";

static void register_version(void);
static void register_restart(void);
static void register_restart_rp2040(void);
static void register_log_level(void);
static void register_read_mac();
static void register_read_bmp3xx(void);

void register_sys(void)
{
    register_version();
    register_restart();
    register_log_level();
    register_restart_rp2040();
    register_read_mac();
    register_read_bmp3xx();
}

/* 'version' command */
static int get_version(int argc, char **argv)
{
    const char *model;
    esp_chip_info_t info;
    uint32_t flash_size;
    esp_chip_info(&info);

    switch(info.model) {
        case CHIP_ESP32:
            model = "ESP32";
            break;
        case CHIP_ESP32S2:
            model = "ESP32-S2";
            break;
        case CHIP_ESP32S3:
            model = "ESP32-S3";
            break;
        case CHIP_ESP32C3:
            model = "ESP32-C3";
            break;
        default:
            model = "Unknown";
            break;
    }

    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return 1;
    }
    printf("IDF Version:%s\r\n", esp_get_idf_version());
    printf("Chip info:\r\n");
    printf("\tmodel:%s\r\n", model);
    printf("\tcores:%d\r\n", info.cores);
    printf("\tfeature:%s%s%s%s%d%s\r\n",
           info.features & CHIP_FEATURE_WIFI_BGN ? "/802.11bgn" : "",
           info.features & CHIP_FEATURE_BLE ? "/BLE" : "",
           info.features & CHIP_FEATURE_BT ? "/BT" : "",
           info.features & CHIP_FEATURE_EMB_FLASH ? "/Embedded-Flash:" : "/External-Flash:",
           flash_size / (1024 * 1024), " MB");
    printf("\trevision number:%d\r\n", info.revision);
    return 0;
}

static void register_version(void)
{
    const esp_console_cmd_t cmd = {
        .command = "version",
        .help = "Get version of chip and SDK",
        .hint = NULL,
        .func = &get_version,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/** 'restart' command restarts the program */
static int restart(int argc, char **argv)
{
    ESP_LOGI(TAG, "Restarting");
    esp_restart();
}

static void register_restart(void)
{
    const esp_console_cmd_t cmd = {
        .command = "restart",
        .help = "Software reset of the chip",
        .hint = NULL,
        .func = &restart,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/** 'restart rp2040' command restarts the program */
static int restart_rp2040(int argc, char **argv)
{
    ESP_LOGI(TAG, "Restarting");
#define EXPANDER_IO_RP2040_RESET   8
    board_res_desc_t *brd = bsp_board_get_description();
    brd->io_expander_ops->set_level(EXPANDER_IO_RP2040_RESET, 0);
    vTaskDelay( 100 / portTICK_PERIOD_MS);
    brd->io_expander_ops->set_level(EXPANDER_IO_RP2040_RESET, 1);

    return 0;
}

static void register_restart_rp2040(void)
{
    const esp_console_cmd_t cmd = {
        .command = "restart_rp2040",
        .help = "Reset of the rp2040 chip",
        .hint = NULL,
        .func = &restart_rp2040,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}


/** 'read mac' command restarts the program */
static int read_mac(int argc, char **argv)
{
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    printf("wifi_sta_mac: " MACSTR "\n", MAC2STR(mac));
    esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);
    printf("wifi_softap_mac: " MACSTR "\n", MAC2STR(mac));
    esp_read_mac(mac, ESP_MAC_BT);
    printf("bt_mac: " MACSTR "\n", MAC2STR(mac));
    return 0;
}

static void register_read_mac(void)
{
    const esp_console_cmd_t cmd = {
        .command = "read_mac",
        .help = "Read mac address",
        .hint = NULL,
        .func = &read_mac,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/** 'read bmp3xx sensor data' command restarts the program */
static int read_bmp3xx(int argc, char **argv)
{
    esp_err_t ret =0;
    float pressure = 0.0;
    float temperature = 0.0;
    ret = bmp3xx_read_data(&pressure, &temperature);
    if( ret == ESP_OK ) {
        printf("Data  T: %.2f deg C, P: %.2f Pa\n", temperature, pressure);
    } else {
        printf("Read Fail!\n");
    }
    return 0;
}

static void register_read_bmp3xx(void)
{
    const esp_console_cmd_t cmd = {
        .command = "read_bmp3xx",
        .help = "Read bmp3xx sensor data",
        .hint = NULL,
        .func = &read_bmp3xx,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/** log_level command changes log level via esp_log_level_set */
static struct {
    struct arg_str *tag;
    struct arg_str *level;
    struct arg_end *end;
} log_level_args;

static const char* s_log_level_names[] = {
    "none",
    "error",
    "warn",
    "info",
    "debug",
    "verbose"
};

static int log_level(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &log_level_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, log_level_args.end, argv[0]);
        return 1;
    }
    assert(log_level_args.tag->count == 1);
    assert(log_level_args.level->count == 1);
    const char* tag = log_level_args.tag->sval[0];
    const char* level_str = log_level_args.level->sval[0];
    esp_log_level_t level;
    size_t level_len = strlen(level_str);
    for (level = ESP_LOG_NONE; level <= ESP_LOG_VERBOSE; level++) {
        if (memcmp(level_str, s_log_level_names[level], level_len) == 0) {
            break;
        }
    }
    if (level > ESP_LOG_VERBOSE) {
        printf("Invalid log level '%s', choose from none|error|warn|info|debug|verbose\n", level_str);
        return 1;
    }
    if (level > CONFIG_LOG_MAXIMUM_LEVEL) {
        printf("Can't set log level to %s, max level limited in menuconfig to %s. "
               "Please increase CONFIG_LOG_MAXIMUM_LEVEL in menuconfig.\n",
               s_log_level_names[level], s_log_level_names[CONFIG_LOG_MAXIMUM_LEVEL]);
        return 1;
    }
    esp_log_level_set(tag, level);
    return 0;
}

static void register_log_level(void)
{
    log_level_args.tag = arg_str1(NULL, NULL, "<tag|*>", "Log tag to set the level for, or * to set for all tags");
    log_level_args.level = arg_str1(NULL, NULL, "<none|error|warn|debug|verbose>", "Log level to set. Abbreviated words are accepted.");
    log_level_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "log_level",
        .help = "Set log level for all tags or a specific tag.",
        .hint = NULL,
        .func = &log_level,
        .argtable = &log_level_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}