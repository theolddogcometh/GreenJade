/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Userspace SCSI CDB builders + soft LUN + scsi_mid_submit (pure C11).
 *
 * CDB path
 * --------
 * scsi_cdb_* fill struct scsi_cdb (opcode + big-endian fields per SPC/SBC).
 * scsi_mid_submit takes a packed scsi_io and:
 *   - GJ freestanding (GJ_FREESTANDING / __gj_freestanding):
 *       if door READY: decode CDB → gj_scsi(GJ_SCSI_OP_*) → scsi_door
 *       else: userspace soft LUN (same policy as host soft)
 *   - Host / no door (SCSI_HAS_SYS=0):
 *       userspace soft LUN (auto-armed); never needs a kernel door
 *
 * Soft LUN (product mid ownership of sense/LUN/geometry):
 *   TUR, REQUEST SENSE, INQUIRY, MODE SENSE(6), READ CAPACITY(10),
 *   READ(10), WRITE(10), SYNCHRONIZE CACHE(10). LUN 0 only;
 *   illegal LUN / opcode / LBA → CHECK + fixed sense.
 *
 * Opcode constants: include/scsi_mid.h (SCSI_OP_*).
 * Door ops:         user/libgj/include/gj/syscalls.h (GJ_SCSI_OP_*).
 */
#include <scsi_mid.h>

#if defined(GJ_FREESTANDING) || defined(__gj_freestanding)
#include <gj/syscalls.h>
#define SCSI_HAS_SYS 1
#elif defined(__linux__) && !defined(SCSI_HOST_ONLY)
/* Host CI: no kernel door; soft LUN keeps submit green. */
#define SCSI_HAS_SYS 0
#else
#define SCSI_HAS_SYS 0
#endif

/* ---- local mem helpers (no host libc dependency on freestanding path) --- */

static void
mid_memzero(void *p, size_t cb)
{
    uint8_t *pB = (uint8_t *)p;
    size_t i;

    if (pB == NULL) {
        return;
    }
    for (i = 0; i < cb; i++) {
        pB[i] = 0;
    }
}

static void
mid_memcpy(void *pDst, const void *pSrc, size_t cb)
{
    uint8_t *pD = (uint8_t *)pDst;
    const uint8_t *pS = (const uint8_t *)pSrc;
    size_t i;

    if (pD == NULL || pS == NULL) {
        return;
    }
    for (i = 0; i < cb; i++) {
        pD[i] = pS[i];
    }
}

/* ---- soft LUN state ----------------------------------------------------- */

static int g_fSoftArmed;
static uint32_t g_u32SoftIoOk;
static uint32_t g_u32SoftIoFail;
static uint8_t g_aSoftDisk[SCSI_MID_SOFT_SECTORS][SCSI_MID_SOFT_SEC_SIZE];
static uint8_t g_aSoftUnitSense[SCSI_MID_SENSE_MAX];
static uint8_t g_u8SoftUnitSenseLen;

/* ---- CDB builders ------------------------------------------------------- */

void
scsi_cdb_test_unit_ready(struct scsi_cdb *pCdb)
{
    if (pCdb == NULL) {
        return;
    }
    mid_memzero(pCdb, sizeof(*pCdb));
    pCdb->aCdb[0] = SCSI_OP_TEST_UNIT_READY;
    pCdb->u8Len = 6;
}

void
scsi_cdb_request_sense(struct scsi_cdb *pCdb, uint8_t u8Alloc)
{
    if (pCdb == NULL) {
        return;
    }
    mid_memzero(pCdb, sizeof(*pCdb));
    pCdb->aCdb[0] = SCSI_OP_REQUEST_SENSE;
    pCdb->aCdb[4] = u8Alloc ? u8Alloc : 18u; /* standard fixed sense size */
    pCdb->u8Len = 6;
}

/*
 * INQUIRY (6-byte CDB, SPC):
 *   [0] opcode 0x12
 *   [1] EVPD
 *   [2] page code
 *   [3..4] allocation length (BE)
 */
void
scsi_cdb_inquiry(struct scsi_cdb *pCdb, uint8_t u8Evpd, uint8_t u8Page,
                 uint16_t u16Alloc)
{
    if (pCdb == NULL) {
        return;
    }
    mid_memzero(pCdb, sizeof(*pCdb));
    pCdb->aCdb[0] = SCSI_OP_INQUIRY;
    pCdb->aCdb[1] = u8Evpd ? 1u : 0;
    pCdb->aCdb[2] = u8Page;
    pCdb->aCdb[3] = (uint8_t)((u16Alloc >> 8) & 0xff);
    pCdb->aCdb[4] = (uint8_t)(u16Alloc & 0xff);
    pCdb->u8Len = 6;
}

