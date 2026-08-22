/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Net door - netstackd / sshd / UDX hand-off surface (A1 / eng residual).
 * Pure C11 freestanding, dual MIT OR Apache-2.0. Dispatched via GJ_SYS_NET
 * (arg0 = opcode). Ownership grants exclusive virtio TX/RX queue policy
 * to userspace netstackd while the kernel still owns the interim socket
 * and eth demux path until full userspace stack migration.
 *
 * Dispatch contract:
 *   net_door_call(op, arg1, arg2, arg3) -> i64
 *   Success: 0, positive byte/fd counts, or 1 for boolean-ish ops.
 *   Errors:  negative Linux-style errno or GJ_ERR_* (FAULT/INVAL/BUSY/NODEV).
 *   User pointers: copy_{to,from}_user when the range is in the user VA
 *   window; early kernel smokes may pass HHDM/static buffers.
 *
 * Ownership (token policy shared with store/session/vfs doors):
 *   token 0  = kernel interim owns policy
 *   non-zero = netstackd claimed; CLAIM is idempotent for the same 32-bit
 *              token (reclaim soft); a different token returns BUSY
 *   RELEASE when free is soft 0; when owned, token must match
 *
 * Lean soft residual - Dual DoD B UDX handoff eng (Soft!=product; dual):
 *   Product DoD B = UDX not freestanding rtl (G-AC-1; no .ko product AC).
 *   Eng path: CLAIM -> ring MAP/DMA/DESC/USER_AVAIL (netstackd/UDX host
 *   stack) and SOCKET->BIND(:22)->LISTEN->ACCEPT(+POLL yield) for sshd-gj
 *   interim. GJ_NET_OP_POLL advances freestanding eth via net_eth_poll
 *   (thr/door stack only - never IRQ/timer; H1). Soft always 0; never NODEV.
 *   ACCEPT empty soft EAGAIN (-11). dual_dod_b=OPEN_UDX product_sshd_tcp22=OPEN
 *   until host interactive SSH login. Banner != login. Residual: init + first-call once
 *   only (no per-POLL/STATS/RING dumps). Soft!=product dual license.
 *   UDX/host residual deepen (Soft!=product): per-op ring-family tallies +
 *   sparse ring MAP notes (map_va, map_which, map_ok, map_reclaim, map_nodev,
 *   map_fault, map_inval, dma_va, dma_ok, dma_nodev, export_ok, kick_ok,
 *   host_owned_ring, desc|avail|used attach) on residual lamps only -
 *   never per MAP/RING call. H1: door thr only (POLL not IRQ/timer).
 *   greppable: net_door: soft residual
 *   greppable: net_door: soft residual lean
 *   greppable: net_door: soft residual lean PASS
 *   greppable: net_door: ring map notes
 *   greppable: product_dod_b=UDX | not_freestanding_rtl | dual_dod_b=OPEN_UDX
 *   greppable: poll->net_eth_poll | net_eth_poll=run_loop_or_door | irq=0
 *   greppable: handoff=netstackd|sshd|UDX
 *   greppable: udx_host_stack=claim|map_ring|map_dma|desc|user_avail|bounce
 *   greppable: door_thr_only=1 | H1
 *   W11 Dual DoD B FUNCTIONAL residual (Soft!=product; stamp-free bar
 *   v2026.08.04.75; never invent .76): wire handoff + :22 stack for product
 *   sshd. POLL H1 door thr -> eth demux -> soft :22 BIND/LISTEN/ACCEPT/
 *   SOCK_POLL. greppable: wire_handoff+tcp22 | stack=eth|tcp|door|:22
 *   greppable: net_door: soft residual wire22 | sock_poll22
 *
 * Bounce / sizing (aligned with net_tcp multi-seg bulk):
 *   SEND/RECV bounce is NET_XFER_MAX (4096) in net_door.c - large enough
 *   for multi-seg TCP bulk (e.g. 3000 B > MSS) in one door call.
 *
 * Opcode matrix vs net_tcp_* (product interim / Soft!=product):
 *   GJ_NET_OP_SOCKET    -> net_tcp_socket      (SOCK_STREAM; DGRAM->net_lo)
 *   GJ_NET_OP_BIND      -> net_tcp_bind        (port host-order u16)
 *   GJ_NET_OP_LISTEN    -> net_tcp_listen      (soft backlog clamp)
 *   GJ_NET_OP_ACCEPT    -> net_tcp_accept      (-EAGAIN if empty)
 *   GJ_NET_OP_CONNECT   -> net_tcp_connect     (local listener SYN soft)
 *   GJ_NET_OP_SEND      -> net_tcp_send        (multi-seg MSS inside)
 *   GJ_NET_OP_RECV      -> net_tcp_recv        (short OK; 0=EOF soft)
 *   GJ_NET_OP_CLOSE     -> net_tcp_close       (soft FIN / free slot)
 *   GJ_NET_OP_TCP_STATS -> accepts/segs|rtx/rx_b/tx_b getters
 *   GJ_NET_OP_SOCK_POLL -> net_tcp_poll_mask   (POLLIN/OUT/ERR/HUP)
 *   GJ_NET_OP_POLL      -> net_eth_poll        (freestanding eth advance; not IRQ)
 *   CLAIM/RELEASE + ring MAP/DMA/DESC/USER_AVAIL -> netstackd/UDX handoff
 *
 * Soft gaps (Soft!=product; not thrashing product ABI):
 *   shutdown / sockopt / getsockname live on net_tcp_* (Linux ABI); not door opcodes.
 *   rtl8168 freestanding - out of door scope; NOT product DoD B (do not thrash)
 *
 * Greppable product markers (main / netstackd; keep ABI stable):
 *   net_door: PASS / ownership PASS / socket path PASS /
 *   virtio queue PASS / ring map PASS / avail push PASS / user ring PASS
 *   net_door claim soft
 */
