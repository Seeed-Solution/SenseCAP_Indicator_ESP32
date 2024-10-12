/**
 * @file      ral_lr11xx_bsp.h
 *
 * @brief     Board Support Package for the LR11XX-specific RAL.
 *
 * The Clear BSD License
 * Copyright Semtech Corporation 2021. All rights reserved.
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

#ifndef RAL_LR11XX_BSP_H
#define RAL_LR11XX_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <stdint.h>
#include "ral_defs.h"
#include "lr11xx_radio_types.h"
#include "lr11xx_system_types.h"

#include "smtc_shield_lr11xx.h"

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC MACROS -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC CONSTANTS --------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC TYPES ------------------------------------------------------------
 */

typedef struct ral_lr11xx_bsp_tx_cfg_input_params_s
{
    int8_t   system_output_pwr_in_dbm;
    uint32_t freq_in_hz;
} ral_lr11xx_bsp_tx_cfg_input_params_t;

typedef struct ral_lr11xx_bsp_tx_cfg_output_params_s
{
    lr11xx_radio_pa_cfg_t    pa_cfg;
    lr11xx_radio_ramp_time_t pa_ramp_time;
    int8_t                   chip_output_pwr_in_dbm_configured;
    int8_t                   chip_output_pwr_in_dbm_expected;
} ral_lr11xx_bsp_tx_cfg_output_params_t;

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS PROTOTYPES ---------------------------------------------
 */

const smtc_shield_lr11xx_t* ral_lr11xx_get_shield( void );

/**
 * Get the internal RF switch configuration to be applied to the chip
 *
 * @param [in] context Chip implementation context
 * @param [out] rf_switch_cfg RF switch configuration to be applied to the chip
 */
void ral_lr11xx_bsp_get_rf_switch_cfg( const void* context, lr11xx_system_rfswitch_cfg_t* rf_switch_cfg );

/**
 * Get the Tx-related configuration (power amplifier configuration, output power and ramp time) to be applied to the
 * chip
 *
 * @param [in] context Chip implementation context
 * @param [in] input_params Parameters used to compute the chip configuration
 * @param [out] output_params Parameters to be configured in the chip
 */
void ral_lr11xx_bsp_get_tx_cfg( const void* context, const ral_lr11xx_bsp_tx_cfg_input_params_t* input_params,
                                ral_lr11xx_bsp_tx_cfg_output_params_t* output_params );

/**
 * Get the regulator mode
 *
 * @param [in] context Chip implementation context
 * @param [out] reg_mode System regulator mode
 */
void ral_lr11xx_bsp_get_reg_mode( const void* context, lr11xx_system_reg_mode_t* reg_mode );

/**
 * Get the XOSC configuration
 *
 * @remark If no TCXO is present, this function should set tcxo_is_radio_controlled to false, and return.
 *
 * @param [in] context Chip implementation context
 * @param [out] xosc_cfg Let the caller know what kind of XOSC is used
 * @param [out] supply_voltage TCXO supply voltage parameter
 * @param [out] startup_time_in_tick TCXO setup time in clock tick
 */
void ral_lr11xx_bsp_get_xosc_cfg( const void* context, ral_xosc_cfg_t* xosc_cfg,
                                  lr11xx_system_tcxo_supply_voltage_t* supply_voltage, uint32_t* startup_time_in_tick );

/**
 * Get the CRC on SPI state
 *
 * @param [in] context Chip implementation context
 * @param [out] crc_is_activated Let the caller know if the CRC is activated
 */
void ral_lr11xx_bsp_get_crc_state( const void* context, bool* crc_is_activated );

/**
 * Get the RSSI calibration table
 *
 * @param [in] context Chip implementation context
 * @param [in] freq_in_hz RF frequency in Hertz
 * @param [out] rssi_calibration_table Pointer to a structure holding the RSSI calibration table
 */
void ral_lr11xx_bsp_get_rssi_calibration_table( const void* context, const uint32_t freq_in_hz,
                                                lr11xx_radio_rssi_calibration_table_t* rssi_calibration_table );

/**
 * @brief Get the Channel Activity Detection (CAD) DetPeak value
 *
 * @param [in] sf                       CAD LoRa spreading factor
 * @param [in] bw                       CAD LoRa bandwidth
 * @param [in] nb_symbol                CAD on number of symbols
 * @param [in, out] in_out_cad_det_peak  CAD DetPeak value proposed by the ral could be overwritten
 */
void ral_lr11xx_bsp_get_lora_cad_det_peak( ral_lora_sf_t sf, ral_lora_bw_t bw, ral_lora_cad_symbs_t nb_symbol,
                                           uint8_t* in_out_cad_det_peak );

/**
 * Get the Rx boost configuration
 *
 * @param [in] context Chip implementation context
 * @param [out] rx_boost_is_activated Let the caller know if the RX boosted mode is activated
 */
void ral_lr11xx_bsp_get_rx_boost_cfg( const void* context, bool* rx_boost_is_activated );

/**
 * Get the Low-Frequency Clock configuration in sleep mode
 *
 * @param [in] context Chip implementation context
 * @param [out] lfclk_is_running Let the caller know if the LFCLK has to be running while in sleep mode
 */
void ral_lr11xx_bsp_get_lfclk_cfg_in_sleep( const void* context, bool* lfclk_is_running );

/**
 * @brief Get the instantaneous power consumption for the given Tx configuration
 *
 * @param [in] tx_cfg The Tx configuration
 * @param [in] radio_reg_mode The regulator configuration
 * @param [out] pwr_consumption_in_ua The corresponding instantaneous power consumption
 * @return ral_status_t
 */
ral_status_t ral_lr11xx_bsp_get_instantaneous_tx_power_consumption( const ral_lr11xx_bsp_tx_cfg_output_params_t* tx_cfg,
                                                                    lr11xx_system_reg_mode_t radio_reg_mode,
                                                                    uint32_t*                pwr_consumption_in_ua );

/**
 * @brief Get the instantaneous power consumption for the given GFSK Rx configuration
 *
 * @param [in] radio_reg_mode The regulator configuration
 * @param [in] rx_boosted The Rx boosted configuration
 * @param [out] pwr_consumption_in_ua The corresponding instantaneous power consumption
 * @return ral_status_t
 */
ral_status_t ral_lr11xx_bsp_get_instantaneous_gfsk_rx_power_consumption( lr11xx_system_reg_mode_t radio_reg_mode,
                                                                         bool                     rx_boosted,
                                                                         uint32_t* pwr_consumption_in_ua );

/**
 * @brief Get the instantaneous power consumption for the given LoRa Rx configuration
 *
 * @param radio_reg_mode The regulator configuration
 * @param rx_boosted The Rx boosted configuration
 * @param pwr_consumption_in_ua The corresponding instantaneous power consumption
 * @return ral_status_t
 */
ral_status_t ral_lr11xx_bsp_get_instantaneous_lora_rx_power_consumption( lr11xx_system_reg_mode_t radio_reg_mode,
                                                                         bool                     rx_boosted,
                                                                         uint32_t* pwr_consumption_in_ua );
#ifdef __cplusplus
}
#endif

#endif  // RAL_LR11XX_BSP_H

/* --- EOF ------------------------------------------------------------------ */
