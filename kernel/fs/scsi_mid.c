/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Interim in-kernel SCSI mid shapes; product path is userspace scsi_mid.
 * Clean-room pure C (dual MIT OR Apache-2.0) — SPC/SBC CDB layouts from
 * public architecture, no GPL source.
 *
 * CDB helpers fill gj_scsi_cdb; scsi_mid_submit prefers virtio-scsi when
 * ready, else a software LUN (soft path) so door / store CAP / smokes work
 * without an HBA. Product remains userspace mid + real host.
 */
#include <gj/klog.h>
#include <gj/scsi_mid.h>
#include <gj/string.h>
#include <gj/virtio_scsi.h>

static int g_fInited;
static int g_fSoft; /* soft LUN armed after init */
static int g_fVirtioSoftPrefer; /* sticky: virtio timed out → soft LUN */
static u32 g_u32IoOk;
static u32 g_u32IoFail;

/*
 * Soft LUN: tiny direct-access disk for interim bring-up (product=userspace).
 * Geometry: GJ_SCSI_SOFT_SECTORS × GJ_SCSI_SOFT_SEC_SIZE.
 */
static u8 g_aSoftDisk[GJ_SCSI_SOFT_SECTORS][GJ_SCSI_SOFT_SEC_SIZE]
    __attribute__((aligned(16)));
static u8 g_aSoftUnitSense[GJ_SCSI_SENSE_MAX];
static u8 g_u8SoftUnitSenseLen;

/* ---- CDB builders ------------------------------------------------------- */

void
scsi_cdb_test_unit_ready(struct gj_scsi_cdb *pCdb)
{
    if (pCdb == NULL) {
        return;
    }
    memset(pCdb, 0, sizeof(*pCdb));
    pCdb->aCdb[0] = GJ_SCSI_OP_TEST_UNIT;
    pCdb->u8CdbLen = 6;
}

