   _____                      _________    ____
  / ___/___  ____  ________  / ____/   |  / __ \
  \__ \/ _ \/ __ \/ ___/ _ \/ /   / /| | / /_/ /
 ___/ /  __/ / / (__  )  __/ /___/ ___ |/ ____/
/____/\___/_/ /_/____/\___/\____/_/  |_/_/
  (C)2024 Seeed Studio

LoRa Hub project
================

This project demonstrates how to implement a single channel LoRa gateway (LoRaHub)
using an Espressif chip as host MCU and a Semtech LoRa radio.

This project contains the following components:
* `bin`: contains precompiled binaries for all supported platforms.
* `components/liblorahub`: the Hardware Abstraction Layer (HAL) on top of the radio driver.
* `components/radio_drivers`: the sx126x, llcc68, lr11xx radio drivers and associated hardware abstraction layer.
* `components/smtc_ral`: the radio abstraction layer and board/shields definitions.
* `lorahub/main`:
	* a packet forwarder implementation to connect the LoRaHub to a LoRaWAN network server, using Semtech UDP protocol (see PROTOCOL.md file).
	* connectivity and configuration: an example implementation for WiFi provisioning, and LoRaHub configuration through an HTTP server (web interface and RESTful API).
	* an example of OLED display handling for platforms supporting it.

# 1. Components

## 1.1. liblorahub

The Hardware Abstraction Layer (HAL) sits on top of the radio driver to and aims
to handle proper configuration for RX and TX for LoRaHub needs.

The HAL API is similar to what already exists for LoRa gateways (sx130x), it
exposes the following functions:
* `lgw_rxrf_setconf()`: set radio parameters
* `lgw_rxif_setconf()`: set modulation parameters
* `lgw_start()`: connect the host to the radio (SPI) and configure the radio for RX
* `lgw_stop()`: stop the radio
* `lgw_receive()`: check for received packet
* `lgw_send()`: send a packet and configure the radio back to RX after TX done
* `lgw_status()`: returns current LoRaHub status (free, emitting, ...)
* `lgw_get_instcnt()`: returns the current LoRaHub counter value
* `lgw_time_on_air()`: computes the time on air of a packet

The HAL is responsible for timestamping the received uplinks as precisely as
possible in order to allow the network to send a downlink in time back to an
end-device.
For this, the HAL configures the radio to raise an interrupt when a packet is
received. When this interrupt is raised, the HAL just gets the LoRaHub counter
value and returns. The received packet is actually retrieved when the user calls
lgw_receive().

## 1.2. radio drivers & hal

This project relies on the official Semtech's radio drivers for sx126x, llcc68
and lr11xx radio chips.

The drivers are available on Github:
* https://github.com/Lora-net/sx126x_driver
* https://github.com/Lora-net/llcc68_driver
* https://github.com/Lora-net/SWDR001

As usual, a radio HAL is implemented to define how to interact with the radios
depending on the platform it is used with (reset, SPI transfer, ...).

The Semtech Radio Abstraction Layer (smtc_ral) is also provided to abstract
radio usage.

# 2. Main application & Packet Forwarder

The main application handles the following:
* set WiFi provisioning mode via BLE if requested by the user
* start the packet forwarder.
* configure the display/
* start the HTTP server for configuration (channels, LNS address, ...).

## 2.1. Supported platforms

* Heltec WiFi LoRa 32 v3 (sx1262)
* Semtech LoRaHub devkit (sx1261, sx1262, sx1268, llcc68, lrr1121)

Concerning the Semtech LoRaHub devkit, one Semtech radio shield is supported
for each radio type:

| ID     | Region | Radio Shield | Components             |
|--------|--------|--------------|------------------------|
| Sx1261 | EU868  | SX1261MB1BAS | E406VO3A, XTAL, DC-DC  |
| Sx1262 | US915  | SX1262MB1CAS | E428VO3A, XTAL, LDO    |
| Sx1268 | CN470  | SX1268MB1GAS | E512VO1A, XTAL, LDO    |
| LLCC68 | EU868  | LLCC68MB2BAS | E568VO1, XTAL, LDO     |
| LR1121 | EU868  | LR1121MB1DIS | E655VO1A, XTAL         |
| LR1121 | US915  | LR1121MB1DIS | E655VO1A, XTAL         |
| LR1121 | CN470  | LR1121MB1GIS | E655VO1A, XTAL         |

For the LLCC68 shield, there is no connector to plug the display board of the
Semtech devkit, so the display must be disabled in the menuconfig configuration.

## 2.2. Dependencies

This project has been tested with ESP-IDF v5.2.1.

