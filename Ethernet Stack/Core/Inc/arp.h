/**
 ******************************************************************************
 * @file    arp.h
 * @brief   Address Resolution Protocol (ARP) interface.
 *
 * Provides functions and data structures for processing ARP packets.
 * Supports receiving ARP requests and transmitting ARP replies for the
 * embedded Ethernet stack.
 *
 ******************************************************************************
 */

#ifndef ARP_H
#define ARP_H

#include <stdint.h>
#include "ethernet_driver.h"

/******************************************************************************
 * Public Types
 ******************************************************************************/

#pragma pack(push,1)

/**
 * @brief ARP packet format.
 *
 * Represents the payload of an Ethernet frame with EtherType 0x0806.
 */
typedef struct
{
	uint16_t hardwareType;
	uint16_t protocolType;
	uint8_t hardwareLength;
	uint8_t protocolLength;
	uint16_t operation;

	uint8_t senderMac[6];
	uint8_t senderIp[4];

	uint8_t targetMac[6];
	uint8_t targetIp[4];
} ArpPacket;

#pragma pack(pop)

/**
 * @brief Complete Ethernet ARP frame.
 *
 * Combines the Ethernet header and ARP packet into a single
 * transmit/receive frame.
 */
typedef struct
{
	EthernetHeader ethernet;
	ArpPacket arp;

} ArpFrame;
/******************************************************************************
 * Public Macros
 ******************************************************************************/

/* Public constants */
#define ARP_REQUEST        1
#define ARP_REPLY          2

#define ARP_HW_ETHERNET    1
#define ARP_PROTO_IPV4     0x0800
#define ARP_IP_LENGTH 	   4
/******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Process a received ARP packet.
 *
 * Decodes and displays the contents of a received ARP request or reply.
 *
 * @param arp Pointer to the received ARP packet.
 */
void arpReceive(const ArpPacket *arp);

/**
 * @brief Transmit an ARP reply.
 *
 * Constructs and transmits an ARP reply in response to a received
 * ARP request addressed to the local device.
 *
 * @param rxHeader Pointer to the received Ethernet header.
 * @param request  Pointer to the received ARP request.
 */
void arpSendReply(EthernetHeader *rxHeader,
		 	 	   ArpPacket *request);

#endif
