#include "indicator_sensor.h"
#include "esp_sntp.h"
#include "freertos/semphr.h"
#include "indicator_time.h"
#include "esp32_rp2040.h"

#include "view_data.h"
#define sensor_model_DEBUG 0

// This defines an array of strings, one for each sensor type. The strings are defined in the `SENSOR_TYPE_LIST` macro.
#define X(type, str)       str,
const char *SensorTypeStrings[] = {
    SENSOR_TYPE_LIST};
#undef X

static const char *TAG = "sensor-model";

static SemaphoreHandle_t __g_sensors_data_mutex;

static SensorData allSensorData[ENUM_SENSOR_ALL];

static void processData(SensorData *sensor, const float data) // 函数指针，用于处理数据
{
    // Set the sensor value
    sensor->value = data;

    // Set the sensor status to OK
    sensor->status = SENSOR_STATUS_OK; // TODO: Check the data validity

    // Set the timestamp
    sensor->timeValid = isTimeValid();
    sensor->timeStamp = getTimestampSec();

    // Print the timestamp if it is valid
    // if (sensor->timeValid) {
    //     printTimeMs();
    // }
}

/**
 * @brief Updates the data for a specific type of sensor.
 *
 * @param type Type of sensor to update.
 * @param data New sensor data.
 * @return SENSOR_OK on success, otherwise an error code.
 */
int UpdateSensorData(const SensorType type, uint8_t *p_data)
{
    if (p_data == NULL && type >= ENUM_SENSOR_ALL) {
        return SENSOR_ERR_INVALID_TYPE;
    }

    if (xSemaphoreTake(__g_sensors_data_mutex, portMAX_DELAY) != pdTRUE) {
        return SENSOR_ERR_MUTEX_FAIL;
    }

    float sensorValue;
    memcpy(&sensorValue, p_data, sizeof(float));
#if sensor_model_DEBUG == 1
    ESP_LOGI(TAG, "%s: %.0f", getSensorName(type), sensorValue);
#endif
    processData(&allSensorData[type], sensorValue);

    xSemaphoreGive(__g_sensors_data_mutex);

    struct view_data_sensor_data v_data = {
        .sensor_type = type,
        .value       = sensorValue,
    };

    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA, /* Update sensor data for Sensor View */
                      &v_data, sizeof(struct view_data_sensor_data), portMAX_DELAY);

    return SENSOR_OK;
}


/**
 * @brief Initializes a SensorData structure.
 *
 * @param data Pointer to the SensorData structure
 * @param type Sensor type to initialize.
 * @return int SENSOR_OK on success, otherwise an error code.
 */
static int initSensorData(SensorData *sensor, SensorType type)
{
    if (sensor == NULL || type >= ENUM_SENSOR_ALL) {
        return SENSOR_ERR_INVALID_TYPE;
    }
    // init logic
    sensor->type   = type;
    sensor->value  = 0;
    sensor->status = SENSOR_STATUS_INITED;
    return SENSOR_OK;
}

int getSensorIntValue(const SensorType type)
{
    return (int)allSensorData[type].value;
}

float getSensorFloatValue(const SensorType type)
{
    return allSensorData[type].value;
}

uint64_t getSensorTimeStamp(const SensorType type)
{
    if (allSensorData[type].timeValid) {
        return allSensorData[type].timeStamp;
    }
    return 0;
}

char *getSensorName(const SensorType type)
{
    return SensorTypeStrings[type];
}

void indicator_sensor_init(void)
{
    __g_sensors_data_mutex = xSemaphoreCreateMutex();
    if (__g_sensors_data_mutex == NULL) {
        ESP_LOGE(TAG, "xSemaphoreCreateMutex failed");
    }

    for (int i = 0; i < ENUM_SENSOR_ALL; i++) {
        initSensorData(&allSensorData[i], i);
    }
}

SensorData *get_current_sensor_data(SensorData *data, SensorType type)
{
    xSemaphoreTake(__g_sensors_data_mutex, portMAX_DELAY);
    memcpy(data, &allSensorData[type], sizeof(SensorData));
    xSemaphoreGive(__g_sensors_data_mutex);
    return data;
}

/**
 * @brief Parse the data received from the RP2040
 *
 * @param p_data
 * @param len
 * @return int
 */
int __sensor_data_parse_handle(uint8_t *p_data, ssize_t len)
{
    if (len < sizeof(float) + 1) // Length check
    {
        // Handle error or return
        return -1;
    }

    uint8_t pkt_type = p_data[0];
    switch (pkt_type) {
            /*SCD41*/
        case PKT_TYPE_SENSOR_SCD41_TEMP:
            ESP_LOGI(TAG, "PKT_TYPE_SENSOR_SCD41_TEMP"); // Not Used
            UpdateSensorData(SENSOR_TEMP, (p_data + 1));
            break;
        case PKT_TYPE_SENSOR_SCD41_HUMIDITY:
            ESP_LOGI(TAG, "PKT_TYPE_SENSOR_SCD41_HUMIDITY"); // Not Used
            UpdateSensorData(SENSOR_HUMIDITY, (p_data + 1));
            break;
        case PKT_TYPE_SENSOR_SCD41_CO2:
            // ESP_LOGI(TAG, "PKT_TYPE_SENSOR_SCD41_CO2");
            UpdateSensorData(SENSOR_CO2, (p_data + 1));
            break;
            /*SHT41*/
        case PKT_TYPE_SENSOR_SHT41_TEMP:
            // ESP_LOGI(TAG, "PKT_TYPE_SENSOR_SHT41_TEMP");
            UpdateSensorData(SENSOR_TEMP, (p_data + 1));
            break;
        case PKT_TYPE_SENSOR_SHT41_HUMIDITY:
            // ESP_LOGI(TAG, "PKT_TYPE_SENSOR_SHT41_HUMIDITY");
            UpdateSensorData(SENSOR_HUMIDITY, (p_data + 1));
            break;

        case PKT_TYPE_SENSOR_TVOC_INDEX:
            UpdateSensorData(SENSOR_TVOC, (p_data + 1));
            break;

        default:
            // Handle unknown packet type
            break;
    }

    return 0;
}
