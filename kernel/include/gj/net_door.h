/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Net door — netstackd hand-off surface (A1 / product path).
 * Pure C11 freestanding, dual MIT OR Apache-2.0. Dispatched via GJ_SYS_NET
 * (arg0 = opcode). Ownership grants exclusive virtio TX/RX queue policy
 * to userspace netstackd while the kernel still owns the interim socket
 * and eth demux path until full userspace stack migration.
 *
 * Dispatch contract:
 *   net_door_call(op, arg1, arg2, arg3) → i64
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
 * Soft paths (bring-up / product smoke — keep greppable markers stable):
 *   Virtio queue / ring ops are allowed without claim for smokes;
 *   netstackd prefers the owned path.
 *   EXPORT/MAP/KICK soft-skip with NODEV when virtio-net is absent.
 *   RING_STATE always succeeds (free=0 / pushes=0 without device).
 *   MAP records last user VA; re-MAP of the same VA is a soft reclaim
 *   of the map window (re-install PTEs, re-export).
 *
 * Bounce / sizing (must stay aligned with net_tcp multi-seg bulk):
 *   SEND/RECV bounce is NET_XFER_MAX (4096) in net_door.c — large enough
 *   for multi-seg TCP bulk (e.g. 3000 B > MSS) in one door call.
 *   STREAM multi-seg chunking is internal to net_tcp_send; door clamps.
 *
 * Socket path vs virtio path:
 *   SOCKET..ACCEPT / TCP_STATS  → net_lo + net_tcp interim tables
 *   POLL / VIRTIO_* / ring ops  → virtio-net + net_eth demux
 *   netstackd CLAIM then owns ring MAP/KICK/AVAIL programming (UDX)
 *
 * Greppable product markers (main / netstackd; keep ABI stable):
 *   net_door: PASS / ownership PASS / socket path PASS /
 *   virtio queue PASS / ring map PASS / avail push PASS / user ring PASS
 *   net_door claim soft
 */
#pragma once

#include <gj/types.h>

/* ---- door opcodes (GJ_SYS_NET arg0) ---------------------------------- */

#define GJ_NET_OP_POLL   1u /* drain virtio-net / eth poll (net_eth_poll) */
#define GJ_NET_OP_STATS  2u /* arg1=user u32[4] arp,udp,icmp,calls */
/** socket: arg1=domain arg2=type arg3=proto → fd (net_lo / net_tcp) */
#define GJ_NET_OP_SOCKET 3u
/** bind: arg1=fd arg2=port (host order u16 in low bits) */
#define GJ_NET_OP_BIND   4u
/**
 * send: arg1=fd arg2=user buf arg3=len → bytes (clamped to 4 KiB bounce;
 * STREAM multi-seg chunking is internal to net_tcp_send).
 * Short writes possible on peer window / RX pressure.
 */
#define GJ_NET_OP_SEND   5u
/** recv: arg1=fd arg2=user buf arg3=len → bytes (≤4 KiB; short OK; 0=EOF soft) */
#define GJ_NET_OP_RECV   6u
/** connect: arg1=fd arg2=port — loopback pair or SYN to local listener */
#define GJ_NET_OP_CONNECT 7u
/** close: arg1=fd — soft FIN on TCP ESTABLISHED/CLOSE_WAIT */
#define GJ_NET_OP_CLOSE  8u
/** listen: arg1=fd — store/clamp soft backlog */
#define GJ_NET_OP_LISTEN 9u
/**
 * claim: arg1=non-zero 32-bit token — netstackd ownership.
 * Soft reclaim: same token re-CLAIM is idempotent (claim soft count).
 * Different token → BUSY. token 0 → INVAL.
 */
