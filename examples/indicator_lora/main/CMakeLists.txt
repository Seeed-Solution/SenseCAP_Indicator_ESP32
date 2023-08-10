set(BASIC_DEMO_DIR ./demo)
file(GLOB_RECURSE DEMO_SOURCES ${BASIC_DEMO_DIR}/*.c)

set(UTIL_DIR ./util)
file(GLOB_RECURSE UTIL_SOURCES ${UTIL_DIR}/*.c)

idf_component_register(SRCS "main.c" ${DEMO_SOURCES} ${UTIL_SOURCES}
                    INCLUDE_DIRS  "." ${UTIL_DIR})
