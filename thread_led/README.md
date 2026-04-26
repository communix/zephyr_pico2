
# Thread LED

## Overview
********

The Thread LED is the code to test thread

To understand that tread timing is not accurate

Based on the code, turn off thread is start after 500ms and each thread is in sleep in 500ms. So the LED blinking in 1 sec is expected.
Actual result is different from our expectation, you can see the on/off time through the log message.

## Requirements
************

This code is for RP pico2 board.

## Building and Running
********************

> run in the zephyr workspace

``
west build -p auto -b rpi_pico2/rp2350a/m33 -S cdc-acm-console zephyr_pico2/thread_led --build-dir zephyr_pico2/thread_led/build
``

After flashing, the LED starts to blink and Thread LED message is printed on the console (USB Serial).

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