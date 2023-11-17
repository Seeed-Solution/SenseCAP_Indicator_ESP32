<p align="center">
  <a href="https://wiki.seeedstudio.com/SenseCAP_Indicator_How_To_Flash_The_Default_Firmware/">
    <img src="https://files.seeedstudio.com/wiki/wiki-platform/SeeedStudio.png" width="auto" height="auto" alt="SenseCAP">
  </a>
</p>

<div align="center">

# **SenseCAP Indicator ESP32**

<!-- This project is developed based on ESP32-IDF, and Using C/C++ language development. There are some examples to teach you how to use some functions of ESP32. The firmware of the example is burned into the ESP32 MCU. -->

This project is built on the ESP32 IoT Development Framework (IDF) and utilizes the C/C++ programming language for development.

</div>

<p align="center">
  <a href="https://raw.githubusercontent.com/seeed-solution/SenseCAP_Indicator_ESP32/main/LICENSE">
    <img src="https://img.shields.io/github/license/seeed-solution/SenseCAP_Indicator_ESP32" alt="license">
  </a>
  <a href="https://github.com/seeed-solution/SenseCAP_Indicator_ESP32/releases">
    <img src="https://img.shields.io/github/v/release/seeed-solution/SenseCAP_Indicator_ESP32?include_prereleases&color=blueviolet" alt="release">
  </a>
    <img src="https://img.shields.io/github/repo-size/seeed-solution/SenseCAP_indicator_ESP32" alt="repo-size">
</p>

> Relevant: [SenseCAP Indicator RP2040](https://github.com/Seeed-Solution/SenseCAP_Indicator_RP2040)

The project includes various examples that demonstrate how to effectively use ESP32 functions. To test the examples, the firmware is programmed onto the ESP32 microcontroller unit (MCU).

## Example Layout

- `button`  Demonstrates how to use the configurable buttons in SenseCAP Indicator.
- `console` Demonstrates how to use of command line functions.
- `esp32_rp2040_comm` Demonstration showcases the communication between ESP32 and RP2040.
- `indicator_basis`  This is a comprehensive demo,mainly implements time, sensor data display, and some configuration functions.
- `indicator_openai` This is a comprehensive demo,mainly based on the indicator_basis demo with added chartGPT and DALL•E functions.
- `indicator_lora`  Demonstrates how to communicate using lora in SenseCAP Indicator.
- `lvgl_demos` Demonstrated some demos of lvgl.
- `photo_demo` Demonstrates how to display a photo of yourself.
- `squareline_demo` Demonstrates how to display the UI file exported by the squareline project.

## Usage

1. Get and install the [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html#installation-step-by-step) development framework.
2. Download this project and open it in the ESP-IDF development framework.
3. run `idf.py build flash` to compile and burn the firmware.

For more detailed information, please refer to [SenseCAP Indicator How To Flash The Default Firmware](https://wiki.seeedstudio.com/SenseCAP_Indicator_How_To_Flash_The_Default_Firmware/).

## **SenseCAP Indicator**

SenseCAP Indicator is a 4-inch touch screen driven by ESP32-S3 and RP2040 Dual-MCU and supports Wi-Fi/Bluetooth/LoRa communication.

The device comes two Grove interfaces, which supports ADC and I2C transmission protocols, and two USB Type-C ports with GPIO expantion pins inside, so user can easily expand external accessories via USB port.

SenseCAP Indicator is a fully open source powerful IoT development platform for developers. One-stop ODM Fusion service is also available for customization and quick scale-up.

<div align="center"><img width={480} src="https://files.seeedstudio.com/wiki/SenseCAP/SenseCAP_Indicator/SenseCAP_Indicator_1.png"/></div>

<p align="center" style={{textAlign: 'center'}}><a href="https://www.seeedstudio.com/SenseCAP-Indicator-D1-p-5643.html" ><img src="https://files.seeedstudio.com/wiki/RS485_500cm%20ultrasonic_sensor/image%202.png" border="0" /></a></p>


### **Features**



- **Dual MCUs and Rich GPIOs**
Equipped with powerful ESP32S3 and RP2040 dual MCUs and over 400 Grove-compatible GPIOs for flexible expansion options.
- **Real-time Air Quality Monitoring**
Built-in tVOC and CO2 sensors, and an external Grove AHT20 TH sensor for more precise temperature and humidity readings.
- **Local LoRa Hub for IoT Connectivity**
Integrated Semtech SX1262 LoRa chip (optional) for connecting LoRa devices to popular IoT platforms such as Matter via Wi-Fi, without the need for additional compatible devices.
- **Fully Open Source Platform**
Leverage the extensive ESP32 and Raspberry Pi open source ecosystem for infinite application possibilities.
- **Fusion ODM Service Available**
Seeed Studio also provides one-stop ODM service for quick customization and scale-up to meet various needs.(please contact iot@seeed.cc)

