/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Storage door: stats + sector R/W for storaged hand-off (A1 product).
 * Clean-room pure C; dual MIT OR Apache-2.0 (no GPL source).
 *
 * Live path: CLAIM/RELEASE ownership, STATS/CAP/QUEUE_INFO, READ/WRITE
 * via virtio-blk (preferred) or scsi_door when blk is absent. Ring
 * EXPORT/MAP/KICK hand the virtq to storaged without changing sector
 * semantics.
 *
 * Ownership: token 0 means kernel interim owns policy; non-zero means
 * storaged claimed the door. Claim is re-entrant for the same token
 * (idempotent), BUSY for a different token. R/W is allowed without claim
 * for bring-up smokes (owned path preferred by product storaged).
 *
 * User pointers: prefer user_range_ok + copy_{to,from}_user. The !user
 * branch is for early kernel smokes that pass HHDM/static buffers.
 */
#include <gj/config.h>
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/scsi_mid.h>
#include <gj/store_door.h>
#include <gj/string.h>
#include <gj/user_access.h>
#include <gj/virtio_blk.h>

/* Prefer public GJ_STORE_XFER_MAX; local alias for bounce array size. */
#define STORE_XFER_MAX GJ_STORE_XFER_MAX

static int g_fInit;
static u32 g_u32Calls;
static u32 g_u32DoorRw;
static u32 g_u32OwnerToken; /* 0 = kernel interim owns */

/**
 * Copy @cb bytes to caller buffer at @u64Dst.
 * Uses copy_to_user when the range is in the user VA window.
 */
static i64
store_copy_out(u64 u64Dst, const void *pSrc, u32 cb)
{
    if (u64Dst == 0 || pSrc == NULL || cb == 0) {
        return GJ_ERR_INVAL;
    }
    if (user_range_ok(u64Dst, cb)) {
        if (copy_to_user(u64Dst, pSrc, cb) != GJ_OK) {
            return GJ_ERR_FAULT;
        }
    } else {
        /* Kernel-smoke path: destination is a trusted HHDM/static buffer. */
        memcpy((void *)(gj_vaddr_t)u64Dst, pSrc, cb);
    }
    return 0;
}

/**
 * Copy @cb bytes from caller buffer at @u64Src into @pDst.
 */
static i64
store_copy_in(void *pDst, u64 u64Src, u32 cb)
{
    if (pDst == NULL || u64Src == 0 || cb == 0) {
        return GJ_ERR_INVAL;
    }
    if (user_range_ok(u64Src, cb)) {
        if (copy_from_user(pDst, u64Src, cb) != GJ_OK) {
            return GJ_ERR_FAULT;
        }
    } else {
        memcpy(pDst, (const void *)(gj_vaddr_t)u64Src, cb);
    }
    return 0;
}

void
store_door_init(void)
{
    g_fInit = 1;
    g_u32Calls = 0;
    g_u32DoorRw = 0;
    g_u32OwnerToken = 0;
    /* Backends may probe later; report readiness snapshot for bring-up. */
    kprintf("store_door: init xfer_max=%u blk=%d scsi=%d\n", STORE_XFER_MAX,
            virtio_blk_ready() ? 1 : 0, scsi_mid_ready() ? 1 : 0);
}

int
store_door_owned(void)
{
    return g_u32OwnerToken != 0;
}

