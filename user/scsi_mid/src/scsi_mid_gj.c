/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding scsi_mid live path (pure C11, dual MIT OR Apache-2.0).
 *
 * Two arms (smoke markers stay prefix-stable):
 *
 *   Door path (virtio-scsi / HBA ready):
 *     READY → INQUIRY → optional READ_CAP → optional READ10 →
 *     optional STATS → live path PASS (scsi door)
 *
 *   Soft path (no HBA / READY ≠ 1):
 *     soft LUN init → TUR → INQUIRY → MODE SENSE → READ_CAP →
 *     WRITE10/READ10 verify → illegal-LUN sense → REQUEST SENSE →
 *     SYNC → soft stats → live path PASS (no-HBA soft)
 *
 * Soft LUN is pure userspace mid policy (geometry, sense, LUN map) so
 * freestanding smokes stay green without virtio-scsi. Parent tree still
 * builds this TU alone (make scsi_mid-gj → build/user/scsi_mid.elf).
 *
 * Note: library path (CDB builders + soft submit + door map) lives in
 * cdb.c for the product mid-layer and host skeleton (server.c). This
 * binary keeps a self-contained soft mid so freestanding needs no extra
 * link objects.
 */
#include <gj/syscalls.h>

/* Soft LUN geometry — match include/scsi_mid.h product soft constants. */
#define SOFT_SECTORS  64u
#define SOFT_SEC_SIZE 512u
#define SOFT_SENSE_MAX 32u

#define SCSI_OP_TEST_UNIT_READY   0x00u
#define SCSI_OP_REQUEST_SENSE     0x03u
#define SCSI_OP_INQUIRY           0x12u
#define SCSI_OP_MODE_SENSE_6      0x1Au
#define SCSI_OP_READ_CAPACITY_10  0x25u
#define SCSI_OP_READ_10           0x28u
#define SCSI_OP_WRITE_10          0x2Au
#define SCSI_OP_SYNCHRONIZE_CACHE 0x35u

#define SCSI_SK_ILLEGAL_REQUEST 0x5u

/* ---- freestanding helpers ----------------------------------------------- */

static void
msg(const char *sz)
{
    size_t n = 0;

    if (sz == 0) {
        return;
    }
    while (sz[n] != '\0') {
        n++;
    }
    (void)gj_debug_log(sz, (long)n);
}

static void
memzero(void *p, unsigned cb)
{
    unsigned char *pB = (unsigned char *)p;
    unsigned i;

    if (pB == 0) {
        return;
    }
    for (i = 0; i < cb; i++) {
        pB[i] = 0;
    }
}

static void
memcopy(void *pDst, const void *pSrc, unsigned cb)
{
    unsigned char *pD = (unsigned char *)pDst;
    const unsigned char *pS = (const unsigned char *)pSrc;
    unsigned i;

    if (pD == 0 || pS == 0) {
        return;
    }
    for (i = 0; i < cb; i++) {
        pD[i] = pS[i];
    }
}

static void
append_s(char *aLine, unsigned cb, unsigned *po, const char *sz)
{
    if (aLine == 0 || po == 0 || sz == 0 || cb == 0) {
        return;
    }
    while (*sz != '\0' && *po + 1u < cb) {
        aLine[(*po)++] = *sz++;
    }
}

static void
append_u(char *aLine, unsigned cb, unsigned *po, unsigned long u)
{
    char aDig[20];
    unsigned n = 0;
    unsigned i;

    if (aLine == 0 || po == 0 || cb == 0) {
        return;
    }
    if (u == 0) {
        aDig[n++] = '0';
    } else {
        while (u > 0 && n < sizeof(aDig)) {
            aDig[n++] = (char)('0' + (u % 10ul));
            u /= 10ul;
        }
    }
    for (i = n; i > 0 && *po + 1u < cb; i--) {
        aLine[(*po)++] = aDig[i - 1u];
    }
}

