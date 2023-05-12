# Indicator Basis Demo

This demo mainly implements time, sensor data display, and some configuration functions.  

<figure class="third">
    <img src="./docs/page1.png" width="200"/> <img src="./docs/page2.png" width="200"/>
    <img src="./docs/page3.png" width="200"/> <img src="./docs/page4.png" width="200"/>
</figure>

## Function
- [x] Time display.
- [x] CO2, tVOC, Temperature and Humidity data real-time display.
- [x] CO2, tVOC, Temperature and Humidity history data display.
- [x] Wifi config.
- [x] Display config.
- [x] time config.
- [x] chatGPT
- [x] DALL E2


## How to use example

Please first read the [User Guide](https://wiki.seeedstudio.com/Getting_Started) of the SenseCAP Indicator Board to learn about its software and hardware information.


### Build and Flash

1. The project configure PSRAM with Octal 120M by default. please see [here](../../factory/README.md#idf-patch) to enable `PSRAM Octal 120M` feature. 
2. Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.
