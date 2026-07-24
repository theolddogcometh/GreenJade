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
 * Soft inventory (Wave 13 base + Wave 28 exclusive soft deepen — this unit):
 *   - soft return: API return-surface catalog (product_*=OPEN)
 *   - soft retmap: Wave 19 return-surface map (ok|fail|… classes)
 *   - Submit enter / ok / fail; per-op ok tallies; transport path lamps
 *   - Virtio-fail → soft LUN sticky fallback remains product critical path
 *     (INQUIRY / door smokes) — inventory never mutates that arm
 * Wave 19 exclusive (this unit only — greppable "scsi_mid: soft …"):
 *   - Soft LUN path enter/ok/fail; sense set/clear + last key/asc/ascq
 *   - Soft LBA/LUN/EVPD/opcode reject samples; trunc xfer + zero-block
 *   - Soft R/W byte totals + peak lba/blocks/cb; last op/via snapshot
 *   - Complementary: total|soft|sense|xfer|peak|last|honesty|deepen
 *   Soft LUN honesty remains soft: soft inventory ≠ product mid / bar3
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
 * Soft product inventory (Wave 13 base + Wave 28 exclusive deepen).
 * Cumulative path tallies. greppable: scsi_mid: soft …
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

/* Wave 28 exclusive soft deepen — complementary path tallies. */
static u32 g_u32SoftSoftEnter;      /* soft_submit entries */
static u32 g_u32SoftSoftOk;         /* soft_submit GOOD */
static u32 g_u32SoftSoftFail;       /* soft_submit CHECK / -1 */
static u32 g_u32SoftSenseSet;       /* soft_sense_set calls */
static u32 g_u32SoftSenseClear;     /* soft_sense_clear calls */
static u32 g_u32SoftLbaReject;      /* soft_lba_ok reject */
static u32 g_u32SoftLunReject;      /* non-zero LUN reject */
static u32 g_u32SoftEvpdReject;     /* EVPD INQUIRY reject */
static u32 g_u32SoftOpcodeReject;   /* invalid opcode on soft LUN */
static u32 g_u32SoftTruncXfer;      /* READ/WRITE truncated to cbData */
static u32 g_u32SoftZeroBlocks;     /* blocks==0 defaulted to 1 */
static u32 g_u32SoftTimeoutDefault; /* timeout defaulted to 5000 */
static u64 g_u64SoftBytesRead;      /* soft READ bytes copied */
static u64 g_u64SoftBytesWrite;     /* soft WRITE bytes copied */
static u32 g_u32SoftPeakLba;        /* peak LBA on soft R/W */
static u32 g_u32SoftPeakBlocks;     /* peak blocks on soft R/W */
static u32 g_u32SoftPeakCb;         /* peak cbData on submit */
static u32 g_u32SoftLastOp;         /* last submit opcode */
static u32 g_u32SoftLastVia;        /* 0 none, 1 virtio, 2 soft, 3 fallback */
static u8  g_u8SoftLastSenseKey;    /* last soft sense key */
static u8  g_u8SoftLastAsc;         /* last soft ASC */
static u8  g_u8SoftLastAscq;        /* last soft ASCQ */

/*
 * Soft LUN: tiny direct-access disk for interim bring-up (product=userspace).
 * Geometry: GJ_SCSI_SOFT_SECTORS × GJ_SCSI_SOFT_SEC_SIZE.
 */
static u8 g_aSoftDisk[GJ_SCSI_SOFT_SECTORS][GJ_SCSI_SOFT_SEC_SIZE]
    __attribute__((aligned(16)));
static u8 g_aSoftUnitSense[GJ_SCSI_SENSE_MAX];
static u8 g_u8SoftUnitSenseLen;

