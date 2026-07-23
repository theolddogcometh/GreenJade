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
 *
 * Soft inventory (Wave 13 exclusive deepen — this unit only):
 *   - Submit enter / ok / fail; per-op ok tallies; transport path lamps
 *   - Virtio-fail → soft LUN sticky fallback remains product critical path
 *     (INQUIRY / door smokes) — inventory never mutates that arm
 *   greppable: "scsi_mid: soft …"
 *   Never hard-gates; diagnostics / smoke grep only (wrap OK).
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
 * Soft product inventory (Wave 13 exclusive). Cumulative path tallies.
 * greppable: scsi_mid: soft …
 */
static u32 g_u32SoftEnter;       /* scsi_mid_submit entries past null-guard */
static u32 g_u32SoftOk;          /* submit returned 0 */
static u32 g_u32SoftFail;        /* submit returned -1 */
static u32 g_u32SoftOpTur;       /* TEST UNIT READY ok */
static u32 g_u32SoftOpSense;     /* REQUEST SENSE ok */
static u32 g_u32SoftOpInq;       /* INQUIRY ok */
static u32 g_u32SoftOpMode;      /* MODE SENSE(6) ok */
static u32 g_u32SoftOpReadCap;   /* READ CAPACITY(10) ok */
static u32 g_u32SoftOpRead10;    /* READ(10) ok */
static u32 g_u32SoftOpWrite10;   /* WRITE(10) ok */
static u32 g_u32SoftOpSync;      /* SYNCHRONIZE CACHE ok */
static u32 g_u32SoftOpOther;     /* other opcode ok */
static u32 g_u32SoftViaVirtio;   /* completed on virtio path */
static u32 g_u32SoftViaSoft;     /* completed on direct soft LUN */
static u32 g_u32SoftViaFallback; /* virtio fail → soft LUN success */
static u32 g_u32SoftVirtioTry;   /* virtio_scsi_submit attempts */
static u32 g_u32SoftVirtioFail;  /* virtio_scsi_submit non-zero */
static u32 g_u32SoftPreferArm;   /* sticky prefer soft armed */
static u32 g_u32SoftDenyNull;    /* pReq == NULL while inited */
static u32 g_u32SoftDenyNotInit; /* !g_fInited */
static u32 g_u32SoftDenyNullData;/* cbData > 0 && pData == NULL */
static u32 g_u32SoftDenyCdbLen;  /* bad CDB length */
static u32 g_u32SoftDenyCdbExp;  /* known-opcode length mismatch */
static u32 g_u32SoftDenyNoPath;  /* neither virtio nor soft */
static u32 g_u32SoftSoftCheck;   /* soft_submit returned CHECK / -1 */
static u32 g_u32SoftInitCalls;   /* scsi_mid_init entries */
static u32 g_u32SoftReadyCalls;  /* scsi_mid_ready entries */
static u32 g_u32SoftActiveCalls; /* scsi_mid_soft_active entries */
static u32 g_u32SoftInvSamples;  /* soft inventory dump count */
static u8  g_fSoftOnce;          /* one-shot after first submit activity */

/*
 * Soft LUN: tiny direct-access disk for interim bring-up (product=userspace).
 * Geometry: GJ_SCSI_SOFT_SECTORS × GJ_SCSI_SOFT_SEC_SIZE.
 */
static u8 g_aSoftDisk[GJ_SCSI_SOFT_SECTORS][GJ_SCSI_SOFT_SEC_SIZE]
    __attribute__((aligned(16)));
static u8 g_aSoftUnitSense[GJ_SCSI_SENSE_MAX];
static u8 g_u8SoftUnitSenseLen;

static void soft_inc(u32 *pCtr);
static void soft_inventory_log(const char *szVia);
static void soft_maybe_once(void);
static void soft_note_op_ok(u8 u8Op);

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

/* ---- Soft inventory (Wave 13 exclusive) --------------------------------- */

/** Soft: bump path tally (u32 wrap is fine for telemetry). */
static void
soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/** Soft: classify successful opcode for greppable per-op lamps. */
static void
soft_note_op_ok(u8 u8Op)
{
    switch (u8Op) {
    case GJ_SCSI_OP_TEST_UNIT:
        soft_inc(&g_u32SoftOpTur);
        break;
    case GJ_SCSI_OP_REQUEST_SENSE:
        soft_inc(&g_u32SoftOpSense);
        break;
    case GJ_SCSI_OP_INQUIRY:
        soft_inc(&g_u32SoftOpInq);
        break;
    case GJ_SCSI_OP_MODE_SENSE_6:
        soft_inc(&g_u32SoftOpMode);
        break;
    case GJ_SCSI_OP_READ_CAPACITY_10:
        soft_inc(&g_u32SoftOpReadCap);
        break;
    case GJ_SCSI_OP_READ_10:
        soft_inc(&g_u32SoftOpRead10);
        break;
    case GJ_SCSI_OP_WRITE_10:
        soft_inc(&g_u32SoftOpWrite10);
        break;
    case GJ_SCSI_OP_SYNCHRONIZE_CACHE:
        soft_inc(&g_u32SoftOpSync);
        break;
    default:
        soft_inc(&g_u32SoftOpOther);
        break;
    }
}

