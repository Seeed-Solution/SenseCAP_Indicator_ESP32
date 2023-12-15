#include "indicator_wifi.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "esp_event.h"
#include "ping/ping_sock.h"


#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

struct indicator_wifi
{
    struct view_data_wifi_st  st;
    bool is_cfg;
    int wifi_reconnect_cnt;  
};

static struct indicator_wifi _g_wifi_model;
static SemaphoreHandle_t   __g_wifi_mutex;
static SemaphoreHandle_t   __g_data_mutex;
static SemaphoreHandle_t   __g_net_check_sem;


static int s_retry_num = 0;
static int wifi_retry_max  = 3;
static bool __g_ping_done = true;


static EventGroupHandle_t __wifi_event_group;

static const char *TAG = "model_wifi";

static int min(int a, int b) { return (a < b) ? a : b; }

static void __wifi_st_set( struct view_data_wifi_st *p_st )
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    memcpy( &_g_wifi_model.st, p_st, sizeof(struct view_data_wifi_st));
    xSemaphoreGive(__g_data_mutex);
}

static void __wifi_st_get(struct view_data_wifi_st *p_st )
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    memcpy(p_st, &_g_wifi_model.st, sizeof(struct view_data_wifi_st));
    xSemaphoreGive(__g_data_mutex);
}

static void __wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{

    switch (event_id)
    {
        case WIFI_EVENT_STA_START: {
            ESP_LOGI(TAG, "wifi event: WIFI_EVENT_STA_START");
            struct view_data_wifi_st st;
            st.is_connected = false;
            st.is_network   = false;
            st.is_connecting = true;
            memset(st.ssid, 0,  sizeof(st.ssid));
            st.rssi = 0;
            __wifi_st_set(&st);

            esp_wifi_connect();
            break;
        }
        case WIFI_EVENT_STA_CONNECTED: {
            ESP_LOGI(TAG, "wifi event: WIFI_EVENT_STA_CONNECTED");
            wifi_event_sta_connected_t *event = (wifi_event_sta_connected_t*) event_data;
            struct view_data_wifi_st st;

            __wifi_st_get(&st);
            memset(st.ssid, 0,  sizeof(st.ssid));
            memcpy(st.ssid, event->ssid, event->ssid_len);
            st.rssi = -50; //todo
            st.is_connected = true;
            st.is_connecting = false;
            __wifi_st_set(&st);
            
            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_WIFI_ST, &st, sizeof(struct view_data_wifi_st ), portMAX_DELAY);
            
            struct view_data_wifi_connet_ret_msg msg;
            msg.ret = 0;
            strcpy(msg.msg, "Connection successful");
            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_WIFI_CONNECT_RET, &msg, sizeof(msg), portMAX_DELAY);
            break;
        }
        case WIFI_EVENT_STA_DISCONNECTED: {
            ESP_LOGI(TAG, "wifi event: WIFI_EVENT_STA_DISCONNECTED");

            if ( (wifi_retry_max == -1) || s_retry_num < wifi_retry_max) {
                esp_wifi_connect();
                s_retry_num++;
                ESP_LOGI(TAG, "retry to connect to the AP");

            } else {
                
                // update list  todo
                struct view_data_wifi_st st;
        
                __wifi_st_get(&st);
                st.is_connected = false;
                st.is_network   = false;
                st.is_connecting = false;
                __wifi_st_set(&st);

                esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_WIFI_ST, &st, sizeof(struct view_data_wifi_st ), portMAX_DELAY);
                
                char *p_str = "";
                struct view_data_wifi_connet_ret_msg msg;
                msg.ret = 0;
                strcpy(msg.msg, "Connection failure");
                esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_WIFI_CONNECT_RET, &msg, sizeof(msg), portMAX_DELAY);
            }
            break;
        }
    default:
        break;
    }
}

static void __ip_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if ( event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;

        //xEventGroupSetBits(__wifi_event_group, WIFI_CONNECTED_BIT);
        xSemaphoreGive(__g_net_check_sem);  //goto check network
    }
}


// bool indicator_wifi_is_connect(char *p_ssid, int *p_rssi)
// {
//     return true; //todo
// }

static int __wifi_scan(wifi_ap_record_t *p_ap_info, uint16_t number)
{
    uint16_t ap_count;
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_start());
    esp_wifi_scan_start(NULL, true);
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, p_ap_info));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    ESP_LOGI(TAG, " scan ap cont: %d", ap_count);
    
    for (int i = 0; (i < number) && (i < ap_count); i++) {
        ESP_LOGI(TAG, "SSID: %s, RSSI:%d, Channel: %d", p_ap_info[i].ssid, p_ap_info[i].rssi, p_ap_info[i].primary);
    }
    return ap_count;
}


