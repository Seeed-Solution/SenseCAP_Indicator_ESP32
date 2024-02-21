set(srcs 
    "sx126x_sensecap_board.c"
    "radio.c"
    "sx126x.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS .
                    REQUIRES bus i2c_devices)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
