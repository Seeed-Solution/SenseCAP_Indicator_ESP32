#include "indicator_city.h"
#include "freertos/semphr.h"
#include "esp_tls.h"
#include "esp_http_client.h"
#include "cJSON.h"
#include "indicator_time.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "esp_crt_bundle.h"

#define MAX_HTTP_OUTPUT_BUFFER 4096

#define DISPLAY_CFG_STORAGE  "city"


struct indicator_city
{
    // info
    char ip[32];
    char city[32];
    char timezone[64];
    int  local_utc_offset;
    // char country[32];

};

static const char *TAG = "city";

static struct indicator_city __g_city_model;

static SemaphoreHandle_t   __g_http_com_sem;

static bool net_flag = false;

static char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};

static int __city_data_prase(const char *p_str)
{
    //prase
    int ret = 0;

    cJSON *root = NULL;
    cJSON* cjson_item = NULL;

    root = cJSON_Parse(p_str);
    if( root == NULL ) {
        return -1;
    }
   
    cjson_item = cJSON_GetObjectItem(root, "status");
    if( cjson_item != NULL  && cjson_item->valuestring != NULL) {
        if( strcmp(cjson_item->valuestring, "success") != 0 ) {
            ret = -2;
            goto prase_end;
        }
    }
    cjson_item = cJSON_GetObjectItem(root, "city");
    if( cjson_item != NULL  && cjson_item->valuestring != NULL) {
        strncpy(__g_city_model.city, cjson_item->valuestring, sizeof(__g_city_model.city)-1);
    }

    cjson_item = cJSON_GetObjectItem(root, "query");
    if( cjson_item != NULL  && cjson_item->valuestring != NULL) {
        strncpy(__g_city_model.ip, cjson_item->valuestring, sizeof(__g_city_model.ip)-1);
    }

    cjson_item = cJSON_GetObjectItem(root, "timezone");
    if( cjson_item != NULL  && cjson_item->valuestring != NULL) {
        strncpy(__g_city_model.timezone,  cjson_item->valuestring, sizeof(__g_city_model.timezone)-1);
    }
prase_end:

    cJSON_Delete(root);
    
    return ret;
}

#if 0
esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                } else {
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
                // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
    }
    return ESP_OK;
}



static int __city_get(void)
{
    memset(local_response_buffer, 0, sizeof(local_response_buffer));

    esp_http_client_config_t config = {
        .host = "ip-api.com",
        .path = "/json",
        // .query = "format=json",
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,        // Pass address of local buffer to get response
        .disable_auto_redirect = true,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %lld",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
        return -1;
    }
    ESP_LOGI(TAG, "CITY RESPONSE: %s", local_response_buffer);
    esp_http_client_cleanup(client);
    
    return __city_data_prase(local_response_buffer);
}
#else
#define WEB_SERVER "ip-api.com"
#define WEB_PORT "80"
#define WEB_PATH "/json"

static const char *REQUEST = "GET " WEB_PATH " HTTP/1.0\r\n"
    "Host: "WEB_SERVER":"WEB_PORT"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n"
    "\r\n";

static int __city_get(void)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    int retry=0;
    while(net_flag) {
        if( retry > 5) {
            return -1;
        }
        retry++;
        int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

        if(err != 0 || res == NULL) {
            ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        /* Code to print the resolved IP.

           Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
        addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
        ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

        s = socket(res->ai_family, res->ai_socktype, 0);
        if(s < 0) {
            ESP_LOGE(TAG, "... Failed to allocate socket.");
            freeaddrinfo(res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... allocated socket");

        if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
            ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
            close(s);
            freeaddrinfo(res);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }

        ESP_LOGI(TAG, "... connected");
        freeaddrinfo(res);

        if (write(s, REQUEST, strlen(REQUEST)) < 0) {
            ESP_LOGE(TAG, "... socket send failed");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... socket send success");

        struct timeval receiving_timeout;
        receiving_timeout.tv_sec = 5;
        receiving_timeout.tv_usec = 0;
        if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                sizeof(receiving_timeout)) < 0) {
            ESP_LOGE(TAG, "... failed to set socket receiving timeout");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... set socket receiving timeout success");

        /* Read HTTP response */
        int recv_len = 0;
        bzero(local_response_buffer, sizeof(local_response_buffer));

        do {
            r = read(s, local_response_buffer + recv_len, sizeof(local_response_buffer)-1-recv_len);
            recv_len += r;
            for(int i = 0; i < r; i++) {
                putchar(local_response_buffer[i+recv_len]);
            }
        } while(r > 0);

        ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d.", r, errno);
        close(s);

        if( recv_len > 0) {
            char *p_json = strstr(local_response_buffer, "\r\n\r\n");
            if( p_json ) {
                p_json =  p_json + 4;
                return __city_data_prase(p_json);
            } else {
                return -1;
            }
        }

    }
}