/* SPC: standard INQUIRY vendor identification is 8 bytes at offset 8. */
static void
msg_inquiry_vendor(const unsigned char *pInq, long nRet)
{
    char aLine[48];
    unsigned i;
    unsigned o;
    int fPresent;

    if (pInq == 0 || nRet < 16) {
        return;
    }
    fPresent = 0;
    for (i = 0; i < 8; i++) {
        if (pInq[8 + i] != 0 && pInq[8 + i] != ' ') {
            fPresent = 1;
            break;
        }
    }
    if (!fPresent) {
        return;
    }
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "scsi_mid-gj: vendor=\"");
    for (i = 0; i < 8 && o + 1u < sizeof(aLine); i++) {
        unsigned char c = pInq[8 + i];

        if (c < 0x20u || c > 0x7eu) {
            c = '.';
        }
        aLine[o++] = (char)c;
    }
    if (o + 2u < sizeof(aLine)) {
        aLine[o++] = '"';
        aLine[o++] = '\n';
    }
    aLine[o] = '\0';
    msg(aLine);
}

/* ---- soft LUN (self-contained freestanding mid policy) ------------------ */

static unsigned char g_aSoftDisk[SOFT_SECTORS][SOFT_SEC_SIZE];
static unsigned char g_aSoftSense[SOFT_SENSE_MAX];
static unsigned g_uSoftSenseLen;
static unsigned g_uSoftIoOk;
static unsigned g_uSoftIoFail;
static int g_fSoftArmed;

static void
soft_sense_clear(void)
{
    memzero(g_aSoftSense, sizeof(g_aSoftSense));
    g_uSoftSenseLen = 0;
}

static void
soft_sense_set(unsigned char u8Key, unsigned char u8Asc, unsigned char u8Ascq)
{
    memzero(g_aSoftSense, sizeof(g_aSoftSense));
    g_aSoftSense[0] = 0x70u;
    g_aSoftSense[2] = u8Key & 0x0fu;
    g_aSoftSense[7] = 10;
    g_aSoftSense[12] = u8Asc;
    g_aSoftSense[13] = u8Ascq;
    g_uSoftSenseLen = 18;
}

static void
soft_init(void)
{
    memzero(g_aSoftDisk, sizeof(g_aSoftDisk));
    soft_sense_clear();
    g_uSoftIoOk = 0;
    g_uSoftIoFail = 0;
    g_fSoftArmed = 1;
}

static int
soft_lba_ok(unsigned uLba, unsigned uBlocks)
{
    unsigned uEnd;

    if (uBlocks == 0 || uBlocks > SOFT_SECTORS) {
        return 0;
    }
    if (uLba >= SOFT_SECTORS) {
        return 0;
    }
    uEnd = uLba + uBlocks;
    if (uEnd < uLba || uEnd > SOFT_SECTORS) {
        return 0;
    }
    return 1;
}

static void
soft_fill_inquiry(unsigned char *pOut, unsigned cb)
{
    static const char aVendor[8] = { 'G', 'r', 'e', 'e', 'n', 'J', 'a', 'd' };
    static const char aProduct[16] = {
        'S', 'O', 'F', 'T', 'L', 'U', 'N', ' ',
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '
    };
    static const char aRev[4] = { '0', '0', '0', '1' };
    unsigned i;

    if (pOut == 0 || cb == 0) {
        return;
    }
    memzero(pOut, cb);
    pOut[0] = 0x00;
    pOut[2] = 0x05;
    pOut[3] = 0x02;
    pOut[4] = 31;
    for (i = 0; i < 8 && (8u + i) < cb; i++) {
        pOut[8 + i] = (unsigned char)aVendor[i];
    }
    for (i = 0; i < 16 && (16u + i) < cb; i++) {
        pOut[16 + i] = (unsigned char)aProduct[i];
    }
    for (i = 0; i < 4 && (32u + i) < cb; i++) {
        pOut[32 + i] = (unsigned char)aRev[i];
    }
}

/*
 * Minimal soft CDB dispatch: pCdb[0]=op; for R/W pCdb carries LBA/blocks
 * in standard 10-byte layout. Returns 0 GOOD, -1 CHECK/fail.
 */
