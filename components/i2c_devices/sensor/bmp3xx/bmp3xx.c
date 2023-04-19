#include "esp_log.h"
#include "bsp_i2c.h"
#include "esp_check.h"
#include "bsp_board.h"
#include "bmp3xx.h"
#include "bmp3.h"

static const char *TAG = "bmp3xx";

static i2c_bus_device_handle_t bmp3xx_handle = NULL;
static struct bmp3_dev bmp3xx_dev;

BMP3_INTF_RET_TYPE bmp3_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    (void)intf_ptr;
    return i2c_bus_read_bytes(bmp3xx_handle, reg_addr, len, (uint8_t *)reg_data);

}

BMP3_INTF_RET_TYPE bmp3_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    (void)intf_ptr;
    return i2c_bus_write_bytes(bmp3xx_handle, reg_addr, len, (uint8_t *)(reg_data));
}
void bmp3_delay_us(uint32_t period, void *intf_ptr)
{
    (void)intf_ptr;
    // vTaskDelay(pdUS_TO_TICKS(period));
}

void bmp3_check_rslt(const char api_name[], int8_t rslt)
{
    switch (rslt)
    {
        case BMP3_OK:

            /* Do nothing */
            break;
        case BMP3_E_NULL_PTR:
            ESP_LOGE(TAG, "API [%s] Error [%d] : Null pointer", api_name, rslt);
            break;
        case BMP3_E_COMM_FAIL:
            ESP_LOGE(TAG, "API [%s] Error [%d] : Communication failure", api_name, rslt);
            break;
        case BMP3_E_INVALID_LEN:
            ESP_LOGE(TAG, "API [%s] Error [%d] : Incorrect length parameter", api_name, rslt);
            break;
        case BMP3_E_DEV_NOT_FOUND:
            ESP_LOGE(TAG, "API [%s] Error [%d] : Device not found", api_name, rslt);
            break;
        case BMP3_E_CONFIGURATION_ERR:
            ESP_LOGE(TAG, "API [%s] Error [%d] : Configuration Error", api_name, rslt);
            break;
        case BMP3_W_SENSOR_NOT_ENABLED:
            ESP_LOGE(TAG, "API [%s] Error [%d] : Warning when Sensor not enabled", api_name, rslt);
            break;
        case BMP3_W_INVALID_FIFO_REQ_FRAME_CNT:
            ESP_LOGE(TAG, "API [%s] Error [%d] : Warning when Fifo watermark level is not in limit", api_name, rslt);
            break;
        default:
            ESP_LOGE(TAG, "API [%s] Error [%d] : Unknown error code", api_name, rslt);
            break;
    }
}

BMP3_INTF_RET_TYPE bmp3_i2c_interface_init(struct bmp3_dev *bmp3, uint8_t addr)
{
    static uint8_t dev_addr;

    dev_addr = addr;
    bmp3->read = bmp3_i2c_read;
    bmp3->write = bmp3_i2c_write;
    bmp3->intf = BMP3_I2C_INTF;
    bmp3->delay_us = bmp3_delay_us;
    bmp3->intf_ptr = &dev_addr;
    return BMP3_OK;
}


esp_err_t bmp3xx_init(uint8_t i2c_addr)
{
    esp_err_t ret = ESP_OK;
    int8_t rslt;

    ESP_RETURN_ON_FALSE(NULL == bmp3xx_handle, ESP_FAIL, TAG, "bmp3xx already initialized");

    bsp_i2c_add_device(&bmp3xx_handle, i2c_addr);
    ESP_RETURN_ON_FALSE(NULL != bmp3xx_handle, ESP_FAIL, TAG, "add i2c bus device failed");
    
    rslt = bmp3_i2c_interface_init(&bmp3xx_dev, i2c_addr);
    
    rslt = bmp3_init(&bmp3xx_dev);
    bmp3_check_rslt("bmp3_init", rslt);
    if(rslt != BMP3_OK ) {
        ESP_LOGE(TAG, "BMP3xx init fail!");
        return ESP_FAIL;
    }
        
    struct bmp3_settings settings = { 0 };
    uint16_t settings_sel;

    settings.int_settings.drdy_en = BMP3_ENABLE;
    settings.press_en = BMP3_ENABLE;
    settings.temp_en = BMP3_ENABLE;

    settings.odr_filter.press_os = BMP3_OVERSAMPLING_2X;
    settings.odr_filter.temp_os = BMP3_OVERSAMPLING_2X;
    settings.odr_filter.odr = BMP3_ODR_100_HZ;

    settings_sel = BMP3_SEL_PRESS_EN | BMP3_SEL_TEMP_EN | BMP3_SEL_PRESS_OS | BMP3_SEL_TEMP_OS | BMP3_SEL_ODR |
                   BMP3_SEL_DRDY_EN;

    rslt = bmp3_set_sensor_settings(settings_sel, &settings, &bmp3xx_dev);
    bmp3_check_rslt("bmp3_set_sensor_settings", rslt);

    settings.op_mode = BMP3_MODE_NORMAL;
    rslt = bmp3_set_op_mode(&settings, &bmp3xx_dev);
    bmp3_check_rslt("bmp3_set_op_mode", rslt);

    ESP_LOGI(TAG, "BMP3xx initialized");
    return ESP_OK;
}

esp_err_t bmp3xx_read_data(float *pressure, float *temperature)
{
    ESP_RETURN_ON_FALSE(NULL != bmp3xx_handle, ESP_ERR_INVALID_STATE, TAG, "bmp3xx is not initialized");
    
    struct bmp3_status status = { { 0 } };
    struct bmp3_data data = { 0 };

    int8_t rslt;
    rslt = bmp3_get_status(&status, &bmp3xx_dev);
    if ((rslt == BMP3_OK) && (status.intr.drdy == BMP3_ENABLE)) {

        rslt = bmp3_get_sensor_data(BMP3_PRESS_TEMP, &data, &bmp3xx_dev);
        bmp3_check_rslt("bmp3_get_sensor_data", rslt);
        if(rslt != BMP3_OK ) {
            return ESP_FAIL;
        }

        /* NOTE : Read status register again to clear data ready interrupt status */
        rslt = bmp3_get_status(&status, &bmp3xx_dev);
        //bmp3_check_rslt("bmp3_get_status", rslt);

        *temperature = data.temperature;
        *pressure = data.pressure;
        #ifdef BMP3_FLOAT_COMPENSATION
            *temperature = data.temperature;
            *pressure = data.pressure;
            //printf("Data  T: %.2f deg C, P: %.2f Pa\n", (data.temperature), (data.pressure));
        #else
            *temperature = data.temperature /100.0;
            *pressure = data.pressure/ 100.0;
            //printf("Data  T: %ld deg C, P: %lu Pa\n", (long int)(int32_t)(data.temperature / 100),
                (long unsigned int)(uint32_t)(data.pressure / 100));
        #endif
        return ESP_OK;
    }
    return ESP_FAIL;
}

