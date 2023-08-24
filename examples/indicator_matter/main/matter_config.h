#ifndef MATTER_CONFIG_H
#define MATTER_CONFIG_H

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#include "esp_openthread_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define MATTER_UPDATE_INTERVAL_IN_SECONDS         10
#define CHIP_DEVICE_CONFIG_PRODUCT_NAME "SenseCAP Indicator"
#define CHIP_DEVICE_CONFIG_DEVICE_VENDOR_NAME "Seeed Studio"
#define CHIP_DEVICE_CONFIG_DEVICE_HARDWARE_VERSION_STRING "v1.0"
#define CHIP_DEVICE_CONFIG_USE_TEST_SETUP_PIN_CODE 20202021
#define CHIP_DEVICE_CONFIG_USE_TEST_SETUP_DISCRIMINATOR 0xF00
#define CHIP_DEVICE_CONFIG_DEVICE_PRODUCT_NAME "SenseCAP Indicator"
#define CHIP_DEVICE_CONFIG_DEFAULT_DEVICE_HARDWARE_VERSION_STRING "v1.0"

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#define ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG()                                                  \
    {                                                                                          \
        .radio_mode = RADIO_MODE_NATIVE,                                                       \
    }

#define ESP_OPENTHREAD_DEFAULT_HOST_CONFIG()                                                   \
    {                                                                                          \
        .host_connection_mode = HOST_CONNECTION_MODE_NONE,                                     \
    }

#define ESP_OPENTHREAD_DEFAULT_PORT_CONFIG()                                                   \
    {                                                                                          \
        .storage_partition_name = "ot_storage", .netif_queue_size = 10, .task_queue_size = 10, \
    }
#endif

#ifdef __cplusplus
}
#endif

#endif
