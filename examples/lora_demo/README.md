# LoRa examples

It's a lora test program, nothing is displayed. This demo can only run in the SenseCAP Indicator with lora.



## How to use example

Choose one, the others require comment masking.

```
demo_lora_cw();  // Tx lora Continuous Wave.
demo_lora_rx();  // Rx lora data.
demo_lora_tx();  // Tx lora data.
```

### Build and Flash

Run `idf.py -p PORT build flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.
