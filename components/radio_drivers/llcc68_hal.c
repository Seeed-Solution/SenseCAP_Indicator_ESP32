/**
 * @file      llcc68_hal.c
 *
 * @brief     Implements the LLCC68 radio HAL functions
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

#include "llcc68_hal.h"
#include "radio_context.h"

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE MACROS ----------------------------------------------------------
 */

#define WAIT_US( us ) esp_rom_delay_us( us )
#define WAIT_MS( ms ) esp_rom_delay_us( ms * 1000 )

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
void llcc68_hal_wait_on_busy( const void* context );

/**
 * @brief SPI tranfer
 */
static uint8_t spi_transfer( const void* context, uint8_t address );

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS PROTOTYPES ---------------------------------------------
 */

llcc68_hal_status_t llcc68_hal_reset( const void* context )
{
    const radio_context_t* llcc68_context = ( const radio_context_t* ) context;

    gpio_set_level( llcc68_context->gpio_rst, 0 );
    WAIT_MS( 5 );
    gpio_set_level( llcc68_context->gpio_rst, 1 );
    WAIT_MS( 5 );

    return LLCC68_HAL_STATUS_OK;
}

llcc68_hal_status_t llcc68_hal_wakeup( const void* context )
{
    const radio_context_t* llcc68_context = ( const radio_context_t* ) context;

    gpio_set_level( llcc68_context->spi_nss, 0 );
    WAIT_MS( 1 );
    gpio_set_level( llcc68_context->spi_nss, 1 );

    return LLCC68_HAL_STATUS_OK;
}

llcc68_hal_status_t llcc68_hal_write( const void* context, const uint8_t* command, const uint16_t command_length,
                                      const uint8_t* data, const uint16_t data_length )

{
    const radio_context_t* llcc68_context = ( const radio_context_t* ) context;
    int                    i;

    llcc68_hal_wait_on_busy( context );

    gpio_set_level( llcc68_context->spi_nss, 0 );

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

    gpio_set_level( llcc68_context->spi_nss, 1 );

    return LLCC68_HAL_STATUS_OK;
}

llcc68_hal_status_t llcc68_hal_read( const void* context, const uint8_t* command, const uint16_t command_length,
                                     uint8_t* data, const uint16_t data_length )
{
    const radio_context_t* llcc68_context = ( const radio_context_t* ) context;
    int                    i;

    llcc68_hal_wait_on_busy( context );

    gpio_set_level( llcc68_context->spi_nss, 0 );

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

    gpio_set_level( llcc68_context->spi_nss, 1 );

    return LLCC68_HAL_STATUS_OK;
}

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DEFINITION --------------------------------------------
 */

void llcc68_hal_wait_on_busy( const void* context )
{
    const radio_context_t* llcc68_context = ( const radio_context_t* ) context;
    int                    gpio_state;
    do
    {
        gpio_state = gpio_get_level( llcc68_context->gpio_busy );
        WAIT_US( 1 );
    } while( gpio_state == 1 );
}

bool spi_rw_byte( const void* context, uint8_t* data_in, uint8_t* data_out, size_t length )
{
    const radio_context_t*   llcc68_context = ( const radio_context_t* ) context;
    static spi_transaction_t spi_transaction;

    if( length > 0 )
    {
        memset( &spi_transaction, 0, sizeof( spi_transaction_t ) );
        spi_transaction.length    = length * 8; /* in bits */
        spi_transaction.tx_buffer = data_out;
        spi_transaction.rx_buffer = data_in;
        spi_device_transmit( llcc68_context->spi_handle, &spi_transaction );
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