#pragma once

#include <gj/types.h>

/* ---- door opcodes (GJ_SYS_NET arg0) ---------------------------------- */

/**
 * POLL: freestanding eth advance via net_eth_poll (L2 RX demux + net_tcp_poll
 * rtx/TW). Door thr stack only - never IRQ/timer (H1). sshd yield (POLL+ACCEPT)
 * interim eng. Soft!=product dual license: always returns 0 - never NODEV when
 * L2 absent (eth soft-skip still ticks TCP). Product DoD B = UDX not freestanding
 * rtl. Greppable residual: poll->net_eth_poll | net_eth_poll=run_loop_or_door |
 * irq=0.
 */
#define GJ_NET_OP_POLL   1u
/**
 * STATS: arg1=user u32[4] {arp,udp,icmp,door_calls}. Soft-friendly zeros.
 * Soft residual is init + first-call once (no per-STATS dump). Soft!=product.
 */
#define GJ_NET_OP_STATS  2u
/**
 * socket: arg1=domain arg2=type arg3=proto -> fd (net_lo / net_tcp).
 * Dual DoD B residual soft enter for sshd-gj (Soft!=product; product=UDX).
 */
#define GJ_NET_OP_SOCKET 3u
/**
 * bind: arg1=fd arg2=port (host order u16 in low bits).
 * Soft residual: port 22 marks interim :22 (Soft!=product; dual_dod_b=OPEN_UDX).
 */
#define GJ_NET_OP_BIND   4u
/**
 * send: arg1=fd arg2=user buf arg3=len -> bytes (clamped to 4 KiB bounce;
 * STREAM multi-seg chunking is internal to net_tcp_send).
 */
#define GJ_NET_OP_SEND   5u
/** recv: arg1=fd arg2=user buf arg3=len -> bytes (<=4 KiB; short OK; 0=EOF soft) */
#define GJ_NET_OP_RECV   6u
/** connect: arg1=fd arg2=port - loopback pair or SYN to local listener */
#define GJ_NET_OP_CONNECT 7u
/** close: arg1=fd - soft FIN on TCP ESTABLISHED/CLOSE_WAIT */
#define GJ_NET_OP_CLOSE  8u
/**
 * listen: arg1=fd arg2=backlog - store/clamp soft backlog.
 * Soft residual: after BIND :22, listen22 lamp (Soft!=product).
 */
#define GJ_NET_OP_LISTEN 9u
/**
 * claim: arg1=non-zero 32-bit token - netstackd ownership (UDX handoff eng).
 * Soft reclaim: same token re-CLAIM is idempotent. Different token -> BUSY.
 */
#define GJ_NET_OP_CLAIM  10u
/** release: arg1=token (must match when owned; soft 0 when free) */
#define GJ_NET_OP_RELEASE 11u
/**
 * Virtio queue (owned path preferred; allowed without claim for bring-up):
 * VIRTIO_TX: arg1=user eth frame, arg2=len -> 0 or -errno
 * VIRTIO_RX: arg1=user buf, arg2=max -> nbytes (0 empty, -errno fail)
 * QUEUE_INFO: arg1=user u32[5] {tx,rx,ready,owned,vq_calls}
 */
