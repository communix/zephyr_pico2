
# Event LED

## Overview
********

The Event LED test application

- Create two thread and communicate through Event message
- Thread 1 - LED controller, Generate random sleep time and send it to Thread 2 through queue.
- Thread 2 - LED driver. LED on/off when message is received.

## Requirements
************

This code is for RP pico2 board.

## Building and Running
********************

> run in the zephyr workspace

``
west build -p auto -b rpi_pico2/rp2350a/m33 -S cdc-acm-console zephyr_pico2/event_led --build-dir zephyr_pico2/event_led/build
``

After flashing, the LED starts to blink and LED Event message is printed on the console (USB Serial).

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

### version 1.1.0

New architecture.
LED controller send on/off event to LED driver.