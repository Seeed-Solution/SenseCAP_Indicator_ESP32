#include "indicator_storage_spiffs.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_err.h"
#include "esp_partition.h"

static const char *TAG = "indicator_SPIFFS";

esp_err_t indicator_spiffs_init(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = SPIFFS_BASE_PATH,
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to mount or format filesystem");
    }
    else
    {
        ESP_LOGI(TAG, "Mounted filesystem");
    }
    return ret;
}

bool check_file_exists(const char *path)
{
    FILE *file = fopen(path, "r");
    if (file)
    {
        fclose(file);
        return true;
    }
    return false;
}

esp_err_t indicator_spiffs_write_str(const char *path, const char *data)
{
    FILE *f = fopen(path, "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }

    fprintf(f, "%s", data);
    fclose(f);
    ESP_LOGI(TAG, "File written");
    return ESP_OK;
}

esp_err_t indicator_spiffs_read_str(const char *path, char *data, size_t len)
{
    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }

    fgets(data, len, f);
    fclose(f);
    ESP_LOGI(TAG, "File read");
    return ESP_OK;
}

esp_err_t indicator_spiffs_write(const char *path, const void *data, size_t len)
{
    FILE *file = fopen(path, "wb");
    if (file == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }

    size_t written = fwrite(data, 1, len, file);
    if (written != len)
    {
        ESP_LOGE(TAG, "Error writing to file");
        fclose(file);
        return ESP_FAIL;
    }

    fclose(file);
    return ESP_OK;
}

esp_err_t indicator_spiffs_read(const char *path, void *data, size_t len)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }

    size_t read = fread(data, 1, len, file);
    if (read != len)
    {
        ESP_LOGE(TAG, "Error reading from file");
        fclose(file);
        return ESP_FAIL;
    }

    fclose(file);
    return ESP_OK;
}


// deinitialize_spiffs
esp_err_t indicator_spiffs_deinit(void)
{
    return esp_vfs_spiffs_unregister(NULL);
}


esp_err_t indicator_spiffs_format(void) {
    esp_err_t ret;

    // 卸载 SPIFFS 分区
    ret = indicator_spiffs_deinit();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to unregister SPIFFS (%s)", esp_err_to_name(ret));
        return ret;
    }

    // 格式化 SPIFFS 分区
    const esp_partition_t* partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, NULL);
    if (!partition) {
        ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        return ESP_ERR_NOT_FOUND;
    }

    ret = esp_partition_erase_range(partition, 0, partition->size);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to format SPIFFS partition (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "SPIFFS partition formatted");
    }

    // 重新挂载 SPIFFS 分区
    esp_vfs_spiffs_conf_t conf = {
        .base_path = SPIFFS_BASE_PATH,
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
    };
    ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount SPIFFS after formatting (%s)", esp_err_to_name(ret));
    }

    return ret;
}