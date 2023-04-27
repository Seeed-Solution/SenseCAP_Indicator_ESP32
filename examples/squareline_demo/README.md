# SquareLine Example

This is a template project that uses the SquareLine tool to export the UI file, compile and download it to the device.

The [squareline_project.zip](./doc/squareline_project.zip)  are the SquareLine project of the demo.

## How to use example

1. Draw your own UI through the SquareLine tool, and then export the UI file.
2. Delete all the files in the [UI directory](./main/ui/) of the demo, and then copy the exported UI files to this directory.
3. Build, flash and monitor the project.


### Build and Flash

Run `idf.py -p PORT build flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.