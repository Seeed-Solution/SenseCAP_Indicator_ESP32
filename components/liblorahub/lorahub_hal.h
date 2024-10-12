/*______                              _
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
  (C)2024 Semtech

Description:
    LoRaHub Hardware Abstraction Layer

License: Revised BSD License, see LICENSE.TXT file include in the project
*/

#ifndef _LORAHUB_HAL_H
#define _LORAHUB_HAL_H

/* -------------------------------------------------------------------------- */
/* --- DEPENDENCIES --------------------------------------------------------- */

#include <stdint.h>  /* C99 types */
#include <stdbool.h> /* bool type */

/* -------------------------------------------------------------------------- */
/* --- PUBLIC MACROS -------------------------------------------------------- */

#define IS_LORA_BW( bw ) ( ( bw == BW_125KHZ ) || ( bw == BW_250KHZ ) || ( bw == BW_500KHZ ) )
#define IS_LORA_DR( dr )                                                                                  \
    ( ( dr == DR_LORA_SF5 ) || ( dr == DR_LORA_SF6 ) || ( dr == DR_LORA_SF7 ) || ( dr == DR_LORA_SF8 ) || \
      ( dr == DR_LORA_SF9 ) || ( dr == DR_LORA_SF10 ) || ( dr == DR_LORA_SF11 ) || ( dr == DR_LORA_SF12 ) )
#define IS_LORA_CR( cr ) \
    ( ( cr == CR_LORA_4_5 ) || ( cr == CR_LORA_4_6 ) || ( cr == CR_LORA_4_7 ) || ( cr == CR_LORA_4_8 ) )

#define IS_TX_MODE( mode ) ( ( mode == IMMEDIATE ) || ( mode == TIMESTAMPED ) || ( mode == ON_GPS ) )

/* -------------------------------------------------------------------------- */
/* --- PUBLIC CONSTANTS ----------------------------------------------------- */

/* return status code */
#define LGW_HAL_SUCCESS 0
#define LGW_HAL_ERROR -1

/* radio-specific parameters */
#define LGW_XTAL_FREQU 32000000 /* frequency of the RF reference oscillator */
#define LGW_RF_CHAIN_NB 1       /* number of RF chains */

/* concentrator chipset-specific parameters */
/* to use array parameters, declare a local const and use 'if_chain' as index */
#define LGW_IF_CHAIN_NB 1 /* number of IF+modem RX chains */

/* values available for the 'modulation' parameters */
/* NOTE: arbitrary values */
#define MOD_UNDEFINED 0
#define MOD_CW 0x08
#define MOD_LORA 0x10

/* values available for the 'bandwidth' parameters */
#define BW_UNDEFINED 0
#define BW_500KHZ 0x06
#define BW_250KHZ 0x05
#define BW_125KHZ 0x04

/* values available for the 'datarate' parameters */
/* NOTE: LoRa values used directly to code SF bitmask in 'multi' modem, do not change */
#define DR_UNDEFINED 0
#define DR_LORA_SF5 5
#define DR_LORA_SF6 6
#define DR_LORA_SF7 7
#define DR_LORA_SF8 8
#define DR_LORA_SF9 9
#define DR_LORA_SF10 10
#define DR_LORA_SF11 11
#define DR_LORA_SF12 12

/* values available for the 'coderate' parameters (LoRa only) */
/* NOTE: arbitrary values */
#define CR_UNDEFINED 0
#define CR_LORA_4_5 0x01
#define CR_LORA_4_6 0x02
#define CR_LORA_4_7 0x03
#define CR_LORA_4_8 0x04

/* values available for the 'status' parameter */
/* NOTE: values according to hardware specification */
#define STAT_UNDEFINED 0x00
#define STAT_NO_CRC 0x01
#define STAT_CRC_BAD 0x11
#define STAT_CRC_OK 0x10

/* values available for the 'tx_mode' parameter */
#define IMMEDIATE 0
#define TIMESTAMPED 1
#define ON_GPS 2

/* values available for 'select' in the status function */
#define TX_STATUS 1
#define RX_STATUS 2

/* status code for TX_STATUS */
/* NOTE: arbitrary values */
#define TX_STATUS_UNKNOWN 0
#define TX_OFF 1       /* TX modem disabled, it will ignore commands */
#define TX_FREE 2      /* TX modem is free, ready to receive a command */
#define TX_SCHEDULED 3 /* TX modem is loaded, ready to send the packet after an event and/or delay */
#define TX_EMITTING 4  /* TX modem is emitting */

