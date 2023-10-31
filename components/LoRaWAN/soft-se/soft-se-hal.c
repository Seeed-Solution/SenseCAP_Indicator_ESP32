/*!
 * \file      soft-se-hal.h
 *
 * \brief     Secure Element hardware abstraction layer implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2020 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 */
// #include "board.h"

#include "soft-se-hal.h"
#include "esp_mac.h"

void SoftSeHalGetUniqueId( uint8_t *id )
{
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);

    id[7] = mac[5];
    id[6] = mac[4];
    id[5] = mac[3];
    id[4] = mac[2];
    id[3] = mac[1];
    id[2] = mac[0];
    id[1] = 0;
    id[0] = 0;
}
