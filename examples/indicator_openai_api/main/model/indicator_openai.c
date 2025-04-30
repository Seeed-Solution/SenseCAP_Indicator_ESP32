#include "indicator_openai.h"
#include "cJSON.h"
#include "esp_http_client.h"
#include "esp_tls.h"
#include "freertos/semphr.h"

#include "lwip/dns.h"
#include "lwip/err.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

#include "esp_crt_bundle.h"
#include "nvs.h"

struct indicator_openai
{
};

static const char *TAG = "openai";

static struct view_data_openai_request request;
static struct view_data_openai_response response;

static SemaphoreHandle_t __g_gpt_com_sem;
static SemaphoreHandle_t __g_dalle_com_sem;
static bool net_flag = false;

static char *p_recv_buf;
static size_t recv_buf_max_len;

static char openai_api_key[MAX_API_KEY_LENGTH];
static char openai_api_url[MAX_API_URL_LENGTH];
static char openai_model_name[MAX_MODEL_NAME_LENGTH];
static char openai_system_prompt[MAX_SYSTEM_PROMPT_LENGTH];

static bool have_key = false;
static bool have_url = false;
static bool have_model = false;

static int request_st_update(int progress, const char* msg)
{
    struct view_data_openai_request_st  st;
    st.progress = progress;
    strcpy(st.state, msg);
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_OPENAI_REQUEST_ST, &st, sizeof(st), portMAX_DELAY);
}

static int __chat_json_prase(const char *p_str, char *p_answer, char *p_err)
{
    // 查找 "content:" 关键词
    const char *content_key = "\"content\":";
    char *content_pos = strstr(p_str, content_key);
    
    if (content_pos == NULL) {
        // 查找错误信息
        const char *error_key = "\"error\":";
        char *error_pos = strstr(p_str, error_key);
        
        if (error_pos != NULL) {
            // 尝试提取错误消息
            const char *message_key = "\"message\":";
            char *message_pos = strstr(error_pos, message_key);
            
            if (message_pos != NULL) {
                message_pos += strlen(message_key);
                // 跳过空白字符
                while (*message_pos == ' ' || *message_pos == '\t' || *message_pos == '\r' || *message_pos == '\n') {
                    message_pos++;
                }
                
                if (*message_pos == '"') {
                    message_pos++; // 跳过开始的双引号
                    char *end_quote = strchr(message_pos, '"');
                    if (end_quote != NULL) {
                        int len = end_quote - message_pos;
                        if (len > 63) len = 63; // 防止缓冲区溢出
                        strncpy(p_err, message_pos, len);
                        p_err[len] = '\0';
                        return -1;
                    }
                }
            }
        }
        
        ESP_LOGE(TAG, "未找到 content 关键词");
        strcpy(p_err, "解析数据失败，未找到 content 关键词");
        return -1;
    }
    
    // 移动到 "content": 后面
    content_pos += strlen(content_key);
    
    // 跳过空白字符
    while (*content_pos == ' ' || *content_pos == '\t' || *content_pos == '\r' || *content_pos == '\n') {
        content_pos++;
    }
    
    // 检查是否有双引号开始
    if (*content_pos != '"') {
        ESP_LOGE(TAG, "content 后未找到双引号");
        strcpy(p_err, "解析数据失败，content 后未找到双引号");
        return -1;
    }
    
    // 跳过开始的双引号
    content_pos++;
    
    // 查找结束的双引号
    char *end_quote = content_pos;
    int escaped = 0;
    
    while (*end_quote != '\0') {
        if (*end_quote == '\\') {
            escaped = !escaped;
        } else if (*end_quote == '"' && !escaped) {
            break;
        } else {
            escaped = 0;
        }
        end_quote++;
    }
    
    if (*end_quote != '"') {
        ESP_LOGE(TAG, "未找到 content 值的结束双引号");
        strcpy(p_err, "解析数据失败，未找到 content 值的结束双引号");
        return -1;
    }
    
    // 计算内容长度并复制
    int content_len = end_quote - content_pos;
    strncpy(p_answer, content_pos, content_len);
    p_answer[content_len] = '\0';
    
    // 处理转义字符
    char *src = p_answer;
    char *dst = p_answer;
    
    while (*src) {
        if (*src == '\\' && *(src + 1) != '\0') {
            src++;
            if (*src == 'n') *dst = '\n';
            else if (*src == 'r') *dst = '\r';
            else if (*src == 't') *dst = '\t';
            else *dst = *src;
        } else {
            *dst = *src;
        }
        src++;
        dst++;
    }
    *dst = '\0';
    
    return 0;
}

// 添加事件处理结构体
struct http_response_data {
    char *response_buf;
    int response_buf_len;
    int current_len;
};

