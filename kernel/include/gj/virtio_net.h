/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-net (modern PCI): RX/TX multi-buffer soft + UDX ring
 * export (OASIS). Pure C11 freestanding. Dual MIT OR Apache-2.0 only.
 * No Linux virtio source — feature bits and hdr layout are public OASIS.
 *
 * Queues (fixed product indices):
 *   q0 RX — device-write buffers posted from soft pool (GJ_VIRTIO_NET_RX_N)
 *   q1 TX — guest-read frames (2-desc chain preferred: hdr | payload)
 *
 * Feature soft set (negotiate ladder in driver):
 *   VERSION_1 required; MAC / STATUS / MRG_RXBUF accepted when offered.
 *   MRG_RXBUF enables soft multi-buffer RX merge across posted slots.
 *
 * Soft product depth:
 *   - multi-buf RX pool + refill; merge chains into one Ethernet frame
 *   - TX 2-desc chain with single-desc contiguous fallback
 *   - bounce DMA pool for UDX avail_push / map_dma_user
 *   - full soft stats (tx/rx bytes, fails, kicks, user ring pushes)
 *
 * Consumers: net_door / netstackd / kernel smoke TX-RX loopback paths.
 *
 * Bring-up lifecycle:
 *   scan → first KIND_NET → setup → negotiate soft → q_setup RX+TX
 *   → post RX slots → driver_ok → ready PASS
 *
 * Greppable product markers (prefix-stable):
 *   virtio-net: ready PASS
 *   virtio-net: features
 *   virtio-net: multi-buf
 *
 * greppable: GJ_VIRTIO_NET_ virtio_net_probe virtio_net_stats
 */
#pragma once

#include <gj/types.h>
#include <gj/virtio.h>

/** Soft multi-buffer RX pool depth (posted at probe / refill). */
#define GJ_VIRTIO_NET_RX_N 4u
/**
 * RX slot size (virtio-net hdr + MTU headroom; one desc per slot on soft
 * path). 2048 covers standard Ethernet MTU + 1.x hdr with room for merge
 * scrap. Not a hard host MTU — truncation is soft-dropped (rx_drop).
 */
#define GJ_VIRTIO_NET_RX_SZ 2048u

/**
 * Driver soft stats (kernel TX/RX path + UDX ring helpers).
 * All counters are lifetime since last successful probe (wrap OK).
 * Wire-stable field order; extend only at end for door snapshots.
 *
 * u64Features / u64FeaturesDev mirror negotiate snapshot for smokes
 * without calling features_* helpers.
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

/**
 * Probe first virtio-net; negotiate + RX/TX queues + initial RX post.
 * Returns 0 on success, -1 on no device / setup / queue failure.
 * Partial RX post failure still returns 0 (tx-only until refill) when
 * DRIVER_OK was reached — see ready + rx_posted.
 */
int  virtio_net_probe(void);

/** Non-zero when DRIVER_OK and TX (and usually RX) queues are live. */
int  virtio_net_ready(void);

/**
 * Send one frame (raw Ethernet, no virtio-net hdr — driver prepends).
 * cbLen is payload length. Soft path: bounce + kick + poll completion.
 * Returns 0 on success, -1 on not ready / bad args / full / timeout.
 */
int  virtio_net_tx(const void *pFrame, u32 cbLen);

/**
 * Poll RX: copy up to cbMax bytes of Ethernet payload (no virtio-net hdr)
 * into pOut. Soft multi-buffer: may merge MRG_RXBUF chains into one frame.
 * Returns length >0 on frame, 0 if empty, -1 if not ready / bad arg.
 * Does not block beyond a short used-ring poll; empty is common.
 */
i32  virtio_net_rx(void *pOut, u32 cbMax);

/** Lifetime successful kernel TX count (subset of stats). */
u32  virtio_net_tx_count(void);
/** Lifetime RX frames delivered via virtio_net_rx. */
u32  virtio_net_rx_count(void);