Among others, it uses the following espressif managed components:
* LVGL: a graphical library for OLED display, and the associated port for espressif.
* QRCode: a QR code handling library used for WiFi provisoning.

The expected versions are described in the `main/idf_component.yml` file

# 3. Usage

This project comes with precompiled binaries that can be flashed on the
supported platforms listed above. The files are located in the `bin` directory
of this project.

For flashing without the ESP-IDF installed, skip directly to the "flash with
esptool" section below.

For each supported platform/radio, the following binary files are provided:
* `xxx_bootloader.bin`: esp32 bootloader
* `xxx_partition-table.bin`: esp32 flash partition table
* `xxx_lorahub.bin`: the LoRaHub firmware


## 3.1. Install environment

This project is based on the Espressif ESP-IDF development. As such, the
following guide can be used to setup the environment.

https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#installation

Please note that the following instructions are for a linux setup. It may
slightly differ with a Windows setup.

## 3.2. Build

Get the radio drivers:

```console
cd ~/this_project_directory/components/radio_drivers
```

* sx126x driver (sx1261, sx1262, sx1268):

```console
git clone -b v2.3.2 https://github.com/Lora-net/sx126x_driver.git sx126x_driver
```

* llcc68 driver:

```console
git clone -b v2.3.2 https://github.com/Lora-net/llcc68_driver.git llcc68_driver
```

* lr11xx driver (lr1121):

```console
git clone -b v2.4.1 https://github.com/Lora-net/SWDR001.git lr11xx_driver
```

Enter the lorahub directory.

```console
cd ~/this_project_directory/lorahub
```

Prepare your linux terminal for ESP-IDF build from command line. This step can
be skipped on Windows, as the installed "ESP-IDF x.x CMD" command will prepare
it automatically.

```console
. ~/esp/esp-idf/export.sh
```

Configure the ESP32 target to build for.

```console
idf.py set-target esp32s3
```

Customize the build if necessary (it is configured for the Heltec board by default).

```console
idf.py menuconfig
```

Build the project.

```console
idf.py all
```

## 3.3. Flash

### 3.3.1 with esp-idf

Identify the serial device associated to the LoRaHub to be flashed, here we
suppose it is `/dev/ttyUSB0`.

```console
idf.py -p /dev/ttyUSB0 flash
```

If a permission error is returned, check check that the current user is part of
the `dialout` group. If not, do the following, reboot the linux machine and try
again:

```console
sudo usermod -a -G dialout $USERNAME
```

On a Windows setup, let's suppose that the device is mounted as COM14, the above
command would be like:

```console
idf.py -p COM14 flash
```

Launch the monitor console to see logs (optional).

```console
idf.py -p /dev/ttyUSB0 monitor
```

### 3.3.2 with esptool

If not using the complete ESP-IDF environment, it is also possible to flash the
provided binary files using the `esptool` utility.

https://docs.espressif.com/projects/esptool/en/latest/esp32/

```console
esptool.py --chip esp32s3 -p /dev/ttyUSB0 -b 460800 --before=default_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 80m --flash_size 2MB 0x0 bootloader.bin 0x10000 lorahub.bin 0x8000 partition-table.bin
```

On a Windows setup the esptool command for flashing would be:

```console
py -m esptool --chip esp32s3 -p COM14 -b 460800 --before=default_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 80m --flash_size 2MB 0x0 bootloader.bin 0x10000 lorahub.bin 0x8000 partition-table.bin
```

## 3.4. Configure LoRaHub at build time

It is possible to customize the default configuration before build by using the
`menuconfig`.

```console
idf.py menuconfig
```

The configuration specific to LoRaHub are located in the following menu:
* `LoRaWAN 1-CH HUB configuration`

It contains the following submenus:

* `Hardware Configuration`: contains board selection, radio type selection and
display enable/disable.

* `Packet Forwarder Configuration`: contains various options the channel
parameters, the LoRaWAN Network Server address and port etc...

Details are provided in the menus, but the following options are available:
* `Gateway_ID AUTO`: when checked, the MAC address is used to generate the
Gateway ID. When unchecked, a custom ID can be given.
* `Get config from flash in priority`: when checked, the channel configuration,
LNS configuration, ... is retrieved from the flash memory. If there is no
configuration stored in flash, it takes the configuration of the `menuconfig`.

In order to write a configuration in flash memory, the web interface or the REST
API have to be used. Of course, WiFi needs to be configured before.

# 4. Known limitations

* FSK modulation not supported

# 5. Changelog

### v0.1.0 ###

* Initial implementation of a LoRaHub on ESP32 with a sx126x radio.

### v0.2.0 ###

* Added support for llcc68 and lr11xx radios, using smtc_ral for radio
abstraction.
* Added shield definitions for associated radios.
