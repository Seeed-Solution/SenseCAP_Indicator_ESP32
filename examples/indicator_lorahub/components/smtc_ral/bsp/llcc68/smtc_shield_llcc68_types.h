/*!
 * \file      smtc_shield_llcc68_types.h
 *
 * \brief     Interface for types common to all SX126x-based shields
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
#ifndef SMTC_SHIELD_LLCC68_TYPES_H
#define SMTC_SHIELD_LLCC68_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <stdint.h>

#include <driver/gpio.h>

#include "llcc68.h"

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC MACROS -----------------------------------------------------------
 */

#define SMTC_SHIELD_LLCC68_FREQ_MIN 150000000
#define SMTC_SHIELD_LLCC68_FREQ_MAX 960000000

#define SMTC_SHIELD_LLCC68_MIN_PWR -9
#define SMTC_SHIELD_LLCC68_MAX_PWR 22

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC CONSTANTS --------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC TYPES ------------------------------------------------------------
 */

/**
 * @brief Power amplifier and output power configurations structure definition
 */
typedef struct smtc_shield_llcc68_pa_pwr_cfg_s
{
    int8_t                 power;
    llcc68_pa_cfg_params_t pa_config;
} smtc_shield_llcc68_pa_pwr_cfg_t;

/**
 * @brief External 32MHz oscillator configuration structure definition
 */
typedef struct smtc_shield_llcc68_xosc_cfg_s
{
    bool                        tcxo_is_radio_controlled;
    llcc68_tcxo_ctrl_voltages_t supply_voltage;
    uint32_t                    startup_time_in_tick;
} smtc_shield_llcc68_xosc_cfg_t;

/**
 * @brief Pinout structure definition
 */
typedef struct smtc_shield_llcc68_pinout_s
{
    gpio_num_t nss;
    gpio_num_t sclk;
    gpio_num_t mosi;
    gpio_num_t miso;
    gpio_num_t reset;
    gpio_num_t busy;
    gpio_num_t irq;
    gpio_num_t antenna_sw;
    gpio_num_t led_tx;
    gpio_num_t led_rx;
} smtc_shield_llcc68_pinout_t;

/**
 * @brief Capabilities structure definition
 */
typedef struct smtc_shield_llcc68_capabilities_s
{
    uint32_t freq_hz_min;
    uint32_t freq_hz_max;
    int8_t   power_dbm_min;
    int8_t   power_dbm_max;
} smtc_shield_llcc68_capabilities_t;

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS PROTOTYPES ---------------------------------------------
 */

#ifdef __cplusplus
}
#endif

#endif  // SMTC_SHIELD_LLCC68_TYPES_H
