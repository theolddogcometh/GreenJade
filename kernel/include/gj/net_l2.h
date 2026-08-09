/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * L2 backend mux for freestanding Ethernet:
 *   prefer virtio-net (QEMU T0 product) when present;
 *   freestanding rtl8168 only when GJ_RTL8168_PROBE=1 (default 0 SKIP).
 *
 * Pure C11 freestanding. Dual MIT OR Apache-2.0.
 * greppable: net_l2: backend=virtio|rtl8168|none
 * greppable: net_l2: freestanding rtl SKIP GJ_RTL8168_PROBE=0
 * greppable: net_l2: soft residual freestanding rtl SKIP
 * greppable: net_l2: soft residual product=UDX+ABI
 * greppable: net_l2: soft residual lean
 * greppable: net_l2: soft residual product_udx_abi honesty
 * greppable: path=userspace_udx_hot_cold_abi
 * greppable: role=skip_rtl_product_udx_abi
 * greppable: owner=product_udx_abi
 * greppable: need=UDX_OPEN | freestanding_rtl=SKIP | dual_dod_b=OPEN
 * greppable: net_l2: lab wire honesty ... (Soft!=product; G-AC-1)
 * greppable: net_l2: soft linux netdev note n=N (Soft!=product)
 * greppable: net_l2: soft ready handoff listen ... (feeds net_tcp :22)
 * greppable: net_l2: soft gate0 honesty ... (hybrid SOFT; bridge!=MMIO handoff)
 * greppable: net_l2: soft tx residual freestanding ... (Soft!=product)
 * greppable: net_l2: soft tx busy residual ... (Soft!=product)
 * greppable: net_l2: soft tx busy retry ok ... (Soft!=product)
 * greppable: net_l2: soft tx fail residual ... (Soft!=product)
 * greppable: net_l2: soft tx handoff residual ... (Soft!=product)
 * greppable: net_l2: soft bridge hold14 freestanding wire ... (Soft!=product)
 * greppable: net_l2: soft rx residual freestanding ... (Soft!=product; dual DoD B)
 * greppable: net_l2: soft rx demux eth/tcp ... (Soft!=product; dual DoD B)
 * greppable: net_l2: soft bridge rx tally lean ... (Soft!=product; hold14; no flood)
 * greppable: net_l2: soft mac identity honesty ... (Soft!=product; dual DoD B)
 * greppable: net_l2: soft lab ip identity ... (10.200.125.50; Soft!=product)
 * greppable: net_l2: soft udx ready ... (ETH_UDX_READY identity pin)
 *
 * Freestanding rtl SKIP residual (GJ_RTL8168_PROBE=0 default; Soft!=product):
 *   Same class as freestanding USB MSC SKIP. Product laptop NIC =
 *   Linux-shaped userspace UDX over hot+cold ABI
 *   (path=userspace_udx_hot_cold_abi; G-AC-1; not in-kernel .ko wire;
 *   not freestanding class thrash). Virtio T0 still selected when present
 *   (QEMU product net). Do NOT re-enable freestanding rtl by default -
 *   opt-in residual only (-DGJ_RTL8168_PROBE=1). SKIP lamps denser
 *   product=UDX+ABI; soft residual lean only (no dual DoD B freestanding
 *   rtl R-climb claim under SKIP; no freestanding RX/TX thrash). Under
 *   SKIP + backend=none: wire/owner=product_udx_abi lean honesty (not a
 *   freestanding wire claim). Dual DoD B OPEN need=UDX_OPEN (UDX owns
 *   product NIC; freestanding residual never closes DoD B). Soft!=product
 *   * G-AC-1 * dual MIT OR Apache-2.0 * once/hard-cap.
 *   Grep: net_l2: freestanding rtl SKIP GJ_RTL8168_PROBE=0
 *   Grep: net_l2: soft residual freestanding rtl SKIP
 *   Grep: net_l2: soft residual product=UDX+ABI
 *   Grep: net_l2: soft residual lean
 *   Grep: net_l2: soft residual product_udx_abi honesty
 *   Grep: path=userspace_udx_hot_cold_abi
 *   Grep: role=skip_rtl_product_udx_abi
 *   Grep: owner=product_udx_abi
 *   Grep: need=UDX_OPEN | freestanding_rtl=SKIP
 *
 * Soft Linux coexistence: net_l2_soft_linux_note() lamps when a soft
 * netdev is registered; freestanding TX/RX is unchanged when selected
 * (Soft!=product). Soft r8169 is never product TX/RX owner here (G-AC-1;
 * Soft!=product). Under freestanding rtl SKIP: soft note denser
 * product=UDX+ABI lean residual only (no freestanding rtl L2 bridge dual
 * DoD B residual; no freestanding RX/TX thrash). Under SKIP + backend=none
 * soft note claims tx=product_udx_abi (not tx=freestanding).
 *
 * Soft ready handoff -> net_tcp listen (Dual DoD B residual):
 *   net_l2_ready()!=0 is freestanding usable-wire. net_tcp soft listen :22
 *   mints/holds only after ready. Handoff pending/FAULT -> ready=0
 *   (fail-closed). Soft L2 bridge ON is copy-only != MMIO handoff and is
 *   not a ready substitute. Grep: net_l2: soft ready handoff listen
 *
 * Hybrid SOFT gate0 honesty (default lab): freestanding owns BAR/wire
 * when a freestanding backend is selected; Soft L2 bridge may ON as R/T
 * mirror. Under SKIP (backend=none): wire=product_udx_abi lean residual
 * (product=UDX+ABI path=userspace_udx_hot_cold_abi). Soft!=product * G-AC-1.
 * Grep: net_l2: soft gate0 honesty
 *
 * Lab wire honesty (mux residual): freestanding backend owns the wire
 * when selected (virtio T0 product, or rtl8168 lab dual DoD B opt-in).
 * Under freestanding rtl SKIP + no virtio: owner=product_udx_abi
 * role=skip_rtl_product_udx_abi. Soft L2 bridge is copy-only; hold14 R/T
 * mirror tracks freestanding when present (bridge != product wire).
 * Soft!=product * G-AC-1.
 *
 * Dual DoD B residual (host arping+ping / sshd eth; opt-in rtl only):
 *   net_l2_tx busy != fail - pre-TX settle + sliced reclaim + ring OWN
 *   re-try so ARP/ICMP/TCP replies leave; busy exhaust is not sticky;
 *   handoff FAULT fail-closed (handoff != busy). SoftTxBusy / SoftTxFail
 *   counters stay distinct. Inactive under GJ_RTL8168_PROBE=0 SKIP.
 *
 * L2 bridge RX residual (Dual DoD B * freestanding rtl -> eth/tcp demux):
 *   net_l2_rx multi-try poll_hw reclaim so freestanding frames leave the
 *   ring and demux in net_eth (ARP/ICMP) + net_tcp (:22). Strengthened
 *   under DUT L2 BR RX=0 TX>0 freestanding UP / host arping 0 replies:
 *   pre-RX settle+poll, denser empty re-try (R0/R1 lag), denser settle
 *   slice, final poll after empty so next eth drain can hit. Lab IP
 *   identity 10.200.125.50; MAC identity honesty from rtl station. Soft
 *   bridge tallies lean (SoftRxOk + hold14 refresh; no stamp flood / no
 *   feed double-count - eth still owns feed_rx copy). Soft!=product * G-AC-1.
 *   Inactive under freestanding rtl SKIP default.
 * Soft!=product * G-AC-1 * no stamp storms * no inventory flood
 * (once / hard-cap lamps only; NET_L2_SOFT_*_LAMP_CAP).
 */
