/*!
 * \file      ral_sx126x_bsp.c
 *
 * \brief     Implements the HAL functions for SX126X
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

#include "ral_sx126x_bsp.h"
#include "smtc_shield_sx126x.h"

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE MACROS-----------------------------------------------------------
 */

#define SMTC_SHIELD_SX126X_NONE_INSTANTIATE                                                                   \
    {                                                                                                         \
        .get_pa_pwr_cfg = NULL, .is_dio2_set_as_rf_switch = NULL, .get_reg_mode = NULL, .get_xosc_cfg = NULL, \
        .get_pinout = NULL,                                                                                   \
    }

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE CONSTANTS -------------------------------------------------------
 */

static const char* TAG_BSP_SX126X = "BSP_SX126X";

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE TYPES -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE VARIABLES -------------------------------------------------------
 */

#if defined( CONFIG_SEMTECH_DEVKIT )
#if defined( CONFIG_RADIO_TYPE_SX1261 )
#include "smtc_shield_sx1261mb1bas.h"
smtc_shield_sx126x_t shield = SMTC_SHIELD_SX1261MB1BAS_INSTANTIATE;
#elif defined( CONFIG_RADIO_TYPE_SX1262 )
#include "smtc_shield_sx1262mb1cas.h"
smtc_shield_sx126x_t shield = SMTC_SHIELD_SX1262MB1CAS_INSTANTIATE;
#elif defined( CONFIG_RADIO_TYPE_SX1268 )
#include "smtc_shield_sx1268mb1gas.h"
smtc_shield_sx126x_t shield = SMTC_SHIELD_SX1268MB1GAS_INSTANTIATE;
#else
smtc_shield_sx126x_t shield = SMTC_SHIELD_SX126X_NONE_INSTANTIATE;
#endif
#else  // CONFIG_HELTEC_WIFI_LORA_32_V3
#include "heltec_wifi_lora_32_v3.h"
smtc_shield_sx126x_t shield = SMTC_SHIELD_SX1262MB1DAS_INSTANTIATE;
#endif

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DECLARATION -------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS DEFINITION ---------------------------------------------
 */

const smtc_shield_sx126x_t* ral_sx126x_get_shield( void )
{
    return &shield;
}

void ral_sx126x_bsp_get_reg_mode( const void* context, sx126x_reg_mod_t* reg_mode )
{
    *reg_mode = shield.get_reg_mode( );
}

void ral_sx126x_bsp_get_rf_switch_cfg( const void* context, bool* dio2_is_set_as_rf_switch )
{
    *dio2_is_set_as_rf_switch = shield.is_dio2_set_as_rf_switch( );
}

void ral_sx126x_bsp_get_tx_cfg( const void* context, const ral_sx126x_bsp_tx_cfg_input_params_t* input_params,
                                ral_sx126x_bsp_tx_cfg_output_params_t* output_params )

{
    int8_t requested_power_dbm = input_params->system_output_pwr_in_dbm;

    /* Initialize output memory */
    memset( output_params, 0, sizeof( ral_sx126x_bsp_tx_cfg_output_params_t ) );

    const smtc_shield_sx126x_pa_pwr_cfg_t* pa_pwr_cfg =
        shield.get_pa_pwr_cfg( input_params->freq_in_hz, requested_power_dbm );
    if( pa_pwr_cfg != NULL )
    {
        output_params->pa_ramp_time = SX126X_RAMP_40_US;
        output_params->chip_output_pwr_in_dbm_configured =
            pa_pwr_cfg->power; /* the power given to SetTxParams radio command */
        output_params->chip_output_pwr_in_dbm_expected = requested_power_dbm; /* the requested output power */
        memcpy( &output_params->pa_cfg, &pa_pwr_cfg->pa_config, sizeof( sx126x_pa_cfg_params_t ) );
    }
    else
    {
        ESP_LOGE( TAG_BSP_SX126X, "ERROR: failed to get PA config for requested power %d dBm", requested_power_dbm );
    }
}

void ral_sx126x_bsp_get_xosc_cfg( const void* context, ral_xosc_cfg_t* xosc_cfg,
                                  sx126x_tcxo_ctrl_voltages_t* supply_voltage, uint32_t* startup_time_in_tick )
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

    /* Get config from shield defintion */
    const smtc_shield_sx126x_xosc_cfg_t* shield_xosc_cfg = shield.get_xosc_cfg( );
    if( shield_xosc_cfg != NULL )
    {
        if( xosc_cfg != NULL )
        {
            if( shield_xosc_cfg->tcxo_is_radio_controlled )
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
            if( shield_xosc_cfg->tcxo_is_radio_controlled )
            {
                *supply_voltage = shield_xosc_cfg->supply_voltage;
            }
        }

        if( startup_time_in_tick != NULL )
        {
            if( shield_xosc_cfg->tcxo_is_radio_controlled )
            {
                *startup_time_in_tick = shield_xosc_cfg->startup_time_in_tick;
            }
        }
    }
}

void ral_sx126x_bsp_get_trim_cap( const void* context, uint8_t* trimming_cap_xta, uint8_t* trimming_cap_xtb )
{
    // Do nothing, let the driver choose the default values
}

void ral_sx126x_bsp_get_rx_boost_cfg( const void* context, bool* rx_boost_is_activated )
{
    *rx_boost_is_activated = true;
}

void ral_sx126x_bsp_get_ocp_value( const void* context, uint8_t* ocp_in_step_of_2_5_ma )
{
    // Do nothing, let the driver choose the default values
}

void ral_sx126x_bsp_get_lora_cad_det_peak( ral_lora_sf_t sf, ral_lora_bw_t bw, ral_lora_cad_symbs_t nb_symbol,
                                           uint8_t* in_out_cad_det_peak )
{
    // Function used to fine tune the cad detection peak, update if needed
}

ral_status_t ral_sx126x_bsp_get_instantaneous_tx_power_consumption(
    const ral_sx126x_bsp_tx_cfg_output_params_t* tx_cfg_output_params, sx126x_reg_mod_t radio_reg_mode,
    uint32_t* pwr_consumption_in_ua )
{
    *pwr_consumption_in_ua = 0;
    return RAL_STATUS_OK;
}

ral_status_t ral_sx126x_bsp_get_instantaneous_gfsk_rx_power_consumption( sx126x_reg_mod_t radio_reg_mode,
                                                                         bool             rx_boosted,
                                                                         uint32_t*        pwr_consumption_in_ua )
{
    *pwr_consumption_in_ua = 0;
    return RAL_STATUS_OK;
}

ral_status_t ral_sx126x_bsp_get_instantaneous_lora_rx_power_consumption( sx126x_reg_mod_t radio_reg_mode,
                                                                         bool             rx_boosted,
                                                                         uint32_t*        pwr_consumption_in_ua )
{
    *pwr_consumption_in_ua = 0;
    return RAL_STATUS_OK;
}

/* --- EOF ------------------------------------------------------------------ */
