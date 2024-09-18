/*______                              _
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
(C)2024 Semtech

Description:
    LoRaHub Hardware Abstraction Layer - RX

License: Revised BSD License, see LICENSE.TXT file include in the project
*/

/* -------------------------------------------------------------------------- */
/* --- DEPENDANCIES --------------------------------------------------------- */

#include <string.h>

#include "lorahub_aux.h"
#include "lorahub_hal.h"
#include "lorahub_hal_rx.h"

#include "ral.h"
#include "radio_context.h"

/* -------------------------------------------------------------------------- */
/* --- PRIVATE MACROS -------------------------------------------------------- */

#define SET_PPM_ON( bw, dr )                                                             \
    ( ( ( bw == BW_125KHZ ) && ( ( dr == DR_LORA_SF11 ) || ( dr == DR_LORA_SF12 ) ) ) || \
      ( ( bw == BW_250KHZ ) && ( dr == DR_LORA_SF12 ) ) )

/* -------------------------------------------------------------------------- */
/* --- PRIVATE CONSTANTS ----------------------------------------------------- */

static const char* TAG_HAL_RX = "HAL_RX";

#define RX_TIMEOUT_MS 120000 /* 2 minutes */

/* -------------------------------------------------------------------------- */
/* --- PRIVATE TYPES --------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PRIVATE VARIABLES ---------------------------------------------------- */

static volatile bool irq_fired    = false;
static uint32_t      irq_count_us = 0;

static bool flag_rx_done      = false;
static bool flag_rx_crc_error = false;
static bool flag_rx_timeout   = false;

/* -------------------------------------------------------------------------- */
/* --- PRIVATE FUNCTIONS DECLARATION ---------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PRIVATE FUNCTIONS DEFINITION ----------------------------------------- */

static void IRAM_ATTR radio_on_dio_irq( void* args )
{
    irq_fired = true;
    lgw_get_instcnt( &irq_count_us );
}

void radio_irq_process( const ral_t* ral )
{
    if( irq_fired == true )
    {
        irq_fired = false;

        ral_irq_t irq_regs;
        ral_get_and_clear_irq_status( ral, &irq_regs );
        if( ( irq_regs & RAL_IRQ_RX_DONE ) == RAL_IRQ_RX_DONE )
        {
            // printf("%lu: IRQ_RX_DONE\n", irq_count_us);
            flag_rx_done = true;
        }

        if( ( irq_regs & RAL_IRQ_RX_CRC_ERROR ) == RAL_IRQ_RX_CRC_ERROR )
        {
            ESP_LOGW( TAG_HAL_RX, "%lu: IRQ_CRC_ERROR", irq_count_us );
            flag_rx_crc_error = true;
        }

        if( ( irq_regs & RAL_IRQ_RX_TIMEOUT ) == RAL_IRQ_RX_TIMEOUT )
        {
            ESP_LOGW( TAG_HAL_RX, "%lu: RX:IRQ_TIMEOUT", irq_count_us );
            flag_rx_timeout = true;
        }
    }
}

static void set_led_rx( const ral_t* ral, bool on )
{
    const radio_context_t* radio_context = ( const radio_context_t* ) ( ral->context );

    if( radio_context->gpio_led_rx != 0xFF )
    {
        gpio_set_level( radio_context->gpio_led_rx, ( on == true ) ? 1 : 0 );
    }
}

static void set_led_tx( const ral_t* ral, bool on )
{
    const radio_context_t* radio_context = ( const radio_context_t* ) ( ral->context );

    if( radio_context->gpio_led_tx != 0xFF )
    {
        gpio_set_level( radio_context->gpio_led_tx, ( on == true ) ? 1 : 0 );
    }
}

/* -------------------------------------------------------------------------- */
/* --- PUBLIC FUNCTIONS DEFINITION ------------------------------------------ */