#pragma once

#include <gj/types.h>

/** Backend id for STATUS / logs. */
#define GJ_NET_L2_NONE    0u
#define GJ_NET_L2_VIRTIO  1u
#define GJ_NET_L2_RTL8168 2u

/**
 * Probe order: virtio_net (caller) preferred T0 product when present;
 * freestanding rtl8168 only when GJ_RTL8168_PROBE=1 (default 0 SKIP).
 * Picks first ready backend; sets guest IP/MAC policy for that backend.
 * Under SKIP: denser product=UDX+ABI soft residual lean lamps
 * (path=userspace_udx_hot_cold_abi; role=skip_rtl_product_udx_abi;
 * owner=product_udx_abi; need=UDX_OPEN; freestanding_rtl=SKIP);
 * no freestanding rtl class thrash; no freestanding RX/TX thrash.
 * Soft!=product * G-AC-1 * once/hard-cap.
 * Grep: net_l2: freestanding rtl SKIP GJ_RTL8168_PROBE=0
 * Grep: net_l2: soft residual freestanding rtl SKIP
 * Grep: net_l2: soft residual product=UDX+ABI
 * Grep: net_l2: soft residual lean
 * Grep: net_l2: soft residual product_udx_abi honesty
 * Grep: path=userspace_udx_hot_cold_abi
 * Call after virtio_net_probe from kmain.
 */
