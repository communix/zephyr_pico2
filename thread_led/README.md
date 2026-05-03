
# Thread LED

## Overview
********

The Thread LED is the code to test thread

To understand that tread timing is not accurate

Based on the code, turn off thread is start after 500ms and each thread is in sleep in 500ms. So the LED blinking in 1 sec is expected.
Actual result is different from our expectation, you can see the on/off time through the log message.

```
┌─────────────────────────────────────────────────────────────────┐
│                     THREAD LED APPLICATION                       │
└─────────────────────────────────────────────────────────────────┘

                              MAIN()
                                │
                    ┌───────────┴───────────┐
                    │                       │
            ┌──────▼──────┐      ┌─────────▼────────┐
            │ GPIO Setup  │      │ LED Driver Init  │
            │  - Ready?   │      │ - Configure Pin  │
            │  - Configure│      └──────────────────┘
            └──────┬──────┘
                   │
   ┌──────────-──────┐
   │                 │
┌──▼─────────────┐ ┌─▼──────────────┐
│  THREAD 1      │ │  THREAD 2      │
│  led_on()      │ │  led_off()     │
│                │ │                │
│ ∞ Loop:        │ │ ∞ Loop:        │
│ ├─ Lock Mutex  │ │ ├─ Lock Mutex  │
│ ├─ Set LED ON  │ │ ├─ Set LED OFF │
│ ├─ Unlock      │ │ ├─ Unlock      │
│ └─ Sleep (ON)  │ │ └─ Sleep (OFF) │
│    Time ms     │ │    Time ms     │
└────────────────┘ └────────────────┘
         │                 │
         └────────┬────────┘
                  │
              [LED GPIO]
           Alternates ON/OFF
           Controlled by mutex
           to prevent race conditions


KEY COMPONENTS:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
1. LED Device (gpio_dt_spec): LED0_NODE
2. Synchronization: K_MUTEX (led_mutex) - Protects LED pin access
3. Thread 1: Continuously turns LED ON with timing delay
4. Thread 2: Continuously turns LED OFF with timing delay
5. common_led_control(): Shared function with mutex protection
```

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