#endif

/* ---------------------------------------------------------- */
//  time zone 
/* ---------------------------------------------------------- */

static int __time_zone_data_prase(const char *p_str)
{
    //prase
    int ret = 0;

    cJSON *root = NULL;
    cJSON* cjson_item = NULL;
    cJSON* cjson_item_child = NULL;

    root = cJSON_Parse(p_str);
    if( root == NULL ) {
        ESP_LOGI(TAG, "cJSON_Parse err");
        return -1;
    }
   
    cjson_item = cJSON_GetObjectItem(root, "currentUtcOffset");
    if( cjson_item != NULL ) {
        cjson_item_child = cJSON_GetObjectItem(cjson_item, "seconds");
        if( cjson_item_child != NULL) {
            __g_city_model.local_utc_offset = cjson_item_child->valueint;
            ESP_LOGI(TAG, "local_utc_offset:%ds", cjson_item_child->valueint );
        }
    }

    //todo 
prase_end:
    cJSON_Delete(root);
    
    return ret;
}

#if 1
extern const char timeapi_root_cert_pem_start[] asm("_binary_timeapi_cert_pem_start");
extern const char timeapi_root_cert_pem_end[]   asm("_binary_timeapi_cert_pem_end");

static int https_get_request(esp_tls_cfg_t cfg, const char *WEB_SERVER_URL, const char *REQUEST)
{
    int ret, len;
    int recv_len = 0;
    esp_tls_t *tls = esp_tls_init();
    if (!tls) {
        ESP_LOGE(TAG, "Failed to allocate esp_tls handle!");
        goto exit;
    }

    if (esp_tls_conn_http_new_sync(WEB_SERVER_URL, &cfg, tls) == 1) {
        ESP_LOGI(TAG, "Connection established...");
    } else {
        ESP_LOGE(TAG, "Connection failed...");
        goto cleanup;
    }

    size_t written_bytes = 0;
    do {
        ret = esp_tls_conn_write(tls,
                                 REQUEST + written_bytes,
                                 strlen(REQUEST) - written_bytes);
        if (ret >= 0) {
            ESP_LOGI(TAG, "%d bytes written", ret);
            written_bytes += ret;
        } else if (ret != ESP_TLS_ERR_SSL_WANT_READ  && ret != ESP_TLS_ERR_SSL_WANT_WRITE) {
            ESP_LOGE(TAG, "esp_tls_conn_write  returned: [0x%02X](%s)", ret, esp_err_to_name(ret));
            goto cleanup;
        }
    } while (written_bytes < strlen(REQUEST));

    ESP_LOGI(TAG, "Reading HTTP response...");
    memset(local_response_buffer, 0x00, sizeof(local_response_buffer));
    recv_len = 0;
    do {
        len = sizeof(local_response_buffer) - recv_len - 1;
       
        ret = esp_tls_conn_read(tls, (char *)local_response_buffer + recv_len, len);

        if (ret == ESP_TLS_ERR_SSL_WANT_WRITE  || ret == ESP_TLS_ERR_SSL_WANT_READ) {
            continue;
        } else if (ret < 0) {
            recv_len = 0;
            ESP_LOGE(TAG, "esp_tls_conn_read  returned [-0x%02X](%s)", -ret, esp_err_to_name(ret));
            break;
        } else if (ret == 0) {
            ESP_LOGI(TAG, "connection closed");
            break;
        }
        recv_len += ret;
        
        break; //todo Let's say I can receive it all at once
    } while (1);

    if( recv_len > 0 ) {
        printf( "%s", local_response_buffer);
    }
    
cleanup:
    esp_tls_conn_destroy(tls);
exit:
    return recv_len;
}

static int __ip_get(char *ip, int buf_len)
{
    esp_tls_cfg_t cfg = {
        .cacert_buf = (const unsigned char *) timeapi_root_cert_pem_start,
        .cacert_bytes = timeapi_root_cert_pem_end - timeapi_root_cert_pem_start,
    };
    char ip_url[128] = {0};
    char ip_request[200] = {0};
    int len;
    snprintf(ip_url, sizeof(ip_url),"https://api.ipify.org");
    snprintf(ip_request, sizeof(ip_request),"GET %s HTTP/1.1\r\nHost: api.ipify.org\r\nUser-Agent: sensecap\r\n\r\n", ip_url);

    len = https_get_request(cfg, ip_url, ip_request);
    if(len > 0) {
        char *p_ip = strstr(local_response_buffer, "\r\n\r\n");
        if( p_ip ) {
            strncpy(ip, p_ip+4, buf_len);
            return 0;
        } else {
            return -1;
        }
    }
    return -1;
}

