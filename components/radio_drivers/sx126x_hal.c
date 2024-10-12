/**
 * @file      sx126x_hal.c
 *
 * @brief     Implements the SX126x radio HAL functions
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

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "esp_log.h"
#include "esp_rom_sys.h"

#include "sx126x_hal.h"
#include "radio_context.h"

#include "sx126x_lorahub.h"

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE MACROS ----------------------------------------------------------
 */

#define WAIT_US( us ) esp_rom_delay_us( us )
#define WAIT_MS( ms ) esp_rom_delay_us( ms * 1000 )

#define INDICATOR_LORAHUB 1
/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE CONSTANTS -------------------------------------------------------
 */

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

/**
 * @brief Wait until radio busy pin returns to 0
 */
void sx126x_hal_wait_on_busy( const void* context );

/**
 * @brief SPI tranfer
 */
static uint8_t spi_transfer( const void* context, uint8_t address );

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS PROTOTYPES ---------------------------------------------
 */

sx126x_hal_status_t sx126x_hal_reset( const void* context )
{
    const radio_context_t* sx126x_context = ( const radio_context_t* ) context;

#if INDICATOR_LORAHUB
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_RST, 0);
    WAIT_MS( 5 );
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_RST, 1);
    WAIT_MS( 5 );
#else
    gpio_set_level( sx126x_context->gpio_rst, 0 );
    WAIT_MS( 5 );
    gpio_set_level( sx126x_context->gpio_rst, 1 );
    WAIT_MS( 5 );
#endif

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_wakeup( const void* context )
{
    const radio_context_t* sx126x_context = ( const radio_context_t* ) context;

#if INDICATOR_LORAHUB
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 0);
    WAIT_MS( 1 );
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 1);
#else
    gpio_set_level( sx126x_context->spi_nss, 0 );
    WAIT_MS( 1 );
    gpio_set_level( sx126x_context->spi_nss, 1 );
#endif

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_write( const void* context, const uint8_t* command, const uint16_t command_length,
                                      const uint8_t* data, const uint16_t data_length )

{
    const radio_context_t* sx126x_context = ( const radio_context_t* ) context;
    int                    i;

    sx126x_hal_wait_on_busy( context );

#if INDICATOR_LORAHUB
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 0);
#else
    gpio_set_level( sx126x_context->spi_nss, 0 );
#endif

    /* Write command */
    for( i = 0; i < command_length; i++ )
    {
        spi_transfer( context, command[i] );
    }
    /* Write data */
    for( i = 0; i < data_length; i++ )
    {
        spi_transfer( context, data[i] );
    }

#if INDICATOR_LORAHUB 
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 1);
#else
    gpio_set_level( sx126x_context->spi_nss, 1 );
#endif

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_read( const void* context, const uint8_t* command, const uint16_t command_length,
                                     uint8_t* data, const uint16_t data_length )
{
    const radio_context_t* sx126x_context = ( const radio_context_t* ) context;
    int                    i;

    sx126x_hal_wait_on_busy( context );

#if INDICATOR_LORAHUB 
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 0);
#else
    gpio_set_level( sx126x_context->spi_nss, 0 );
#endif

    /* Write command */
    for( i = 0; i < command_length; i++ )
    {
        spi_transfer( context, command[i] );
    }
    /* Read data */
    for( i = 0; i < data_length; i++ )
    {
        data[i] = spi_transfer( context, 0x00 );
    }

#if INDICATOR_LORAHUB
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 1);
#else
    gpio_set_level( sx126x_context->spi_nss, 1 );
#endif

    return SX126X_HAL_STATUS_OK;
}

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DEFINITION --------------------------------------------
 */

void sx126x_hal_wait_on_busy( const void* context )
{
    const radio_context_t* sx126x_context = ( const radio_context_t* ) context;
    int                    gpio_state;

#if INDICATOR_LORAHUB
    uint16_t pin_val;
    do
    {
        esp_err_t ret = indicator_io_expander->read_input_pins(&pin_val);
        WAIT_US( 1 );
    } while( (pin_val & (0x01 << EXPANDER_IO_RADIO_BUSY)) == 1 );
#else
    do
    {
        gpio_state = gpio_get_level( sx126x_context->gpio_busy );
        WAIT_US( 1 );
    } while( gpio_state == 1 );
#endif
}

bool spi_rw_byte( const void* context, uint8_t* data_in, uint8_t* data_out, size_t length )
{
    const radio_context_t*   sx126x_context = ( const radio_context_t* ) context;
    static spi_transaction_t spi_transaction;

    if( length > 0 )
    {
        memset( &spi_transaction, 0, sizeof( spi_transaction_t ) );
        spi_transaction.length    = length * 8; /* in bits */
        spi_transaction.tx_buffer = data_out;
        spi_transaction.rx_buffer = data_in;
        spi_device_transmit( sx126x_context->spi_handle, &spi_transaction );
    }

    return true;
}

static uint8_t spi_transfer( const void* context, uint8_t address )
{
    uint8_t data_in;
    uint8_t data_out = address;
    spi_rw_byte( context, &data_in, &data_out, 1 );
    return data_in;
}

/* --- EOF ------------------------------------------------------------------ */