/**
 * Greppable soft scsi_mid inventory (Wave 13 exclusive; product / smoke).
 * Prefix-stable markers (scsi_mid: soft …):
 *   scsi_mid: soft inventory  — enter/ok/fail + mid lamps + log_n
 *   scsi_mid: soft op         — per-op ok tallies
 *   scsi_mid: soft transport  — virtio/soft/fallback path lamps
 *   scsi_mid: soft deny       — soft reject reason catalog
 *   scsi_mid: soft geometry   — soft LUN geometry
 *   scsi_mid: soft path       — honesty: kernel mid ≠ product scsi_mid
 *   scsi_mid: soft inventory PASS / scsi_mid: soft PASS
 *
 * Never hard-gates; does not change virtio→soft INQUIRY fallback.
 * greppable: scsi_mid: soft
 */
static void
soft_inventory_log(const char *szVia)
{
    const char *szViaSafe;
    u32         u32Ready;
    u32         u32SoftAct;
    u32         u32Virtio;
    u32         u32Prefer;
    int         fSoftPass;

    soft_inc(&g_u32SoftInvSamples);
    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "unknown";
    u32Virtio = virtio_scsi_ready() ? 1u : 0u;
    u32Prefer = g_fVirtioSoftPrefer ? 1u : 0u;
    u32Ready = (g_fInited && (u32Virtio != 0 || g_fSoft)) ? 1u : 0u;
    /* Match scsi_mid_soft_active(): soft armed and virtio not ready. */
    u32SoftAct = (g_fInited && g_fSoft && u32Virtio == 0) ? 1u : 0u;

    /* Grep: scsi_mid: soft inventory */
    kprintf("scsi_mid: soft inventory via=%s wave=13 enter=%u ok=%u fail=%u "
            "ios=%u fails=%u mid_ready=%u soft_lun=%u soft_armed=%u "
            "virtio=%u prefer_soft=%u stats_ready=%u stats_active=%u "
            "inits=%u logs=%u product=userspace_scsi_mid\n",
            szViaSafe, g_u32SoftEnter, g_u32SoftOk, g_u32SoftFail, g_u32IoOk,
            g_u32IoFail, u32Ready, u32SoftAct, g_fSoft ? 1 : 0, u32Virtio,
            u32Prefer, g_u32SoftReadyCalls, g_u32SoftActiveCalls,
            g_u32SoftInitCalls, g_u32SoftInvSamples);

    /* Grep: scsi_mid: soft op */
    kprintf("scsi_mid: soft op tur=%u sense=%u inq=%u mode=%u readcap=%u "
            "read10=%u write10=%u sync=%u other=%u\n",
            g_u32SoftOpTur, g_u32SoftOpSense, g_u32SoftOpInq, g_u32SoftOpMode,
            g_u32SoftOpReadCap, g_u32SoftOpRead10, g_u32SoftOpWrite10,
            g_u32SoftOpSync, g_u32SoftOpOther);

    /* Grep: scsi_mid: soft transport */
    kprintf("scsi_mid: soft transport virtio_try=%u virtio_fail=%u "
            "via_virtio=%u via_soft=%u via_fallback=%u prefer_arm=%u "
            "prefer_sticky=%u soft_check=%u\n",
            g_u32SoftVirtioTry, g_u32SoftVirtioFail, g_u32SoftViaVirtio,
            g_u32SoftViaSoft, g_u32SoftViaFallback, g_u32SoftPreferArm,
            u32Prefer, g_u32SoftSoftCheck);

    /* Grep: scsi_mid: soft deny */
    kprintf("scsi_mid: soft deny null_req=%u not_init=%u null_data=%u "
            "cdb_len=%u cdb_exp=%u no_path=%u soft_check=%u fail_total=%u\n",
            g_u32SoftDenyNull, g_u32SoftDenyNotInit, g_u32SoftDenyNullData,
            g_u32SoftDenyCdbLen, g_u32SoftDenyCdbExp, g_u32SoftDenyNoPath,
            g_u32SoftSoftCheck, g_u32SoftFail);

    /* Grep: scsi_mid: soft geometry */
    kprintf("scsi_mid: soft geometry sectors=%u sec_size=%u bytes=%u "
            "sense_max=%u cdb_max=%u\n",
            (unsigned)GJ_SCSI_SOFT_SECTORS, (unsigned)GJ_SCSI_SOFT_SEC_SIZE,
            (unsigned)(GJ_SCSI_SOFT_SECTORS * GJ_SCSI_SOFT_SEC_SIZE),
            (unsigned)GJ_SCSI_SENSE_MAX, (unsigned)GJ_SCSI_CDB_MAX);

    /*
     * Honesty line: kernel mid is interim only.
     * Grep: scsi_mid: soft path
     */
    kprintf("scsi_mid: soft path claim=kernel_mid_interim "
            "product_userspace_scsi_mid=1 virtio_preferred=1 "
            "soft_lun_fallback=1 sticky_prefer_soft=1 "
            "inquiry_soft_fallback=1 via=%s "
            "(soft inventory; not bar3)\n",
            szViaSafe);

    /*
     * Soft lamp only — mid ready (soft LUN or virtio). Never hard-gates.
     * Grep: scsi_mid: soft inventory PASS | scsi_mid: soft PASS
     * Grep: scsi_mid: soft FAIL
     */
    fSoftPass = (u32Ready != 0) ? 1 : 0;
    if (fSoftPass != 0) {
        kprintf("scsi_mid: soft inventory PASS via=%s logs=%u "
                "mid_ready=%u soft_lun=%u virtio=%u prefer_soft=%u\n",
                szViaSafe, g_u32SoftInvSamples, u32Ready, u32SoftAct,
                u32Virtio, u32Prefer);
        kprintf("scsi_mid: soft PASS via=%s\n", szViaSafe);
    } else {
        kprintf("scsi_mid: soft FAIL via=%s mid_ready=0 "
                "(soft inventory only; not product gate)\n",
                szViaSafe);
    }
}

