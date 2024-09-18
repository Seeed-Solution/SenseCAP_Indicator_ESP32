# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(EXTRA_COMPONENT_DIRS 
    ../../components
    ${CMAKE_CURRENT_SOURCE_DIR}/components
)

add_compile_definitions(LR11XX_DISABLE_WARNINGS)

add_compile_options(-fdiagnostics-color=always -w)

project(indicator_lorahub)