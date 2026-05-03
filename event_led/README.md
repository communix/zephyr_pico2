
# Event LED

## Overview
********

The Event LED test application

- Create two thread and communicate through Event message
- Thread 1 - LED controller, Generate random sleep time and send it to Thread 2 through queue.
- Thread 2 - LED driver. LED on/off when message is received.

```
+-------------------+     +-------------------+
|   Main Function   |     |  Shared Event     |
|                   |     |  Data Structure   |
| - Init GPIO LED   |     | (event_data_t)    |
| - Init event data |     |                   |
| - Create threads  |     | - k_event (signal)|
| - Send SYSTEM_ON  |     | - k_mutex (protect|
|                   |     |   event_message)  |
+-------------------+     | - uint32_t msg    |
          |               +-------------------+
          |                       ^
          v                       |
+-------------------+             |
| led_controller    |             |
| Thread            |             |
|                   |             |
| - Wait SYSTEM_ON  |             |
| - Loop:           |             |
|   * Toggle state  |             |
|   * Gen random    |             |
|     time (100-    |             |
|     1000ms)       |             |
|   * Send ON/OFF   |             |
|     event with    |             |
|     time          |             |
+-------------------+             |
          |                       |
          v                       |
+-------------------+             |
| led_driver Thread |             |
|                   |             |
| - Wait SYSTEM_ON  |             |
| - Loop:           |             |
|   * Wait ON/OFF   |             |
|     event         |             |
|   * Get time from |-------------+
|     shared data   |
|   * Set LED GPIO  |
|   * Clear event   |
+-------------------+
          |
          v
+-------------------+
|   Hardware LED    |
|   (GPIO Pin)      |
|                   |
| - Blinks with     |
|   random intervals|
+-------------------+
```
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
