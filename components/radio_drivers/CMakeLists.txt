set(component_hal "sx126x_hal.c" "llcc68_hal.c" "lr11xx_hal.c")
set(component_sx126x_driver "sx126x_driver/src/sx126x.c")
set(component_llcc68_driver "llcc68_driver/src/llcc68.c")
set(component_lr11xx_driver "lr11xx_driver/src/lr11xx_system.c" "lr11xx_driver/src/lr11xx_radio.c" "lr11xx_driver/src/lr11xx_regmem.c")

idf_component_register(SRCS "${component_hal}" "${component_sx126x_driver}" "${component_llcc68_driver}" "${component_lr11xx_driver}"
                    PRIV_REQUIRES driver
                    REQUIRES lora
                    INCLUDE_DIRS "." "sx126x_driver/src" "llcc68_driver/src" "lr11xx_driver/src")
