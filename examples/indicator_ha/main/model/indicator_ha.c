#include "indicator_ha.h"
#include "cJSON.h"
#include "indicator_cmd.h"
#include "nvs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "lwip/dns.h"
#include "lwip/err.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

#include "ha_config.h"
#include "mqtt_client.h"

#define HA_CFG_STORAGE "ha-cfg"

static const char *TAG = "HA";

typedef struct ha_sensor_entity {
    int   index;
    char *key;
    char *topic;
    int   qos;
} ha_sensor_entity_t;

typedef struct ha_switch_entity {
    int   index;
    char *key;
    char *topic_set;
    char *topic_state;
    int   qos;
} ha_switch_entity_t;

// ui  {"index": 1, vaule: "27.2"}  <==MQTT==  topic:/xxx/state {"key": "27.2"}  HA
ha_sensor_entity_t ha_sensor_entites[CONFIG_HA_SENSOR_ENTITY_NUM];

// ui  {"index": 1, vaule: 1}  <==MQTT==  topic:/xxx/set    {"key": 1}  HA
// ui  {"index": 1, vaule: 1}  ==MQTT==>  topic:/xxx/state  {"key": 1}  HA
ha_switch_entity_t ha_switch_entites[CONFIG_HA_SWITCH_ENTITY_NUM];

/****************** MQTT Configutation ******************/
static void _mqtt_ha_start(instance_mqtt *instance);
static void __mqtt_ha_init(instance_mqtt *instance);
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

/*Instance*/
instance_mqtt          mqtt_ha_instance; // global entrance
static instance_mqtt_t instance_ptr = &mqtt_ha_instance;

