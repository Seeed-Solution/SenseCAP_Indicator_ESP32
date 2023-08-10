# SenseCAP Indicator LoRa Communication Demos

This folder contains demo examples showcasing LoRa communication functionality for the SenseCAP Indicator D1L and D1Pro device(which have LoRa chip).
<div align="center">
   <img src="https://media-cdn.seeedstudio.com/media/wysiwyg/Indicator5.png" width="auto" height="auto" alt="img">

</div>

<div align="center">
<a href="https://www.seeedstudio.com/SenseCAP-Indicator-D1Pro-p-5644.html">
    <img src="https://svgur.com/i/wPJ.svg" width="400" height="120" alt="Indicator">
</a>
</div>

## Demos

- **Simple LoRa examples**: Basic example demonstrating how to set up a simple LoRa communication link.

  - **LoRa Communication Setup** The code demonstrates how to set up LoRa communication using the SX126x LoRa transceiver.

     - `demo_lora_cw.c`
     - `demo_lora_rx.c`
     - `demo_lora_tx.c`

  - **Beep**: When sent `ON` String on the specific channel, the beep on RP2040 will turned on, so as `OFF`

    - Lean the basic communication between ESP32S3 and RP2040 chips on the SenseCAP Indicator

    - How to process the initial received payload

  - **PingPong**: The code implements a ping-pong communication pattern between a master  and a slave device.

     - The master sends a "PING" message, and the slave  responds with a "PONG" message.
     - This pattern demonstrates the concept of simple two-way communication over LoRa.
     - **Finite State Machine (FSM)**: enabling the devices to respond and transition between these states based on various conditions.

- **Advanced LoRa demos**: Specific scenarios for quick start, have data transmission simulation in simple protocol
  - [Multi-sensors data Upload](https://github.com/Seeed-Solution/indicator_lora_commu)
    - XIAOS3 collects data , using Wio-E5(LoRa module, AT Commands) Upload sensor data to Indicator.

## Getting Started

To use the demo examples, follow these steps:

1. Clone or download this repository
2. Activate ESP-IDF environment
3. Navigate to the chosen demo project
4. Connect your SenseCAP Indicator D1L or D1Pro device to your computer
5. Configure the necessary parameters, such as LoRa frequency
6. Compile and upload the code to your device
7. Monitor the device's serial output to observe the communication process

## How to use example

In `main.c` define the Marco you want to run, the others require comment masking.

```c
#define USING_LORA_BEEP -> run func_beep()
```

### Build and Flash

Run `idf.py -p PORT build flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

---

For any issues, questions, or contributions, please refer to the [GitHub repository](https://github.com/Seeed-Solution/sensecap_indicator_esp32) and follow the contribution guidelines..
