/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Interim in-kernel SCSI mid shapes; product path is userspace scsi_mid.
 * Clean-room pure C (dual MIT OR Apache-2.0) — SPC/SBC CDB layouts from
 * public architecture, no GPL source.
 *
 * CDB helpers fill gj_scsi_cdb; scsi_mid_submit forwards to virtio-scsi
 * when ready (scsi_door / store_door CAP fallback / vfs_ram sda path).
 */
#include <gj/klog.h>
#include <gj/scsi_mid.h>
#include <gj/string.h>
#include <gj/virtio_scsi.h>

static int g_fInited;

void
scsi_cdb_inquiry(struct gj_scsi_cdb *pCdb, u8 u8Evpd, u8 u8Page, u16 u16Alloc)
{
    if (pCdb == NULL) {
        return; /* defensive: callers always pass stack CDBs */
    }
    memset(pCdb, 0, sizeof(*pCdb));
    pCdb->aCdb[0] = GJ_SCSI_OP_INQUIRY;
    pCdb->aCdb[1] = u8Evpd ? 1u : 0;
    pCdb->aCdb[2] = u8Page;
    pCdb->aCdb[3] = (u8)((u16Alloc >> 8) & 0xff);
    pCdb->aCdb[4] = (u8)(u16Alloc & 0xff);
    pCdb->u8CdbLen = 6;
}

void
scsi_cdb_read_capacity10(struct gj_scsi_cdb *pCdb)
{
    if (pCdb == NULL) {
        return;
    }
    memset(pCdb, 0, sizeof(*pCdb));
    pCdb->aCdb[0] = GJ_SCSI_OP_READ_CAPACITY_10;
    pCdb->u8CdbLen = 10;
}

void
scsi_cdb_read10(struct gj_scsi_cdb *pCdb, u32 u32Lba, u16 u16Blocks)
{
    if (pCdb == NULL) {
        return;
    }
    /* Zero blocks is valid CDB shape; transport / door may default to 1. */
    memset(pCdb, 0, sizeof(*pCdb));
    pCdb->aCdb[0] = GJ_SCSI_OP_READ_10;
    pCdb->aCdb[2] = (u8)((u32Lba >> 24) & 0xff);
    pCdb->aCdb[3] = (u8)((u32Lba >> 16) & 0xff);
    pCdb->aCdb[4] = (u8)((u32Lba >> 8) & 0xff);
    pCdb->aCdb[5] = (u8)(u32Lba & 0xff);
    pCdb->aCdb[7] = (u8)((u16Blocks >> 8) & 0xff);
    pCdb->aCdb[8] = (u8)(u16Blocks & 0xff);
    pCdb->u8CdbLen = 10;
}

void
scsi_cdb_write10(struct gj_scsi_cdb *pCdb, u32 u32Lba, u16 u16Blocks)
{
    if (pCdb == NULL) {
        return;
    }
    scsi_cdb_read10(pCdb, u32Lba, u16Blocks);
    pCdb->aCdb[0] = GJ_SCSI_OP_WRITE_10;
}

void
scsi_mid_init(void)
{
    g_fInited = 1;
    /* virtio_scsi may still be probing; ready() reflects transport state. */
    kprintf("scsi_mid: init interim kernel mid virtio_scsi=%d "
            "(product=userspace)\n",
            virtio_scsi_ready() ? 1 : 0);
}

int
scsi_mid_ready(void)
{
    return g_fInited && virtio_scsi_ready();
}

int
scsi_mid_submit(struct gj_scsi_request *pReq)
{
    if (!g_fInited || pReq == NULL) {
        return -1;
    }
    /* Require data buffer when transfer length is non-zero. */
    if (pReq->cbData > 0 && pReq->pData == NULL) {
        return -1;
    }
    /* Defensive CDB length: 0 or > max is never a legal transport CDB. */
    if (pReq->cdb.u8CdbLen == 0 || pReq->cdb.u8CdbLen > GJ_SCSI_CDB_MAX) {
        return -1;
    }
    if (!virtio_scsi_ready()) {
        return -1;
    }
    return virtio_scsi_submit(pReq);
}
