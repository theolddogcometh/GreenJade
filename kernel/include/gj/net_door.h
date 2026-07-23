/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Net door — netstackd hand-off surface (A1).
 * Pure C, dual-licensed. Ownership grants exclusive virtio TX/RX queue
 * access via door ops.
 *
 * SEND/RECV bounce is NET_XFER_MAX (4096) in net_door.c — must stay large
 * enough for multi-seg TCP bulk (e.g. 3000 B > MSS) in one door call.
 */
#pragma once

#include <gj/types.h>

#define GJ_NET_OP_POLL   1u /* drain virtio-net / eth poll */
#define GJ_NET_OP_STATS  2u /* arg1=user u32[4] arp,udp,icmp,calls */
/** socket: arg1=domain arg2=type arg3=proto → fd */
#define GJ_NET_OP_SOCKET 3u
/** bind: arg1=fd arg2=port */
#define GJ_NET_OP_BIND   4u
/**
 * send: arg1=fd arg2=user buf arg3=len → bytes (clamped to 4 KiB bounce;
 * STREAM multi-seg chunking is internal to net_tcp_send).
 */
#define GJ_NET_OP_SEND   5u
/** recv: arg1=fd arg2=user buf arg3=len → bytes (≤4 KiB; short OK) */
#define GJ_NET_OP_RECV   6u
/** connect: arg1=fd arg2=port */
#define GJ_NET_OP_CONNECT 7u
/** close: arg1=fd */
#define GJ_NET_OP_CLOSE  8u
/** listen: arg1=fd */
#define GJ_NET_OP_LISTEN 9u
/** claim: arg1=token — netstackd ownership */
#define GJ_NET_OP_CLAIM  10u
/** release: arg1=token */
#define GJ_NET_OP_RELEASE 11u
/**
 * Virtio queue (owned path preferred; allowed without claim for bring-up):
 * VIRTIO_TX: arg1=user eth frame, arg2=len → 0 or -errno
 * VIRTIO_RX: arg1=user buf, arg2=max → nbytes
 * QUEUE_INFO: arg1=user u32[5] {tx,rx,ready,owned,vq_calls}
 */
#define GJ_NET_OP_VIRTIO_TX  12u
#define GJ_NET_OP_VIRTIO_RX  13u
#define GJ_NET_OP_QUEUE_INFO 14u
/** EXPORT_RING: arg1=which(0=rx,1=tx) arg2=user ptr to gj_virtq_export */
#define GJ_NET_OP_EXPORT_RING 15u
/** MAP_RING: arg1=which arg2=user VA base (page-aligned) arg3=export out ptr */
#define GJ_NET_OP_MAP_RING    16u
/** KICK: arg1=which — notify device of available buffers */
#define GJ_NET_OP_KICK        17u
/**
 * AVAIL_PUSH (UDX ring programming):
 *   arg1=which (0=rx,1=tx) arg2=user buf arg3=len
 *   Copies to bounce, fills desc, pushes avail, kicks TX.
 */
#define GJ_NET_OP_AVAIL_PUSH  18u
/** USED_REAP: arg1=which arg2=max → reaped count */
#define GJ_NET_OP_USED_REAP   19u
/** RING_STATE: arg1=user u32[4] {free_tx, free_rx, avail_pushes, vq_calls} */
#define GJ_NET_OP_RING_STATE  20u
/**
 * MAP_DMA: arg1=user VA base (page-aligned) arg2=user ptr to gj_virtq_dma_export
 * Maps bounce pool for userspace payload + returns PAs.
 */
#define GJ_NET_OP_MAP_DMA     21u
/**
 * DESC_ALLOC: arg1=which → free descriptor index (userspace fills desc fields).
 */
#define GJ_NET_OP_DESC_ALLOC  22u
/**
 * USER_AVAIL: arg1=which arg2=desc_head arg3=flags
 *   bit0=kick  bit1=kernel push avail (else userspace already wrote avail ring)
 * Userspace has programmed desc[head] on MAP_RING VA.
 */
#define GJ_NET_OP_USER_AVAIL  23u
/**
 * BOUNCE_FILL: arg1=slot arg2=user buf arg3=len
 * Copy frame into DMA slot (TX prepends virtio-net hdr). Returns filled len.
 */
#define GJ_NET_OP_BOUNCE_FILL 24u
/**
 * ACCEPT: arg1=listen fd → connected peer fd or -EAGAIN.
 * Used by product sshd after LISTEN + client CONNECT on net_lo.
 */
#define GJ_NET_OP_ACCEPT 25u
/** TCP_STATS: arg1=user u32[4] {accepts, segs, rx_bytes, tx_bytes} */
#define GJ_NET_OP_TCP_STATS 26u

void net_door_init(void);
i64  net_door_call(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3);
int  net_door_owned(void);
