/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-blk (modern PCI) — pure C11 freestanding, dual license,
 * no GPL. Product storage path: request virtqueue (q0) sector R/W for
 * store_door / storaged / vfsd. OASIS virtio-blk layout numbers only;
 * no Linux virtio source.
 *
 * Queue layout (only queue used by this driver):
 *   q0 request — hdr (device-R) + data (R or W) + status (device-W)
 *                FLUSH: hdr + status only (no data desc)
 *
 * Request types (OASIS public; implemented in driver .c, not re-exported):
 *   T_IN=0 read, T_OUT=1 write, T_FLUSH=4 barrier
 * Status: S_OK=0, S_IOERR=1, S_UNSUP=2 (UNSUP on FLUSH → soft fsync success)
 *
 * Soft product depth (bring-up / smoke):
 *   - queue stats (kicks, free watermark, multi-seg, flush counters)
 *   - multi-segment soft bounce (GJ_VIRTIO_BLK_SOFT_SEGS sectors / chain)
 *   - FLUSH/sync serial depth-1 (one outstanding barrier)
 *
 * Bring-up lifecycle:
 *   virtio_pci_scan → probe first KIND_BLK → setup → negotiate VERSION_1
 *   → q_setup(q0) → driver_ok → capacity from device config (8-byte sectors)
 *
 * Export: store_door MAP_RING uses export_q / map_q_user on request q0.
 *
 * Greppable markers (prefix-stable; serial in virtio_blk.c):
 *   "virtio-blk: ready …"
 *   "virtio-blk: soft queue stats …"
 *   "virtio-blk: soft multi-seg …"
 *   "virtio-blk: soft flush …"
 *
 * greppable: GJ_VIRTIO_BLK_ virtio_blk_probe virtio_blk_q_stats
 */
#pragma once

#include <gj/types.h>
#include <gj/virtio.h>

/** Logical block size (OASIS virtio-blk sector). All R/W lengths multiple of this. */
#define GJ_VIRTIO_BLK_SECTOR 512u

/** Request virtqueue index (only queue used by this driver). */
#define GJ_VIRTIO_BLK_Q_REQUEST 0u

/**
 * Soft multi-segment ceiling: max sectors per single hdr+data+status chain.
 * Multi-sector R/W walks this soft window (one contiguous data desc per
 * chain). Matches store_door bounce budget (4 KiB) for product path:
 * 8 * 512 = 4096. Longer transfers loop in the driver.
 */
#define GJ_VIRTIO_BLK_SOFT_SEGS 8u

/**
 * Request-queue soft product stats (fill via virtio_blk_q_stats).
 * Wire-stable field order for door/UDX snapshots; extend only at end.
 * Safe when not ready: implementation returns -1; do not assume zeros.
 *
 * Counters are lifetime since last successful probe (not reset by flush).
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

/**
 * Probe first virtio-blk in inventory; set up request q0 + DRIVER_OK.
 * Idempotent soft: re-probe after ready may no-op success. Returns 0 on
 * success, -1 if no device / setup / negotiate / queue fail.
 */
int  virtio_blk_probe(void);

/** Non-zero when DRIVER_OK and request queue are live. */
int  virtio_blk_ready(void);

/**
 * Capacity in 512-byte sectors from device config (0 if not ready).
 * Host may change capacity only with config_generation dance; product
 * soft path treats capacity as fixed after probe.
 */
u64  virtio_blk_capacity_sectors(void);

/**
 * Read contiguous bytes starting at u64Sector into pBuf.
 * cbLen must be a multiple of GJ_VIRTIO_BLK_SECTOR and non-zero.
 * Soft multi-segment: up to GJ_VIRTIO_BLK_SOFT_SEGS sectors per request
 * chain; longer xfers loop. Bounds: sector+len must not exceed capacity.
 * Returns 0 on success, -1 on not ready / bad args / I/O error / timeout.
 */
int  virtio_blk_read(u64 u64Sector, void *pBuf, u32 cbLen);

/**
 * Write contiguous bytes from pBuf starting at u64Sector.
 * Same alignment, multi-seg, and bounds rules as virtio_blk_read.
 * Returns 0 on success, -1 on failure.
 */
int  virtio_blk_write(u64 u64Sector, const void *pBuf, u32 cbLen);

/**
 * FLUSH / fsync-shaped barrier on the request queue (OASIS T_FLUSH).
 * Depth-1 serial soft path (one outstanding). Device S_UNSUP → soft
 * success (fsync-shaped) and increments flush_soft. Hard failure:
 * timeout / q_add fail → -1. Returns 0 on OK/soft, -1 hard.
 */
int  virtio_blk_flush(void);

/** Completed R/W (and successful device FLUSH) operations since probe. */
u32  virtio_blk_io_count(void);
/** Device FLUSH OK count since probe (not soft UNSUP). */
u32  virtio_blk_flush_count(void);
/** Soft FLUSH (UNSUP / soft barrier) count since probe. */
u32  virtio_blk_flush_soft_count(void);

/**
 * Snapshot request-queue soft product stats into *pOut.
 * Returns 0 on success, -1 if pOut NULL or not ready.
 */
int  virtio_blk_q_stats(struct gj_virtio_blk_q_stats *pOut);

/**
 * Export request queue rings for UDX / store_door MAP_RING.
 * Fills *pOut with PAs/size/notify; u32Ready set when live.
 * Returns 0 on success, -1 if not ready / pOut NULL.
 */
int  virtio_blk_export_q(struct gj_virtq_export *pOut);

/** Notify device of available buffers on the request queue. Returns 0 / -1. */
int  virtio_blk_kick_q(void);

/** Free descriptor count on the request queue (0 if not ready). */
u16  virtio_blk_q_free(void);

/**
 * Map request ring pages into the active address space at page-aligned
 * vaBase: desc | avail | used (one page each). Fills *pOut if non-NULL
 * with export snapshot + offsets. Returns 0 on success, -1 else.
 */
int  virtio_blk_map_q_user(u64 u64VaBase, struct gj_virtq_export *pOut);
