/**
 * @file pingpong.c
 * @brief Ping-Pong application example for SX126x LoRa tranceiver.
 * @date 2023-08-09
 * @note This example is based on pingpong example from Semtech LoRaMac-node.
 * @copyright https://github.com/Lora-net/LoRaMac-node
 */

#include "bsp_board.h"
#include "esp_log.h"
#include "radio.h"
#include <string.h>

static const char *TAG = "app_pingpong";

/**
 * @brief LoRa parameters
 */

#define REGION_EU868

#if defined( REGION_AS923 )

#define RF_FREQUENCY 923000000  // Hz

#elif defined( REGION_AU915 )

#define RF_FREQUENCY 915000000  // Hz

#elif defined( REGION_CN779 )

#define RF_FREQUENCY 779000000  // Hz

#elif defined( REGION_EU868 )

#define RF_FREQUENCY 868000000  // Hz

#elif defined( REGION_KR920 )

#define RF_FREQUENCY 920000000  // Hz

#elif defined( REGION_IN865 )

#define RF_FREQUENCY 865000000  // Hz

#elif defined( REGION_US915 )

#define RF_FREQUENCY 915000000  // Hz

#elif defined( REGION_RU864 )

#define RF_FREQUENCY 864000000  // Hz

#else
#error "Please define a frequency band in the compiler options."
#endif

#define TX_OUTPUT_POWER 14             // dBm

#define LORA_BANDWIDTH             0   // [0: 125 kHz,
                                       //  1: 250 kHz,
                                       //  2: 500 kHz,
                                       //  3: Reserved]
#define LORA_SPREADING_FACTOR      12  // [SF7..SF12]
#define LORA_CODINGRATE            1   // [1: 4/5,
                                       //  2: 4/6,
                                       //  3: 4/7,
                                       //  4: 4/8]
#define LORA_PREAMBLE_LENGTH       8   // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT        5   // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON       false

typedef enum {
    LOWPOWER,
    RX,
    RX_TIMEOUT,
    RX_ERROR,
    TX,
    TX_TIMEOUT,
} States_t;

#define RX_TIMEOUT_VALUE 1000
#define BUFFER_SIZE      64  // Define the payload size here

const uint8_t PingMsg[] = "PING";
const uint8_t PongMsg[] = "PONG";

uint16_t BufferSize = BUFFER_SIZE;
uint8_t Buffer[BUFFER_SIZE];

States_t State = LOWPOWER;

int8_t RssiValue = 0;
int8_t SnrValue  = 0;

static void DelayMs( uint32_t ms ) {
    vTaskDelay( ms / portTICK_PERIOD_MS );
}

/*!
 * Radio events function pointer
 */
static RadioEvents_t RadioEvents;

/*!
 * \brief Function to be executed on Radio Tx Done event
 */
void OnTxDone( void );

/*!
 * \brief Function to be executed on Radio Rx Done event
 */
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );

/*!
 * \brief Function executed on Radio Tx Timeout event
 */
void OnTxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Timeout event
 */
void OnRxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Error event
 */
void OnRxError( void );

/**
 * PingPong application entry point.
 */