int lgw_radio_init_rx( const ral_t* ral )
{
    const radio_context_t* radio_context = (const radio_context_t*) (ral->context);

    ESP_LOGI(TAG_HAL_RX, "lgw_radio_init_rx");

    gpio_install_isr_service( 0 );
    gpio_isr_handler_add( radio_context->gpio_dio1, radio_on_dio_irq, NULL );

    return LGW_HAL_SUCCESS;
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

int lgw_radio_set_rx( const ral_t* ral, uint32_t freq_hz, uint32_t datarate, uint8_t bandwidth, uint8_t coderate )
{
    set_led_rx( ral, false );
    set_led_tx( ral, false );

    ASSERT_RAL_RC( ral_set_standby( ral, RAL_STANDBY_CFG_RC ) );

    ral_pkt_type_t pkt_type = RAL_PKT_TYPE_LORA;
    ASSERT_RAL_RC( ral_set_pkt_type( ral, RAL_PKT_TYPE_LORA ) );

    if( pkt_type == RAL_PKT_TYPE_LORA )
    {
        ral_lora_sf_t ral_dr;
        switch( datarate )
        {
        case DR_LORA_SF5:
            ral_dr = RAL_LORA_SF5;
            break;
        case DR_LORA_SF6:
            ral_dr = RAL_LORA_SF6;
            break;
        case DR_LORA_SF7:
            ral_dr = RAL_LORA_SF7;
            break;
        case DR_LORA_SF8:
            ral_dr = RAL_LORA_SF8;
            break;
        case DR_LORA_SF9:
            ral_dr = RAL_LORA_SF9;
            break;
        case DR_LORA_SF10:
            ral_dr = RAL_LORA_SF10;
            break;
        case DR_LORA_SF11:
            ral_dr = RAL_LORA_SF11;
            break;
        case DR_LORA_SF12:
            ral_dr = RAL_LORA_SF12;
            break;
        default:
            ESP_LOGW( TAG_HAL_RX, "datarate %lu not supported", datarate );
            return LGW_HAL_ERROR;
        }
        ral_lora_bw_t ral_bw;
        switch( bandwidth )
        {
        case BW_125KHZ:
            ral_bw = RAL_LORA_BW_125_KHZ;
            break;
        case BW_250KHZ:
            ral_bw = RAL_LORA_BW_250_KHZ;
            break;
        case BW_500KHZ:
            ral_bw = RAL_LORA_BW_500_KHZ;
            break;
        default:
            ESP_LOGW( TAG_HAL_RX, "bandwidth %u not supported", bandwidth );
            return LGW_HAL_ERROR;
        }
        ral_lora_cr_t ral_cr;
        switch( coderate )
        {
        case CR_LORA_4_5:
            ral_cr = RAL_LORA_CR_4_5;
            break;
        case CR_LORA_4_6:
            ral_cr = RAL_LORA_CR_4_6;
            break;
        case CR_LORA_4_7:
            ral_cr = RAL_LORA_CR_4_7;
            break;
        case CR_LORA_4_8:
            ral_cr = RAL_LORA_CR_4_8;
            break;
        default:
            ESP_LOGW( TAG_HAL_RX, "coderate %u not supported", coderate );
            return LGW_HAL_ERROR;
        }

        ral_lora_mod_params_t lora_mod_params = {
            .sf = ral_dr, .bw = ral_bw, .cr = ral_cr, .ldro = SET_PPM_ON( bandwidth, datarate )
        };
        ASSERT_RAL_RC( ral_set_lora_mod_params( ral, &lora_mod_params ) );

        const ral_lora_pkt_params_t lora_pkt_params = {
            .preamble_len_in_symb = STD_LORA_PREAMBLE,
            .header_type          = RAL_LORA_PKT_EXPLICIT,
            .pld_len_in_bytes     = 0,
            .crc_is_on            = true,
            .invert_iq_is_on      = false,
        };

        ASSERT_RAL_RC( ral_set_lora_pkt_params( ral, &lora_pkt_params ) );
    }
    else
    {
        ESP_LOGE( TAG_HAL_RX, "ERROR: packet type not supported" );
        return LGW_HAL_ERROR;
    }

    const ral_irq_t rx_irq_mask = RAL_IRQ_RX_DONE | RAL_IRQ_RX_CRC_ERROR | RAL_IRQ_RX_TIMEOUT;
    ASSERT_RAL_RC( ral_set_dio_irq_params( ral, rx_irq_mask ) );
    ASSERT_RAL_RC( ral_clear_irq_status( ral, RAL_IRQ_ALL ) );

    /* Set RX */
    ASSERT_RAL_RC( ral_set_rf_freq( ral, freq_hz ) );
    ASSERT_RAL_RC( ral_set_lora_symb_nb_timeout( ral, 0 ) );
    ASSERT_RAL_RC( ral_set_rx( ral, RX_TIMEOUT_MS ) );

    return LGW_HAL_SUCCESS;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

int lgw_radio_get_pkt( const ral_t* ral, bool* irq_received, uint32_t* count_us, int8_t* rssi, int8_t* snr,
                       uint8_t* status, uint16_t* size, uint8_t* payload )
{
    int nb_pkt_received = 0;

    /* Initialize return values */
    *count_us     = 0;
    *rssi         = 0;
    *snr          = 0;
    *status       = STAT_UNDEFINED;
    *size         = 0;
    *irq_received = false;

    /* Check if a packet has been received */
    radio_irq_process( ral );
    if( ( flag_rx_done == true ) || ( flag_rx_crc_error == true ) )
    {
        set_led_rx( ral, true );

        *irq_received = true;
        *count_us     = irq_count_us;

        ral_lora_rx_pkt_status_t pkt_status_lora;
        ASSERT_RAL_RC( ral_get_lora_rx_pkt_status( ral, &pkt_status_lora ) );
        *rssi = pkt_status_lora.rssi_pkt_in_dbm;
        *snr  = pkt_status_lora.snr_pkt_in_db;

        if( flag_rx_crc_error == true )
        {
            *status = STAT_CRC_BAD;
        }
        else
        {
            *status = STAT_CRC_OK;

            /* Get packet payload */
            ASSERT_RAL_RC( ral_get_pkt_payload( ral, 256, payload, size ) );  // TODO: define for max payload size
#if 0
            ESP_LOGI(TAG_HAL_RX, "%d byte packet received:", *size);
            ESP_LOG_BUFFER_HEX_LEVEL(TAG_HAL_RX, payload, *size, ESP_LOG_INFO);
#endif
        }

        nb_pkt_received += 1;

        /* Update status */
        flag_rx_done      = false;
        flag_rx_crc_error = false;
    }
    else if( flag_rx_timeout == true )
    {
        *irq_received = true;

        /* Update status */
        flag_rx_timeout = false;
    }

    return nb_pkt_received;
}

/* --- EOF ------------------------------------------------------------------ */