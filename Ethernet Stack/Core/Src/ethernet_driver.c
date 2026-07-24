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
#include "arp.h"
#include <string.h>

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
/******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Private Functions
 ******************************************************************************/



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

HAL_StatusTypeDef ethernetReceive(void)
{
    void *rxPacket;

    if (HAL_ETH_ReadData(&heth, &rxPacket) != HAL_OK)
    {
    	return HAL_ERROR;
    }

    debugPrint("Frame Received!\r\n");

    debugPrintf("Packet = %p\r\n", rxPacket);


    EthernetHeader *rxHeader = (EthernetHeader *)rxPacket;

    printMac(rxHeader->destination);
    printMac(rxHeader->source);

    uint16_t etherType =
            __builtin_bswap16(rxHeader->etherType);

    debugPrintf("EtherType = 0x%04X\r\n",
                etherType);

    switch (etherType)
    {
    case ETHERTYPE_ARP:
    {
        debugPrint("ARP\r\n");

        uint8_t *payload =
        		(uint8_t *)rxPacket + sizeof(EthernetHeader);

        ArpPacket *arp = (ArpPacket *)payload;



        for(int i = 0; i < sizeof(ArpPacket); i++)
        {
            debugPrintf("%02X ",
                ((uint8_t*)arp)[i]);

            if((i+1)%16==0)
                debugPrint("\r\n");
        }

        arpReceive(arp);

        arpSendReply(rxHeader, arp);

        break;
    }
    case ETHERTYPE_IPV4:
    {
        debugPrint("IPv4\r\n");
        break;
    }
    case ETHERTYPE_IPV6:
    {
        debugPrint("IPv6\r\n");
        break;
    }
    default:
    {
    	return HAL_OK;
    }
    /* End switch */
    }

    return HAL_OK;
}

void HAL_ETH_RxAllocateCallback(uint8_t **buff)
{
    debugPrint("Allocate Callback\r\n");

    *buff = rxBuffers[rxBufferIndex];

    rxBufferIndex++;

    if(rxBufferIndex >= ETH_RX_BUFFER_COUNT)
    {
        rxBufferIndex = 0;
    }
}


void HAL_ETH_RxLinkCallback(void **pStart,
                            void **pEnd,
                            uint8_t *buff,
                            uint16_t Length)
{
    (void)Length;

    *pStart = buff;
    *pEnd = buff;
}




