/**
 * LED Event - template for a simple Zephyr application
 * @file main.c
 * @brief A simple "LED Event" application for Zephyr RTOS on Raspberry Pi Pico2
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

/** @brief Structure to hold LED event data */
typedef struct event_data_s {
    struct k_event event;
    struct k_mutex event_data_mutex;
    uint32_t event_message; // on-off miliseconed time, 50 - LED on or off for 50 ms.
} event_data_t;

event_data_t led_event;
struct k_thread thread_led_on_s;
struct k_thread thread_led_off_s;
struct k_mutex led_mutex;
struct k_mutex csrand_mutex;
K_THREAD_STACK_DEFINE(thread_led_on_stack, STACKSIZE);
K_THREAD_STACK_DEFINE(thread_led_off_stack, STACKSIZE);

/**
 * @brief Initialize the LED event data structure.
 * @param p_led_event Pointer to the LED event data structure.
 * @return void
 */
void init_led_event(event_data_t *p_led_event) {
    k_event_init(&p_led_event->event);
    k_mutex_init(&p_led_event->event_data_mutex);
    p_led_event->event_message = 0;
}

/**
 * @brief Get the LED event message.
 * @param p_led_event Pointer to the LED event data structure.
 * @return The LED event message in milliseconds.
 */
uint32_t get_led_event_message(event_data_t *p_led_event) {
    uint32_t event_message;
    if (k_mutex_lock(&p_led_event->event_data_mutex, K_MSEC(MUTEX_TIMEOUT_MS)) == 0) {
        event_message = p_led_event->event_message;
        k_mutex_unlock(&p_led_event->event_data_mutex);
    } else {
        LOG_ERR("Cannot get LED event message");
        event_message = DEFAULT_LED_SLEEP_TIME_MS; // Default time in milliseconds if mutex lock fails
    }
    return event_message;
}

/**
 * @brief Set the LED event message.
 * @param p_led_event Pointer to the LED event data structure.
 * @param event_message The LED event message in milliseconds to set.
 * @return 0 on success, -1 on failure.
 */
int set_led_event_message(event_data_t *p_led_event, uint32_t event_message) {
    if (k_mutex_lock(&p_led_event->event_data_mutex, K_MSEC(MUTEX_TIMEOUT_MS)) == 0) {
        p_led_event->event_message = event_message;
        k_mutex_unlock(&p_led_event->event_data_mutex);
    } else {
        LOG_ERR("Cannot set LED event message");
        return -1; // Return error code if mutex lock fails
    }
    return 0; // Success
}

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
    if (k_mutex_lock(&csrand_mutex, K_MSEC(MUTEX_TIMEOUT_MS)) == 0) {
        if (sys_csrand_get(&rand_value, sizeof(rand_value)) < 0) {
            LOG_ERR("Failed to get random value for next timer interval, using default %d ms", default_time_ms);
        } else {
            k_mutex_unlock(&csrand_mutex);
            // Randomize the next timer interval between 100 ms and 1000 ms
            rand_time_ms = 100 + (rand_value % 901); // 100 to 1000 ms
        }
    } else {
        LOG_ERR("Cannot get CSRAND, using default %d ms", default_time_ms);
    }
    return rand_time_ms;
}

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
        LOG_ERR("Cannot get LED0");
    }
}

/**
 * @brief Send an LED event.
 * @param p_event_data Pointer to the event data structure.
 * @param event_mask The event mask to send.
 * @param thread_name The name of the thread sending the event.
 * @return 0 on success, -1 on failure.
 */
int send_led_event(event_data_t *p_event_data, uint32_t event_mask, const char *thread_name) {
    int result = 0;
    uint32_t event_message = 0; // Initialize event message
    if (event_mask != LED_EVENT_SYSTEM_ON) { // For LED_EVENT_SYSTEM_ON event, we do not need to set the event message, as it is only used to start the LED control threads. For other events, we need to set the event message to a random time for the next LED on/off interval.
        event_message = get_random_time_ms(DEFAULT_LED_SLEEP_TIME_MS);
        result = set_led_event_message(p_event_data, event_message);
        if (result != 0) {
            LOG_ERR("Failed to set LED event message. Use previous event message for next LED interval.");
        }
    }
    LOG_INF("%s thread - send 0x%x event, message=%d ms", thread_name, event_mask, event_message);
    k_event_post(&p_event_data->event, event_mask);
    return result;
}

/**
 * @brief Function to turn the LED on.
 * @param arg1 event_data_t structure pointer passed as an argument
 * @param arg2 Unused parameter.
 * @param arg3 Unused parameter.
 * @return void
 */
