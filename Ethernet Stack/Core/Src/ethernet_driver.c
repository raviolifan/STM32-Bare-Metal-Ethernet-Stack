/**
 ******************************************************************************
 * @file    ethernet_driver.c
 * @brief   Ethernet driver implementation.
 *
 * Implements initialization, transmission, reception, and PHY management
 * for the STM32 Ethernet peripheral.
 *
 ******************************************************************************
 */

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "ethernet_driver.h"
#include "main.h"
#include "debug.h"
#include <string.h>
#include "arp.h"
#include "ethernet_frame.h"
#include "ipv4.h"
/******************************************************************************
 * Private Types
 ******************************************************************************/

extern ETH_HandleTypeDef heth;
extern ETH_TxPacketConfigTypeDef TxConfig;

/******************************************************************************
 * Private Constants
 ******************************************************************************/

#define ETH_RX_BUFFER_SIZE   1524
#define ETH_RX_BUFFER_COUNT  ETH_RX_DESC_CNT

/******************************************************************************
 * Private Variables
 ******************************************************************************/

static uint8_t rxBuffers[ETH_RX_BUFFER_COUNT][ETH_RX_BUFFER_SIZE];
static uint32_t rxBufferIndex = 0;
static uint16_t rxFrameLength;
/******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/
static void ethernetDispatchFrame(EthernetFrame *frame);

//static void ethernetPrintFrame(EthernetFrame *frame);

static uint16_t ethernetGetEtherType(EthernetHeader *header);

static void ethernetHandleArp(EthernetFrame *frame);

static void ethernetHandleIpv4(EthernetFrame *frame);

static void ethernetHandleIpv6(EthernetFrame *frame);
/******************************************************************************
 * Private Functions
 ******************************************************************************/

static void ethernetDispatchFrame(EthernetFrame *frame)
{
//	debugPrintf("EtherType = 0x%04X\r\n", frame->etherType);
    switch (frame->etherType)
    {
    case ETHERTYPE_ARP:
    {
    	ethernetHandleArp(frame);
        break;
    }
    case ETHERTYPE_IPV4:
    {
    	ethernetHandleIpv4(frame);
        break;
    }
    case ETHERTYPE_IPV6:
    {
    	ethernetHandleIpv6(frame);
        break;
    }
    default:
    {
        /* Ignore unsupported Ethernet protocols. */
    	return;
    }
    }
}

static void ethernetHandleArp(EthernetFrame *frame)
{
    debugPrint("Dispatching ARP\r\n");

    arpReceiveFrame(frame);
}

static void ethernetHandleIpv4(EthernetFrame *frame)
{
    debugPrint("IPv4\r\n");

    ipv4ReceiveFrame(frame);
}

static void ethernetHandleIpv6(EthernetFrame *frame)
{
    debugPrint("IPv6\r\n");
}


//static void ethernetPrintFrame(EthernetFrame *frame)
//{
//	debugPrint("Frame Received!\r\n");
//
//	debugPrintf("Packet = %p\r\n", frame->packet);
//
//	printMac(frame->header->destination);
//	printMac(frame->header->source);
//
//	debugPrintf("EtherType = 0x%04X\r\n",
//	            frame->etherType);
//}

static uint16_t ethernetGetEtherType(EthernetHeader *header)
{
    return __builtin_bswap16(header->etherType);
}

/******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initialize the Ethernet peripheral.
 *
 * Starts the STM32 Ethernet MAC and prepares the peripheral for
 * network communication.
 *
 * @return HAL_OK if initialization succeeds.
 * @return HAL_ERROR otherwise.
 */
HAL_StatusTypeDef ethernetInit(void)
{
	debugPrint("\r\n");
	debugPrint("=====================================\r\n");
	debugPrint(" STM32 Ethernet Diagnostics\r\n");
	debugPrint("=====================================\r\n");
	debugPrint("Ethernet Started\r\n");

    return HAL_ETH_Start(&heth);
}

/**
 * @brief Wait for an active Ethernet link.
 *
 * Polls the PHY until a valid Ethernet link is established or the
 * timeout expires.
 *
 * @return HAL_OK if the Ethernet link is established.
 * @return HAL_TIMEOUT if the timeout expires.
 */
HAL_StatusTypeDef ethernetWaitForLink(void)
{
    uint32_t bsr;

    debugPrint("Waiting for link...\r\n");

    uint32_t timeout = HAL_GetTick();

    while ((HAL_GetTick() - timeout) < 5000)
    {
        HAL_Delay(100);

        HAL_ETH_ReadPHYRegister(&heth,PHY_REG_BCR,PHY_REG_BSR,&bsr);

        if (bsr & (1<<2))
        {
            debugPrint("Link Established!\r\n");
            return HAL_OK;
        }
    }

    if (bsr & (1 << 2))
    {
  	  debugPrint("Link: UP\r\n");
    }
    else
    {
  	  debugPrint("Link: DOWN\r\n");
    }

    if (bsr & (1 << 5))
    {
  	  debugPrint("Auto Negotiation: Complete\r\n");
    }
    else
    {
  	  debugPrint("Auto Negotiation: In Progress\r\n");
    }

    debugPrint("Link timeout!\r\n");

    return HAL_TIMEOUT;
}

