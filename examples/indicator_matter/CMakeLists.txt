cmake_minimum_required(VERSION 3.5)

if(NOT DEFINED ENV{ESP_MATTER_PATH})
    message(FATAL_ERROR "Please set ESP_MATTER_PATH to the path of esp-matter repo")
endif(NOT DEFINED ENV{ESP_MATTER_PATH})

set(ENV{ESP_MATTER_DEVICE_PATH} $ENV{ESP_MATTER_PATH}/device_hal/device/esp32s3_devkit_c)

set(PROJECT_VER "1.0")
set(PROJECT_VER_NUMBER 1)

set(ESP_MATTER_PATH $ENV{ESP_MATTER_PATH})
set(MATTER_SDK_PATH ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
include($ENV{ESP_MATTER_DEVICE_PATH}/esp_matter_device.cmake)

set(EXTRA_COMPONENT_DIRS
"../../components"
"${MATTER_SDK_PATH}/config/esp32/components"
"${ESP_MATTER_PATH}/components"
)

add_compile_options(-fdiagnostics-color=always -w)
add_compile_options(-fdata-sections)
add_compile_options(-ffunction-sections)
add_link_options(-Wl,-gc-sections)

project(indicator_matter)

idf_build_set_property(CXX_COMPILE_OPTIONS "-std=gnu++17;-Os;-DCHIP_HAVE_CONFIG_H;-Wno-overloaded-virtual" APPEND)
idf_build_set_property(C_COMPILE_OPTIONS "-Os" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-Wno-format-nonliteral;-Wno-format-security" APPEND)