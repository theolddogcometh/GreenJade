/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * L2 backend mux for freestanding Ethernet:
 *   prefer virtio-net (QEMU T0 product), else rtl8168 (G752 Realtek).
 *
 * Pure C11 freestanding. Dual MIT OR Apache-2.0.
 * greppable: net_l2: backend=virtio|rtl8168|none
 * greppable: net_l2: soft linux netdev note n=N (Soft≠product)
 *
 * Soft Linux coexistence: net_l2_soft_linux_note() lamps when a soft
 * netdev is registered; freestanding TX/RX is unchanged (Soft≠product).
 */
#pragma once

#include <gj/types.h>

/** Backend id for STATUS / logs. */
#define GJ_NET_L2_NONE    0u
#define GJ_NET_L2_VIRTIO  1u
#define GJ_NET_L2_RTL8168 2u

/**
 * Probe order: virtio_net_probe (caller) then rtl8168_probe.
 * Picks first ready backend; sets guest IP/MAC policy for that backend.
 * Call after virtio_net_probe from kmain.
 */
void net_l2_init(void);

/**
 * Refresh MAC from freestanding rtl8168 after reclaim_wire (hybrid 4a).
 * Soft≠product. No-op if backend is not rtl8168.
 */
void net_l2_refresh_mac(void);

/** Active backend (GJ_NET_L2_*). */
u32 net_l2_backend(void);

/** Non-zero if TX/RX path is usable. */
int net_l2_ready(void);

/** TX one Ethernet frame. 0 ok, -1 fail. */
int net_l2_tx(const void *pFrame, u32 cbLen);

/** RX one frame; length or -1. */
i32 net_l2_rx(void *pOut, u32 cbMax);

/** Current station MAC (6). */
void net_l2_mac(u8 *pMac);

/** Current IPv4 (4). QEMU virtio: 10.0.2.15; Realtek lab: 10.0.0.50. */
void net_l2_ip(u8 *pIp);

/** Soft name for STATUS ("virtio" / "rtl8168" / "none"). */
const char *net_l2_name(void);

/**
 * Soft Linux netdev coexistence lamp (once-ish).
 * When linux_netdev_soft_count()≥1 and primary exists, emit:
 *   net_l2: soft linux netdev note n=N (Soft≠product)
 * Does not switch TX/RX — freestanding backend remains product datapath
 * until a real soft probe/datapath is wired. Safe no-op if soft absent.
 * Call after soft r8169 / register_netdev path from kmain.
 */
void net_l2_soft_linux_note(void);

/**
 * Soft MMIO handoff state (phase 1+; Soft≠product).
 * Grep: net_l2: soft mmio handoff
 */
void net_l2_soft_handoff_mark_pending(void);
/** Non-zero while freestanding quiesced and soft open not complete. */
int  net_l2_soft_handoff_pending(void);
/** Non-zero after dual-drive FAULT (fail closed). */
int  net_l2_soft_handoff_fault(void);
/** Record dual-drive FAULT (idempotent log). */
void net_l2_soft_handoff_set_fault(const char *szWhy);
