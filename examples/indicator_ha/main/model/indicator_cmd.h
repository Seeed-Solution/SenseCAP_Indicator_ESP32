#ifndef indicator_CMD_H
#define indicator_CMD_H

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

ESP_EVENT_DECLARE_BASE(CMD_CFG_EVENT_BASE);
extern esp_event_loop_handle_t cmd_cfg_event_handle;

int indicator_cmd_init(void);

#ifdef __cplusplus
}
#endif

#endif