/** Soft stats getters (subset also in virtio_net_stats). */
u32  virtio_net_tx_fail_count(void);
u32  virtio_net_tx_timeout_count(void);
u32  virtio_net_rx_drop_count(void);
u32  virtio_net_kick_count(void);
/** RX slots currently posted on the device (refill target GJ_VIRTIO_NET_RX_N). */
u32  virtio_net_rx_posted(void);
u64  virtio_net_tx_bytes(void);
u64  virtio_net_rx_bytes(void);

/** Negotiated guest features (0 if not ready). */
u64  virtio_net_features(void);
/** Device-offered features snapshot from last probe negotiate. */
u64  virtio_net_features_dev(void);

/**
 * Fill *pOut with full soft stats; returns 0 or -1 if pOut NULL.
 * Works when ready; when not ready may still fill zeros / last snapshot
 * depending on implementation — prefer check ready first for smokes.
 */
int  virtio_net_stats(struct gj_virtio_net_stats *pOut);

/* ---- UDX / door ring + bounce export ------------------------------------ */

/**
 * Ring export for UDX / door MAP_RING.
 * u16Which: 0 = RX, 1 = TX. Returns 0 on success, -1 if not ready / bad which.
 */
int  virtio_net_export_q(u16 u16Which, struct gj_virtq_export *pOut);

/** Kick queue u16Which (0 RX / 1 TX). Returns 0 / -1. */
int  virtio_net_kick_q(u16 u16Which);

/**
 * Map ring pages into active AS at page-aligned vaBase
 * (desc | avail | used contiguous). Fills *pOut if non-NULL.
 */
int  virtio_net_map_q_user(u16 u16Which, u64 u64VaBase,
                           struct gj_virtq_export *pOut);

/**
 * Map bounce DMA pool into user VA; export PAs for desc.addr.
 * Slot count/size are driver-defined (see dma export u32NSlots/u32SlotSz).
 */
int  virtio_net_map_dma_user(u64 u64VaBase, struct gj_virtq_dma_export *pOut);

/** Allocate free desc for userspace programming. Returns head ≥0 or -1. */
int  virtio_net_desc_alloc(u16 u16Which);

/**
 * After userspace programmed desc[head] (and optionally avail):
 * fFlags bit0 = kick, bit1 = kernel also pushes avail (else user wrote avail).
 * Counts as userspace ring push (not kernel TX / AVAIL_PUSH bounce).
 * Returns 0 on success, -1 on error.
 */
int  virtio_net_user_avail(u16 u16Which, u16 u16Head, int fFlags);

/**
 * Fill bounce slot (for kernel smoke / helper).
 * fTxHdr non-zero: prepend soft virtio-net hdr for TX-shaped buffers.
 * Returns byte count written or -1.
 */
int  virtio_net_bounce_fill(u32 u32Slot, const void *pBuf, u32 cbLen, int fTxHdr);

/** Physical address of bounce slot (for desc.addr); 0 if invalid slot. */
gj_paddr_t virtio_net_bounce_pa(u32 u32Slot);

/**
 * Ring-level submit (UDX path): copy user frame into bounce, AVAIL push,
 * optional kick. fKick: non-zero → notify device. fWrite: 1 for RX-style
 * device-write buffer. Returns 0 on success, -1 on fail.
 */
int  virtio_net_avail_push(u16 u16Which, const void *pBuf, u32 cbLen, int fWrite,
                           int fKick);

/**
 * Reap completed TX/RX used entries without blocking spin.
 * Returns count reaped (0 if none). Frees descriptor chains.
 */
u32  virtio_net_used_reap(u16 u16Which, u32 u32Max);

/** Free descriptor count on queue u16Which (0 if not ready / bad which). */
u16  virtio_net_q_free(u16 u16Which);

/** Lifetime AVAIL_PUSH + user_avail counts (kernel bounce path). */
u32  virtio_net_avail_pushes(void);
/** Lifetime userspace MAP_RING pushes only. */
u32  virtio_net_user_ring_pushes(void);
