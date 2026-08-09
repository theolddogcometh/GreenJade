/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal Ethernet/ARP/UDP/ICMP helpers over L2 (pure C).
 * Dual MIT OR Apache-2.0 freestanding L2/L3 demux for bring-up and the
 * interim kernel path before full userspace netstackd owns RX.
 *
 * Role in the stack:
 *   net_l2_rx -> net_eth_poll batch -> ARP reply / ICMP echo / UDP echo
 *   (port 7) / IPv4 TCP demux -> net_tcp_input; then net_tcp_poll for
 *   multi-seg last-segment rtx + TIME_WAIT soft reap.
 *
 * Product track (lean residual; Soft!=product; G-AC-1):
 *   - T0 product net: virtio-net via net_l2 (QEMU / virt)
 *   - Product NIC (laptop): UDX L2 behind net_l2 (userspace; not this unit)
 *   - freestanding_class=SKIP (no freestanding rtl R-climb product track)
 *   - No dense7 multi-pass thrash; one POLL_MAX batch per poll
 *   - H1 thr-only poll: net_eth_poll on scheduler_run thr stack only
 *     (never timer / APIC / MSI-X / IRQ stack; net_eth_irq=0)
 *   - Stack-safe: RX/TX/VLAN peel workspaces are static BSS
 *   - Soft inventory: soft_log <= one short line; cadence no-op
 *   - Rate-limit who-has/icmp PASS + short TX FAIL (CAP=4; no stamp storms)
 *   - Lifetime soft counters for frames / drops / TCP demux
 *
 * Guest identity (not DHCP client):
 *   QEMU virtio: MAC 52:54:00:12:34:56, IPv4 10.0.2.15
 *   lab residual pin (if L2 backend is rtl): static 10.200.125.50
 *   (identity only; freestanding SKIP; not R0 rabbit hole)
 *
 * Greppable product markers (keep stable; Soft!=product):
 *   net_eth: ARP/UDP/ICMP-echo helpers
 *   net_eth: soft arp|icmp|identity
 *   net_eth: soft arp|icmp ... Soft!=product
 *   net_eth: soft arp reply (once; who-has TX ok → L2/UDX pull)
 *   net_eth: who-has PASS|TX FAIL | net_eth: icmp TX FAIL
 *   net: eth soft inventory
 *   net_eth_poll=run_loop_only | net_eth_irq=0 | owner=scheduler_run
 *   poll_own=thr_stack | freestanding_class=SKIP | product=UDX
 *   lab_ip=10.200.125.50
 *   lean residual (virtio T0 + product UDX L2)
 *   W11 Dual DoD B FUNCTIONAL: wire handoff + :22 stack (tcp22_seen);
 *   thr_only H1 poll; Dual DoD OPEN; stamp-free bar v2026.08.04.75.
 * greppable: wire_handoff+tcp22 | tcp22_seen | stack=eth|tcp|door|:22
 *
 * Soft!=product. G-AC-1 (no in-kernel .ko product AC).
 */
#pragma once

#include <gj/types.h>

/**
 * Reset soft counters. Does not probe virtio-net (caller/probe order).
 * Safe to re-call; logs guest IP + poll_max.
 */
void net_eth_init(void);

/**
 * Pull MAC/IPv4 from net_l2 after NIC probe (virtio T0 or residual L2).
 * net_eth_init runs early (before probe); call this after net_l2_init so
 * ARP/ICMP match the active backend IP (QEMU 10.0.2.15 or lab pin).
 */
void net_eth_apply_l2_identity(void);

/**
 * Poll RX (multi-frame soft batch up to NET_ETH_POLL_MAX):
 *   - demux ethertype / IPv4 proto
 *   - ARP request -> reply (our MAC/IP; short multi-try under L2 busy)
 *   - ICMP echo request -> reply (IHL-aware; short multi-try)
 *   - UDP port 7 -> echo
 *   - TCP -> net_tcp_input (counts tcp_demux on consume)
 *   - single-tag 802.1Q peel -> inner ARP/IPv4 (else vlan_skip)
 *   - then net_tcp_poll (rtx / TIME_WAIT tick)
 * H1 thr-only: scheduler_run thr stack (full kstack) - never IRQ poll.
 * Soft inventory: no multi-line dump; cadence no-op. Soft!=product.
 * No-op soft when L2 not ready (no hard fail).
 * Lean: one POLL_MAX batch; thrash-stripped; freestanding_class=SKIP.
 * greppable: net_eth_poll=run_loop_only | net_eth_irq=0 | owner=scheduler_run
 * greppable: poll_own=thr_stack | freestanding_class=SKIP | product=UDX
 * greppable: lab_ip=10.200.125.50
 */
void net_eth_poll(void);

/**
 * Inject one L2 frame into demux (UDX host thr-poll RX residual).
 * Same demux as net_l2_rx path (ARP/ICMP/UDP7/TCP:22). Soft!=product.
 * Dual DoD B: path=rtl8168_udx product_udx_abi; freestanding_class=SKIP.
 * greppable: net_eth: soft udx inject | path=rtl8168_udx
 * Returns 1 if demux touched, 0 if drop/short.
 */
int net_eth_input_frame(const void *pFrame, u32 cb);

/** Lifetime who-has ARP replies TX ok (soft; excludes gratuitous announce). */
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
