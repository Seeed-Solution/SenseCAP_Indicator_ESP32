set(UI_DIR ./ui)
file(GLOB_RECURSE UI_SOURCES ${UI_DIR}/*.c)

idf_component_register(SRCS "main.c" "lv_port.c" "cobs.c" ${UI_SOURCES}
                        INCLUDE_DIRS  "."  ${UI_DIR})

