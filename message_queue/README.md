
# Message Queue example

## Overview
********

The random sensor data processing appliacation

```
+-------------------+
|   Main Thread     |
|                   |
| - Initialize HW   |
| - Create threads  |
| - Setup timer     |
| - Handle events   |
+-------------------+
          |
          | (creates)
          v
+-------------------+  +-------------------+  +-------------------+
| Sensor Thread 1   |  | Sensor Thread 2   |  | Sensor Thread 3   |
|                   |  |                   |  |                   |
| - Generate data   |  | - Generate data   |  | - Generate data   |
| - Send to MsgQ    |  | - Send to MsgQ    |  | - Send to MsgQ    |
+-------------------+  +-------------------+  +-------------------+
          |                    |                    |
          | (output to Data Agg) | (output to Data Agg) | (output to Data Agg)
          v                    v                    v
+-------------------------------------------------------+     +-------------------+     +-------------------+
| Data Agg Thread                                       | <---|   Hardware Timer  |     | Timer Callback    |
|                                                       |     |                   |     |                   |
| - Poll MsgQ                                           |     | - Periodic alarm  |     | - Toggle LED      |
| - Aggregate data                                      |     | - Triggers every  |     | - Signal agg      |
| - Send to Pipe                                        |     |   1 second        |     |   thread          |
+-------------------------------------------------------+     +-------------------+     +-------------------+
          |
          | (aggregated data)
          v
+-------------------+
| Sensor Core Thread|
|                   |
| - Read from Pipe  |
| - Log aggregated  |
|   data            |
+-------------------+
```
## Requirements
************

This code is for RP pico2 board.

## Building and Running
********************

> run in the zephyr workspace

``
west build -p auto -b rpi_pico2/rp2350a/m33 -S cdc-acm-console zephyr_pico2/message_queue --build-dir zephyr_pico2/message_queue/build
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
