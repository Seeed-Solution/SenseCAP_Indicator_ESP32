/*______                              _
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
  (C)2024 Semtech

Description:
    LoRaHub HAL auxiliary functions

License: Revised BSD License, see LICENSE.TXT file include in the project
*/

/* -------------------------------------------------------------------------- */
/* --- DEPENDANCIES --------------------------------------------------------- */

#include <math.h> /* pow, ceil */

#include "lorahub_aux.h"
#include "lorahub_hal.h"

/* -------------------------------------------------------------------------- */
/* --- PRIVATE MACROS ------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PRIVATE CONSTANTS ----------------------------------------------------- */

static const char* TAG_AUX = "LORAHUB_AUX";

/* -------------------------------------------------------------------------- */
/* --- PUBLIC FUNCTIONS DEFINITION ------------------------------------------ */

uint32_t lora_packet_time_on_air( const uint8_t bw, const uint8_t sf, const uint8_t cr,
                                  const uint16_t n_symbol_preamble, const bool no_header, const bool no_crc,
                                  const uint8_t size, double* out_nb_symbols, uint32_t* out_nb_symbols_payload,
                                  uint16_t* out_t_symbol_us )
{
    uint8_t  H, DE, n_bit_crc;
    uint8_t  bw_pow;
    uint16_t t_symbol_us;
    double   n_symbol;
    uint32_t toa_us, n_symbol_payload;

    /* Check input parameters */
    if( IS_LORA_DR( sf ) == false )
    {
        ESP_LOGE( TAG_AUX, "ERROR: wrong datarate - %s\n", __FUNCTION__ );
        return 0;
    }
    if( IS_LORA_BW( bw ) == false )
    {
        ESP_LOGE( TAG_AUX, "ERROR: wrong bandwidth - %s\n", __FUNCTION__ );
        return 0;
    }
    if( IS_LORA_CR( cr ) == false )
    {
        ESP_LOGE( TAG_AUX, "ERROR: wrong coding rate - %s\n", __FUNCTION__ );
        return 0;
    }

    /* Get bandwidth 125KHz divider*/
    switch( bw )
    {
    case BW_125KHZ:
        bw_pow = 1;
        break;
    case BW_250KHZ:
        bw_pow = 2;
        break;
    case BW_500KHZ:
        bw_pow = 4;
        break;
    default:
        ESP_LOGE( TAG_AUX, "ERROR: unsupported bandwidth 0x%02X (%s)\n", bw, __FUNCTION__ );
        return 0;
    }

    /* Duration of 1 symbol */
    t_symbol_us = ( 1 << sf ) * 8 / bw_pow; /* 2^SF / BW , in microseconds */

    /* Packet parameters */
    H         = ( no_header == false ) ? 1 : 0; /* header is always enabled, except for beacons */
    DE        = ( sf >= 11 ) ? 1 : 0;           /* Low datarate optimization enabled for SF11 and SF12 */
    n_bit_crc = ( no_crc == false ) ? 16 : 0;

    /* Number of symbols in the payload */
    n_symbol_payload =
        ceil( MAX( ( double ) ( 8 * size + n_bit_crc - 4 * sf + ( ( sf >= 7 ) ? 8 : 0 ) + 20 * H ), 0.0 ) /
              ( double ) ( 4 * ( sf - 2 * DE ) ) ) *
        ( cr + 4 ); /* Explicitely cast to double to keep precision of the division */

    /* number of symbols in packet */
    n_symbol = ( double ) n_symbol_preamble + ( ( sf >= 7 ) ? 4.25 : 6.25 ) + 8.0 + ( double ) n_symbol_payload;

    /* Duration of packet in microseconds */
    toa_us = ( uint32_t )( ( double ) n_symbol * ( double ) t_symbol_us );

    // ESP_LOGI(TAG_AUX, "INFO: LoRa packet ToA: %lu us (n_symbol:%f, t_symbol_us:%u)\n", toa_us, n_symbol,
    // t_symbol_us);

    /* Return details if required */
    if( out_nb_symbols != NULL )
    {
        *out_nb_symbols = n_symbol;
    }
    if( out_nb_symbols_payload != NULL )
    {
        *out_nb_symbols_payload = n_symbol_payload;
    }
    if( out_t_symbol_us != NULL )
    {
        *out_t_symbol_us = t_symbol_us;
    }

    return toa_us;
}

/* --- EOF ------------------------------------------------------------------ */
