/**
 * @file main.h
 * @brief Header file for the thread LED application.
 *
 * This header file defines constants, macros, and function prototypes for controlling an LED
 * using threads and mutexes in a Zephyr-based application. It includes definitions for log levels,
 * thread priorities, sleep times, and the devicetree node for the LED.
 *
 * @author Changjae Kim
 * @date 2024-06
 */

/** @brief Default log level for the application. Set to LOG_LEVEL_DBG for detailed debug output. */
#define LOG_LEVEL LOG_LEVEL_DBG

/** @brief Macro to enable the use of k_thread macros. */
#define USE_KTHREAD_MACRO 0

/** @brief The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

/** @brief Sleep time in milliseconds for the LED on state */
#define LED_ON_SLEEP_TIME_MS 500
/** @brief Sleep time in milliseconds for the LED off state */
#define LED_OFF_SLEEP_TIME_MS 500
/** @brief Stack size for the threads */
#define STACKSIZE 1024
/** @brief Priority for the LED on thread */
#define ON_PRIORITY 7
/** @brief Priority for the LED off thread */
#define OFF_PRIORITY 8
/** @brief Mutex timeout in milliseconds */
#define MUTEX_TIMEOUT_MS 100

/** @brief thread function to turn the LED on and off*/
void led_on_function(void *arg1, void *arg2, void *arg3);
void led_off_function(void *arg1, void *arg2, void *arg3);
