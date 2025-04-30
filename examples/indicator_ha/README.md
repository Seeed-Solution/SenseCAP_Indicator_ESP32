# Indicator Home Assistant Demo

This demo is mainly based on the indicator_basis demo with added Home Assistant functions.

<figure class="third">
    <img src="./docs/Home Assistant Data.png" width="480"/>
    <img src="./docs/Home Assistant.png" width="480"/>
    <img src="./docs/Home Assistant Control(ON).png" width="480"/>
    <img src="./docs/Home Assistant Control(OFF).png" width="480"/>
</figure>

## Function
- [x] Time display.
- [x] CO2, tVOC, Temperature and Humidity data real-time display.
- [x] CO2, tVOC, Temperature and Humidity history data display.
- [x] Wifi config.
- [x] Display config.
- [x] time config.
- [x] Home Assistant data.
- [x] Home Assistant control.

## How to use example

Please first read the [User Guide](https://wiki.seeedstudio.com/SenseCAP_Indicator_Get_Started) of the SenseCAP Indicator Board to learn about its software and hardware information.


Here are some simple steps to use.

- Step 1: [Install Home Assistant](https://www.home-assistant.io/installation/)
- Step 2: Install MQTT Broker
- Step 3: Add MQTT  integration and config
- Step 4: Modify "configuration.yaml" to add Indicator entity
- Step 5: Edit Dashboard


Add the following to your "configuration.yaml" file

Note: You need to replace {sta_mac} with the STA MAC address of the current device;Remove the colon from the MAC address and capitalize it.
For example, for the device `cc:ba:97:00:df:30`, the state_topic of the sensor is `CCBA9700DF3C/indicator/switch/state`
You can obtain the STA MAC address using the `read_mac` command.  

```
# Example configuration.yaml entry
mqtt:
  sensor:
    - unique_id: indicator_temperature
      name: "Indicator Temperature"
      state_topic: "{sta_mac}/indicator/sensor"
      suggested_display_precision: 1
      unit_of_measurement: "°C"
      value_template: "{{ value_json.temp }}"
    - unique_id: indicator_humidity
      name: "Indicator Humidity"
      state_topic: "{sta_mac}/indicator/sensor"
      unit_of_measurement: "%"
      value_template: "{{ value_json.humidity }}"
    - unique_id: indicator_co2
      name: "Indicator CO2"
      state_topic: "{sta_mac}/indicator/sensor"
      unit_of_measurement: "ppm"
      value_template: "{{ value_json.co2 }}"
    - unique_id: indicator_tvoc
      name: "Indicator tVOC"
      state_topic: "{sta_mac}/indicator/sensor"
      unit_of_measurement: ""
      value_template: "{{ value_json.tvoc }}"
  switch:
    - unique_id: indicator_switch1
      name: "Indicator Switch1"
      state_topic: "{sta_mac}/indicator/switch/state"
      command_topic: "{sta_mac}/indicator/switch/set"
      value_template: "{{ value_json.switch1 }}"
      payload_on: '{"switch1":1}'
      payload_off: '{"switch1":0}'
      state_on: 1
      state_off: 0
    - unique_id: indicator_switch2
      name: "Indicator Switch2"
      state_topic: "{sta_mac}/indicator/switch/state"
      command_topic: "{sta_mac}/indicator/switch/set"
      value_template: "{{ value_json.switch2 }}"
      payload_on: '{"switch2":1}'
      payload_off: '{"switch2":0}'
      state_on: 1
      state_off: 0
    - unique_id: indicator_switch3
      name: "Indicator Switch3"
      state_topic: "{sta_mac}/indicator/switch/state"
      command_topic: "{sta_mac}/indicator/switch/set"
      value_template: "{{ value_json.switch3 }}"
      payload_on: '{"switch3":1}'
      payload_off: '{"switch3":0}'
      state_on: 1
      state_off: 0
    - unique_id: indicator_switch4
      name: "Indicator Switch4"
      state_topic: "{sta_mac}/indicator/switch/state"
      command_topic: "{sta_mac}/indicator/switch/set"
      value_template: "{{ value_json.switch4 }}"
      payload_on: '{"switch4":1}'
      payload_off: '{"switch4":0}'
      state_on: 1
      state_off: 0
    - unique_id: indicator_switch6
      name: "Indicator Switch6"
      state_topic: "{sta_mac}/indicator/switch/state"
      command_topic: "{sta_mac}/indicator/switch/set"
      value_template: "{{ value_json.switch6 }}"
      payload_on: '{"switch6":1}'
      payload_off: '{"switch6":0}'
      state_on: 1
      state_off: 0
    - unique_id: indicator_switch7
      name: "Indicator Switch7"
      state_topic: "{sta_mac}/indicator/switch/state"
      command_topic: "{sta_mac}/indicator/switch/set"
      value_template: "{{ value_json.switch7 }}"
      payload_on: '{"switch7":1}'
      payload_off: '{"switch7":0}'
      state_on: 1
      state_off: 0
  number:
    - unique_id: indicator_switch5
      name: "Indicator Switch5"
      state_topic: "{sta_mac}/indicator/switch/state"
      command_topic: "{sta_mac}/indicator/switch/set"
      command_template: '{"switch5": {{ value }} }'
      value_template: "{{ value_json.switch5 }}"
    - unique_id: indicator_switch8
      name: "Indicator Switch8"
      state_topic: "{sta_mac}/indicator/switch/state"
      command_topic: "{sta_mac}/indicator/switch/set"
      command_template: '{"switch8": {{ value }} }'
      value_template: "{{ value_json.switch8 }}"
```


Add the following to the raw configuration editor of the dashboard.

```
views:
  - title: Indicator device
    icon: ''
    badges: []
    cards:
      - graph: line
        type: sensor
        detail: 1
        icon: mdi:molecule-co2
        unit: ppm
        entity: sensor.indicator_co2
      - graph: line
        type: sensor
        entity: sensor.indicator_temperature
        detail: 1
        icon: mdi:coolant-temperature
      - graph: line
        type: sensor
        detail: 1
        entity: sensor.indicator_humidity
      - graph: line
        type: sensor
        entity: sensor.indicator_tvoc
        detail: 1
        icon: mdi:air-filter
      - type: entities
        entities:
          - entity: switch.indicator_switch1
          - entity: switch.indicator_switch2
          - entity: switch.indicator_switch3
          - entity: switch.indicator_switch4
          - entity: number.indicator_switch5
          - entity: switch.indicator_switch6
          - entity: switch.indicator_switch7
          - entity: number.indicator_switch8
        title: Indicator control
        show_header_toggle: false
        state_color: true

```

 <img src="./docs/Home Assistant Dashboard.png" />


### Build and Flash

1. The project configure PSRAM with Octal 120M by default. please see [here](../../tools/patch/README.md#idf-patch) to enable `PSRAM Octal 120M` feature.
2. Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.
