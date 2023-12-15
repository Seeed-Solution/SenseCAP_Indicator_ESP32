#ifndef INDICATOR_LORAWAN_H
#define INDICATOR_LORAWAN_H

#include "config.h"
#include "view_data.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LORAWAN_BASIC_CFG_STORAGE   "lw-basic-cfg"

#define LORAWAN_NETWORK_INFO_STORAGE   "lw-nwk-info"

#define LORAWAN_AUTO_JOIN_FLAG_STORAGE   "lw-join-flag"

#define LORAWAN_LIGHT_FLAG_STORAGE   "lw-light-flag"


#define FIRMWARE_VERSION                              0x01000000 // 1.0.0.0

/*!
 * LoRaWAN Adaptive Data Rate
 *
 * \remark Please note that when ADR is enabled the end-device should be static
 */
#define LORAWAN_ADR_STATE                           LORAMAC_HANDLER_ADR_ON

/*!
 * Default datarate
 *
 * \remark Please note that LORAWAN_DEFAULT_DATARATE is used only when ADR is disabled 
 */
#define LORAWAN_DEFAULT_DATARATE                    DR_5

/*!
 * LoRaWAN confirmed messages
 */
#define LORAWAN_DEFAULT_CONFIRMED_MSG_STATE         LORAMAC_HANDLER_CONFIRMED_MSG

/*!
 * User application data buffer size
 */
#define LORAWAN_APP_DATA_BUFFER_MAX_SIZE            242

/*!
 * LoRaWAN ETSI duty cycle control enable/disable
 *
 * \remark Please note that ETSI mandates duty cycled transmissions. Use only for test purposes
 */
#define LORAWAN_DUTYCYCLE_ON                        true

/*!
 * LoRaWAN application port
 * @remark The allowed port range is from 1 up to 223. Other values are reserved.
 */
#define LORAWAN_APP_PORT                            2

/*!
 * Defines a random delay for application data transmission duty cycle. 1s,
 * value in [ms].
 */
#define APP_TX_DUTYCYCLE_RND                        2000

int indicator_lorawan_init(void);


void lorawan_network_info_printf(struct view_data_lorawan_network_info *p_info);

void lorawan_basic_cfg_printf(struct view_data_lorawan_basic_cfg *p_info);

#ifdef __cplusplus
}
#endif

#endif
