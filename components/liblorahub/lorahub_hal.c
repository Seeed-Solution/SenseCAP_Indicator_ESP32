/*______                              _
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
(C)2024 Semtech

Description:
    LoRaHub Hardware Abstraction Layer

License: Revised BSD License, see LICENSE.TXT file include in the project
*/

/* -------------------------------------------------------------------------- */
/* --- DEPENDENCIES --------------------------------------------------------- */

#include <string.h>

#include <esp_timer.h>

#include "lorahub_aux.h"
#include "lorahub_hal.h"
#include "lorahub_hal_rx.h"
#include "lorahub_hal_tx.h"

#include "radio_context.h"
#include "ral.h"

#include "sx126x_lorahub.h"

#define INDICATOR_LORAHUB 1

#if defined( CONFIG_HELTEC_WIFI_LORA_32_V3 )
#if !defined( CONFIG_RADIO_TYPE_SX1262 )
#error "Wrong radio selected for Heltec WiFi LoRa 32 v3 board. Please select sx1262 radio type"
#endif
#endif

#if defined( CONFIG_RADIO_TYPE_SX1261 ) || defined( CONFIG_RADIO_TYPE_SX1262 ) || defined( CONFIG_RADIO_TYPE_SX1268 )
#include "ral_sx126x.h"
#include "ral_sx126x_bsp.h"
#include "smtc_shield_sx126x.h"
#elif defined( CONFIG_RADIO_TYPE_LLCC68 )
#include "ral_llcc68.h"
#include "ral_llcc68_bsp.h"
#include "smtc_shield_llcc68.h"
#elif defined( CONFIG_RADIO_TYPE_LR1121 )
#include "ral_lr11xx.h"
#include "ral_lr11xx_bsp.h"
#include "smtc_shield_lr11xx.h"
#include "lr11xx_system.h"
#else
#error "Please select radio type.."
#endif

/* -------------------------------------------------------------------------- */
/* --- PRIVATE MACROS ------------------------------------------------------- */

#define CHECK_NULL( a )       \
    if( a == NULL )           \
    {                         \
        return LGW_HAL_ERROR; \
    }

/* -------------------------------------------------------------------------- */
/* --- PRIVATE CONSTANTS ---------------------------------------------------- */

#define SPI_SPEED 2000000

static const char* TAG_HAL = "LORAHUB_HAL";

#define LORA_SYNC_WORD_PRIVATE 0x12  // 0x12 Private Network
#define LORA_SYNC_WORD_PUBLIC 0x34   // 0x34 Public Network

/* -------------------------------------------------------------------------- */
/* --- PRIVATE TYPES -------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PRIVATE VARIABLES ---------------------------------------------------- */

static bool    is_started = false;
static uint8_t rx_status  = RX_STATUS_UNKNOWN;
static uint8_t tx_status  = TX_STATUS_UNKNOWN;

static struct lgw_conf_rxrf_s rxrf_conf = { .freq_hz = 0, .rssi_offset = 0.0, .tx_enable = false };

static struct lgw_conf_rxif_s rxif_conf = {
    .bandwidth = BW_UNDEFINED, .coderate = CR_UNDEFINED, .datarate = DR_UNDEFINED, .modulation = MOD_UNDEFINED
};

static spi_host_device_t spi_host_id = SPI2_HOST;

static radio_context_t radio_context = { 0 };
#define RADIO_CONTEXT ( ( void* ) &radio_context )

#if defined( CONFIG_RADIO_TYPE_SX1261 ) || defined( CONFIG_RADIO_TYPE_SX1262 ) || defined( CONFIG_RADIO_TYPE_SX1268 )
const ral_t lgw_ral = RAL_SX126X_INSTANTIATE( RADIO_CONTEXT );
#elif defined( CONFIG_RADIO_TYPE_LLCC68 )
const ral_t lgw_ral = RAL_LLCC68_INSTANTIATE( RADIO_CONTEXT );
#elif defined( CONFIG_RADIO_TYPE_LR1121 )
const ral_t lgw_ral = RAL_LR11XX_INSTANTIATE( RADIO_CONTEXT );
#else
#error "Please select radio type.."
#endif

