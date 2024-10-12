#include <stdlib.h>
#include "driver/i2c.h"
#include <string.h>

#include "bsp_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EXPANDER_IO_RADIO_NSS                                   0
#define EXPANDER_IO_RADIO_RST                                   1
#define EXPANDER_IO_RADIO_BUSY                                  2
#define EXPANDER_IO_RADIO_DIO_1                                 3

#define ESP32_RADIO_MOSI        GPIO_NUM_48
#define ESP32_RADIO_MISO        GPIO_NUM_47
#define ESP32_RADIO_SCLK        GPIO_NUM_41
#define ESP32_EXPANDER_IO_INT   GPIO_NUM_42

extern io_expander_ops_t *indicator_io_expander;


void sx126x_init(void);




#ifdef __cplusplus
}
#endif
