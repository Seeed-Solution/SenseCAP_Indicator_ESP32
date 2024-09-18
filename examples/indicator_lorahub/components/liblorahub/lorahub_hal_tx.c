/*______                              _
/ _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
\____ \| ___ |    (_   _) ___ |/ ___)  _ \
_____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
(C)2024 Semtech

Description:
    LoRaHub Hardware Abstraction Layer - TX

License: Revised BSD License, see LICENSE.TXT file include in the project
*/

/* -------------------------------------------------------------------------- */
/* --- DEPENDANCIES --------------------------------------------------------- */

#include <string.h>

#include "lorahub_aux.h"
#include "lorahub_hal.h"
#include "lorahub_hal_tx.h"

#include "ral.h"
#include "radio_context.h"

/* -------------------------------------------------------------------------- */
/* --- PRIVATE MACROS -------------------------------------------------------- */

#define SET_PPM_ON( bw, dr )                                                             \
    ( ( ( bw == BW_125KHZ ) && ( ( dr == DR_LORA_SF11 ) || ( dr == DR_LORA_SF12 ) ) ) || \
      ( ( bw == BW_250KHZ ) && ( dr == DR_LORA_SF12 ) ) )

/* -------------------------------------------------------------------------- */
/* --- PRIVATE CONSTANTS ----------------------------------------------------- */

static const char* TAG_HAL_TX = "HAL_TX";

/* -------------------------------------------------------------------------- */
/* --- PRIVATE TYPES --------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PRIVATE VARIABLES ---------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PRIVATE FUNCTIONS DECLARATION ---------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PRIVATE FUNCTIONS DEFINITION ----------------------------------------- */

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

int lgw_radio_configure_tx( const ral_t* ral, struct lgw_pkt_tx_s* pkt_data )
{
    set_led_rx( ral, false );
    set_led_tx( ral, true );

    /* Configure for TX */
    ASSERT_RAL_RC( ral_set_standby( ral, RAL_STANDBY_CFG_RC ) );

    ral_pkt_type_t pkt_type = RAL_PKT_TYPE_LORA;
    ASSERT_RAL_RC( ral_set_pkt_type( ral, pkt_type ) );
    ASSERT_RAL_RC( ral_set_rf_freq( ral, pkt_data->freq_hz ) );
    ASSERT_RAL_RC( ral_set_tx_cfg( ral, pkt_data->rf_power, pkt_data->freq_hz ) );

    if( pkt_type == RAL_PKT_TYPE_LORA )
    {
        ral_lora_sf_t ral_dr;
        switch( pkt_data->datarate )
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
            ESP_LOGW( TAG_HAL_TX, "datarate %lu not supported", pkt_data->datarate );
            return LGW_HAL_ERROR;
        }
        ral_lora_bw_t ral_bw;
        switch( pkt_data->bandwidth )
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
            ESP_LOGW( TAG_HAL_TX, "bandwidth %u not supported", pkt_data->bandwidth );
            return LGW_HAL_ERROR;
        }
        ral_lora_cr_t ral_cr;
        switch( pkt_data->coderate )
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
            ESP_LOGW( TAG_HAL_TX, "coderate %u not supported", pkt_data->coderate );
            return LGW_HAL_ERROR;
        }
        ral_lora_mod_params_t lora_mod_params = {
            .sf = ral_dr, .bw = ral_bw, .cr = ral_cr, .ldro = SET_PPM_ON( pkt_data->bandwidth, pkt_data->datarate )
        };
        ASSERT_RAL_RC( ral_set_lora_mod_params( ral, &lora_mod_params ) );

        const ral_lora_pkt_params_t lora_pkt_params = {
            .preamble_len_in_symb = pkt_data->preamble,
            .header_type          = ( pkt_data->no_header ) ? RAL_LORA_PKT_IMPLICIT : RAL_LORA_PKT_EXPLICIT,
            .pld_len_in_bytes     = pkt_data->size,
            .crc_is_on            = !pkt_data->no_crc,
            .invert_iq_is_on      = pkt_data->invert_pol,
        };
        ASSERT_RAL_RC( ral_set_lora_pkt_params( ral, &lora_pkt_params ) );
    }
    else
    {
        ESP_LOGE( TAG_HAL_TX, "ERROR: packet type not supported" );
        return LGW_HAL_ERROR;
    }

    const ral_irq_t tx_irq_mask = RAL_IRQ_TX_DONE | RAL_IRQ_RX_TIMEOUT;
    ASSERT_RAL_RC( ral_set_dio_irq_params( ral, tx_irq_mask ) );
    ASSERT_RAL_RC( ral_clear_irq_status( ral, RAL_IRQ_ALL ) );

    ASSERT_RAL_RC( ral_set_pkt_payload( ral, pkt_data->payload, pkt_data->size ) );

    return LGW_HAL_SUCCESS;
}

/* --- EOF ------------------------------------------------------------------ */