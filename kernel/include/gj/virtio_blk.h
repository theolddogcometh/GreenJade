/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-blk (modern PCI) — pure C, dual license, no GPL.
 * Product storage path: request virtqueue (q0) sector R/W for store_door /
 * storaged / vfsd. OASIS virtio-blk; no Linux virtio source.
 *
 * Soft product depth (bring-up / smoke):
 *   queue stats, multi-segment soft bounce, FLUSH/sync depth-1.
 * Greppable markers (prefix-stable):
 *   "virtio-blk: ready …"
 *   "virtio-blk: soft queue stats …"
 *   "virtio-blk: soft multi-seg …"
 *   "virtio-blk: soft flush …"
 */
#pragma once

#include <gj/types.h>
#include <gj/virtio.h>

/** Logical block size (OASIS virtio-blk sector). */
#define GJ_VIRTIO_BLK_SECTOR 512u

/** Request virtqueue index (only queue used by this driver). */
#define GJ_VIRTIO_BLK_Q_REQUEST 0u

/**
 * Soft multi-segment ceiling: max sectors per single hdr+data+status chain.
 * Multi-sector R/W walks this soft window (one contiguous data desc).
 * Matches store_door bounce budget (4 KiB) for product path.
 */
#define GJ_VIRTIO_BLK_SOFT_SEGS 8u

/**
 * Request-queue soft product stats (fill via virtio_blk_q_stats).
 * Wire-stable field order for door/UDX snapshots; extend only at end.
 */
struct gj_virtio_blk_q_stats {
    u32 u32IoCount;     /* completed R/W (+ successful device FLUSH) ops */
    u32 u32Kicks;       /* virtqueue notifies issued */
    u32 u32MultiSegOps; /* xfers that used >1 soft sector segment */
    u32 u32MultiSegs;   /* total soft sector-segments across multi ops */
    u32 u32FlushCount;  /* device FLUSH OK completions */
    u32 u32FlushSoft;   /* soft fsync: UNSUP or no-op barrier depth */
    u32 u32Errors;      /* timeout / status / q_add failures */
    u32 u32BytesIn;     /* bytes read (IN) */
    u32 u32BytesOut;    /* bytes written (OUT) */
    u16 u16QSize;       /* request queue size */
    u16 u16FreeMin;     /* lowest free-desc watermark since probe */
    u16 u16FreeNow;     /* free descriptors now */
    u16 u16FlushDepth;  /* max concurrent FLUSH soft depth (1 = serial) */
};

/** Probe first virtio-blk; set up request q0. Returns 0 on success. */
int  virtio_blk_probe(void);
/** Non-zero when DRIVER_OK and request queue are live. */
int  virtio_blk_ready(void);

/** Capacity in 512-byte sectors (0 if not ready). */
u64  virtio_blk_capacity_sectors(void);

/**
 * Read/write contiguous bytes starting at sector (length must be a multiple
 * of GJ_VIRTIO_BLK_SECTOR). Soft multi-segment: up to GJ_VIRTIO_BLK_SOFT_SEGS
 * sectors per request chain; longer xfers loop. Returns 0 on success, -1.
 */
int  virtio_blk_read(u64 u64Sector, void *pBuf, u32 cbLen);
int  virtio_blk_write(u64 u64Sector, const void *pBuf, u32 cbLen);

/**
 * FLUSH / fsync-shaped barrier on the request queue (OASIS T_FLUSH).
 * Depth-1 serial soft path (one outstanding). Device UNSUP → soft success
 * (fsync-shaped) and increments flush_soft. Returns 0 on OK/soft, -1 hard.
 */
int  virtio_blk_flush(void);

/** Completed R/W (and successful device FLUSH) operations since probe. */
u32  virtio_blk_io_count(void);
/** Device FLUSH OK count since probe. */
u32  virtio_blk_flush_count(void);
/** Soft FLUSH (UNSUP / soft barrier) count since probe. */
u32  virtio_blk_flush_soft_count(void);
/** Snapshot request-queue soft product stats into *pOut. Returns 0 or -1. */
int  virtio_blk_q_stats(struct gj_virtio_blk_q_stats *pOut);

/** Export request queue rings for UDX / store_door MAP_RING. */
int  virtio_blk_export_q(struct gj_virtq_export *pOut);
/** Notify device of available buffers on the request queue. */
int  virtio_blk_kick_q(void);
/** Free descriptor count on the request queue (0 if not ready). */
u16  virtio_blk_q_free(void);
/**
 * Map request ring pages into the active address space at page-aligned
 * vaBase: desc | avail | used (one page each). Fills *pOut if non-NULL.
 */
int  virtio_blk_map_q_user(u64 u64VaBase, struct gj_virtq_export *pOut);