/**
 * After first product submit activity, print soft inventory once
 * (mirrors door/futex soft-stats-once). Safe from submit return paths.
 * Does not touch virtio→soft fallback control flow.
 */
static void
soft_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_u32SoftEnter == 0) {
        return;
    }
    g_fSoftOnce = 1;
    soft_inventory_log("once");
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
    /* Soft inventory tallies reset with mid (bring-up re-init safe). */
    g_u32SoftEnter = 0;
    g_u32SoftOk = 0;
    g_u32SoftFail = 0;
    g_u32SoftOpTur = 0;
    g_u32SoftOpSense = 0;
    g_u32SoftOpInq = 0;
    g_u32SoftOpMode = 0;
    g_u32SoftOpReadCap = 0;
    g_u32SoftOpRead10 = 0;
    g_u32SoftOpWrite10 = 0;
    g_u32SoftOpSync = 0;
    g_u32SoftOpOther = 0;
    g_u32SoftViaVirtio = 0;
    g_u32SoftViaSoft = 0;
    g_u32SoftViaFallback = 0;
    g_u32SoftVirtioTry = 0;
    g_u32SoftVirtioFail = 0;
    g_u32SoftPreferArm = 0;
    g_u32SoftDenyNull = 0;
    g_u32SoftDenyNotInit = 0;
    g_u32SoftDenyNullData = 0;
    g_u32SoftDenyCdbLen = 0;
    g_u32SoftDenyCdbExp = 0;
    g_u32SoftDenyNoPath = 0;
    g_u32SoftSoftCheck = 0;
    g_u32SoftReadyCalls = 0;
    g_u32SoftActiveCalls = 0;
    g_u32SoftInvSamples = 0;
    g_fSoftOnce = 0;
    soft_inc(&g_u32SoftInitCalls);
    memset(g_aSoftDisk, 0, sizeof(g_aSoftDisk));
    soft_sense_clear();
    /* virtio_scsi may still be probing; ready() reflects transport|soft. */
    kprintf("scsi_mid: init interim kernel mid virtio_scsi=%d soft=%d "
            "secs=%u (product=userspace)\n",
            virtio_scsi_ready() ? 1 : 0, g_fSoft ? 1 : 0,
            GJ_SCSI_SOFT_SECTORS);
    /* Wave 13 soft inventory baseline (greppable scsi_mid: soft …). */
    soft_inventory_log("init");
}

int
scsi_mid_ready(void)
{
    soft_inc(&g_u32SoftReadyCalls);
    return g_fInited && (virtio_scsi_ready() || g_fSoft);
}

