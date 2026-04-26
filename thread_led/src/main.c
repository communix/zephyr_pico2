/**
 * Thread LED
 * @file main.c
 * @brief Two threads to turn on and off the LED.
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

// For the version control
#include <app_version.h>
#include "main.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL);
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/**
 * @brief Thread definitions for LED on and off functions.
 * Depending on the value of USE_KTHREAD_MACRO,
 * either K_THREAD_DEFINE is used to create the threads at compile time, or
 * k_thread_create is used to create the threads at runtime in the main function.
*/
#if (USE_KTHREAD_MACRO == 1)
K_THREAD_DEFINE(thread_led_on, STACKSIZE, led_on_function, NULL, NULL, NULL, ON_PRIORITY, 0, 0);
K_THREAD_DEFINE(thread_led_off, STACKSIZE, led_off_function, NULL, NULL, NULL, OFF_PRIORITY, 0, LED_ON_SLEEP_TIME_MS);
K_MUTEX_DEFINE(led_mutex);
#else // use k_thread_create function in the main()
K_THREAD_STACK_DEFINE(thread_led_on_stack, STACKSIZE);
K_THREAD_STACK_DEFINE(thread_led_off_stack, STACKSIZE);
struct k_thread thread_led_on_s;
struct k_thread thread_led_off_s;
struct k_mutex led_mutex;
#endif // USE_KTHREAD_MACRO

/**
 * @brief Common function to control the LED state (on/off) with mutex protection.
 * This function attempts to lock the mutex before setting the LED state.
 * @param led_on A boolean value indicating whether to turn the LED on (1) or off (0).
 * @return void
 */
void common_led_control(int led_on) {
    if (k_mutex_lock(&led_mutex, K_MSEC(MUTEX_TIMEOUT_MS)) == 0) {
    /* mutex successfully locked */
        gpio_pin_set_dt(&led, led_on);
        k_mutex_unlock(&led_mutex);
    } else {
        LOG_ERR("Cannot lock LED0");
    }
}

/**
 * @brief Function to turn the LED on.
 * @param arg1 Unused parameter.
 * @param arg2 Unused parameter.
 * @param arg3 Unused parameter.
 * @return void
 */
void led_on_function(void *arg1, void *arg2, void *arg3) {
    while(true) {
        LOG_INF("LED0 ON - led_on_function starts");
        common_led_control(1);
        k_msleep(LED_ON_SLEEP_TIME_MS);
    }

}

/**
 * @brief Function to turn the LED off.
 * @param arg1 Unused parameter.
 * @param arg2 Unused parameter.
 * @param arg3 Unused parameter.
 * @return void
 */
void led_off_function(void *arg1, void *arg2, void *arg3) {
    while (true) {
        LOG_INF("LED0 OFF - thread_led_off starts");
        common_led_control(0);
        k_msleep(LED_OFF_SLEEP_TIME_MS);
    }
}

/**
 * @brief main - Entry point for the Thread LED application
 * This function initializes the LED device, configures the GPIO pin,
 * and creates two threads to control the LED on and off states.
 *
 * @param void
 * @return 0 on success, non-zero on failure.
 */

int main(void)
{
    int ret;

    LOG_DBG("Thread LED - version %s", APP_VERSION_STRING);
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

#if (USE_KTHREAD_MACRO != 1)
    k_mutex_init(&led_mutex);
    k_tid_t led_on_tid = k_thread_create(&thread_led_on_s, thread_led_on_stack,
                                         K_THREAD_STACK_SIZEOF(thread_led_on_stack),
                                         led_on_function, NULL, NULL, NULL, ON_PRIORITY, 0, K_NO_WAIT);
    k_tid_t led_off_tid = k_thread_create(&thread_led_off_s, thread_led_off_stack,
                                         K_THREAD_STACK_SIZEOF(thread_led_off_stack),
                                         led_off_function, NULL, NULL, NULL, OFF_PRIORITY, 0, K_MSEC(LED_ON_SLEEP_TIME_MS));
    (void)led_on_tid; // To avoid warining
    (void)led_off_tid; // To avoid warining
#endif
    return 0;
}