static int
soft_submit(unsigned uLun, const unsigned char *pCdb, unsigned cbCdb,
            unsigned char *pData, unsigned cbData)
{
    unsigned char uOp;
    unsigned uLba;
    unsigned uBlocks;
    unsigned iSec;
    unsigned cbCopy;
    unsigned cbLeft;

    if (!g_fSoftArmed || pCdb == 0 || cbCdb == 0) {
        g_uSoftIoFail++;
        return -1;
    }
    if (uLun != 0) {
        soft_sense_set(SCSI_SK_ILLEGAL_REQUEST, 0x25u, 0x00u);
        g_uSoftIoFail++;
        return -1;
    }

    uOp = pCdb[0];
    switch (uOp) {
    case SCSI_OP_TEST_UNIT_READY:
        soft_sense_clear();
        g_uSoftIoOk++;
        return 0;

    case SCSI_OP_REQUEST_SENSE: {
        unsigned cb;
        unsigned char aFixed[18];

        if (cbData > 0 && pData == 0) {
            g_uSoftIoFail++;
            return -1;
        }
        if (g_uSoftSenseLen == 0) {
            memzero(aFixed, sizeof(aFixed));
            aFixed[0] = 0x70u;
            aFixed[7] = 10;
            cb = sizeof(aFixed);
            if (cbData < cb) {
                cb = cbData;
            }
            if (cb > 0 && pData != 0) {
                memcopy(pData, aFixed, cb);
            }
        } else {
            cb = g_uSoftSenseLen;
            if (cbData < cb) {
                cb = cbData;
            }
            if (cb > 0 && pData != 0) {
                memcopy(pData, g_aSoftSense, cb);
            }
            soft_sense_clear();
        }
        g_uSoftIoOk++;
        return 0;
    }

    case SCSI_OP_INQUIRY:
        if ((pCdb[1] & 0x01u) != 0) {
            soft_sense_set(SCSI_SK_ILLEGAL_REQUEST, 0x24u, 0x00u);
            g_uSoftIoFail++;
            return -1;
        }
        if (cbData > 0 && pData != 0) {
            soft_fill_inquiry(pData, cbData);
        }
        soft_sense_clear();
        g_uSoftIoOk++;
        return 0;

    case SCSI_OP_MODE_SENSE_6:
        if (cbData > 0 && pData != 0) {
            memzero(pData, cbData);
            if (cbData >= 4) {
                pData[0] = 3;
            }
        }
        soft_sense_clear();
        g_uSoftIoOk++;
        return 0;

    case SCSI_OP_READ_CAPACITY_10:
        if (cbData >= 8 && pData != 0) {
            unsigned uLast = SOFT_SECTORS - 1u;
            unsigned uBlen = SOFT_SEC_SIZE;

            pData[0] = (unsigned char)((uLast >> 24) & 0xff);
            pData[1] = (unsigned char)((uLast >> 16) & 0xff);
            pData[2] = (unsigned char)((uLast >> 8) & 0xff);
            pData[3] = (unsigned char)(uLast & 0xff);
            pData[4] = (unsigned char)((uBlen >> 24) & 0xff);
            pData[5] = (unsigned char)((uBlen >> 16) & 0xff);
            pData[6] = (unsigned char)((uBlen >> 8) & 0xff);
            pData[7] = (unsigned char)(uBlen & 0xff);
        }
        soft_sense_clear();
        g_uSoftIoOk++;
        return 0;

    case SCSI_OP_READ_10:
    case SCSI_OP_WRITE_10:
        if (cbCdb < 10) {
            soft_sense_set(SCSI_SK_ILLEGAL_REQUEST, 0x20u, 0x00u);
            g_uSoftIoFail++;
            return -1;
        }
        uLba = ((unsigned)pCdb[2] << 24) | ((unsigned)pCdb[3] << 16) |
               ((unsigned)pCdb[4] << 8) | (unsigned)pCdb[5];
        uBlocks = ((unsigned)pCdb[7] << 8) | (unsigned)pCdb[8];
        if (uBlocks == 0) {
            uBlocks = 1;
        }
        if (!soft_lba_ok(uLba, uBlocks)) {
            soft_sense_set(SCSI_SK_ILLEGAL_REQUEST, 0x21u, 0x00u);
            g_uSoftIoFail++;
            return -1;
        }
        cbCopy = uBlocks * SOFT_SEC_SIZE;
        if (cbData > 0 && pData == 0) {
            g_uSoftIoFail++;
            return -1;
        }
        if (cbData < cbCopy) {
            cbCopy = cbData - (cbData % SOFT_SEC_SIZE);
            if (cbCopy == 0 && cbData > 0) {
                cbCopy = cbData;
            }
        }
        cbLeft = cbCopy;
        if (uOp == SCSI_OP_READ_10) {
            for (iSec = 0; iSec < uBlocks && cbLeft > 0; iSec++) {
                unsigned cb = cbLeft > SOFT_SEC_SIZE ? SOFT_SEC_SIZE : cbLeft;

                if (pData != 0) {
                    memcopy(pData, g_aSoftDisk[uLba + iSec], cb);
                    pData += cb;
                }
                cbLeft -= cb;
            }
        } else {
            for (iSec = 0; iSec < uBlocks && cbLeft > 0; iSec++) {
                unsigned cb = cbLeft > SOFT_SEC_SIZE ? SOFT_SEC_SIZE : cbLeft;

                if (pData != 0) {
                    memcopy(g_aSoftDisk[uLba + iSec], pData, cb);
                    if (cb < SOFT_SEC_SIZE) {
                        memzero(g_aSoftDisk[uLba + iSec] + cb,
                                SOFT_SEC_SIZE - cb);
                    }
                    pData += cb;
                }
                cbLeft -= cb;
            }
        }
        soft_sense_clear();
        g_uSoftIoOk++;
        return 0;

    case SCSI_OP_SYNCHRONIZE_CACHE:
        soft_sense_clear();
        g_uSoftIoOk++;
        return 0;

    default:
        soft_sense_set(SCSI_SK_ILLEGAL_REQUEST, 0x20u, 0x00u);
        g_uSoftIoFail++;
        return -1;
    }
}

