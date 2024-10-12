#include "indicator_lorahub.h"
#include "freertos/semphr.h"
#include "nvs.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_netif_sntp.h"
#include "driver/gpio.h"
#include "driver/temperature_sensor.h"

// #include "wifi.h"
// #include "http_server.h"
#include "pkt_fwd.h"
#include "config_nvs.h"
#include "sx126x_lorahub.h"

#include "indicator_storage_spiffs.h"
#include "indicator_sensor.h"
#include "view_pages.h"

static const char *TAG = "lorahub-model";
static const char *TAG_MAIN = "lorahub";

static temperature_sensor_handle_t temp_sensor = NULL;
volatile bool exit_sig = false; /* 1 -> application terminates cleanly (shut down hardware, close open files, etc) */

static SemaphoreHandle_t __g_data_mutex;
static bool _g_wifi_is_connected = false;

#define USING_NVS 1
#if  USING_NVS == 0
#include "indicator_storage_spiffs.h"
#endif

#define SNTP_ADDRESS_STR_MAX_SIZE ( 64 )
#define LNS_ADDRESS_STR_MAX_SIZE ( 64 )
#define LNS_PORT_STR_MAX_SIZE ( 6 )   /* [0..65535] + \0 */
#define CHAN_FREQ_STR_MAX_SIZE ( 12 ) /* [150.000000..960.000000] + \0 */
#define CHAN_DR_STR_MAX_SIZE ( 4 )    /* [7..12] + \0 */
#define CHAN_BW_STR_MAX_SIZE ( 4 )    /* [125..500] + \0 */

char    web_inf_mac_addr_str[18] = "unknown";
uint8_t web_inf_mac_addr[6]      = { 0 };

static char     web_cfg_lns_address[LNS_ADDRESS_STR_MAX_SIZE]        = { 0 };
static char     web_cfg_lns_port_str[LNS_PORT_STR_MAX_SIZE]          = { 0 };
static char     web_cfg_chan_freq_mhz_str[CHAN_FREQ_STR_MAX_SIZE]    = { 0 };
static char     web_cfg_chan_datarate_str[CHAN_DR_STR_MAX_SIZE]      = { 0 }; /* TODO check size for FSK datarate */
static char     web_cfg_chan_bandwidth_khz_str[CHAN_BW_STR_MAX_SIZE] = { 0 };
static char     web_cfg_sntp_address[SNTP_ADDRESS_STR_MAX_SIZE] = { 0 };

static uint16_t web_cfg_lns_port                                     = 0;
static double   web_cfg_chan_freq_mhz                                = 0;
static uint32_t web_cfg_chan_freq_hz;
static uint32_t web_cfg_chan_datarate                           = 0; /* uint32 for FSK datarate */
static uint16_t web_cfg_chan_bandwidth_khz                      = 0;

static struct lorahub __g_lorahub;

static esp_err_t lorahub_cfg_set( void );
static esp_err_t lorahub_cfg_get(void);
static esp_err_t lorahub_cfg_update_from_ui(void);

void wait_on_error( lorahub_error_t error, int line )
{
    while( 1 )
    {
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    };
}

static void __lorahub_task(void *p_arg)
{
    ESP_LOGI(TAG, "Starting LoRaHUB task");
    static bool lorahub_started = false;

    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &temp_sensor));
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_sensor));

    while (!exit_sig) {
        if(_g_wifi_is_connected)
        {
            if(!lorahub_started)
            {
                ESP_LOGI(TAG, "lora gateway started !");
                launch_pkt_fwd(temp_sensor);
                lorahub_started = true;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    ESP_LOGI( TAG_MAIN, "INFO: Exiting LoRaHUB\n" );
    /* Reset the ESP32 */
    esp_restart( );

    vTaskDelete(NULL);
}

static void __view_event_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
    switch (id) {
        case VIEW_EVENT_SENSOR_DATA: {
            struct view_data_sensor_data *p_data = (struct view_data_sensor_data *)event_data;
            // Handle sensor data event
            break;
        }

        case VIEW_EVENT_WIFI_ST:{
            ESP_LOGI(TAG, "VIEW_EVENT_WIFI_ST");
            struct view_data_wifi_st *view_wifi_msg = (struct view_data_wifi_st *)event_data;
            ESP_LOGI(TAG, "WiFi connection %s", view_wifi_msg->is_connected ? "successful" :  "failed" );
            if(view_wifi_msg->is_connected)
            {
                _g_wifi_is_connected = true;
                lv_img_set_src(ui_LblLoRaStatus1, &ui_img_lora_connected_png);
                lv_img_set_src(ui_LblLoRaStatus2, &ui_img_lora_connected_png);
            }else{
                _g_wifi_is_connected = false;
                lv_img_set_src(ui_LblLoRaStatus1, &ui_img_lora_disconnect_png);
                lv_img_set_src(ui_LblLoRaStatus2, &ui_img_lora_disconnect_png);
            }
            break;
        }

        case VIEW_EVENT_LORAHUB_CONFIGURE:{
            ESP_LOGI(TAG, "VIEW_EVENT_LORAHUB_CONFIGURE");
            esp_err_t err = lorahub_cfg_update_from_ui();
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to update configuration from UI");
            } else {
                ESP_LOGI(TAG, "Configuration updated successfully");

                lorahub_cfg_get();
            }

            break;
        }

        case VIEW_EVENT_LORAHUB_REBOOT:{
            ESP_LOGI(TAG, "VIEW_EVENT_LORAHUB_REBOOT");

            indicator_spiffs_deinit();
            vTaskDelay(pdMS_TO_TICKS(20));
            esp_restart();

            break;
        }

        default:
            break;
    }
}

static esp_err_t lorahub_cfg_get(void)
{
    esp_err_t err = ESP_OK;

    wifi_get_mac_address( web_inf_mac_addr );
    snprintf( web_inf_mac_addr_str, sizeof web_inf_mac_addr_str, "%02x:%02x:%02x:%02x:%02x:%02x", web_inf_mac_addr[0],
              web_inf_mac_addr[1], web_inf_mac_addr[2], web_inf_mac_addr[3], web_inf_mac_addr[4], web_inf_mac_addr[5] );
    
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_LORAHUB_MAC, web_inf_mac_addr_str, sizeof(web_inf_mac_addr_str), portMAX_DELAY);

    /* Get default values from menuconfig (used if never set to NVS) */
    snprintf( __g_lorahub.web_cfg_chan_freq_mhz_str, sizeof __g_lorahub.web_cfg_chan_freq_mhz_str, "%.6f",
              ( ( double ) CONFIG_CHANNEL_FREQ_HZ / 1e6 ) ); /* hz to mhz string */
    snprintf( __g_lorahub.web_cfg_chan_datarate_str, sizeof __g_lorahub.web_cfg_chan_datarate_str, "%" PRIu32,
              ( uint32_t ) CONFIG_CHANNEL_LORA_DATARATE );
    snprintf( __g_lorahub.web_cfg_chan_bandwidth_khz_str, sizeof __g_lorahub.web_cfg_chan_bandwidth_khz_str, "%" PRIu16,
              ( uint16_t ) CONFIG_CHANNEL_LORA_BANDWIDTH );
    snprintf( __g_lorahub.web_cfg_lns_address, sizeof __g_lorahub.web_cfg_lns_address, "%s", CONFIG_NETWORK_SERVER_ADDRESS );
    snprintf( __g_lorahub.web_cfg_lns_port_str, sizeof __g_lorahub.web_cfg_lns_port_str, "%" PRIu16, ( uint16_t ) CONFIG_NETWORK_SERVER_PORT );
    snprintf( __g_lorahub.web_cfg_sntp_address, sizeof __g_lorahub.web_cfg_sntp_address, "%s", CONFIG_SNTP_SERVER_ADDRESS );

    /* Get configuration from NVS */
    printf( "Opening Non-Volatile Storage (NVS) handle for reading... " );
    nvs_handle_t my_handle;
    err = nvs_open( "storage", NVS_READONLY, &my_handle );
    if( err != ESP_OK )
    {
        ESP_LOGW( TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name( err ) );
    }
    else
    {
        printf( "Done\n" );

        size_t size = sizeof( __g_lorahub.web_cfg_lns_address );
        err         = nvs_get_str( my_handle, CFG_NVS_KEY_LNS_ADDRESS, __g_lorahub.web_cfg_lns_address, &size );
        if( err == ESP_OK )
        {
            printf( "NVS -> %s = %s\n", CFG_NVS_KEY_LNS_ADDRESS, __g_lorahub.web_cfg_lns_address );
        }
        else
        {
            ESP_LOGW( TAG, "Failed to get %s from NVS - %s\n", CFG_NVS_KEY_LNS_ADDRESS, esp_err_to_name( err ) );
        }

        err = nvs_get_u16( my_handle, CFG_NVS_KEY_LNS_PORT, &web_cfg_lns_port );
        if( err == ESP_OK )
        {
            printf( "NVS -> %s = %" PRIu16 "\n", CFG_NVS_KEY_LNS_PORT, web_cfg_lns_port );
            snprintf( __g_lorahub.web_cfg_lns_port_str, sizeof __g_lorahub.web_cfg_lns_port_str, "%" PRIu16, web_cfg_lns_port );
        }
        else
        {
            ESP_LOGW( TAG, "Failed to get %s from NVS - %s\n", CFG_NVS_KEY_LNS_PORT, esp_err_to_name( err ) );
        }

        err = nvs_get_u32( my_handle, CFG_NVS_KEY_CHAN_FREQ, &web_cfg_chan_freq_hz );
        if( err == ESP_OK )
        {
            printf( "NVS -> %s = %" PRIu32 "hz\n", CFG_NVS_KEY_CHAN_FREQ, web_cfg_chan_freq_hz );
            snprintf( __g_lorahub.web_cfg_chan_freq_mhz_str, sizeof __g_lorahub.web_cfg_chan_freq_mhz_str, "%.1f",
                      ( ( double ) web_cfg_chan_freq_hz / 1e6 ) ); /* hz to mhz string */
        }
        else
        {
            ESP_LOGW( TAG, "Failed to get %s from NVS - %s\n", CFG_NVS_KEY_CHAN_FREQ, esp_err_to_name( err ) );
        }

        err = nvs_get_u32( my_handle, CFG_NVS_KEY_CHAN_DR, &web_cfg_chan_datarate );
        if( err == ESP_OK )
        {
            printf( "NVS -> %s = %" PRIu32 "\n", CFG_NVS_KEY_CHAN_DR, web_cfg_chan_datarate );
            snprintf( __g_lorahub.web_cfg_chan_datarate_str, sizeof __g_lorahub.web_cfg_chan_datarate_str, "%" PRIu32, web_cfg_chan_datarate );
        }
        else
        {
            ESP_LOGW( TAG, "Failed to get %s from NVS - %s\n", CFG_NVS_KEY_CHAN_DR, esp_err_to_name( err ) );
        }

        err = nvs_get_u16( my_handle, CFG_NVS_KEY_CHAN_BW, &web_cfg_chan_bandwidth_khz );
        if( err == ESP_OK )
        {
            printf( "NVS -> %s = %" PRIu16 "khz\n", CFG_NVS_KEY_CHAN_BW, web_cfg_chan_bandwidth_khz );
            snprintf( __g_lorahub.web_cfg_chan_bandwidth_khz_str, sizeof __g_lorahub.web_cfg_chan_bandwidth_khz_str, "%" PRIu16,
                      web_cfg_chan_bandwidth_khz );
        }
        else
        {
            ESP_LOGW( TAG, "Failed to get %s from NVS - %s\n", CFG_NVS_KEY_CHAN_BW, esp_err_to_name( err ) );
        }

        size = sizeof( __g_lorahub.web_cfg_sntp_address );
        err  = nvs_get_str( my_handle, CFG_NVS_KEY_SNTP_ADDRESS, __g_lorahub.web_cfg_sntp_address, &size );
        if( err == ESP_OK )
        {
            printf( "NVS -> %s = %s\n", CFG_NVS_KEY_SNTP_ADDRESS, __g_lorahub.web_cfg_sntp_address );
        }
        else
        {
            ESP_LOGW( TAG, "Failed to get %s from NVS - %s\n", CFG_NVS_KEY_SNTP_ADDRESS, esp_err_to_name( err ) );
        }
    }
    nvs_close( my_handle );

    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_LORAHUB_DATA_UPDATE, &__g_lorahub, sizeof(__g_lorahub), portMAX_DELAY);

    return ESP_OK;
}

static esp_err_t lorahub_cfg_set( void )
{
    esp_err_t    err = ESP_OK;
    nvs_handle_t my_handle;

    printf( "Opening Non-Volatile Storage (NVS) handle for writing... " );
    err = nvs_open( "storage", NVS_READWRITE, &my_handle );
    if( err != ESP_OK )
    {
        printf( "Error (%s) opening NVS handle!\n", esp_err_to_name( err ) );
        return ESP_FAIL;
    }
    else
    {
        printf( "Done\n" );
    }

    printf( "NVS <- %s = %s ... ", CFG_NVS_KEY_LNS_ADDRESS, web_cfg_lns_address );
    err = nvs_set_str( my_handle, CFG_NVS_KEY_LNS_ADDRESS, web_cfg_lns_address );
    if( err == ESP_OK )
    {
        printf( "Done\n" );
    }
    else
    {
        printf( "Failed\n" );
        nvs_close( my_handle );
        printf( "Closed NVS handle for writing.\n" );
        return ESP_FAIL;
    }

    printf( "NVS <- %s = %" PRIu16 " ... ", CFG_NVS_KEY_LNS_PORT, web_cfg_lns_port );
    err = nvs_set_u16( my_handle, CFG_NVS_KEY_LNS_PORT, web_cfg_lns_port );
    if( err == ESP_OK )
    {
        printf( "Done\n" );
    }
    else
    {
        printf( "Failed\n" );
        nvs_close( my_handle );
        printf( "Closed NVS handle for writing.\n" );
        return ESP_FAIL;
    }

    web_cfg_chan_freq_hz = ( uint32_t )( ( double ) ( 1.0e6 ) * web_cfg_chan_freq_mhz ); /* mhz -> hz */
    printf( "NVS <- %s = %" PRIu32 " ... ", CFG_NVS_KEY_CHAN_FREQ, web_cfg_chan_freq_hz );
    err = nvs_set_u32( my_handle, CFG_NVS_KEY_CHAN_FREQ, web_cfg_chan_freq_hz );
    if( err == ESP_OK )
    {
        printf( "Done\n" );
    }
    else
    {
        printf( "Failed\n" );
        nvs_close( my_handle );
        printf( "Closed NVS handle for writing.\n" );
        return ESP_FAIL;
    }

    printf( "NVS <- %s = %" PRIu32 " ... ", CFG_NVS_KEY_CHAN_DR, web_cfg_chan_datarate );
    err = nvs_set_u32( my_handle, CFG_NVS_KEY_CHAN_DR, web_cfg_chan_datarate );
    if( err == ESP_OK )
    {
        printf( "Done\n" );
    }
    else
    {
        printf( "Failed\n" );
        nvs_close( my_handle );
        printf( "Closed NVS handle for writing.\n" );
        return ESP_FAIL;
    }

    printf( "NVS <- %s = %" PRIu16 " ... ", CFG_NVS_KEY_CHAN_BW, web_cfg_chan_bandwidth_khz );
    err = nvs_set_u16( my_handle, CFG_NVS_KEY_CHAN_BW, web_cfg_chan_bandwidth_khz );
    if( err == ESP_OK )
    {
        printf( "Done\n" );
    }
    else
    {
        printf( "Failed\n" );
        nvs_close( my_handle );
        printf( "Closed NVS handle for writing.\n" );
        return ESP_FAIL;
    }

    printf( "NVS <- %s = %s ... ", CFG_NVS_KEY_SNTP_ADDRESS, web_cfg_sntp_address );
    err = nvs_set_str( my_handle, CFG_NVS_KEY_SNTP_ADDRESS, web_cfg_sntp_address );
    if( err == ESP_OK )
    {
        printf( "Done\n" );
    }
    else
    {
        printf( "Failed\n" );
        nvs_close( my_handle );
        printf( "Closed NVS handle for writing.\n" );
        return ESP_FAIL;
    }

    printf( "Committing updates in NVS ... " );
    err = nvs_commit( my_handle );
    if( err == ESP_OK )
    {
        printf( "Done\n" );
    }
    else
    {
        printf( "Failed\n" );
        nvs_close( my_handle );
        printf( "Closed NVS handle for writing.\n" );
        return ESP_FAIL;
    }

    nvs_close( my_handle );
    printf( "Closed NVS handle for writing.\n" );

    return ESP_OK;
}

static esp_err_t lorahub_cfg_update_from_ui(void)
{
    esp_err_t err = ESP_OK;

    // Read data from UI components
    const char* lns_address = lv_textarea_get_text(objects.adresstext);
    const char* lns_port = lv_textarea_get_text(objects.porttext);
    const char* chan_freq = lv_textarea_get_text(objects.freqtext);
    uint16_t datarate = lv_dropdown_get_selected(objects.spreadfactordw) + 7; // Adding 7 because the dropdown starts from 0
    uint16_t bandwidth = 125; // Default value
    switch(lv_dropdown_get_selected(objects.bandwidth)) {
        case 0: bandwidth = 125; break;
        case 1: bandwidth = 250; break;
        case 2: bandwidth = 500; break;
    }
    const char* sntp_address = lv_textarea_get_text(objects.sntptext);

    // Update local variables
    strncpy(web_cfg_lns_address, lns_address, sizeof(web_cfg_lns_address) - 1);
    web_cfg_lns_port = atoi(lns_port);
    web_cfg_chan_freq_mhz = atof(chan_freq);
    web_cfg_chan_datarate = datarate;
    web_cfg_chan_bandwidth_khz = bandwidth;
    strncpy(web_cfg_sntp_address, sntp_address, sizeof(web_cfg_sntp_address) - 1);

    // Call the existing function to write to NVS
    err = lorahub_cfg_set();

    return err;
}


int indicator_lorahub_init(void)
{
    ESP_ERROR_CHECK(lorahub_cfg_get());

    xTaskCreate(&__lorahub_task, "__lorahub_task", 1024 * 4, NULL, 2, NULL);

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                            VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA,
                                                            __view_event_handler, NULL, NULL));
                                                        
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                            VIEW_EVENT_BASE, VIEW_EVENT_WIFI_ST,
                                                            __view_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                            VIEW_EVENT_BASE, VIEW_EVENT_LORAHUB_CONFIGURE,
                                                            __view_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                            VIEW_EVENT_BASE, VIEW_EVENT_LORAHUB_REBOOT,
                                                            __view_event_handler, NULL, NULL));


    return 0;
}