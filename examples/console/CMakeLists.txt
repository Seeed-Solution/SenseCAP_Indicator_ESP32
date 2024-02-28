# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

add_compile_options(-fdiagnostics-color=always -w)

set(EXTRA_COMPONENT_DIRS ../../components
                        $ENV{IDF_PATH}/examples/system/console/advanced/components
                        $ENV{IDF_PATH}/examples/common_components/iperf)

set(EXCLUDE_COMPONENTS LoRaWAN)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(console)