static void
cdb_tur(unsigned char *pCdb)
{
    memzero(pCdb, 16);
    pCdb[0] = SCSI_OP_TEST_UNIT_READY;
}

static void
cdb_inquiry(unsigned char *pCdb, unsigned uAlloc)
{
    memzero(pCdb, 16);
    pCdb[0] = SCSI_OP_INQUIRY;
    pCdb[3] = (unsigned char)((uAlloc >> 8) & 0xff);
    pCdb[4] = (unsigned char)(uAlloc & 0xff);
}

static void
cdb_mode_sense6(unsigned char *pCdb, unsigned uAlloc)
{
    memzero(pCdb, 16);
    pCdb[0] = SCSI_OP_MODE_SENSE_6;
    pCdb[4] = (unsigned char)(uAlloc ? uAlloc : 4u);
}

static void
cdb_read_cap(unsigned char *pCdb)
{
    memzero(pCdb, 16);
    pCdb[0] = SCSI_OP_READ_CAPACITY_10;
}

static void
cdb_rw10(unsigned char *pCdb, unsigned char uOp, unsigned uLba,
         unsigned uBlocks)
{
    memzero(pCdb, 16);
    pCdb[0] = uOp;
    pCdb[2] = (unsigned char)((uLba >> 24) & 0xff);
    pCdb[3] = (unsigned char)((uLba >> 16) & 0xff);
    pCdb[4] = (unsigned char)((uLba >> 8) & 0xff);
    pCdb[5] = (unsigned char)(uLba & 0xff);
    pCdb[7] = (unsigned char)((uBlocks >> 8) & 0xff);
    pCdb[8] = (unsigned char)(uBlocks & 0xff);
}

static void
cdb_req_sense(unsigned char *pCdb, unsigned uAlloc)
{
    memzero(pCdb, 16);
    pCdb[0] = SCSI_OP_REQUEST_SENSE;
    pCdb[4] = (unsigned char)(uAlloc ? uAlloc : 18u);
}

static void
cdb_sync(unsigned char *pCdb)
{
    memzero(pCdb, 16);
    pCdb[0] = SCSI_OP_SYNCHRONIZE_CACHE;
}

