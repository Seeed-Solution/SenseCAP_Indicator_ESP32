#include "indicator_sensor.h"

#include "view_data.h"
#include "ui.h"

// #define VIEW_DEBUG
#define BUF_SIZE 32

static const char *TAG = "sensor_view";

typedef struct SensorView {
    lv_obj_t *ui_lbl;
} SensorView;

static SensorView sensorPanel[ENUM_SENSOR_ALL] = {NULL};

static void format_sensor_data(char *buf, enum sensor_data_type sensor_type, const float data);

/**
 * @brief Get the data from RP2040
 * @attention called in indicator_view.c
 */
void view_event_update_present_sensorData(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
    if (id != VIEW_EVENT_SENSOR_DATA)
        return;
    // ESP_LOGI(TAG, "event: VIEW_EVENT_SENSOR_DATA");
    struct view_data_sensor_data *p_data = (struct view_data_sensor_data *)event_data;
    if (p_data == NULL) {
        ESP_LOGE(TAG, "event_data is NULL");
        return;
    }
    if (sensorPanel[p_data->sensor_type].ui_lbl == NULL) {
        ESP_LOGE(TAG, "SensePanel did't init completely");
        return;
    }
    char data_buf[BUF_SIZE];

    memset(data_buf, 0, sizeof(data_buf));

    format_sensor_data(data_buf, p_data->sensor_type, p_data->value);
#ifdef VIEW_DEBUG
    ESP_LOGI(TAG, "update %s:%s", SensorTypeStrings[p_data->sensor_type], data_buf);
#endif
    lv_port_sem_take();
    lv_label_set_text(sensorPanel[p_data->sensor_type].ui_lbl, data_buf);
    lv_port_sem_give();
}

// format sensor data according to sensor type
static void format_sensor_data(char *buf, enum sensor_data_type sensor_type, const float data)
{
    char *format_style;

    if (data < 0) {
        snprintf(buf, BUF_SIZE, "N/A");
        return;
    }

    switch (sensor_type) {
        case SENSOR_DATA_TEMP:
            format_style = "%.1f";
            break;
        case SENSOR_DATA_CO2:
        case SENSOR_DATA_TVOC:
        case SENSOR_DATA_HUMIDITY:
        default:
            format_style = "%.0f";
            break;
    }
    snprintf(buf, BUF_SIZE, format_style, data);
}

void view_sensor_init()
{
    if (ui_ScreenSensor == NULL) {
        ESP_LOGE(TAG, "Sensor Screen is not init");
        return;
    }
    sensorPanel[SENSOR_DATA_CO2].ui_lbl      = ui_LblDataCO2;
    sensorPanel[SENSOR_DATA_TVOC].ui_lbl     = ui_LblDataTVOC;
    sensorPanel[SENSOR_DATA_TEMP].ui_lbl     = ui_LblDataTmp;
    sensorPanel[SENSOR_DATA_HUMIDITY].ui_lbl = ui_LblDataHumi;

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA,
                                                             view_event_update_present_sensorData,
                                                             NULL, NULL));
}