static void ha_entites_init(void)
{
    // snesor entites init
    ha_sensor_entites[0].index       = 0;
    ha_sensor_entites[0].key         = CONFIG_SENSOR1_VALUE_KEY;
    ha_sensor_entites[0].topic       = CONFIG_SENSOR1_TOPIC_DATA;
    ha_sensor_entites[0].qos         = CONFIG_TOPIC_SENSOR_DATA_QOS;

    ha_sensor_entites[1].index       = 1;
    ha_sensor_entites[1].key         = CONFIG_SENSOR2_VALUE_KEY;
    ha_sensor_entites[1].topic       = CONFIG_SENSOR2_TOPIC_DATA;
    ha_sensor_entites[1].qos         = CONFIG_TOPIC_SENSOR_DATA_QOS;

    ha_sensor_entites[2].index       = 2;
    ha_sensor_entites[2].key         = CONFIG_SENSOR3_VALUE_KEY;
    ha_sensor_entites[2].topic       = CONFIG_SENSOR3_TOPIC_DATA;
    ha_sensor_entites[2].qos         = CONFIG_TOPIC_SENSOR_DATA_QOS;

    ha_sensor_entites[3].index       = 3;
    ha_sensor_entites[3].key         = CONFIG_SENSOR4_VALUE_KEY;
    ha_sensor_entites[3].topic       = CONFIG_SENSOR4_TOPIC_DATA;
    ha_sensor_entites[3].qos         = CONFIG_TOPIC_SENSOR_DATA_QOS;

    ha_sensor_entites[4].index       = 4;
    ha_sensor_entites[4].key         = CONFIG_SENSOR5_VALUE_KEY;
    ha_sensor_entites[4].topic       = CONFIG_SENSOR5_TOPIC_DATA;
    ha_sensor_entites[4].qos         = CONFIG_TOPIC_SENSOR_DATA_QOS;

    ha_sensor_entites[5].index       = 5;
    ha_sensor_entites[5].key         = CONFIG_SENSOR6_VALUE_KEY;
    ha_sensor_entites[5].topic       = CONFIG_SENSOR6_TOPIC_DATA;
    ha_sensor_entites[5].qos         = CONFIG_TOPIC_SENSOR_DATA_QOS;

    // switch entites init
    ha_switch_entites[0].index       = 0;
    ha_switch_entites[0].key         = CONFIG_SWITCH1_VALUE_KEY;
    ha_switch_entites[0].topic_set   = CONFIG_SWITCH1_TOPIC_SET;
    ha_switch_entites[0].topic_state = CONFIG_SWITCH1_TOPIC_STATE;
    ha_switch_entites[0].qos         = CONFIG_TOPIC_SWITCH_QOS;

    ha_switch_entites[1].index       = 1;
    ha_switch_entites[1].key         = CONFIG_SWITCH2_VALUE_KEY;
    ha_switch_entites[1].topic_set   = CONFIG_SWITCH2_TOPIC_SET;
    ha_switch_entites[1].topic_state = CONFIG_SWITCH2_TOPIC_STATE;
    ha_switch_entites[1].qos         = CONFIG_TOPIC_SWITCH_QOS;

    ha_switch_entites[2].index       = 2;
    ha_switch_entites[2].key         = CONFIG_SWITCH3_VALUE_KEY;
    ha_switch_entites[2].topic_set   = CONFIG_SWITCH3_TOPIC_SET;
    ha_switch_entites[2].topic_state = CONFIG_SWITCH3_TOPIC_STATE;
    ha_switch_entites[2].qos         = CONFIG_TOPIC_SWITCH_QOS;

    ha_switch_entites[3].index       = 3;
    ha_switch_entites[3].key         = CONFIG_SWITCH4_VALUE_KEY;
    ha_switch_entites[3].topic_set   = CONFIG_SWITCH4_TOPIC_SET;
    ha_switch_entites[3].topic_state = CONFIG_SWITCH4_TOPIC_STATE;
    ha_switch_entites[3].qos         = CONFIG_TOPIC_SWITCH_QOS;

    ha_switch_entites[4].index       = 4;
    ha_switch_entites[4].key         = CONFIG_SWITCH5_VALUE_KEY;
    ha_switch_entites[4].topic_set   = CONFIG_SWITCH5_TOPIC_SET;
    ha_switch_entites[4].topic_state = CONFIG_SWITCH5_TOPIC_STATE;
    ha_switch_entites[4].qos         = CONFIG_TOPIC_SWITCH_QOS;

    ha_switch_entites[5].index       = 6;
    ha_switch_entites[5].key         = CONFIG_SWITCH6_VALUE_KEY;
    ha_switch_entites[5].topic_set   = CONFIG_SWITCH6_TOPIC_SET;
    ha_switch_entites[5].topic_state = CONFIG_SWITCH6_TOPIC_STATE;
    ha_switch_entites[5].qos         = CONFIG_TOPIC_SWITCH_QOS;

    ha_switch_entites[6].index       = 6;
    ha_switch_entites[6].key         = CONFIG_SWITCH7_VALUE_KEY;
    ha_switch_entites[6].topic_set   = CONFIG_SWITCH7_TOPIC_SET;
    ha_switch_entites[6].topic_state = CONFIG_SWITCH7_TOPIC_STATE;
    ha_switch_entites[6].qos         = CONFIG_TOPIC_SWITCH_QOS;

    ha_switch_entites[7].index       = 7;
    ha_switch_entites[7].key         = CONFIG_SWITCH8_VALUE_KEY;
    ha_switch_entites[7].topic_set   = CONFIG_SWITCH8_TOPIC_SET;
    ha_switch_entites[7].topic_state = CONFIG_SWITCH8_TOPIC_STATE;
    ha_switch_entites[7].qos         = CONFIG_TOPIC_SWITCH_QOS;
}

int switch_state[CONFIG_HA_SWITCH_ENTITY_NUM];

static void ha_ctrl_cfg_restore(void)
{
    esp_err_t ret = 0;
    size_t    len = sizeof(switch_state);
    ret           = indicator_storage_read(HA_CFG_STORAGE, (void *)switch_state, &len);

    if (ret == ESP_OK && len == (sizeof(switch_state))) {
        ESP_LOGI(TAG, "cfg read successful");
    } else {
        // err or not find
        if (ret == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGI(TAG, "cfg not find");
        } else {
            ESP_LOGI(TAG, "cfg read err:%d", ret);
        }
        memset(switch_state, 0, sizeof(switch_state));
    }
}

