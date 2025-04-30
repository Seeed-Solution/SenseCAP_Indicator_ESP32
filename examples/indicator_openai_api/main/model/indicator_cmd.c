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
} api_key_args;

static int openai_api_key_set(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &api_key_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, api_key_args.end, argv[0]);
        return 1;
    }

    char key_buf[MAX_API_KEY_LENGTH] = {0};
    int len = 0;

    if (api_key_args.key->count) {
        len = strlen(api_key_args.key->sval[0]);
        if(len >= MAX_API_KEY_LENGTH) {
            ESP_LOGE(TAG, "API key too long, max %d bytes", MAX_API_KEY_LENGTH - 1);
            return -1;
        }
        
        // 复制API Key并确保字符串正确终止
        strncpy(key_buf, api_key_args.key->sval[0], MAX_API_KEY_LENGTH - 1);
        key_buf[MAX_API_KEY_LENGTH - 1] = '\0';
        
        ESP_LOGI(TAG, "Writing API key");
        esp_err_t write_ret = indicator_storage_write(API_KEY_STORAGE, (void *)key_buf, len + 1);
        if (write_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to write API key (err: %d)", write_ret);
            return -1;
        }
        
        // 触发API Key重新读取事件
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, 
                         VIEW_EVENT_OPENAI_API_KEY_READ, NULL, 0, portMAX_DELAY);
    }

    // 读取存储的API Key进行验证
    len = sizeof(key_buf);
    memset(key_buf, 0, sizeof(key_buf));
    esp_err_t ret = indicator_storage_read(API_KEY_STORAGE, (void *)key_buf, &len);
    if (ret == ESP_OK && len > 0) {
        ESP_LOGI(TAG, "API key read successful");
    } else {
        ESP_LOGE(TAG, "API key read failed (err: %d)", ret);
    }
    
    return 0;
}

static struct {
    struct arg_str *url;
    struct arg_end *end;
} api_url_args;

static int openai_api_url_set(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &api_url_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, api_url_args.end, argv[0]);
        return 1;
    }

    char url_buf[MAX_API_URL_LENGTH] = {0};
    int len = 0;

    if (api_url_args.url->count) {
        len = strlen(api_url_args.url->sval[0]);
        if(len >= MAX_API_URL_LENGTH) {
            ESP_LOGE(TAG, "URL too long, max %d bytes", MAX_API_URL_LENGTH - 1);
            return -1;
        }
        
        // 复制URL并确保字符串正确终止
        strncpy(url_buf, api_url_args.url->sval[0], MAX_API_URL_LENGTH - 1);
        url_buf[MAX_API_URL_LENGTH - 1] = '\0';
        
        ESP_LOGI(TAG, "Writing API URL: %s", url_buf);
        esp_err_t write_ret = indicator_storage_write(API_URL_STORAGE, (void *)url_buf, len + 1);
        if (write_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to write API URL (err: %d)", write_ret);
            return -1;
        }
        
        // 触发URL重新读取事件
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, 
                         VIEW_EVENT_OPENAI_API_URL_READ, NULL, 0, portMAX_DELAY);
    }

    // 读取存储的URL进行验证
    len = sizeof(url_buf);
    memset(url_buf, 0, sizeof(url_buf));
    esp_err_t ret = indicator_storage_read(API_URL_STORAGE, (void *)url_buf, &len);
    if (ret == ESP_OK && len > 0) {
        ESP_LOGI(TAG, "Read API URL: %s", url_buf);
    } else {
        ESP_LOGE(TAG, "API URL read failed (err: %d)", ret);
    }
    
    return 0;
}

static struct {
    struct arg_str *model;
    struct arg_end *end;
} model_name_args;

/** model name set command **/
static int openai_model_name_set(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &model_name_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, model_name_args.end, argv[0]);
        return 1;
    }

    char model_buf[MAX_MODEL_NAME_LENGTH] = {0};
    int len = 0;

    if (model_name_args.model->count) {
        len = strlen(model_name_args.model->sval[0]);
        if(len >= MAX_MODEL_NAME_LENGTH) {
            ESP_LOGE(TAG, "Model name too long, max %d bytes", MAX_MODEL_NAME_LENGTH - 1);
            return -1;
        }
        
        // 复制模型名称并确保字符串正确终止
        strncpy(model_buf, model_name_args.model->sval[0], MAX_MODEL_NAME_LENGTH - 1);
        model_buf[MAX_MODEL_NAME_LENGTH - 1] = '\0';
        
        ESP_LOGI(TAG, "Writing model name: %s", model_buf);
        esp_err_t write_ret = indicator_storage_write(MODEL_NAME_STORAGE, (void *)model_buf, len + 1);
        if (write_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to write model name (err: %d)", write_ret);
            return -1;
        }
        
        // 触发模型名称重新读取事件
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, 
                         VIEW_EVENT_OPENAI_MODEL_NAME_READ, NULL, 0, portMAX_DELAY);
    }

    // 读取存储的模型名称进行验证
    len = sizeof(model_buf);
    memset(model_buf, 0, sizeof(model_buf));
    esp_err_t ret = indicator_storage_read(MODEL_NAME_STORAGE, (void *)model_buf, &len);
    if (ret == ESP_OK && len > 0) {
        ESP_LOGI(TAG, "Read model name: %s", model_buf);
    } else {
        ESP_LOGE(TAG, "Model name read failed (err: %d)", ret);
    }
    
    return 0;
}

