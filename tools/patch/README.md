
# IDF Patch
The [patch](./patch/release5.0_psram_octal_120m.patch) is intended to achieve the best performance of RGB LCD by using the **PSRAM Octal 120 MHz** feature. The patch is only used for the **release/v5.0** branch of ESP-IDF. For the **master** branch, the PSRAM Octal 120 MHz feature can be directly used by enabling the `IDF_EXPERIMENTAL_FEATURES` option.

**Note: The PSRAM Octal 120 MHz feature has temperature risks and cannot guarantee normal functioning with a temperature higher than 65 degrees Celsius.**

Please make sure your IDF project is clean (use `git status` to check), then the patch can be applied by following commands:

### Manual
```sh
cd <root directory of IDF>

git apply --whitespace=fix <path of the patch>/release5.0_psram_octal_120m.patch # Nothing return if success
```

```sh
git status  # Check whether the operation is successful, the output should look like below:

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

These uncommitted modifications can be cleared by the following commands and **the other uncommitted modifications will be cleared too**.

```
git reset --hard
git clean -xdf
```

---
### Script
Also, this patch has been tested on ESP-IDF versions v5.0 and v5.0.1.
We have provided a shell script to quickly apply the patch:

1. Set up your ESP-IDF virtual environment in a shell.
2. Navigate to the folder and execute `./install_patch.sh`.
3. Verify that the patch has been applied correctly, as described above.


## Q/A
If you have any questions or concerns, please don't hesitate to reach out to us. You can open an issue on the relevant repository or contact us through our support channels.