static void ha_ctrl_cfg_save(void)
{
    esp_err_t ret = 0;
    ret           = indicator_storage_write(HA_CFG_STORAGE, (void *)switch_state, sizeof(switch_state));
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "cfg write err:%d", ret);
    } else {
        ESP_LOGI(TAG, "cfg write successful");
    }
}

static int mqtt_msg_handler(const char *p_topic, int topic_len, const char *p_data, int data_len)
{
    cJSON *root       = NULL;
    cJSON *cjson_item = NULL;

    root              = cJSON_ParseWithLength(p_data, data_len);
    if (root == NULL) {
        return -1;
    }
    struct view_data_ha_sensor_data sensor_data;
    struct view_data_ha_switch_data switch_data;

    memset(&sensor_data, 0, sizeof(sensor_data));
    memset(&switch_data, 0, sizeof(switch_data));

    for (int i = 0; i < CONFIG_HA_SENSOR_ENTITY_NUM; i++) {
        cjson_item = cJSON_GetObjectItem(root, ha_sensor_entites[i].key);
        if (cjson_item != NULL && cjson_item->valuestring != NULL && 0 == strncmp(p_topic, ha_sensor_entites->topic, topic_len)) {
            sensor_data.index = i;
            strncpy(sensor_data.value, cjson_item->valuestring, sizeof(sensor_data.value) - 1);
            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_SENSOR, &sensor_data, sizeof(sensor_data), portMAX_DELAY);
            // return 0;
        }
    }

    for (int i = 0; i < CONFIG_HA_SWITCH_ENTITY_NUM; i++) {
        cjson_item = cJSON_GetObjectItem(root, ha_switch_entites[i].key);
        if (cjson_item != NULL && 0 == strncmp(p_topic, ha_switch_entites->topic_set, topic_len)) {
            switch_data.index = i;
            switch_data.value = cjson_item->valueint;
            printf("valueint :%d", cjson_item->valueint);
            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_SWITCH_SET, &switch_data, sizeof(switch_data), portMAX_DELAY);
            // return 0;
        }
    }
prase_end:
    cJSON_Delete(root);
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t  event  = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int                      msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:

            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            instance_ptr->mqtt_connected_flag = true;

            for (int i = 0; i < CONFIG_HA_SENSOR_ENTITY_NUM; i++) {
                msg_id = esp_mqtt_client_subscribe(client, ha_sensor_entites[i].topic, ha_sensor_entites[i].qos);
                ESP_LOGI(TAG, "subscribe:%s, msg_id=%d", ha_sensor_entites[i].topic, msg_id);
            }

            for (int i = 0; i < CONFIG_HA_SWITCH_ENTITY_NUM; i++) {
                msg_id = esp_mqtt_client_subscribe(client, ha_switch_entites[i].topic_set, ha_switch_entites[i].qos);
                ESP_LOGI(TAG, "subscribe:%s, msg_id=%d", ha_switch_entites[i].topic_set, msg_id);
            }

            //  restore switch state for UI and HA.
            struct view_data_ha_switch_data switch_data;
            for (int i = 0; i < CONFIG_HA_SWITCH_ENTITY_NUM; i++) {
                switch_data.index = i;
                switch_data.value = switch_state[i];
                esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_SWITCH_SET, &switch_data, sizeof(switch_data), portMAX_DELAY);
            }

            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_HA_MQTT_CONNECTED, NULL, 0, portMAX_DELAY);

            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            instance_ptr->mqtt_connected_flag = false;
            
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            // msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            mqtt_msg_handler(event->topic, event->topic_len, event->data, event->data_len);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