void net_l2_init(void);

/**
 * Refresh MAC from freestanding rtl8168 after reclaim_wire (hybrid 4a).
 * Soft!=product. No-op if backend is not rtl8168.
 */
void net_l2_refresh_mac(void);

/** Active backend (GJ_NET_L2_*). */
u32 net_l2_backend(void);

/**
 * Non-zero if freestanding TX/RX path is usable.
 * Soft ready handoff residual (Dual DoD B): when non-zero, net_tcp may
 * mint/hold soft listen :22 (tcp_soft_ensure_listen22). Handoff pending
 * or FAULT -> 0 (fail-closed; no dead :22 mint). Soft L2 bridge ON is
 * copy-only and is not a substitute for this ready bit.
 * Grep: net_l2: soft ready handoff listen (once / hard-cap).
 */
int net_l2_ready(void);

/**
 * TX one Ethernet frame. 0 ok, -1 did not leave.
 * Freestanding mux residual (busy != fail; Soft!=product * lab dual DoD B):
 *   - rtl8168: pre-TX settle when ring OWN; sliced poll_hw; L2 busy re-try
 *     so single-try callers still deliver ARP/ICMP/TCP (soft l2_xmit / SYN-ACK)
 *   - virtio: freestanding T0 product path; soft counters only (no OWN settle)
 *   - busy exhaust -> SoftTxBusy (not SoftTxFail); -1 is not sticky
 *   - hard backend fail -> SoftTxFail immediately
 *   - handoff pending/FAULT -> SoftTxHandoffClosed fail-closed (!= busy)
 * Soft r8169 is never selected as TX owner (G-AC-1).
 */
int net_l2_tx(const void *pFrame, u32 cbLen);

/**
 * RX one frame; length or -1.
 * Freestanding mux only (virtio|rtl8168). Soft r8169 never owns RX.
 * Dual DoD B residual (Soft!=product * lab freestanding rtl -> eth/tcp):
 *   - rtl: multi-try poll_hw reclaim so frames leave ring under R0/R1 lag
 *     (pre-RX settle+poll * denser empty re-try * denser settle slice *
 *     final poll after empty - DUT L2 BR RX=0 class; frames -> net_eth)
 *   - SoftRxOk / SoftRxEmpty / SoftRxRetryOk lean tallies (hard-cap lamps)
 *   - MAC identity honesty (station MAC from rtl; demux ARP/ICMP match)
 *   - lab IP identity 10.200.125.50 (never QEMU 10.0.2.15 on rtl backend)
 *   - soft bridge lean: hold14 refresh on ok (eth still owns feed_rx copy;
 *     no double-count); Soft L2 bridge ON != MMIO handoff / != ready
 * Grep: net_l2: soft rx residual freestanding
 * Grep: net_l2: soft rx demux eth/tcp
 * Grep: net_l2: soft bridge rx tally lean
 * Grep: net_l2: soft mac identity honesty
 * Grep: net_l2: soft lab ip identity
 */