void
scsi_cdb_mode_sense6(struct scsi_cdb *pCdb, uint8_t u8Page, uint8_t u8Alloc)
{
    if (pCdb == NULL) {
        return;
    }
    mid_memzero(pCdb, sizeof(*pCdb));
    pCdb->aCdb[0] = SCSI_OP_MODE_SENSE_6;
    pCdb->aCdb[2] = u8Page;
    pCdb->aCdb[4] = u8Alloc ? u8Alloc : 4u; /* mode parameter header only */
    pCdb->u8Len = 6;
}

/*
 * READ CAPACITY (10) (10-byte CDB, SBC):
 *   [0] opcode 0x25; remaining bytes zero for simple capacity probe.
 */
void
scsi_cdb_read_capacity10(struct scsi_cdb *pCdb)
{
    if (pCdb == NULL) {
        return;
    }
    mid_memzero(pCdb, sizeof(*pCdb));
    pCdb->aCdb[0] = SCSI_OP_READ_CAPACITY_10;
    pCdb->u8Len = 10;
}

/*
 * READ (10) (10-byte CDB, SBC):
 *   [0] opcode 0x28
 *   [2..5] LBA (BE)
 *   [7..8] transfer length in logical blocks (BE)
 */
void
scsi_cdb_read10(struct scsi_cdb *pCdb, uint32_t u32Lba, uint16_t u16Blocks)
{
    if (pCdb == NULL) {
        return;
    }
    mid_memzero(pCdb, sizeof(*pCdb));
    pCdb->aCdb[0] = SCSI_OP_READ_10;
    pCdb->aCdb[2] = (uint8_t)((u32Lba >> 24) & 0xff);
    pCdb->aCdb[3] = (uint8_t)((u32Lba >> 16) & 0xff);
    pCdb->aCdb[4] = (uint8_t)((u32Lba >> 8) & 0xff);
    pCdb->aCdb[5] = (uint8_t)(u32Lba & 0xff);
    pCdb->aCdb[7] = (uint8_t)((u16Blocks >> 8) & 0xff);
    pCdb->aCdb[8] = (uint8_t)(u16Blocks & 0xff);
    pCdb->u8Len = 10;
}

/*
 * WRITE (10): same layout as READ (10); opcode 0x2A.
 */
void
scsi_cdb_write10(struct scsi_cdb *pCdb, uint32_t u32Lba, uint16_t u16Blocks)
{
    if (pCdb == NULL) {
        return;
    }
    scsi_cdb_read10(pCdb, u32Lba, u16Blocks);
    pCdb->aCdb[0] = SCSI_OP_WRITE_10;
}

void
scsi_cdb_synchronize_cache10(struct scsi_cdb *pCdb, uint32_t u32Lba,
                             uint16_t u16Blocks)
{
    if (pCdb == NULL) {
        return;
    }
    mid_memzero(pCdb, sizeof(*pCdb));
    pCdb->aCdb[0] = SCSI_OP_SYNCHRONIZE_CACHE;
    pCdb->aCdb[2] = (uint8_t)((u32Lba >> 24) & 0xff);
    pCdb->aCdb[3] = (uint8_t)((u32Lba >> 16) & 0xff);
    pCdb->aCdb[4] = (uint8_t)((u32Lba >> 8) & 0xff);
    pCdb->aCdb[5] = (uint8_t)(u32Lba & 0xff);
    pCdb->aCdb[7] = (uint8_t)((u16Blocks >> 8) & 0xff);
    pCdb->aCdb[8] = (uint8_t)(u16Blocks & 0xff);
    pCdb->u8Len = 10;
}

/* ---- sense decode ------------------------------------------------------- */