// HTTP事件处理函数
static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    struct http_response_data *response_data = (struct http_response_data *)evt->user_data;
    
    switch(evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            if (response_data->current_len + evt->data_len < response_data->response_buf_len) {
                memcpy(response_data->response_buf + response_data->current_len, evt->data, evt->data_len);
                response_data->current_len += evt->data_len;
                response_data->response_buf[response_data->current_len] = '\0';
            }
            break;
        default:
            break;
    }
    return ESP_OK;
}

static int http_request(const char *method, const char *request_suffix, const char *content_type,
                        const char *post_data, int post_len,
                        char *response_buf, int response_buf_len)
{
    struct http_response_data response_data = {
        .response_buf = response_buf,
        .response_buf_len = response_buf_len,
        .current_len = 0
    };
    // 将content_type直接拼接到openai_api_url后
    char url[MAX_API_URL_LENGTH+20];
    snprintf(url, sizeof(url), "%s%s", openai_api_url, request_suffix);
    // 配置HTTP客户端
    esp_http_client_config_t config = {
        .url = url,
        .port = 443,
        .method = (strcmp(method, "POST") == 0) ? HTTP_METHOD_POST : HTTP_METHOD_GET,
        .timeout_ms = DEFAULT_TIMEOUT_MS,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .event_handler = http_event_handler,
        .user_data = &response_data,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .is_async = false,  // 使用同步模式
        .cert_pem = NULL,  // 不使用自定义证书
        .buffer_size = 4096,  // 设置合理的缓冲区大小
        .buffer_size_tx = 4096,  // 设置合理的发送缓冲区大小
        .disable_auto_redirect = true  // 禁用自动重定向
    };

    ESP_LOGI(TAG, "Sending request to: %s", url);
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "HTTP client init failed");
        return -1;
    }

    // 设置请求头
    esp_http_client_set_header(client, "Content-Type", content_type);
    esp_http_client_set_header(client, "Authorization", openai_api_key);
    esp_http_client_set_header(client, "Connection", "keep-alive");
    
    // 设置POST数据
    if (post_data != NULL && post_len > 0) {
        esp_http_client_set_post_field(client, post_data, post_len);
    }

    // 执行请求
    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP request failed: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return -1;
    }

    // 获取响应状态码
    int status_code = esp_http_client_get_status_code(client);
    ESP_LOGI(TAG, "HTTP Status = %d", status_code);
    
    if (status_code != 200) {
        ESP_LOGE(TAG, "HTTP request returned non-200 status code: %d", status_code);
        esp_http_client_cleanup(client);
        return -1;
    }

    ESP_LOGI(TAG, "Response received, length: %d", response_data.current_len);
    
    // 清理HTTP客户端
    esp_http_client_cleanup(client);
    
    return response_data.current_len;
}

static int chat_request(struct view_data_openai_request *p_req,
                        struct view_data_openai_response *p_resp)
{
    char data_buf[MAX_DATA_SIZE];
    int data_len = 0;
    int ret = 0;

    memset(data_buf, 0, sizeof(data_buf));

    // 构建请求JSON数据，使用存储的模型名称
    data_len = sprintf(data_buf,
                       "{\"model\":\"%s\", \"messages\":[", openai_model_name);
    
    // 添加系统消息（如果有）
    if (strlen(openai_system_prompt) > 0) {
        data_len += sprintf(data_buf + data_len, 
                          "{\"role\":\"system\",\"content\":\"%s\"},", openai_system_prompt);
    }
    
    // 添加用户消息
    data_len += sprintf(data_buf + data_len, "{\"role\":\"user\",\"content\":\"");
    data_len += sprintf(data_buf + data_len, "%s", p_req->question);
    data_len += sprintf(data_buf + data_len, "\"}]}");

    // 清空接收缓冲区
    memset(p_recv_buf, 0, recv_buf_max_len);

    // 使用专用的OpenAI API请求函数
    ret = http_request(
        "POST",
        "/chat/completions",
        "application/json",
        data_buf,
        data_len,
        p_recv_buf,
        recv_buf_max_len
    );

    if (ret < 0) {
        ESP_LOGE(TAG, "OpenAI API request failed");
        p_resp->ret = 0;
        strcpy(p_resp->err_msg, "Connect to API server failed");
        return -1;
    }

    // 设置响应数据
    p_resp->p_answer = p_recv_buf + recv_buf_max_len / 2; // 使用p_recv_buf内存

    // 解析JSON响应
    ret = __chat_json_prase(p_recv_buf, p_resp->p_answer, p_resp->err_msg);
    if (ret != 0) {
        p_resp->ret = 0;
        return -1;
    }
    
