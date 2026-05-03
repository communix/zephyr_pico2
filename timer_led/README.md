
# Timer LED

## Overview
********

Blinking LED using HW timer

```
+-----------------------------+
|          main()             |
|  (Entry Point Block)        |
+-----------------------------+
| - Log version & devices     |
| - Check LED ready           |
| - Configure LED pin         |
| - Call hw_timer_setup()     |
| - Return 0                  |
+-----------------------------+
              |
              | calls
              v
+-----------------------------+
|     hw_timer_setup()        |
|  (Timer Setup Block)        |
+-----------------------------+
| - Check timer device ready  |
| - Configure alarm with      |
|   callback & interval       |
| - Set channel alarm         |
| - Return success/failure    |
+-----------------------------+
              |
              | triggers (after alarm)
              v
+-----------------------------+
|     timer_callback()        |
|  (Callback Block)           |
+-----------------------------+
| - Toggle LED state          |
| - If RANDOM_SLEEP_TIME:     |
|   - Get random value        |
|   - Calculate next interval |
| - Else: use SLEEP_TIME_MS   |
| - Call hw_timer_setup()     |
|   for next alarm            |
| - Log info                  |
+-----------------------------+
              ^
              | loops back (reschedules timer)
```
## Requirements
************

This code is for RP pico2 board.

## Building and Running
********************

> run in the zephyr workspace

``
west build -p auto -b rpi_pico2/rp2350a/m33 -S cdc-acm-console zephyr_pico2/timer_led --build-dir zephyr_pico2/timer_led/build
``

After flashing, the LED starts to blink and Timer LED message is printed on the console (USB Serial).

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

HW Ramdom number generation for LED blinking timing.