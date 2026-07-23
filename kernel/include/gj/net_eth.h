/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal Ethernet/ARP/UDP helpers over virtio-net (clean-room pure C).
 * Poll also demuxes IPv4 TCP → net_tcp_input and ticks multi-seg rtx.
 */
#pragma once

#include <gj/types.h>

void net_eth_init(void);

/** Poll RX; ARP/ICMP/UDP echo; TCP demux + net_tcp_poll rtx tick. */
void net_eth_poll(void);

u32  net_eth_arp_replies(void);
u32  net_eth_udp_echoes(void);
u32  net_eth_icmp_echoes(void);