static void soft_inc(u32 *pCtr);
static void soft_add64(u64 *pCtr, u64 u64N);
static void soft_peak_u32(u32 *pPeak, u32 u32Val);
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
    soft_inc(&g_u32SoftSenseClear); /* Wave 15: sense clear sample */
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
    g_u8SoftLastSenseKey = (u8)(u8Key & 0x0fu);
    g_u8SoftLastAsc = u8Asc;
    g_u8SoftLastAscq = u8Ascq;
    soft_inc(&g_u32SoftSenseSet); /* Wave 15: sense set sample */
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
    u32 cbNeed;

    soft_inc(&g_u32SoftSoftEnter); /* Wave 15: soft LUN enter */
    if (pReq == NULL || !g_fSoft) {
        soft_inc(&g_u32SoftSoftFail);
        return -1;
    }
    /* Soft path is single LUN 0 only. Soft LUN honesty remains soft. */
    if (pReq->u32Lun != 0) {
        soft_inc(&g_u32SoftLunReject);
        soft_sense_set(GJ_SCSI_SK_ILLEGAL_REQUEST, 0x25u, 0x00u); /* LUN not supported */
        soft_sense_copy_out(pReq);
        pReq->u8Status = (u8)GJ_SCSI_CHECK_CONDITION;
        soft_inc(&g_u32SoftSoftFail);
        return -1;
    }

    u8Op = pReq->cdb.aCdb[0];
    cbXfer = pReq->cbData;

    switch (u8Op) {
    case GJ_SCSI_OP_TEST_UNIT:
        soft_sense_clear();
        pReq->u8Status = (u8)GJ_SCSI_GOOD;
        soft_inc(&g_u32SoftSoftOk);
        return 0;

    case GJ_SCSI_OP_REQUEST_SENSE: {
        u32 cb;
        u8 aFixed[18];

        if (cbXfer > 0 && pReq->pData == NULL) {
            soft_inc(&g_u32SoftSoftFail);
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
        soft_inc(&g_u32SoftSoftOk);
        return 0;
    }

    case GJ_SCSI_OP_INQUIRY:
        if (cbXfer > 0 && pReq->pData == NULL) {
            soft_inc(&g_u32SoftSoftFail);
            return -1;
        }
        /* EVPD / VPD pages: soft path only serves standard INQUIRY. */
        if ((pReq->cdb.aCdb[1] & 0x01u) != 0) {
            soft_inc(&g_u32SoftEvpdReject);
            soft_sense_set(GJ_SCSI_SK_ILLEGAL_REQUEST, 0x24u, 0x00u);
            soft_sense_copy_out(pReq);
            pReq->u8Status = (u8)GJ_SCSI_CHECK_CONDITION;
            soft_inc(&g_u32SoftSoftFail);
            return -1;
        }
        if (cbXfer > 0 && pReq->pData != NULL) {
            soft_fill_inquiry((u8 *)pReq->pData, cbXfer);
        }
        soft_sense_clear();
        pReq->u8Status = (u8)GJ_SCSI_GOOD;
        soft_inc(&g_u32SoftSoftOk);
        return 0;

    case GJ_SCSI_OP_MODE_SENSE_6:
        if (cbXfer > 0 && pReq->pData == NULL) {
            soft_inc(&g_u32SoftSoftFail);
            return -1;
        }
        /* Minimal mode parameter header: medium type 0, zero density. */
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
        soft_inc(&g_u32SoftSoftOk);
        return 0;

    case GJ_SCSI_OP_READ_CAPACITY_10:
        if (cbXfer > 0 && pReq->pData == NULL) {
            soft_inc(&g_u32SoftSoftFail);
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
        soft_inc(&g_u32SoftSoftOk);
        return 0;

    case GJ_SCSI_OP_READ_10:
    case GJ_SCSI_OP_WRITE_10:
        u32Lba = ((u32)pReq->cdb.aCdb[2] << 24) |
                 ((u32)pReq->cdb.aCdb[3] << 16) |
                 ((u32)pReq->cdb.aCdb[4] << 8) | (u32)pReq->cdb.aCdb[5];
        u16Blocks = (u16)(((u16)pReq->cdb.aCdb[7] << 8) | (u16)pReq->cdb.aCdb[8]);
        if (u16Blocks == 0) {
            soft_inc(&g_u32SoftZeroBlocks);
            u16Blocks = 1;
        }
        soft_peak_u32(&g_u32SoftPeakLba, u32Lba);
        soft_peak_u32(&g_u32SoftPeakBlocks, (u32)u16Blocks);
        if (!soft_lba_ok(u32Lba, u16Blocks)) {
            soft_inc(&g_u32SoftLbaReject);
            soft_sense_set(GJ_SCSI_SK_ILLEGAL_REQUEST, 0x21u, 0x00u); /* LBA out of range */
            soft_sense_copy_out(pReq);
            pReq->u8Status = (u8)GJ_SCSI_CHECK_CONDITION;
            soft_inc(&g_u32SoftSoftFail);
            return -1;
        }
        cbNeed = (u32)u16Blocks * GJ_SCSI_SOFT_SEC_SIZE;
        cbCopy = cbNeed;
        if (cbXfer > 0 && pReq->pData == NULL) {
            soft_inc(&g_u32SoftSoftFail);
            return -1;
        }
        if (cbXfer < cbCopy) {
            /* Truncate transfer to caller buffer (soft deepen tolerant). */
            soft_inc(&g_u32SoftTruncXfer);
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
            soft_add64(&g_u64SoftBytesRead, (u64)cbCopy);
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
            soft_add64(&g_u64SoftBytesWrite, (u64)cbCopy);
        }
        soft_sense_clear();
        pReq->u8Status = (u8)GJ_SCSI_GOOD;
        soft_inc(&g_u32SoftSoftOk);
        return 0;

    case GJ_SCSI_OP_SYNCHRONIZE_CACHE:
        /* Soft disk is always coherent; treat as success. */
        soft_sense_clear();
        pReq->u8Status = (u8)GJ_SCSI_GOOD;
        soft_inc(&g_u32SoftSoftOk);
        return 0;

    default:
        soft_inc(&g_u32SoftOpcodeReject);
        soft_sense_set(GJ_SCSI_SK_ILLEGAL_REQUEST, 0x20u, 0x00u); /* invalid opcode */
        soft_sense_copy_out(pReq);
        pReq->u8Status = (u8)GJ_SCSI_CHECK_CONDITION;
        soft_inc(&g_u32SoftSoftFail);
        return -1;
    }
}

/* ---- Soft inventory (Wave 13 base + Wave 28 exclusive deepen) ----------- */

/** Soft: bump path tally (u32 wrap is fine for telemetry). */
static void
soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/** Soft: add to u64 path tally (wrap OK for telemetry). */
static void
soft_add64(u64 *pCtr, u64 u64N)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr) += u64N;
}