static void __view_event_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
    switch (id) {
        case VIEW_EVENT_WIFI_ST: {
            ESP_LOGI(TAG, "event: VIEW_EVENT_WIFI_ST");
            struct view_data_wifi_st *p_st = (struct view_data_wifi_st *)event_data;
            if (p_st->is_network) {
                esp_event_post_to(mqtt_app_event_handle, MQTT_APP_EVENT_BASE, MQTT_APP_START, &instance_ptr, sizeof(instance_mqtt_t), portMAX_DELAY);
            } else {
                // shutdown mqtt?
            }
            break;
        }
        case VIEW_EVENT_SENSOR_DATA: {
            if (instance_ptr->mqtt_connected_flag == false) {
                break;
            }
            ESP_LOGI(TAG, "event: VIEW_EVENT_SENSOR_DATA");

            struct view_data_sensor_data *p_data = (struct view_data_sensor_data *)event_data;

            char data_buf[64];
            int  len = 0;
            memset(data_buf, 0, sizeof(data_buf));

            switch (p_data->sensor_type) {
                case SENSOR_DATA_CO2: {
                    len = snprintf(data_buf, sizeof(data_buf), "{\"%s\":\"%d\"}", CONFIG_SENSOR_BUILDIN_CO2_VALUE_KEY, (int)p_data->vaule);
                    esp_mqtt_client_publish(instance_ptr->mqtt_client, CONFIG_SENSOR_BUILDIN_TOPIC_DATA, data_buf, len, 0, 0);
                    break;
                }
                case SENSOR_DATA_TVOC: {
                    len = snprintf(data_buf, sizeof(data_buf), "{\"%s\":\"%d\"}", CONFIG_SENSOR_BUILDIN_TVOC_VALUE_KEY, (int)p_data->vaule);
                    esp_mqtt_client_publish(instance_ptr->mqtt_client, CONFIG_SENSOR_BUILDIN_TOPIC_DATA, data_buf, len, 0, 0);
                    break;
                }
                case SENSOR_DATA_TEMP: {
                    len = snprintf(data_buf, sizeof(data_buf), "{\"%s\":\"%.1f\"}", CONFIG_SENSOR_BUILDIN_TEMP_VALUE_KEY, p_data->vaule);
                    esp_mqtt_client_publish(instance_ptr->mqtt_client, CONFIG_SENSOR_BUILDIN_TOPIC_DATA, data_buf, len, 0, 0);
                    break;
                }
                case SENSOR_DATA_HUMIDITY: {
                    len = snprintf(data_buf, sizeof(data_buf), "{\"%s\":\"%d\"}", CONFIG_SENSOR_BUILDIN_HUMIDITY_VALUE_KEY, (int)p_data->vaule);
                    esp_mqtt_client_publish(instance_ptr->mqtt_client, CONFIG_SENSOR_BUILDIN_TOPIC_DATA, data_buf, len, 0, 0);
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case VIEW_EVENT_HA_SWITCH_ST: {
            if (instance_ptr->mqtt_connected_flag == false) {
                break;
            }
            ESP_LOGI(TAG, "event: VIEW_EVENT_HA_SWITCH_ST");

            struct view_data_ha_switch_data *p_data = (struct view_data_ha_switch_data *)event_data;

            char data_buf[64];
            int  len = 0;
            memset(data_buf, 0, sizeof(data_buf));

            switch (p_data->index) {
                case 0: {
                    len = snprintf(data_buf, sizeof(data_buf), "{\"%s\": %d}", CONFIG_SWITCH1_VALUE_KEY, (int)p_data->value);
                    esp_mqtt_client_publish(instance_ptr->mqtt_client, CONFIG_SWITCH1_TOPIC_STATE, data_buf, len, 0, 0);
                    break;
                }
                case 1: {
                    len = snprintf(data_buf, sizeof(data_buf), "{\"%s\": %d}", CONFIG_SWITCH2_VALUE_KEY, (int)p_data->value);
                    esp_mqtt_client_publish(instance_ptr->mqtt_client, CONFIG_SWITCH2_TOPIC_STATE, data_buf, len, 0, 0);
                    break;
                }
                case 2: {
                    len = snprintf(data_buf, sizeof(data_buf), "{\"%s\": %d}", CONFIG_SWITCH3_VALUE_KEY, (int)p_data->value);
                    esp_mqtt_client_publish(instance_ptr->mqtt_client, CONFIG_SWITCH3_TOPIC_STATE, data_buf, len, 0, 0);
                    break;
                }
                case 3: {
                    len = snprintf(data_buf, sizeof(data_buf), "{\"%s\": %d}", CONFIG_SWITCH4_VALUE_KEY, (int)p_data->value);
                    esp_mqtt_client_publish(instance_ptr->mqtt_client, CONFIG_SWITCH4_TOPIC_STATE, data_buf, len, 0, 0);
                    break;
                }
                case 4: {
                    len = snprintf(data_buf, sizeof(data_buf), "{\"%s\": %d}", CONFIG_SWITCH5_VALUE_KEY, (int)p_data->value);
                    esp_mqtt_client_publish(instance_ptr->mqtt_client, CONFIG_SWITCH5_TOPIC_STATE, data_buf, len, 0, 0);
                    break;
                }
                case 5: {
                    len = snprintf(data_buf, sizeof(data_buf), "{\"%s\": %d}", CONFIG_SWITCH6_VALUE_KEY, (int)p_data->value);
                    esp_mqtt_client_publish(instance_ptr->mqtt_client, CONFIG_SWITCH6_TOPIC_STATE, data_buf, len, 0, 0);
                    break;
                }
                case 6: {
                    len = snprintf(data_buf, sizeof(data_buf), "{\"%s\": %d}", CONFIG_SWITCH7_VALUE_KEY, (int)p_data->value);
                    esp_mqtt_client_publish(instance_ptr->mqtt_client, CONFIG_SWITCH7_TOPIC_STATE, data_buf, len, 0, 0);
                    break;
                }
                case 7: {
                    len = snprintf(data_buf, sizeof(data_buf), "{\"%s\": %d}", CONFIG_SWITCH8_VALUE_KEY, (int)p_data->value);
                    esp_mqtt_client_publish(instance_ptr->mqtt_client, CONFIG_SWITCH7_TOPIC_STATE, data_buf, len, 0, 0);
                    break;
                }
                default:
                    break;
            }

            if (p_data->value < CONFIG_HA_SWITCH_ENTITY_NUM) {
                switch_state[p_data->index] = p_data->value;
            }
            ha_ctrl_cfg_save(); // save switch state to flash
            break;
        }
        default:
            break;
    }
}

/****************** MQTT client init ******************/
static void __cfg_event_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
    ha_cfg_interface *p_cfg = NULL;
    switch (id) {
        case HA_CFG_SET:
            ESP_LOGI(TAG, "event: HA_CFG_BROKER_SET");
            // p_cfg = (ha_cfg_interface *)event_data;
            // ESP_LOGI(TAG, "| Broker Address               | %-40s |", p_cfg->broker_url);
            // ESP_LOGI(TAG, "| Client ID                    | %-40s |", p_cfg->client_id);
            // ESP_LOGI(TAG, "| MQTT username                | %-40s |", p_cfg->username);
            // ESP_LOGI(TAG, "| MQTT password                | %-40s |", p_cfg->password);
            // restart will run _mqtt_ha_start, read configuration from nvs
            esp_event_post_to(mqtt_app_event_handle, MQTT_APP_EVENT_BASE, MQTT_APP_RESTART, &instance_ptr, sizeof(instance_mqtt_t), portMAX_DELAY);
            break;
        default:
            break;
    }
}

static void _mqtt_ha_start(instance_mqtt *instance)
{
    if (get_mqtt_net_flag() == false) {
        return;
    }

    if (instance->mqtt_cfg != NULL) {
        free(instance->mqtt_cfg);
    }

    if(instance->mqtt_client != NULL)
    {
        esp_mqtt_client_stop(instance->mqtt_client);
        esp_mqtt_client_destroy(instance->mqtt_client);
    }

    // read from nvs
    ha_cfg_interface hf_cfg;
    ha_cfg_get(&hf_cfg);

    instance->mqtt_cfg  = (esp_mqtt_client_config_t *)malloc(sizeof(esp_mqtt_client_config_t));
    *instance->mqtt_cfg = (esp_mqtt_client_config_t){
        .broker.address.uri                  = hf_cfg.broker_url,
        .credentials.client_id               = hf_cfg.client_id,
        .credentials.username                = hf_cfg.username,
        .credentials.authentication.password = hf_cfg.password,
    };

    ESP_LOGI(TAG, "| Broker Address               | %-40s |", hf_cfg.broker_url);
    ESP_LOGI(TAG, "| Client ID                    | %-40s |", hf_cfg.client_id);
    ESP_LOGI(TAG, "| username                     | %-40s |", hf_cfg.username);
    ESP_LOGI(TAG, "| password                     | %-40s |", hf_cfg.password);

    instance->mqtt_client = esp_mqtt_client_init(instance->mqtt_cfg);
    esp_mqtt_client_register_event(instance->mqtt_client, ESP_EVENT_ANY_ID, instance->mqtt_event_handler, NULL);
    esp_mqtt_client_start(instance->mqtt_client);
}

static void __mqtt_ha_init(instance_mqtt *instance)
{
    if (instance == NULL) {
        ESP_LOGE(TAG, "instance is NULL");
        return;
    }

    instance->mqtt_name           = TAG;
    instance->mqtt_connected_flag = false;
    instance->mqtt_client         = NULL; // client handler, use for start and stop
    instance->mqtt_cfg            = NULL;
    instance->mqtt_event_handler  = mqtt_event_handler;
    instance->mqtt_starter        = _mqtt_ha_start;
}

/****************** Public Function Definitions ******************/
void mqtt_ha_init(void)
{
    mqtt_ha_instance.is_using = true;
    /*instance_ptr = &mqtt_ha_instance;*/
    __mqtt_ha_init(&mqtt_ha_instance);

    ESP_LOGI(TAG, "mqtt_ha_init");

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(cmd_cfg_event_handle,
                                                             CMD_CFG_EVENT_BASE, HA_CFG_SET,
                                                             __cfg_event_handler, NULL, NULL));
}

