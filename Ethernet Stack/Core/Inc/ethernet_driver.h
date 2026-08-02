/**
 ******************************************************************************
 * @file    ethernet_driver.h
 * @brief   Ethernet driver interface.
 *
 * Provides initialization, transmission, reception, and callback
 * functions for the STM32 Ethernet peripheral.
 *
 ******************************************************************************
 */

#ifndef ETHERNET_DRIVER_H
#define ETHERNET_DRIVER_H

#include "main.h"
#include "ethernet_frame.h"

/******************************************************************************
 * Public Types
 ******************************************************************************/

//typedef struct
//{
//	void *packet;
//	EthernetHeader *header;
//	uint16_t etherType;
//} EthernetFrame;

/******************************************************************************
 * Public Macros
 ******************************************************************************/

#define ETHERTYPE_IPV4    0x0800
#define ETHERTYPE_ARP     0x0806
#define ETHERTYPE_IPV6    0x86DD

#define PHY_REG_BCR     0
#define PHY_REG_BSR     1
#define PHY_REG_ID1     2
#define PHY_REG_ID2     3
#define PHY_REG_SCSR    31

/******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initialize the Ethernet peripheral.
 *
 * Configures the STM32 Ethernet MAC, DMA descriptors, PHY, and receive
 * buffers required for Ethernet communication.
 *
 * @return HAL_OK if initialization succeeded.
 * @return HAL_ERROR otherwise.
 */
HAL_StatusTypeDef ethernetInit(void);

/**
 * @brief Wait for the Ethernet link to become active.
 *
 * Polls the PHY until a valid network link is established.
 *
 * @return HAL_OK if the link is established.
 * @return HAL_TIMEOUT if the operation times out.
 * @return HAL_ERROR if an error occurs.
 */
HAL_StatusTypeDef ethernetWaitForLink(void);

/**
 * @brief Print Ethernet PHY status information.
 *
 * Displays the current PHY link state and configuration over the
 * debug interface.
 */
void ethernetPrintStatus(void);

/**
 * @brief Build an Ethernet frame.
 *
 * Initializes the Ethernet header prior to transmission.
 *
 * @param frame Pointer to the Ethernet frame.
 */
void ethernetBuildFrame(EthernetFrame *frame);

/**
 * @brief Transmit an Ethernet frame.
 *
 * Sends a completed Ethernet frame using the STM32 Ethernet MAC.
 *
 * @param frame Pointer to the Ethernet frame.
 *
 * @return HAL_OK if the frame was transmitted successfully.
 * @return HAL_ERROR otherwise.
 */
HAL_StatusTypeDef ethernetTransmit(const void *frame, uint16_t length);

/**
 * @brief Process a received Ethernet frame.
 *
 * Receives an Ethernet frame from the DMA, parses the Ethernet header,
 * and dispatches the payload to the appropriate protocol handler.
 *
 * @return HAL_OK if a frame was processed successfully.
 * @return HAL_ERROR otherwise.
 */
HAL_StatusTypeDef ethernetReceive(void);

/**
 * @brief Allocate a receive buffer.
 *
 * Callback invoked by the HAL to provide a receive buffer for DMA.
 *
 * @param buff Pointer to the receive buffer pointer.
 */
void HAL_ETH_RxAllocateCallback(uint8_t **buff);

/**
 * @brief Link received buffers.
 *
 * Callback invoked by the HAL to link received DMA buffers into a
 * complete Ethernet frame.
 *
 * @param pStart Pointer to the start of the linked buffer chain.
 * @param pEnd Pointer to the end of the linked buffer chain.
 * @param buff Pointer to the current receive buffer.
 * @param Length Length of the received buffer in bytes.
 */
void HAL_ETH_RxLinkCallback(void **pStart,
                            void **pEnd,
                            uint8_t *buff,
                            uint16_t Length);

#endif
