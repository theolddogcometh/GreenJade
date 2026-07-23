/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Door-shaped submit path for userspace scsi_mid (M5 product).
 * Clean-room pure C; dual MIT OR Apache-2.0. Interim: kernel virtio-scsi
 * behind a C API; full door ENDPOINT is the product next step.
 *
 * Ops (pReq->u32Op / GJ_SCSI_DOOR_OP_*):
 *   0 INQUIRY  1 READ_CAPACITY10  2 READ10  3 WRITE10
 *   4 raw CDB (u8CdbLen + aCdb + fDataIn)
 *
 * Used by GJ_SYS_SCSI and by store_door CAP/R/W when virtio-blk is absent.
 */
#include <gj/klog.h>
#include <gj/scsi_mid.h>
#include <gj/string.h>
#include <gj/types.h>

static u32 g_u32DoorIos;

int
scsi_door_submit(struct scsi_door_req *pReq, void *pData, u32 cbData)
{
    struct gj_scsi_request req;
    u16 u16Blocks;

    if (pReq == NULL) {
        return -1;
    }
    /* Data-bearing ops need a buffer when cbData > 0. */
    if (cbData > 0 && pData == NULL) {
        return -1;
    }

    memset(&req, 0, sizeof(req));
    switch (pReq->u32Op) {
    case GJ_SCSI_DOOR_OP_INQUIRY:
        /* Default allocation length matches standard INQUIRY page size. */
        scsi_cdb_inquiry(&req.cdb, 0, 0, (u16)(cbData ? cbData : 36u));
        req.fDataIn = 1;
        break;
    case GJ_SCSI_DOOR_OP_READ_CAP:
        scsi_cdb_read_capacity10(&req.cdb);
        req.fDataIn = 1;
        break;
    case GJ_SCSI_DOOR_OP_READ10:
        u16Blocks = pReq->u16Blocks ? pReq->u16Blocks : 1u;
        scsi_cdb_read10(&req.cdb, pReq->u32Lba, u16Blocks);
        req.fDataIn = 1;
        break;
    case GJ_SCSI_DOOR_OP_WRITE10:
        u16Blocks = pReq->u16Blocks ? pReq->u16Blocks : 1u;
        scsi_cdb_write10(&req.cdb, pReq->u32Lba, u16Blocks);
        req.fDataIn = 0;
        break;
    case GJ_SCSI_DOOR_OP_RAW:
        /* Raw path: caller owns CDB bytes; clamp length defensively. */
        if (pReq->u8CdbLen == 0 || pReq->u8CdbLen > GJ_SCSI_CDB_MAX) {
            return -1;
        }
        memcpy(req.cdb.aCdb, pReq->aCdb, pReq->u8CdbLen);
        req.cdb.u8CdbLen = pReq->u8CdbLen;
        req.fDataIn = pReq->fDataIn ? 1 : 0;
        break;
    default:
        return -1;
    }

    req.pData = pData;
    req.cbData = cbData;
    if (scsi_mid_submit(&req) != 0) {
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

void
scsi_door_init(void)
{
    g_u32DoorIos = 0;
    kprintf("scsi_door: init ios=0 (kernel mid shim; product=userspace)\n");
}
