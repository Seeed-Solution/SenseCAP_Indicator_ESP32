#ifndef LV_PORT_FS_H
#define LV_PORT_FS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "esp_err.h"
#include "lvgl/lvgl.h"

// LVGL version: 8.3.1 
// Note: call after lv_port_init()
esp_err_t lv_port_fs_init(void);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif