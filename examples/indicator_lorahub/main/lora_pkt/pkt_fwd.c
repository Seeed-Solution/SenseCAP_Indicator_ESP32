/*______                              _
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
  (C)2024 Semtech

Description:
    Configure LoRaHub and forward packets to a server over UDP.

License: Revised BSD License, see LICENSE.TXT file include in the project
*/

/* -------------------------------------------------------------------------- */
/* --- DEPENDENCIES --------------------------------------------------------- */

#include <stdint.h>  /* C99 types */
#include <stdbool.h> /* bool type */
#include <time.h>    /* time, clock_gettime, strftime, gmtime */
#include <stdlib.h>  /* atoi, exit */
#include <math.h>    /* modf */

#include <sys/types.h>
#include <sys/socket.h> /* socket specific definitions */
#include <netdb.h>
#include <arpa/inet.h> /* IP address conversion stuff */
#include <pthread.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>
#include <esp_pthread.h>

#include <nvs_flash.h>

/* Packet forwarder helpers and HAL */
#include "pkt_fwd.h"
#include "trace.h"
#include "jitqueue.h"
#include "parson.h"
#include "base64.h"
#include "lorahub_hal.h"

/* Services */
// #include "display.h"
// #include "wifi.h"
#include <esp_netif.h>
#include <esp_wifi.h>
#include <esp_mac.h>

#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_ble.h>

#include "indicator_lorahub.h"
#include "config_nvs.h"

/* -------------------------------------------------------------------------- */
/* --- PRIVATE MACROS ------------------------------------------------------- */

#define ARRAY_SIZE( a ) ( sizeof( a ) / sizeof( ( a )[0] ) )
#define STRINGIFY( x ) #x
#define STR( x ) STRINGIFY( x )

/* -------------------------------------------------------------------------- */
/* --- PRIVATE CONSTANTS ---------------------------------------------------- */

#define DEFAULT_PORT_UP 1700
#define DEFAULT_PORT_DW 1700
#define DEFAULT_KEEPALIVE 10 /* default time interval for downstream keep-alive packet */
#define DEFAULT_STAT 30      /* default time interval for statistics */
#define PUSH_TIMEOUT_MS 100
#define PULL_TIMEOUT_MS 200
#define FETCH_SLEEP_MS 10 /* nb of ms waited when a fetch return no packets */

#define PROTOCOL_VERSION 2 /* v1.3 */
#define PROTOCOL_JSON_RXPK_FRAME_FORMAT 1

#define PKT_PUSH_DATA 0
#define PKT_PUSH_ACK 1
#define PKT_PULL_DATA 2
#define PKT_PULL_RESP 3
#define PKT_PULL_ACK 4
#define PKT_TX_ACK 5

#define NB_PKT_MAX 1 /* max number of packets per fetch/send cycle */

#define STATUS_SIZE 200
#define TX_BUFF_SIZE ( ( 540 * NB_PKT_MAX ) + 30 + STATUS_SIZE )
#define ACK_BUFF_SIZE 64

/* ESP32 logging tags */
static const char* TAG_PKT_FWD = "lora-pkt-fwd";
static const char* TAG_UP      = "th_up";
static const char* TAG_DOWN    = "th_down";
static const char* TAG_JIT     = "th_jit";

/* -------------------------------------------------------------------------- */
/* --- PRIVATE TYPES -------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PRIVATE VARIABLES (GLOBAL) ------------------------------------------- */

/* signal handling variables */
extern volatile bool exit_sig;

/* packets filtering configuration variables */
static bool fwd_valid_pkt = true;  /* packets with PAYLOAD CRC OK are forwarded */
static bool fwd_error_pkt = false; /* packets with PAYLOAD CRC ERROR are NOT forwarded */
static bool fwd_nocrc_pkt = false; /* packets with NO PAYLOAD CRC are NOT forwarded */

/* network configuration variables */
static uint64_t lgwm              = 0;                                 /* LoRa gateway MAC address */
static char     serv_addr[64]     = CONFIG_NETWORK_SERVER_ADDRESS;     /* address of the server (host name or IPv4) */
static char     serv_port_up[8]   = STR( CONFIG_NETWORK_SERVER_PORT ); /* server port for upstream traffic */
static char     serv_port_down[8] = STR( CONFIG_NETWORK_SERVER_PORT ); /* server port for downstream traffic */
static int      keepalive_time = DEFAULT_KEEPALIVE; /* send a PULL_DATA request every X seconds, negative = disabled */

/* statistics collection configuration variables */
static unsigned stat_interval =
    DEFAULT_STAT; /* time interval (in sec) at which statistics are collected and displayed */

/* gateway <-> MAC protocol variables */
static uint32_t net_mac_h; /* Most Significant Nibble, network order */
static uint32_t net_mac_l; /* Least Significant Nibble, network order */
static uint8_t  wifi_mac_addr[6] = { 0 };

/* network sockets */
static int sock_up;   /* socket for upstream traffic */
static int sock_down; /* socket for downstream traffic */

/* network protocol variables */
static struct timeval push_timeout_half = { 0, ( PUSH_TIMEOUT_MS * 500 ) };  /* cut in half, critical for throughput */
static struct timeval pull_timeout      = { 0, ( PULL_TIMEOUT_MS * 1000 ) }; /* non critical for throughput */

/* hardware access control and correction */
pthread_mutex_t mx_concent = PTHREAD_MUTEX_INITIALIZER; /* control access to the concentrator */

/* measurements to establish statistics */
static pthread_mutex_t mx_meas_up       = PTHREAD_MUTEX_INITIALIZER; /* control access to the upstream measurements */
static uint32_t        meas_nb_rx_rcv   = 0;                         /* count packets received */
static uint32_t        meas_nb_rx_ok    = 0;                         /* count packets received with PAYLOAD CRC OK */
static uint32_t        meas_nb_rx_bad   = 0;                         /* count packets received with PAYLOAD CRC ERROR */
static uint32_t        meas_nb_rx_nocrc = 0;                         /* count packets received with NO PAYLOAD CRC */
static uint32_t        meas_up_pkt_fwd  = 0;     /* number of radio packet forwarded to the server */
static uint32_t        meas_up_network_byte = 0; /* sum of UDP bytes sent for upstream traffic */
static uint32_t        meas_up_payload_byte = 0; /* sum of radio payload bytes sent for upstream traffic */
static uint32_t        meas_up_dgram_sent   = 0; /* number of datagrams sent for upstream traffic */
static uint32_t        meas_up_ack_rcv      = 0; /* number of datagrams acknowledged for upstream traffic */

static pthread_mutex_t mx_meas_dw = PTHREAD_MUTEX_INITIALIZER; /* control access to the downstream measurements */
static uint32_t        meas_dw_pull_sent    = 0;               /* number of PULL requests sent for downstream traffic */
static uint32_t        meas_dw_ack_rcv      = 0; /* number of PULL requests acknowledged for downstream traffic */
static uint32_t        meas_dw_dgram_rcv    = 0; /* count PULL response packets received for downstream traffic */
static uint32_t        meas_dw_network_byte = 0; /* sum of UDP bytes sent for upstream traffic */
static uint32_t        meas_dw_payload_byte = 0; /* sum of radio payload bytes sent for upstream traffic */
static uint32_t        meas_nb_tx_ok        = 0; /* count packets emitted successfully */
static uint32_t        meas_nb_tx_fail      = 0; /* count packets were TX failed for other reasons */
static uint32_t        meas_nb_tx_requested = 0; /* count TX request from server (downlinks) */
static uint32_t        meas_nb_tx_rejected_collision_packet =
    0; /* count packets were TX request were rejected due to collision with another packet already programmed */
static uint32_t meas_nb_tx_rejected_collision_beacon =
    0; /* count packets were TX request were rejected due to collision with a beacon already programmed */
static uint32_t meas_nb_tx_rejected_too_late =
    0; /* count packets were TX request were rejected because it is too late to program it */
static uint32_t meas_nb_tx_rejected_too_early =
    0; /* count packets were TX request were rejected because timestamp is too much in advance */

static pthread_mutex_t mx_stat_rep  = PTHREAD_MUTEX_INITIALIZER; /* control access to the status report */
static bool            report_ready = false;       /* true when there is a new report to send to the server */
static char            status_report[STATUS_SIZE]; /* status report as a JSON object */

/* auto-quit function */
static uint32_t autoquit_threshold =
    10; /* enable auto-quit after a number of non-acknowledged PULL_DATA (0 = disabled)*/

/* Just In Time TX scheduling */
static struct jit_queue_s jit_queue[LGW_RF_CHAIN_NB];

/* Gateway specificities */
static int8_t antenna_gain = 0;

/* TX capabilities */
static bool tx_enable[LGW_RF_CHAIN_NB] = { false }; /* Is TX enabled for a given RF chain ? */

static temperature_sensor_handle_t temp_sensor = NULL;

static int lorahub_log_display(uint8_t level, const char *fmt, ...)
{
    int                         n;
    struct view_data_lorahub_log log = {0};

    va_list args;
    va_start(args, fmt);
    n = vsnprintf(log.data, sizeof(log.data), fmt, args);
    if (n > (int)sizeof(log.data)) {
        n = sizeof(log.data);
    }
    va_end(args);

    log.level = level;

    printf("%s", log.data);
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_LORAHUB_MONITOR, (void *)&log, sizeof(log), portMAX_DELAY);

    return n;
}

int wifi_get_mac_address( uint8_t mac_address[6] )
{
    esp_err_t err;

    err = esp_wifi_get_mac( WIFI_IF_STA, mac_address );
    if( err != ESP_OK )
    {
        ESP_LOGE( "pkt_wifi", "ERROR: esp_wifi_get_mac failed with %s\n", esp_err_to_name( err ) );
        return -1;
    }
    ESP_LOG_BUFFER_HEX_LEVEL( "pkt_wifi", mac_address, 6, ESP_LOG_INFO );

    return 0;
}

/* -------------------------------------------------------------------------- */
/* --- PRIVATE FUNCTIONS DECLARATION ---------------------------------------- */

static double difftimespec( struct timespec end, struct timespec beginning );

/* -------------------------------------------------------------------------- */
/* --- PRIVATE FUNCTIONS DEFINITION ----------------------------------------- */

static int parse_radio_configuration( void )
{
    int                    err_lgw;
    struct lgw_conf_rxrf_s rxrf_conf;
    struct lgw_conf_rxif_s rxif_conf;
    uint16_t               bw;

    memset( &rxrf_conf, 0, sizeof( struct lgw_conf_rxrf_s ) );
    memset( &rxif_conf, 0, sizeof( struct lgw_conf_rxif_s ) );

    /* Initialize configuration from menuconfig info */
    ESP_LOGI( TAG_PKT_FWD, "Get channel configuration from menuconfig" );
    rxrf_conf.freq_hz  = CONFIG_CHANNEL_FREQ_HZ;
    rxif_conf.datarate = CONFIG_CHANNEL_LORA_DATARATE;
    bw                 = CONFIG_CHANNEL_LORA_BANDWIDTH;

    /* Overwrite from NVS data if available */
#ifdef CONFIG_GET_CFG_FROM_FLASH
    esp_err_t err = ESP_OK;

    ESP_LOGI( TAG_PKT_FWD, "Get channel configuration from NVS" );

    /* Get configuration from NVS */
    printf( "Opening Non-Volatile Storage (NVS) handle for reading... " );
    nvs_handle_t my_handle;
    err = nvs_open( "storage", NVS_READONLY, &my_handle );
    if( err != ESP_OK )
    {
        printf( "Error (%s) opening NVS handle!\n", esp_err_to_name( err ) );
    }
    else
    {
        printf( "Done\n" );

        err = nvs_get_u32( my_handle, CFG_NVS_KEY_CHAN_FREQ, &( rxrf_conf.freq_hz ) );
        if( err == ESP_OK )
        {
            printf( "NVS -> %s = %" PRIu32 "hz\n", CFG_NVS_KEY_CHAN_FREQ, rxrf_conf.freq_hz );
            /* sanity check */
            if( ( rxrf_conf.freq_hz < 150000000 ) || ( rxrf_conf.freq_hz > 960000000 ) )
            {
                ESP_LOGE( TAG_PKT_FWD, "ERROR: wrong channel frequency configuration from NVS, set to %" PRIu32 "hz\n",
                          ( uint32_t ) CONFIG_CHANNEL_FREQ_HZ );
                rxrf_conf.freq_hz = CONFIG_CHANNEL_FREQ_HZ;
            }
        }
        else
        {
            printf( "Failed to get %s from NVS - %s\n", CFG_NVS_KEY_CHAN_FREQ, esp_err_to_name( err ) );
        }

        err = nvs_get_u32( my_handle, CFG_NVS_KEY_CHAN_DR, &( rxif_conf.datarate ) );
        if( err == ESP_OK )
        {
            printf( "NVS -> %s = %" PRIu32 "\n", CFG_NVS_KEY_CHAN_DR, rxif_conf.datarate );
            /* sanity check */
            if( ( rxif_conf.datarate < 7 ) || ( rxif_conf.datarate > 12 ) )
            {
                ESP_LOGE( TAG_PKT_FWD, "ERROR: wrong channel datarate configuration from NVS, set to %" PRIu32 "\n",
                          ( uint32_t ) CONFIG_CHANNEL_LORA_DATARATE );
                rxif_conf.datarate = CONFIG_CHANNEL_LORA_DATARATE;
            }
        }
        else
        {
            printf( "Failed to get %s from NVS - %s\n", CFG_NVS_KEY_CHAN_DR, esp_err_to_name( err ) );
        }

        err = nvs_get_u16( my_handle, CFG_NVS_KEY_CHAN_BW, &bw );
        if( err == ESP_OK )
        {
            printf( "NVS -> %s = %" PRIu16 "khz\n", CFG_NVS_KEY_CHAN_BW, bw );
            /* sanity check */
            if( ( bw != 125 ) && ( bw != 250 ) && ( bw != 500 ) )
            {
                ESP_LOGE( TAG_PKT_FWD, "ERROR: wrong channel bandwidth configuration from NVS, set to %" PRIu16 "khz\n",
                          ( uint16_t ) CONFIG_CHANNEL_LORA_BANDWIDTH );
                bw = CONFIG_CHANNEL_LORA_BANDWIDTH;
            }
        }
        else
        {
            printf( "Failed to get %s from NVS - %s\n", CFG_NVS_KEY_CHAN_BW, esp_err_to_name( err ) );
        }
    }
    nvs_close( my_handle );
    printf( "Closed NVS handle for reading.\n" );
#endif

    /* Update OLED display with channel config */
    // TODO?
    // display_channel_conf_t chan_cfg = { .freq_hz = rxrf_conf.freq_hz, .datarate = rxif_conf.datarate, .bw_khz = bw };
    // display_update_channel_config( &chan_cfg );

    /* Radio config */
    /* rxrf_conf.freq_hz DONE above*/
    rxrf_conf.rssi_offset = 0;
    rxrf_conf.tx_enable   = true;
    err_lgw               = lgw_rxrf_setconf( &rxrf_conf );
    if( err_lgw != LGW_HAL_SUCCESS )
    {
        ESP_LOGE( TAG_PKT_FWD, "ERROR: lgw_rxrf_setconf() failed\n" );
        return -1;
    }

    /* Save for later usage */
    tx_enable[0] = rxrf_conf.tx_enable;

    /* Modulation config */
    rxif_conf.modulation = MOD_LORA;
    /* rxif_conf.datarate DONE above */
    switch( bw )
    {
    case 125:
        rxif_conf.bandwidth = BW_125KHZ;
        break;
    case 250:
        rxif_conf.bandwidth = BW_250KHZ;
        break;
    case 500:
        rxif_conf.bandwidth = BW_500KHZ;
        break;
    default:
        ESP_LOGE( TAG_PKT_FWD, "ERROR: bandwidth configuration not supported %d\n", CONFIG_CHANNEL_LORA_BANDWIDTH );
        return -1;
    }
    rxif_conf.coderate = CR_LORA_4_5;
    err_lgw            = lgw_rxif_setconf( &rxif_conf );
    if( err_lgw != LGW_HAL_SUCCESS )
    {
        ESP_LOGE( TAG_PKT_FWD, "ERROR: lgw_rxif_setconf() failed\n" );
        return -1;
    }

    return 0;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static int parse_gateway_configuration( void )
{
    char gateway_eui_str[18] = "unknown";
    /* Gateway IF configuration (AUTO or CUSTOM) */
#ifdef CONFIG_GATEWAY_ID_AUTO
    /* format Gateway ID from MAC address for UDP protocol to LNS */
    wifi_get_mac_address( wifi_mac_addr );
    lgwm = ( ( uint64_t )( wifi_mac_addr[0] ) << 56 ) | ( ( uint64_t )( wifi_mac_addr[1] ) << 48 ) |
           ( ( uint64_t )( wifi_mac_addr[2] ) << 40 ) | ( ( uint64_t )( 0xFF ) << 32 );
    lgwm |= ( ( uint64_t )( 0xFE ) << 24 ) | ( ( uint64_t )( wifi_mac_addr[3] ) << 16 ) |
            ( ( uint64_t )( wifi_mac_addr[4] ) << 8 ) | ( ( uint64_t )( wifi_mac_addr[5] ) << 0 );
    ESP_LOGI( TAG_PKT_FWD, "Gateway ID is set to AUTO" );
    lorahub_log_display(LORAHUB_LOG_LEVEL_INFO, "Gateway ID is set to AUTO");
#else
    size_t len;
    if( ( len = strlen( CONFIG_GATEWAY_ID_CUSTOM ) ) != 16 )
    { /* 16 chars for 8 bytes hex string */
        ESP_LOGE( TAG_PKT_FWD, "Gateway ID length should be 8 bytes (current hexstring contains %d chars) - %s", len,
                  CONFIG_GATEWAY_ID_CUSTOM );
        return -1;
    }
    lgwm = ( uint64_t ) strtoull( CONFIG_GATEWAY_ID_CUSTOM, NULL, 16 );
    ESP_LOGI( TAG_PKT_FWD, "Gateway ID is set to CUSTOM (%s)", CONFIG_GATEWAY_ID_CUSTOM );
    lorahub_log_display(LORAHUB_LOG_LEVEL_INFO, "Gateway ID is set to CUSTOM (%s)", CONFIG_GATEWAY_ID_CUSTOM );
#endif
    ESP_LOGI( TAG_PKT_FWD, "Gateway ID: 0x%08llX", lgwm );
    lorahub_log_display(LORAHUB_LOG_LEVEL_INFO, "Gateway ID: 0x%08llX", lgwm );

    snprintf(gateway_eui_str, sizeof(gateway_eui_str), "%016" PRIx64, lgwm);
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_LORAHUB_EUI, gateway_eui_str, sizeof(gateway_eui_str), portMAX_DELAY);


    ESP_LOGI( TAG_PKT_FWD, "INFO: Auto-quit after %lu non-acknowledged PULL_DATA\n", autoquit_threshold );

    /* Configure LNS address and port from NVS */
#ifdef CONFIG_GET_CFG_FROM_FLASH
    esp_err_t err = ESP_OK;

    ESP_LOGI( TAG_PKT_FWD, "Get gateway configuration from NVS" );

    /* Get configuration from NVS */
    printf( "Opening Non-Volatile Storage (NVS) handle for reading... " );
    nvs_handle_t my_handle;
    err = nvs_open( "storage", NVS_READONLY, &my_handle );
    if( err != ESP_OK )
    {
        printf( "Error (%s) opening NVS handle!\n", esp_err_to_name( err ) );
    }
    else
    {
        printf( "Done\n" );

        size_t size = sizeof( serv_addr );
        err         = nvs_get_str( my_handle, CFG_NVS_KEY_LNS_ADDRESS, serv_addr, &size );
        if( err == ESP_OK )
        {
            printf( "NVS -> %s = %s\n", CFG_NVS_KEY_LNS_ADDRESS, serv_addr );
        }
        else
        {
            printf( "Failed to get %s from NVS - %s\n", CFG_NVS_KEY_LNS_ADDRESS, esp_err_to_name( err ) );
        }

        uint16_t lns_port;
        err = nvs_get_u16( my_handle, CFG_NVS_KEY_LNS_PORT, &lns_port );
        if( err == ESP_OK )
        {
            printf( "NVS -> %s = %" PRIu16 "\n", CFG_NVS_KEY_LNS_PORT, lns_port );
            snprintf( serv_port_up, sizeof serv_port_up, "%" PRIu16, lns_port );
            snprintf( serv_port_down, sizeof serv_port_down, "%" PRIu16, lns_port );
        }
        else
        {
            printf( "Failed to get %s from NVS - %s\n", CFG_NVS_KEY_LNS_PORT, esp_err_to_name( err ) );
        }
    }
    nvs_close( my_handle );
    printf( "Closed NVS handle for reading.\n" );
#endif

    return 0;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static double difftimespec( struct timespec end, struct timespec beginning )
{
    double x;

    x = 1E-9 * ( double ) ( end.tv_nsec - beginning.tv_nsec );
    x += ( double ) ( end.tv_sec - beginning.tv_sec );

    return x;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static uint8_t buff_tx_ack[ACK_BUFF_SIZE]; /* buffer to give feedback to server */

static int send_tx_ack( uint8_t token_h, uint8_t token_l, enum jit_error_e error, int32_t error_value )
{
    int buff_index;
    int j;

    /* reset buffer */
    memset( &buff_tx_ack, 0, sizeof buff_tx_ack );

    /* Prepare downlink feedback to be sent to server */
    buff_tx_ack[0]                     = PROTOCOL_VERSION;
    buff_tx_ack[1]                     = token_h;
    buff_tx_ack[2]                     = token_l;
    buff_tx_ack[3]                     = PKT_TX_ACK;
    *( uint32_t* ) ( buff_tx_ack + 4 ) = net_mac_h;
    *( uint32_t* ) ( buff_tx_ack + 8 ) = net_mac_l;
    buff_index                         = 12; /* 12-byte header */

    /* Put no JSON string if there is nothing to report */
    if( error != JIT_ERROR_OK )
    {
        /* start of JSON structure */
        memcpy( ( void* ) ( buff_tx_ack + buff_index ), ( void* ) "{\"txpk_ack\":{", 13 );
        buff_index += 13;
        /* set downlink error/warning status in JSON structure */
        switch( error )
        {
        case JIT_ERROR_TX_POWER:
            memcpy( ( void* ) ( buff_tx_ack + buff_index ), ( void* ) "\"warn\":", 7 );
            buff_index += 7;
            break;
        default:
            memcpy( ( void* ) ( buff_tx_ack + buff_index ), ( void* ) "\"error\":", 8 );
            buff_index += 8;
            break;
        }
        /* set error/warning type in JSON structure */
        switch( error )
        {
        case JIT_ERROR_FULL:
        case JIT_ERROR_COLLISION_PACKET:
            memcpy( ( void* ) ( buff_tx_ack + buff_index ), ( void* ) "\"COLLISION_PACKET\"", 18 );
            buff_index += 18;
            /* update stats */
            pthread_mutex_lock( &mx_meas_dw );
            meas_nb_tx_rejected_collision_packet += 1;
            pthread_mutex_unlock( &mx_meas_dw );
            break;
        case JIT_ERROR_TOO_LATE:
            memcpy( ( void* ) ( buff_tx_ack + buff_index ), ( void* ) "\"TOO_LATE\"", 10 );
            buff_index += 10;
            /* update stats */
            pthread_mutex_lock( &mx_meas_dw );
            meas_nb_tx_rejected_too_late += 1;
            pthread_mutex_unlock( &mx_meas_dw );
            break;
        case JIT_ERROR_TOO_EARLY:
            memcpy( ( void* ) ( buff_tx_ack + buff_index ), ( void* ) "\"TOO_EARLY\"", 11 );
            buff_index += 11;
            /* update stats */
            pthread_mutex_lock( &mx_meas_dw );
            meas_nb_tx_rejected_too_early += 1;
            pthread_mutex_unlock( &mx_meas_dw );
            break;
        case JIT_ERROR_COLLISION_BEACON:
            memcpy( ( void* ) ( buff_tx_ack + buff_index ), ( void* ) "\"COLLISION_BEACON\"", 18 );
            buff_index += 18;
            /* update stats */
            pthread_mutex_lock( &mx_meas_dw );
            meas_nb_tx_rejected_collision_beacon += 1;
            pthread_mutex_unlock( &mx_meas_dw );
            break;
        case JIT_ERROR_TX_FREQ:
            memcpy( ( void* ) ( buff_tx_ack + buff_index ), ( void* ) "\"TX_FREQ\"", 9 );
            buff_index += 9;
            break;
        case JIT_ERROR_TX_POWER:
            memcpy( ( void* ) ( buff_tx_ack + buff_index ), ( void* ) "\"TX_POWER\"", 10 );
            buff_index += 10;
            break;
        case JIT_ERROR_GPS_UNLOCKED:
            memcpy( ( void* ) ( buff_tx_ack + buff_index ), ( void* ) "\"GPS_UNLOCKED\"", 14 );
            buff_index += 14;
            break;
        default:
            memcpy( ( void* ) ( buff_tx_ack + buff_index ), ( void* ) "\"UNKNOWN\"", 9 );
            buff_index += 9;
            break;
        }
        /* set error/warning details in JSON structure */
        switch( error )
        {
        case JIT_ERROR_TX_POWER:
            j = snprintf( ( char* ) ( buff_tx_ack + buff_index ), ACK_BUFF_SIZE - buff_index, ",\"value\":%ld",
                          error_value );
            if( j > 0 )
            {
                buff_index += j;
            }
            else
            {
                ESP_LOGE( TAG_JIT, "ERROR: [up] snprintf failed line %u\n", ( __LINE__ - 4 ) );
                wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
            }
            break;
        default:
            /* Do nothing */
            break;
        }
        /* end of JSON structure */
        memcpy( ( void* ) ( buff_tx_ack + buff_index ), ( void* ) "}}", 2 );
        buff_index += 2;
    }

    buff_tx_ack[buff_index] = 0; /* add string terminator, for safety */

    /* send datagram to server */
    return send( sock_down, ( void* ) buff_tx_ack, buff_index, 0 );
}

/* -------------------------------------------------------------------------- */
/* --- THREAD 1: RECEIVING PACKETS AND FORWARDING THEM ---------------------- */

static uint8_t buff_up[TX_BUFF_SIZE]; /* buffer to compose the upstream packet */
static uint8_t buff_up_ack[32];       /* buffer to receive acknowledges */

void thread_up( void )
{
    int      i, j;         /* loop variables */
    unsigned pkt_in_dgram; /* nb on Lora packet in the current datagram */
    char     stat_timestamp[24];
    time_t   t;

    /* allocate memory for packet fetching and processing */
    struct lgw_pkt_rx_s  rxpkt[NB_PKT_MAX]; /* array containing inbound packets + metadata */
    struct lgw_pkt_rx_s* p;                 /* pointer on a RX packet */
    int                  nb_pkt;

    /* data buffers */
    int buff_index;

    /* protocol variables */
    uint8_t token_h; /* random token for acknowledgement matching */
    uint8_t token_l; /* random token for acknowledgement matching */

    /* ping measurement variables */
    struct timespec send_time;
    struct timespec recv_time;

    /* report management variable */
    bool send_report = false;

    /* mote info variables */
    uint32_t mote_addr = 0;
    uint16_t mote_fcnt = 0;

    /* set upstream socket RX timeout */
    i = setsockopt( sock_up, SOL_SOCKET, SO_RCVTIMEO, ( void* ) &push_timeout_half, sizeof push_timeout_half );
    if( i != 0 )
    {
        ESP_LOGE( TAG_UP, "ERROR: [up] setsockopt returned %s\n", strerror( errno ) );
        lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [up] setsockopt returned %s\n", strerror( errno ) );
        wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
    }

    /* pre-fill the data buffer with fixed fields */
    buff_up[0]                     = PROTOCOL_VERSION;
    buff_up[3]                     = PKT_PUSH_DATA;
    *( uint32_t* ) ( buff_up + 4 ) = net_mac_h;
    *( uint32_t* ) ( buff_up + 8 ) = net_mac_l;

    while( !exit_sig )
    {
        // ESP_LOGI(TAG_UP, "UP");

        /* fetch packets */
        pthread_mutex_lock( &mx_concent );
        nb_pkt = lgw_receive( NB_PKT_MAX, rxpkt );
        pthread_mutex_unlock( &mx_concent );
        if( nb_pkt == LGW_HAL_ERROR )
        {
            ESP_LOGE( TAG_UP, "ERROR: [up] failed packet fetch, exiting\n" );
            lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [up] failed packet fetch, exiting\n" );
            wait_on_error( LRHB_ERROR_HAL, __LINE__ );
        }

        /* check if there are status report to send */
        send_report = report_ready; /* copy the variable so it doesn't change mid-function */
        /* no mutex, we're only reading */

        /* wait a short time if no packets, nor status report */
        if( ( nb_pkt == 0 ) && ( send_report == false ) )
        {
            vTaskDelay( FETCH_SLEEP_MS / portTICK_PERIOD_MS );
            continue;
        }

        /* get timestamp for statistics */
        t = time( NULL );
        strftime( stat_timestamp, sizeof stat_timestamp, "%F %T %Z", gmtime( &t ) );

        /* start composing datagram with the header */
        token_h    = ( uint8_t ) rand( ); /* random token */
        token_l    = ( uint8_t ) rand( ); /* random token */
        buff_up[1] = token_h;
        buff_up[2] = token_l;
        buff_index = 12; /* 12-byte header */

        /* start of JSON structure */
        memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) "{\"rxpk\":[", 9 );
        buff_index += 9;

        /* serialize Lora packets metadata and payload */
        pkt_in_dgram = 0;
        for( i = 0; i < nb_pkt; ++i )
        {
            p = &rxpkt[i];

            /* Get mote information from current packet (addr, fcnt) */
            /* FHDR - DevAddr */
            if( p->size >= 8 )
            {
                mote_addr = p->payload[1];
                mote_addr |= p->payload[2] << 8;
                mote_addr |= p->payload[3] << 16;
                mote_addr |= p->payload[4] << 24;
                /* FHDR - FCnt */
                mote_fcnt = p->payload[6];
                mote_fcnt |= p->payload[7] << 8;
            }
            else
            {
                mote_addr = 0;
                mote_fcnt = 0;
            }

            /* basic packet filtering */
            pthread_mutex_lock( &mx_meas_up );
            meas_nb_rx_rcv += 1;
            switch( p->status )
            {
            case STAT_CRC_OK:
                meas_nb_rx_ok += 1;
                if( !fwd_valid_pkt )
                {
                    pthread_mutex_unlock( &mx_meas_up );
                    continue; /* skip that packet */
                }
                break;
            case STAT_CRC_BAD:
                meas_nb_rx_bad += 1;
                if( !fwd_error_pkt )
                {
                    pthread_mutex_unlock( &mx_meas_up );
                    continue; /* skip that packet */
                }
                break;
            case STAT_NO_CRC:
                meas_nb_rx_nocrc += 1;
                if( !fwd_nocrc_pkt )
                {
                    pthread_mutex_unlock( &mx_meas_up );
                    continue; /* skip that packet */
                }
                break;
            default:
                ESP_LOGW( TAG_UP,
                          "WARNING: [up] received packet with unknown status %u (size %u, modulation %u, BW %u, DR "
                          "%lu, RSSI %.1f)\n",
                          p->status, p->size, p->modulation, p->bandwidth, p->datarate, p->rssic );
                pthread_mutex_unlock( &mx_meas_up );
                continue; /* skip that packet */
            }
            meas_up_pkt_fwd += 1;
            meas_up_payload_byte += p->size;
            pthread_mutex_unlock( &mx_meas_up );
            printf( "\nINFO: Received pkt from mote: %08lX (fcnt=%u)", mote_addr, mote_fcnt );
            lorahub_log_display(LORAHUB_LOG_LEVEL_WARN, "\nINFO: Received pkt from mote: %08lX (fcnt=%u)", mote_addr, mote_fcnt );

            /* Start of packet, add inter-packet separator if necessary */
            if( pkt_in_dgram == 0 )
            {
                buff_up[buff_index] = '{';
                ++buff_index;
            }
            else
            {
                buff_up[buff_index]     = ',';
                buff_up[buff_index + 1] = '{';
                buff_index += 2;
            }

            /* JSON rxpk frame format version, 8 useful chars */
            j = snprintf( ( char* ) ( buff_up + buff_index ), TX_BUFF_SIZE - buff_index, "\"jver\":%d",
                          PROTOCOL_JSON_RXPK_FRAME_FORMAT );
            if( j > 0 )
            {
                buff_index += j;
            }
            else
            {
                ESP_LOGE( TAG_UP, "ERROR: [up] snprintf failed line %u\n", ( __LINE__ - 4 ) );
                lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [up] snprintf failed line %u\n", ( __LINE__ - 4 ) );
                wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
            }

            /* RAW timestamp, 8-17 useful chars */
            j = snprintf( ( char* ) ( buff_up + buff_index ), TX_BUFF_SIZE - buff_index, ",\"tmst\":%lu", p->count_us );
            if( j > 0 )
            {
                buff_index += j;
            }
            else
            {
                ESP_LOGE( TAG_UP, "ERROR: [up] snprintf failed line %u\n", ( __LINE__ - 4 ) );
                lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [up] snprintf failed line %u\n", ( __LINE__ - 4 ) );
                wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
            }

            /* Packet concentrator channel, RF chain & RX frequency, 34-36 useful chars */
            j = snprintf( ( char* ) ( buff_up + buff_index ), TX_BUFF_SIZE - buff_index,
                          ",\"chan\":%1u,\"rfch\":%1u,\"freq\":%.6lf", p->if_chain, p->rf_chain,
                          ( ( double ) p->freq_hz / 1e6 ) );
            if( j > 0 )
            {
                buff_index += j;
            }
            else
            {
                ESP_LOGE( TAG_UP, "ERROR: [up] snprintf failed line %u\n", ( __LINE__ - 4 ) );
                lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [up] snprintf failed line %u\n", ( __LINE__ - 4 ) );
                wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
            }

            /* Packet status, 9-10 useful chars */
            switch( p->status )
            {
            case STAT_CRC_OK:
                memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"stat\":1", 9 );
                buff_index += 9;
                break;
            case STAT_CRC_BAD:
                memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"stat\":-1", 10 );
                buff_index += 10;
                break;
            case STAT_NO_CRC:
                memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"stat\":0", 9 );
                buff_index += 9;
                break;
            default:
                ESP_LOGE( TAG_UP, "ERROR: [up] received packet with unknown status 0x%02X\n", p->status );
                memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"stat\":?", 9 );
                buff_index += 9;
                lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [up] received packet with unknown status 0x%02X\n", p->status );
                wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
            }

            /* Packet modulation, 13-14 useful chars */
            if( p->modulation == MOD_LORA )
            {
                memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"modu\":\"LORA\"", 14 );
                buff_index += 14;

                /* Lora datarate & bandwidth, 16-19 useful chars */
                switch( p->datarate )
                {
                case DR_LORA_SF5:
                    memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"datr\":\"SF5", 12 );
                    buff_index += 12;
                    break;
                case DR_LORA_SF6:
                    memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"datr\":\"SF6", 12 );
                    buff_index += 12;
                    break;
                case DR_LORA_SF7:
                    memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"datr\":\"SF7", 12 );
                    buff_index += 12;
                    break;
                case DR_LORA_SF8:
                    memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"datr\":\"SF8", 12 );
                    buff_index += 12;
                    break;
                case DR_LORA_SF9:
                    memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"datr\":\"SF9", 12 );
                    buff_index += 12;
                    break;
                case DR_LORA_SF10:
                    memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"datr\":\"SF10", 13 );
                    buff_index += 13;
                    break;
                case DR_LORA_SF11:
                    memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"datr\":\"SF11", 13 );
                    buff_index += 13;
                    break;
                case DR_LORA_SF12:
                    memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"datr\":\"SF12", 13 );
                    buff_index += 13;
                    break;
                default:
                    ESP_LOGE( TAG_UP, "ERROR: [up] lora packet with unknown datarate 0x%02lX\n", p->datarate );
                    memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"datr\":\"SF?", 12 );
                    buff_index += 12;
                    lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [up] lora packet with unknown datarate 0x%02lX\n", p->datarate );
                    wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
                }
                switch( p->bandwidth )
                {
                case BW_125KHZ:
                    memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) "BW125\"", 6 );
                    buff_index += 6;
                    break;
                case BW_250KHZ:
                    memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) "BW250\"", 6 );
                    buff_index += 6;
                    break;
                case BW_500KHZ:
                    memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) "BW500\"", 6 );
                    buff_index += 6;
                    break;
                default:
                    ESP_LOGE( TAG_UP, "ERROR: [up] lora packet with unknown bandwidth 0x%02X\n", p->bandwidth );
                    memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) "BW?\"", 4 );
                    buff_index += 4;
                    lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [up] lora packet with unknown bandwidth 0x%02X\n", p->bandwidth );
                    wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
                }

                /* Packet ECC coding rate, 11-13 useful chars */
                switch( p->coderate )
                {
                case CR_LORA_4_5:
                    memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"codr\":\"4/5\"", 13 );
                    buff_index += 13;
                    break;
                case CR_LORA_4_6:
                    memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"codr\":\"4/6\"", 13 );
                    buff_index += 13;
                    break;
                case CR_LORA_4_7:
                    memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"codr\":\"4/7\"", 13 );
                    buff_index += 13;
                    break;
                case CR_LORA_4_8:
                    memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"codr\":\"4/8\"", 13 );
                    buff_index += 13;
                    break;
                case 0: /* treat the CR0 case (mostly false sync) */
                    memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"codr\":\"OFF\"", 13 );
                    buff_index += 13;
                    break;
                default:
                    ESP_LOGE( TAG_UP, "ERROR: [up] lora packet with unknown coderate 0x%02X\n", p->coderate );
                    memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"codr\":\"?\"", 11 );
                    buff_index += 11;
                    lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [up] lora packet with unknown coderate 0x%02X\n", p->coderate );
                    wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
                }

                /* Lora SNR */
                j = snprintf( ( char* ) ( buff_up + buff_index ), TX_BUFF_SIZE - buff_index, ",\"lsnr\":%.1f", p->snr );
                if( j > 0 )
                {
                    buff_index += j;
                }
                else
                {
                    ESP_LOGE( TAG_UP, "ERROR: [up] snprintf failed line %u\n", ( __LINE__ - 4 ) );
                    wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
                }
            }
            else
            {
                ESP_LOGE( TAG_UP, "ERROR: [up] received packet with unknown modulation 0x%02X\n", p->modulation );
                lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [up] received packet with unknown modulation 0x%02X\n", p->modulation );
                wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
            }

            /* Channel RSSI, payload size, 18-23 useful chars */
            j = snprintf( ( char* ) ( buff_up + buff_index ), TX_BUFF_SIZE - buff_index, ",\"rssi\":%.0f,\"size\":%u",
                          roundf( p->rssic ), p->size );
            if( j > 0 )
            {
                buff_index += j;
            }
            else
            {
                ESP_LOGE( TAG_UP, "ERROR: [up] snprintf failed line %u\n", ( __LINE__ - 4 ) );
                lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [up] snprintf failed line %u\n", ( __LINE__ - 4 ) );
                wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
            }

            /* Packet base64-encoded payload, 14-350 useful chars */
            memcpy( ( void* ) ( buff_up + buff_index ), ( void* ) ",\"data\":\"", 9 );
            buff_index += 9;
            j = bin_to_b64( p->payload, p->size, ( char* ) ( buff_up + buff_index ),
                            341 ); /* 255 bytes = 340 chars in b64 + null char */
            if( j >= 0 )
            {
                buff_index += j;
            }
            else
            {
                ESP_LOGE( TAG_UP, "ERROR: [up] bin_to_b64 failed line %u\n", ( __LINE__ - 4 ) );
                lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [up] bin_to_b64 failed line %u\n", ( __LINE__ - 4 ) );
                wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
            }
            buff_up[buff_index] = '"';
            ++buff_index;

            /* End of packet serialization */
            buff_up[buff_index] = '}';
            ++buff_index;
            ++pkt_in_dgram;
        }

        /* restart fetch sequence without sending empty JSON if all packets have been filtered out */
        if( pkt_in_dgram == 0 )
        {
            if( send_report == true )
            {
                /* need to clean up the beginning of the payload */
                buff_index -= 8; /* removes "rxpk":[ */
            }
            else
            {
                /* all packet have been filtered out and no report, restart loop */
                continue;
            }
        }
        else
        {
            /* end of packet array */
            buff_up[buff_index] = ']';
            ++buff_index;
            /* add separator if needed */
            if( send_report == true )
            {
                buff_up[buff_index] = ',';
                ++buff_index;
            }
        }

        /* add status report if a new one is available */
        if( send_report == true )
        {
            pthread_mutex_lock( &mx_stat_rep );
            report_ready = false;
            j = snprintf( ( char* ) ( buff_up + buff_index ), TX_BUFF_SIZE - buff_index, "%s", status_report );
            pthread_mutex_unlock( &mx_stat_rep );
            if( j > 0 )
            {
                buff_index += j;
            }
            else
            {
                ESP_LOGE( TAG_UP, "ERROR: [up] snprintf failed line %u\n", ( __LINE__ - 5 ) );
                wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
            }
        }

        /* end of JSON datagram payload */
        buff_up[buff_index] = '}';
        ++buff_index;
        buff_up[buff_index] = 0; /* add string terminator, for safety */

        printf( "\nJSON up: %s\n", ( char* ) ( buff_up + 12 ) ); /* DEBUG: display JSON payload */
        lorahub_log_display(LORAHUB_LOG_LEVEL_INFO, "\nJSON up: %s\n", ( char* ) ( buff_up + 12 ) );

        /* send datagram to server */
        j = send( sock_up, ( void* ) buff_up, buff_index, 0 );
        if( j < 0 )
        {
            ESP_LOGE( TAG_UP, "ERROR: [up] failed to send datagram to server - %s\n", strerror( errno ) );
        }
        clock_gettime( CLOCK_MONOTONIC, &send_time );
        pthread_mutex_lock( &mx_meas_up );
        meas_up_dgram_sent += 1;
        meas_up_network_byte += buff_index;

        /* wait for acknowledge (in 2 times, to catch extra packets) */
        for( i = 0; i < 2; ++i )
        {
            j = recv( sock_up, ( void* ) buff_up_ack, sizeof buff_up_ack, 0 );
            clock_gettime( CLOCK_MONOTONIC, &recv_time );
            if( j == -1 )
            {
                if( errno == EAGAIN )
                { /* timeout */
                    continue;
                }
                else
                { /* server connection error */
                    break;
                }
            }
            else if( ( j < 4 ) || ( buff_up_ack[0] != PROTOCOL_VERSION ) || ( buff_up_ack[3] != PKT_PUSH_ACK ) )
            {
                ESP_LOGW( TAG_UP, "WARNING: [up] ignored invalid non-ACL packet\n" );
                continue;
            }
            else if( ( buff_up_ack[1] != token_h ) || ( buff_up_ack[2] != token_l ) )
            {
                ESP_LOGW( TAG_UP, "WARNING: [up] ignored out-of sync ACK packet\n" );
                continue;
            }
            else
            {
                ESP_LOGI( TAG_UP, "INFO: [up] PUSH_ACK received in %i ms",
                          ( int ) ( 1000 * difftimespec( recv_time, send_time ) ) );
                meas_up_ack_rcv += 1;
                break;
            }
        }
        pthread_mutex_unlock( &mx_meas_up );

        /* Update display */
        // TODO?
        if( nb_pkt > 0 )
        {
            // display_stats_t rx_tx_stats = { .nb_rx = nb_pkt, .nb_tx = 0 };
            // display_update_statistics( &rx_tx_stats );

            // display_last_rx_packet_t last_rx_pkt = { .devaddr = mote_addr,
            //                                          .rssi    = rxpkt[0].rssic,
            //                                          .snr     = rxpkt[0].snr };
            // display_update_last_rx_packet( &last_rx_pkt );
        }
    }
    ESP_LOGI( TAG_UP, "\nINFO: End of upstream thread\n" );
}

/* -------------------------------------------------------------------------- */
/* --- THREAD 2: POLLING SERVER AND ENQUEUING PACKETS IN JIT QUEUE ---------- */

static uint8_t buff_down[1000]; /* buffer to receive downstream packets */
static uint8_t buff_req[12];    /* buffer to compose pull requests */

void thread_down( void )
{
    int i; /* loop variables */

    /* configuration and metadata for an outbound packet */
    struct lgw_pkt_tx_s txpkt;
    bool                sent_immediate = false; /* option to sent the packet immediately */

    /* data buffers */
    int msg_len;

    /* local timekeeping variables */
    struct timespec send_time; /* time of the pull request */
    struct timespec recv_time; /* time of return from recv socket call */

    /* protocol variables */
    uint8_t token_h;         /* random token for acknowledgement matching */
    uint8_t token_l;         /* random token for acknowledgement matching */
    bool    req_ack = false; /* keep track of whether PULL_DATA was acknowledged or not */

    /* JSON parsing variables */
    JSON_Value*  root_val = NULL;
    JSON_Object* txpk_obj = NULL;
    JSON_Value*  val      = NULL; /* needed to detect the absence of some fields */
    const char*  str;             /* pointer to sub-strings in the JSON data */
    short        x0, x1;

    /* auto-quit variable */
    uint32_t autoquit_cnt = 0; /* count the number of PULL_DATA sent since the latest PULL_ACK */

    /* Just In Time downlink */
    uint32_t            current_concentrator_time;
    enum jit_error_e    jit_result = JIT_ERROR_OK;
    enum jit_pkt_type_e downlink_type;
    enum jit_error_e    warning_result = JIT_ERROR_OK;
    int32_t             warning_value  = 0;

    /* set downstream socket RX timeout */
    i = setsockopt( sock_down, SOL_SOCKET, SO_RCVTIMEO, ( void* ) &pull_timeout, sizeof pull_timeout );
    if( i != 0 )
    {
        ESP_LOGE( TAG_DOWN, "ERROR: [down] setsockopt returned %s\n", strerror( errno ) );
        lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [down] setsockopt returned %s\n", strerror( errno ) );
        wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
    }

    /* pre-fill the pull request buffer with fixed fields */
    buff_req[0]                     = PROTOCOL_VERSION;
    buff_req[3]                     = PKT_PULL_DATA;
    *( uint32_t* ) ( buff_req + 4 ) = net_mac_h;
    *( uint32_t* ) ( buff_req + 8 ) = net_mac_l;

    /* JIT queue initialization */
    for( i = 0; i < LGW_RF_CHAIN_NB; i++ )
    {
        jit_queue_init( &jit_queue[i] );
    }

    while( !exit_sig )
    {
        // ESP_LOGI(TAG_DOWN, "DOWN");

        /* auto-quit if the threshold is crossed */
        if( ( autoquit_threshold > 0 ) && ( autoquit_cnt >= autoquit_threshold ) )
        {
            exit_sig = true;
            ESP_LOGW( TAG_DOWN, "WARNING: [down] the last %lu PULL_DATA were not ACKed, exiting application\n",
                      autoquit_threshold );
            break;
        }

        /* generate random token for request */
        token_h     = ( uint8_t ) rand( ); /* random token */
        token_l     = ( uint8_t ) rand( ); /* random token */
        buff_req[1] = token_h;
        buff_req[2] = token_l;

        /* send PULL_DATA request and record time */
        i = send( sock_down, ( void* ) buff_req, sizeof buff_req, 0 );
        if( i < 0 )
        {
            ESP_LOGE( TAG_DOWN, "ERROR: [down] failed to send PULL_DATA to server - %s\n", strerror( errno ) );
            lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [down] failed to send PULL_DATA to server - %s\n", strerror( errno ));
        }
        clock_gettime( CLOCK_MONOTONIC, &send_time );
        pthread_mutex_lock( &mx_meas_dw );
        meas_dw_pull_sent += 1;
        pthread_mutex_unlock( &mx_meas_dw );
        req_ack = false;
        autoquit_cnt++;

        /* listen to packets and process them until a new PULL request must be sent */
        recv_time = send_time;
        while( ( int ) difftimespec( recv_time, send_time ) < keepalive_time )
        {
            /* try to receive a datagram */
            msg_len = recv( sock_down, ( void* ) buff_down, ( sizeof buff_down ) - 1, 0 );
            clock_gettime( CLOCK_MONOTONIC, &recv_time );

            /* if no network message was received, got back to listening sock_down socket */
            if( msg_len == -1 )
            {
                // ESP_LOGW(TAG_DOWN, "WARNING: [down] recv returned %s\n", strerror(errno)); /* too verbose */
                continue;
            }

            /* if the datagram does not respect protocol, just ignore it */
            if( ( msg_len < 4 ) || ( buff_down[0] != PROTOCOL_VERSION ) ||
                ( ( buff_down[3] != PKT_PULL_RESP ) && ( buff_down[3] != PKT_PULL_ACK ) ) )
            {
                ESP_LOGW( TAG_DOWN, "WARNING: [down] ignoring invalid packet len=%d, protocol_version=%d, id=%d\n",
                          msg_len, buff_down[0], buff_down[3] );
                continue;
            }

            /* if the datagram is an ACK, check token */
            if( buff_down[3] == PKT_PULL_ACK )
            {
                if( ( buff_down[1] == token_h ) && ( buff_down[2] == token_l ) )
                {
                    if( req_ack )
                    {
                        ESP_LOGI( TAG_DOWN, "INFO: [down] duplicate ACK received :)\n" );
                    }
                    else
                    { /* if that packet was not already acknowledged */
                        req_ack      = true;
                        autoquit_cnt = 0;
                        pthread_mutex_lock( &mx_meas_dw );
                        meas_dw_ack_rcv += 1;
                        pthread_mutex_unlock( &mx_meas_dw );
                        ESP_LOGI( TAG_DOWN, "INFO: [down] PULL_ACK received in %i ms",
                                  ( int ) ( 1000 * difftimespec( recv_time, send_time ) ) );
                    }
                }
                else
                { /* out-of-sync token */
                    ESP_LOGI( TAG_DOWN, "INFO: [down] received out-of-sync ACK\n" );
                }
                continue;
            }

            /* the datagram is a PULL_RESP */
            buff_down[msg_len] = 0; /* add string terminator, just to be safe */
            ESP_LOGI( TAG_DOWN, "INFO: [down] PULL_RESP received  - token[%d:%d] :)", buff_down[1],
                      buff_down[2] );                                   /* very verbose */
            printf( "\nJSON down: %s\n", ( char* ) ( buff_down + 4 ) ); /* DEBUG: display JSON payload */
            lorahub_log_display(LORAHUB_LOG_LEVEL_INFO, "\nJSON down: %s\n", ( char* ) ( buff_down + 4 ) );

            /* initialize TX struct and try to parse JSON */
            memset( &txpkt, 0, sizeof txpkt );
            root_val = json_parse_string_with_comments( ( const char* ) ( buff_down + 4 ) ); /* JSON offset */
            if( root_val == NULL )
            {
                ESP_LOGW( TAG_DOWN, "WARNING: [down] invalid JSON, TX aborted\n" );
                continue;
            }

            /* look for JSON sub-object 'txpk' */
            txpk_obj = json_object_get_object( json_value_get_object( root_val ), "txpk" );
            if( txpk_obj == NULL )
            {
                ESP_LOGW( TAG_DOWN, "WARNING: [down] no \"txpk\" object in JSON, TX aborted\n" );
                json_value_free( root_val );
                continue;
            }

            /* Parse "immediate" tag, or target timestamp, or UTC time to be converted by GPS (mandatory) */
            i = json_object_get_boolean( txpk_obj,
                                         "imme" ); /* can be 1 if true, 0 if false, or -1 if not a JSON boolean */
            if( i == 1 )
            {
                /* TX procedure: send immediately */
                sent_immediate = true;
                downlink_type  = JIT_PKT_TYPE_DOWNLINK_CLASS_C;
                ESP_LOGI( TAG_DOWN, "INFO: [down] a packet will be sent in \"immediate\" mode\n" );
            }
            else
            {
                sent_immediate = false;
                val            = json_object_get_value( txpk_obj, "tmst" );
                if( val != NULL )
                {
                    /* TX procedure: send on timestamp value */
                    txpkt.count_us = ( uint32_t ) json_value_get_number( val );

                    /* Concentrator timestamp is given, we consider it is a Class A downlink */
                    downlink_type = JIT_PKT_TYPE_DOWNLINK_CLASS_A;
                }
                else
                {
                    ESP_LOGW( TAG_DOWN, "WARNING: [down] no mandatory \"txpk.tmst\" objects in JSON, TX aborted\n" );
                    json_value_free( root_val );
                    continue;
                }
            }

            /* Parse "No CRC" flag (optional field) */
            val = json_object_get_value( txpk_obj, "ncrc" );
            if( val != NULL )
            {
                txpkt.no_crc = ( bool ) json_value_get_boolean( val );
            }

            /* Parse "No header" flag (optional field) */
            val = json_object_get_value( txpk_obj, "nhdr" );
            if( val != NULL )
            {
                txpkt.no_header = ( bool ) json_value_get_boolean( val );
            }

            /* parse target frequency (mandatory) */
            val = json_object_get_value( txpk_obj, "freq" );
            if( val == NULL )
            {
                ESP_LOGW( TAG_DOWN, "WARNING: [down] no mandatory \"txpk.freq\" object in JSON, TX aborted\n" );
                json_value_free( root_val );
                continue;
            }
            txpkt.freq_hz = ( uint32_t )( ( double ) ( 1.0e6 ) * json_value_get_number( val ) );

            /* parse RF chain used for TX (mandatory) */
            val = json_object_get_value( txpk_obj, "rfch" );
            if( val == NULL )
            {
                ESP_LOGW( TAG_DOWN, "WARNING: [down] no mandatory \"txpk.rfch\" object in JSON, TX aborted\n" );
                json_value_free( root_val );
                continue;
            }
            txpkt.rf_chain = ( uint8_t ) json_value_get_number( val );
            if( tx_enable[txpkt.rf_chain] == false )
            {
                ESP_LOGW( TAG_DOWN, "WARNING: [down] TX is not enabled on RF chain %u, TX aborted\n", txpkt.rf_chain );
                json_value_free( root_val );
                continue;
            }

            /* parse TX power (optional field) */
            val = json_object_get_value( txpk_obj, "powe" );
            if( val != NULL )
            {
                txpkt.rf_power = ( int8_t ) json_value_get_number( val ) - antenna_gain;
            }

            /* Parse modulation (mandatory) */
            str = json_object_get_string( txpk_obj, "modu" );
            if( str == NULL )
            {
                ESP_LOGW( TAG_DOWN, "WARNING: [down] no mandatory \"txpk.modu\" object in JSON, TX aborted\n" );
                json_value_free( root_val );
                continue;
            }
            if( strcmp( str, "LORA" ) == 0 )
            {
                /* Lora modulation */
                txpkt.modulation = MOD_LORA;

                /* Parse Lora spreading-factor and modulation bandwidth (mandatory) */
                str = json_object_get_string( txpk_obj, "datr" );
                if( str == NULL )
                {
                    ESP_LOGW( TAG_DOWN, "WARNING: [down] no mandatory \"txpk.datr\" object in JSON, TX aborted\n" );
                    json_value_free( root_val );
                    continue;
                }
                i = sscanf( str, "SF%2hdBW%3hd", &x0, &x1 );
                if( i != 2 )
                {
                    ESP_LOGW( TAG_DOWN, "WARNING: [down] format error in \"txpk.datr\", TX aborted\n" );
                    json_value_free( root_val );
                    continue;
                }
                switch( x0 )
                {
                case 5:
                    txpkt.datarate = DR_LORA_SF5;
                    break;
                case 6:
                    txpkt.datarate = DR_LORA_SF6;
                    break;
                case 7:
                    txpkt.datarate = DR_LORA_SF7;
                    break;
                case 8:
                    txpkt.datarate = DR_LORA_SF8;
                    break;
                case 9:
                    txpkt.datarate = DR_LORA_SF9;
                    break;
                case 10:
                    txpkt.datarate = DR_LORA_SF10;
                    break;
                case 11:
                    txpkt.datarate = DR_LORA_SF11;
                    break;
                case 12:
                    txpkt.datarate = DR_LORA_SF12;
                    break;
                default:
                    ESP_LOGW( TAG_DOWN, "WARNING: [down] format error in \"txpk.datr\", invalid SF, TX aborted\n" );
                    json_value_free( root_val );
                    continue;
                }
                switch( x1 )
                {
                case 125:
                    txpkt.bandwidth = BW_125KHZ;
                    break;
                case 250:
                    txpkt.bandwidth = BW_250KHZ;
                    break;
                case 500:
                    txpkt.bandwidth = BW_500KHZ;
                    break;
                default:
                    ESP_LOGW( TAG_DOWN, "WARNING: [down] format error in \"txpk.datr\", invalid BW, TX aborted\n" );
                    json_value_free( root_val );
                    continue;
                }

                /* Parse ECC coding rate (optional field) */
                str = json_object_get_string( txpk_obj, "codr" );
                if( str == NULL )
                {
                    ESP_LOGW( TAG_DOWN, "WARNING: [down] no mandatory \"txpk.codr\" object in json, TX aborted\n" );
                    json_value_free( root_val );
                    continue;
                }
                if( strcmp( str, "4/5" ) == 0 )
                    txpkt.coderate = CR_LORA_4_5;
                else if( strcmp( str, "4/6" ) == 0 )
                    txpkt.coderate = CR_LORA_4_6;
                else if( strcmp( str, "2/3" ) == 0 )
                    txpkt.coderate = CR_LORA_4_6;
                else if( strcmp( str, "4/7" ) == 0 )
                    txpkt.coderate = CR_LORA_4_7;
                else if( strcmp( str, "4/8" ) == 0 )
                    txpkt.coderate = CR_LORA_4_8;
                else if( strcmp( str, "1/2" ) == 0 )
                    txpkt.coderate = CR_LORA_4_8;
                else
                {
                    ESP_LOGW( TAG_DOWN, "WARNING: [down] format error in \"txpk.codr\", TX aborted\n" );
                    json_value_free( root_val );
                    continue;
                }

                /* Parse signal polarity switch (optional field) */
                val = json_object_get_value( txpk_obj, "ipol" );
                if( val != NULL )
                {
                    txpkt.invert_pol = ( bool ) json_value_get_boolean( val );
                }

                /* parse Lora preamble length (optional field, optimum min value enforced) */
                val = json_object_get_value( txpk_obj, "prea" );
                if( val != NULL )
                {
                    i = ( int ) json_value_get_number( val );
                    if( i >= MIN_LORA_PREAMBLE )
                    {
                        txpkt.preamble = ( uint16_t ) i;
                    }
                    else
                    {
                        txpkt.preamble = ( uint16_t ) MIN_LORA_PREAMBLE;
                    }
                }
                else
                {
                    txpkt.preamble = ( uint16_t ) STD_LORA_PREAMBLE;
                }
            }
            else
            {
                ESP_LOGW( TAG_DOWN, "WARNING: [down] invalid modulation in \"txpk.modu\", TX aborted\n" );
                json_value_free( root_val );
                continue;
            }

            /* Parse payload length (mandatory) */
            val = json_object_get_value( txpk_obj, "size" );
            if( val == NULL )
            {
                ESP_LOGW( TAG_DOWN, "WARNING: [down] no mandatory \"txpk.size\" object in JSON, TX aborted\n" );
                json_value_free( root_val );
                continue;
            }
            txpkt.size = ( uint16_t ) json_value_get_number( val );

            /* Parse payload data (mandatory) */
            str = json_object_get_string( txpk_obj, "data" );
            if( str == NULL )
            {
                ESP_LOGW( TAG_DOWN, "WARNING: [down] no mandatory \"txpk.data\" object in JSON, TX aborted\n" );
                json_value_free( root_val );
                continue;
            }
            i = b64_to_bin( str, strlen( str ), txpkt.payload, sizeof txpkt.payload );
            if( i != txpkt.size )
            {
                ESP_LOGW( TAG_DOWN,
                          "WARNING: [down] mismatch between .size and .data size once converter to binary\n" );
            }

            /* free the JSON parse tree from memory */
            json_value_free( root_val );

            /* select TX mode */
            if( sent_immediate )
            {
                txpkt.tx_mode = IMMEDIATE;
            }
            else
            {
                txpkt.tx_mode = TIMESTAMPED;
            }

            /* record measurement data */
            pthread_mutex_lock( &mx_meas_dw );
            meas_dw_dgram_rcv += 1;          /* count only datagrams with no JSON errors */
            meas_dw_network_byte += msg_len; /* meas_dw_network_byte */
            meas_dw_payload_byte += txpkt.size;
            pthread_mutex_unlock( &mx_meas_dw );

            /* reset error/warning results */
            jit_result = warning_result = JIT_ERROR_OK;
            warning_value               = 0;

            /* check TX frequency before trying to queue packet */
            uint32_t tx_freq_hz_min, tx_freq_hz_max;
            lgw_get_min_max_freq_hz( &tx_freq_hz_min, &tx_freq_hz_max );
            if( ( txpkt.freq_hz < tx_freq_hz_min ) || ( txpkt.freq_hz > tx_freq_hz_max ) )
            {
                jit_result = JIT_ERROR_TX_FREQ;
                ESP_LOGE( TAG_DOWN, "ERROR: Packet REJECTED, unsupported frequency - %lu (min:%lu,max:%lu)\n",
                          txpkt.freq_hz, tx_freq_hz_min, tx_freq_hz_max );
            }

            /* check TX power before trying to queue packet, send a warning if not supported */
            if( jit_result == JIT_ERROR_OK )
            {
                int8_t tx_power_min, tx_power_max;
                lgw_get_min_max_power_dbm( &tx_power_min, &tx_power_max );
                if( txpkt.rf_power < tx_power_min )
                {
                    /* this RF power is not supported, throw a warning, and use the closest lower power supported */
                    warning_result = JIT_ERROR_TX_POWER;
                    warning_value  = ( int32_t ) tx_power_min;
                    ESP_LOGW( TAG_DOWN,
                              "WARNING: Requested TX power is not supported (%ddBm), actual power used: %lddBm\n",
                              txpkt.rf_power, warning_value );
                    txpkt.rf_power = tx_power_min;
                }
                if( txpkt.rf_power > tx_power_max )
                {
                    /* this RF power is not supported, throw a warning, and use the closest lower power supported */
                    warning_result = JIT_ERROR_TX_POWER;
                    warning_value  = ( int32_t ) tx_power_max;
                    ESP_LOGW( TAG_DOWN,
                              "WARNING: Requested TX power is not supported (%ddBm), actual power used: %lddBm\n",
                              txpkt.rf_power, warning_value );
                    txpkt.rf_power = tx_power_max;
                }
            }

            /* insert packet to be sent into JIT queue */
            if( jit_result == JIT_ERROR_OK )
            {
                lgw_get_instcnt( &current_concentrator_time );
                jit_result =
                    jit_enqueue( &jit_queue[txpkt.rf_chain], current_concentrator_time, &txpkt, downlink_type );
                if( jit_result != JIT_ERROR_OK )
                {
                    ESP_LOGE( TAG_DOWN, "ERROR: Packet REJECTED (jit error=%d)\n", jit_result );
                }
                else
                {
                    /* In case of a warning having been raised before, we notify it */
                    jit_result = warning_result;
                }
                pthread_mutex_lock( &mx_meas_dw );
                meas_nb_tx_requested += 1;
                pthread_mutex_unlock( &mx_meas_dw );
            }

            /* Send acknoledge datagram to server */
            i = send_tx_ack( buff_down[1], buff_down[2], jit_result, warning_value );
            if( i < 0 )
            {
                ESP_LOGE( TAG_DOWN, "ERROR: Failed to send tx_ack datagram - %d\n", i );
            }
        }
    }
    ESP_LOGI( TAG_DOWN, "\nINFO: End of downstream thread\n" );
}

