/**
 ******************************************************************************
 * @file    ethernet_frame.h
 * @brief   Ethernet frame definitions.
 *
 * Defines Ethernet frame structures and constants used for transmitting
 * and receiving Ethernet packets.
 *
 ******************************************************************************
 */

#ifndef ETHERNET_FRAME_H
#define ETHERNET_FRAME_H

#include <stdint.h>

/******************************************************************************
 * Private Constants
 ******************************************************************************/

/** Length of an Ethernet MAC address in bytes. */
#define ETH_MAC_LENGTH     6

/** Minimum Ethernet payload size in bytes. */
#define ETH_MIN_PAYLOAD_SIZE    46

/******************************************************************************
 * Public Types
 ******************************************************************************/

/**
 * @brief Ethernet frame header.
 *
 * Represents the 14-byte Ethernet header present at the beginning of every
 * Ethernet frame.
 */
typedef struct
{
    uint8_t destination[ETH_MAC_LENGTH];   /**< Destination MAC address. */
    uint8_t source[ETH_MAC_LENGTH];        /**< Source MAC address. */
    uint16_t etherType;                    /**< Encapsulated protocol type. */

} EthernetHeader;

/**
 * @brief Complete Ethernet frame.
 *
 * Represents an Ethernet frame consisting of a header and payload.
 * The payload contains the encapsulated protocol data (for example,
 * ARP or IPv4).
 */
typedef struct
{
    void *packet;
    EthernetHeader *header;
    uint16_t etherType;
    uint16_t length;

} EthernetFrame;


/******************************************************************************
 * Public Macros
 ******************************************************************************/

/* Public constants */

/******************************************************************************
 * Public Functions
 ******************************************************************************/




#endif
