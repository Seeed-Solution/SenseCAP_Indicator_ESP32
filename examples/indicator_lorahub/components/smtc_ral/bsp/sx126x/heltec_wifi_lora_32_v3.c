/*!
 * @file      heltec_wifi_lora_32_v3.c
 *
 * @brief     Implementation specific to Heltec WiFi LoRa 32 v3 board.
 *              Same definition than SX1262MB1DAS shield from SWSD003 project.
 *
 * The Clear BSD License
 * Copyright Semtech Corporation 2022. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Semtech corporation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "smtc_shield_sx126x_types.h"

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE MACROS-----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE CONSTANTS -------------------------------------------------------
 */

const smtc_shield_sx126x_pa_pwr_cfg_t smtc_shield_sx1262mb1das_pa_pwr_cfg_table[SMTC_SHIELD_SX1262_MAX_PWR - SMTC_SHIELD_SX1262_MIN_PWR + 1] = {
    {   // Expected output power = -9dBm
        .power = 3,
        .pa_config = {
            .hp_max        = 0x01,
            .pa_duty_cycle = 0x00,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = -8dBm
        .power = 4,
        .pa_config = {
            .hp_max        = 0x01,
            .pa_duty_cycle = 0x00,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = -7dBm
        .power = 6,
        .pa_config = {
            .hp_max        = 0x01,
            .pa_duty_cycle = 0x00,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = -6dBm
        .power = 7,
        .pa_config = {
            .hp_max        = 0x01,
            .pa_duty_cycle = 0x00,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = -5dBm
        .power = 7,
        .pa_config = {
            .hp_max        = 0x01,
            .pa_duty_cycle = 0x01,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = -4dBm
        .power = 9,
        .pa_config = {
            .hp_max        = 0x01,
            .pa_duty_cycle = 0x00,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = -3dBm
        .power = 10,
        .pa_config = {
            .hp_max        = 0x01,
            .pa_duty_cycle = 0x00,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = -2dBm
        .power = 11,
        .pa_config = {
            .hp_max        = 0x01,
            .pa_duty_cycle = 0x00,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = -1dBm
        .power = 12,
        .pa_config = {
            .hp_max        = 0x01,
            .pa_duty_cycle = 0x00,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 0dBm
        .power = 13,
        .pa_config = {
            .hp_max        = 0x01,
            .pa_duty_cycle = 0x00,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 1dBm
        .power = 14,
        .pa_config = {
            .hp_max        = 0x01,
            .pa_duty_cycle = 0x01,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 2dBm
        .power = 20,
        .pa_config = {
            .hp_max        = 0x01,
            .pa_duty_cycle = 0x00,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 3dBm
        .power = 19,
        .pa_config = {
            .hp_max        = 0x01,
            .pa_duty_cycle = 0x02,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 4dBm
        .power = 20,
        .pa_config = {
            .hp_max        = 0x01,
            .pa_duty_cycle = 0x01,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 5dBm
        .power = 21,
        .pa_config = {
            .hp_max        = 0x01,
            .pa_duty_cycle = 0x00,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 6dBm
        .power = 22,
        .pa_config = {
            .hp_max        = 0x01,
            .pa_duty_cycle = 0x00,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 7dBm
        .power = 22,
        .pa_config = {
            .hp_max        = 0x01,
            .pa_duty_cycle = 0x01,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 8dBm
        .power = 22,
        .pa_config = {
            .hp_max        = 0x01,
            .pa_duty_cycle = 0x02,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 9dBm
        .power = 22,
        .pa_config = {
            .hp_max        = 0x01,
            .pa_duty_cycle = 0x03,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 10dBm
        .power = 21,
        .pa_config = {
            .hp_max        = 0x02,
            .pa_duty_cycle = 0x00,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 11dBm
        .power = 22,
        .pa_config = {
            .hp_max        = 0x02,
            .pa_duty_cycle = 0x00,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 12dBm
        .power = 22,
        .pa_config = {
            .hp_max        = 0x02,
            .pa_duty_cycle = 0x01,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 13dBm
        .power = 22,
        .pa_config = {
            .hp_max        = 0x02,
            .pa_duty_cycle = 0x02,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 14dBm
        .power = 22,
        .pa_config = {
            .hp_max        = 0x02,
            .pa_duty_cycle = 0x03,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 15dBm
        .power = 22,
        .pa_config = {
            .hp_max        = 0x03,
            .pa_duty_cycle = 0x01,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 16dBm
        .power = 22,
        .pa_config = {
            .hp_max        = 0x03,
            .pa_duty_cycle = 0x02,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 17dBm
        .power = 22,
        .pa_config = {
            .hp_max        = 0x05,
            .pa_duty_cycle = 0x00,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 18dBm
        .power = 22,
        .pa_config = {
            .hp_max        = 0x05,
            .pa_duty_cycle = 0x01,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 19dBm
        .power = 22,
        .pa_config = {
            .hp_max        = 0x05,
            .pa_duty_cycle = 0x02,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 20dBm
        .power = 22,
        .pa_config = {
            .hp_max        = 0x07,
            .pa_duty_cycle = 0x02,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 21dBm
        .power = 22,
        .pa_config = {
            .hp_max        = 0x07,
            .pa_duty_cycle = 0x03,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
    { // Expected output power = 22dBm
        .power = 22,
        .pa_config = {
            .hp_max        = 0x07,
            .pa_duty_cycle = 0x04,
            .device_sel    = 0x00,
            .pa_lut        = 0x01,
        },
    },
};

/**
 * @brief XOSC configuration
 */
const smtc_shield_sx126x_xosc_cfg_t smtc_shield_sx1262mb1das_xosc_cfg = {
    .tcxo_is_radio_controlled = true,
    .supply_voltage           = SX126X_TCXO_CTRL_3_0V,
    .startup_time_in_tick     = 300,
};

/**
 * @brief GPIO configuration
 * ESP32-S3 chip features 45 physical GPIO pins (GPIO0 ~ GPIO21 and GPIO26 ~ GPIO48)
 */
const smtc_shield_sx126x_pinout_t smtc_shield_sx1262mb1das_pinout = {
    .nss   = 8,  /* GPIO8 */
    .sclk  = 9,  /* GPIO9 */
    .mosi  = 10, /* GPIO10 */
    .miso  = 11, /* GPIO11 */
    .reset = 12, /* GPIO12 */
    .busy  = 13, /* GPIO13 */
    .irq   = 14,
    /* GPIO14 */        /* DIO1 */
    .antenna_sw = 0xFF, /* NOT USED */
    .led_tx     = 0xFF, /* NOT USED */
    .led_rx     = 0xFF, /* NOT USED */
};

/**
 * @brief Board capabilities
 */
const smtc_shield_sx126x_capabilities_t smtc_shield_sx1262mb1das_capabilities = {
    .freq_hz_min   = SMTC_SHIELD_SX126X_FREQ_MIN,
    .freq_hz_max   = SMTC_SHIELD_SX126X_FREQ_MAX,
    .power_dbm_min = SMTC_SHIELD_SX1261_MIN_PWR,
    .power_dbm_max = SMTC_SHIELD_SX1261_MAX_PWR,
};

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE TYPES -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE VARIABLES -------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DECLARATION -------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC VARIABLES --------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS DEFINITION ---------------------------------------------
 */

const smtc_shield_sx126x_pa_pwr_cfg_t* smtc_shield_sx1262mb1das_get_pa_pwr_cfg(
    const uint32_t rf_freq_in_hz, const int8_t expected_output_pwr_in_dbm )
{
    if( ( SMTC_SHIELD_SX126X_FREQ_MIN <= rf_freq_in_hz ) && ( rf_freq_in_hz <= SMTC_SHIELD_SX126X_FREQ_MAX ) )
    {
        if( ( SMTC_SHIELD_SX1262_MIN_PWR <= expected_output_pwr_in_dbm ) &&
            ( expected_output_pwr_in_dbm <= SMTC_SHIELD_SX1262_MAX_PWR ) )
        {
            return &(
                smtc_shield_sx1262mb1das_pa_pwr_cfg_table[expected_output_pwr_in_dbm - SMTC_SHIELD_SX1262_MIN_PWR] );
        }
    }

    return NULL;
}

bool smtc_shield_sx1262mb1das_is_dio2_set_as_rf_switch( void )
{
    return true;
}

sx126x_reg_mod_t smtc_shield_sx1262mb1das_get_reg_mode( void )
{
    return SX126X_REG_MODE_LDO;
}

const smtc_shield_sx126x_xosc_cfg_t* smtc_shield_sx1262mb1das_get_xosc_cfg( void )
{
    return &smtc_shield_sx1262mb1das_xosc_cfg;
}

const smtc_shield_sx126x_pinout_t* smtc_shield_sx1262mb1das_get_pinout( void )
{
    return &smtc_shield_sx1262mb1das_pinout;
}

const smtc_shield_sx126x_capabilities_t* smtc_shield_sx1262mb1das_get_capabilities( void )
{
    return &smtc_shield_sx1262mb1das_capabilities;
}

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DEFINITION --------------------------------------------
 */

/* --- EOF ------------------------------------------------------------------ */
