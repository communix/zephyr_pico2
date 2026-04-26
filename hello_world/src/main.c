/**
 * Hello World - template for a simple Zephyr application
 * @file main.c
 * @brief A simple "Hello World" application for Zephyr RTOS on Raspberry Pi Pico2
 */

#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

// For the version control
#include <app_version.h>
#include "main.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL);

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/**
 * @brief main - Entry point for the Hello World application
 *
 * This function initializes the GPIO for the LED,
 * then enters an infinite loop where it toggles the LED state
 * and prints characters from the "Hello World!" string one by one.
 * The index is reset to 0 when it reaches the end of the string,
 * creating a continuous loop of output.
 *
 * @param void
 * @return 0 on success, non-zero on failure.
 */

int main(void)
{
    int ret;
    bool led_state = true;
    const char hello[] = "Hello World! ";
    unsigned int hello_len = strlen(hello);
    uint8_t current_index = 0;

    LOG_DBG("Hello World - version %s", APP_VERSION_STRING);
    LOG_INF("LED device: %s, pin: %d", led.port->name, led.pin);
    if (!gpio_is_ready_dt(&led)) {
        LOG_ERR("Error: LED device %s is not ready", led.port->name);
        return -1;
    }

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Error %d: failed to configure LED pin %d", ret, led.pin);
        return -1;
    }

    while (1) {
        ret = gpio_pin_toggle_dt(&led);
        if (ret < 0) {
            LOG_ERR("Error %d: failed to toggle LED pin %d", ret, led.pin);
            return -1;
        }

        led_state = !led_state;
        LOG_INF("%c", hello[current_index++]);

        // reset current_index to 0 when it reaches the end of the string
        if (current_index >= hello_len) {
            current_index = 0;
        }
        k_msleep(SLEEP_TIME_MS);
    }
    return 0;
}
