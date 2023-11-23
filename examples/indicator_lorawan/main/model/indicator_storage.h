#ifndef INDICATOR_STORAGE_H
#define INDICATOR_STORAGE_H

#include "config.h"
#include "view_data.h"

#ifdef __cplusplus
extern "C" {
#endif


int indicator_storage_init(void);

esp_err_t indicator_storage_write(char *p_key, void *p_data, size_t len);


//p_len : inout
esp_err_t indicator_storage_read(char *p_key, void *p_data, size_t *p_len);

#ifdef __cplusplus
}
#endif

#endif
