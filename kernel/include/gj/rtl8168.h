/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Realtek RTL8111/8168 Gigabit Ethernet (PCI 10ec:8168).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no r8169 paste.
 *
 * Public PCI IDs + MMIO register map only (Realtek / OSDev-class docs).
 * Poll-mode TX/RX for freestanding sshd on G752VT-class laptops.
 *
 * Soft ≠ product close for every rev; soft SKIP if no matching PCI.
 * greppable: rtl8168: probe PASS|SKIP|FAIL | rtl8168: ready PASS
 */
#pragma once

#include <gj/types.h>

/**
 * Scan PCI for first 10ec:8168, map BAR, soft-reset, program desc rings,
 * enable TX/RX. Soft SKIP (return 0) when absent (QEMU virtio-only).
 * Returns 0 always for bring-up (never hard-gates boot).
 */
int rtl8168_probe(void);

/** Non-zero if link path is programmed and rings are live. */
int rtl8168_ready(void);

/** Soft: non-zero if PHY reports link (best-effort; 0 if unknown). */
int rtl8168_link_up(void);

/**
 * Transmit one Ethernet frame (dst..payload, no FCS). Returns 0 on queue,
 * -1 if not ready / ring busy (OWN) / bad args. Ring-full → tx_busy;
 * bad args / not ready → tx_fail (busy ≠ fail).
 */
int rtl8168_tx(const void *pFrame, u32 cbLen);

/**
 * Poll one completed RX frame into pOut (up to cbMax). Returns byte length
 * or -1 if empty / not ready.
 */
i32 rtl8168_rx(void *pOut, u32 cbMax);

/** Copy station MAC (6 bytes) into pMac when ready; else zeros. */
void rtl8168_mac(u8 *pMac);

/**
 * Keep CHIPCMD TE|RE asserted and clear IntrStatus. Call from net_eth_poll
 * so RX does not stall after long idle / bus power quirks (G752).
 */
void rtl8168_poll_hw(void);

/** Soft counters for panel / kprintf. */
u32 rtl8168_tx_count(void);
u32 rtl8168_rx_count(void);
/**
 * Real TX errors only (bad args / not ready). Ring-full is tx_busy, not fail
 * (OpenBSD/if_re style: busy ≠ fail).
 */
u32 rtl8168_tx_fail(void);
/** TX ring full / OWN still set after brief wait (would block). */
u32 rtl8168_tx_busy(void);
/** Frames completed by NIC but dropped (RES/ROR/bad len). */
u32 rtl8168_rx_drop(void);
