/**
 * Timer LED
 * @file main.c
 * @brief "Timer LED" application for Zephyr RTOS on Raspberry Pi Pico2
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/logging/log.h>


// For the version control
#include <app_version.h>
#include "main.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL);

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct device *timer = DEVICE_DT_GET(HW_TIMER);

/**
 * @brief Set up the hardware timer with the specified callback and time interval
 * @param dev Pointer to the timer device
 * @param callback Timer callback function to be called when the timer expires
 * @param time_ms Time interval in milliseconds for the timer alarm
 * @return 0 on success, -1 on failure
 */
static int hw_timer_setup(const struct device *dev, counter_alarm_callback_t callback, uint32_t time_ms)
{
    if (!device_is_ready(dev)) {
        LOG_ERR("Error: Timer device %s is not ready", dev->name);
        return -1;
    }

    struct counter_alarm_cfg alarm_cfg = {
        .flags = 0,
        .ticks = counter_us_to_ticks(dev, time_ms * 1000),
        .callback = callback, // No callback needed for this application
        .user_data = NULL
    };

    int ret = counter_set_channel_alarm(dev, 0, &alarm_cfg);
    if (ret < 0) {
        LOG_ERR("Error %d: failed to set timer alarm", ret);
        return -1;
    }

    return 0;
}

/**
 * @brief Timer callback function, toggle the LED and set up the next timer alarm
 * @param dev Pointer to the timer device
 * @param chan_id Channel ID
 * @param ticks Number of ticks
 * @param user_data User data pointer
 */
static void timer_callback(const struct device *dev, uint8_t chan_id, uint32_t ticks, void *user_data)
{
    /* Toggle the LED state */
    gpio_pin_toggle_dt(&led);
    LOG_INF("Timer callback triggered: toggled LED, ticks: %u", ticks);

    hw_timer_setup(timer, timer_callback, SLEEP_TIME_MS);
}
/**
 * @brief main - Entry point for the Timer LED application
 *
 * This function initializes the GPIO for the LED and sets up a hardware timer 
 * to toggle the LED state at regular intervals defined by SLEEP_TIME_MS. 
 * It also logs the initialization status and any errors encountered during setup.
 *
 * @param void
 * @return 0 on success, non-zero on failure.
 */

int main(void)
{
    int ret;

    LOG_DBG("Timer LED - version %s", APP_VERSION_STRING);
    LOG_INF("LED device: %s, pin: %d", led.port->name, led.pin);
    LOG_INF("Timer device: %s", timer->name);

    if (!gpio_is_ready_dt(&led)) {
        LOG_ERR("Error: LED device %s is not ready", led.port->name);
        return -1;
    }

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Error %d: failed to configure LED pin %d", ret, led.pin);
        return -1;
    }

    ret = hw_timer_setup(timer, timer_callback, SLEEP_TIME_MS);
    if (ret < 0) {
        LOG_ERR("Failed to set up hardware timer %s", timer->name);
        return -1;
    }

    // Do not need this code. it's better to terminate the main function and let the timer callback handle the LED toggling.
    // while (1) {
    //     k_msleep(SLEEP_TIME_MS);
    // }
    return 0;
}
