idf_component_register(
    SRCS "main.c" "lv_port.c" "lv_port_fs.c"
    INCLUDE_DIRS ".")
spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)
