/*______                              _
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
  (C)2024 Semtech

Description:
    LoRaHub configuration helpers

License: Revised BSD License, see LICENSE.TXT file include in the project
*/

#ifndef _CONFIG_NVS_H
#define _CONFIG_NVS_H

/* -------------------------------------------------------------------------- */
/* --- DEPENDENCIES --------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PUBLIC MACROS -------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PUBLIC CONSTANTS ----------------------------------------------------- */

#define CFG_NVS_KEY_STR_MAX_SIZE ( 16 )

/* Size of the following string must be < CFG_NVS_KEY_STR_MAX_SIZE */
#define CFG_NVS_KEY_LNS_ADDRESS "lns_addr"
#define CFG_NVS_KEY_LNS_PORT "lns_port"
#define CFG_NVS_KEY_CHAN_FREQ "chan_freq"
#define CFG_NVS_KEY_CHAN_DR "chan_dr"
#define CFG_NVS_KEY_CHAN_BW "chan_bw"
#define CFG_NVS_KEY_SNTP_ADDRESS "sntp_addr"

/* -------------------------------------------------------------------------- */
/* --- PUBLIC TYPES --------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PUBLIC FUNCTIONS PROTOTYPES ------------------------------------------ */

#endif  // _CONFIG_NVS_H

/* --- EOF ------------------------------------------------------------------ */