void led_on_main(void *arg1, void *arg2, void *arg3) {
    event_data_t *p_event_data = (event_data_t *)arg1;

    // Wait for the system on event to start the LED control thread
    k_event_wait(&p_event_data->event, LED_EVENT_SYSTEM_ON, false, K_FOREVER);
    LOG_INF("LED_ON thread - LED_EVENT_SYSTEM_ON event received, LED is ON now");
    common_led_control(1); // LED ON
    k_sleep(K_MSEC((SYSTEM_ON_SLEEP_TIME_MS))); // Sleep for other thread to check LED_EVENT_SYSTEM_ON event

    LOG_INF("LED_ON thread - LED_EVENT_SYSTEM_ON event bit is cleared");
    k_event_set_masked(&p_event_data->event, 0, LED_EVENT_SYSTEM_ON); // Clear the LED_EVENT_SYSTEM_ON event

    int result = send_led_event(p_event_data, LED_EVENT_OFF, "LED_ON");
    if (result != 0) {
        LOG_ERR("Failed to send LED_EVENT_OFF event");
        k_panic(); // Panic if we cannot send the LED_EVENT_OFF event, as the application cannot function properly without it
    }

    // Main loop to wait for LED on events and control the LED accordingly
    while(true) {
        k_event_wait(&p_event_data->event, LED_EVENT_ON, false, K_FOREVER);
        uint32_t sleep_time_ms = get_led_event_message(p_event_data);
        LOG_INF("LED_ON thread - LED_EVENT_ON event received, turn LED ON for %d ms", sleep_time_ms);
        common_led_control(1); // LED ON
        k_event_set_masked(&p_event_data->event, 0, LED_EVENT_ON); // Clear the LED_EVENT_SYSTEM_ON event
        k_sleep(K_MSEC((sleep_time_ms))); // Sleep for other thread to check LED_EVENT_SYSTEM_ON event

        int result = send_led_event(p_event_data, LED_EVENT_OFF, "LED_ON");
        if (result != 0) {
            LOG_ERR("Failed to send LED_EVENT_OFF event");
            k_panic(); // Panic if we cannot send the LED_EVENT_OFF event, as the application cannot function properly without it
        }
    }
}

/**
 * @brief Function to turn the LED off.
 * @param arg1 event_data_t structure pointer passed as an argument
 * @param arg2 Unused parameter.
 * @param arg3 Unused parameter.
 * @return void
 */
void led_off_main(void *arg1, void *arg2, void *arg3) {
    event_data_t *p_event_data = (event_data_t *)arg1;

    // Wait for the system on event to start the LED control thread
    k_event_wait(&p_event_data->event, LED_EVENT_SYSTEM_ON, false, K_FOREVER);
    LOG_INF("LED_OFF thread - LED_EVENT_SYSTEM_ON event received");
    k_sleep(K_MSEC((SYSTEM_ON_SLEEP_TIME_MS))); // Sleep for other thread to check LED_EVENT_SYSTEM_ON event
    LOG_INF("LED_OFF thread - LED_EVENT_SYSTEM_ON event bit is cleared");
    k_event_set_masked(&p_event_data->event, 0, LED_EVENT_SYSTEM_ON); // Clear the LED_EVENT_SYSTEM_ON event

    // Main loop to wait for LED on events and control the LED accordingly
    while(true) {
        k_event_wait(&p_event_data->event, LED_EVENT_OFF, false, K_FOREVER);
        uint32_t sleep_time_ms = get_led_event_message(p_event_data);
        LOG_INF("LED_OFF thread - LED_EVENT_OFF event received, turn LED OFF for %d ms", sleep_time_ms);
        common_led_control(0); // LED OFF
        k_event_set_masked(&p_event_data->event, 0, LED_EVENT_OFF); // Clear the LED_EVENT_SYSTEM_ON event
        k_sleep(K_MSEC((sleep_time_ms))); // Sleep for other thread to check LED_EVENT_SYSTEM_ON event

        int result = send_led_event(p_event_data, LED_EVENT_ON, "LED_OFF");
        if (result != 0) {
            LOG_ERR("Failed to send LED_EVENT_ON event");
            k_panic(); // Panic if we cannot send the LED_EVENT_ON event, as the application cannot function properly without it
        }
    }
}

/**
 * @brief main - Entry point for the LED Event application
 *
 * @param void
 * @return 0 on success, non-zero on failure.
 */

int main(void)
{
    int ret;

    LOG_DBG("LED Event - version %s", APP_VERSION_STRING);
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

    k_mutex_init(&led_mutex);
    k_mutex_init(&csrand_mutex);

    init_led_event(&led_event);
    k_tid_t led_on_tid = k_thread_create(&thread_led_on_s, thread_led_on_stack,
                                         K_THREAD_STACK_SIZEOF(thread_led_on_stack),
                                         led_on_main, (void *)&led_event, NULL, NULL, ON_PRIORITY, 0, K_NO_WAIT);
    k_tid_t led_off_tid = k_thread_create(&thread_led_off_s, thread_led_off_stack,
                                         K_THREAD_STACK_SIZEOF(thread_led_off_stack),
                                         led_off_main, (void *)&led_event, NULL, NULL, OFF_PRIORITY, 0, K_NO_WAIT);

    send_led_event(&led_event, LED_EVENT_SYSTEM_ON, "main");
 
    (void)led_on_tid; // To avoid warining
    (void)led_off_tid; // To avoid warining
    // Send initial LED on event
    
    return 0;
}