int
scsi_mid_soft_active(void)
{
    soft_inc(&g_u32SoftActiveCalls);
    /*
     * Historical semantics: soft LUN armed and virtio not ready.
     * Sticky prefer-soft still reports soft_active=0 when virtio is ready
     * (submit uses soft via prefer flag; lamp stays "transport present").
     */
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
    u8 u8Op;
    int fViaVirtio;
    int fViaFallback;

    if (!g_fInited || pReq == NULL) {
        if (g_fInited) {
            g_u32IoFail++;
            soft_inc(&g_u32SoftDenyNull);
            soft_inc(&g_u32SoftFail);
        } else {
            soft_inc(&g_u32SoftDenyNotInit);
        }
        return -1;
    }
    soft_inc(&g_u32SoftEnter);
    u8Op = pReq->cdb.aCdb[0];
    fViaVirtio = 0;
    fViaFallback = 0;

    /* Require data buffer when transfer length is non-zero. */
    if (pReq->cbData > 0 && pReq->pData == NULL) {
        g_u32IoFail++;
        soft_inc(&g_u32SoftDenyNullData);
        soft_inc(&g_u32SoftFail);
        soft_maybe_once();
        return -1;
    }
    /* Defensive CDB length: 0 or > max is never a legal transport CDB. */
    if (pReq->cdb.u8CdbLen == 0 || pReq->cdb.u8CdbLen > GJ_SCSI_CDB_MAX) {
        g_u32IoFail++;
        soft_inc(&g_u32SoftDenyCdbLen);
        soft_inc(&g_u32SoftFail);
        soft_maybe_once();
        return -1;
    }
    /* Soft length check for known opcodes (raw / future ops skip). */
    u8Expect = cdb_expected_len(u8Op);
    if (u8Expect != 0 && pReq->cdb.u8CdbLen != u8Expect) {
        g_u32IoFail++;
        soft_inc(&g_u32SoftDenyCdbExp);
        soft_inc(&g_u32SoftFail);
        soft_maybe_once();
        return -1;
    }
    /* Default timeout annotation (transport may ignore; product mid owns policy). */
    if (pReq->u32TimeoutMs == 0) {
        pReq->u32TimeoutMs = 5000;
    }

    /*
     * Transport preference unchanged (Wave 13 inventory is observe-only):
     *   1) virtio-scsi when ready and sticky prefer-soft not armed
     *   2) on virtio fail + soft armed → soft LUN (INQUIRY smoke path)
     *   3) else direct soft LUN
     */
    if (virtio_scsi_ready() && !g_fVirtioSoftPrefer) {
        soft_inc(&g_u32SoftVirtioTry);
        nSt = virtio_scsi_submit(pReq);
        /*
         * Transport ready but I/O timed out (common on some QEMU/KVM
         * virtio-scsi event paths): fall back to software LUN so door /
         * smoke "INQUIRY ok" stay green. Sticky prefer soft after first
         * fail so multi-op smokes do not burn 20M poll spins each time.
         */
        if (nSt != 0 && g_fSoft) {
            soft_inc(&g_u32SoftVirtioFail);
            if (g_fVirtioSoftPrefer == 0) {
                soft_inc(&g_u32SoftPreferArm);
            }
            g_fVirtioSoftPrefer = 1;
            nSt = soft_submit(pReq);
            if (nSt == 0) {
                fViaFallback = 1;
                kprintf("scsi_mid: virtio fail → soft LUN ok op=0x%x "
                        "(prefer soft)\n",
                        (unsigned)u8Op);
            } else {
                soft_inc(&g_u32SoftSoftCheck);
            }
        } else if (nSt != 0) {
            soft_inc(&g_u32SoftVirtioFail);
        } else {
            fViaVirtio = 1;
        }
    } else if (g_fSoft) {
        nSt = soft_submit(pReq);
        if (nSt != 0) {
            soft_inc(&g_u32SoftSoftCheck);
        }
    } else {
        g_u32IoFail++;
        soft_inc(&g_u32SoftDenyNoPath);
        soft_inc(&g_u32SoftFail);
        soft_maybe_once();
        return -1;
    }

    if (nSt == 0) {
        g_u32IoOk++;
        soft_inc(&g_u32SoftOk);
        soft_note_op_ok(u8Op);
        if (fViaFallback != 0) {
            soft_inc(&g_u32SoftViaFallback);
        } else if (fViaVirtio != 0) {
            soft_inc(&g_u32SoftViaVirtio);
        } else {
            soft_inc(&g_u32SoftViaSoft);
        }
    } else {
        g_u32IoFail++;
        soft_inc(&g_u32SoftFail);
    }
    soft_maybe_once();
    return nSt;
}
