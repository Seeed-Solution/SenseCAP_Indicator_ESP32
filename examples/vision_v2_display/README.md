# ESP-IDF / CMake / Eclipse / VScode  project for Seeed Studio SenseCAP Indicator development-device exported by SquareLine Studio


## Prerequisites

- Get and install ESP-IDF toolchain and its dependencies.
  [ESP-IDF Get started](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)

  - Install ESP-IDF on Windows: [Description page](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/windows-setup.html)
  - There's a ESP-related IDE too, made by Espressif, containing ESP-IDF, called Espressif-IDE which is based on Eclipse CDT. [Espressif-IDE](https://github.com/espressif/idf-eclipse-plugin/blob/master/docs/Espressif-IDE.md)
    - Get the [ESP-IDF offline Windows installer](https://dl.espressif.com/dl/idf-installer/esp-idf-tools-setup-offline-5.1.1.exe?) or [Espressif-IDE Windows installer](https://dl.espressif.com/dl/idf-installer/espressif-ide-setup-2.11.0-with-esp-idf-5.1.1.exe)
    - Install it on your Windows system accepting all offered options and default settings. This automagically installs Python, git, CMake, etc all at once under C:\Espressif folder.
    - You can start building in command-line from the PowerShell/CMD entries created in the start-menu, but with the help of the included build.bat you can build on a normal commandline too
    - Or you can build the project in the IDE GUI, see 'Usage' section.

  - Install ESP-IDF on Linux/MacOS: [Description page](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html)
    - Get Python3, git, cmake, ninja-build, wget, flex, bison, libusb, etc
    - In a directory (preferably a created $HOME/esp) type `git clone --recursive https://github.com/espressif/esp-idf.git`
      - (This gets the latest version into an 'esp-idf' subfolder. In ESP-IDF v5.0 a patch is needed to set PSRAM frequency to 120MHz.)
    - In the esp-idf directory run `./install.sh`, then you can add ESP32S3 support (needed for SenseCAP indicator) by `./install.sh esp32s3`
    - To use ESP-IDF ('idf.py') you need to type `. ./export.sh`, but it's only active in the current shell.
      - (To call it in any shell later easily, add `alias get_idf='. $HOME/esp/esp-idf/export.sh'` to your .bashrc file, so a `get_idf` command will get the IDF environment temporarily.)


## Usage

### Building the project and flashing to the device from command-line
- In this folder type `idf.py build` command to build the binaries for the SenseCAP indicator (in 'build' folder, a full `sdkconfig` file will be created if it didn't exist)
  (If the compiled application .bin file doesn't fit into the 'app' partition a size-check error will follow. In this project the app partition is set to 7MB in 'partitions.csv' file, total flash of SenseCAP is 8MB)
- To flash it to the device use `idf.py flash` command, after flashing or when switched on, it will start automatically. (If you issue this command first, it will build the project beforehand, if not already built.)
- (To monitor the output of the running application you can use `idf.py monitor` command. It will restart the device. It can be used together with flashing: `idf.py flash monitor`. Press Control+] to exit the monitor-shell.)
- (The SenseCAP device port is detected automatically, but you can specify it with -p <portname> argument, e.g. `idf.py -p /dev/ttyUSB0 monitor` or `idf.py -p COM6 flash`.)

### Project customization
- ESP-IDF uses 'menuconfig' to create the 'sdkconfig' (based on Kconfig) file which holds the whole configuration of the project (including detailed component settings), use the `idf.py menuconfig` command to start it
  - (sdkconfig in an ESP-IDF project has LVGL-configuration in the 'Component config' subcategory and 'lv_conf.h' as such is no longer used to configure LVGL features.)
- In this project all built-in ('montserrat') LVGL fonts are enabled. If you want to add/remove the LVGL built-infonts to select only the fonts your project needs look in submenu: Component config / LVGL configuration / Font usage / Enable built-in fonts.
- Above ESP-IDF 5.0 the 120MHz speed option for the PSRAM (necessary for SenseCAP Indicator) is selectable (no patch needed) after enabling 'Make experimental features visible', and is enabled in submenu: Components / ESP PSRAM / SPI RAM config / Set RAM clock speed
- Exit from menuconfig by Q and answer Y to save the modifications to file 'sdkconfig'.
- (If you want a clean sdkconfig file containing only the modifications you can type `idf.py save-defconfig` to get it. The resulting `sdkconfig.defaults` file will be used if `sdkconfig` file is not found.)
- (You can edit 'sdkconfig.defaults' directly by hand, and if you delete 'sdkconfig' and rebuild the project, it will be recreated. But please leave 'CONFIG_LV_MEM_CUSTOM=y' setting untouched, otherwise you might face a freeze.)


## Alternative build-methods/tools

- Command-line CMake-based building works as well, but the above-mentioned 'export.sh' or 'get_idf' should be used beforehand to pull the IDF environment into the terminal,
  - You need to create a 'build' folder (to avoid littering the project-folder), cd into it then type `cmake -G "Ninja" ..` to generate the files (or `cmake ..` for the slower 'Make' based build)
  - When the makefile gets generated you can build it with `ninja -j 4` command ('-j 4' tells it to use 4 CPUs if possible) and flash with `ninja flash` command
  - To make this easier there's a `build.sh` and `build.bat` file which does this plus cleans build-folder before every build, and then `flash.sh` or `flash.bat` uploads the program into the device.

- For GUI-based development Visual Studio Code and Eclipse-IDE basic project files are included too so you can build in these tools too, as the project files are based on CMake.

  - Eclipse CDT has a plugin described at [Eclipse Marketplace ESP-IDF plugin](https://marketplace.eclipse.org/content/esp-idf-eclipse-plugin) and [ESP-IDF plugin github-page](https://github.com/espressif/idf-eclipse-plugin)
    - In 'Help' / 'Install new software' submenu you can install 'Espressif IDF' from 'https://dl.espressif.com/dl/idf-eclipse-plugin/updates/latest/' by following the instructions
  - (But if you installed Espressif-IDE in the 'Prerequisites' section this will probably be most compatible with ESP-IDF development.)
    - Follow the instructions at the GitHub-page (browsing existing ESP-IDF folder when asked) and you'll get an Eclipse project environment to build and flash.
    - To open your exported project-template, use menu File / Import, select 'Existing IDF Project' and select the project folder. Pressing Build (hammer) button will compile the project to .elf and .bin files in 'build' folder.

  - VScode has an Espressif-IDF extension that should be used, installable from within VScode.
    - [Installation](https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md)
    - [Basic Use](https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/basic_use.md)

