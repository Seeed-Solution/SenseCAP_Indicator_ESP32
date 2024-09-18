/* 
   _____                      _________    ____
  / ___/___  ____  ________  / ____/   |  / __ \
  \__ \/ _ \/ __ \/ ___/ _ \/ /   / /| | / /_/ /
 ___/ /  __/ / / (__  )  __/ /___/ ___ |/ ____/
/____/\___/_/ /_/____/\___/\____/_/  |_/_/
  (C)2024 Seeed Studio

Description:
    LoRaHub configuration helpers

License: Revised BSD License, see LICENSE.TXT file include in the project
*/

#ifndef _CONFIG_NVS_H
#define _CONFIG_NVS_H

/* -------------------------------------------------------------------------- */
/* --- DEPENDANCIES --------------------------------------------------------- */

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