/* -------------------------------------------------------------------------- */
/* --- THREAD 3: CHECKING PACKETS TO BE SENT FROM JIT QUEUE AND SEND THEM --- */

void print_tx_status( uint8_t tx_status )
{
    switch( tx_status )
    {
    case TX_OFF:
        ESP_LOGI( TAG_JIT, "INFO: [jit] lgw_status returned TX_OFF\n" );
        break;
    case TX_FREE:
        ESP_LOGI( TAG_JIT, "INFO: [jit] lgw_status returned TX_FREE\n" );
        break;
    case TX_EMITTING:
        ESP_LOGI( TAG_JIT, "INFO: [jit] lgw_status returned TX_EMITTING\n" );
        break;
    case TX_SCHEDULED:
        ESP_LOGI( TAG_JIT, "INFO: [jit] lgw_status returned TX_SCHEDULED\n" );
        break;
    default:
        ESP_LOGI( TAG_JIT, "INFO: [jit] lgw_status returned UNKNOWN (%d)\n", tx_status );
        break;
    }
}

void thread_jit( void )
{
    int                 result = LGW_HAL_SUCCESS;
    struct lgw_pkt_tx_s pkt;
    int                 pkt_index                 = -1;
    uint32_t            current_concentrator_time = 0;
    enum jit_error_e    jit_result;
    enum jit_pkt_type_e pkt_type;
    uint8_t             tx_status;
    int                 i;

    while( !exit_sig )
    {
        // ESP_LOGI(TAG_JIT, "JIT");
        vTaskDelay( 10 / portTICK_PERIOD_MS );

        for( i = 0; i < LGW_RF_CHAIN_NB; i++ )
        {
            /* transfer data and metadata to the concentrator, and schedule TX */
            lgw_get_instcnt( &current_concentrator_time );
            jit_result = jit_peek( &jit_queue[i], current_concentrator_time, &pkt_index );
            if( jit_result == JIT_ERROR_OK )
            {
                if( pkt_index > -1 )
                {
                    jit_result = jit_dequeue( &jit_queue[i], pkt_index, &pkt, &pkt_type );
                    if( jit_result == JIT_ERROR_OK )
                    {
                        /* update beacon stats */
                        if( pkt_type == JIT_PKT_TYPE_BEACON )
                        {
#if 0
                            /* Compensate breacon frequency with xtal error */
                            pthread_mutex_lock(&mx_xcorr);
                            pkt.freq_hz = (uint32_t)(xtal_correct * (double)pkt.freq_hz);
                            MSG_DEBUG(DEBUG_BEACON, "beacon_pkt.freq_hz=%lu (xtal_correct=%.15lf)\n", pkt.freq_hz, xtal_correct);
                            pthread_mutex_unlock(&mx_xcorr);

                            /* Update statistics */
                            pthread_mutex_lock(&mx_meas_dw);
                            meas_nb_beacon_sent += 1;
                            pthread_mutex_unlock(&mx_meas_dw);
                            ESP_LOGI(TAG_JIT, "INFO: Beacon dequeued (count_us=%lu)\n", pkt.count_us);
#else
                            ESP_LOGE( TAG_JIT, "NO SUPPORT FOR BEACONING\n" );
                            continue;
#endif
                        }

                        /* check if concentrator is free for sending new packet */
                        result = lgw_status( pkt.rf_chain, TX_STATUS, &tx_status );
                        if( result == LGW_HAL_ERROR )
                        {
                            ESP_LOGW( TAG_JIT, "WARNING: [jit%d] lgw_status failed\n", i );
                        }
                        else
                        {
                            if( tx_status == TX_EMITTING )
                            {
                                ESP_LOGE( TAG_JIT, "ERROR: concentrator is currently emitting on rf_chain %d\n", i );
                                print_tx_status( tx_status );
                                continue;
                            }
                            else if( tx_status == TX_SCHEDULED )
                            {
                                ESP_LOGW(
                                    TAG_JIT,
                                    "WARNING: a downlink was already scheduled on rf_chain %d, overwritting it...\n",
                                    i );
                                print_tx_status( tx_status );
                            }
                            else
                            {
                                /* Nothing to do */
                            }
                        }

                        /* send packet to concentrator */
                        pthread_mutex_lock( &mx_concent ); /* may have to wait for a fetch to finish */
                        result = lgw_send( &pkt );
                        pthread_mutex_unlock( &mx_concent ); /* free concentrator ASAP */
                        if( result != LGW_HAL_SUCCESS )
                        {
                            pthread_mutex_lock( &mx_meas_dw );
                            meas_nb_tx_fail += 1;
                            pthread_mutex_unlock( &mx_meas_dw );
                            ESP_LOGW( TAG_JIT, "WARNING: [jit] lgw_send failed on rf_chain %d\n", i );
                            continue;
                        }
                        else
                        {
                            pthread_mutex_lock( &mx_meas_dw );
                            meas_nb_tx_ok += 1;
                            pthread_mutex_unlock( &mx_meas_dw );
                            MSG_DEBUG( DEBUG_PKT_FWD, "lgw_send done on rf_chain %d: count_us=%lu\n", i, pkt.count_us );

                            /* Update display */
                            // TODO?
                            // display_stats_t rx_tx_stats = { .nb_rx = 0, .nb_tx = 1 };
                            // display_update_statistics( &rx_tx_stats );
                        }
                    }
                    else
                    {
                        ESP_LOGE( TAG_JIT, "ERROR: jit_dequeue failed on rf_chain %d with %d\n", i, jit_result );
                    }
                }
            }
            else if( jit_result == JIT_ERROR_EMPTY )
            {
                /* Do nothing, it can happen */
            }
            else
            {
                ESP_LOGE( TAG_JIT, "ERROR: jit_peek failed on rf_chain %d with %d\n", i, jit_result );
            }
        }
    }
}