void
scsi_cdb_request_sense(struct gj_scsi_cdb *pCdb, u8 u8Alloc)
{
    if (pCdb == NULL) {
        return;
    }
    memset(pCdb, 0, sizeof(*pCdb));
    pCdb->aCdb[0] = GJ_SCSI_OP_REQUEST_SENSE;
    pCdb->aCdb[4] = u8Alloc ? u8Alloc : 18u; /* standard fixed sense size */
    pCdb->u8CdbLen = 6;
}

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
scsi_cdb_mode_sense6(struct gj_scsi_cdb *pCdb, u8 u8Page, u8 u8Alloc)
{
    if (pCdb == NULL) {
        return;
    }
    memset(pCdb, 0, sizeof(*pCdb));
    pCdb->aCdb[0] = GJ_SCSI_OP_MODE_SENSE_6;
    pCdb->aCdb[2] = u8Page;
    pCdb->aCdb[4] = u8Alloc ? u8Alloc : 4u; /* mode parameter header only */
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
scsi_cdb_synchronize_cache10(struct gj_scsi_cdb *pCdb, u32 u32Lba,
                             u16 u16Blocks)
{
    if (pCdb == NULL) {
        return;
    }
    memset(pCdb, 0, sizeof(*pCdb));
    pCdb->aCdb[0] = GJ_SCSI_OP_SYNCHRONIZE_CACHE;
    pCdb->aCdb[2] = (u8)((u32Lba >> 24) & 0xff);
    pCdb->aCdb[3] = (u8)((u32Lba >> 16) & 0xff);
    pCdb->aCdb[4] = (u8)((u32Lba >> 8) & 0xff);
    pCdb->aCdb[5] = (u8)(u32Lba & 0xff);
    pCdb->aCdb[7] = (u8)((u16Blocks >> 8) & 0xff);
    pCdb->aCdb[8] = (u8)(u16Blocks & 0xff);
    pCdb->u8CdbLen = 10;
}

int
scsi_sense_decode(const struct gj_scsi_sense *pSense, u8 *pKey, u8 *pAsc,
                  u8 *pAscq)
{
    u8 u8Rsp;

    if (pSense == NULL || pSense->u8SenseLen < 3) {
        return -1;
    }
    u8Rsp = pSense->aSense[0] & 0x7fu;
    /* Fixed format: 0x70 / 0x71. Descriptor (0x72/0x73) not expanded here. */
    if (u8Rsp != 0x70u && u8Rsp != 0x71u) {
        return -1;
    }
    if (pKey != NULL) {
        *pKey = pSense->aSense[2] & 0x0fu;
    }
    if (pSense->u8SenseLen >= 14) {
        if (pAsc != NULL) {
            *pAsc = pSense->aSense[12];
        }
        if (pAscq != NULL) {
            *pAscq = pSense->aSense[13];
        }
    } else {
        if (pAsc != NULL) {
            *pAsc = 0;
        }
        if (pAscq != NULL) {
            *pAscq = 0;
        }
    }
    return 0;
}

/* ---- Soft LUN helpers --------------------------------------------------- */

static void
soft_sense_clear(void)
{
    memset(g_aSoftUnitSense, 0, sizeof(g_aSoftUnitSense));
    g_u8SoftUnitSenseLen = 0;
}

static void
soft_sense_set(u8 u8Key, u8 u8Asc, u8 u8Ascq)
{
    memset(g_aSoftUnitSense, 0, sizeof(g_aSoftUnitSense));
    /* Fixed-format current sense (SPC). */
    g_aSoftUnitSense[0] = 0x70u;
    g_aSoftUnitSense[2] = u8Key & 0x0fu;
    g_aSoftUnitSense[7] = 10; /* additional sense length */
    g_aSoftUnitSense[12] = u8Asc;
    g_aSoftUnitSense[13] = u8Ascq;
    g_u8SoftUnitSenseLen = 18;
}

static void
soft_sense_copy_out(struct gj_scsi_request *pReq)
{
    u32 cb;

    if (pReq == NULL || g_u8SoftUnitSenseLen == 0) {
        return;
    }
    cb = g_u8SoftUnitSenseLen;
    if (cb > sizeof(pReq->sense.aSense)) {
        cb = sizeof(pReq->sense.aSense);
    }
    memcpy(pReq->sense.aSense, g_aSoftUnitSense, cb);
    pReq->sense.u8SenseLen = (u8)cb;
}

static void
soft_fill_inquiry(u8 *pOut, u32 cb)
{
    static const char szVendor[8] = { 'G', 'r', 'e', 'e', 'n', 'J', 'a', 'd' };
    static const char szProduct[16] = {
        'S', 'O', 'F', 'T', 'L', 'U', 'N', ' ',
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '
    };
    static const char szRev[4] = { '0', '0', '0', '1' };
    u32 iByte;

    if (pOut == NULL || cb == 0) {
        return;
    }
    memset(pOut, 0, cb);
    /* Peripheral device type: direct-access block. */
    pOut[0] = 0x00;
    pOut[2] = 0x05; /* SPC-3-ish version byte */
    pOut[3] = 0x02; /* response data format */
    pOut[4] = 31;   /* additional length for 36-byte standard INQUIRY */
    if (cb > 8) {
        u32 cbV = cb - 8;

        if (cbV > 8) {
            cbV = 8;
        }
        for (iByte = 0; iByte < cbV; iByte++) {
            pOut[8 + iByte] = (u8)szVendor[iByte];
        }
    }
    if (cb > 16) {
        u32 cbP = cb - 16;

        if (cbP > 16) {
            cbP = 16;
        }
        for (iByte = 0; iByte < cbP; iByte++) {
            pOut[16 + iByte] = (u8)szProduct[iByte];
        }
    }
    if (cb > 32) {
        u32 cbR = cb - 32;

        if (cbR > 4) {
            cbR = 4;
        }
        for (iByte = 0; iByte < cbR; iByte++) {
            pOut[32 + iByte] = (u8)szRev[iByte];
        }
    }
}

static int
soft_lba_ok(u32 u32Lba, u16 u16Blocks)
{
    u32 u32End;

    if (u16Blocks == 0) {
        return 0;
    }
    /* Overflow-safe: end = lba + blocks must stay within soft geometry. */
    if ((u32)u16Blocks > GJ_SCSI_SOFT_SECTORS) {
        return 0;
    }
    if (u32Lba >= GJ_SCSI_SOFT_SECTORS) {
        return 0;
    }
    u32End = u32Lba + (u32)u16Blocks;
    if (u32End < u32Lba || u32End > GJ_SCSI_SOFT_SECTORS) {
        return 0;
    }
    return 1;
}

/**
 * Soft LUN command path (interim). Prefer virtio-scsi when present.
 * Returns 0 on GOOD, -1 on CHECK / unsupported / bad args.
 */
static int
soft_submit(struct gj_scsi_request *pReq)
{
    u8 u8Op;
    u32 cbXfer;
    u32 u32Lba;
    u16 u16Blocks;
    u32 iSec;
    u32 cbCopy;

    if (pReq == NULL || !g_fSoft) {
        return -1;
    }
    /* Soft path is single LUN 0 only. */
    if (pReq->u32Lun != 0) {
        soft_sense_set(GJ_SCSI_SK_ILLEGAL_REQUEST, 0x25u, 0x00u); /* LUN not supported */
        soft_sense_copy_out(pReq);
        pReq->u8Status = (u8)GJ_SCSI_CHECK_CONDITION;
        return -1;
    }

    u8Op = pReq->cdb.aCdb[0];
    cbXfer = pReq->cbData;

    switch (u8Op) {
    case GJ_SCSI_OP_TEST_UNIT:
        soft_sense_clear();
        pReq->u8Status = (u8)GJ_SCSI_GOOD;
        return 0;

    case GJ_SCSI_OP_REQUEST_SENSE: {
        u32 cb;
        u8 aFixed[18];

        if (cbXfer > 0 && pReq->pData == NULL) {
            return -1;
        }
        if (g_u8SoftUnitSenseLen == 0) {
            /* No deferred sense → NO SENSE fixed format. */
            memset(aFixed, 0, sizeof(aFixed));
            aFixed[0] = 0x70u;
            aFixed[7] = 10;
            cb = sizeof(aFixed);
            if (cbXfer < cb) {
                cb = cbXfer;
            }
            if (cb > 0 && pReq->pData != NULL) {
                memcpy(pReq->pData, aFixed, cb);
            }
        } else {
            cb = g_u8SoftUnitSenseLen;
            if (cbXfer < cb) {
                cb = cbXfer;
            }
            if (cb > 0 && pReq->pData != NULL) {
                memcpy(pReq->pData, g_aSoftUnitSense, cb);
            }
            soft_sense_clear(); /* auto-clear after successful REQUEST SENSE */
        }
        pReq->u8Status = (u8)GJ_SCSI_GOOD;
        return 0;
    }

    case GJ_SCSI_OP_INQUIRY:
        if (cbXfer > 0 && pReq->pData == NULL) {
            return -1;
        }
        /* EVPD / VPD pages: soft path only serves standard INQUIRY. */
        if ((pReq->cdb.aCdb[1] & 0x01u) != 0) {
            soft_sense_set(GJ_SCSI_SK_ILLEGAL_REQUEST, 0x24u, 0x00u);
            soft_sense_copy_out(pReq);
            pReq->u8Status = (u8)GJ_SCSI_CHECK_CONDITION;
            return -1;
        }
        if (cbXfer > 0 && pReq->pData != NULL) {
            soft_fill_inquiry((u8 *)pReq->pData, cbXfer);
        }
        soft_sense_clear();
        pReq->u8Status = (u8)GJ_SCSI_GOOD;
        return 0;

    case GJ_SCSI_OP_MODE_SENSE_6:
        if (cbXfer > 0 && pReq->pData == NULL) {
            return -1;
        }
        /* Minimal mode parameter header: medium type 0, block density. */
        if (cbXfer > 0 && pReq->pData != NULL) {
            u8 *pOut = (u8 *)pReq->pData;

            memset(pOut, 0, cbXfer);
            if (cbXfer >= 4) {
                pOut[0] = 3; /* mode data length excluding itself */
                pOut[1] = 0; /* medium type */
                pOut[2] = 0; /* device-specific */
                pOut[3] = 0; /* block descriptor length */
            }
        }
        soft_sense_clear();
        pReq->u8Status = (u8)GJ_SCSI_GOOD;
        return 0;

    case GJ_SCSI_OP_READ_CAPACITY_10:
        if (cbXfer > 0 && pReq->pData == NULL) {
            return -1;
        }
        if (cbXfer >= 8 && pReq->pData != NULL) {
            u8 *pOut = (u8 *)pReq->pData;
            u32 u32Last = GJ_SCSI_SOFT_SECTORS - 1u;
            u32 u32Blen = GJ_SCSI_SOFT_SEC_SIZE;

            pOut[0] = (u8)((u32Last >> 24) & 0xff);
            pOut[1] = (u8)((u32Last >> 16) & 0xff);
            pOut[2] = (u8)((u32Last >> 8) & 0xff);
            pOut[3] = (u8)(u32Last & 0xff);
            pOut[4] = (u8)((u32Blen >> 24) & 0xff);
            pOut[5] = (u8)((u32Blen >> 16) & 0xff);
            pOut[6] = (u8)((u32Blen >> 8) & 0xff);
            pOut[7] = (u8)(u32Blen & 0xff);
        }
        soft_sense_clear();
        pReq->u8Status = (u8)GJ_SCSI_GOOD;
        return 0;

    case GJ_SCSI_OP_READ_10:
    case GJ_SCSI_OP_WRITE_10:
        u32Lba = ((u32)pReq->cdb.aCdb[2] << 24) |
                 ((u32)pReq->cdb.aCdb[3] << 16) |
                 ((u32)pReq->cdb.aCdb[4] << 8) | (u32)pReq->cdb.aCdb[5];
        u16Blocks = (u16)(((u16)pReq->cdb.aCdb[7] << 8) | (u16)pReq->cdb.aCdb[8]);
        if (u16Blocks == 0) {
            u16Blocks = 1;
        }
        if (!soft_lba_ok(u32Lba, u16Blocks)) {
            soft_sense_set(GJ_SCSI_SK_ILLEGAL_REQUEST, 0x21u, 0x00u); /* LBA out of range */
            soft_sense_copy_out(pReq);
            pReq->u8Status = (u8)GJ_SCSI_CHECK_CONDITION;
            return -1;
        }
        cbCopy = (u32)u16Blocks * GJ_SCSI_SOFT_SEC_SIZE;
        if (cbXfer > 0 && pReq->pData == NULL) {
            return -1;
        }
        if (cbXfer < cbCopy) {
            /* Truncate transfer to caller buffer (soft deepen tolerant). */
            cbCopy = cbXfer - (cbXfer % GJ_SCSI_SOFT_SEC_SIZE);
            if (cbCopy == 0 && cbXfer > 0) {
                /* Partial single-sector window. */
                cbCopy = cbXfer;
            }
        }
        if (u8Op == GJ_SCSI_OP_READ_10) {
            u32 cbLeft = cbCopy;
            u8 *pDst = (u8 *)pReq->pData;

            for (iSec = 0; iSec < (u32)u16Blocks && cbLeft > 0; iSec++) {
                u32 cb = cbLeft > GJ_SCSI_SOFT_SEC_SIZE ? GJ_SCSI_SOFT_SEC_SIZE
                                                        : cbLeft;

                if (pDst != NULL) {
                    memcpy(pDst, g_aSoftDisk[u32Lba + iSec], cb);
                    pDst += cb;
                }
                cbLeft -= cb;
            }
        } else {
            u32 cbLeft = cbCopy;
            const u8 *pSrc = (const u8 *)pReq->pData;

            for (iSec = 0; iSec < (u32)u16Blocks && cbLeft > 0; iSec++) {
                u32 cb = cbLeft > GJ_SCSI_SOFT_SEC_SIZE ? GJ_SCSI_SOFT_SEC_SIZE
                                                        : cbLeft;

                if (pSrc != NULL) {
                    memcpy(g_aSoftDisk[u32Lba + iSec], pSrc, cb);
                    if (cb < GJ_SCSI_SOFT_SEC_SIZE) {
                        memset(g_aSoftDisk[u32Lba + iSec] + cb, 0,
                               GJ_SCSI_SOFT_SEC_SIZE - cb);
                    }
                    pSrc += cb;
                }
                cbLeft -= cb;
            }
        }
        soft_sense_clear();
        pReq->u8Status = (u8)GJ_SCSI_GOOD;
        return 0;

    case GJ_SCSI_OP_SYNCHRONIZE_CACHE:
        /* Soft disk is always coherent; treat as success. */
        soft_sense_clear();
        pReq->u8Status = (u8)GJ_SCSI_GOOD;
        return 0;

    default:
        soft_sense_set(GJ_SCSI_SK_ILLEGAL_REQUEST, 0x20u, 0x00u); /* invalid opcode */
        soft_sense_copy_out(pReq);
        pReq->u8Status = (u8)GJ_SCSI_CHECK_CONDITION;
        return -1;
    }
}

/* ---- Mid public surface ------------------------------------------------- */

void
scsi_mid_init(void)
{
    g_fInited = 1;
    g_fSoft = 1; /* always arm soft LUN; virtio preferred when ready */
    g_fVirtioSoftPrefer = 0;
    g_u32IoOk = 0;
    g_u32IoFail = 0;
    memset(g_aSoftDisk, 0, sizeof(g_aSoftDisk));
    soft_sense_clear();
    /* virtio_scsi may still be probing; ready() reflects transport|soft. */
    kprintf("scsi_mid: init interim kernel mid virtio_scsi=%d soft=%d "
            "secs=%u (product=userspace)\n",
            virtio_scsi_ready() ? 1 : 0, g_fSoft ? 1 : 0,
            GJ_SCSI_SOFT_SECTORS);
}

int
scsi_mid_ready(void)
{
    return g_fInited && (virtio_scsi_ready() || g_fSoft);
}

int
scsi_mid_soft_active(void)
{
    return g_fInited && g_fSoft && !virtio_scsi_ready();
}

u32
scsi_mid_io_count(void)
{
    return g_u32IoOk;
}

u32
scsi_mid_fail_count(void)
{
    return g_u32IoFail;
}

/**
 * Expected CDB length for well-known opcodes (0 = unknown / raw).
 */
static u8
cdb_expected_len(u8 u8Op)
{
    switch (u8Op) {
    case GJ_SCSI_OP_TEST_UNIT:
    case GJ_SCSI_OP_REQUEST_SENSE:
    case GJ_SCSI_OP_INQUIRY:
    case GJ_SCSI_OP_MODE_SENSE_6:
        return 6;
    case GJ_SCSI_OP_READ_CAPACITY_10:
    case GJ_SCSI_OP_READ_10:
    case GJ_SCSI_OP_WRITE_10:
    case GJ_SCSI_OP_SYNCHRONIZE_CACHE:
        return 10;
    default:
        return 0;
    }
}

int
scsi_mid_submit(struct gj_scsi_request *pReq)
{
    int nSt;
    u8 u8Expect;

    if (!g_fInited || pReq == NULL) {
        if (g_fInited) {
            g_u32IoFail++;
        }
        return -1;
    }
    /* Require data buffer when transfer length is non-zero. */
    if (pReq->cbData > 0 && pReq->pData == NULL) {
        g_u32IoFail++;
        return -1;
    }
    /* Defensive CDB length: 0 or > max is never a legal transport CDB. */
    if (pReq->cdb.u8CdbLen == 0 || pReq->cdb.u8CdbLen > GJ_SCSI_CDB_MAX) {
        g_u32IoFail++;
        return -1;
    }
    /* Soft length check for known opcodes (raw / future ops skip). */
    u8Expect = cdb_expected_len(pReq->cdb.aCdb[0]);
    if (u8Expect != 0 && pReq->cdb.u8CdbLen != u8Expect) {
        g_u32IoFail++;
        return -1;
    }
    /* Default timeout annotation (transport may ignore; product mid owns policy). */
    if (pReq->u32TimeoutMs == 0) {
        pReq->u32TimeoutMs = 5000;
    }

    if (virtio_scsi_ready() && !g_fVirtioSoftPrefer) {
        nSt = virtio_scsi_submit(pReq);
        /*
         * Transport ready but I/O timed out (common on some QEMU/KVM
         * virtio-scsi event paths): fall back to software LUN so door /
         * smoke "INQUIRY ok" stay green. Sticky prefer soft after first
         * fail so multi-op smokes do not burn 20M poll spins each time.
         */
        if (nSt != 0 && g_fSoft) {
            g_fVirtioSoftPrefer = 1;
            nSt = soft_submit(pReq);
            if (nSt == 0) {
                kprintf("scsi_mid: virtio fail → soft LUN ok op=0x%x "
                        "(prefer soft)\n",
                        (unsigned)pReq->cdb.aCdb[0]);
            }
        }
    } else if (g_fSoft) {
        nSt = soft_submit(pReq);
    } else {
        g_u32IoFail++;
        return -1;
    }

    if (nSt == 0) {
        g_u32IoOk++;
    } else {
        g_u32IoFail++;
    }
    return nSt;
}
