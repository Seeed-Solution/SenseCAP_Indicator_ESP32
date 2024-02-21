
idf_component_register(
    SRC_DIRS
        "icm42670"
        "touch_panel"
        "io_expander"
        "sensor/bmp3xx"
    INCLUDE_DIRS
        "icm42670/include"
        "touch_panel/include"
        "io_expander"
        "sensor/bmp3xx"
    REQUIRES
        bsp)
