rd build  /S  /Q
rem  md build
rem  cd build
set IDF_PATH=C:\Espressif\frameworks\esp-idf-v5.1.1
C:\Espressif\idf_cmd_init.bat && idf.py build
rem  cmake  -G "Ninja"  ..  -DCMAKE_BUILD_TYPE=Release  #ninja  -j 4

