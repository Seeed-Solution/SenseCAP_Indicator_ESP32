#include <Arduino.h>
#include <PacketSerial.h>

#define DEBUG 0

// Type of transfer packet
#define PKT_TYPE_ACK 0x00

#define PKT_TYPE_CMD_BEEP_ON  0xA1
#define PKT_TYPE_CMD_BEEP_OFF 0xA2

PacketSerial myPacketSerial;

/************************ beep ****************************/

#define Buzzer 19  // Buzzer GPIO

void beep_init( void ) {
    pinMode( Buzzer, OUTPUT );
}
void beep_off( void ) {
    digitalWrite( 19, LOW );
}
void beep_on( void ) {
    analogWrite( Buzzer, 127 );
    delay( 50 );
    analogWrite( Buzzer, 0 );
}

/************************ recv cmd from esp32  ****************************/

void onPacketReceived( const uint8_t *buffer, size_t size ) {
#if DEBUG
    Serial.printf( "<--- recv len:%d, data: ", size );
    for ( int i = 0; i < size; i++ ) {
        Serial.printf( "0x%x ", buffer[i] );
    }
    Serial.println( "" );
#endif

    if ( size < 1 ) {
        return;
    }
    switch ( buffer[0] ) {
        case PKT_TYPE_ACK: {
            Serial.printf( "recv: %s\r\n ", &buffer[1] );
            break;
        }
        case PKT_TYPE_CMD_BEEP_ON: {
            beep_on();
            break;
        }
        case PKT_TYPE_CMD_BEEP_OFF: {
            beep_off();
            break;
        }
        default:
            break;
    }
}

void setup() {
    Serial.begin( 115200 );

    Serial1.setRX( 17 );
    Serial1.setTX( 16 );
    Serial1.begin( 115200 );
    myPacketSerial.setStream( &Serial1 );
    myPacketSerial.setPacketHandler( &onPacketReceived );

    beep_init();
    beep_on();
}

void loop() {
    myPacketSerial.update();
    if ( myPacketSerial.overflow() ) {
    }
    delay( 10 );
}
