/**
 * @file main.h
 * @brief Header file for the application main.c
 *
 * @author Changjae Kim
 * @date 2024-06
 */

/** @brief Default log level for the application. Set to LOG_LEVEL_DBG for detailed debug output. */
#define LOG_LEVEL LOG_LEVEL_DBG
/** @brief Sleeping time in milliseconds, 500 = 500 milliseconds. */
#define SLEEP_TIME_MS   500
/** @brief The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
/** @brief The devicetree node identifier for the "timer0" label. */
#define HW_TIMER DT_NODELABEL(timer0)
/** @brief Flag to enable random sleep time. */
#define RANDOM_SLEEP_TIME 1