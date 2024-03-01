# Grove Vision V2 X SenseCAP Indicator

## Prerequisites

- Get and install ESP-IDF toolchain and its dependencies.
  [ESP-IDF Get started](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)

  - Install ESP-IDF on Windows: [Description page](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/windows-setup.html)
  - There's a ESP-related IDE too, made by Espressif, containing ESP-IDF, called Espressif-IDE which is based on Eclipse CDT. [Espressif-IDE](https://github.com/espressif/idf-eclipse-plugin/blob/master/docs/Espressif-IDE.md)
    - Get the [ESP-IDF offline Windows installer](https://dl.espressif.com/dl/idf-installer/esp-idf-tools-setup-offline-5.1.1.exe?) or [Espressif-IDE Windows installer](https://dl.espressif.com/dl/idf-installer/espressif-ide-setup-2.11.0-with-esp-idf-5.1.1.exe)
    - Install it on your Windows system accepting all offered options and default settings. This automagically installs Python, git, CMake, etc all at once under C:\Espressif folder.
    - You can start building in command-line from the PowerShell/CMD entries created in the start-menu, but with the help of the included build.bat you can build on a normal commandline too
    - Or you can build the project in the IDE GUI, see 'Usage' section.