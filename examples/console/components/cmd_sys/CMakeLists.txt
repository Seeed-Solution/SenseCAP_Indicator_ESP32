idf_component_register(SRCS "cmd_sys.c"
                    INCLUDE_DIRS .
                    REQUIRES console i2c_devices spi_flash driver)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
