/**
 ******************************************************************************
 * @file    arp.c
 * @brief   Address Resolution Protocol (ARP) implementation.
 *
 * Implements ARP packet processing and ARP reply generation for the
 * embedded Ethernet stack.
 *
 ******************************************************************************
 */

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>
#include "arp.h"
#include "debug.h"
#include "main.h"
#include "ethernet_frame.h"
#include "ethernet_driver.h"
/******************************************************************************
 * Private Types
 ******************************************************************************/

/* Private structures */
extern ETH_HandleTypeDef heth;

/******************************************************************************
 * Private Constants
 ******************************************************************************/

/* Static configuration tables */
static uint8_t localMac[6] =
{
		0x02,
		0x80,
		0xE1,
		0X00,
		0X00,
		0X01
};

static uint8_t localIp[4] =
{
		192,
		168,
		7,
		200
};
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
 * @brief Process a received ARP packet.
 *
 * Displays information contained in a received ARP request or reply.
 *
 * @param arp Pointer to the received ARP packet.
 */
 void arpReceive(const ArpPacket *arp)
 {
	 debugPrint("\r\n==== ARP Packet =====\r\n");

	 debugPrintf("Operation: %u\r\n",
			 __builtin_bswap16(arp->operation));

	 debugPrintf("Sender IP: %u.%u.%u.%u\r\n",
			 arp->senderIp[0],
			 arp->senderIp[1],
			 arp->senderIp[2],
			 arp->senderIp[3]);

	 debugPrintf("Target IP: %u.%u.%u.%u\r\n",
			 arp->targetIp[0],
			 arp->targetIp[1],
			 arp->targetIp[2],
			 arp->targetIp[3]);
 }

 /**
  * @brief Transmit an ARP reply.
  *
  * Constructs an ARP reply using the received Ethernet and ARP headers
  * and transmits it through the STM32 Ethernet MAC.
  *
  * @param rxHeader Pointer to the received Ethernet header.
  * @param request Pointer to the received ARP request.
  */
 void arpSendReply(EthernetHeader *rxHeader,
		 	 	   ArpPacket *request)
 {
	 static ArpFrame reply;

	 /* Verify the ARP request is for this device. */
	 if(memcmp(request->targetIp, localIp, 4) != 0)
	 {
	     return;
	 }

	 /* Build Ethernet header. */
	 memcpy(reply.ethernet.destination,
			 rxHeader->source,
			 ETH_MAC_LENGTH);

	 memcpy(reply.ethernet.source,
			 localMac,
			 ETH_MAC_LENGTH);


	 reply.ethernet.etherType 	= __builtin_bswap16(ETHERTYPE_ARP);

	 reply.arp.hardwareType 	= __builtin_bswap16(ARP_HW_ETHERNET);
	 reply.arp.protocolType 	= __builtin_bswap16(ARP_PROTO_IPV4);

	 reply.arp.hardwareLength	= ETH_MAC_LENGTH;
	 reply.arp.protocolLength 	= ARP_IP_LENGTH;

	 reply.arp.operation 		= __builtin_bswap16(ARP_REPLY);

	 /* Build ARP packet. */
	 memcpy(reply.arp.senderMac,
			 localMac,
			 ETH_MAC_LENGTH);

	 memcpy(reply.arp.senderIp,
			 localIp,
			 ARP_IP_LENGTH);

	 memcpy(reply.arp.targetMac,
			 request->senderMac,
			 ETH_MAC_LENGTH);

	 memcpy(reply.arp.targetIp,
			 request->senderIp,
			 ARP_IP_LENGTH);

	 /* Configure transmit buffer. */
	 ETH_BufferTypeDef txBuffer = {0};

	 txBuffer.buffer = (uint8_t *)&reply;
	 txBuffer.len    = sizeof(ArpFrame);
	 txBuffer.next   = NULL;

	 ETH_TxPacketConfigTypeDef txConfig = {0};

	 txConfig.Length 	= sizeof(ArpFrame);
	 txConfig.TxBuffer 	= &txBuffer;
	 txConfig.pData 	= &reply;

	 debugPrint("\r\n===== ARP Reply =====\r\n");

	 /* Print packet contents */
	 debugPrintf("Operation: %u\r\n",
	             __builtin_bswap16(reply.arp.operation));

	 debugPrint("Sender MAC: ");
	 printMac(reply.arp.senderMac);

	 debugPrint("Target MAC: ");
	 printMac(reply.arp.targetMac);

	 debugPrintf("Sender IP: %u.%u.%u.%u\r\n",
	         reply.arp.senderIp[0],
	         reply.arp.senderIp[1],
	         reply.arp.senderIp[2],
	         reply.arp.senderIp[3]);

	 debugPrintf("Target IP: %u.%u.%u.%u\r\n",
	         reply.arp.targetIp[0],
	         reply.arp.targetIp[1],
	         reply.arp.targetIp[2],
	         reply.arp.targetIp[3]);

	 debugPrint("Destination MAC: ");
	 printMac(reply.ethernet.destination);

	 debugPrint("Source MAC: ");
	 printMac(reply.ethernet.source);

	 /* Transmit ARP reply. */
	 if (ethernetTransmit(&reply, sizeof(reply)) == HAL_OK)
	 {
		 debugPrint("ARP Reply Sent!\r\n");
	 }
	 else
	 {
		 debugPrint("ARP Reply Failed!\r\n");
	 }
 }

