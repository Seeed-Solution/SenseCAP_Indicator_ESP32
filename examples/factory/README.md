# Factory Bin

This is the first version of bin for factory firmware which is programmed by [terminal_demo](../terminal_demo/).

# IDF Patch

The patch is intended to achieve best perfermance of RGB LCD by using Octal PSRAM 120MHz feature. Therefore, the examples can work normally under the default LCD PCLK of 18 MHz.
The patch can be applied in IDF master **(commit id:f3159)** by following commands:
```
cd <root directory of IDF>
git checkout --recurse-submodules master
git checkout --recurse-submodules f3159
git apply <path of the patch>/idf_psram_120m.patch
git status      # Check whether the operation is successful, the output should look like below:

HEAD detached at f315986401
Changes not staged for commit:
  (use "git add <file>..." to update what will be committed)
  (use "git restore <file>..." to discard changes in working directory)
        modified:   components/esp_hw_support/port/esp32s3/rtc_clk.c
        modified:   components/esp_psram/esp32s3/Kconfig.spiram
        modified:   components/hal/esp32s3/include/hal/spimem_flash_ll.h
        modified:   components/spi_flash/esp32s3/mspi_timing_tuning_configs.h
        modified:   components/spi_flash/esp32s3/spi_timing_config.c
        modified:   components/spi_flash/esp32s3/spi_timing_config.h
        modified:   components/spi_flash/spi_flash_timing_tuning.c

Untracked files:
  (use "git add <file>..." to include in what will be committed)
        tools/test_apps/system/flash_psram/sdkconfig.ci.f8r8_120ddr
        tools/test_apps/system/flash_psram/sdkconfig.ci.f8r8_120ddr_120ddr
```