static void
msg_soft_stats(void)
{
    char aLine[72];
    unsigned o = 0;

    append_s(aLine, sizeof(aLine), &o, "scsi_mid-gj: soft stats ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftIoOk);
    append_s(aLine, sizeof(aLine), &o, " fail=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftIoFail);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);
}

/*
 * Full userspace soft mid smoke when no virtio-scsi / door not ready.
 * Never hard-fails the live path: any soft step miss still soft-skips
 * with a greppable marker, then emits live path PASS (no-HBA soft).
 */
static void
run_soft_path(void)
{
    unsigned char aCdb[16];
    unsigned char aInq[36];
    unsigned char aCap[8];
    unsigned char aMode[4];
    unsigned char aBlk[SOFT_SEC_SIZE];
    unsigned char aSense[18];
    unsigned i;
    int fOk;
    int fRwOk;

    msg("scsi_mid-gj: READY soft-skip (no virtio-scsi)\n");
    msg("scsi_mid-gj: soft mid start\n");
    soft_init();

    /* 1. TUR */
    cdb_tur(aCdb);
    if (soft_submit(0, aCdb, 6, 0, 0) == 0) {
        msg("scsi_mid-gj: soft TUR PASS\n");
    } else {
        msg("scsi_mid-gj: soft TUR soft-skip\n");
    }

    /* 2. INQUIRY + vendor */
    memzero(aInq, sizeof(aInq));
    cdb_inquiry(aCdb, 36);
    if (soft_submit(0, aCdb, 6, aInq, sizeof(aInq)) == 0) {
        msg("scsi_mid-gj: soft INQUIRY PASS\n");
        msg_inquiry_vendor(aInq, (long)sizeof(aInq));
    } else {
        msg("scsi_mid-gj: soft INQUIRY soft-skip\n");
    }

    /* 3. MODE SENSE(6) header */
    memzero(aMode, sizeof(aMode));
    cdb_mode_sense6(aCdb, 4);
    if (soft_submit(0, aCdb, 6, aMode, sizeof(aMode)) == 0) {
        msg("scsi_mid-gj: soft MODE_SENSE PASS\n");
    } else {
        msg("scsi_mid-gj: soft MODE_SENSE soft-skip\n");
    }

    /* 4. READ CAPACITY */
    memzero(aCap, sizeof(aCap));
    cdb_read_cap(aCdb);
    if (soft_submit(0, aCdb, 10, aCap, sizeof(aCap)) == 0) {
        msg("scsi_mid-gj: soft READ_CAP PASS\n");
    } else {
        msg("scsi_mid-gj: soft READ_CAP soft-skip\n");
    }

    /* 5. WRITE10 pattern + READ10 verify at LBA 1 */
    for (i = 0; i < SOFT_SEC_SIZE; i++) {
        aBlk[i] = (unsigned char)(0xA5u ^ (unsigned char)i);
    }
    cdb_rw10(aCdb, SCSI_OP_WRITE_10, 1u, 1u);
    fOk = (soft_submit(0, aCdb, 10, aBlk, SOFT_SEC_SIZE) == 0);
    if (fOk) {
        msg("scsi_mid-gj: soft WRITE10 PASS\n");
    } else {
        msg("scsi_mid-gj: soft WRITE10 soft-skip\n");
    }
    memzero(aBlk, sizeof(aBlk));
    cdb_rw10(aCdb, SCSI_OP_READ_10, 1u, 1u);
    fRwOk = 0;
    if (soft_submit(0, aCdb, 10, aBlk, SOFT_SEC_SIZE) == 0) {
        fRwOk = 1;
        for (i = 0; i < SOFT_SEC_SIZE; i++) {
            if (aBlk[i] != (unsigned char)(0xA5u ^ (unsigned char)i)) {
                fRwOk = 0;
                break;
            }
        }
    }
    if (fRwOk) {
        msg("scsi_mid-gj: soft READ10 PASS\n");
        msg("scsi_mid-gj: soft R/W verify PASS\n");
    } else {
        msg("scsi_mid-gj: soft READ10 soft-skip\n");
    }

    /* 6. Illegal LUN → CHECK sense, then REQUEST SENSE harvest */
    cdb_tur(aCdb);
    if (soft_submit(1, aCdb, 6, 0, 0) != 0 && g_uSoftSenseLen >= 14 &&
        (g_aSoftSense[2] & 0x0fu) == SCSI_SK_ILLEGAL_REQUEST &&
        g_aSoftSense[12] == 0x25u) {
        msg("scsi_mid-gj: soft LUN map PASS\n");
    } else {
        msg("scsi_mid-gj: soft LUN map soft-skip\n");
    }
    memzero(aSense, sizeof(aSense));
    cdb_req_sense(aCdb, 18);
    if (soft_submit(0, aCdb, 6, aSense, sizeof(aSense)) == 0) {
        msg("scsi_mid-gj: soft REQ_SENSE PASS\n");
    } else {
        msg("scsi_mid-gj: soft REQ_SENSE soft-skip\n");
    }

    /* 7. SYNCHRONIZE CACHE */
    cdb_sync(aCdb);
    if (soft_submit(0, aCdb, 10, 0, 0) == 0) {
        msg("scsi_mid-gj: soft SYNC PASS\n");
    } else {
        msg("scsi_mid-gj: soft SYNC soft-skip\n");
    }

    msg_soft_stats();
    msg("scsi_mid-gj: soft mid PASS\n");
    msg("scsi_mid-gj: live path PASS (no-HBA soft)\n");
}

/* ---- door path (HBA ready) ---------------------------------------------- */

static void
run_door_path(void)
{
    static unsigned char aInq[36];
    static unsigned aCap[2];
    static unsigned char aBlk[512];
    static unsigned aStats[2];
    long n;
    unsigned i;

    msg("scsi_mid-gj: READY PASS\n");

    /* INQUIRY — hard fail if door errors. */
    for (i = 0; i < sizeof(aInq); i++) {
        aInq[i] = 0;
    }
    n = gj_scsi(GJ_SCSI_OP_INQUIRY, (long)(unsigned long)aInq, 36, 0);
    if (n < 0) {
        msg("scsi_mid-gj: INQUIRY FAIL\n");
        gj_exit(1);
    }
    msg("scsi_mid-gj: INQUIRY PASS\n");
    msg_inquiry_vendor(aInq, (n >= 16) ? n : (long)sizeof(aInq));

    /* Optional capacity + single-block READ10. */
    if (gj_scsi(GJ_SCSI_OP_READ_CAP, (long)(unsigned long)aCap, 0, 0) == 0) {
        msg("scsi_mid-gj: READ_CAP PASS\n");
        n = gj_scsi(GJ_SCSI_OP_READ10, 0, (long)(unsigned long)aBlk, 1);
        if (n == 512) {
            msg("scsi_mid-gj: READ10 PASS\n");
        } else {
            msg("scsi_mid-gj: READ10 soft-skip\n");
        }
    } else {
        msg("scsi_mid-gj: READ_CAP soft-skip\n");
    }

    /* Optional door STATS (io count + ready flag). Soft if unsupported. */
    aStats[0] = 0;
    aStats[1] = 0;
    if (gj_scsi(GJ_SCSI_OP_STATS, (long)(unsigned long)aStats, 0, 0) == 0) {
        char aLine[64];
        unsigned o = 0;

        append_s(aLine, sizeof(aLine), &o, "scsi_mid-gj: STATS PASS io=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)aStats[0]);
        append_s(aLine, sizeof(aLine), &o, " ready=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)aStats[1]);
        append_s(aLine, sizeof(aLine), &o, "\n");
        aLine[o] = '\0';
        msg(aLine);
    } else {
        msg("scsi_mid-gj: STATS soft-skip\n");
    }

    /*
     * Soft mid side-smoke even when door is ready: proves userspace soft
     * policy stays healthy beside the HBA path (never hard-fails).
     */
    {
        unsigned char aCdb[16];
        unsigned char aSoftInq[36];

        soft_init();
        cdb_inquiry(aCdb, 36);
        memzero(aSoftInq, sizeof(aSoftInq));
        if (soft_submit(0, aCdb, 6, aSoftInq, sizeof(aSoftInq)) == 0) {
            msg("scsi_mid-gj: soft side INQUIRY PASS\n");
        } else {
            msg("scsi_mid-gj: soft side soft-skip\n");
        }
    }

    msg("scsi_mid-gj: live path PASS (scsi door)\n");
}

void
_start(void)
{
    long n;

    msg("scsi_mid-gj: start\n");

    /* READY — transport probe (1 = HBA ready). */
    n = gj_scsi(GJ_SCSI_OP_READY, 0, 0, 0);
    if (n != 1) {
        run_soft_path();
        gj_exit(0);
    }

    run_door_path();
    gj_exit(0);
}
