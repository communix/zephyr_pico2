# zephyr_pico2 project

Zephyr OS applications for RP PICO 2 board

## build command for pico2 board

> run in the zephyr workspace

``
west build -p auto -b rpi_pico2/rp2350a/m33 -S cdc-acm-console zephyr_pico2/hello_world --build-dir zephyr_pico2/hello_world/build
``

cdc-acm_console is needed to see the debug message through USB Serial port

``
screen /dev/tty.usbmodem0000 115200
``

## Zephyr worksspace application structure

> Please clone this project in the zephyr workspace

    zephyr workspace

    |--- zephyr

    |--- zephyr_pico2  --> zephyr_pico2 project

        |--- README.md --> this file
        
        |-- hello_world --> copy from blinky, this is a template of other projects

        |--- blinky_thread


## Templates

### basic_application

``
git clone https://github.com/zephyrproject-rtos/example-application 
``

### simple_application

``
cp -rf hello_world new_application
``
