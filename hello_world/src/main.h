/**
 * @file main.h
 * @brief Header file for the application main.c
 *
 * @author Changjae Kim
 * @date 2024-06
 */

/** @brief Default log level for the application. Set to LOG_LEVEL_DBG for detailed debug output. */
#define LOG_LEVEL LOG_LEVEL_DBG
/** @brief Sleeping time in milliseconds, 1000 = 1000 milliseconds. */
#define SLEEP_TIME_MS   1000
/** @brief The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
