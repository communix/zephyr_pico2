
# Hello World - template for a simple Zephyr application

## Overview
********

The Hello World sample demonstrates a simple Zephyr application.

## Requirements
************

This code is for RP pico2 board.

## Building and Running
********************

> run in the zephyr workspace

``
west build -p auto -b rpi_pico2/rp2350a/m33 -S cdc-acm-console zephyr_pico2/hello_world --build-dir zephyr_pico2/hello_world/build
``

After flashing, the LED starts to blink and Hello World message is printed on the console (USB Serial).

> use terminal application to see the usb-uart console output

``
screen /dev/tty.usbmodem0000 115200
``

## Build errors
************

None

## Adding board support
********************

Tips:
None

## Release Note

### version 1.0.0

Initial release.

### version 1.0.1

Seperate main.h - No functinal change