/* -------------------------------------------------------------------------- */
/* --- PRIVATE FUNCTIONS DECLARATION ---------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PRIVATE FUNCTIONS DEFINITION ----------------------------------------- */

int lgw_connect( void )
{
#if (INDICATOR_LORAHUB)
    sx126x_init();
#endif
    esp_err_t ret;

#if defined( CONFIG_RADIO_TYPE_SX1261 ) || defined( CONFIG_RADIO_TYPE_SX1262 ) || defined( CONFIG_RADIO_TYPE_SX1268 )
    const smtc_shield_sx126x_t*        shield        = ral_sx126x_get_shield( );
    const smtc_shield_sx126x_pinout_t* shield_pinout = shield->get_pinout( );
#elif defined( CONFIG_RADIO_TYPE_LLCC68 )
    const smtc_shield_llcc68_t*        shield        = ral_llcc68_get_shield( );
    const smtc_shield_llcc68_pinout_t* shield_pinout = shield->get_pinout( );
#elif defined( CONFIG_RADIO_TYPE_LR1121 )
    const smtc_shield_lr11xx_t*        shield        = ral_lr11xx_get_shield( );
    const smtc_shield_lr11xx_pinout_t* shield_pinout = shield->get_pinout( );
#endif

    /* Initialize radio context */
    radio_context.spi_nss     = shield_pinout->nss;
    radio_context.spi_sclk    = shield_pinout->sclk;
    radio_context.spi_miso    = shield_pinout->miso;
    radio_context.spi_mosi    = shield_pinout->mosi;
    radio_context.gpio_rst    = shield_pinout->reset;
    radio_context.gpio_busy   = shield_pinout->busy;
    radio_context.gpio_dio1   = shield_pinout->irq;
    radio_context.gpio_led_tx = shield_pinout->led_tx;
    radio_context.gpio_led_rx = shield_pinout->led_rx;

#if (INDICATOR_LORAHUB)
    indicator_io_expander->set_direction( radio_context.gpio_busy, 0 );

    indicator_io_expander->set_direction( radio_context.spi_nss, 1 );
    indicator_io_expander->set_level( radio_context.spi_nss, 1 );

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_NEGEDGE; //falling edge
    io_conf.pin_bit_mask = (1ULL << radio_context.gpio_dio1);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
#else
    /* GPIO configuration for radio */
    gpio_reset_pin( radio_context.gpio_busy );
    gpio_set_direction( radio_context.gpio_busy, GPIO_MODE_INPUT );

    gpio_reset_pin( radio_context.spi_nss );
    gpio_set_direction( radio_context.spi_nss, GPIO_MODE_OUTPUT );
    gpio_set_level( radio_context.spi_nss, 1 );

    gpio_reset_pin( radio_context.gpio_rst );
    gpio_set_direction( radio_context.gpio_rst, GPIO_MODE_OUTPUT );

    gpio_reset_pin( radio_context.gpio_dio1 );
    gpio_set_direction( radio_context.gpio_dio1, GPIO_MODE_INPUT );
    gpio_set_intr_type( radio_context.gpio_dio1, GPIO_INTR_POSEDGE );
#endif

    /* GPIO configuration for antenna switch */
#if defined( CONFIG_RADIO_TYPE_SX1261 ) || defined( CONFIG_RADIO_TYPE_SX1262 ) || \
    defined( CONFIG_RADIO_TYPE_SX1268 ) || defined( CONFIG_RADIO_TYPE_LLCC68 )
    if( shield_pinout->antenna_sw != 0xFF )
    {
        ESP_LOGI( TAG_HAL, "Set ANT_SW to 1 through GPIO%d", shield_pinout->antenna_sw );
        gpio_reset_pin( shield_pinout->antenna_sw );
        gpio_set_direction( shield_pinout->antenna_sw, GPIO_MODE_OUTPUT );
        gpio_set_level( shield_pinout->antenna_sw, 1 );
    }
    else
    {
        ESP_LOGI( TAG_HAL, "ANT_SW GPIO not set" );
    }
#endif

    /* GPIO configuration for radio shields RX/TX LEDs */
    if( shield_pinout->led_rx != 0xFF )
    {
        ESP_LOGI( TAG_HAL, "Radio shield RX led: GPIO%d", shield_pinout->led_rx );
        gpio_reset_pin( shield_pinout->led_rx );
        gpio_set_direction( shield_pinout->led_rx, GPIO_MODE_OUTPUT );
        gpio_set_level( shield_pinout->led_rx, 0 );
    }
    else
    {
        ESP_LOGI( TAG_HAL, "LED_RX_GPIO not set" );
    }

    if( shield_pinout->led_tx != 0xFF )
    {
        ESP_LOGI( TAG_HAL, "Radio shield TX led: GPIO%d", shield_pinout->led_tx );
        gpio_reset_pin( shield_pinout->led_tx );
        gpio_set_direction( shield_pinout->led_tx, GPIO_MODE_OUTPUT );
        gpio_set_level( shield_pinout->led_tx, 0 );
    }
    else
    {
        ESP_LOGI( TAG_HAL, "LED_TX_GPIO not set" );
    }

    /* SPI configuration */
    spi_bus_config_t spi_bus_config = { .mosi_io_num   = radio_context.spi_mosi,
                                        .miso_io_num   = radio_context.spi_miso,
                                        .sclk_io_num   = radio_context.spi_sclk,
                                        .quadwp_io_num = -1,
                                        .quadhd_io_num = -1 };

    ret = spi_bus_initialize( spi_host_id, &spi_bus_config, SPI_DMA_CH_AUTO );
    if( ret != ESP_OK )
    {
        ESP_LOGE( TAG_HAL, "ERROR: spi_bus_initialize failed with %d", ret );
        return -1;
    }

    spi_device_interface_config_t devcfg;
    memset( &devcfg, 0, sizeof( spi_device_interface_config_t ) );
    devcfg.clock_speed_hz = SPI_SPEED;
    devcfg.spics_io_num   = -1;
    devcfg.queue_size     = 7;
    devcfg.mode           = 0;
    devcfg.flags          = SPI_DEVICE_NO_DUMMY;

    ret = spi_bus_add_device( spi_host_id, &devcfg, &( radio_context.spi_handle ) );
    if( ret != ESP_OK )
    {
        ESP_LOGE( TAG_HAL, "ERROR: spi_bus_add_device failed with %d", ret );
        return -1;
    }

    return 0;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

int lgw_radio_setup( void )
{
    ASSERT_RAL_RC( ral_reset( &lgw_ral ) );
    ASSERT_RAL_RC( ral_init( &lgw_ral ) );

#if defined( CONFIG_RADIO_TYPE_LR1121 )
    lr11xx_status_t ret;

    lr11xx_system_stat1_t stat1 = { 0 };
    ret                         = lr11xx_system_get_status( lgw_ral.context, &stat1, 0, 0 );
    if( ret != LR11XX_STATUS_OK )
    {
        ESP_LOGE( TAG_HAL, "ERROR: lr11xx_system_get_status failed" );
        return LGW_HAL_ERROR;
    }
    /* Sanity check */
    if( stat1.command_status < LR11XX_SYSTEM_CMD_STATUS_OK )
    {
        ESP_LOGE( TAG_HAL, "ERROR: LR11xx status (stat1.command_status): 0x%02X", stat1.command_status );
        return LGW_HAL_ERROR;
    }

    lr11xx_system_version_t version = { 0 };
    ret                             = lr11xx_system_get_version( lgw_ral.context, &version );
    if( ret != LR11XX_STATUS_OK )
    {
        ESP_LOGE( TAG_HAL, "ERROR: lr11xx_system_get_version failed" );
        return LGW_HAL_ERROR;
    }
    ESP_LOGI( TAG_HAL, "LR11xx version information:" );
    ESP_LOGI( TAG_HAL, "  - Hardware = 0x%02X", version.hw );
    ESP_LOGI( TAG_HAL, "  - Type     = 0x%02X (0x01 for LR1110, 0x02 for LR1120, 0x03 for LR1121)", version.type );
    ESP_LOGI( TAG_HAL, "  - Firmware = 0x%04X", version.fw );
    /* Sanity check */
    if( ( version.type != LR11XX_SYSTEM_VERSION_TYPE_LR1110 ) &&
        ( version.type != LR11XX_SYSTEM_VERSION_TYPE_LR1120 ) && ( version.type != LR11XX_SYSTEM_VERSION_TYPE_LR1121 ) )
    {
        ESP_LOGE( TAG_HAL, "ERROR: LR11xx type: 0x%02X", version.type );
        return LGW_HAL_ERROR;
    }
#endif

    ASSERT_RAL_RC( ral_set_rx_tx_fallback_mode( &lgw_ral, RAL_FALLBACK_STDBY_RC ) );
    ASSERT_RAL_RC(
        ral_set_lora_sync_word( &lgw_ral, LORA_SYNC_WORD_PUBLIC ) );  // TODO: make it configurable in menuconfig

    /* Install interrupt handler for RX IRQs */
    lgw_radio_init_rx( &lgw_ral );

    return LGW_HAL_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/* --- PUBLIC FUNCTIONS DEFINITION ------------------------------------------ */

int lgw_rxrf_setconf( struct lgw_conf_rxrf_s* conf )
{
    CHECK_NULL( conf );

    /* check if the concentrator is running */
    if( is_started == true )
    {
        ESP_LOGI( TAG_HAL, "ERROR: CONCENTRATOR IS RUNNING, STOP IT BEFORE CHANGING CONFIGURATION\n" );
        return LGW_HAL_ERROR;
    }

    memcpy( &rxrf_conf, conf, sizeof( struct lgw_conf_rxrf_s ) );

    return LGW_HAL_SUCCESS;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

int lgw_rxif_setconf( struct lgw_conf_rxif_s* conf )
{
    CHECK_NULL( conf );

    /* check if the concentrator is running */
    if( is_started == true )
    {
        ESP_LOGI( TAG_HAL, "ERROR: CONCENTRATOR IS RUNNING, STOP IT BEFORE CHANGING CONFIGURATION\n" );
        return LGW_HAL_ERROR;
    }

    memcpy( &rxif_conf, conf, sizeof( struct lgw_conf_rxif_s ) );

    return LGW_HAL_SUCCESS;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

int lgw_start( void )
{
    int err;

    if( is_started == true )
    {
        ESP_LOGW( TAG_HAL, "Note: LoRa concentrator already started, restarting it now\n" );
    }

    /* Check configuration */
    if( rxrf_conf.freq_hz == 0 )
    {
        ESP_LOGE( TAG_HAL, "ERROR: radio frequency not configured\n" );
        return LGW_HAL_ERROR;
    }
    if( rxif_conf.modulation == MOD_UNDEFINED )
    {
        ESP_LOGE( TAG_HAL, "ERROR: modulation type not configured\n" );
        return LGW_HAL_ERROR;
    }
    if( rxif_conf.bandwidth == BW_UNDEFINED )
    {
        ESP_LOGE( TAG_HAL, "ERROR: modulation bandwidth not configured\n" );
        return LGW_HAL_ERROR;
    }
    if( rxif_conf.coderate == CR_UNDEFINED )
    {
        ESP_LOGE( TAG_HAL, "ERROR: modulation coderate not configured\n" );
        return LGW_HAL_ERROR;
    }
    if( rxif_conf.datarate == DR_UNDEFINED )
    {
        ESP_LOGE( TAG_HAL, "ERROR: modulation datarate not configured\n" );
        return LGW_HAL_ERROR;
    }

    /* Configure SPI and GPIOs */
    err = lgw_connect( );
    if( err == LGW_HAL_ERROR )
    {
        ESP_LOGE( TAG_HAL, "ERROR: FAILED TO CONNECT BOARD\n" );
        return LGW_HAL_ERROR;
    }

    /* Configure radio */
    err = lgw_radio_setup( );
    if( err == LGW_HAL_ERROR )
    {
        ESP_LOGE( TAG_HAL, "ERROR: FAILED TO SETUP RADIO\n" );
        return LGW_HAL_ERROR;
    }

    /* Update RX status */
    rx_status = RX_OFF;

    /* Set RX */
    lgw_radio_set_rx( &lgw_ral, rxrf_conf.freq_hz, rxif_conf.datarate, rxif_conf.bandwidth, rxif_conf.coderate );

    /* Update RX status */
    rx_status = RX_ON;

    /* Update TX status */
    if( rxrf_conf.tx_enable == false )
    {
        tx_status = TX_OFF;
    }
    else
    {
        tx_status = TX_FREE;
    }

    /* set hal state */
    is_started = true;

    return LGW_HAL_SUCCESS;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

int lgw_stop( void )
{
    if( is_started == false )
    {
        ESP_LOGI( TAG_HAL, "Note: LoRa concentrator was not started...\n" );
        return LGW_HAL_SUCCESS;
    }

    /* set hal state */
    is_started = false;

    return LGW_HAL_SUCCESS;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

int lgw_receive( uint8_t max_pkt, struct lgw_pkt_rx_s* pkt_data )
{
    struct lgw_pkt_rx_s* p = &pkt_data[0];
    uint32_t             count_us;
    int8_t               rssi, snr;
    uint8_t              status;
    uint16_t             size;
    bool                 irq_received;
    int                  nb_packet_received = 0;

    /* max_pkt is ignored, only 1 packet can be received at a time */

    /* check if the concentrator is running */
    if( is_started == false )
    {
        ESP_LOGE( TAG_HAL, "ERROR: CONCENTRATOR IS NOT RUNNING, START IT BEFORE RECEIVING\n" );
        return LGW_HAL_ERROR;
    }

    memset( p, 0, sizeof( struct lgw_pkt_rx_s ) );
    nb_packet_received =
        lgw_radio_get_pkt( &lgw_ral, &irq_received, &count_us, &rssi, &snr, &status, &size, p->payload );
    if( nb_packet_received > 0 )
    {
        p->count_us   = count_us;
        p->freq_hz    = rxrf_conf.freq_hz;
        p->if_chain   = 0;
        p->rf_chain   = 0;
        p->status     = status;
        p->modulation = rxif_conf.modulation;
        p->datarate   = rxif_conf.datarate;
        p->bandwidth  = rxif_conf.bandwidth;
        p->coderate   = rxif_conf.coderate;
        p->rssic      = ( float ) rssi;
        p->snr        = ( float ) snr;
        p->size       = size;

        /* Compensate timestamp with for radio processing delay */
        uint32_t count_us_correction = lgw_radio_timestamp_correction( rxif_conf.datarate, rxif_conf.bandwidth );
        ESP_LOGI( TAG_HAL, "count_us correction: %lu us", count_us_correction );
        p->count_us -= count_us_correction;
    }

    if( irq_received == true )
    {
        /* Reconfigure RX */
        lgw_radio_set_rx( &lgw_ral, rxrf_conf.freq_hz, rxif_conf.datarate, rxif_conf.bandwidth, rxif_conf.coderate );
    }

    return nb_packet_received;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

int lgw_send( struct lgw_pkt_tx_s* pkt_data )
{
    /* check if the concentrator is running */
    if( is_started == false )
    {
        ESP_LOGE( TAG_HAL, "ERROR: CONCENTRATOR IS NOT RUNNING, START IT BEFORE SENDING\n" );
        return LGW_HAL_ERROR;
    }

    /* Update RX status */
    rx_status = RX_SUSPENDED;

    /* Configure for TX */
    lgw_radio_configure_tx( &lgw_ral, pkt_data );

    /* Update TX status */
    tx_status = TX_SCHEDULED;

    /* Get TCXO startup time, if any */
    uint32_t tcxo_startup_time_in_tick = 0;
#if defined( CONFIG_RADIO_TYPE_SX1261 ) || defined( CONFIG_RADIO_TYPE_SX1262 ) || defined( CONFIG_RADIO_TYPE_SX1268 )
    ral_sx126x_bsp_get_xosc_cfg( NULL, NULL, NULL, &tcxo_startup_time_in_tick );
#elif defined( CONFIG_RADIO_TYPE_LLCC68 )
    ral_llcc68_bsp_get_xosc_cfg( NULL, NULL, NULL, &tcxo_startup_time_in_tick );
#elif defined( CONFIG_RADIO_TYPE_LR1121 )
    ral_lr11xx_bsp_get_xosc_cfg( NULL, NULL, NULL, &tcxo_startup_time_in_tick );
#endif
    uint32_t tcxo_startup_time_us = tcxo_startup_time_in_tick * 15625 / 1000;

    /* Wait for time to send packet */
    uint32_t count_us_now;
    do
    {
        lgw_get_instcnt( &count_us_now );
        WAIT_US( 100 );
    } while( ( int32_t )( pkt_data->count_us - count_us_now ) > ( int32_t ) tcxo_startup_time_us );

    /* Send packet */
    ASSERT_RAL_RC( ral_set_tx( &lgw_ral ) );

    /* Update TX status */
    tx_status = TX_EMITTING;

    /* Wait for TX_DONE */
    bool      flag_tx_done    = false;
    bool      flag_tx_timeout = false;
    ral_irq_t irq_regs;
    do
    {
        ASSERT_RAL_RC( ral_get_and_clear_irq_status( &lgw_ral, &irq_regs ) );
        if( ( irq_regs & RAL_IRQ_TX_DONE ) == RAL_IRQ_TX_DONE )
        {
            lgw_get_instcnt( &count_us_now );
            printf( "%lu: IRQ_TX_DONE\n", count_us_now );
            flag_tx_done = true;
        }
        if( ( irq_regs & RAL_IRQ_RX_TIMEOUT ) == RAL_IRQ_RX_TIMEOUT )
        {  // TODO: check if IRQ also valid for TX
            lgw_get_instcnt( &count_us_now );
            ESP_LOGW( TAG_HAL, "%lu: TX:IRQ_TIMEOUT\n", count_us_now );
            flag_tx_timeout = true;
        }

        /* Yield for 10ms (avoid TWDT watchdog timeout) for long TX */
        vTaskDelay( pdMS_TO_TICKS( 10 ) );
    } while( ( flag_tx_done == false ) && ( flag_tx_timeout == false ) );

    /* Update TX status */
    tx_status = TX_FREE;

    /* Back to RX config */
    lgw_radio_set_rx( &lgw_ral, rxrf_conf.freq_hz, rxif_conf.datarate, rxif_conf.bandwidth, rxif_conf.coderate );

    /* Update RX status */
    rx_status = RX_ON;

    return ( flag_tx_timeout == false ) ? LGW_HAL_SUCCESS : LGW_HAL_ERROR;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

int lgw_status( uint8_t rf_chain, uint8_t select, uint8_t* code )
{
    // ESP_LOGI(TAG_HAL, "lgw_status()");

    /* check input variables */
    CHECK_NULL( code );
    if( rf_chain >= LGW_RF_CHAIN_NB )
    {
        ESP_LOGE( TAG_HAL, "ERROR: NOT A VALID RF_CHAIN NUMBER\n" );
        return LGW_HAL_ERROR;
    }

    /* Get status */
    if( select == TX_STATUS )
    {
        if( is_started == false )
        {
            *code = TX_OFF;
        }
        else
        {
            *code = tx_status;
        }
    }
    else if( select == RX_STATUS )
    {
        if( is_started == false )
        {
            *code = RX_OFF;
        }
        else
        {
            *code = rx_status;
        }
    }
    else
    {
        ESP_LOGE( TAG_HAL, "ERROR: SELECTION INVALID, NO STATUS TO RETURN\n" );
        return LGW_HAL_ERROR;
    }

    return LGW_HAL_SUCCESS;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

int lgw_get_instcnt( uint32_t* inst_cnt_us )
{
    int64_t count_us_64 = esp_timer_get_time( );

    *inst_cnt_us = ( uint32_t ) count_us_64;

    return LGW_HAL_SUCCESS;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

uint32_t lgw_time_on_air( const struct lgw_pkt_tx_s* packet )
{
    uint32_t toa_ms, toa_us;

    if( packet == NULL )
    {
        ESP_LOGE( TAG_HAL, "ERROR: Failed to compute time on air, wrong parameter\n" );
        return 0;
    }

    if( packet->modulation == MOD_LORA )
    {
        toa_us = lora_packet_time_on_air( packet->bandwidth, packet->datarate, packet->coderate, packet->preamble,
                                          packet->no_header, packet->no_crc, packet->size, NULL, NULL, NULL );
        toa_ms = ( uint32_t )( ( double ) toa_us / 1000.0 + 0.5 );
    }
    else
    {
        toa_ms = 0;
        ESP_LOGE( TAG_HAL, "ERROR: Cannot compute time on air for this packet, unsupported modulation (0x%02X)\n",
                  packet->modulation );
    }

    return toa_ms;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

void lgw_get_min_max_freq_hz( uint32_t* min_freq_hz, uint32_t* max_freq_hz )
{
#if defined( CONFIG_RADIO_TYPE_SX1261 ) || defined( CONFIG_RADIO_TYPE_SX1262 ) || defined( CONFIG_RADIO_TYPE_SX1268 )
    const smtc_shield_sx126x_t*              shield              = ral_sx126x_get_shield( );
    const smtc_shield_sx126x_capabilities_t* shield_capabilities = shield->get_capabilities( );
#elif defined( CONFIG_RADIO_TYPE_LLCC68 )
    const smtc_shield_llcc68_t*              shield              = ral_llcc68_get_shield( );
    const smtc_shield_llcc68_capabilities_t* shield_capabilities = shield->get_capabilities( );
#elif defined( CONFIG_RADIO_TYPE_LR1121 )
    const smtc_shield_lr11xx_t*              shield              = ral_lr11xx_get_shield( );
    const smtc_shield_lr11xx_capabilities_t* shield_capabilities = shield->get_capabilities( );
#endif
    *min_freq_hz = shield_capabilities->freq_hz_min;
    *max_freq_hz = shield_capabilities->freq_hz_max;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

void lgw_get_min_max_power_dbm( int8_t* min_power_dbm, int8_t* max_power_dbm )
{
#if defined( CONFIG_RADIO_TYPE_SX1261 ) || defined( CONFIG_RADIO_TYPE_SX1262 ) || defined( CONFIG_RADIO_TYPE_SX1268 )
    const smtc_shield_sx126x_t*              shield              = ral_sx126x_get_shield( );
    const smtc_shield_sx126x_capabilities_t* shield_capabilities = shield->get_capabilities( );
#elif defined( CONFIG_RADIO_TYPE_LLCC68 )
    const smtc_shield_llcc68_t*              shield              = ral_llcc68_get_shield( );
    const smtc_shield_llcc68_capabilities_t* shield_capabilities = shield->get_capabilities( );
#elif defined( CONFIG_RADIO_TYPE_LR1121 )
    const smtc_shield_lr11xx_t*              shield              = ral_lr11xx_get_shield( );
    const smtc_shield_lr11xx_capabilities_t* shield_capabilities = shield->get_capabilities( );
#endif
    *min_power_dbm = shield_capabilities->power_dbm_min;
    *max_power_dbm = shield_capabilities->power_dbm_max;
}

/* --- EOF ------------------------------------------------------------------ */