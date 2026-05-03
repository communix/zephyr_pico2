/**
 * @file main.h
 * @brief Header file for the application main.c
 *
 * @author Changjae Kim
 * @date 2024-06
 */

/** @brief Default log level for the application. Set to LOG_LEVEL_DBG for detailed debug output. */
#define LOG_LEVEL LOG_LEVEL_DBG
/** @brief Sleeping time in milliseconds, 50 = 50 milliseconds. */
#define SYSTEM_ON_SLEEP_TIME_MS   50
/** @brief Default sleep time for the LED in milliseconds. */
#define DEFAULT_LED_SLEEP_TIME_MS 500
/** @brief The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

/** @brief Stack size for the threads */
#define STACKSIZE 1024
/** @brief Priority for the LED on thread */
#define PRIORITY 7
/** @brief Mutex timeout in milliseconds */
#define MUTEX_TIMEOUT_MS 100

#define LED_EVENT_SYSTEM_ON 1 // BIT 0 
#define LED_EVENT_ON        2 // BIT 1
#define LED_EVENT_OFF       4 // BIT 2

#define LED_ON 1
#define LED_OFF 0
