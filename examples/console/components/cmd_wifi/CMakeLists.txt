idf_component_register(SRCS "cmd_wifi.c"
                    INCLUDE_DIRS .
                    REQUIRES console esp_wifi  iperf)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
