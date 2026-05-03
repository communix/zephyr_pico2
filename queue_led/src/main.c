/**
 * Queue LED - template for a simple Zephyr application
 * @file main.c
 * @brief A simple "Queue LED" application for Zephyr RTOS on Raspberry Pi Pico2
 */

#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/random/random.h>
// For the version control
#include <app_version.h>
#include "main.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL);

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/** @brief Structure to hold Queue LED data */
typedef struct queue_data_s {
    uint16_t led_control; // LED_CTRL_ON or LED_CTRL_OFF This is actual control message
    uint16_t message; // on-off miliseconed time, 50 - LED on or off for 50 ms. This is not used by LED controller, but just for logging purpose to show the random time generated for each LED on/off interval.
} queue_data_t;

struct k_thread led_controller_s;
struct k_thread led_driver_s;
K_THREAD_STACK_DEFINE(led_controller_stack, STACKSIZE);
K_THREAD_STACK_DEFINE(led_driver_stack, STACKSIZE);

struct k_fifo queue;

/**
 * @brief Get a random time in milliseconds for the next LED on/off interval.
 * This function uses the sys_csrand_get API to obtain a random value and calculates
 * a random time between 100 ms and 1000 ms. If the random value cannot be obtained,
 * it returns a default time provided as an argument.
 *
 * @param default_time_ms The default time in milliseconds to use if random value retrieval fails.
 * @return A random time in milliseconds for the next LED interval.
 */

uint32_t get_random_time_ms(uint32_t default_time_ms) {
    uint32_t rand_value;
    uint32_t rand_time_ms = default_time_ms; // Output random time in milliseconds
    if (sys_csrand_get(&rand_value, sizeof(rand_value)) < 0) {
        LOG_ERR("Failed to get random value for next timer interval, using default %d ms", default_time_ms);
    } else {
        // Randomize the next timer interval between 100 ms and 1000 ms
        rand_time_ms = 100 + (rand_value % 901); // 100 to 1000 ms
    }
    return rand_time_ms;
}

/** @brief Toggle the state of the LED.
 * @param current_state The current state of the LED.
 * @return The new state of the LED.
 */
static inline uint8_t toggle_led_state(uint8_t current_state) {
    return current_state == LED_ON ? LED_OFF : LED_ON; // Toggle between ON and OFF
}

/** @brief Main function for the LED controller thread.
 * @param arg1 Pointer to the queue.
 * @param arg2 Unused.
 * @param arg3 Unused.
 */
void led_controller_main(void *arg1, void *arg2, void *arg3) {
        // led_controller thread implementation
        struct k_fifo *queue = (struct k_fifo *)arg1;
        uint8_t led_state = LED_OFF; // Start with LED off
        while (1) {
            uint32_t sleep_time_ms = get_random_time_ms(DEFAULT_LED_SLEEP_TIME_MS);
            led_state = toggle_led_state(led_state); // Toggle LED state
            queue_data_t *p_data = k_malloc(sizeof(queue_data_t));
            p_data->led_control = led_state; // Control value to turn LED on or off
            p_data->message = (uint16_t)sleep_time_ms;
            k_fifo_put(queue, p_data); // Send data to the queue
            k_msleep(sleep_time_ms); // Sleep for the random time before toggling the
        }
    // led_controller thread implementation
}

/** @brief Main function for the LED driver thread.
 * @param arg1 Pointer to the queue.
 * @param arg2 Unused.
 * @param arg3 Unused.
 */
void led_driver_main(void *arg1, void *arg2, void *arg3) {
    // led_driver thread implementation
    struct k_fifo *queue = (struct k_fifo *)arg1;
    while (1) {
        queue_data_t *p_data = k_fifo_get(queue, K_FOREVER); // Wait for data from the queue
        if (p_data) {
            gpio_pin_set(led.port, led.pin, p_data->led_control); // Set the LED state based on the control value
            LOG_INF("Message is received: Turn %s LED", p_data->led_control == LED_ON ? "ON" : "OFF");
            LOG_INF("Next message will be sent after %d ms", p_data->message);
            k_free(p_data); // Free the allocated memory for the queue data
        }
    }
}

/**
 * @brief main - Entry point for the Queue LED application
 *
 * @param void
 * @return 0 on success, non-zero on failure.
 */

int main(void)
{
    int ret;

    LOG_DBG("Queue LED - version %s", APP_VERSION_STRING);
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

    k_fifo_init(&queue);
    k_tid_t led_controller_tid = k_thread_create(&led_controller_s, led_controller_stack,
                                         K_THREAD_STACK_SIZEOF(led_controller_stack),
                                         led_controller_main, (void *)&queue, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
    k_tid_t led_driver_tid = k_thread_create(&led_driver_s, led_driver_stack,
                                         K_THREAD_STACK_SIZEOF(led_driver_stack),
                                         led_driver_main, (void *)&queue, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
 
    (void)led_controller_tid; // To avoid warining
    (void)led_driver_tid; // To avoid warining

    return 0;
}
