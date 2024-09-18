/*!
 * \file      ral_lr11xx_bsp.c
 *
 * \brief     Implements the HAL functions for LR11XX
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

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <stdint.h>   // C99 types
#include <stdbool.h>  // bool type
#include <stddef.h>
#include <string.h>  // memcpy

#include <esp_log.h>
#include "sdkconfig.h"  // ESP32 project configuration

#include "ral_lr11xx_bsp.h"
#include "smtc_shield_lr11xx.h"

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE MACROS-----------------------------------------------------------
 */

#define SMTC_SHIELD_LR11XX_NONE_INSTANTIATE                                                                          \
    {                                                                                                                \
        .get_pa_pwr_cfg = NULL, .get_rssi_calibration_table = NULL, .get_rf_switch_cfg = NULL, .get_reg_mode = NULL, \
        .get_xosc_cfg = NULL, .get_lfclk_cfg = NULL, .get_pinout = NULL,                                             \
        .get_rttof_recommended_rx_tx_delay_indicator = NULL,                                                         \
    }

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE CONSTANTS -------------------------------------------------------
 */

static const char* TAG_BSP_LR11XX = "BSP_LR11XX";

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE TYPES -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE VARIABLES -------------------------------------------------------
 */

#if defined( CONFIG_SEMTECH_DEVKIT )
#if defined( CONFIG_RADIO_TYPE_LR1121 )
#include "smtc_shield_lr1121mb1dis.h"
smtc_shield_lr11xx_t shield = SMTC_SHIELD_LR1121MB1DIS_INSTANTIATE;
#else
smtc_shield_lr11xx_t shield = SMTC_SHIELD_LR11XX_NONE_INSTANTIATE;
#endif
#else
smtc_shield_lr11xx_t shield = SMTC_SHIELD_LR11XX_NONE_INSTANTIATE;
#endif

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DECLARATION -------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS DEFINITION ---------------------------------------------
 */

const smtc_shield_lr11xx_t* ral_lr11xx_get_shield( void )
{
    return &shield;
}

void ral_lr11xx_bsp_get_reg_mode( const void* context, lr11xx_system_reg_mode_t* reg_mode )
{
    *reg_mode = shield.get_reg_mode( );
}

void ral_lr11xx_bsp_get_rf_switch_cfg( const void* context, lr11xx_system_rfswitch_cfg_t* rf_switch_cfg )
{
    const lr11xx_system_rfswitch_cfg_t* rf_sw_cfg = shield.get_rf_switch_cfg( );
    if( rf_sw_cfg != NULL )
    {
        memcpy( rf_switch_cfg, rf_sw_cfg, sizeof( lr11xx_system_rfswitch_cfg_t ) );
    }
}

void ral_lr11xx_bsp_get_tx_cfg( const void* context, const ral_lr11xx_bsp_tx_cfg_input_params_t* input_params,
                                ral_lr11xx_bsp_tx_cfg_output_params_t* output_params )

{
    int8_t power_dbm = input_params->system_output_pwr_in_dbm;

    output_params->pa_ramp_time                      = LR11XX_RADIO_RAMP_48_US;
    output_params->chip_output_pwr_in_dbm_configured = power_dbm;
    output_params->chip_output_pwr_in_dbm_expected   = power_dbm;

    const smtc_shield_lr11xx_pa_pwr_cfg_t* pa_pwr_cfg = shield.get_pa_pwr_cfg( input_params->freq_in_hz, power_dbm );
    if( pa_pwr_cfg != NULL )
    {
        memcpy( &output_params->pa_cfg, &pa_pwr_cfg->pa_config, sizeof( lr11xx_radio_pa_cfg_t ) );
    }
    else
    {
        ESP_LOGE( TAG_BSP_LR11XX, "ERROR: failed to get PA config for requested power %d dBm", power_dbm );
    }
}

