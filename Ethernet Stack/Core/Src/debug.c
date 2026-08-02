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
#define DEBUG_BUFFER_SIZE 128

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
	 HAL_StatusTypeDef status = HAL_UART_Transmit(
	     &huart3,
	     (uint8_t *)text,
	     strlen(text),
	     HAL_MAX_DELAY);

	 if (status != HAL_OK)
	 {
	     // Future: count errors or set a debug flag
	 }
}

 /**
  * @brief Transmit a null-terminated string over the debug UART.
  *
  * @param text Pointer to the string to transmit.
  */
void debugPrintf(const char *fmt, ...)
{
	char buffer[DEBUG_BUFFER_SIZE];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	debugPrint(buffer);
}

void debugPrintLine(const char *text)
{
    debugPrint(text);
    debugPrint("\r\n");
}

/* Print ethernet mac */
void printMac(const uint8_t *mac)
{
    debugPrintf("%02X:%02X:%02X:%02X:%02X:%02X",
        mac[0], mac[1], mac[2],
        mac[3], mac[4], mac[5]);

	debugPrint("\n");
}

void debugInit(void)
{
	debugPrintLine("=================================");
	debugPrintLine(" Project 15 HTTP Web Server");
	debugPrintLine(" STM32 NUCLEO-H563ZI");
	debugPrintLine("=================================");

	debugPrintf("System Clock: %lu Hz\r\n", HAL_RCC_GetSysClockFreq());

	debugPrintLine("UART Debug Initialized");
}
