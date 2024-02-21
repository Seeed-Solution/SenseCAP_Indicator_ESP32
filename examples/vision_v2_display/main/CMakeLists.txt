set(UI_DIR ./ui)
file(GLOB_RECURSE UI_SOURCES ${CMAKE_SOURCE_DIR} "${UI_DIR}/*.c" )

idf_component_register(
    SRCS "main.c" "lv_port.c" ${UI_SOURCES}
    INCLUDE_DIRS "." ${UI_DIR} )
