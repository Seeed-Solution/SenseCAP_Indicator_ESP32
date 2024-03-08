#pragma once


// Serial
#define PIN_SERIAL1_TX (16u)
#define PIN_SERIAL1_RX (17u)

// #define PIN_SERIAL2_TX (8u) // Default Nothing used
// #define PIN_SERIAL2_RX (9u)
#define PIN_SERIAL2_TX (20u) // For Grove Vision(Right Grove) Default I2C USED
#define PIN_SERIAL2_RX (21u) // Don't use Serial2.begin() if using Wire.begin();

// SPI
#define PIN_SPI0_MISO  (0u)
#define PIN_SPI0_MOSI  (3u)
#define PIN_SPI0_SCK   (2u)
#define PIN_SPI0_SS    (1u)

#define PIN_SPI1_MISO  (12u)
#define PIN_SPI1_MOSI  (11u)
#define PIN_SPI1_SCK   (10u)
#define PIN_SPI1_SS    (13u)

// Wire
#define PIN_WIRE0_SDA  (20u)
#define PIN_WIRE0_SCL  (21u)

// #define PIN_WIRE1_SDA  (14u) // Default
// #define PIN_WIRE1_SCL  (15u)

#define PIN_WIRE1_SDA  (26u) // For Grove Vision(Left Grove) Default ADC USED
#define PIN_WIRE1_SCL  (27u)

#define SERIAL_HOWMANY (3u)
#define SPI_HOWMANY    (2u)
#define WIRE_HOWMANY   (2u)

#include "../generic/common.h"