/** 系统提示词设置相关参数结构体 **/
static struct {
    struct arg_str *prompt;
    struct arg_end *end;
} system_prompt_args;

/** 设置系统提示词的命令处理函数 **/
static int openai_system_prompt_set(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &system_prompt_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, system_prompt_args.end, argv[0]);
        return 1;
    }

    char prompt_buf[MAX_SYSTEM_PROMPT_LENGTH] = {0};
    int len = 0;

    if (system_prompt_args.prompt->count) {
        len = strlen(system_prompt_args.prompt->sval[0]);
        if(len >= MAX_SYSTEM_PROMPT_LENGTH) {
            ESP_LOGE(TAG, "System prompt too long, max %d bytes", MAX_SYSTEM_PROMPT_LENGTH - 1);
            return -1;
        }
        
        // 复制系统提示词并确保字符串正确终止
        strncpy(prompt_buf, system_prompt_args.prompt->sval[0], MAX_SYSTEM_PROMPT_LENGTH - 1);
        prompt_buf[MAX_SYSTEM_PROMPT_LENGTH - 1] = '\0';
        
        ESP_LOGI(TAG, "Writing system prompt: %s", prompt_buf);
        esp_err_t write_ret = indicator_storage_write(SYSTEM_PROMPT_STORAGE, (void *)prompt_buf, len + 1);
        if (write_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to write system prompt (err: %d)", write_ret);
            return -1;
        }
        
        // 触发系统提示词重新读取事件
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, 
                         VIEW_EVENT_OPENAI_SYSTEM_PROMPT_READ, NULL, 0, portMAX_DELAY);
    }

    // 读取存储的系统提示词进行验证
    len = sizeof(prompt_buf);
    memset(prompt_buf, 0, sizeof(prompt_buf));
    esp_err_t ret = indicator_storage_read(SYSTEM_PROMPT_STORAGE, (void *)prompt_buf, &len);
    if (ret == ESP_OK && len > 0) {
        ESP_LOGI(TAG, "Read system prompt: %s", prompt_buf);
    } else {
        ESP_LOGE(TAG, "System prompt read failed (err: %d)", ret);
    }
    
    return 0;
}

// openai_api -k sk-xxxx
static void register_openai_api_key(void)
{
    api_key_args.key =  arg_str0("k", NULL, "<k>", "set key, eg: sk-xxxx..., 164 bytes"); 
    api_key_args.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "openai_api",
        .help = "To set openai api key.",
        .hint = NULL,
        .func = &openai_api_key_set,
        .argtable = &api_key_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

// openai_url -u https://api.openai.com/v1
static void register_openai_api_url(void)
{
    api_url_args.url =  arg_str0("u", NULL, "<url>", "set openai api url, max 100 bytes"); 
    api_url_args.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "openai_url",
        .help = "To set openai api url.",
        .hint = NULL,
        .func = &openai_api_url_set,
        .argtable = &api_url_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

// openai_model "model_name"
static void register_openai_model_name(void)
{
    model_name_args.model =  arg_str1(NULL, NULL, "<model>", "set model name, max 50 bytes"); 
    model_name_args.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "openai_model",
        .help = "To set openai model name.",
        .hint = NULL,
        .func = &openai_model_name_set,
        .argtable = &model_name_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

// openai_prompt "prompt"
static void register_openai_system_prompt(void)
{
    system_prompt_args.prompt = arg_str1(NULL, NULL, "<prompt>", "set system prompt, max 256 bytes");
    system_prompt_args.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "openai_prompt",
        .help = "To set openai system prompt.",
        .hint = NULL,
        .func = &openai_system_prompt_set,
        .argtable = &system_prompt_args
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
    register_openai_api_url();
    register_openai_model_name();
    register_openai_system_prompt();

    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
    ESP_ERROR_CHECK(esp_console_start_repl(repl));

}
