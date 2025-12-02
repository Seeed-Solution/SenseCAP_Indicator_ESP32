# Indicator OpenAI API Demo

This demo is a modified version of the **indicator openai demo**, removing the DALL·E image generation feature and adapting to more large language models compatible with the **OpenAI API format**, such as **DeepSeek**, **Qwen**, **Claude**, etc.

## Function
- [x] Time display.
- [x] CO2, tVOC, Temperature and Humidity data real-time display.
- [x] CO2, tVOC, Temperature and Humidity history data display.
- [x] Wifi config.
- [x] Display config.
- [x] Time config.
- [x] Chat with selectable large language models

## How to use example

Please first read the [User Guide](https://wiki.seeedstudio.com/SenseCAP_Indicator_Get_Started) of the SenseCAP Indicator Board to learn about its software and hardware information.

When using LLM-related functions, you must first configure the necessary parameters through the ESP32 serial port using the following commands:

### Available Commands

1. **Set OpenAI API Key**

```
openai_api -k <your key>
```
Sets your API key for authentication with the LLM service. Maximum length is 164 bytes.

2. **Set API URL**

```
openai_api -u <your url>
```
Sets the API endpoint URL. Default is "https://api.openai.com/v1" for OpenAI. For other providers, use their specific endpoints. Maximum length is 100 bytes.

Examples:
- DeepSeek: `openai_url -u https://api.deepseek.com/v1`
- SiliconFlow: `openai_url -u https://api.siliconflow.com/v1`

3. **Set Model**

```
openai_model <model_name>
```
Sets the model to use for chat completions. Maximum length is 50 bytes.

Examples:
- OpenAI: `openai_model gpt-3.5-turbo`
- DeepSeek: `openai_model deepseek-chat`
- Qwen: `openai_model qwen-7b`

4. **Set System Prompt**

```
openai_system <prompt>
```
Sets the system prompt that defines the assistant's behavior. Maximum length is 256 bytes.

Default prompt:
```
Your are SenseCAP Indicator, developed by Seeed Studio, has been launched on April 20th, 2023. You are a 4-inch touch screen driven by ESP32 and RP2040 dual-MCU, and support Wi-Fi/BLE/LoRa communication. You are a fully open-source powerful IoT development platform for developers. You are on behalf of Seeed Studio to answer requests. Each time your answer text should not exceed 100 words.
```

### Build and Flash

1. The project configure PSRAM with Octal 120M by default. please see [here](../../tools/patch/README.md#idf-patch) to enable `PSRAM Octal 120M` feature.
2. Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## To-do List
- [ ] Optimize the Set API Key UI elements and display; currently, UI element modifications don't synchronize with the information used by the LLM
- [ ] Test more LLM service providers (currently tested: DeepSeek, SiliconFlow, Alibaba Cloud Bailian)
- [ ] Conduct more comprehensive feature testing and bug testing
- [ ] Optimize the UI
- [ ] Change images