static int __time_zone_get(char *ip)
{
    esp_tls_cfg_t cfg = {
        .cacert_buf = (const unsigned char *) timeapi_root_cert_pem_start,
        .cacert_bytes = timeapi_root_cert_pem_end - timeapi_root_cert_pem_start,
    };

    char time_zone_url[128] = {0};
    char time_zone_request[200] = {0};
    int len  = 0;
    snprintf(time_zone_url, sizeof(time_zone_url),"https://www.timeapi.io/api/TimeZone/ip?ipAddress=%s",ip);
    snprintf(time_zone_request, sizeof(time_zone_request),"GET %s HTTP/1.1\r\nHost: www.timeapi.io\r\nUser-Agent: sensecap\r\n\r\n", time_zone_url);

    len = https_get_request(cfg, time_zone_url, time_zone_request);
    if( len > 0) {
        // TIME ZONE RESPONSE: HTTP/1.1 200 OK
        // Server: nginx/1.18.0 (Ubuntu)
        // Date: Thu, 02 Feb 2023 09:40:26 GMT
        // Content-Type: application/json; charset=utf-8
        // Transfer-Encoding: chunked
        // Connection: keep-alive

        // 128
        // {"timeZone":"UTC","currentLocalTime":"2023-02-02T09:40:26.1233729","currentUtcOffset":{"seconds":0,"milliseconds":0,"ticks":0,"nanoseconds":0},"standardUtcOffset":{"seconds":0,"milliseconds":0,"ticks":0,"nanoseconds":0},"hasDayLightSaving":false,"isDayLightSavingActive":false,"dstInterval":null}
        // 0
        char *p_json = strstr(local_response_buffer, "\r\n\r\n");
        if( p_json ) {
            p_json =  p_json + 4 + 3; //todo
            return __time_zone_data_prase(p_json);
        } else {
            return -1;
        }
       
    }
    return -1;
}

#else

