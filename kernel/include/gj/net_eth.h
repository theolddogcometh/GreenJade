/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal Ethernet/ARP/UDP/ICMP helpers over virtio-net (clean-room pure C).
 * Dual MIT OR Apache-2.0 freestanding L2/L3 demux for bring-up and the
 * interim kernel path before full userspace netstackd owns RX.
 *
 * Role in the stack:
 *   virtio_net_rx → net_eth_poll batch → ARP reply / ICMP echo / UDP echo
 *   (port 7) / IPv4 TCP demux → net_tcp_input; then net_tcp_poll for
 *   multi-seg last-segment rtx + TIME_WAIT soft reap.
 *
 * Soft deepen (v1, no redesign):
 *   - Multi-frame RX drain per poll (NET_ETH_POLL_MAX = 8 in net_eth.c)
 *   - 802.1Q frames counted and skipped (vlan_skip)
 *   - IHL-aware ICMP echo reply path
 *   - Lifetime soft counters for frames / drops / TCP demux
 *
 * Guest identity (QEMU user-net shaped; not DHCP client):
 *   MAC 52:54:00:12:34:56, IPv4 10.0.2.15 — shared with net_tcp TX.
 *
 * Greppable product markers (keep stable):
 *   net_eth: ARP/UDP/ICMP-echo helpers
 *   (smoke paths exercise arp/udp/icmp counters via net_door STATS/POLL)
 */
#pragma once

#include <gj/types.h>

/**
 * Reset soft counters. Does not probe virtio-net (caller/probe order).
 * Safe to re-call; logs guest IP + poll_max.
 */
void net_eth_init(void);

/**
 * Poll RX (multi-frame soft batch up to NET_ETH_POLL_MAX):
 *   - demux ethertype / IPv4 proto
 *   - ARP request → reply (our MAC/IP)
 *   - ICMP echo request → reply (IHL-aware)
 *   - UDP port 7 → echo
 *   - TCP → net_tcp_input (counts tcp_demux on consume)
 *   - then net_tcp_poll (rtx / TIME_WAIT tick)
 * No-op soft when virtio-net not ready (no hard fail).
 */
void net_eth_poll(void);

/** Lifetime ARP replies transmitted (soft stats). */
u32  net_eth_arp_replies(void);
/** Lifetime UDP echo replies (port 7 soft). */
u32  net_eth_udp_echoes(void);
/** Lifetime ICMP echo replies. */
u32  net_eth_icmp_echoes(void);
/** Soft: frames accepted into demux path (post-RX, pre-drop). */
u32  net_eth_frames_rx(void);
/** Soft: short / unknown ethertype / non-IPv4 drops. */
u32  net_eth_frames_drop(void);
/** Soft: frames handed to net_tcp_input (TCP demux hits). */
u32  net_eth_tcp_demux(void);
/** Soft: 802.1Q frames counted and skipped (not stripped). */
u32  net_eth_vlan_skip(void);
