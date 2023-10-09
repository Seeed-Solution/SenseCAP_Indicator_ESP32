#include "indicator_cmd.h"
#include "argtable3/argtable3.h"
#include "esp_console.h"
#include "esp_event.h"
#include "esp_event_base.h"
#include "esp_log.h"
#include "indicator_ha.h"
#include "indicator_storage.h"
#include "indicator_util.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


static const char *TAG = "CMD_RESP";

#define PROMPT_STR "Indicator"

ESP_EVENT_DEFINE_BASE(CMD_CFG_EVENT_BASE);
esp_event_loop_handle_t cmd_cfg_event_handle;

static ha_cfg_interface ha_cfg;

// Function to read and display the current contents of ha_cfg
static int read_ha_config(int argc, char **argv)
{
    ESP_LOGI(TAG, "| Broker Address               | %-40s |", ha_cfg.broker_url);
    ESP_LOGI(TAG, "| Client ID                    | %-40s |", ha_cfg.client_id);
    ESP_LOGI(TAG, "| MQTT username                | %-40s |", ha_cfg.username);
    ESP_LOGI(TAG, "| MQTT password                | %-40s |", ha_cfg.password);

    return 0;
}

// Register the "readconfig" console command
static void register_read_config(void)
{
    const esp_console_cmd_t cmd = {
        .command = "haconfig",
        .help    = "Read and display current configuration",
        .hint    = NULL,
        .func    = &read_ha_config,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

struct {
    struct arg_str *username;
    struct arg_str *password;
    struct arg_str *broker_url;
    struct arg_str *client_id;
    struct arg_end *end;
} mqtt_args;

// Function to set MQTT configuration
static int mqtt_config_set(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&mqtt_args);

    if (nerrors != 0) {
        arg_print_errors(stderr, mqtt_args.end, argv[0]);
        return 1;
    }

    if (!mqtt_args.username->count &&
        !mqtt_args.password->count &&
        !mqtt_args.broker_url->count &&
        !mqtt_args.client_id->count) {
        // None of the relevant arguments are provided
        // ESP_LOGI(TAG, "No MQTT configuration arguments provided.");
        return 0; // Exit without updating configuration
    }


    if (mqtt_args.username->count > 0) {
        memset(ha_cfg.username, 0, sizeof(ha_cfg.username));
        strncpy(ha_cfg.username, mqtt_args.username->sval[0], sizeof(ha_cfg.username) - 1);
        ESP_LOGI(TAG, "Set MQTT username: %s", ha_cfg.username);
    }

    if (mqtt_args.password->count > 0) {
        memset(ha_cfg.password, 0, sizeof(ha_cfg.password));
        strncpy(ha_cfg.password, mqtt_args.password->sval[0], sizeof(ha_cfg.password) - 1);
        ESP_LOGI(TAG, "Set MQTT password: %s", ha_cfg.password);
    }

    if (mqtt_args.broker_url->count > 0) {
        char *url = mqtt_args.broker_url->sval[0];

        // Check if "mqtt://" is not already present
        if (strncmp(url, "mqtt://", 7) != 0) {
            char modified_url[128]; // Adjust the size as needed

            // Add "mqtt://" to the beginning of the URL
            snprintf(modified_url, sizeof(modified_url), "mqtt://%s", url);

            // Check if the modified URL is valid
            if (isValidIP(modified_url + 7) || isValidDomain(modified_url + 7)) {
                memset(ha_cfg.broker_url, 0, sizeof(ha_cfg.broker_url));
                strncpy(ha_cfg.broker_url, modified_url, sizeof(ha_cfg.broker_url) - 1);
                ESP_LOGI(TAG, "Set MQTT broker URL: %s", ha_cfg.broker_url);
            } else {
                ESP_LOGE(TAG, "Invalid broker URL: %s", modified_url);
            }
        } else {
            // "mqtt://" is already present
            // Check if the URL is valid
            if (isValidIP(url + 7) || isValidDomain(url + 7)) {
                memset(ha_cfg.broker_url, 0, sizeof(ha_cfg.broker_url));
                strncpy(ha_cfg.broker_url, url, sizeof(ha_cfg.broker_url) - 1);
                ESP_LOGI(TAG, "Set MQTT broker URL: %s", ha_cfg.broker_url);
            } else {
                ESP_LOGE(TAG, "Invalid broker URL: %s", url);
            }
        }
    }

    if (mqtt_args.client_id->count > 0) {
        memset(ha_cfg.client_id, 0, sizeof(ha_cfg.client_id));
        strncpy(ha_cfg.client_id, mqtt_args.client_id->sval[0], sizeof(ha_cfg.client_id) - 1);
        ESP_LOGI(TAG, "Set MQTT client ID: %s", ha_cfg.client_id);
    }

    esp_event_post_to(cmd_cfg_event_handle, CMD_CFG_EVENT_BASE, HA_CFG_SET, &ha_cfg, sizeof(ha_cfg_interface), portMAX_DELAY);

    return 0;
}

// Register MQTT configuration command
static void register_mqtt_config(void)
{
    mqtt_args.username          = arg_str0("u", "usr", "<username>", "MQTT username");
    mqtt_args.password          = arg_str0("p", "psw", "<password>", "MQTT password");
    mqtt_args.broker_url        = arg_str0("a", "addr", "<broker_url>", "MQTT broker URL");
    mqtt_args.client_id         = arg_str0("c", "id", "<client_id>", "MQTT client ID");
    mqtt_args.end               = arg_end(4);

    const esp_console_cmd_t cmd = {
        .command  = "setmqtt",
        .help     = "Set MQTT configuration",
        .hint     = NULL,
        .func     = &mqtt_config_set,
        .argtable = &mqtt_args,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

int indicator_cmd_init(void)
{
    esp_console_repl_t       *repl            = NULL;
    esp_console_repl_config_t repl_config     = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    repl_config.prompt                        = PROMPT_STR ">";
    repl_config.max_cmdline_length            = 1024;

    esp_event_loop_args_t cmd_event_task_args = {
        .queue_size      = 2,
        .task_name       = "cmd_event_task",
        .task_priority   = uxTaskPriorityGet(NULL),
        .task_stack_size = 4096,
        .task_core_id    = tskNO_AFFINITY,
    };
    ESP_ERROR_CHECK(esp_event_loop_create(&cmd_event_task_args, &cmd_cfg_event_handle));

    ha_cfg_get(&ha_cfg);
    register_read_config();
    // register_set_broker_url();
    // register_mqtt_credentials();
    register_mqtt_config();

    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}