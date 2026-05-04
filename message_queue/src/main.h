/**
 * @file main.h
 * @brief Header file for the application main.c
 *
 * @author Changjae Kim
 * @date 2026-05-03
 */

/** @brief Default log level for the application. Set to LOG_LEVEL_DBG for detailed debug output. */
#define LOG_LEVEL LOG_LEVEL_DBG
/** @brief The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
/** @brief The devicetree node identifier for the "timer0" label. */
#define HW_TIMER DT_NODELABEL(timer0)

/** @brief Stack size for the threads */
#define STACKSIZE 2048
/** @brief Priority for the LED on thread */
#define PRIORITY 7
/** @brief Mutex timeout in milliseconds */
#define MUTEX_TIMEOUT_MS 100

#define TIMER_INTERVAL_MS 1000 // Timer interval in milliseconds

#define EVENT_SYSTEM_ON             1 // BIT 0 
#define EVENT_1S_TIMER_EXPIRED      2 // BIT 1

#define SENSOR_DATA_MSGQ_SIZE       10 // Maximum number of messages in the queue
#define AGGREGATED_DATA_PIPE_SIZE   10 // Maximum number of messages in the pipe