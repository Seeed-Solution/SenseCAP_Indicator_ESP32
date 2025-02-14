#include "indicator_cmd.h"
#include "indicator_storage.h"
#include "indicator_openai.h"
#include "esp_log.h"
#include "esp_console.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "argtable3/argtable3.h"

static const char *TAG = "cmd";

#define PROMPT_STR "Indicator"

/** openai api key set command **/
static struct {
    struct arg_str *key;
    struct arg_end *end;
} openai_api_key_args;

static int openai_api_key_set(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &openai_api_key_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, openai_api_key_args.end, argv[0]);
        return 1;
    }
    char key_buf[165] = {0,};
    int len = 0;
    if (openai_api_key_args.key->count) {
        int len = strlen(openai_api_key_args.key->sval[0]);
        if( len > sizeof(key_buf)) {
            ESP_LOGE(TAG,  "out of 165 bytes :%s", openai_api_key_args.key->sval[0]);
            return -1;
        }
        strncpy( key_buf, openai_api_key_args.key->sval[0], len );

        ESP_LOGI(TAG,"wirte openai api key:%s", key_buf);
        indicator_storage_write(OPENAI_API_KEY_STORAGE, (void *)key_buf, sizeof(key_buf));
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_OPENAI_API_KEY_READ, NULL, 0, portMAX_DELAY);
    }
   
    len=sizeof(key_buf);
    memset(key_buf, 0, sizeof(key_buf));
    esp_err_t ret = indicator_storage_read(OPENAI_API_KEY_STORAGE, (void *)key_buf, &len);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG,"read openai api key:%s", key_buf);
	} else {
        ESP_LOGE(TAG, "openai api key read fail!");
	}
    return 0;
}

//openai_api -k sk-xxxx
static void register_openai_api_key(void)
{
    openai_api_key_args.key =  arg_str0("k", NULL, "<k>", "set key, eg: sk-xxxx..., 164 bytes"); 
    openai_api_key_args.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "openai_api",
        .help = "To set openai api key.",
        .hint = NULL,
        .func = &openai_api_key_set,
        .argtable = &openai_api_key_args
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

    register_openai_api_key();

    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
    ESP_ERROR_CHECK(esp_console_start_repl(repl));

}