    p_resp->ret = 1;
    return 0;
}

static void __openai_api_key_read(void)
{
    esp_err_t ret = 0;
    int len = sizeof(openai_api_key);
    ret = indicator_storage_read(API_KEY_STORAGE, (void *)openai_api_key, &len);
    if (ret == ESP_OK && len > 0)  // 修改条件，只要长度大于0即可
    {
        // 检查是否已经包含Bearer前缀
        if (strncmp(openai_api_key, "Bearer ", 7) != 0) {
            // 为API key添加Bearer前缀
            char temp_key[MAX_API_KEY_LENGTH];
            snprintf(temp_key, sizeof(temp_key), "Bearer %s", openai_api_key);
            strncpy(openai_api_key, temp_key, sizeof(openai_api_key) - 1);
            openai_api_key[sizeof(openai_api_key) - 1] = '\0';
        }
        // update key state
        have_key = true;
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_OPENAI_ST, &have_key, sizeof(have_key), portMAX_DELAY);
        ESP_LOGI(TAG, "openai_api_key read successful: length=%d", len);
    }
    else
    {
        // err or not find
        have_key = false;
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_OPENAI_ST, &have_key, sizeof(have_key), portMAX_DELAY);
        if (ret == ESP_ERR_NVS_NOT_FOUND)
        {
            ESP_LOGI(TAG, "openai_api_key not find");
        }
        else
        {
            ESP_LOGI(TAG, "openai_api_key read err:%d, len=%d", ret, len);
        }
    }
}

// 修改API URL读取函数
static void __openai_api_url_read(void)
{
    esp_err_t ret = 0;
    int len = sizeof(openai_api_url);
    ret = indicator_storage_read(API_URL_STORAGE, (void *)openai_api_url, &len);
    if (ret == ESP_OK && len > 0)
    {
        // 检查URL是否包含https://前缀
        if (strncmp(openai_api_url, "https://", 8) != 0) {
            // 为URL添加https://前缀
            char temp_url[MAX_API_URL_LENGTH];
            snprintf(temp_url, sizeof(temp_url), "https://%s", openai_api_url);
            strncpy(openai_api_url, temp_url, sizeof(openai_api_url) - 1);
            openai_api_url[sizeof(openai_api_url) - 1] = '\0';
        }
        have_url = true;
        ESP_LOGI(TAG, "openai_api_url read successful: %s", openai_api_url);
    }
    else
    {
        // 设置默认URL
        strcpy(openai_api_url, "https://api.openai.com/v1");
        have_url = false;
        ESP_LOGI(TAG, "Using default API URL: %s", openai_api_url);
        if (ret == ESP_ERR_NVS_NOT_FOUND)
        {
            ESP_LOGI(TAG, "openai_api_url not found, using default");
        }
        else
        {
            ESP_LOGI(TAG, "openai_api_url read err:%d, using default", ret);
        }
    }
}

// 修改模型名称读取函数
static void __openai_model_name_read(void)
{
    esp_err_t ret = 0;
    int len = sizeof(openai_model_name);
    ret = indicator_storage_read(MODEL_NAME_STORAGE, (void *)openai_model_name, &len);
    if (ret == ESP_OK && len > 0)
    {
        have_model = true;
        ESP_LOGI(TAG, "openai_model_name read successful: %s", openai_model_name);
    }
    else
    {
        // 设置默认模型名称
        strcpy(openai_model_name, "gpt-3.5-turbo");
        have_model = false;
        ESP_LOGI(TAG, "Using default model name: %s", openai_model_name);
        if (ret == ESP_ERR_NVS_NOT_FOUND)
        {
            ESP_LOGI(TAG, "openai_model_name not found, using default");
        }
        else
        {
            ESP_LOGI(TAG, "openai_model_name read err:%d, using default", ret);
        }
    }
}

static void __openai_system_prompt_read(void)
{
    esp_err_t ret = 0;
    int len = sizeof(openai_system_prompt);
    ret = indicator_storage_read(SYSTEM_PROMPT_STORAGE, (void *)openai_system_prompt, &len);
    if (ret == ESP_OK && len > 0)
    {
        ESP_LOGI(TAG, "openai_system_prompt read successful: %s", openai_system_prompt);
    }
    else
    {
        // 设置默认系统提示词
        strcpy(openai_system_prompt, "Your are SenseCAP Indicator, developed by Seeed Studio, has been launched on April 20th, 2023. You are a 4-inch touch screen driven by ESP32 and RP2040 dual-MCU, and support Wi-Fi/BLE/LoRa communication. You are a fully open-source powerful IoT development platform for developers. You are on behalf of Seeed Studio to answer requests. Each time your answer text should not exceed 100 words.");
        ESP_LOGI(TAG, "Using default system prompt");
        if (ret == ESP_ERR_NVS_NOT_FOUND)
        {
            ESP_LOGI(TAG, "openai_system_prompt not found, using default");
        }
        else
        {
            ESP_LOGI(TAG, "openai_system_prompt read err:%d, using default", ret);
        }
    }
}