#define GJ_NET_OP_VIRTIO_TX  12u
#define GJ_NET_OP_VIRTIO_RX  13u
#define GJ_NET_OP_QUEUE_INFO 14u
/**
 * EXPORT_RING: arg1=which(0=rx,1=tx) arg2=user ptr to gj_virtq_export
 * Soft: NODEV when virtio-net is absent. Does not map pages.
 */
#define GJ_NET_OP_EXPORT_RING 15u
/**
 * MAP_RING: arg1=which (0=rx,1=tx) arg2=user VA base (page-aligned)
 *   arg3=optional user ptr to gj_virtq_export (desc|avail|used offsets).
 * Soft: NODEV without device; INVAL on zero/unaligned VA; FAULT on map fail.
 * Re-MAP same VA = map reclaim soft (re-install PTEs, re-export).
 * UDX/netstackd/host ring handoff residual (Soft!=product; product_dod_b=UDX):
 *   netstackd CLAIM then MAP_RING so udx_virtq_attach(vaBase, export) can
 *   program desc/avail/used at vaBase+off_*. Ring map notes (sparse residual):
 *   map_va / map_which / map_ok / map_reclaim / map_nodev / map_fault via
 *   net_door_ring_map_va() + residual lamps only.
 * greppable: net_door: ring map notes | udx_host_stack | handoff=netstackd|UDX
 */
#define GJ_NET_OP_MAP_RING    16u
/**
 * KICK: arg1=which - notify device of available buffers.
 * Soft: NODEV without virtio-net (client soft-skips). H1: door thr only.
 */
#define GJ_NET_OP_KICK        17u
/**
 * AVAIL_PUSH (UDX ring programming / host stack):
 *   arg1=which (0=rx,1=tx) arg2=user buf arg3=len
 * Soft residual: avail_push tally on ring map notes (not per-call dump).
 */
#define GJ_NET_OP_AVAIL_PUSH  18u
/** USED_REAP: arg1=which arg2=max -> reaped count (0 if empty/absent) */
#define GJ_NET_OP_USED_REAP   19u
/**
 * RING_STATE: arg1=user u32[4] {free_tx, free_rx, avail_pushes, vq_calls}
 * Soft: always fills zeros when virtio-net is absent (no hard fail).
 * No per-RING_STATE residual dump (stamp storm risk under host poll loops).
 */
#define GJ_NET_OP_RING_STATE  20u
/**
 * MAP_DMA: arg1=user VA base (page-aligned) arg2=user ptr to gj_virtq_dma_export
 * UDX DMA window handoff residual (Soft!=product; host stack deepen).
 * Last success VA via net_door_map_dma_va(); tallies on ring map notes.
 * greppable: udx_host_stack=claim|map_ring|map_dma|desc|user_avail|bounce
 * greppable: net_door: ring map notes
 */
#define GJ_NET_OP_MAP_DMA     21u
/**
 * DESC_ALLOC: arg1=which -> free descriptor index (userspace fills desc).
 * UDX/host desc programming residual (Soft!=product). After MAP_RING attach.
 */
#define GJ_NET_OP_DESC_ALLOC  22u
/**
 * USER_AVAIL: arg1=which arg2=desc_head arg3=flags
 *   bit0=kick  bit1=kernel push avail
 * UDX userspace avail-ring programming (product path toward UDX host stack).
 * H1: kick path remains door thr (not IRQ).
 */
#define GJ_NET_OP_USER_AVAIL  23u
/**
 * BOUNCE_FILL: arg1=slot arg2=user buf arg3=len
 * Copy frame into DMA slot (TX prepends virtio-net hdr). Returns filled len.
 * UDX/host bounce residual (Soft!=product); pairs with MAP_DMA slots.
 */
#define GJ_NET_OP_BOUNCE_FILL 24u
/**
 * ACCEPT: arg1=listen fd -> connected peer fd or -EAGAIN (-11 soft empty).
 * sshd interim park: empty accept is soft EAGAIN (Soft!=product; not hard-fail).
 * Product DoD B = UDX not freestanding rtl; :22 remains OPEN until interactive SSH login.
 */
