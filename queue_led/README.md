
# Queue LED

## Overview
********

The Queue LED test application

- Create two thread and communicate through FIFO
- Thread 1 - LED controller, Generate random sleep time and send it to Thread 2 through queue.
- Thread 2 - LED driver. LED on/off when message is received.
```
┌─────────────────────────────────────────────────────────────────┐
│                         MAIN THREAD                              │
├─────────────────────────────────────────────────────────────────┤
│ 1. Initialize GPIO LED (pin config)                             │
│ 2. Initialize k_fifo queue                                      │
│ 3. Create led_controller thread                                 │
│ 4. Create led_driver thread                                     │
│ 5. Return (background execution)                                │
└─────────────────────────────────────────────────────────────────┘
         │                              │
         ├──────────────────────────────┤
         ▼                              ▼
┌─────────────────────────┐    ┌──────────────────────────┐
│  LED CONTROLLER THREAD   │    │   LED DRIVER THREAD       │
├─────────────────────────┤    ├──────────────────────────┤
│ Loop:                   │    │ Loop:                    │
│ 1. Get random time      │    │ 1. Wait for queue msg    │
│    (100-1000 ms)        │    │ 2. Extract LED control   │
│ 2. Toggle LED state     │    │ 3. Set GPIO pin          │
│    (ON ↔ OFF)          │    │ 4. Log message           │
│ 3. Create queue_data    │    │ 5. Free memory           │
│    structure            │    │                          │
│ 4. Put in FIFO queue───┼────▶ (K_FOREVER wait)         │
│ 5. Sleep               │    └──────────────────────────┘
│    (random ms)         │
└─────────────────────────┘

                  ▲
                  │
        ┌─────────┴──────────┐
        │   k_fifo queue     │
        │  (queue_data_t)    │
        │ - led_control      │
        │ - message time_ms  │
        └────────────────────┘
```

## Requirements
************

This code is for RP pico2 board.

## Building and Running
********************

> run in the zephyr workspace

``
west build -p auto -b rpi_pico2/rp2350a/m33 -S cdc-acm-console zephyr_pico2/queue_led --build-dir zephyr_pico2/queue_led/build
``

After flashing, the LED starts to blink and LED control message is printed on the console (USB Serial).

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