int https_mbedtls_request(const char *p_url, const char * port, const char *p_request, uint32_t request_len)
{
    int ret, flags, len;

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_x509_crt cacert;
    mbedtls_ssl_config conf;
    mbedtls_net_context server_fd;

    mbedtls_ssl_init(&ssl);
    mbedtls_x509_crt_init(&cacert);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    ESP_LOGI(TAG, "Seeding the random number generator");

    mbedtls_ssl_config_init(&conf);

    mbedtls_entropy_init(&entropy);
    if((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                    NULL, 0)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned %d", ret);
        abort();
    }
    ESP_LOGI(TAG, "Attaching the certificate bundle...");
    ret = esp_crt_bundle_attach(&conf);

    if(ret < 0)
    {
        ESP_LOGE(TAG, "esp_crt_bundle_attach returned -0x%x\n\n", -ret);
        abort();
    }

    ESP_LOGI(TAG, "Setting hostname for TLS session...");

     /* Hostname set here should match CN in server certificate */
    if((ret = mbedtls_ssl_set_hostname(&ssl, p_url)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ssl_set_hostname returned -0x%x", -ret);
        abort();
    }

    ESP_LOGI(TAG, "Setting up the SSL/TLS structure...");

    if((ret = mbedtls_ssl_config_defaults(&conf,
                                          MBEDTLS_SSL_IS_CLIENT,
                                          MBEDTLS_SSL_TRANSPORT_STREAM,
                                          MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned %d", ret);
        goto exit;
    }

    /* MBEDTLS_SSL_VERIFY_OPTIONAL is bad for security, in this example it will print
       a warning if CA verification fails but it will continue to connect.

       You should consider using MBEDTLS_SSL_VERIFY_REQUIRED in your own code.
    */
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    //mbedtls_esp_enable_debug_log(&conf, 1);

#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
    mbedtls_ssl_conf_min_version(&conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_4);
    mbedtls_ssl_conf_max_version(&conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_4);
#endif

    if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ssl_setup returned -0x%x\n\n", -ret);
        goto exit;
    }

    while(1) {
        mbedtls_net_init(&server_fd);

        ESP_LOGI(TAG, "Connecting to %s:%s...", p_url, port);

        if ((ret = mbedtls_net_connect(&server_fd, p_url,
                                      port, MBEDTLS_NET_PROTO_TCP)) != 0)
        {
            ESP_LOGE(TAG, "mbedtls_net_connect returned -%x", -ret);
            goto exit;
        }

        ESP_LOGI(TAG, "Connected.");

        mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

        ESP_LOGI(TAG, "Performing the SSL/TLS handshake...");


        while ((ret = mbedtls_ssl_handshake(&ssl)) != 0)
        {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
            {
                ESP_LOGE(TAG, "mbedtls_ssl_handshake returned -0x%x", -ret);
                goto exit;
            }
        }

        ESP_LOGI(TAG, "Verifying peer X.509 certificate...");

        if ((flags = mbedtls_ssl_get_verify_result(&ssl)) != 0)
        {
            /* In real life, we probably want to close connection if ret != 0 */
            ESP_LOGW(TAG, "Failed to verify peer certificate!");
            bzero(local_response_buffer, sizeof(local_response_buffer));
            mbedtls_x509_crt_verify_info(local_response_buffer, sizeof(local_response_buffer), "  ! ", flags);
            ESP_LOGW(TAG, "verification info: %s", local_response_buffer);
        }
        else {
            ESP_LOGI(TAG, "Certificate verified.");
        }

        ESP_LOGI(TAG, "Cipher suite is %s", mbedtls_ssl_get_ciphersuite(&ssl));

        ESP_LOGI(TAG, "Writing HTTP request...");
        size_t written_bytes = 0;
        do {
            ret = mbedtls_ssl_write(&ssl,
                                    (const unsigned char *)p_request + written_bytes,
                                    request_len- written_bytes);
            if (ret >= 0) {
                ESP_LOGI(TAG, "%d bytes written", ret);
                written_bytes += ret;
            } else if (ret != MBEDTLS_ERR_SSL_WANT_WRITE && ret != MBEDTLS_ERR_SSL_WANT_READ) {
                ESP_LOGE(TAG, "mbedtls_ssl_write returned -0x%x", -ret);
                goto exit;
            }
        } while(written_bytes < request_len);

        ESP_LOGI(TAG, "Reading HTTP response...");
        int recv_len = 0;
        len = sizeof(local_response_buffer) - 1;
        bzero(local_response_buffer, sizeof(local_response_buffer));
        do
        {
            len -= recv_len;
            ret = mbedtls_ssl_read(&ssl, (unsigned char *)local_response_buffer+recv_len, len);

            if(ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
                continue;

            if(ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
                ret = 0;
                break;
            }

            if(ret < 0)
            {
                ESP_LOGE(TAG, "mbedtls_ssl_read returned -0x%x", -ret);
                break;
            }

            if(ret == 0)
            {
                ESP_LOGI(TAG, "connection closed");
                break;
            }

            ESP_LOGD(TAG, "%d bytes read", ret);
            /* Print response directly to stdout as it is read */
            for(int i = 0; i < ret; i++) {
                putchar(local_response_buffer[i+recv_len]);
            }
            recv_len += ret;

        } while(1);

        mbedtls_ssl_close_notify(&ssl);

    exit:
        mbedtls_ssl_session_reset(&ssl);
        mbedtls_net_free(&server_fd);

        if(ret != 0)
        {
            mbedtls_strerror(ret, local_response_buffer, 100);
            ESP_LOGE(TAG, "Last error was: -0x%x - %s", -ret, local_response_buffer);
            return -1;
        }

        putchar('\n'); // JSON output doesn't have a newline at end

        return recv_len;
    }
}

static int __ip_get(char *ip, int buf_len)
{
    char ip_url[128] = {0};
    char ip_request[200] = {0};
    int len;
    snprintf(ip_url, sizeof(ip_url),"https://api.ipify.org");
    len = snprintf(ip_request, sizeof(ip_request),"GET %s HTTP/1.0\r\nHost: api.ipify.org\r\nUser-Agent: esp-idf/1.0 esp32\r\n\r\n", ip_url);

    len = https_mbedtls_request("api.ipify.org", "443", ip_request, len);
    if(len > 0) {
        char *p_ip = strstr(local_response_buffer, "\r\n\r\n");
        if( p_ip ) {
            strncpy(ip, p_ip+4, buf_len);
            return 0;
        } else {
            return -1;
        }
    }
    return -1;
}

static int __time_zone_get(char *ip)
{

    char time_zone_url[128] = {0};
    char time_zone_request[200] = {0};
    int len  = 0;
    snprintf(time_zone_url, sizeof(time_zone_url),"https://www.timeapi.io/api/TimeZone/ip?ipAddress=%s",ip);
    len = snprintf(time_zone_request, sizeof(time_zone_request),"GET %s HTTP/1.1\r\nHost: www.timeapi.io\r\nUser-Agent: sensecap\r\n\r\n", time_zone_url);

    len = https_mbedtls_request("www.timeapi.io", "443", time_zone_request, len);

    if( len > 0) {
        // TIME ZONE RESPONSE: HTTP/1.1 200 OK
        // Server: nginx/1.18.0 (Ubuntu)
        // Date: Thu, 02 Feb 2023 09:40:26 GMT
        // Content-Type: application/json; charset=utf-8
        // Transfer-Encoding: chunked
        // Connection: keep-alive

        // 128
        // {"timeZone":"UTC","currentLocalTime":"2023-02-02T09:40:26.1233729","currentUtcOffset":{"seconds":0,"milliseconds":0,"ticks":0,"nanoseconds":0},"standardUtcOffset":{"seconds":0,"milliseconds":0,"ticks":0,"nanoseconds":0},"hasDayLightSaving":false,"isDayLightSavingActive":false,"dstInterval":null}
        // 0
        char *p_json = strstr(local_response_buffer, "\r\n\r\n");
        if( p_json ) {
            p_json =  p_json + 4 + 3; //todo
            return __time_zone_data_prase(p_json);
        } else {
            return -1;
        }
       
    }
    return -1;
}
#endif

static void __indicator_http_task(void *p_arg)
{
    int err = -1;

    bool city_flag = false;
    bool ip_flag = false;
    bool time_zone_flag = false;

    xSemaphoreTake(__g_http_com_sem, portMAX_DELAY);

    ESP_LOGI(TAG, "start Get city and time zone");

    while(1) {

        if( net_flag  && !city_flag) {
            
            err = __city_get();
            if( err == 0) {
                ESP_LOGI(TAG, "Get succesfully");
                ESP_LOGI(TAG, "ip       : %s", __g_city_model.ip);
                ESP_LOGI(TAG, "city     : %s", __g_city_model.city);
                ESP_LOGI(TAG, "timezone : %s", __g_city_model.timezone);
                city_flag = true;
                ip_flag= true;
                esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_CITY, &__g_city_model.city, sizeof(__g_city_model.city), portMAX_DELAY);
            }
        }

        if( net_flag && !ip_flag ) {
            ESP_LOGI(TAG, "Get ip...");
            err = __ip_get(__g_city_model.ip, sizeof(__g_city_model.ip));
            if( err ==0 ) {
                ESP_LOGI(TAG, "ip: %s", __g_city_model.ip);
                ip_flag= true;
            }
        }
        if(  net_flag && ip_flag && !time_zone_flag) {
            ESP_LOGI(TAG, "Get time zone...");
            err =  __time_zone_get(__g_city_model.ip); 
            if( err == 0) {
                char zone_str[32];
                float zone = __g_city_model.local_utc_offset / 3600.0;

                if( zone >= 0) {
                    snprintf(zone_str, sizeof(zone_str) - 1, "UTC-%.1f", zone);
                } else {
                    snprintf(zone_str, sizeof(zone_str) - 1, "UTC+%.1f", 0 - zone);
                }
                indicator_time_net_zone_set( zone_str );

                time_zone_flag = true;
            }
        }

        if( city_flag  && time_zone_flag) {
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
        
    }
    vTaskDelete(NULL);
}

static void __view_event_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    switch (id)
    {
        case VIEW_EVENT_WIFI_ST: {
            ESP_LOGI(TAG, "event: VIEW_EVENT_WIFI_ST");
            struct view_data_wifi_st *p_st = ( struct view_data_wifi_st *)event_data;
            if( p_st->is_network) {
                net_flag = true;
                xSemaphoreGive(__g_http_com_sem); //right away  get city and time zone
            } else {
                net_flag = false;
            }
            break;
        }
        default:
            break;
    }
}

int indicator_city_init(void)
{
    __g_http_com_sem = xSemaphoreCreateBinary();
    
    xTaskCreate(&__indicator_http_task, "__indicator_http_task", 1024 * 5, NULL, 10, NULL);

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, 
                                                        VIEW_EVENT_BASE, VIEW_EVENT_WIFI_ST, 
                                                        __view_event_handler, NULL, NULL));                                                        
}


