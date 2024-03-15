#include "app_image.h"
#include "mbedtls/base64.h"
#include <cJSON.h>
static const char* TAG = "app_image";

#define IMG_WIDTH CANVAS_WIDTH
#define IMG_HEIGHT CANVAS_HEIGHT

#define SEE_DECODEDE_LOG 0
#define USING_CANVAS 1

static lv_img_dsc_t img_dsc = {
    .header.always_zero = 0,
    .header.w = IMG_WIDTH,
    .header.h = IMG_HEIGHT,
    .data_size = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = NULL,
};
static lv_draw_img_dsc_t draw_dsc; // 必须 static 或全局变量

#if IMG_WIDTH != IMG_HEIGHT
#error "IMG_WIDTH must be equal to IMG_HEIGHT"
#elif IMG_WIDTH == 240
#define DECODED_IMAGE_MAX_SIZE (15 * 1024)
#elif IMG_WIDTH == 480
#define DECODED_IMAGE_MAX_SIZE (25 * 1024)
#endif

size_t decode_base64_image(const unsigned char* p_data, unsigned char* decoded_str) {
    if (!p_data || !decoded_str)
        return 0;

    size_t str_len = strlen((const char*)p_data);
    size_t output_len = 0;
    // 获取解码后的长度
    int decode_ret = mbedtls_base64_decode(NULL, 0, &output_len, p_data, str_len);
    if (decode_ret == MBEDTLS_ERR_BASE64_INVALID_CHARACTER) {
        ESP_LOGE(TAG, "Invalid character in Base64 string");
        return 0;
    }

    // 检查解码后的长度是否合理
    if (output_len == 0 || output_len > DECODED_IMAGE_MAX_SIZE) {
        ESP_LOGE(TAG, "Base64 decode output size is too large or zero.");
        return 0;
    }

    // 实际解码操作
    decode_ret = mbedtls_base64_decode(decoded_str, DECODED_IMAGE_MAX_SIZE, &output_len, p_data, str_len);
    if (decode_ret) { // !=0
        ESP_LOGE(TAG, "Failed to decode Base64 string, error: %d", decode_ret);
        return 0;
    }
#if SEE_DECODEDE_LOG
    ESP_LOGI(TAG, "str_len: %d, output_len: %d", str_len, output_len);
#endif
    return output_len;
}

void update_canvas_with_image(lv_obj_t* canvas, const unsigned char* image_data, size_t image_size) {
    if (!canvas || !image_data || image_size == 0)
        return;

    img_dsc.data = image_data;
    img_dsc.data_size = image_size;

    lv_canvas_draw_img(canvas, 0, 0, &img_dsc, &draw_dsc);
}

void update_jpeg_with_image(lv_obj_t* obj, const unsigned char* image_data, size_t image_size) {
    if (!obj || !image_data || image_size == 0)
        return;

    img_dsc.data = image_data;
    img_dsc.data_size = image_size; // 确保设置了正确的data_size
    lv_img_set_src(obj, &img_dsc);
}

// 从JSON解析并显示图像的函数
void display_image_from_json(cJSON* json, lv_obj_t* canvas) {
    static unsigned char img_buf[DECODED_IMAGE_MAX_SIZE] = {0};
    static unsigned char decoded_image[DECODED_IMAGE_MAX_SIZE + 1]; // 静态分配解码后的图片数据缓冲区

    if (!json || !canvas)
        return;

    cJSON* img = cJSON_GetObjectItem(json, "img");
    if (!cJSON_IsString(img) || img->valuestring == NULL) {
        ESP_LOGE(TAG, "Invalid img string");
        return;
    }

    lv_port_sem_take();
    lv_memset_00(img_buf, sizeof(img_buf));
    strncpy(img_buf, img->valuestring, sizeof(img_buf) - 1);

    // 解码图片数据
    size_t decoded_image_size = decode_base64_image(img_buf, decoded_image);
    if (decoded_image_size == 0) {
        ESP_LOGE(TAG, "Failed to decode image");
        return;
    }
    update_canvas_with_image(canvas, decoded_image, decoded_image_size);
    lv_port_sem_give();
}

void init_image(void) {
    lv_draw_img_dsc_init(&draw_dsc);
}
////////////////////////

// cJSON* img = cJSON_GetObjectItem(receivedJson, "img");
// if (cJSON_IsString(img) && (img->valuestring != NULL)) {
//     memset(img_buf, 0, DECODED_STR_MAX_SIZE);
//     strncpy(img_buf, img->valuestring, DECODED_STR_MAX_SIZE - 1);
//     lv_port_sem_take();
//     lv_obj_clean(ui_v2_image); // must be ahead
//     display_one_image(ui_v2_image, &img_buf);
//     lv_port_sem_give();
//     vTaskDelay(1);
// }

/**
 * @brief transform base64 encoded image data to lvgl image object
 *
 * @param image lv_obj_t image object
 * @param p_data base64 encoded image data
 */
// void display_one_image(lv_obj_t* image, const unsigned char* p_data) {
//     if (!p_data)
//         return;

//     size_t str_len = strlen((const char*)p_data);
//     size_t output_len = 0;
//     // 首先获取解码后的长度
//     int decode_ret = mbedtls_base64_decode(NULL, 0, &output_len, p_data, str_len);
//     if (decode_ret == MBEDTLS_ERR_BASE64_INVALID_CHARACTER) {
//         ESP_LOGE(TAG, "Invalid character in Base64 string");
//         return;
//     }

//     // 检查解码后的长度是否超出预期
//     if (output_len == 0 || output_len > DECODED_STR_MAX_SIZE) {
//         ESP_LOGE(TAG, "Base64 decode output size is too large or zero.");
//         return;
//     }

//     // 执行实际的解码操作
//     decode_ret = mbedtls_base64_decode(decoded_str, DECODED_STR_MAX_SIZE, &output_len, p_data, str_len);
//     if (decode_ret == 0) {
//         img_dsc.data = decoded_str;
//         img_dsc.data_size = output_len; // 确保设置了正确的data_size
//         lv_img_set_src(image, &img_dsc);
//         // ESP_LOGI(TAG, "displayed successfully.");
//     } else if (decode_ret == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) {
//         ESP_LOGE(TAG, "Buffer too small for decoding %d bytes %d output", str_len, output_len);
//         return;
//     } else {
//         ESP_LOGE(TAG, "Failed to decode Base64 string, error: %d", decode_ret);
//     }
// }

/////////////////////
