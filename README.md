# STM32 Bare-Metal Ethernet Stack

## Overview

This project implements a lightweight Ethernet networking stack for the **STM32 NUCLEO-H563ZI** using the STM32 HAL Ethernet driver. Rather than relying on middleware such as **LwIP**, the project builds the networking stack from the ground up by manually parsing and constructing Ethernet frames.

The project demonstrates how Ethernet communication works at the packet level, including frame reception, protocol dispatching, ARP request/reply handling, IPv4 frame recognition, and ICMP Echo Reply (Ping). Network traffic was verified using **Wireshark** and standard network utilities.

---

## Features

- Ethernet MAC and DMA initialization
- PHY link detection and diagnostics
- Receive buffer management using HAL callbacks
- Ethernet frame parsing
- EtherType protocol dispatch
- ARP request parsing
- ARP reply generation
- IPv4 frame detection
- ICMP Echo Reply (Ping)
- UART debug interface
- Wireshark packet verification

---
## Software Architecture


```
 Application
      │
      ▼
     ARP
      │
      ▼
Ethernet Driver
      │
      ▼
STM32 HAL Ethernet
      │
      ▼
Ethernet MAC / DMA
      │
      ▼
     PHY
      │
      ▼
Ethernet Network
```


Incoming Ethernet frames are processed by the Ethernet driver, which parses the Ethernet header and dispatches packets to the appropriate protocol handler based on the EtherType field.


```
HAL_ETH_ReadData()
      │
      ▼
Ethernet Header
      │
      ▼
  EtherType
      │
      ├── ARP
      ├── IPv4
      └── IPv6     
```

---

## ARP Processing

When an ARP request is received, the project:

1. Parses the Ethernet header.
2. Parses the ARP packet.
3. Verifies that the requested IP address matches the local device.
4. Constructs an ARP reply.
5. Transmits the reply using the STM32 Ethernet HAL.


```
      ARP Request
            │
            ▼
      Parse Packet
            │
            ▼
      Verify Target IP
            │
            ▼
      Build Reply
            │
            ▼
Transmit Ethernet Frame
```


---

## Ethernet Transmission

Outgoing Ethernet frames are transmitted through a generic Ethernet transmit interface.

```c
HAL_StatusTypeDef ethernetTransmit(const void *frame,
                                   uint16_t length);
```

This abstraction allows multiple protocols (ARP, IPv4, ICMP, and future protocols) to share the same transmission path without directly accessing the STM32 HAL.

---

## Hardware

- STM32 NUCLEO-H563ZI
- On-board Ethernet PHY
- Ethernet switch/router
- PC running Wireshark and Tera Term

---

## Tools

- STM32CubeIDE
- STM32 HAL Drivers
- Tera Term
- Wireshark

---

## Verification

The implementation was verified by:

- Establishing a valid Ethernet link.
- Receiving Ethernet frames.
- Successfully responding to ARP requests.
- Verifying Ethernet packets in Wireshark.
- Successfully responding to ICMP Echo Requests (Ping).

Example:

```
C:\> ping 192.168.7.200

Reply from 192.168.7.200:
Reply from 192.168.7.200:
Reply from 192.168.7.200:
Reply from 192.168.7.200
```

---

## Concepts Demonstrated

- Ethernet frame format
- MAC addressing
- EtherType parsing
- DMA buffer management
- STM32 HAL Ethernet driver
- ARP protocol
- IPv4 packet processing
- ICMP Echo Reply
- Network debugging with Wireshark
- Embedded driver architecture

---

## Future Improvements

- UDP implementation
- TCP implementation
- DHCP client
- HTTP web server
- Network configuration module
- Additional protocol support

---

## License

This project is provided for educational purposes.