void func_pingpong( void ) {
    bool isMaster = true;
    uint8_t i;

    // Target board initialization
    ESP_LOGI( TAG, "board initialization" );
    ESP_ERROR_CHECK( bsp_board_init() );

    // Radio initialization
    RadioEvents.TxDone    = OnTxDone;
    RadioEvents.RxDone    = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError   = OnRxError;

    Radio.Init( &RadioEvents );

    Radio.SetChannel( RF_FREQUENCY );

    /**
     * @brief LoRa TX config
     *
     * @param modem         MODEM_LORA
     * @param power         14 [dBm]
     * @param fdev          0       // FSK only
     * @param bandwidth     125 kHz // [0: 125 kHz,
                                    //  1: 250 kHz,
                                    //  2: 500 kHz,
                                    //  3: Reserved]
     * @param datarate      SF12
     * @param coderate      4/5     // [1: 4/5,
                                    //  2: 4/6,
                                    //  3: 4/7,
                                    //  4: 4/8]
     * @param preambleLen   8       // Same for Tx and Rx
     * @param fixLen        false   // [0: variable, 1: fixed]
     * @param crcOn         true
     * @param freqHopOn     false
     * @param hopPeriod     0       // [Number of symbols]
     * @param iqInverted    false   // [0: not inverted, 1: inverted]
     * @param timeout       3000    // Timeout duration [ms]
     */
    Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                       LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                       LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                       true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );

    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                       LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                       LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                       0, true, 0, 0, LORA_IQ_INVERSION_ON, true );

    Radio.SetMaxPayloadLength( MODEM_LORA, BUFFER_SIZE );

    Radio.Rx( RX_TIMEOUT_VALUE );

    while ( 1 ) {
        switch ( State ) {
            case RX:
                if ( isMaster == true ) {
                    if ( BufferSize > 0 ) {
                        if ( strncmp( (const char *)Buffer, (const char *)PongMsg, 4 ) == 0 ) {
                            // Indicates on a LED that the received frame is a PONG
                            ESP_LOGI( TAG, "Received PONG frame from slave" );

                            // Send the next PING frame
                            Buffer[0] = 'P';
                            Buffer[1] = 'I';
                            Buffer[2] = 'N';
                            Buffer[3] = 'G';
                            // We fill the buffer with numbers for the payload
                            for ( i = 4; i < BufferSize; i++ ) {
                                Buffer[i] = i - 4;
                            }
                            DelayMs( 1 );
                            Radio.Send( Buffer, BufferSize );
                        } else if ( strncmp( (const char *)Buffer, (const char *)PingMsg, 4 ) == 0 ) {  // A master already exists then become a slave
                            isMaster = false;
                            ESP_LOGI( TAG, "A master already exists then become a slave" );

                            Radio.Rx( RX_TIMEOUT_VALUE );
                        } else  // valid reception but neither a PING or a PONG message
                        {       // Set device as master ans start again
                            isMaster = true;
                            Radio.Rx( RX_TIMEOUT_VALUE );
                        }
                    }
                } else {
                    if ( BufferSize > 0 ) {
                        if ( strncmp( (const char *)Buffer, (const char *)PingMsg, 4 ) == 0 ) {
                            // Indicates that the received frame is a PING
                            ESP_LOGI( TAG, "Received PING frame from master" );

                            // Send the reply to the PONG string
                            Buffer[0] = 'P';
                            Buffer[1] = 'O';
                            Buffer[2] = 'N';
                            Buffer[3] = 'G';
                            // We fill the buffer with numbers for the payload
                            for ( i = 4; i < BufferSize; i++ ) {
                                Buffer[i] = i - 4;
                            }
                            DelayMs( 1 );
                            Radio.Send( Buffer, BufferSize );
                        } else  // valid reception but not a PING as expected
                        {       // Set device as master and start again
                            isMaster = true;
                            Radio.Rx( RX_TIMEOUT_VALUE );
                        }
                    }
                }
                State = LOWPOWER;
                break;
            case TX:
                // Indicates that we have sent a PING [Master]
                // Indicates that we have sent a PONG [Slave]
                ESP_LOGI( TAG, "Mode %s, Sent Payload: %s", isMaster == true ? "Master" : "Slave", Buffer );
                Radio.Rx( RX_TIMEOUT_VALUE );
                State = LOWPOWER;
                break;
            case RX_TIMEOUT:
            case RX_ERROR:
                if ( isMaster == true ) {
                    // Send the next PING frame
                    Buffer[0] = 'P';
                    Buffer[1] = 'I';
                    Buffer[2] = 'N';
                    Buffer[3] = 'G';
                    for ( i = 4; i < BufferSize; i++ ) {
                        Buffer[i] = i - 4;
                    }
                    DelayMs( 1 );
                    Radio.Send( Buffer, BufferSize );
                } else {
                    Radio.Rx( RX_TIMEOUT_VALUE );
                }
                State = LOWPOWER;
                break;
            case TX_TIMEOUT:
                Radio.Rx( RX_TIMEOUT_VALUE );
                State = LOWPOWER;
                break;
            case LOWPOWER:
            default:
                // Set low power
                break;
        }

        vTaskDelay( 3000 / portTICK_PERIOD_MS );
    }
}

void OnTxDone( void ) {
    Radio.Sleep();
    State = TX;
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ) {
    Radio.Sleep();
    BufferSize = size;
    memcpy( Buffer, payload, BufferSize );
    RssiValue = rssi;
    SnrValue  = snr;
    State     = RX;
}

void OnTxTimeout( void ) {
    Radio.Sleep();
    State = TX_TIMEOUT;
}

void OnRxTimeout( void ) {
    Radio.Sleep();
    State = RX_TIMEOUT;
}

void OnRxError( void ) {
    Radio.Sleep();
    State = RX_ERROR;
}
