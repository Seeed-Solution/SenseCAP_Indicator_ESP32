#pragma once

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bmp3xx_init(uint8_t i2c_addr);
esp_err_t bmp3xx_read_data(float *pressure, float *temperature);

#ifdef __cplusplus
}
#endif