i32 net_l2_rx(void *pOut, u32 cbMax);

/** Current station MAC (6). */
void net_l2_mac(u8 *pMac);

/** Current IPv4 (4). QEMU virtio: 10.0.2.15; Realtek lab: 10.200.125.50. */
void net_l2_ip(u8 *pIp);

/**
 * Soft UDX ready identity residual (backend=none + freestanding rtl SKIP):
 * pin lab IPv4 10.200.125.50 and soft demux lab MAC
 * LAB_MAC_UDX=02:00:00:47:4a:50 (same as rtl8168_udx product IDR
 * lab_fallback; not QEMU g_aVirtMac) so ARP/ICMP match / replies leave
 * into ETH_TX_PULL. Soft demux MAC aligns with product idr lab_fallback
 * Soft!=product. OPEN: idr keep=1 EEPROM station not yet published here.
 * No-op when a freestanding backend owns identity. Idempotent.
 * greppable: net_l2: soft udx ready
 * greppable: LAB_MAC_UDX=02:00:00:47:4a:50
 */
void net_l2_udx_ready_identity(void);

/** Soft name for STATUS ("virtio" / "rtl8168" / "none"). */
const char *net_l2_name(void);

/**
 * Soft Linux netdev coexistence lamp (once-ish; hard-capped residual lines).
 * When linux_netdev_soft_count()≥1 and primary exists, emit:
 *   net_l2: soft linux netdev note n=N (Soft!=product)
 *   net_l2: soft linux note no dual-drive ...
 *   net_l2: lab wire honesty soft_present=... owner=freestanding|product_udx_abi
 *   net_l2: soft residual freestanding rtl SKIP ... (when GJ_RTL8168_PROBE=0)
 *   net_l2: soft residual product=UDX+ABI path=userspace_udx_hot_cold_abi ...
 *   net_l2: soft residual lean ... owner=product_udx_abi need=UDX_OPEN
 *   net_l2: soft residual product_udx_abi honesty ... wire=product_udx_abi
 *   net_l2: soft gate0 honesty [denser] ... (hybrid SOFT; bridge!=MMIO handoff)
 *   net_l2: soft ready handoff listen ... (if freestanding already ready -> :22)
 * May enable Soft L2 bridge (copy-only) when freestanding rtl owns wire
 * (opt-in GJ_RTL8168_PROBE=1 only). Under freestanding rtl SKIP: soft
 * residual lean only - denser product=UDX+ABI lamps
 * (path=userspace_udx_hot_cold_abi; role=skip_rtl_product_udx_abi;
 * owner=product_udx_abi when backend=none; dual_dod_b=OPEN need=UDX_OPEN);
 * no freestanding rtl L2 bridge dual DoD B residual; no freestanding RX/TX
 * thrash. Under SKIP + backend=none soft note uses tx=product_udx_abi (not
 * tx=freestanding). Soft L2 bridge ON != freestanding MMIO handoff and is
 * not ready substitute. Does not switch TX/RX. Soft r8169 is
 * inventory/control only (Soft!=product * G-AC-1). Virtio T0 still selected
 * when present. No inventory flood: once + NET_L2_SOFT_*_LAMP_CAP hard-cap.
 * Safe no-op if soft absent.
 * Call after soft r8169 / register_netdev from kmain.
 */
void net_l2_soft_linux_note(void);

/**
 * Soft MMIO handoff state (phase 1+; Soft!=product).
 * Grep: net_l2: soft mmio handoff
 */
void net_l2_soft_handoff_mark_pending(void);
/** Non-zero while freestanding quiesced and soft open not complete. */
int  net_l2_soft_handoff_pending(void);
/** Non-zero after dual-drive FAULT (fail closed). */
int  net_l2_soft_handoff_fault(void);
/** Record dual-drive FAULT (idempotent log). */
void net_l2_soft_handoff_set_fault(const char *szWhy);