void ral_lr11xx_bsp_get_xosc_cfg( const void* context, ral_xosc_cfg_t* xosc_cfg,
                                  lr11xx_system_tcxo_supply_voltage_t* supply_voltage, uint32_t* startup_time_in_tick )
{
    /* Initialize output values */
    if( supply_voltage != NULL )
    {
        *supply_voltage = 0;
    }
    if( startup_time_in_tick != NULL )
    {
        *startup_time_in_tick = 0;
    }

    const smtc_shield_lr11xx_xosc_cfg_t* shield_xosc_cfg = shield.get_xosc_cfg( );
    if( shield_xosc_cfg != NULL )
    {
        if( xosc_cfg != NULL )
        {
            if( shield_xosc_cfg->has_tcxo )
            {
                *xosc_cfg = RAL_XOSC_CFG_TCXO_RADIO_CTRL;
            }
            else
            {
                *xosc_cfg = RAL_XOSC_CFG_XTAL;
            }
        }

        if( supply_voltage != NULL )
        {
            if( shield_xosc_cfg->has_tcxo )
            {
                *supply_voltage = shield_xosc_cfg->supply;
            }
        }

        if( startup_time_in_tick != NULL )
        {
            if( shield_xosc_cfg->has_tcxo )
            {
                *startup_time_in_tick = shield_xosc_cfg->startup_time_in_tick;
            }
        }
    }
}

void ral_lr11xx_bsp_get_rx_boost_cfg( const void* context, bool* rx_boost_is_activated )
{
    *rx_boost_is_activated = true;
}

void ral_lr11xx_bsp_get_crc_state( const void* context, bool* crc_is_activated )
{
    *crc_is_activated = false;
}

void ral_lr11xx_bsp_get_lfclk_cfg_in_sleep( const void* context, bool* lfclk_is_running )
{
    *lfclk_is_running = false;
}

void ral_lr11xx_bsp_get_rssi_calibration_table( const void* context, const uint32_t freq_in_hz,
                                                lr11xx_radio_rssi_calibration_table_t* rssi_calibration_table )
{
    const lr11xx_radio_rssi_calibration_table_t* shield_rssi_cal_table =
        shield.get_rssi_calibration_table( freq_in_hz );
    if( shield_rssi_cal_table != NULL )
    {
        memcpy( rssi_calibration_table, shield_rssi_cal_table, sizeof( lr11xx_radio_rssi_calibration_table_t ) );
    }
}

void ral_lr11xx_bsp_get_lora_cad_det_peak( ral_lora_sf_t sf, ral_lora_bw_t bw, ral_lora_cad_symbs_t nb_symbol,
                                           uint8_t* in_out_cad_det_peak )
{
    // Function used to fine tune the cad detection peak, update if needed
}

ral_status_t ral_lr11xx_bsp_get_instantaneous_tx_power_consumption( const ral_lr11xx_bsp_tx_cfg_output_params_t* tx_cfg,
                                                                    lr11xx_system_reg_mode_t radio_reg_mode,
                                                                    uint32_t*                pwr_consumption_in_ua )
{
    *pwr_consumption_in_ua = 0;
    return RAL_STATUS_OK;
}

ral_status_t ral_lr11xx_bsp_get_instantaneous_gfsk_rx_power_consumption( lr11xx_system_reg_mode_t radio_reg_mode,
                                                                         bool                     rx_boosted,
                                                                         uint32_t* pwr_consumption_in_ua )
{
    *pwr_consumption_in_ua = 0;
    return RAL_STATUS_OK;
}

ral_status_t ral_lr11xx_bsp_get_instantaneous_lora_rx_power_consumption( lr11xx_system_reg_mode_t radio_reg_mode,
                                                                         bool                     rx_boosted,
                                                                         uint32_t* pwr_consumption_in_ua )
{
    *pwr_consumption_in_ua = 0;
    return RAL_STATUS_OK;
}

/* --- EOF ------------------------------------------------------------------ */
