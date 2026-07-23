/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Door-shaped submit path for userspace scsi_mid (M5 product).
 * Clean-room pure C; dual MIT OR Apache-2.0. Interim: kernel virtio-scsi
 * or soft LUN behind a C API; full door ENDPOINT is the product next step.
 *
 * Ops (pReq->u32Op / GJ_SCSI_DOOR_OP_*):
 *   0 INQUIRY  1 READ_CAPACITY10  2 READ10  3 WRITE10
 *   4 raw CDB (u8CdbLen + aCdb + fDataIn)
 *   5 TEST UNIT READY  6 SYNCHRONIZE CACHE(10)  7 REQUEST SENSE
 *
 * Used by GJ_SYS_SCSI and by store_door CAP/R/W when virtio-blk is absent.
 * Product path remains userspace scsi_mid; this is the kernel mid shim.
 */
#include <gj/klog.h>
#include <gj/scsi_mid.h>
#include <gj/string.h>
#include <gj/types.h>

static u32 g_u32DoorIos;
static u32 g_u32DoorFails;

static void
door_fail(void)
{
    g_u32DoorFails++;
}

/**
 * Soft validation for block-shaped transfers:
 * - default blocks to 1 when zero
 * - when cbData is non-zero, require at least one sector of room
 * - cap blocks so blocks * 512 does not overflow u32 and fits cbData
 */
static int
door_blocks_ok(u16 *pBlocks, u32 cbData)
{
    u32 u32Blocks;
    u32 u32Need;

    if (pBlocks == NULL) {
        return -1;
    }
    u32Blocks = *pBlocks ? (u32)*pBlocks : 1u;
    if (u32Blocks == 0 || u32Blocks > 0xffffu) {
        return -1;
    }
    /* Overflow guard: blocks * 512 must fit u32. */
    if (u32Blocks > (0xffffffffu / GJ_SCSI_SOFT_SEC_SIZE)) {
        return -1;
    }
    u32Need = u32Blocks * GJ_SCSI_SOFT_SEC_SIZE;
    if (cbData > 0 && cbData < GJ_SCSI_SOFT_SEC_SIZE) {
        /* Allow short soft buffers only for single-block probes. */
        if (u32Blocks != 1u) {
            return -1;
        }
    } else if (cbData > 0 && cbData < u32Need) {
        /*
         * Truncate block count to what the buffer can hold (sector-aligned).
         * Callers that pass exact sizes (store_door) keep full transfer.
         */
        u32Blocks = cbData / GJ_SCSI_SOFT_SEC_SIZE;
        if (u32Blocks == 0) {
            return -1;
        }
    }
    *pBlocks = (u16)u32Blocks;
    return 0;
}

int
scsi_door_submit(struct scsi_door_req *pReq, void *pData, u32 cbData)
{
    struct gj_scsi_request req;
    u16 u16Blocks;

    if (pReq == NULL) {
        door_fail();
        return -1;
    }
    /* Data-bearing ops need a buffer when cbData > 0. */
    if (cbData > 0 && pData == NULL) {
        door_fail();
        return -1;
    }
    if (!scsi_mid_ready()) {
        door_fail();
        return -1;
    }

    memset(&req, 0, sizeof(req));
    switch (pReq->u32Op) {
    case GJ_SCSI_DOOR_OP_INQUIRY:
        /* Default allocation length matches standard INQUIRY page size. */
        scsi_cdb_inquiry(&req.cdb, 0, 0, (u16)(cbData ? cbData : 36u));
        req.fDataIn = 1;
        if (cbData == 0) {
            door_fail();
            return -1;
        }
        break;

    case GJ_SCSI_DOOR_OP_READ_CAP:
        scsi_cdb_read_capacity10(&req.cdb);
        req.fDataIn = 1;
        /* Capacity payload is 8 bytes; reject empty windows. */
        if (cbData < 8) {
            door_fail();
            return -1;
        }
        break;

    case GJ_SCSI_DOOR_OP_READ10:
        u16Blocks = pReq->u16Blocks ? pReq->u16Blocks : 1u;
        if (door_blocks_ok(&u16Blocks, cbData) != 0) {
            door_fail();
            return -1;
        }
        scsi_cdb_read10(&req.cdb, pReq->u32Lba, u16Blocks);
        req.fDataIn = 1;
        break;

    case GJ_SCSI_DOOR_OP_WRITE10:
        u16Blocks = pReq->u16Blocks ? pReq->u16Blocks : 1u;
        if (door_blocks_ok(&u16Blocks, cbData) != 0) {
            door_fail();
            return -1;
        }
        scsi_cdb_write10(&req.cdb, pReq->u32Lba, u16Blocks);
        req.fDataIn = 0;
        break;

    case GJ_SCSI_DOOR_OP_RAW:
        /* Raw path: caller owns CDB bytes; clamp length defensively. */
        if (pReq->u8CdbLen == 0 || pReq->u8CdbLen > GJ_SCSI_CDB_MAX) {
            door_fail();
            return -1;
        }
        memcpy(req.cdb.aCdb, pReq->aCdb, pReq->u8CdbLen);
        req.cdb.u8CdbLen = pReq->u8CdbLen;
        req.fDataIn = pReq->fDataIn ? 1 : 0;
        break;

    case GJ_SCSI_DOOR_OP_TEST_UNIT:
        scsi_cdb_test_unit_ready(&req.cdb);
        req.fDataIn = 0;
        break;

    case GJ_SCSI_DOOR_OP_SYNC_CACHE:
        u16Blocks = pReq->u16Blocks; /* 0 = whole medium per SBC soft path */
        scsi_cdb_synchronize_cache10(&req.cdb, pReq->u32Lba, u16Blocks);
        req.fDataIn = 0;
        break;

    case GJ_SCSI_DOOR_OP_REQ_SENSE:
        scsi_cdb_request_sense(&req.cdb, (u8)(cbData > 255u ? 255u : cbData));
        req.fDataIn = 1;
        if (cbData == 0) {
            door_fail();
            return -1;
        }
        break;

    default:
        door_fail();
        return -1;
    }

    req.pData = pData;
    req.cbData = cbData;
    /* Propagate optional LUN from door pad space via u32Lba only for block ops;
     * LUN stays 0 for interim single-target soft/virtio path. */
    req.u32Lun = 0;
    req.u32TimeoutMs = 5000;

    if (scsi_mid_submit(&req) != 0) {
        door_fail();
        return -1;
    }
    g_u32DoorIos++;
    return 0;
}

u32
scsi_door_io_count(void)
{
    return g_u32DoorIos;
}

u32
scsi_door_fail_count(void)
{
    return g_u32DoorFails;
}

int
scsi_door_stats(struct scsi_door_stats *pOut)
{
    if (pOut == NULL) {
        return -1;
    }
    pOut->u32Ios = g_u32DoorIos;
    pOut->u32Fails = g_u32DoorFails;
    pOut->u32Ready = scsi_mid_ready() ? 1u : 0u;
    pOut->u32Soft = scsi_mid_soft_active() ? 1u : 0u;
    return 0;
}

void
scsi_door_init(void)
{
    g_u32DoorIos = 0;
    g_u32DoorFails = 0;
    kprintf("scsi_door: init ios=0 fails=0 mid_ready=%d soft=%d "
            "(kernel mid shim; product=userspace)\n",
            scsi_mid_ready() ? 1 : 0, scsi_mid_soft_active() ? 1 : 0);
}
