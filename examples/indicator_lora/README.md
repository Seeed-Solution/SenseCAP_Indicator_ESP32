# SenseCAP Indicator LoRa Communication Demonstrations

This directory comprises demo illustrations that exhibit the LoRa communication capabilities of the SenseCAP Indicator D1L and D1Pro devices, both equipped with LoRa chips.

<div align="center">
   <img src="https://media-cdn.seeedstudio.com/media/wysiwyg/Indicator5.png" width="auto" height="auto" alt="Image">
</div>

<div align="center">
<a href="https://www.seeedstudio.com/SenseCAP-Indicator-D1Pro-p-5644.html">
    <img src="https://svgur.com/i/wPJ.svg" width="400" height="120" alt="Indicator">
</a>
</div>

## Demonstrations

- **Basic LoRa Examples**: Fundamental illustrations showcasing the setup of a straightforward LoRa communication link.
  - **LoRa Communication Configuration**: This code demonstrates the configuration of LoRa communication using the SX126x LoRa transceiver.

    - `demo_lora_cw.c`
    - `demo_lora_rx.c`
    - `demo_lora_tx.c`

  - **Beep Control**: Sending the string `ON` on the specified channel activates the beep on the RP2040, similarly sending `OFF` turns it off.

    - Learn about basic communication between ESP32S3 and RP2040 chips on the SenseCAP Indicator.
    - Understand how to process the initial received payload.

  - **PingPong Communication**: This code establishes a ping-pong communication pattern between a master and a slave device.

    - The master sends a "PING" message, and the slave responds with a "PONG" message.
    - This pattern showcases the concept of simple two-way communication over LoRa.
    - Implementation of a **Finite State Machine (FSM)** that enables devices to respond and transition between different states based on various conditions.

- **Advanced LoRa Demonstrations**: Tailored scenarios for rapid initiation, featuring data transmission simulation in a simple protocol.
  - [Multi-Sensor Data Upload](https://github.com/Seeed-Solution/indicator_lora_commu)
    - XIAOS3 collects data and utilizes Wio-E5 (with LoRa module and AT Commands) to upload sensor data to the Indicator.

## Getting Started

To employ the demo examples, adhere to the subsequent steps:

1. Clone or download this repository.
2. Activate the ESP-IDF environment.
3. Navigate to the preferred demo project.
4. Connect your SenseCAP Indicator D1L or D1Pro device to your computer.
5. Configure essential parameters, such as LoRa frequency.
6. Compile and upload the code to your device.
7. Monitor the device's serial output to observe the communication process.

## Example Usage

In `main.c`, define the macro corresponding to the function you wish to run; comment out the others.

```c
#define USING_LORA_BEEP // Run func_beep()
```

### Build and Flash

Execute `idf.py -p PORT build flash monitor` to build, flash, and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

For comprehensive steps to configure and utilize ESP-IDF for building projects, refer to the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html).

---

For any inquiries, questions, or contributions, please refer to the [GitHub repository](https://github.com/Seeed-Solution/sensecap_indicator_esp32) and adhere to the contribution guidelines.
