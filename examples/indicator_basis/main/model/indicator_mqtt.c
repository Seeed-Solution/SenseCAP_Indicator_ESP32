#include "indicator_mqtt.h"
#include "cJSON.h"
#include "nvs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "mqtt_client.h"


static const char *TAG = "mqtt";
static bool net_flag = false;

static esp_mqtt_client_handle_t mqtt_client;
static bool mqtt_connected_flag =  false;


static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static int mqtt_msg_handler(const char *p_topic, int topic_len, const char *p_data, int data_len)
{
    //todo
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            mqtt_connected_flag = true;

            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            mqtt_connected_flag = false;
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
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
                log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}


static void mqtt_start(void)
{
    static bool init_flag = false;
    if( init_flag ) {
        return;
    }
    init_flag = true;
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_BROKER_URL,
        .credentials.username = CONFIG_MQTT_USERNAME,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

static void __view_event_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    switch (id)
    {
        case VIEW_EVENT_WIFI_ST: {
            ESP_LOGI(TAG, "event: VIEW_EVENT_WIFI_ST");
            struct view_data_wifi_st *p_st = ( struct view_data_wifi_st *)event_data;
            if( p_st->is_network) {
                net_flag = true;
                mqtt_start();
            } else {
                net_flag = false;
            }
            break;
        }
        case VIEW_EVENT_SENSOR_DATA: {
            if( mqtt_connected_flag == false ) {
                break;
            }
            ESP_LOGI(TAG, "event: VIEW_EVENT_SENSOR_DATA");
            
            struct view_data_sensor_data  *p_data = (struct view_data_sensor_data *) event_data;

            char data_buf[64];
            int len = 0;
            memset(data_buf, 0, sizeof(data_buf));

            switch (p_data->sensor_type)
            {
                case SENSOR_DATA_CO2: {
                    len = snprintf(data_buf, sizeof(data_buf), "{\"%s\":\"%d\"}", CONFIG_SENSOR_CO2_VALUE_KEY, (int)p_data->vaule);
                    esp_mqtt_client_publish(mqtt_client, CONFIG_SENSOR_TOPIC_DATA, data_buf, len, 0, 0);
                    break;
                }
                case SENSOR_DATA_TVOC: {
                    len = snprintf(data_buf, sizeof(data_buf), "{\"%s\":\"%d\"}", CONFIG_SENSOR_TVOC_VALUE_KEY, (int)p_data->vaule);
                    esp_mqtt_client_publish(mqtt_client, CONFIG_SENSOR_TOPIC_DATA, data_buf, len, 0, 0);
                    break;
                }           
                case SENSOR_DATA_TEMP: {
                    len = snprintf(data_buf, sizeof(data_buf), "{\"%s\":\"%.1f\"}", CONFIG_SENSOR_TEMP_VALUE_KEY, p_data->vaule);
                    esp_mqtt_client_publish(mqtt_client, CONFIG_SENSOR_TOPIC_DATA, data_buf, len, 0, 0);
                    break;
                }
                case SENSOR_DATA_HUMIDITY: {
                    len = snprintf(data_buf, sizeof(data_buf), "{\"%s\":\"%d\"}", CONFIG_SENSOR_HUMIDITY_VALUE_KEY, (int)p_data->vaule);
                    esp_mqtt_client_publish(mqtt_client, CONFIG_SENSOR_TOPIC_DATA, data_buf, len, 0, 0);
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
}

int indicator_mqtt_init(void)
{

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, 
                                                        VIEW_EVENT_BASE, VIEW_EVENT_WIFI_ST, 
                                                        __view_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, 
                                                        VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA, 
                                                        __view_event_handler, NULL, NULL));

}

