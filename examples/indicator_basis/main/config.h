

#ifndef CONFIG_H
#define CONFIG_H


#include <stdbool.h>
#include <string.h> 
#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_event_base.h"
#include "bsp_board.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif


ESP_EVENT_DECLARE_BASE(VIEW_EVENT_BASE);

extern esp_event_loop_handle_t view_event_handle;



#ifdef __cplusplus
}
#endif

#endif
