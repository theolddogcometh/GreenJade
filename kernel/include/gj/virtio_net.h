/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-net (modern PCI): RX/TX multi-buffer soft + UDX ring
 * export (OASIS). No Linux virtio source. Dual MIT OR Apache-2.0 only.
 *
 * Greppable product markers (prefix-stable):
 *   virtio-net: ready PASS
 *   virtio-net: features
 *   virtio-net: multi-buf
 */
#pragma once

#include <gj/types.h>
#include <gj/virtio.h>

/** Soft multi-buffer RX pool depth (posted at probe / refill). */
#define GJ_VIRTIO_NET_RX_N 4u
/** RX slot size (hdr + MTU headroom; one desc per slot). */
#define GJ_VIRTIO_NET_RX_SZ 2048u

/**
 * Driver soft stats (kernel TX/RX path + UDX ring helpers).
 * All counters are lifetime since last successful probe.
 */
struct gj_virtio_net_stats {
    u32 u32TxCount;        /* completed kernel TX submits counted as success */
    u32 u32RxCount;        /* RX frames delivered to virtio_net_rx */
    u32 u32TxFail;         /* TX rejected (queue full / bad args after ready) */
    u32 u32TxTimeout;      /* TX poll exhausted without used */
    u32 u32TxMulti;        /* TX via 2-desc chain (hdr | payload) */
    u32 u32TxSingle;       /* TX via single contiguous desc fallback */
    u32 u32RxDrop;         /* RX truncated / undersized / merge fail soft-drop */
    u32 u32RxEmpty;        /* RX poll with no used entry */
    u32 u32RxPosted;       /* RX slots currently on the device */
    u32 u32RxPostFail;     /* failed RX refill posts */
    u32 u32RxMerge;        /* frames soft-merged across >1 buffer */
    u32 u32Kicks;          /* notify writes from this driver */
    u32 u32Reaps;          /* used entries reaped (used_reap + TX poll frees) */
    u32 u32AvailPushes;    /* bounce AVAIL_PUSH + user_avail */
    u32 u32UserRingPushes; /* userspace MAP_RING path only */
    u64 u64TxBytes;        /* Ethernet payload bytes accepted on TX */
    u64 u64RxBytes;        /* Ethernet payload bytes delivered on RX */
    u64 u64Features;       /* negotiated guest features snapshot */
    u64 u64FeaturesDev;    /* device-offered features at negotiate time */
};

int  virtio_net_probe(void);
int  virtio_net_ready(void);
/** Send one frame (raw Ethernet). Returns 0 on success, -1 on fail. */
int  virtio_net_tx(const void *pFrame, u32 cbLen);
/**
 * Poll RX: copy up to cbMax bytes of Ethernet payload (no virtio-net hdr)
 * into pOut. Soft multi-buffer: may merge MRG_RXBUF chains. Returns length
 * or 0 if empty, -1 if not ready.
 */
i32  virtio_net_rx(void *pOut, u32 cbMax);
u32  virtio_net_tx_count(void);
u32  virtio_net_rx_count(void);
/** Soft stats getters (subset also in virtio_net_stats). */
u32  virtio_net_tx_fail_count(void);
u32  virtio_net_tx_timeout_count(void);
u32  virtio_net_rx_drop_count(void);
u32  virtio_net_kick_count(void);
u32  virtio_net_rx_posted(void);
u64  virtio_net_tx_bytes(void);
u64  virtio_net_rx_bytes(void);
/** Negotiated guest features (0 if not ready). */
u64  virtio_net_features(void);
/** Device-offered features snapshot from last probe negotiate. */
u64  virtio_net_features_dev(void);
/** Fill *pOut with full soft stats; returns 0 or -1 if pOut NULL. */
int  virtio_net_stats(struct gj_virtio_net_stats *pOut);

/** Ring export for UDX / door MAP_RING (which: 0=RX 1=TX). */
int  virtio_net_export_q(u16 u16Which, struct gj_virtq_export *pOut);
int  virtio_net_kick_q(u16 u16Which);
/** Map ring pages into active AS at vaBase (desc|avail|used contiguous). */
int  virtio_net_map_q_user(u16 u16Which, u64 u64VaBase, struct gj_virtq_export *pOut);
/** Map bounce DMA pool into user VA; export PAs for desc.addr. */
int  virtio_net_map_dma_user(u64 u64VaBase, struct gj_virtq_dma_export *pOut);
/** Allocate free desc for userspace programming. Returns head ≥0 or -1. */
int  virtio_net_desc_alloc(u16 u16Which);
/**
 * After userspace programmed desc[head] (and optionally avail):
 * fFlags bit0=kick, bit1=kernel also pushes avail (else user wrote avail).
 * Counts as userspace ring push (not kernel TX / AVAIL_PUSH bounce).
 */
int  virtio_net_user_avail(u16 u16Which, u16 u16Head, int fFlags);
/** Fill bounce slot (for kernel smoke / helper); returns byte count or -1. */
int  virtio_net_bounce_fill(u32 u32Slot, const void *pBuf, u32 cbLen, int fTxHdr);
/** Physical address of bounce slot (for desc.addr). */
gj_paddr_t virtio_net_bounce_pa(u32 u32Slot);

/**
 * Ring-level submit (UDX path): copy user frame into bounce, AVAIL push, optional kick.
 * fKick: non-zero → notify device. fWrite: 1 for RX-style device-write buffer.
 * Returns 0 on success.
 */
int  virtio_net_avail_push(u16 u16Which, const void *pBuf, u32 cbLen, int fWrite,
                           int fKick);
/** Reap completed TX/RX used entries. Returns count. */
u32  virtio_net_used_reap(u16 u16Which, u32 u32Max);
u16  virtio_net_q_free(u16 u16Which);
u32  virtio_net_avail_pushes(void);
u32  virtio_net_user_ring_pushes(void);
