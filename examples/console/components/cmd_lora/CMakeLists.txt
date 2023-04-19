idf_component_register(SRCS "cmd_lora.c"
                    INCLUDE_DIRS .
                    REQUIRES console lora driver)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