void ha_cfg_get(ha_cfg_interface *ha_cfg)
{
    int len = sizeof(ha_cfg_interface);
    memset(ha_cfg, 0, sizeof(ha_cfg_interface));
    esp_err_t err = indicator_storage_read(MQTT_HA_CFG_STORAGE, ha_cfg, &len);
    if (err == ESP_OK && len == sizeof(ha_cfg_interface)) {
        ESP_LOGI(TAG, "mqtt cfg read successful");
    } else {
        // err or not find
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGI(TAG, "mqtt cfg not find");
        } else {
            ESP_LOGI(TAG, "mqtt cfg read err:%d", err);
        }
        // using default setting
        strncpy(ha_cfg->broker_url, CONFIG_BROKER_URL, sizeof(ha_cfg->broker_url) - 1);
        strncpy(ha_cfg->client_id, CONFIG_MQTT_CLIENT_ID, sizeof(ha_cfg->client_id) - 1);
        strncpy(ha_cfg->username, CONFIG_MQTT_USERNAME, sizeof(ha_cfg->username) - 1);
        strncpy(ha_cfg->password, CONFIG_MQTT_PASSWORD, sizeof(ha_cfg->password) - 1);
    }
}
void ha_cfg_set(ha_cfg_interface *cfg)
{
    esp_err_t err = indicator_storage_write(MQTT_HA_CFG_STORAGE, cfg, sizeof(ha_cfg_interface));
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "cfg write err:%d", err);
    } else {
        ESP_LOGI(TAG, "cfg write successful");
    }
}

int indicator_ha_init(void)
{

    ha_ctrl_cfg_restore();
    ha_entites_init();
    mqtt_ha_init();

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_WIFI_ST,
                                                             __view_event_handler, NULL, NULL));

    // buildin sensor
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA,
                                                             __view_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_HA_SWITCH_ST,
                                                             __view_event_handler, NULL, NULL));
}
