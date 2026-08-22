/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <netinet/ip.h> IPTOS catalog for OpenSSH defines.h.
 * Dual DoD B OPEN. Soft!=product. G-AC-1.
 */
#pragma once

#ifndef IPTOS_LOWDELAY
#define IPTOS_LOWDELAY    0x10
#endif
#ifndef IPTOS_THROUGHPUT
#define IPTOS_THROUGHPUT  0x08
#endif
#ifndef IPTOS_RELIABILITY
#define IPTOS_RELIABILITY 0x04
#endif
#ifndef IPTOS_LOWCOST
#define IPTOS_LOWCOST     0x02
#endif
#ifndef IPTOS_MINCOST
#define IPTOS_MINCOST     IPTOS_LOWCOST
#endif
#ifndef IPTOS_DSCP_LE
#define IPTOS_DSCP_LE    0x04
#endif
#ifndef IPTOS_DSCP_AF11
#define IPTOS_DSCP_AF11  0x28
#define IPTOS_DSCP_AF12  0x30
#define IPTOS_DSCP_AF13  0x38
#define IPTOS_DSCP_AF21  0x48
#define IPTOS_DSCP_AF22  0x50
#define IPTOS_DSCP_AF23  0x58
#define IPTOS_DSCP_AF31  0x68
#define IPTOS_DSCP_AF32  0x70
#define IPTOS_DSCP_AF33  0x78
#define IPTOS_DSCP_AF41  0x88
#define IPTOS_DSCP_AF42  0x90
#define IPTOS_DSCP_AF43  0x98
#define IPTOS_DSCP_CS0   0x00
#define IPTOS_DSCP_CS1   0x20
#define IPTOS_DSCP_CS2   0x40
#define IPTOS_DSCP_CS3   0x60
#define IPTOS_DSCP_CS4   0x80
#define IPTOS_DSCP_CS5   0xa0
#define IPTOS_DSCP_CS6   0xc0
#define IPTOS_DSCP_CS7   0xe0
#define IPTOS_DSCP_EF    0xb8
#define IPTOS_DSCP_VA    0xb0
#endif
#ifndef IPTOS_PREC_NETCONTROL
#define IPTOS_PREC_NETCONTROL      0xe0
#define IPTOS_PREC_INTERNETCONTROL 0xc0
#define IPTOS_PREC_CRITIC_ECP      0xa0
#define IPTOS_PREC_FLASHOVERRIDE   0x80
#define IPTOS_PREC_FLASH           0x60
#define IPTOS_PREC_IMMEDIATE       0x40
#define IPTOS_PREC_PRIORITY        0x20
#define IPTOS_PREC_ROUTINE         0x00
#endif
#ifndef IPVERSION
#define IPVERSION 4
#endif
#ifndef IP_DF
#define IP_DF 0x4000
#endif
#ifndef IP_MF
#define IP_MF 0x2000
#endif
#ifndef IP_OFFMASK
#define IP_OFFMASK 0x1fff
#endif
