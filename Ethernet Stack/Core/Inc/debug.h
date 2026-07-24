/**
 ******************************************************************************
 * @file    debug.h
 * @brief   Debug output interface.
 *
 * Provides helper functions for formatted debug output over the UART
 * interface, including printing text, formatted strings, and Ethernet
 * MAC addresses.
 *
 ******************************************************************************
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
 * Public Types
 ******************************************************************************/

/* Enums */

/* Structures */

/******************************************************************************
 * Public Macros
 ******************************************************************************/

/* Public constants */

/******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Transmit a null-terminated string.
 *
 * Sends a text string over the configured debug interface.
 *
 * @param text Pointer to the null-terminated string.
 */
void debugPrint(const char *text);

/**
 * @brief Transmit a formatted string.
 *
 * Formats a string using printf-style formatting and sends it over the
 * configured debug interface.
 *
 * @param fmt Format string.
 * @param ... Variable argument list.
 */
void debugPrintf(const char *fmt, ...);


/**
 * @brief Print an Ethernet MAC address.
 *
 * Prints a 6-byte MAC address in hexadecimal notation
 * (for example: 02:80:E1:00:00:01).
 *
 * @param mac Pointer to the 6-byte MAC address.
 */
void printMac(const uint8_t *mac);

#endif
