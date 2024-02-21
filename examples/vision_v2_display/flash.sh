#!/bin/sh
export IDF_PATH=~/esp/esp-idf
. $IDF_PATH/export.sh  #get_idf
idf.py flash monitor  #esptool.py write_flash 0x10000 build/__UI_PROJECT_NAME__.bin
