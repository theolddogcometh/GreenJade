/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Realtek RTL8111/8168 (PCI 10ec:8168) freestanding residual.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no r8169 paste.
 *
 * *** FREESTANDING SKIP DEFAULT · NOT PRODUCT (ABI-first pivot) ***
 * GJ_RTL8168_PROBE stays 0: freestanding class SKIP (same policy as USB MSC).
 * Product laptop NIC = userspace rtl8168_udx + hot+cold ABI + DDI/UDX caps.
 * T0 product net = virtio-net until UDX owns wire. Soft!=product · G-AC-1.
 *
 * STOP freestanding rtl rabbit hole. NEVER re-enable freestanding NIC as
 * default. NEVER thrash R-climb as product engineering. Residual thrash
 * stripped: this unit is lean SKIP honesty stubs only (API for linkers).
 * Opt-in -DGJ_RTL8168_PROBE=1 does not restore BAR/rings/R-climb.
 *
 * greppable: rtl8168: probe SKIP | GJ_RTL8168_PROBE=0
 * greppable: rtl8168: soft residual product=UDX+ABI
 * greppable: rtl8168: soft residual lean
 * greppable: Soft!=product
 */
#pragma once

#include <gj/types.h>

/**
 * Freestanding residual probe — always SKIP honesty under default gate.
 * GJ_RTL8168_PROBE stays 0: no BAR claim, no rings, no R-climb.
 * Returns 0 always (never hard-gates boot). Soft!=product.
 * Product NIC = rtl8168_udx + ABI. Grep: rtl8168: probe SKIP
 */
int rtl8168_probe(void);

/** Always 0 — freestanding residual never owns wire. Soft!=product. */
int rtl8168_ready(void);

/** Always 0 — no freestanding PHY path. Soft!=product. */
int rtl8168_link_up(void);

/**
 * Always fail (-1). Product TX = rtl8168_udx + ABI. Soft!=product.
 */
int rtl8168_tx(const void *pFrame, u32 cbLen);

/**
 * Always empty (-1). Product RX = rtl8168_udx + ABI. Soft!=product.
 */
i32 rtl8168_rx(void *pOut, u32 cbMax);

/** Zeros pMac — freestanding has no station. Soft!=product. */
void rtl8168_mac(u8 *pMac);

/**
 * No-op keep-alive. Freestanding SKIP / no R-climb thrash. Soft!=product.
 */
void rtl8168_poll_hw(void);

/** Soft counters always 0 (honesty; Soft!=product). */
u32 rtl8168_tx_count(void);
u32 rtl8168_rx_count(void);
u32 rtl8168_tx_fail(void);
u32 rtl8168_tx_busy(void);
u32 rtl8168_rx_drop(void);

/**
 * Soft MMIO handoff prepare — residual SKIP honesty only.
 * Soft!=product · G-AC-1. Product NIC = rtl8168_udx + ABI.
 * Grep: rtl8168: soft mmio handoff SKIP
 */
void rtl8168_soft_handoff_prepare(void);

/** Always 0 — freestanding never prepares handoff. Soft!=product. */
int  rtl8168_soft_handoff_prepared(void);

/**
 * Soft kick wire — residual SKIP (no freestanding thrash). Soft!=product.
 * Grep: rtl8168: soft kick wire SKIP
 */
void rtl8168_kick_wire(void);

/**
 * Post-TE rearm — residual SKIP. Soft!=product.
 * Grep: rtl8168: soft post-te rearm SKIP
 */
int rtl8168_post_te_rearm(void);

/**
 * Reclaim wire — residual SKIP. Soft!=product · G-AC-1.
 * Grep: rtl8168: soft reclaim wire SKIP
 */
int  rtl8168_reclaim_wire(void);