/**
 * @brief Display Ethernet PHY status.
 *
 * Reads and prints the PHY identification and status registers
 * over the debug interface.
 */
void ethernetPrintStatus(void)
{
    uint32_t phyId1;
    uint32_t phyId2;
    uint32_t bcr;
    uint32_t scsr;

    HAL_ETH_ReadPHYRegister(&heth,PHY_REG_BCR,PHY_REG_ID1,&phyId1);
    HAL_ETH_ReadPHYRegister(&heth,PHY_REG_BCR,PHY_REG_ID2,&phyId2);
    HAL_ETH_ReadPHYRegister(&heth,PHY_REG_BCR,PHY_REG_BCR,&bcr);
    HAL_ETH_ReadPHYRegister(&heth,PHY_REG_BCR,PHY_REG_SCSR,&scsr);

    debugPrintf("PHY ID1 = 0x%04X\r\n",(uint16_t)phyId1);
    debugPrintf("PHY ID2 = 0x%04X\r\n",(uint16_t)phyId2);

    debugPrintf("BCR = 0x%04X\r\n",(uint16_t)bcr);
    debugPrintf("SCSR = 0x%04X\r\n",(uint16_t)scsr);
}

/**
 * @brief Transmit an Ethernet frame.
 *
 * Configures the transmit DMA descriptor and sends an Ethernet
 * frame through the STM32 Ethernet MAC.
 *
 * @param frame Pointer to the Ethernet frame buffer.
 * @param length Length of the Ethernet frame in bytes.
 *
 * @return HAL_OK if transmission succeeds.
 * @return HAL_ERROR otherwise.
 */
HAL_StatusTypeDef ethernetTransmit(const void *frame, uint16_t length)
{
    ETH_BufferTypeDef txBuffer;

    txBuffer.buffer = (uint8_t *)frame;
    txBuffer.len    = length;
    txBuffer.next   = NULL;

    TxConfig.Length   = length;
    TxConfig.TxBuffer = &txBuffer;

    return HAL_ETH_Transmit(
            &heth,
            &TxConfig,
            HAL_MAX_DELAY);
}

/**
 * @brief Receive and process an Ethernet frame.
 *
 * Retrieves a received Ethernet frame from the DMA, parses the
 * Ethernet header, and dispatches the payload to the appropriate
 * protocol handler based on the EtherType field.
 *
 * @return HAL_OK if a frame is processed successfully.
 * @return HAL_ERROR if no frame is available or an error occurs.
 */
HAL_StatusTypeDef ethernetReceive(void)
{
    void *rxPacket;

    /* Retrieve the next received Ethernet frame. */
    if (HAL_ETH_ReadData(&heth, &rxPacket) != HAL_OK)
    {
    	return HAL_ERROR;
    }

    /* Parse the Ethernet header. */
    EthernetFrame frame;

    frame.packet = rxPacket;
    frame.header = (EthernetHeader *)rxPacket;
    frame.etherType = ethernetGetEtherType(frame.header);
    frame.length = rxFrameLength;

//    ethernetPrintFrame(&frame);

    /* Dispatch the payload based on the EtherType field. */
    ethernetDispatchFrame(&frame);

    return HAL_OK;
}

/**
 * @brief Allocate a receive buffer.
 *
 * HAL callback invoked when a new receive buffer is required.
 * Supplies the next available receive buffer from the buffer pool.
 *
 * @param buff Pointer to the receive buffer pointer.
 */
void HAL_ETH_RxAllocateCallback(uint8_t **buff)
{
//    debugPrint("Allocate Callback\r\n");

    *buff = rxBuffers[rxBufferIndex];

    rxBufferIndex++;

    if(rxBufferIndex >= ETH_RX_BUFFER_COUNT)
    {
        rxBufferIndex = 0;
    }
}

/**
 * @brief Link received DMA buffers.
 *
 * HAL callback used to associate received DMA buffers with the
 * received Ethernet frame.
 *
 * @param pStart Pointer to the first buffer in the frame.
 * @param pEnd Pointer to the last buffer in the frame.
 * @param buff Pointer to the current receive buffer.
 * @param Length Length of the received buffer in bytes.
 */
void HAL_ETH_RxLinkCallback(void **pStart,
                            void **pEnd,
                            uint8_t *buff,
                            uint16_t Length)
{
    *pStart = buff;
    *pEnd = buff;

    rxFrameLength = Length;
}