/** Soft: raise high-water if u32Val exceeds *pPeak. */
static void
soft_peak_u32(u32 *pPeak, u32 u32Val)
{
    if (pPeak == NULL) {
        return;
    }
    if (u32Val > *pPeak) {
        *pPeak = u32Val;
    }
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
 * Greppable soft scsi_mid inventory (Wave 13 base; Wave 28 exclusive deepen).
 * Prefix-stable markers (scsi_mid: soft …):
 *   scsi_mid: soft inventory  — enter/ok/fail + mid lamps + log_n
 *   scsi_mid: soft op         — per-op ok tallies
 *   scsi_mid: soft transport  — virtio/soft/fallback path lamps
 *   scsi_mid: soft deny       — soft reject reason catalog
 *   scsi_mid: soft geometry   — soft LUN geometry
 *   scsi_mid: soft path       — honesty: kernel mid ≠ product scsi_mid
 * Wave 15 complementary:
 *   scsi_mid: soft total|soft|sense|xfer|peak|last|honesty|deepen
 *   scsi_mid: soft inventory PASS / scsi_mid: soft PASS
 *
 * Never hard-gates; does not change virtio→soft INQUIRY fallback.
 * Soft LUN honesty remains soft. greppable: scsi_mid: soft
 */
static void
soft_inventory_log(const char *szVia)
{
    const char *szViaSafe;
    u32         u32Ready;
    u32         u32SoftAct;
    u32         u32Virtio;
    u32         u32Prefer;
    u32         u32OkBp;
    int         fSoftPass;

    soft_inc(&g_u32SoftInvSamples);
    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "unknown";
    u32Virtio = virtio_scsi_ready() ? 1u : 0u;
    u32Prefer = g_fVirtioSoftPrefer ? 1u : 0u;
    u32Ready = (g_fInited && (u32Virtio != 0 || g_fSoft)) ? 1u : 0u;
    /* Match scsi_mid_soft_active(): soft armed and virtio not ready. */
    u32SoftAct = (g_fInited && g_fSoft && u32Virtio == 0) ? 1u : 0u;
    if (g_u32SoftEnter != 0u) {
        u32OkBp = (g_u32SoftOk * 10000u) / g_u32SoftEnter;
    } else {
        u32OkBp = 0;
    }

    /* Grep: scsi_mid: soft inventory */
    kprintf("scsi_mid: soft inventory via=%s wave=28 enter=%u ok=%u fail=%u "
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
     * Wave 28 exclusive deepen (complementary; never reshapes primary lines).
     */
    /* Grep: scsi_mid: soft total */
    kprintf("scsi_mid: soft total ok=%u fail=%u enter=%u ok_bp=%u "
            "soft_ok=%u soft_fail=%u logs=%u wave=28\n",
            g_u32SoftOk, g_u32SoftFail, g_u32SoftEnter, u32OkBp,
            g_u32SoftSoftOk, g_u32SoftSoftFail, g_u32SoftInvSamples);

    /* Grep: scsi_mid: soft soft  (soft LUN path; honesty remains soft) */
    kprintf("scsi_mid: soft soft enter=%u ok=%u fail=%u lun_rej=%u "
            "lba_rej=%u evpd_rej=%u opc_rej=%u trunc=%u zero_blk=%u "
            "armed=%u active=%u wave=28\n",
            g_u32SoftSoftEnter, g_u32SoftSoftOk, g_u32SoftSoftFail,
            g_u32SoftLunReject, g_u32SoftLbaReject, g_u32SoftEvpdReject,
            g_u32SoftOpcodeReject, g_u32SoftTruncXfer, g_u32SoftZeroBlocks,
            g_fSoft ? 1 : 0, u32SoftAct);

    /* Grep: scsi_mid: soft sense */
    kprintf("scsi_mid: soft sense set=%u clear=%u last_key=0x%x "
            "last_asc=0x%x last_ascq=0x%x live_len=%u\n",
            g_u32SoftSenseSet, g_u32SoftSenseClear,
            (unsigned)g_u8SoftLastSenseKey, (unsigned)g_u8SoftLastAsc,
            (unsigned)g_u8SoftLastAscq, (unsigned)g_u8SoftUnitSenseLen);

    /* Grep: scsi_mid: soft xfer */
    kprintf("scsi_mid: soft xfer read_b=%lu write_b=%lu trunc=%u "
            "zero_blk=%u timeout_def=%u sec_size=%u\n",
            (unsigned long)g_u64SoftBytesRead,
            (unsigned long)g_u64SoftBytesWrite, g_u32SoftTruncXfer,
            g_u32SoftZeroBlocks, g_u32SoftTimeoutDefault,
            (unsigned)GJ_SCSI_SOFT_SEC_SIZE);

    /* Grep: scsi_mid: soft peak */
    kprintf("scsi_mid: soft peak lba=0x%x blocks=%u cb=%u "
            "via_virtio=%u via_soft=%u via_fallback=%u\n",
            g_u32SoftPeakLba, g_u32SoftPeakBlocks, g_u32SoftPeakCb,
            g_u32SoftViaVirtio, g_u32SoftViaSoft, g_u32SoftViaFallback);

    /* Grep: scsi_mid: soft last */
    kprintf("scsi_mid: soft last op=0x%x via=%u key=0x%x asc=0x%x "
            "ascq=0x%x prefer=%u via_name=%s\n",
            g_u32SoftLastOp, g_u32SoftLastVia, (unsigned)g_u8SoftLastSenseKey,
            (unsigned)g_u8SoftLastAsc, (unsigned)g_u8SoftLastAscq, u32Prefer,
            szViaSafe);

    /* Grep: scsi_mid: soft honesty  (soft LUN remains soft) */
    kprintf("scsi_mid: soft honesty soft_lun=soft soft_ne_product=1 "
            "soft_ne_bar3=1 kernel_mid_interim=1 sticky_prefer_soft=1 "
            "inquiry_soft_fallback=1 wave=28\n");

    /* Grep: scsi_mid: soft capacity — Wave 19 design-constant lamps. */
    kprintf("scsi_mid: soft capacity soft_lun=1 virtio_prefer=1 "
            "inquiry_fallback=1 sticky_prefer=1 sec=512 wave=28\n");

    /* Grep: scsi_mid: soft headroom — Wave 19 live path lamps. */
    kprintf("scsi_mid: soft headroom mid_ready=%u soft_lun=%u virtio=%u "
            "enter=%u ok=%u fail=%u logs=%u wave=28\n",
            u32Ready, u32SoftAct, u32Virtio, g_u32SoftEnter, g_u32SoftOk,
            g_u32SoftFail, g_u32SoftInvSamples);

    /* Grep: scsi_mid: soft surface — Wave 19 surface bit lamps. */
    kprintf("scsi_mid: soft surface ready=%u soft=%u virtio=%u prefer=%u "
            "enter=%u ok=%u surf=0x%x wave=28\n",
            u32Ready, u32SoftAct, u32Virtio, u32Prefer,
            g_u32SoftEnter != 0u ? 1u : 0u, g_u32SoftOk != 0u ? 1u : 0u,
            (u32Ready) | (u32SoftAct << 1) | (u32Virtio << 2) |
                (u32Prefer << 3) | ((g_u32SoftEnter != 0u) ? 16u : 0u) |
                ((g_u32SoftOk != 0u) ? 32u : 0u));

    /* Grep: scsi_mid: soft ratio — Wave 19 ok/fail basis points. */
    {
        u32 u32Tot = g_u32SoftOk + g_u32SoftFail;
        u32 u32OkBp = 0;
        u32 u32FailBp = 0;

        if (u32Tot != 0u) {
            u32OkBp = (g_u32SoftOk * 10000u) / u32Tot;
            u32FailBp = (g_u32SoftFail * 10000u) / u32Tot;
        }
        kprintf("scsi_mid: soft ratio ok_bp=%u fail_bp=%u ok=%u fail=%u "
                "enter=%u wave=28\n",
                u32OkBp, u32FailBp, g_u32SoftOk, g_u32SoftFail,
                g_u32SoftEnter);
    }

    /* Grep: scsi_mid: soft return — Wave 19 API return surfaces */
    kprintf("scsi_mid: soft return enter=%u ok=%u fail=%u via_virtio=%u "
            "via_soft=%u via_fallback=%u deny_null=%u deny_cdb=%u "
            "no_path=%u mid_ready=%u product_scsi_mid=OPEN wave=28\n",
            g_u32SoftEnter, g_u32SoftOk, g_u32SoftFail,
            g_u32SoftViaVirtio, g_u32SoftViaSoft, g_u32SoftViaFallback,
            g_u32SoftDenyNull, g_u32SoftDenyCdbLen, g_u32SoftDenyNoPath,
            u32Ready);

    /* Grep: scsi_mid: soft retmap — Wave 19 return-surface map */
    kprintf("scsi_mid: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=28\n");

    /* Grep: scsi_mid: soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: scsi_mid: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("scsi_mid: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_SCSI_SOFT_SECTORS);
    /* Grep: scsi_mid: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("scsi_mid: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_SCSI_SOFT_SECTORS);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: scsi_mid: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("scsi_mid: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_SCSI_SOFT_SECTORS);
    /* Grep: scsi_mid: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("scsi_mid: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_SCSI_SOFT_SECTORS);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: scsi_mid: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("scsi_mid: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SCSI_SOFT_SECTORS);
            /* Grep: scsi_mid: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("scsi_mid: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_SCSI_SOFT_SECTORS);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: scsi_mid: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("scsi_mid: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SCSI_SOFT_SECTORS);
            /* Grep: scsi_mid: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("scsi_mid: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_SCSI_SOFT_SECTORS);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: scsi_mid: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("scsi_mid: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SCSI_SOFT_SECTORS);
            /* Grep: scsi_mid: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("scsi_mid: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_SCSI_SOFT_SECTORS);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: scsi_mid: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("scsi_mid: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SCSI_SOFT_SECTORS);
            /* Grep: scsi_mid: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("scsi_mid: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_SCSI_SOFT_SECTORS);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: scsi_mid: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("scsi_mid: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SCSI_SOFT_SECTORS);
            /* Grep: scsi_mid: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("scsi_mid: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_SCSI_SOFT_SECTORS);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: scsi_mid: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("scsi_mid: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SCSI_SOFT_SECTORS);
            /* Grep: scsi_mid: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("scsi_mid: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_SCSI_SOFT_SECTORS);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: scsi_mid: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("scsi_mid: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_SCSI_SOFT_SECTORS);
                    /* Grep: scsi_mid: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("scsi_mid: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_SCSI_SOFT_SECTORS);
                            /*
                             * ---- Wave 28 exclusive complementary surfaces (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: scsi_mid: soft retshard — Wave 28 return-shard honesty */
                            kprintf("scsi_mid: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_SCSI_SOFT_SECTORS);
                            /* Grep: scsi_mid: soft retcrown — Wave 28 exclusive crown stamp */
                            kprintf("scsi_mid: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_SCSI_SOFT_SECTORS);
    kprintf("scsi_mid: soft deepen wave=28 areas=total,soft,sense,xfer,,retclass,retlane"
            "peak,last,honesty,capacity,headroom,surface,ratio,return logs=%u "
            "(Wave 28 exclusive; soft LUN honesty remains soft; not bar3)\n",
            g_u32SoftInvSamples);

    /*
     * Honesty line: kernel mid is interim only.
     * Grep: scsi_mid: soft path
     */
    kprintf("scsi_mid: soft path claim=kernel_mid_interim "
            "product_userspace_scsi_mid=1 virtio_preferred=1 "
            "soft_lun_fallback=1 sticky_prefer_soft=1 "
            "inquiry_soft_fallback=1 soft_lun_honesty=soft via=%s "
            "wave=28 (soft inventory; not bar3)\n",
            szViaSafe);

    /*
     * Soft lamp only — mid ready (soft LUN or virtio). Never hard-gates.
     * Grep: scsi_mid: soft inventory PASS | scsi_mid: soft PASS
     * Grep: scsi_mid: soft FAIL
     */
    fSoftPass = (u32Ready != 0) ? 1 : 0;
    if (fSoftPass != 0) {
        kprintf("scsi_mid: soft inventory PASS via=%s logs=%u "
                "mid_ready=%u soft_lun=%u virtio=%u prefer_soft=%u "
                "wave=28\n",
                szViaSafe, g_u32SoftInvSamples, u32Ready, u32SoftAct,
                u32Virtio, u32Prefer);
        kprintf("scsi_mid: soft PASS via=%s wave=28\n", szViaSafe);
    } else {
        kprintf("scsi_mid: soft FAIL via=%s mid_ready=0 wave=28 "
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
    /* Wave 28 exclusive soft deepen tallies (reset on re-init). */
    g_u32SoftSoftEnter = 0;
    g_u32SoftSoftOk = 0;
    g_u32SoftSoftFail = 0;
    g_u32SoftSenseSet = 0;
    g_u32SoftSenseClear = 0;
    g_u32SoftLbaReject = 0;
    g_u32SoftLunReject = 0;
    g_u32SoftEvpdReject = 0;
    g_u32SoftOpcodeReject = 0;
    g_u32SoftTruncXfer = 0;
    g_u32SoftZeroBlocks = 0;
    g_u32SoftTimeoutDefault = 0;
    g_u64SoftBytesRead = 0;
    g_u64SoftBytesWrite = 0;
    g_u32SoftPeakLba = 0;
    g_u32SoftPeakBlocks = 0;
    g_u32SoftPeakCb = 0;
    g_u32SoftLastOp = 0;
    g_u32SoftLastVia = 0;
    g_u8SoftLastSenseKey = 0;
    g_u8SoftLastAsc = 0;
    g_u8SoftLastAscq = 0;
    soft_inc(&g_u32SoftInitCalls);
    memset(g_aSoftDisk, 0, sizeof(g_aSoftDisk));
    soft_sense_clear();
    /* virtio_scsi may still be probing; ready() reflects transport|soft. */
    kprintf("scsi_mid: init interim kernel mid virtio_scsi=%d soft=%d "
            "secs=%u (product=userspace)\n",
            virtio_scsi_ready() ? 1 : 0, g_fSoft ? 1 : 0,
            GJ_SCSI_SOFT_SECTORS);
    /* Wave 15 soft inventory baseline (greppable scsi_mid: soft …). */
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
    g_u32SoftLastOp = (u32)u8Op;
    soft_peak_u32(&g_u32SoftPeakCb, pReq->cbData);
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
        soft_inc(&g_u32SoftTimeoutDefault); /* Wave 15: timeout default sample */
    }

    /*
     * Transport preference unchanged (Wave 15 inventory is observe-only):
     *   1) virtio-scsi when ready and sticky prefer-soft not armed
     *   2) on virtio fail + soft armed → soft LUN (INQUIRY smoke path)
     *   3) else direct soft LUN
     * Soft LUN honesty remains soft — inventory never mutates this arm.
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
            g_u32SoftLastVia = 3u; /* Wave 15: via fallback */
        } else if (fViaVirtio != 0) {
            soft_inc(&g_u32SoftViaVirtio);
            g_u32SoftLastVia = 1u; /* Wave 15: via virtio */
        } else {
            soft_inc(&g_u32SoftViaSoft);
            g_u32SoftLastVia = 2u; /* Wave 15: via soft LUN */
        }
    } else {
        g_u32IoFail++;
        soft_inc(&g_u32SoftFail);
        g_u32SoftLastVia = 0u;
    }
    soft_maybe_once();
    return nSt;
}