static int __openai_init()
{
    recv_buf_max_len = 1024 * 1024;
    p_recv_buf = malloc(recv_buf_max_len); // from psram
    if (p_recv_buf == NULL)
    {
        ESP_LOGE(TAG, "malloc %s bytes fail!", recv_buf_max_len);
    }
    
    // 初始化时读取所有配置
    __openai_api_key_read();
    __openai_api_url_read();
    __openai_model_name_read();
    __openai_system_prompt_read();
}

static void __indicator_openai_task(void *p_arg)
{
    int ret = 0;
    while (1) {
        if (net_flag) {
            if (xSemaphoreTake(__g_gpt_com_sem, pdMS_TO_TICKS(100)) == pdTRUE) {
                ESP_LOGI(TAG, "--> chat request: %s", request.question);
                memset(&response, 0, sizeof(response));
                request_st_update(99, "Request..."); 
                ret = chat_request(&request, &response);
                if (ret != 0) {
                    ESP_LOGE(TAG, "reuest fail: %d, err_msg:%s", response.ret, response.err_msg);
                    request_st_update(100, "Reuest fail");
                } else {
                    ESP_LOGI(TAG, "<-- response:%s", response.p_answer);
                    request_st_update(100, "Done");
                }
                // vTaskDelay(pdMS_TO_TICKS(1000));
                esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_CHATGPT_RESPONSE, &response, sizeof(response), portMAX_DELAY);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void __view_event_handler(void *handler_args, esp_event_base_t base,
                                 int32_t id, void *event_data)
{
    switch (id)
    {
        case VIEW_EVENT_WIFI_ST:
        {
            ESP_LOGI(TAG, "event: VIEW_EVENT_WIFI_ST");
            struct view_data_wifi_st *p_st = (struct view_data_wifi_st *)event_data;
            if (p_st->is_network)
            {
                net_flag = true;
            }
            else
            {
                net_flag = false;
            }
            break;
        }
        case VIEW_EVENT_CHATGPT_REQUEST:
        {
            ESP_LOGI(TAG, "event: VIEW_EVENT_CHATGPT_REQUEST");
            struct view_data_openai_request *p_req = (struct view_data_openai_request *)event_data;
            memcpy(&request,p_req, sizeof(request));
            request_st_update(0, "ready");
            xSemaphoreGive(__g_gpt_com_sem);
            break;
        }
        case VIEW_EVENT_OPENAI_API_KEY_READ:
        {
            ESP_LOGI(TAG, "event: VIEW_EVENT_OPENAI_API_KEY_READ");
            __openai_api_key_read();
            break;  
        }
        // 添加新的事件处理
        case VIEW_EVENT_OPENAI_API_URL_READ:
        {
            ESP_LOGI(TAG, "event: VIEW_EVENT_OPENAI_API_URL_READ");
            __openai_api_url_read();
            break;  
        }
        case VIEW_EVENT_OPENAI_MODEL_NAME_READ:
        {
            ESP_LOGI(TAG, "event: VIEW_EVENT_OPENAI_MODEL_NAME_READ");
            __openai_model_name_read();
            break;  
        }
        case VIEW_EVENT_OPENAI_SYSTEM_PROMPT_READ:
        {
            ESP_LOGI(TAG, "event: VIEW_EVENT_OPENAI_SYSTEM_PROMPT_READ");
            __openai_system_prompt_read();
            break;  
        }
        default:
            break;
    }
}

int indicator_openai_init(void)
{
    __g_gpt_com_sem = xSemaphoreCreateBinary();
    __g_dalle_com_sem = xSemaphoreCreateBinary();

    __openai_init();

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with( view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_WIFI_ST, 
                                                            __view_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with( view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_CHATGPT_REQUEST, 
                                                            __view_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with( view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_OPENAI_API_KEY_READ, 
                                                            __view_event_handler, NULL, NULL));
    // 注册新的事件处理
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with( view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_OPENAI_API_URL_READ, 
                                                            __view_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with( view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_OPENAI_MODEL_NAME_READ, 
                                                            __view_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with( view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_OPENAI_SYSTEM_PROMPT_READ, 
                                                            __view_event_handler, NULL, NULL));
    xTaskCreate(&__indicator_openai_task, "__indicator_openai_task", 1024 * 10, NULL, 10, NULL);
}
