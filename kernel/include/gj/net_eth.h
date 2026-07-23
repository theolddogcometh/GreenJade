/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal Ethernet/ARP/UDP helpers over virtio-net (clean-room pure C).
 * Poll drains up to N frames; demuxes IPv4 TCP → net_tcp_input and ticks
 * multi-seg rtx + TIME_WAIT soft reap. Soft counters for frames/drops/VLAN.
 */
#pragma once

#include <gj/types.h>

void net_eth_init(void);

/**
 * Poll RX (multi-frame soft batch); ARP/ICMP/UDP echo; TCP demux +
 * net_tcp_poll rtx/TIME_WAIT tick.
 */
void net_eth_poll(void);

u32  net_eth_arp_replies(void);
u32  net_eth_udp_echoes(void);
u32  net_eth_icmp_echoes(void);
/** Soft: frames accepted into demux path. */
u32  net_eth_frames_rx(void);
/** Soft: short/unknown ethertype / non-IPv4 drops. */
u32  net_eth_frames_drop(void);
/** Soft: frames handed to net_tcp_input. */
u32  net_eth_tcp_demux(void);
/** Soft: 802.1Q frames counted and skipped. */
u32  net_eth_vlan_skip(void);