/* status code for RX_STATUS */
/* NOTE: arbitrary values */
#define RX_STATUS_UNKNOWN 0
#define RX_OFF 1       /* RX modem is disabled, it will ignore commands  */
#define RX_ON 2        /* RX modem is receiving */
#define RX_SUSPENDED 3 /* RX is suspended while a TX is ongoing */

#define MIN_LORA_PREAMBLE 6
#define STD_LORA_PREAMBLE 8

/* -------------------------------------------------------------------------- */
/* --- PUBLIC TYPES --------------------------------------------------------- */

/**
@struct lgw_conf_rxrf_s
@brief Radio configuration structure
*/
struct lgw_conf_rxrf_s
{
    uint32_t freq_hz;     /*!> center frequency of the radio in Hz */
    float    rssi_offset; /*!> Board-specific RSSI correction factor */
    bool     tx_enable;   /*!> enable or disable TX on that RF chain */
};

/**
@struct lgw_conf_rxif_s
@brief Modulation configuration structure
*/
struct lgw_conf_rxif_s
{
    uint8_t  modulation; /*!> RX modulation */
    uint8_t  bandwidth;  /*!> RX bandwidth, 0 for default */
    uint32_t datarate;   /*!> RX datarate, 0 for default */
    uint8_t  coderate;   /*!> RX coding rate (LoRa only) */
};

/**
@struct lgw_pkt_rx_s
@brief Structure containing the metadata of a packet that was received and a pointer to the payload
*/
struct lgw_pkt_rx_s
{
    uint32_t freq_hz;      /*!> central frequency of the IF chain */
    uint8_t  if_chain;     /*!> by which IF chain was packet received */
    uint8_t  status;       /*!> status of the received packet */
    uint32_t count_us;     /*!> internal concentrator counter for timestamping, 1 microsecond resolution */
    uint8_t  rf_chain;     /*!> through which RF chain the packet was received */
    uint8_t  modulation;   /*!> modulation used by the packet */
    uint8_t  bandwidth;    /*!> modulation bandwidth (LoRa only) */
    uint32_t datarate;     /*!> RX datarate of the packet (SF for LoRa) */
    uint8_t  coderate;     /*!> error-correcting code of the packet (LoRa only) */
    float    rssic;        /*!> average RSSI of the channel in dB */
    float    snr;          /*!> average packet SNR, in dB (LoRa only) */
    uint16_t size;         /*!> payload size in bytes */
    uint8_t  payload[256]; /*!> buffer containing the payload */
};

/**
@struct lgw_pkt_tx_s
@brief Structure containing the configuration of a packet to send and a pointer to the payload
*/
struct lgw_pkt_tx_s
{
    uint32_t freq_hz;      /*!> center frequency of TX */
    uint8_t  tx_mode;      /*!> select on what event/time the TX is triggered */
    uint32_t count_us;     /*!> timestamp or delay in microseconds for TX trigger */
    uint8_t  rf_chain;     /*!> through which RF chain will the packet be sent */
    int8_t   rf_power;     /*!> TX power, in dBm */
    uint8_t  modulation;   /*!> modulation to use for the packet */
    int8_t   freq_offset;  /*!> frequency offset from Radio Tx frequency (CW mode) */
    uint8_t  bandwidth;    /*!> modulation bandwidth (LoRa only) */
    uint32_t datarate;     /*!> TX datarate (SF for LoRa) */
    uint8_t  coderate;     /*!> error-correcting code of the packet (LoRa only) */
    bool     invert_pol;   /*!> invert signal polarity, for orthogonal downlinks (LoRa only) */
    uint16_t preamble;     /*!> set the preamble length, 0 for default */
    bool     no_crc;       /*!> if true, do not send a CRC in the packet */
    bool     no_header;    /*!> if true, enable implicit header mode (LoRa) */
    uint16_t size;         /*!> payload size in bytes */
    uint8_t  payload[256]; /*!> buffer containing the payload */
};

/* -------------------------------------------------------------------------- */
/* --- PUBLIC FUNCTIONS PROTOTYPES ------------------------------------------ */