#define GJ_NET_OP_CLAIM  10u
/** release: arg1=token (must match when owned; soft 0 when free) */
#define GJ_NET_OP_RELEASE 11u
/**
 * Virtio queue (owned path preferred; allowed without claim for bring-up):
 * VIRTIO_TX: arg1=user eth frame, arg2=len → 0 or -errno
 * VIRTIO_RX: arg1=user buf, arg2=max → nbytes (0 empty, -errno fail)
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
 * MAP_RING: arg1=which arg2=user VA base (page-aligned) arg3=export out ptr
 * Soft: NODEV without device; re-MAP same VA is map reclaim soft
 * (re-install PTEs, refresh export; last VA recorded for diagnostics).
 */
#define GJ_NET_OP_MAP_RING    16u
/** KICK: arg1=which — notify device of available buffers (NODEV soft-skip) */
#define GJ_NET_OP_KICK        17u
/**
 * AVAIL_PUSH (UDX ring programming):
 *   arg1=which (0=rx,1=tx) arg2=user buf arg3=len
 *   Copies to bounce, fills desc, pushes avail, kicks TX.
 *   Counts toward RING_STATE avail_pushes.
 */
#define GJ_NET_OP_AVAIL_PUSH  18u
/** USED_REAP: arg1=which arg2=max → reaped count (0 if empty/absent) */
#define GJ_NET_OP_USED_REAP   19u
/**
 * RING_STATE: arg1=user u32[4] {free_tx, free_rx, avail_pushes, vq_calls}
 * Soft: always fills zeros when virtio-net is absent (no hard fail).
 * aS[2] packs high16=user_ring_pushes, low16=total avail pushes.
 */
#define GJ_NET_OP_RING_STATE  20u
/**
 * MAP_DMA: arg1=user VA base (page-aligned) arg2=user ptr to gj_virtq_dma_export
 * Maps bounce pool for userspace payload + returns PAs (UDX DMA path).
 */
#define GJ_NET_OP_MAP_DMA     21u
/**
 * DESC_ALLOC: arg1=which → free descriptor index (userspace fills desc fields
 * on the MAP_RING VA). -errno if queue full / not ready.
 */
#define GJ_NET_OP_DESC_ALLOC  22u
/**
 * USER_AVAIL: arg1=which arg2=desc_head arg3=flags
 *   bit0=kick  bit1=kernel push avail (else userspace already wrote avail ring)
 * Userspace has programmed desc[head] on MAP_RING VA (user ring path).
 */
#define GJ_NET_OP_USER_AVAIL  23u
/**
 * BOUNCE_FILL: arg1=slot arg2=user buf arg3=len
 * Copy frame into DMA slot (TX prepends virtio-net hdr). Returns filled len.
 */
#define GJ_NET_OP_BOUNCE_FILL 24u
/**
 * ACCEPT: arg1=listen fd → connected peer fd or -EAGAIN.
 * Used by product sshd after LISTEN + client CONNECT on net_lo / net_tcp.
 */
#define GJ_NET_OP_ACCEPT 25u
/** TCP_STATS: arg1=user u32[4] {accepts, segs, rx_bytes, tx_bytes} */
#define GJ_NET_OP_TCP_STATS 26u

/**
 * One-shot init of ownership/soft counters. Safe to re-call (idempotent soft).
 * Call from kmain after virtio-net probe path (device may still be absent).
 */
void net_door_init(void);

/**
 * Dispatch net opcode. Returns bytes / fd / 0 / 1 or negative error.
 * Unknown op → -ENOSYS-shaped. Not ready / null context → soft fail.
 */
i64  net_door_call(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3);

/** Non-zero when a userspace owner token is held (CLAIM succeeded). */
int  net_door_owned(void);

/** Current owner token, or 0 if kernel interim owns policy. */
u32  net_door_owner_token(void);

/** Last successful MAP_RING user VA base, or 0 if never mapped. */
u64  net_door_ring_map_va(void);

/** Soft path: EXPORT/MAP/KICK/RING_STATE (+ related ring) call count. */
u32  net_door_ring_calls(void);

/**
 * Soft diagnostics: first claims + idempotent reclaims.
 * greppable: net_door claim soft
 */
u32  net_door_claim_count(void);