static int __wifi_connect(const char *p_ssid, const char *p_password, int retry_num)
{
    wifi_retry_max = retry_num; //todo
    s_retry_num =0;

    wifi_config_t wifi_config = {0};
    strlcpy((char *)wifi_config.sta.ssid, p_ssid, sizeof(wifi_config.sta.ssid));
    ESP_LOGI(TAG, "ssid: %s", p_ssid);
    if( p_password ) {
        ESP_LOGI(TAG, "password: %s", p_password);
        strlcpy((char *)wifi_config.sta.password, p_password, sizeof(wifi_config.sta.password));
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK; //todo
    } else {
        wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
    }
    wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;

    esp_wifi_stop();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );

    _g_wifi_model.is_cfg = true;

    struct view_data_wifi_st st = {0};
    st.is_connected = false;
    st.is_connecting = false;
    st.is_network   = false;
    __wifi_st_set(&st);

    ESP_ERROR_CHECK(esp_wifi_start());
    //esp_wifi_connect();
    
    ESP_LOGI(TAG, "connect...");
}

static void __wifi_cfg_restore(void) 
{
    _g_wifi_model.is_cfg = false;
    
    struct view_data_wifi_st st = {0};
    st.is_connected = false;
    st.is_connecting = false;
    st.is_network   = false;
    __wifi_st_set(&st);

    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_WIFI_ST, &st, sizeof(struct view_data_wifi_st ), portMAX_DELAY);

    // restore and stop
    esp_wifi_restore();
}

static void __wifi_shutdown(void) 
{
    _g_wifi_model.is_cfg = false;  //disable reconnect
    
    struct view_data_wifi_st st = {0};
    st.is_connected = false;
    st.is_connecting = false;
    st.is_network   = false;
    __wifi_st_set(&st);

    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_WIFI_ST, &st, sizeof(struct view_data_wifi_st ), portMAX_DELAY);

    esp_wifi_stop();
}

static void __ping_end(esp_ping_handle_t hdl, void *args)
{
    ip_addr_t target_addr;
    uint32_t transmitted =0;
    uint32_t received =0;
    uint32_t total_time_ms =0 ;
    uint32_t loss = 0;
    esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
    esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));
    
    if( transmitted > 0 ) {
        loss = (uint32_t)((1 - ((float)received) / transmitted) * 100);
    } else {
        loss = 100;
    }
     
    if (IP_IS_V4(&target_addr)) {
        printf("\n--- %s ping statistics ---\n", inet_ntoa(*ip_2_ip4(&target_addr)));
    } else {
        printf("\n--- %s ping statistics ---\n", inet6_ntoa(*ip_2_ip6(&target_addr)));
    }
    printf("%d packets transmitted, %d received, %d%% packet loss, time %dms\n",
           transmitted, received, loss, total_time_ms);

    esp_ping_delete_session(hdl);

    struct view_data_wifi_st st;
    if( received > 0) {
        __wifi_st_get(&st);
        st.is_network = true;
        __wifi_st_set(&st);
    } else {
        __wifi_st_get(&st);
        st.is_network = false;
        __wifi_st_set(&st);
    }
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_WIFI_ST, &st, sizeof(struct view_data_wifi_st ), portMAX_DELAY);
    __g_ping_done = true;
}

static void __ping_start(void)
{
    esp_ping_config_t config = ESP_PING_DEFAULT_CONFIG();

    ip_addr_t target_addr;
    ipaddr_aton("8.8.8.8", &target_addr);

    config.target_addr = target_addr;

    esp_ping_callbacks_t cbs = {
        .cb_args = NULL,
        .on_ping_success = NULL,
        .on_ping_timeout = NULL,
        .on_ping_end = __ping_end
    };
    esp_ping_handle_t ping;
    esp_ping_new_session(&config, &cbs, &ping);
    __g_ping_done = false;
    esp_ping_start(ping);
}

// net check
static void __indicator_wifi_task(void *p_arg)
{
    int cnt = 0;
    struct view_data_wifi_st st;

    while(1) {

        xSemaphoreTake(__g_net_check_sem, pdMS_TO_TICKS(5000));
        __wifi_st_get(&st);

        // Periodically check the network connection status
        if( st.is_connected) {

            if(__g_ping_done ) {
                if( st.is_network ) {
                    cnt++;
                    //5min check network
                    if( cnt > 60) {
                        cnt = 0;
                        ESP_LOGI(TAG, "Network normal last time, retry check network...");
                        __ping_start();
                    }
                } else {
                    ESP_LOGI(TAG, "Last network exception, check network...");
                    __ping_start();
                }
            }

        } else if(  _g_wifi_model.is_cfg && !st.is_connecting) {
            // Periodically check the wifi connection status

            // 5min retry connect 
            if( _g_wifi_model.wifi_reconnect_cnt > 5 ) {
                ESP_LOGI(TAG, " Wifi reconnect...");
                _g_wifi_model.wifi_reconnect_cnt =0;
                wifi_retry_max = 3;
                s_retry_num =0;

                esp_wifi_stop();
                ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
                ESP_ERROR_CHECK(esp_wifi_start());
            }
            _g_wifi_model.wifi_reconnect_cnt++;
        }

    }
}



