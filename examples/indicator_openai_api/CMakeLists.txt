# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(EXTRA_COMPONENT_DIRS  ../../components)

add_compile_options(-fdiagnostics-color=always -w)

project(indicator_openai)

# 添加esp_http_client和esp_crt_bundle组件
set(COMPONENT_REQUIRES esp_http_client esp_crt_bundle)