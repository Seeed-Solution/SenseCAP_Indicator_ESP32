#include "indicator_storage_nvs.h"
#include "nvs_flash.h"

#define STORAGE_NAMESPACE "indicator"

int indicator_nvs_init(void)
{
    // ESP_ERROR_CHECK(nvs_flash_erase());
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
}

esp_err_t indicator_nvs_write(char *p_key, void *p_data, size_t len)
{
    nvs_handle_t my_handle;
    esp_err_t    err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_set_blob(my_handle, p_key, p_data, len);
    if (err == ESP_OK) {
        err = nvs_commit(my_handle);
    }

    nvs_close(my_handle);
    return err;
}

esp_err_t indicator_nvs_read(char *p_key, void *p_data, size_t *p_len)
{
    nvs_handle_t my_handle;
    esp_err_t    err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_get_blob(my_handle, p_key, p_data, p_len);

    nvs_close(my_handle);
    return err;
}
