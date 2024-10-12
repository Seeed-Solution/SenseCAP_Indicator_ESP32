set(component_ral "src/ral_sx126x.c" "src/ral_llcc68.c" "src/ral_lr11xx.c")
set(component_ral_bsp "bsp/sx126x/ral_sx126x_bsp.c" "bsp/llcc68/ral_llcc68_bsp.c" "bsp/lr11xx/ral_lr11xxx_bsp.c")
set(component_shields_sx126x "bsp/sx126x/smtc_shield_sx1261mb1bas.c" "bsp/sx126x/smtc_shield_sx1262mb1cas.c" "bsp/sx126x/smtc_shield_sx1268mb1gas.c" "bsp/sx126x/heltec_wifi_lora_32_v3.c")
set(component_shields_llcc68 "bsp/llcc68/smtc_shield_llcc68mb2cas.c")
set(component_shields_lr11xx "bsp/lr11xx/smtc_shield_lr11xx_common.c" "bsp/lr11xx/smtc_shield_lr11x1_common.c" "bsp/lr11xx/smtc_shield_lr1121mb1dis.c")

idf_component_register(SRCS "${component_ral}" "${component_ral_bsp}" "${component_shields_sx126x}" "${component_shields_llcc68}" "${component_shields_lr11xx}"
                    PRIV_REQUIRES driver radio_drivers
                    REQUIRES lora
                    INCLUDE_DIRS "./src" "./bsp" "./bsp/sx126x" "./bsp/llcc68" "./bsp/lr11xx" "../radio_drivers/sx126x_driver/src" "../radio_drivers/llcc68_driver/src" "../radio_drivers/lr11xx_driver/src")