#define GJ_NET_OP_ACCEPT 25u
/**
 * TCP_STATS: arg1=user u32[4]
 *   [0] accepts
 *   [1] low16=segments, high16=retransmits
 *   [2] rx_bytes
 *   [3] tx_bytes (full 32-bit)
 */
#define GJ_NET_OP_TCP_STATS 26u
/**
 * SOCK_POLL: arg1=fd arg2=want (POLLIN=1 POLLOUT=4 POLLERR=8 POLLHUP=0x10)
 * -> readiness mask bits via net_tcp_poll_mask. Non-TCP fd -> 0 soft.
 */
#define GJ_NET_OP_SOCK_POLL 27u
/**
 * ETH_INJECT: arg1=user frame ptr arg2=len (14..1514 soft).
 * UDX host thr-poll RX residual → net_eth demux (ARP/ICMP/TCP:22).
 * Soft!=product Dual DoD B path=rtl8168_udx. greppable: ETH_INJECT
 */
#define GJ_NET_OP_ETH_INJECT 28u
/**
 * ETH_TX_PULL: arg1=user buf arg2=max_len → bytes copied of soft TX queue
 * (or 0 empty). UDX host park drains demux ARP/ICMP/TCP replies when
 * freestanding backend=none. Soft!=product. greppable: ETH_TX_PULL
 */
#define GJ_NET_OP_ETH_TX_PULL 29u
/**
 * ETH_UDX_READY: arg1=1 arm / 0 drop. Marks product UDX L2 soft ready so
 * net_l2_ready / soft TX enqueue work under freestanding rtl SKIP.
 * Soft!=product Dual DoD B. greppable: ETH_UDX_READY
 */
#define GJ_NET_OP_ETH_UDX_READY 30u
/**
 * ETH_SET_MAC: arg1=user ptr to 6-byte station MAC (product IDR).
 * Publishes into net_l2 soft demux so ARP SHA matches on-wire IDR keep=1.
 * Soft!=product Dual DoD B. greppable: ETH_SET_MAC | net_l2: soft station mac
 */
#define GJ_NET_OP_ETH_SET_MAC 31u

/**
 * Soft UDX TX enqueue for demux replies when freestanding backend=none.
 * Returns 0 if queued, -1 if not ready / full / inval. Soft!=product.
 */
int net_door_udx_tx_soft(const void *pFrame, u32 cb);

/** Live ETH_TX_PULL queue depth (0 = empty). Soft!=product. */
u32 net_door_udx_tx_pending(void);

/** Non-zero when ETH_UDX_READY armed (product UDX L2 soft). Soft!=product. */
int net_door_udx_ready(void);

/**
 * One-shot init of ownership/soft counters. Safe to re-call (idempotent soft).
 * Call from kmain after virtio-net probe path (device may still be absent).
 */
void net_door_init(void);

/**
 * Dispatch net opcode. Returns bytes / fd / 0 / 1 or negative error.
 * Unknown op -> -ENOSYS-shaped. Not ready / null context -> soft fail.
 */
i64  net_door_call(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3);

/** Non-zero when a userspace owner token is held (CLAIM succeeded). */
int  net_door_owned(void);

/** Current owner token, or 0 if kernel interim owns policy. */
u32  net_door_owner_token(void);

/**
 * Last successful MAP_RING user VA base, or 0 if never mapped.
 * Ring map notes residual (Soft!=product): sparse lamps print map_va/map_held.
 * UDX attach: udx_virtq_attach((void *)va, &export) after MAP_RING.
 * greppable: net_door: ring map notes
 */
u64  net_door_ring_map_va(void);

/**
 * Last successful MAP_DMA user VA base, or 0 if never mapped.
 * Ring map notes residual (Soft!=product): dma_va on sparse lamps.
 * greppable: net_door: ring map notes | map_dma
 */
u64  net_door_map_dma_va(void);

/**
 * Soft path: EXPORT/MAP/KICK/RING_STATE (+ MAP_DMA/DESC/USER_AVAIL/BOUNCE)
 * call count. UDX/host stack residual tally surface (Soft!=product).
 */
u32  net_door_ring_calls(void);

/**
 * Soft diagnostics: first claims + idempotent reclaims.
 * greppable: net_door claim soft
 * greppable: udx_host_stack=claim|map_ring|map_dma|desc|user_avail|bounce
 * greppable: handoff=netstackd|sshd|UDX
 */
u32  net_door_claim_count(void);
