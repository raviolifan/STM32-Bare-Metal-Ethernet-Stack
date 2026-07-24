/**
 ******************************************************************************
 * @file    driver.c
 * @brief   Driver implementation.
 ******************************************************************************
 */

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "string.h"
#include "main.h"
#include <stdio.h>
#include <stdarg.h>

#include "stm32h5xx_hal.h"
#include "debug.h"
/******************************************************************************
 * Private Types
 ******************************************************************************/

/* Private structures */

/******************************************************************************
 * Private Constants
 ******************************************************************************/

/* Static configuration tables */
extern UART_HandleTypeDef huart3;

/******************************************************************************
 * Private Variables
 ******************************************************************************/

/* Runtime state */

/******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Private Functions
 ******************************************************************************/

/*
 * Retrieve the runtime state for the requested button.
 *
 * Returns NULL if the button identifier is invalid.
 */

/******************************************************************************
 * Public Functions
 ******************************************************************************/

 /**
 * @brief Turn an LED on.
 *
 * @param led LED identifier.
 *
 * @return true if successful.
 * @return false otherwise.
 */
 void debugPrint(const char *text)
{
	HAL_UART_Transmit(&huart3,
					  (uint8_t *)text,
					  strlen(text),
					  HAL_MAX_DELAY);
}

void debugPrintf(const char *fmt, ...)
{
	char buffer[128];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	debugPrint(buffer);
}

/* Print ethernet mac */
void printMac(const uint8_t *mac)
{
    debugPrintf("%02X:%02X:%02X:%02X:%02X:%02X",
        mac[0], mac[1], mac[2],
        mac[3], mac[4], mac[5]);

	debugPrint("\n");
}
