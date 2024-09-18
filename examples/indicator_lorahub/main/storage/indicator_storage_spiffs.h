/**
 * @file indicator_storage_spiffs.h
 * @date  10 December 2023

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2023, Seeed Studio
 */

#ifndef INDICATOR_STORAGE_SPIFFS_H
#define INDICATOR_STORAGE_SPIFFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <esp_err.h>
#include <stdbool.h>

#define SPIFFS_BASE_PATH "/data"

esp_err_t indicator_spiffs_init(void);
esp_err_t indicator_spiffs_write_str(const char *path, const char *data);
esp_err_t indicator_spiffs_read_str(const char *path, char *data, size_t len);
esp_err_t indicator_spiffs_write(const char *path, const void *data, size_t len);
esp_err_t indicator_spiffs_read(const char *path, void *data, size_t len);
bool check_file_exists(const char *filename);
esp_err_t indicator_spiffs_deinit(void);
esp_err_t indicator_spiffs_format(void);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*INDICATOR_STORAGE_SPIFFS_H*/