#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"
#include "bsp_board.h"
#include "nvs_flash.h"
#include "cmd_sys.h"
#include "cmd_lora.h"
#include "cmd_wifi.h"

// #ifndef CONFIG_LCD_BOARD_SENSECAP_TERMINAL_WXM
//     #error "Must be a SenseCAP Terminal WXM Board"
// #endif

static const char* TAG = "example";
#define PROMPT_STR CONFIG_IDF_TARGET

void app_main(void)
{
    ESP_ERROR_CHECK(bsp_board_init());

    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    repl_config.prompt = PROMPT_STR ">";
    repl_config.max_cmdline_length = 1024;

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

    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
