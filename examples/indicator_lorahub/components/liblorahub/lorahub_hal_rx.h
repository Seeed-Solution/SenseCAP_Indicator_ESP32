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

#ifndef _LORAHUB_HAL_RX_H
#define _LORAHUB_HAL_RX_H

/* -------------------------------------------------------------------------- */
/* --- DEPENDANCIES --------------------------------------------------------- */

#include "ral.h"

/* -------------------------------------------------------------------------- */
/* --- PUBLIC MACROS -------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PUBLIC CONSTANTS ----------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PUBLIC TYPES --------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PUBLIC FUNCTIONS PROTOTYPES ------------------------------------------ */

int lgw_radio_init_rx( const ral_t* ral );

int lgw_radio_set_rx( const ral_t* ral, uint32_t freq_hz, uint32_t datarate, uint8_t bandwidth, uint8_t coderate );

int lgw_radio_get_pkt( const ral_t* ral, bool* irq_received, uint32_t* count_us, int8_t* rssi, int8_t* snr,
                       uint8_t* status, uint16_t* size, uint8_t* payload );

#endif  // _LORAHUB_HAL_RX_H

/* --- EOF ------------------------------------------------------------------ */