i64
store_door_call(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3)
{
    if (!g_fInit) {
        return GJ_ERR_NODEV;
    }
    g_u32Calls++;

    switch (u32Op) {
    case GJ_STORE_OP_CLAIM:
        /* arg1 = non-zero ownership token (low 32 bits only). */
        if (u64Arg1 == 0 || (u64Arg1 >> 32) != 0) {
            return GJ_ERR_INVAL;
        }
        if (g_u32OwnerToken != 0 && g_u32OwnerToken != (u32)u64Arg1) {
            return GJ_ERR_BUSY; /* another storaged */
        }
        g_u32OwnerToken = (u32)u64Arg1;
        kprintf("store_door: CLAIM token=0x%x (userspace owns storage)\n",
                g_u32OwnerToken);
        return 0;

    case GJ_STORE_OP_RELEASE:
        /* arg1 must match claim token when owned. */
        if (g_u32OwnerToken == 0) {
            return 0; /* already free */
        }
        if ((u64Arg1 >> 32) != 0 || (u32)u64Arg1 != g_u32OwnerToken) {
            return GJ_ERR_INVAL;
        }
        kprintf("store_door: RELEASE token=0x%x\n", g_u32OwnerToken);
        g_u32OwnerToken = 0;
        return 0;

    case GJ_STORE_OP_STATS: {
        /* aSt: [0]=virtio-blk io, [1]=scsi_door io, [2]=door call count */
        u32 aSt[3];
        i64 st;

        if (u64Arg1 == 0) {
            return GJ_ERR_INVAL;
        }
        aSt[0] = virtio_blk_ready() ? virtio_blk_io_count() : 0u;
        aSt[1] = scsi_door_io_count();
        aSt[2] = g_u32Calls;
        st = store_copy_out(u64Arg1, aSt, sizeof(aSt));
        return st;
    }

    case GJ_STORE_OP_CAP: {
        u64 u64Cap;
        i64 st;

        if (u64Arg1 == 0) {
            return GJ_ERR_INVAL;
        }
        if (virtio_blk_ready()) {
            u64Cap = virtio_blk_capacity_sectors();
        } else if (scsi_mid_ready()) {
            /* scsi → storaged CAP path when virtio-blk absent */
            struct scsi_door_req dreq;
            u8 aCap[8];
            u32 u32Last;

            memset(&dreq, 0, sizeof(dreq));
            dreq.u32Op = GJ_SCSI_DOOR_OP_READ_CAP;
            dreq.cbData = 8;
            if (scsi_door_submit(&dreq, aCap, 8) != 0) {
                return GJ_ERR_NODEV;
            }
            /* READ CAPACITY(10): big-endian last LBA in first 4 bytes. */
            u32Last = ((u32)aCap[0] << 24) | ((u32)aCap[1] << 16) |
                      ((u32)aCap[2] << 8) | (u32)aCap[3];
            /* Capacity = last LBA + 1 (sectors); widen before add. */
            u64Cap = (u64)u32Last + 1ull;
        } else {
            return GJ_ERR_NODEV;
        }
        st = store_copy_out(u64Arg1, &u64Cap, sizeof(u64Cap));
        return st;
    }

    case GJ_STORE_OP_READ:
    case GJ_STORE_OP_WRITE: {
        u64 u64Lba = u64Arg1;
        u32 cb = (u32)u64Arg3;
        static u8 aTmp[STORE_XFER_MAX];
        int nIo;
        int fBlk = virtio_blk_ready();
        int fScsi = !fBlk && scsi_mid_ready();
        i64 st;

        if (!fBlk && !fScsi) {
            return GJ_ERR_NODEV;
        }
        /* Reject wide arg3 truncation; require sector-multiple length. */
        if (u64Arg2 == 0 || cb == 0 || (u64Arg3 >> 32) != 0 ||
            cb > STORE_XFER_MAX || (cb % GJ_VIRTIO_BLK_SECTOR) != 0) {
            return GJ_ERR_INVAL;
        }
        /*
         * READ10/WRITE10 LBA field is 32-bit. Reject out-of-range LBAs on
         * the scsi fallback path (virtio-blk accepts full u64 sector).
         */
        if (fScsi && (u64Lba >> 32) != 0) {
            return GJ_ERR_INVAL;
        }

        if (u32Op == GJ_STORE_OP_WRITE) {
            st = store_copy_in(aTmp, u64Arg2, cb);
            if (st != 0) {
                return st;
            }
            if (fBlk) {
                nIo = virtio_blk_write(u64Lba, aTmp, cb);
            } else {
                struct scsi_door_req dreq;
                u32 u32Blocks = cb / GJ_VIRTIO_BLK_SECTOR;

                /* Defensive: blocks must fit u16 (xfer_max guarantees this). */
                if (u32Blocks == 0 || u32Blocks > 0xffffu) {
                    return GJ_ERR_INVAL;
                }
                memset(&dreq, 0, sizeof(dreq));
                dreq.u32Op = GJ_SCSI_DOOR_OP_WRITE10;
                dreq.u32Lba = (u32)u64Lba;
                dreq.u16Blocks = (u16)u32Blocks;
                dreq.cbData = cb;
                nIo = scsi_door_submit(&dreq, aTmp, cb);
            }
        } else {
            if (fBlk) {
                nIo = virtio_blk_read(u64Lba, aTmp, cb);
            } else {
                struct scsi_door_req dreq;
                u32 u32Blocks = cb / GJ_VIRTIO_BLK_SECTOR;

                if (u32Blocks == 0 || u32Blocks > 0xffffu) {
                    return GJ_ERR_INVAL;
                }
                memset(&dreq, 0, sizeof(dreq));
                dreq.u32Op = GJ_SCSI_DOOR_OP_READ10;
                dreq.u32Lba = (u32)u64Lba;
                dreq.u16Blocks = (u16)u32Blocks;
                dreq.cbData = cb;
                nIo = scsi_door_submit(&dreq, aTmp, cb);
            }
            if (nIo == 0) {
                st = store_copy_out(u64Arg2, aTmp, cb);
                if (st != 0) {
                    return st;
                }
            }
        }
        if (nIo != 0) {
            return GJ_ERR_IO;
        }
        g_u32DoorRw++;
        return (i64)cb;
    }

    case GJ_STORE_OP_QUEUE_INFO: {
        /* aQ: [0]=blk_io [1]=scsi_io [2]=door_rw [3]=owned(0/1) */
        u32 aQ[4];
        i64 st;

        if (u64Arg1 == 0) {
            return GJ_ERR_INVAL;
        }
        aQ[0] = virtio_blk_ready() ? virtio_blk_io_count() : 0u;
        aQ[1] = scsi_door_io_count();
        aQ[2] = g_u32DoorRw;
        aQ[3] = g_u32OwnerToken ? 1u : 0u;
        st = store_copy_out(u64Arg1, aQ, sizeof(aQ));
        return st;
    }

    case GJ_STORE_OP_FLUSH:
        /*
         * Virtio-blk flush not wired; fsync-shaped success when a block
         * transport is ready. scsi_mid has no SYNCHRONIZE CACHE yet.
         */
        if (!virtio_blk_ready() && !scsi_mid_ready()) {
            return GJ_ERR_NODEV;
        }
        return 0;

    case GJ_STORE_OP_EXPORT_RING: {
        struct gj_virtq_export ex;
        i64 st;

        if (u64Arg1 == 0) {
            return GJ_ERR_INVAL;
        }
        if (virtio_blk_export_q(&ex) != 0) {
            return GJ_ERR_NODEV;
        }
        st = store_copy_out(u64Arg1, &ex, sizeof(ex));
        return st;
    }

    case GJ_STORE_OP_KICK:
        if (virtio_blk_kick_q() != 0) {
            return GJ_ERR_NODEV;
        }
        return 0;

    case GJ_STORE_OP_RING_STATE: {
        u32 aS[2];
        i64 st;

        if (u64Arg1 == 0) {
            return GJ_ERR_INVAL;
        }
        aS[0] = virtio_blk_q_free();
        aS[1] = virtio_blk_ready() ? 1u : 0u;
        st = store_copy_out(u64Arg1, aS, sizeof(aS));
        return st;
    }

    case GJ_STORE_OP_MAP_RING: {
        struct gj_virtq_export ex;
        i64 st;

        if (u64Arg1 == 0) {
            return GJ_ERR_INVAL;
        }
        /* VA base must be page-aligned for ring map into user AS. */
        if ((u64Arg1 & (GJ_PAGE_SIZE - 1ull)) != 0) {
            return GJ_ERR_INVAL;
        }
        if (virtio_blk_map_q_user(u64Arg1, &ex) != 0) {
            return GJ_ERR_FAULT;
        }
        if (u64Arg2 != 0) {
            st = store_copy_out(u64Arg2, &ex, sizeof(ex));
            if (st != 0) {
                return st;
            }
        }
        return 0;
    }

    default:
        return GJ_ERR_NOSUPPORT;
    }
}
