# Photo Demos Example

This is a picture demo.

## How to use example

You can put images in png format in the spiffs directory. The demo will poll and play pictures in the spiffs directory.

**Note**
1. You need to select a picture in png format, and the size is cut to 480*480.
2. You can put pictures within 5M in the spiffs directory.
3. The current demo display is limited to 20 through the "FILES_CNT_MAX" variable.




### Build and Flash

Run `idf.py -p PORT build flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.