int
scsi_sense_decode(const struct scsi_sense *pSense, uint8_t *pKey, uint8_t *pAsc,
                  uint8_t *pAscq)
{
    uint8_t u8Rsp;

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

/* ---- soft LUN helpers --------------------------------------------------- */

static void
soft_sense_clear(void)
{
    mid_memzero(g_aSoftUnitSense, sizeof(g_aSoftUnitSense));
    g_u8SoftUnitSenseLen = 0;
}

static void
soft_sense_set(uint8_t u8Key, uint8_t u8Asc, uint8_t u8Ascq)
{
    mid_memzero(g_aSoftUnitSense, sizeof(g_aSoftUnitSense));
    /* Fixed-format current sense (SPC). */
    g_aSoftUnitSense[0] = 0x70u;
    g_aSoftUnitSense[2] = u8Key & 0x0fu;
    g_aSoftUnitSense[7] = 10; /* additional sense length */
    g_aSoftUnitSense[12] = u8Asc;
    g_aSoftUnitSense[13] = u8Ascq;
    g_u8SoftUnitSenseLen = 18;
}

static void
soft_sense_copy_out(struct scsi_io *pIo)
{
    uint32_t cb;

    if (pIo == NULL || g_u8SoftUnitSenseLen == 0) {
        return;
    }
    cb = g_u8SoftUnitSenseLen;
    if (cb > sizeof(pIo->sense.aSense)) {
        cb = (uint32_t)sizeof(pIo->sense.aSense);
    }
    mid_memcpy(pIo->sense.aSense, g_aSoftUnitSense, cb);
    pIo->sense.u8SenseLen = (uint8_t)cb;
    pIo->sense.u8Key = g_aSoftUnitSense[2] & 0x0fu;
    pIo->sense.u8Asc = g_aSoftUnitSense[12];
    pIo->sense.u8Ascq = g_aSoftUnitSense[13];
}

static void
soft_fill_inquiry(uint8_t *pOut, size_t cb)
{
    static const char aVendor[8] = { 'G', 'r', 'e', 'e', 'n', 'J', 'a', 'd' };
    static const char aProduct[16] = {
        'S', 'O', 'F', 'T', 'L', 'U', 'N', ' ',
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '
    };
    static const char aRev[4] = { '0', '0', '0', '1' };
    size_t iByte;

    if (pOut == NULL || cb == 0) {
        return;
    }
    mid_memzero(pOut, cb);
    /* Peripheral device type: direct-access block. */
    pOut[0] = 0x00;
    pOut[2] = 0x05; /* SPC-3-ish version byte */
    pOut[3] = 0x02; /* response data format */
    pOut[4] = 31;   /* additional length for 36-byte standard INQUIRY */
    if (cb > 8) {
        size_t cbV = cb - 8;

        if (cbV > 8) {
            cbV = 8;
        }
        for (iByte = 0; iByte < cbV; iByte++) {
            pOut[8 + iByte] = (uint8_t)aVendor[iByte];
        }
    }
    if (cb > 16) {
        size_t cbP = cb - 16;

        if (cbP > 16) {
            cbP = 16;
        }
        for (iByte = 0; iByte < cbP; iByte++) {
            pOut[16 + iByte] = (uint8_t)aProduct[iByte];
        }
    }
    if (cb > 32) {
        size_t cbR = cb - 32;

        if (cbR > 4) {
            cbR = 4;
        }
        for (iByte = 0; iByte < cbR; iByte++) {
            pOut[32 + iByte] = (uint8_t)aRev[iByte];
        }
    }
}

static int
soft_lba_ok(uint32_t u32Lba, uint16_t u16Blocks)
{
    uint32_t u32End;

    if (u16Blocks == 0) {
        return 0;
    }
    if ((uint32_t)u16Blocks > SCSI_MID_SOFT_SECTORS) {
        return 0;
    }
    if (u32Lba >= SCSI_MID_SOFT_SECTORS) {
        return 0;
    }
    u32End = u32Lba + (uint32_t)u16Blocks;
    if (u32End < u32Lba || u32End > SCSI_MID_SOFT_SECTORS) {
        return 0;
    }
    return 1;
}

static void
soft_fail_check(struct scsi_io *pIo, uint8_t u8Key, uint8_t u8Asc,
                uint8_t u8Ascq)
{
    soft_sense_set(u8Key, u8Asc, u8Ascq);
    soft_sense_copy_out(pIo);
    pIo->iStatus = SCSI_STATUS_CHECK_CONDITION;
    g_u32SoftIoFail++;
}

void
scsi_mid_soft_init(void)
{
    g_fSoftArmed = 1;
    g_u32SoftIoOk = 0;
    g_u32SoftIoFail = 0;
    mid_memzero(g_aSoftDisk, sizeof(g_aSoftDisk));
    soft_sense_clear();
}

int
scsi_mid_soft_ready(void)
{
    return g_fSoftArmed ? 1 : 0;
}

int
scsi_mid_stats(struct scsi_mid_stats *pOut)
{
    if (pOut == NULL) {
        return -1;
    }
    pOut->u32IoOk = g_u32SoftIoOk;
    pOut->u32IoFail = g_u32SoftIoFail;
    pOut->u32Ready = g_fSoftArmed ? 1u : 0u;
    pOut->u32Soft = g_fSoftArmed ? 1u : 0u;
    return 0;
}

/**
 * Soft LUN command path. Returns 0 on GOOD, -1 on CHECK / unsupported.
 */
int
scsi_mid_soft_submit(struct scsi_io *pIo)
{
    uint8_t u8Op;
    size_t cbXfer;
    uint32_t u32Lba;
    uint16_t u16Blocks;
    uint32_t iSec;
    uint32_t cbCopy;

    if (pIo == NULL) {
        return -1;
    }
    if (!g_fSoftArmed) {
        /* Auto-arm so host skeleton / cold callers still get soft policy. */
        scsi_mid_soft_init();
    }

    mid_memzero(&pIo->sense, sizeof(pIo->sense));
    pIo->iStatus = -1;

    /* Soft path is single LUN 0 only. */
    if (pIo->u32Lun != 0) {
        soft_fail_check(pIo, SCSI_SK_ILLEGAL_REQUEST, 0x25u, 0x00u);
        return -1;
    }

    if (pIo->cdb.u8Len == 0) {
        soft_fail_check(pIo, SCSI_SK_ILLEGAL_REQUEST, 0x20u, 0x00u);
        return -1;
    }

    u8Op = pIo->cdb.aCdb[0];
    cbXfer = pIo->cbData;

    switch (u8Op) {
    case SCSI_OP_TEST_UNIT_READY:
        soft_sense_clear();
        pIo->iStatus = SCSI_STATUS_GOOD;
        g_u32SoftIoOk++;
        return 0;

    case SCSI_OP_REQUEST_SENSE: {
        uint32_t cb;
        uint8_t aFixed[18];

        if (cbXfer > 0 && pIo->pData == NULL) {
            g_u32SoftIoFail++;
            return -1;
        }
        if (g_u8SoftUnitSenseLen == 0) {
            mid_memzero(aFixed, sizeof(aFixed));
            aFixed[0] = 0x70u;
            aFixed[7] = 10;
            cb = (uint32_t)sizeof(aFixed);
            if (cbXfer < cb) {
                cb = (uint32_t)cbXfer;
            }
            if (cb > 0 && pIo->pData != NULL) {
                mid_memcpy(pIo->pData, aFixed, cb);
            }
        } else {
            cb = g_u8SoftUnitSenseLen;
            if (cbXfer < cb) {
                cb = (uint32_t)cbXfer;
            }
            if (cb > 0 && pIo->pData != NULL) {
                mid_memcpy(pIo->pData, g_aSoftUnitSense, cb);
            }
            soft_sense_clear(); /* auto-clear after successful REQUEST SENSE */
        }
        pIo->iStatus = SCSI_STATUS_GOOD;
        g_u32SoftIoOk++;
        return 0;
    }

    case SCSI_OP_INQUIRY:
        if (cbXfer > 0 && pIo->pData == NULL) {
            g_u32SoftIoFail++;
            return -1;
        }
        /* EVPD / VPD pages: soft path only serves standard INQUIRY. */
        if ((pIo->cdb.aCdb[1] & 0x01u) != 0) {
            soft_fail_check(pIo, SCSI_SK_ILLEGAL_REQUEST, 0x24u, 0x00u);
            return -1;
        }
        if (cbXfer > 0 && pIo->pData != NULL) {
            soft_fill_inquiry((uint8_t *)pIo->pData, cbXfer);
        }
        soft_sense_clear();
        pIo->iStatus = SCSI_STATUS_GOOD;
        g_u32SoftIoOk++;
        return 0;

    case SCSI_OP_MODE_SENSE_6:
        if (cbXfer > 0 && pIo->pData == NULL) {
            g_u32SoftIoFail++;
            return -1;
        }
        if (cbXfer > 0 && pIo->pData != NULL) {
            uint8_t *pOut = (uint8_t *)pIo->pData;

            mid_memzero(pOut, cbXfer);
            if (cbXfer >= 4) {
                pOut[0] = 3; /* mode data length excluding itself */
                pOut[1] = 0;
                pOut[2] = 0;
                pOut[3] = 0;
            }
        }
        soft_sense_clear();
        pIo->iStatus = SCSI_STATUS_GOOD;
        g_u32SoftIoOk++;
        return 0;

    case SCSI_OP_READ_CAPACITY_10:
        if (cbXfer > 0 && pIo->pData == NULL) {
            g_u32SoftIoFail++;
            return -1;
        }
        if (cbXfer >= 8 && pIo->pData != NULL) {
            uint8_t *pOut = (uint8_t *)pIo->pData;
            uint32_t u32Last = SCSI_MID_SOFT_SECTORS - 1u;
            uint32_t u32Blen = SCSI_MID_SOFT_SEC_SIZE;

            pOut[0] = (uint8_t)((u32Last >> 24) & 0xff);
            pOut[1] = (uint8_t)((u32Last >> 16) & 0xff);
            pOut[2] = (uint8_t)((u32Last >> 8) & 0xff);
            pOut[3] = (uint8_t)(u32Last & 0xff);
            pOut[4] = (uint8_t)((u32Blen >> 24) & 0xff);
            pOut[5] = (uint8_t)((u32Blen >> 16) & 0xff);
            pOut[6] = (uint8_t)((u32Blen >> 8) & 0xff);
            pOut[7] = (uint8_t)(u32Blen & 0xff);
        }
        soft_sense_clear();
        pIo->iStatus = SCSI_STATUS_GOOD;
        g_u32SoftIoOk++;
        return 0;

    case SCSI_OP_READ_10:
    case SCSI_OP_WRITE_10:
        u32Lba = ((uint32_t)pIo->cdb.aCdb[2] << 24) |
                 ((uint32_t)pIo->cdb.aCdb[3] << 16) |
                 ((uint32_t)pIo->cdb.aCdb[4] << 8) |
                 (uint32_t)pIo->cdb.aCdb[5];
        u16Blocks = (uint16_t)(((uint16_t)pIo->cdb.aCdb[7] << 8) |
                               (uint16_t)pIo->cdb.aCdb[8]);
        if (u16Blocks == 0) {
            u16Blocks = 1;
        }
        if (!soft_lba_ok(u32Lba, u16Blocks)) {
            soft_fail_check(pIo, SCSI_SK_ILLEGAL_REQUEST, 0x21u, 0x00u);
            return -1;
        }
        cbCopy = (uint32_t)u16Blocks * SCSI_MID_SOFT_SEC_SIZE;
        if (cbXfer > 0 && pIo->pData == NULL) {
            g_u32SoftIoFail++;
            return -1;
        }
        if (cbXfer < cbCopy) {
            cbCopy = (uint32_t)cbXfer -
                     ((uint32_t)cbXfer % SCSI_MID_SOFT_SEC_SIZE);
            if (cbCopy == 0 && cbXfer > 0) {
                cbCopy = (uint32_t)cbXfer;
            }
        }
        if (u8Op == SCSI_OP_READ_10) {
            uint32_t cbLeft = cbCopy;
            uint8_t *pDst = (uint8_t *)pIo->pData;

            for (iSec = 0; iSec < (uint32_t)u16Blocks && cbLeft > 0; iSec++) {
                uint32_t cb = cbLeft > SCSI_MID_SOFT_SEC_SIZE
                                  ? SCSI_MID_SOFT_SEC_SIZE
                                  : cbLeft;

                if (pDst != NULL) {
                    mid_memcpy(pDst, g_aSoftDisk[u32Lba + iSec], cb);
                    pDst += cb;
                }
                cbLeft -= cb;
            }
        } else {
            uint32_t cbLeft = cbCopy;
            const uint8_t *pSrc = (const uint8_t *)pIo->pData;

            for (iSec = 0; iSec < (uint32_t)u16Blocks && cbLeft > 0; iSec++) {
                uint32_t cb = cbLeft > SCSI_MID_SOFT_SEC_SIZE
                                  ? SCSI_MID_SOFT_SEC_SIZE
                                  : cbLeft;

                if (pSrc != NULL) {
                    mid_memcpy(g_aSoftDisk[u32Lba + iSec], pSrc, cb);
                    if (cb < SCSI_MID_SOFT_SEC_SIZE) {
                        mid_memzero(g_aSoftDisk[u32Lba + iSec] + cb,
                                    SCSI_MID_SOFT_SEC_SIZE - cb);
                    }
                    pSrc += cb;
                }
                cbLeft -= cb;
            }
        }
        soft_sense_clear();
        pIo->iStatus = SCSI_STATUS_GOOD;
        g_u32SoftIoOk++;
        return 0;

    case SCSI_OP_SYNCHRONIZE_CACHE:
        /* Soft disk is always coherent; treat as success. */
        soft_sense_clear();
        pIo->iStatus = SCSI_STATUS_GOOD;
        g_u32SoftIoOk++;
        return 0;

    default:
        soft_fail_check(pIo, SCSI_SK_ILLEGAL_REQUEST, 0x20u, 0x00u);
        return -1;
    }
}

/* ---- door path (freestanding) + public submit --------------------------- */

#if SCSI_HAS_SYS
static int
door_submit(struct scsi_io *pIo)
{
    long nRet;
    unsigned uOp;
    uint32_t u32Blocks;
    uint32_t u32Lba;

    if (pIo->cdb.u8Len == 0) {
        pIo->iStatus = -1;
        return -1;
    }
    switch (pIo->cdb.aCdb[0]) {
    case SCSI_OP_INQUIRY:
        nRet = gj_scsi(GJ_SCSI_OP_INQUIRY, (long)(uintptr_t)pIo->pData,
                       (long)(pIo->cbData ? pIo->cbData : 36), 0);
        if (nRet < 0) {
            pIo->iStatus = -1;
            return -1;
        }
        pIo->iStatus = 0;
        return 0;
    case SCSI_OP_READ_CAPACITY_10: {
        unsigned aCap[2];

        nRet = gj_scsi(GJ_SCSI_OP_READ_CAP, (long)(uintptr_t)aCap, 0, 0);
        if (nRet < 0) {
            pIo->iStatus = -1;
            return -1;
        }
        /* SBC READ CAPACITY (10) data: 4B last LBA + 4B block length (BE). */
        if (pIo->pData != NULL && pIo->cbData >= 8) {
            uint8_t *pOut = (uint8_t *)pIo->pData;

            pOut[0] = (uint8_t)((aCap[0] >> 24) & 0xff);
            pOut[1] = (uint8_t)((aCap[0] >> 16) & 0xff);
            pOut[2] = (uint8_t)((aCap[0] >> 8) & 0xff);
            pOut[3] = (uint8_t)(aCap[0] & 0xff);
            pOut[4] = (uint8_t)((aCap[1] >> 24) & 0xff);
            pOut[5] = (uint8_t)((aCap[1] >> 16) & 0xff);
            pOut[6] = (uint8_t)((aCap[1] >> 8) & 0xff);
            pOut[7] = (uint8_t)(aCap[1] & 0xff);
        }
        pIo->iStatus = 0;
        return 0;
    }
    case SCSI_OP_READ_10:
    case SCSI_OP_WRITE_10:
        u32Lba = ((uint32_t)pIo->cdb.aCdb[2] << 24) |
                 ((uint32_t)pIo->cdb.aCdb[3] << 16) |
                 ((uint32_t)pIo->cdb.aCdb[4] << 8) |
                 (uint32_t)pIo->cdb.aCdb[5];
        u32Blocks = ((uint32_t)pIo->cdb.aCdb[7] << 8) |
                    (uint32_t)pIo->cdb.aCdb[8];
        if (u32Blocks == 0) {
            u32Blocks = 1;
        }
        uOp = (pIo->cdb.aCdb[0] == SCSI_OP_WRITE_10) ? GJ_SCSI_OP_WRITE10
                                                     : GJ_SCSI_OP_READ10;
        nRet = gj_scsi(uOp, (long)u32Lba, (long)(uintptr_t)pIo->pData,
                       (long)u32Blocks);
        if (nRet < 0) {
            pIo->iStatus = -1;
            return -1;
        }
        pIo->iStatus = 0;
        return 0;
    default:
        /* Door surface is a subset; fall soft for TUR/SENSE/MODE/SYNC. */
        return scsi_mid_soft_submit(pIo);
    }
}
#endif /* SCSI_HAS_SYS */

/*
 * Submit packed I/O.
 *
 * Freestanding: prefer door when READY; else soft LUN.
 * Host:         soft LUN (auto-armed).
 *
 * Returns 0 on success, -1 on failure / unsupported.
 */
int
scsi_mid_submit(struct scsi_io *pIo)
{
    if (pIo == NULL) {
        return -1;
    }
#if SCSI_HAS_SYS
    {
        long nReady;

        nReady = gj_scsi(GJ_SCSI_OP_READY, 0, 0, 0);
        if (nReady == 1) {
            return door_submit(pIo);
        }
        /* No HBA / door not ready — product soft mid owns the I/O. */
        return scsi_mid_soft_submit(pIo);
    }
#else
    return scsi_mid_soft_submit(pIo);
#endif
}
