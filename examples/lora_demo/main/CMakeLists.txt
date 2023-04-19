set(DEMO_DIR ./demo)
file(GLOB_RECURSE DEMO_SOURCES ${DEMO_DIR}/*.c)
idf_component_register(SRCS "main.c" ${DEMO_SOURCES}
                    INCLUDE_DIRS  ".")
