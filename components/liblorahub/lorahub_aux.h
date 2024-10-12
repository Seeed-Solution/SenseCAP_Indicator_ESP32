/*______                              _
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
  (C)2024 Semtech

Description:
    LoRaHub HAL common auxiliary functions

License: Revised BSD License, see LICENSE.TXT file include in the project
*/

#ifndef _LORAHUB_AUX_H
#define _LORAHUB_AUX_H

/* -------------------------------------------------------------------------- */
/* --- DEPENDENCIES --------------------------------------------------------- */

#include <stdint.h>   /* C99 types */
#include <stdbool.h>  /* bool type */
#include <sys/time.h> /* gettimeofday, structtimeval */

#include "esp_rom_sys.h"
#include "esp_log.h"

/* -------------------------------------------------------------------------- */
/* --- PUBLIC CONSTANTS ----------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PUBLIC MACROS -------------------------------------------------------- */

#define MIN( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#define MAX( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )

/**
@brief Get a particular bit value from a byte
@param b [in]   Any byte from which we want a bit value
@param p [in]   Position of the bit in the byte [0..7]
@param n [in]   Number of bits we want to get
@return The value corresponding the requested bits
*/
#define TAKE_N_BITS_FROM( b, p, n ) ( ( ( b ) >> ( p ) ) & ( ( 1 << ( n ) ) - 1 ) )

/**
@brief Wait for a certain time (microsecond accuracy)
@param us number of microseconds to wait.
*/
#define WAIT_US( us ) esp_rom_delay_us( us )

/**
@brief Wait for a certain time (millisecond accuracy)
@param ms number of milliseconds to wait.
*/
#define WAIT_MS( ms ) esp_rom_delay_us( ms * 1000 )

/*!
 * @brief Stringify constants
 */
#define xstr( a ) str( a )
#define str( a ) #a

/*!
 * @brief Helper macro that returned a human-friendly message if a command does not return RAL_STATUS_OK
 *
 * @remark The macro is implemented to be used with functions returning a @ref ral_status_t
 *
 * @param[in] rc  Return code
 */
#define ASSERT_RAL_RC( rc )                                                                                     \
    {                                                                                                           \
        const ral_status_t status = rc;                                                                         \
        if( status != RAL_STATUS_OK )                                                                           \
        {                                                                                                       \
            if( status == RAL_STATUS_ERROR )                                                                    \
            {                                                                                                   \
                ESP_LOGE( "RAL_STATUS", "In %s - %s (line %d): %s", __FILE__, __func__, __LINE__,               \
                          xstr( RAL_STATUS_ERROR ) );                                                           \
                return -1;                                                                                      \
            }                                                                                                   \
            else                                                                                                \
            {                                                                                                   \
                ESP_LOGW( "RAL_STATUS", "In %s - %s (line %d): Status code = %d", __FILE__, __func__, __LINE__, \
                          status );                                                                             \
                return -1;                                                                                      \
            }                                                                                                   \
        }                                                                                                       \
    }

/* -------------------------------------------------------------------------- */
/* --- PUBLIC FUNCTIONS PROTOTYPES ------------------------------------------ */

/**
@brief Calculate the time on air of a LoRa packet in microseconds
@param bw packet bandwidth
@param sf packet spreading factor
@param cr packet coding rate
@param n_symbol_preamble packet preamble length (number of symbols)
@param no_header true if packet has no header
@param no_crc true if packet has no CRC
@param size packet size in bytes
@param nb_symbols pointer to return the total number of symbols in packet
@param nb_symbols_payload pointer to return the number of symbols in packet payload
@param t_symbol_us pointer to return the duration of a symbol in microseconds
@return the packet time on air in microseconds
*/
uint32_t lora_packet_time_on_air( const uint8_t bw, const uint8_t sf, const uint8_t cr,
                                  const uint16_t n_symbol_preamble, const bool no_header, const bool no_crc,
                                  const uint8_t size, double* nb_symbols, uint32_t* nb_symbols_payload,
                                  uint16_t* t_symbol_us );

#endif

/* --- EOF ------------------------------------------------------------------ */