static void __view_event_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    switch (id)
    {
        case VIEW_EVENT_WIFI_LIST_REQ: { // when enter wifi config screen
            ESP_LOGI(TAG, "event: VIEW_EVENT_WIFI_LIST_REQ");

            uint16_t number = WIFI_SCAN_LIST_SIZE;
            uint16_t ap_count = 0;
            wifi_ap_record_t ap_info[WIFI_SCAN_LIST_SIZE];
            ap_count = __wifi_scan(ap_info, number);

            struct view_data_wifi_list list;
            struct view_data_wifi_st st;

            memset(&list, 0 , sizeof(struct view_data_wifi_list ));
            
            __wifi_st_get(&st);

            list.is_connect = st.is_connected;  // grab the current connection status
            if( st.is_connected ) {
                strlcpy((char *)list.connect.ssid, (char *)st.ssid, sizeof(list.connect.ssid));
                list.connect.auth_mode =false;
                list.connect.rssi = st.rssi;
            }
            
            ap_count= min(number, ap_count);
            
            bool is_exist =  false;
            int list_cnt = 0;
            for(int i = 0; i <  ap_count; i++ ) {

                is_exist = false;
                for( int j = 0; j < list_cnt; j++ ) {
                    if(strcmp(list.aps[j].ssid, ap_info[i].ssid) == 0) {
                        ESP_LOGI(TAG, "list exit ap:%s", ap_info[i].ssid);
                        is_exist = true;
                        break;
                    }
                }
                if(!is_exist) {
                    strcpy(list.aps[list_cnt].ssid, ap_info[i].ssid);
                    list.aps[list_cnt].rssi = ap_info[i].rssi;
                    if( ap_info[i].authmode == WIFI_AUTH_OPEN) {
                        list.aps[list_cnt].auth_mode = false;
                    } else {
                        list.aps[list_cnt].auth_mode = true;
                    } 
                    list_cnt++;
                }
            }
            list.cnt = list_cnt;
            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_WIFI_LIST, &list, sizeof(struct view_data_wifi_list ), portMAX_DELAY);

            break;
        }
        case VIEW_EVENT_WIFI_CONNECT: {
            ESP_LOGI(TAG, "event: VIEW_EVENT_WIFI_CONNECT");
            struct view_data_wifi_config * p_cfg = (struct view_data_wifi_config *)event_data;

            if( p_cfg->have_password) {
                __wifi_connect(p_cfg->ssid, p_cfg->password, 3);
            } else {
                __wifi_connect(p_cfg->ssid, NULL, 3);
            }
            break;
        }  
        case VIEW_EVENT_WIFI_CFG_DELETE: {
            ESP_LOGI(TAG, "event: VIEW_EVENT_WIFI_CFG_DELETE");
            __wifi_cfg_restore();
            break;
        }
        case VIEW_EVENT_SHUTDOWN: {
            ESP_LOGI(TAG, "event: VIEW_EVENT_SHUTDOWN");
            __wifi_shutdown();
            break;
        }
    default:
        break;
    }
}

static void __wifi_model_init(void)
{
    memset(&_g_wifi_model, 0, sizeof(_g_wifi_model));
}

int indicator_wifi_init(void)
{
    __g_wifi_mutex  = xSemaphoreCreateMutex( );
    __g_data_mutex  =  xSemaphoreCreateMutex();
    __g_net_check_sem = xSemaphoreCreateBinary();
    //__wifi_event_group = xEventGroupCreate();

    __wifi_model_init();
    
    xTaskCreate(&__indicator_wifi_task, "__indicator_wifi_task", 1024 * 5, NULL, 10, NULL);


    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &__wifi_event_handler,
                                                        0,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &__ip_event_handler,
                                                        0,
                                                        &instance_got_ip));


    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_WIFI_LIST_REQ, 
                                                            __view_event_handler, NULL, NULL));

    
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_WIFI_CONNECT, 
                                                            __view_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_WIFI_CFG_DELETE, 
                                                            __view_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_SHUTDOWN, 
                                                            __view_event_handler, NULL, NULL));

    wifi_config_t wifi_cfg;
    esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg);
   
    if (strlen((const char *) wifi_cfg.sta.ssid)) {
        _g_wifi_model.is_cfg = true;
        ESP_LOGI(TAG, "last config ssid: %s",  wifi_cfg.sta.ssid);
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_start());
    } else {
        ESP_LOGI(TAG, "Not config wifi, Entry wifi config screen");
        uint8_t screen = SCREEN_WIFI_CONFIG;
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_start());
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SCREEN_START, &screen, sizeof(screen), portMAX_DELAY);
    }

    return 0;
}