/* -------------------------------------------------------------------------- */
/* --- THREAD 4: INITIALIZE AND RUN PACKET FORWARDER ------------------------ */

void thread_pktfwd( void )
{
    int       i; /* loop variable and temporary variable for return value */
    esp_err_t esp_err;
    float     temperature;

    /* network socket creation */
    struct addrinfo  hints;
    struct addrinfo* result; /* store result of getaddrinfo */

    /* threads */
    pthread_t thrid_up;
    pthread_t thrid_down;
    pthread_t thrid_jit;

    /* variables to get local copies of measurements */
    uint32_t cp_nb_rx_rcv;
    uint32_t cp_nb_rx_ok;
    uint32_t cp_nb_rx_bad;
    uint32_t cp_nb_rx_nocrc;
    uint32_t cp_up_pkt_fwd;
    uint32_t cp_up_network_byte;
    uint32_t cp_up_payload_byte;
    uint32_t cp_up_dgram_sent;
    uint32_t cp_up_ack_rcv;
    uint32_t cp_dw_pull_sent;
    uint32_t cp_dw_ack_rcv;
    uint32_t cp_dw_dgram_rcv;
    uint32_t cp_dw_network_byte;
    uint32_t cp_dw_payload_byte;
    uint32_t cp_nb_tx_ok;
    uint32_t cp_nb_tx_fail;
    uint32_t cp_nb_tx_requested                 = 0;
    uint32_t cp_nb_tx_rejected_collision_packet = 0;
    uint32_t cp_nb_tx_rejected_collision_beacon = 0;
    uint32_t cp_nb_tx_rejected_too_late         = 0;
    uint32_t cp_nb_tx_rejected_too_early        = 0;

    /* statistics variable */
    time_t t;
    char   stat_timestamp[24];
    float  rx_ok_ratio;
    float  rx_bad_ratio;
    float  rx_nocrc_ratio;
    float  up_ack_ratio;
    float  dw_ack_ratio;

    /* get timezone info */
    tzset( );

    /* Configure radio parameters of the hub */
    i = parse_radio_configuration( );
    if( i != 0 )
    {
        ESP_LOGE( TAG_PKT_FWD, "ERROR: failed to parse radio configuration\n" );
        lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: failed to parse radio configuration\n");
        wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
    }

    /* Configure gateway parameters */
    i = parse_gateway_configuration( );
    if( i != 0 )
    {
        ESP_LOGE( TAG_PKT_FWD, "ERROR: failed to parse gateway configuration\n" );
        lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: failed to parse gateway configuration\n");
        wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
    }

    /* Update display with connection info */
    // TODO?
    // display_connection_info_t connect_info = { .gateway_id = lgwm };
    // display_update_connection_info( &connect_info );

    /* process some of the configuration variables */
    net_mac_h = htonl( ( uint32_t )( 0xFFFFFFFF & ( lgwm >> 32 ) ) );
    net_mac_l = htonl( ( uint32_t )( 0xFFFFFFFF & lgwm ) );

    /* prepare hints to open network sockets */
    memset( &hints, 0, sizeof hints );
    hints.ai_family   = AF_INET; /* WA: Forcing IPv4 as AF_UNSPEC makes connection on localhost to fail */
    hints.ai_socktype = SOCK_DGRAM;

    /* look for server address w/ upstream port */
    i = getaddrinfo( serv_addr, serv_port_up, &hints, &result );
    if( i != 0 || result == NULL )
    {
        ESP_LOGE( TAG_PKT_FWD, "ERROR: [up] getaddrinfo on address %s (PORT %s) returned %d\n", serv_addr, serv_port_up,
                  i );
        lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [up] getaddrinfo on address %s (PORT %s) returned %d\n", serv_addr, serv_port_up,
                  i);
        wait_on_error( LRHB_ERROR_LNS, __LINE__ );
    }

    /* try to open socket for upstream traffic */
    sock_up = socket( result->ai_family, result->ai_socktype, result->ai_protocol );
    if( sock_up < 0 )
    {
        ESP_LOGE( TAG_PKT_FWD, "ERROR: [up] failed to open socket for uplink\n" );
        lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [up] failed to open socket for uplink\n");
        wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
    }

    /* connect so we can send/receive packet with the server only */
    i = connect( sock_up, result->ai_addr, result->ai_addrlen );
    if( i != 0 )
    {
        ESP_LOGE( TAG_PKT_FWD, "ERROR: [up] connect returned %s\n", strerror( errno ) );
        lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [up] connect returned %s\n", strerror( errno ) );
        wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
    }
    freeaddrinfo( result );

    /* look for server address w/ downstream port */
    i = getaddrinfo( serv_addr, serv_port_down, &hints, &result );
    if( i != 0 || result == NULL )
    {
        ESP_LOGE( TAG_PKT_FWD, "ERROR: [up] getaddrinfo on address %s (PORT %s) returned %d\n", serv_addr,
                  serv_port_down, i );
        lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [up] getaddrinfo on address %s (PORT %s) returned %d\n", serv_addr,
                  serv_port_down, i);
        wait_on_error( LRHB_ERROR_LNS, __LINE__ );
    }

    /* try to open socket for downstream traffic */
    sock_down = socket( result->ai_family, result->ai_socktype, result->ai_protocol );
    if( sock_down < 0 )
    {
        ESP_LOGE( TAG_PKT_FWD, "ERROR: [down] failed to open socket for downlink\n" );
        lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [down] failed to open socket for downlink\n");
        wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
    }

    /* connect so we can send/receive packet with the server only */
    i = connect( sock_down, result->ai_addr, result->ai_addrlen );
    if( i != 0 )
    {
        ESP_LOGE( TAG_PKT_FWD, "ERROR: [down] connect returned %s\n", strerror( errno ) );
        lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [down] connect returned %s\n", strerror( errno ) );
        wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
    }
    freeaddrinfo( result );

    /* starting the hub */
    i = lgw_start( );
    if( i == LGW_HAL_SUCCESS )
    {
        ESP_LOGI( TAG_PKT_FWD, "INFO: [main] LoRaHub started, packet can now be received" );
        lorahub_log_display(LORAHUB_LOG_LEVEL_INFO, "INFO: [main] LoRaHub started, packet can now be received" );
    }
    else
    {
        ESP_LOGE( TAG_PKT_FWD, "ERROR: [main] failed to start the concentrator\n" );
        lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [main] failed to start the concentrator\n" );
        wait_on_error( LRHB_ERROR_HAL, __LINE__ );
    }

    /* spawn threads to manage upstream and downstream */
    i = pthread_create( &thrid_up, NULL, ( void* ( * ) ( void* ) ) thread_up, NULL );
    if( i != 0 )
    {
        ESP_LOGE( TAG_PKT_FWD, "ERROR: [main] impossible to create upstream thread\n" );
        lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [main] impossible to create upstream thread\n" );
        wait_on_error( LRHB_ERROR_OS, __LINE__ );
    }
    i = pthread_create( &thrid_down, NULL, ( void* ( * ) ( void* ) ) thread_down, NULL );
    if( i != 0 )
    {
        ESP_LOGE( TAG_PKT_FWD, "ERROR: [main] impossible to create downstream thread\n" );
        lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [main] impossible to create downstream thread\n" );
        wait_on_error( LRHB_ERROR_OS, __LINE__ );
    }
    i = pthread_create( &thrid_jit, NULL, ( void* ( * ) ( void* ) ) thread_jit, NULL );
    if( i != 0 )
    {
        ESP_LOGE( TAG_PKT_FWD, "ERROR: [main] impossible to create JIT thread\n" );
        lorahub_log_display(LORAHUB_LOG_LEVEL_ERROR, "ERROR: [main] impossible to create JIT thread\n" );
        wait_on_error( LRHB_ERROR_OS, __LINE__ );
    }

    /* Update status for display */
    // TODO?
    // display_update_status( DISPLAY_STATUS_RECEIVING );
    // display_stats_t rx_tx_stats = { 0 };
    // display_update_statistics( &rx_tx_stats );

    /* main loop task : statistics collection */
    while( !exit_sig )
    {
        /* wait for next reporting interval */
        vTaskDelay( stat_interval * 1000 / portTICK_PERIOD_MS );

        /* get timestamp for statistics */
        t = time( NULL );
        strftime( stat_timestamp, sizeof stat_timestamp, "%F %T %Z", gmtime( &t ) );

        /* access upstream statistics, copy and reset them */
        pthread_mutex_lock( &mx_meas_up );
        cp_nb_rx_rcv         = meas_nb_rx_rcv;
        cp_nb_rx_ok          = meas_nb_rx_ok;
        cp_nb_rx_bad         = meas_nb_rx_bad;
        cp_nb_rx_nocrc       = meas_nb_rx_nocrc;
        cp_up_pkt_fwd        = meas_up_pkt_fwd;
        cp_up_network_byte   = meas_up_network_byte;
        cp_up_payload_byte   = meas_up_payload_byte;
        cp_up_dgram_sent     = meas_up_dgram_sent;
        cp_up_ack_rcv        = meas_up_ack_rcv;
        meas_nb_rx_rcv       = 0;
        meas_nb_rx_ok        = 0;
        meas_nb_rx_bad       = 0;
        meas_nb_rx_nocrc     = 0;
        meas_up_pkt_fwd      = 0;
        meas_up_network_byte = 0;
        meas_up_payload_byte = 0;
        meas_up_dgram_sent   = 0;
        meas_up_ack_rcv      = 0;
        pthread_mutex_unlock( &mx_meas_up );
        if( cp_nb_rx_rcv > 0 )
        {
            rx_ok_ratio    = ( float ) cp_nb_rx_ok / ( float ) cp_nb_rx_rcv;
            rx_bad_ratio   = ( float ) cp_nb_rx_bad / ( float ) cp_nb_rx_rcv;
            rx_nocrc_ratio = ( float ) cp_nb_rx_nocrc / ( float ) cp_nb_rx_rcv;
        }
        else
        {
            rx_ok_ratio    = 0.0;
            rx_bad_ratio   = 0.0;
            rx_nocrc_ratio = 0.0;
        }
        if( cp_up_dgram_sent > 0 )
        {
            up_ack_ratio = ( float ) cp_up_ack_rcv / ( float ) cp_up_dgram_sent;
        }
        else
        {
            up_ack_ratio = 0.0;
        }

        /* access downstream statistics, copy and reset them */
        pthread_mutex_lock( &mx_meas_dw );
        cp_dw_pull_sent    = meas_dw_pull_sent;
        cp_dw_ack_rcv      = meas_dw_ack_rcv;
        cp_dw_dgram_rcv    = meas_dw_dgram_rcv;
        cp_dw_network_byte = meas_dw_network_byte;
        cp_dw_payload_byte = meas_dw_payload_byte;
        cp_nb_tx_ok        = meas_nb_tx_ok;
        cp_nb_tx_fail      = meas_nb_tx_fail;
        cp_nb_tx_requested += meas_nb_tx_requested;
        cp_nb_tx_rejected_collision_packet += meas_nb_tx_rejected_collision_packet;
        cp_nb_tx_rejected_collision_beacon += meas_nb_tx_rejected_collision_beacon;
        cp_nb_tx_rejected_too_late += meas_nb_tx_rejected_too_late;
        cp_nb_tx_rejected_too_early += meas_nb_tx_rejected_too_early;
        meas_dw_pull_sent                    = 0;
        meas_dw_ack_rcv                      = 0;
        meas_dw_dgram_rcv                    = 0;
        meas_dw_network_byte                 = 0;
        meas_dw_payload_byte                 = 0;
        meas_nb_tx_ok                        = 0;
        meas_nb_tx_fail                      = 0;
        meas_nb_tx_requested                 = 0;
        meas_nb_tx_rejected_collision_packet = 0;
        meas_nb_tx_rejected_collision_beacon = 0;
        meas_nb_tx_rejected_too_late         = 0;
        meas_nb_tx_rejected_too_early        = 0;
        pthread_mutex_unlock( &mx_meas_dw );
        if( cp_dw_pull_sent > 0 )
        {
            dw_ack_ratio = ( float ) cp_dw_ack_rcv / ( float ) cp_dw_pull_sent;
        }
        else
        {
            dw_ack_ratio = 0.0;
        }

        /* display a report */
        printf( "\n##### %s #####\n", stat_timestamp );
        printf( "### [UPSTREAM] ###\n" );
        printf( "# RF packets received by concentrator: %lu\n", cp_nb_rx_rcv );
        printf( "# CRC_OK: %.2f%%, CRC_FAIL: %.2f%%, NO_CRC: %.2f%%\n", 100.0 * rx_ok_ratio, 100.0 * rx_bad_ratio,
                100.0 * rx_nocrc_ratio );
        printf( "# RF packets forwarded: %lu (%lu bytes)\n", cp_up_pkt_fwd, cp_up_payload_byte );
        printf( "# PUSH_DATA datagrams sent: %lu (%lu bytes)\n", cp_up_dgram_sent, cp_up_network_byte );
        printf( "# PUSH_DATA acknowledged: %.2f%%\n", 100.0 * up_ack_ratio );
        printf( "### [DOWNSTREAM] ###\n" );
        printf( "# PULL_DATA sent: %lu (%.2f%% acknowledged)\n", cp_dw_pull_sent, 100.0 * dw_ack_ratio );
        printf( "# PULL_RESP(onse) datagrams received: %lu (%lu bytes)\n", cp_dw_dgram_rcv, cp_dw_network_byte );
        printf( "# RF packets sent to concentrator: %lu (%lu bytes)\n", ( cp_nb_tx_ok + cp_nb_tx_fail ),
                cp_dw_payload_byte );
        printf( "# TX errors: %lu\n", cp_nb_tx_fail );
        if( cp_nb_tx_requested != 0 )
        {
            printf( "# TX rejected (collision packet): %.2f%% (req:%lu, rej:%lu)\n",
                    100.0 * cp_nb_tx_rejected_collision_packet / cp_nb_tx_requested, cp_nb_tx_requested,
                    cp_nb_tx_rejected_collision_packet );
            printf( "# TX rejected (collision beacon): %.2f%% (req:%lu, rej:%lu)\n",
                    100.0 * cp_nb_tx_rejected_collision_beacon / cp_nb_tx_requested, cp_nb_tx_requested,
                    cp_nb_tx_rejected_collision_beacon );
            printf( "# TX rejected (too late): %.2f%% (req:%lu, rej:%lu)\n",
                    100.0 * cp_nb_tx_rejected_too_late / cp_nb_tx_requested, cp_nb_tx_requested,
                    cp_nb_tx_rejected_too_late );
            printf( "# TX rejected (too early): %.2f%% (req:%lu, rej:%lu)\n",
                    100.0 * cp_nb_tx_rejected_too_early / cp_nb_tx_requested, cp_nb_tx_requested,
                    cp_nb_tx_rejected_too_early );
        }
        printf( "### [JIT] ###\n" );
        jit_print_queue( &jit_queue[0], false, DEBUG_LOG );
        temperature = 0;
        if( temp_sensor != NULL )
        {
            esp_err = temperature_sensor_get_celsius( temp_sensor, &temperature );
            if( esp_err != ESP_OK )
            {
                printf( "### Concentrator temperature unknown - %s\n", esp_err_to_name( esp_err ) );
            }
            else
            {
                printf( "### Concentrator temperature: %.0f°C ###\n", temperature );
            }
        }
        else
        {
            printf( "### No temperature sensor initialized\n" );
        }
        printf( "##### END #####\n" );

        /* generate a JSON report (will be sent to server by upstream thread) */
        pthread_mutex_lock( &mx_stat_rep );
        snprintf( status_report, STATUS_SIZE,
                  "\"stat\":{\"time\":\"%s\",\"rxnb\":%lu,\"rxok\":%lu,\"rxfw\":%lu,\"ackr\":%.1f,\"dwnb\":%lu,"
                  "\"txnb\":%lu,\"temp\":%.0f}",
                  stat_timestamp, cp_nb_rx_rcv, cp_nb_rx_ok, cp_up_pkt_fwd, 100.0 * up_ack_ratio, cp_dw_dgram_rcv,
                  cp_nb_tx_ok, temperature );
        report_ready = true;
        pthread_mutex_unlock( &mx_stat_rep );
    }

    /* wait for upstream thread to finish (1 fetch cycle max) */
    pthread_join( thrid_up, NULL );
    pthread_cancel( thrid_down ); /* don't wait for downstream thread */
    pthread_cancel( thrid_jit );  /* don't wait for jit thread */

    /* shut down network sockets */
    shutdown( sock_up, SHUT_RDWR );
    shutdown( sock_down, SHUT_RDWR );
    /* stop the hardware */
    i = lgw_stop( );
    if( i == LGW_HAL_SUCCESS )
    {
        ESP_LOGI( TAG_PKT_FWD, "INFO: concentrator stopped successfully\n" );
    }
    else
    {
        ESP_LOGW( TAG_PKT_FWD, "WARNING: failed to stop concentrator successfully\n" );
    }

    ESP_LOGI( TAG_PKT_FWD, "INFO: Exiting packet forwarder thread\n" );
}

/* -------------------------------------------------------------------------- */
/* --- PUBLIC FUNCTIONS DEFINITION ------------------------------------------ */

int launch_pkt_fwd( temperature_sensor_handle_t temperature_sensor )
{
    int       i;
    pthread_t thrid_pktfwd;

    temp_sensor = temperature_sensor;

    i = pthread_create( &thrid_pktfwd, NULL, ( void* ( * ) ( void* ) ) thread_pktfwd, NULL );
    if( i != 0 )
    {
        ESP_LOGE( TAG_PKT_FWD, "ERROR: [main] impossible to create packet forwarder thread\n" );
        wait_on_error( LRHB_ERROR_UNKNOWN, __LINE__ );
    }

    return 0;
}