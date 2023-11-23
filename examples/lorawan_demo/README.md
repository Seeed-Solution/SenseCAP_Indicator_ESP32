# lorawan examples

This is a simple example of LoRawan usage.

## How to use example

- Step 1: Creat Device of your LoRaWAN Network Server.
- Step 2: Modify the ACTIVE_REGION of main.c.
- Step 3: Modify the device_eui and app_key of main.c.

Such as:

```
#define ACTIVE_REGION LORAMAC_REGION_EU868

/*!
 * LoRaWAN Deivce EUI
 */
static uint8_t device_eui[8] = { 0x6E, 0x92, 0xD1, 0x39, 0x1E, 0x80, 0xC4, 0x4B } ;

/*!
 * LoRaWAN Deivce APP KEY
 */
static uint8_t app_key[16] = { 0x2C, 0xBB, 0x9C, 0x1B, 0xB7, 0x94, 0xE3, 0xCC,  \
                              0x36, 0x92, 0x00, 0xFD, 0x84, 0xD6, 0xF8, 0x7F } ;
```


### Build and Flash

Run `idf.py -p PORT build flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.
