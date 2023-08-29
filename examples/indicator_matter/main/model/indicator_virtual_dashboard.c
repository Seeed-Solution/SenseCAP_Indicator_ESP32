#include "indicator_virtual_dashboard.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "freertos/semphr.h"

#define MATTER_DASHBOARD_STORAGE  "md"
static const char *TAG = "virtual_dashboard";

static SemaphoreHandle_t       __g_matter_virtual_dashboard_mutex;
static struct virtual_dashboard __g_virtual_dashboard;

static void __dashboard_data_set(struct virtual_dashboard *dashboard_data )
{
    xSemaphoreTake(__g_matter_virtual_dashboard_mutex, portMAX_DELAY);
    memcpy( &__g_virtual_dashboard, dashboard_data, sizeof(struct virtual_dashboard));
    xSemaphoreGive(__g_matter_virtual_dashboard_mutex);
}

void dashboard_data_get(struct virtual_dashboard *dashboard_data )
{
    xSemaphoreTake(__g_matter_virtual_dashboard_mutex, portMAX_DELAY);
    memcpy(dashboard_data, &__g_virtual_dashboard, sizeof(struct virtual_dashboard));
    xSemaphoreGive(__g_matter_virtual_dashboard_mutex);
}

static void __dashboard_data_save(struct virtual_dashboard *dashboard_data ) 
{
    esp_err_t ret = 0;
    ret = indicator_storage_write(MATTER_DASHBOARD_STORAGE, (void *)dashboard_data, sizeof(struct virtual_dashboard));
    if( ret != ESP_OK ) {
        ESP_LOGI(TAG, "matter dashboard storage write err:%d", ret);
    } else {
        ESP_LOGI(TAG, "matter dashboard storage write successful");
    }
}

static void __view_event_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    switch (id)
    {
        case VIEW_EVENT_MATTER_SET_DASHBOARD_DATA:
        case VIEW_EVENT_MATTER_DASHBOARD_DATA: {
            ESP_LOGI(TAG, "event: VIEW_EVENT_MATTER_DASHBOARD_DATA");

            struct view_data_matter_dashboard_data  *p_data = (struct view_data_matter_dashboard_data *) event_data;

            switch (p_data->dashboard_data_type)
            {
                case DASHBOARD_DATA_ARC: {
                        xSemaphoreTake(__g_matter_virtual_dashboard_mutex, portMAX_DELAY);
                        __g_virtual_dashboard.arc_value = p_data->value;
                        xSemaphoreGive(__g_matter_virtual_dashboard_mutex);
                    break;
                }
                case DASHBOARD_DATA_SWITCH: {
                        xSemaphoreTake(__g_matter_virtual_dashboard_mutex, portMAX_DELAY);
                        __g_virtual_dashboard.switch_state = (bool)p_data->value;
                        xSemaphoreGive(__g_matter_virtual_dashboard_mutex);
                    break;
                }
                case DASHBOARD_DATA_SLIDER: {
                        xSemaphoreTake(__g_matter_virtual_dashboard_mutex, portMAX_DELAY);
                        __g_virtual_dashboard.slider_value = p_data->value;
                        xSemaphoreGive(__g_matter_virtual_dashboard_mutex);
                    break;
                }
                case DASHBOARD_DATA_BUTTON1: {
                        xSemaphoreTake(__g_matter_virtual_dashboard_mutex, portMAX_DELAY);
                        __g_virtual_dashboard.button1 = (bool)p_data->value;
                        xSemaphoreGive(__g_matter_virtual_dashboard_mutex);
                    break;
                }
                case DASHBOARD_DATA_BUTTON2: {
                        xSemaphoreTake(__g_matter_virtual_dashboard_mutex, portMAX_DELAY);
                        __g_virtual_dashboard.button2 = (bool)p_data->value;
                        xSemaphoreGive(__g_matter_virtual_dashboard_mutex);
                    break;
                }
                default:
                    break;
            }

            xSemaphoreTake(__g_matter_virtual_dashboard_mutex, portMAX_DELAY);
            __dashboard_data_save(&__g_virtual_dashboard);
            xSemaphoreGive(__g_matter_virtual_dashboard_mutex);

            break;
        }
        default:
            break;
    }
}

static void __dashboard_data_restore()
{
    esp_err_t ret = 0;
    struct virtual_dashboard dashboard_data;
    
    size_t len = sizeof(dashboard_data);
    
    ret = indicator_storage_read(MATTER_DASHBOARD_STORAGE, (void *)&dashboard_data, &len);
    if( ret == ESP_OK  && len== (sizeof(dashboard_data)) ) {
        ESP_LOGI(TAG, "matter dashboard storage read successful");
        __dashboard_data_set(&dashboard_data);
    } else {
        // err or not find
        if( ret == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGI(TAG, "matter dashboard storage not found");
        }else {
            ESP_LOGI(TAG, "matter dashboard storage read err:%d", ret);
        } 
        
        dashboard_data.arc_value = 0;
        dashboard_data.switch_state = false;
        dashboard_data.button1 = false;
        dashboard_data.button2 = false;
        dashboard_data.slider_value = 0;
        __dashboard_data_set(&dashboard_data);
    }  
}

int indicator_virtual_dashboard_init(void)
{
    __g_matter_virtual_dashboard_mutex  =  xSemaphoreCreateMutex();  
    memset(&__g_virtual_dashboard, 0, sizeof(__g_virtual_dashboard));
    __dashboard_data_restore();

    ESP_LOGI(TAG, "Dimmer switch: esp_matter_attr_val_t value is %d", (int)__g_virtual_dashboard.button1);
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                        VIEW_EVENT_BASE, VIEW_EVENT_MATTER_DASHBOARD_DATA,
                                                        __view_event_handler, NULL, NULL));
}