/**
@brief Configure the radio parameters (must configure before start)
@param conf structure containing the configuration parameters
@return LGW_HAL_ERROR id the operation failed, LGW_HAL_SUCCESS else
*/
int lgw_rxrf_setconf( struct lgw_conf_rxrf_s* conf );

/**
@brief Configure the modulation parameters (must configure before start)
@param conf structure containing the configuration parameters
@return LGW_HAL_ERROR id the operation failed, LGW_HAL_SUCCESS else
*/
int lgw_rxif_setconf( struct lgw_conf_rxif_s* conf );

/**
@brief Connect to the LoRa concentrator, reset it and configure it according to previously set parameters
@return LGW_HAL_ERROR id the operation failed, LGW_HAL_SUCCESS else
*/
int lgw_start( void );

/**
@brief Stop the LoRa concentrator and disconnect it
@return LGW_HAL_ERROR id the operation failed, LGW_HAL_SUCCESS else
*/
int lgw_stop( void );

/**
@brief A non-blocking function that will fetch up to 'max_pkt' packets from the LoRa concentrator FIFO and data buffer
@param max_pkt maximum number of packet that must be retrieved (equal to the size of the array of struct)
@param pkt_data pointer to an array of struct that will receive the packet metadata and payload pointers
@return LGW_HAL_ERROR id the operation failed, else the number of packets retrieved
*/
int lgw_receive( uint8_t max_pkt, struct lgw_pkt_rx_s* pkt_data );

/**
@brief Schedule a packet to be send immediately or after a delay depending on tx_mode
@param pkt_data structure containing the data and metadata for the packet to send
@return LGW_HAL_ERROR id the operation failed, LGW_HAL_SUCCESS else

/!\ When sending a packet, there is a delay (approx 1.5ms) for the analog
circuitry to start and be stable. This delay is adjusted by the HAL depending
on the board version (lgw_i_tx_start_delay_us).

In 'timestamp' mode, this is transparent: the modem is started
lgw_i_tx_start_delay_us microseconds before the user-set timestamp value is
reached, the preamble of the packet start right when the internal timestamp
counter reach target value.

In 'immediate' mode, the packet is emitted as soon as possible: transferring the
packet (and its parameters) from the host to the concentrator takes some time,
then there is the lgw_i_tx_start_delay_us, then the packet is emitted.

In 'triggered' mode (aka PPS/GPS mode), the packet, typically a beacon, is
emitted lgw_i_tx_start_delay_us microsenconds after a rising edge of the
trigger signal. Because there is no way to anticipate the triggering event and
start the analog circuitry beforehand, that delay must be taken into account in
the protocol.
*/
int lgw_send( struct lgw_pkt_tx_s* pkt_data );

/**
@brief Give the the status of different part of the LoRa concentrator
@param select is used to select what status we want to know
@param code is used to return the status code
@return LGW_HAL_ERROR id the operation failed, LGW_HAL_SUCCESS else
*/
int lgw_status( uint8_t rf_chain, uint8_t select, uint8_t* code );

/**
@brief Return instateneous value of internal counter
@param inst_cnt_us pointer to receive timestamp value
@return LGW_HAL_ERROR id the operation failed, LGW_HAL_SUCCESS else
*/
int lgw_get_instcnt( uint32_t* inst_cnt_us );

/**
@brief Return time on air of given packet, in milliseconds
@param packet is a pointer to the packet structure
@return the packet time on air in milliseconds
*/
uint32_t lgw_time_on_air( const struct lgw_pkt_tx_s* packet );

/**
@brief Return minimum and maximum frequency supported by the configured radio (in Hz)
@param  min_freq_hz pointer to hold the minimum frequency supported
@param  max_freq_hz pointer to hold the maximum frequency supported
@return N/A
*/
void lgw_get_min_max_freq_hz( uint32_t* min_freq_hz, uint32_t* max_freq_hz );

/**
@brief Return minimum and maximum TX power supported by the configured radio (in dBm)
@param  min_freq_hz pointer to hold the minimum TX power supported
@param  max_freq_hz pointer to hold the maximum TX power supported
@return N/A
*/
void lgw_get_min_max_power_dbm( int8_t* min_power_dbm, int8_t* max_power_dbm );

#endif  // _LORAHUB_HAL_H

/* --- EOF ------------------------------------------------------------------ */