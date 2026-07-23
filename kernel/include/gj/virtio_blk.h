/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-blk (modern PCI) — pure C, dual license, no GPL.
 * Product storage path: request virtqueue (q0) sector R/W for store_door /
 * storaged / vfsd. OASIS virtio-blk; no Linux virtio source.
 */
#pragma once

#include <gj/types.h>
#include <gj/virtio.h>

/** Logical block size (OASIS virtio-blk sector). */
#define GJ_VIRTIO_BLK_SECTOR 512u

/** Request virtqueue index (only queue used by this driver). */
#define GJ_VIRTIO_BLK_Q_REQUEST 0u

/** Probe first virtio-blk; set up request q0. Returns 0 on success. */
int  virtio_blk_probe(void);
/** Non-zero when DRIVER_OK and request queue are live. */
int  virtio_blk_ready(void);

/** Capacity in 512-byte sectors (0 if not ready). */
u64  virtio_blk_capacity_sectors(void);

/**
 * Read/write contiguous bytes starting at sector (length must be a multiple
 * of GJ_VIRTIO_BLK_SECTOR). One outstanding request; multi-sector loops
 * sector-by-sector. Returns 0 on success, -1 on error.
 */
int  virtio_blk_read(u64 u64Sector, void *pBuf, u32 cbLen);
int  virtio_blk_write(u64 u64Sector, const void *pBuf, u32 cbLen);

/** Completed R/W (and future FLUSH) operations since probe. */
u32  virtio_blk_io_count(void);

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
