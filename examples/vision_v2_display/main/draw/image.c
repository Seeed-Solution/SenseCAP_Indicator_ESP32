#include "image.h"
#include "mbedtls/base64.h"

static const char *TAG = "view_image";

#define IMG_WIDTH 240
#define IMG_HEIGHT 240

static lv_img_dsc_t img_dsc = {
    .header.always_zero = 0,
    .header.w = IMG_WIDTH,
    .header.h = IMG_HEIGHT,
    .data_size = 0,
    .header.cf = LV_IMG_CF_RAW_ALPHA,
    .data = NULL,
};

static unsigned char decoded_str[DECODED_STR_MAX_SIZE + 1]; // 静态分配

void display_one_image(lv_obj_t *image, const unsigned char *p_data)
{
    if (!p_data)
        return;

    size_t str_len = strlen((const char *)p_data);
    size_t output_len = 0;
    // 首先获取解码后的长度
    int decode_ret = mbedtls_base64_decode(NULL, 0, &output_len, p_data, str_len);
    if (decode_ret == MBEDTLS_ERR_BASE64_INVALID_CHARACTER)
    {
        ESP_LOGE(TAG, "Invalid character in Base64 string");
        return;
    }

    // 检查解码后的长度是否超出预期
    if (output_len == 0 || output_len > DECODED_STR_MAX_SIZE)
    {
        ESP_LOGE(TAG, "Base64 decode output size is too large or zero.");
        return;
    }

    // 执行实际的解码操作
    decode_ret = mbedtls_base64_decode(decoded_str, DECODED_STR_MAX_SIZE, &output_len, p_data, str_len);
    if (decode_ret == 0)
    {
        img_dsc.data = decoded_str;
        img_dsc.data_size = output_len; // 确保设置了正确的data_size 
        lv_img_set_src(image, &img_dsc); 
        // ESP_LOGI(TAG, "displayed successfully.");
    }
    else if (decode_ret == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL)
    {
        ESP_LOGE(TAG, "Buffer too small for decoding %d bytes %d output", str_len, output_len);
        return;
    }
    else
    {
        ESP_LOGE(TAG, "Failed to decode Base64 string, error: %d", decode_ret);
    }
}
