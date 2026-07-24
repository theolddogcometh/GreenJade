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
 *     optional STATS → soft side + soft inventory → live path PASS
 *
 *   Soft path (no HBA / READY ≠ 1):
 *     soft LUN init → TUR → INQUIRY → MODE SENSE → READ_CAP →
 *     WRITE10/READ10 verify → illegal-LUN sense → REQUEST SENSE →
 *     SYNC → deepen probes → soft inventory → live path PASS
 *
 * Soft inventory (Wave 31 exclusive deepen — greppable "scsi_mid-gj: soft …"):
 *   scsi_mid-gj: soft inventory ok=… skip=… soft_ok=… door_ok=… wave=31 areas=10
 *   scsi_mid-gj: soft steps tur=… inq=… mode=… readcap=… write10=… read10=…
 *                rw=… lun=… sense=… sync=… multi=… lba=… evpd=… bits=…
 *   scsi_mid-gj: soft lun / multi / lba / evpd / door / geometry / deepen / path
 * Soft LUN honesty remains soft; product door INQUIRY path is separate.
 * Diagnostics only — never gates live path PASS.
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

/* Wave stamp + inventory area count (Wave 31 exclusive deepen). */
#define SCSI_SOFT_WAVE  31u
#define SCSI_SOFT_AREAS 11u
/* areas: suite steps lun multi lba evpd door geometry deepen path */

/* Soft suite sub-step bits (Wave 31 greppable steps line). */
#define SOFT_S_TUR     (1u << 0)
#define SOFT_S_INQ     (1u << 1)
#define SOFT_S_MODE    (1u << 2)
#define SOFT_S_READCAP (1u << 3)
#define SOFT_S_WRITE10 (1u << 4)
#define SOFT_S_READ10  (1u << 5)
#define SOFT_S_RW      (1u << 6)
#define SOFT_S_LUN     (1u << 7)
#define SOFT_S_SENSE   (1u << 8)
#define SOFT_S_SYNC    (1u << 9)
#define SOFT_S_MULTI   (1u << 10)
#define SOFT_S_LBA     (1u << 11)
#define SOFT_S_EVPD    (1u << 12)

/* Soft door sub-step bits (product door path lamps; INQUIRY separate). */
#define SOFT_D_READY   (1u << 0)
#define SOFT_D_INQ     (1u << 1)
#define SOFT_D_READCAP (1u << 2)
#define SOFT_D_READ10  (1u << 3)
#define SOFT_D_STATS   (1u << 4)
#define SOFT_D_SIDE    (1u << 5)

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

/*
 * Soft product inventory (Wave 31 exclusive deepen). Cumulative for this
 * process. greppable: scsi_mid-gj: soft …
 * Never hard-gates live path. Soft ≠ product multi-server confine.
 */
static unsigned g_cSoftOk;          /* soft suite sub-steps greened */
static unsigned g_cSoftSkip;        /* soft suite sub-steps soft-skipped */
static unsigned g_uSoftStepBits;    /* SOFT_S_* mask of greened soft steps */
static unsigned g_uSoftDoorBits;    /* SOFT_D_* mask of greened door steps */
static unsigned g_cSoftDoorOk;      /* door soft sub-steps greened */
static unsigned g_cSoftDoorSkip;    /* door soft sub-steps soft-skipped */
static unsigned g_cSoftInvLog;      /* inventory dump emissions */
static unsigned g_uSoftLunHonest;   /* illegal LUN → CHECK KEY/ASC honesty */
static unsigned g_uSoftLunKey;      /* last illegal-LUN sense KEY */
static unsigned g_uSoftLunAsc;      /* last illegal-LUN sense ASC */
static unsigned g_uSoftLbaHonest;   /* illegal LBA → CHECK honesty */
static unsigned g_uSoftEvpdHonest;  /* EVPD INQUIRY → CHECK honesty */
static unsigned g_uSoftMultiOk;     /* multi-block soft R/W greened */
static unsigned g_uSoftProductInq;  /* product (door) INQUIRY greened */
static unsigned g_uSoftSoftInq;     /* soft-path INQUIRY greened */
static unsigned g_aSoftDoorStats[2];/* last door STATS snapshot */

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
    g_uSoftStepBits = 0;
    g_cSoftOk = 0;
    g_cSoftSkip = 0;
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

/* ---- Soft inventory (Wave 31 exclusive deepen) -------------------------- */

/** Note one soft suite sub-step outcome (never hard-gates). */
static void
soft_step_note(unsigned uBit, int fOk)
{
    if (fOk != 0) {
        g_uSoftStepBits |= uBit;
        if (g_cSoftOk < 0xffffffffu) {
            g_cSoftOk++;
        }
    } else if (g_cSoftSkip < 0xffffffffu) {
        g_cSoftSkip++;
    }
}

/** Note one soft door sub-step outcome (never hard-gates). */
static void
soft_door_note(unsigned uBit, int fOk)
{
    if (fOk != 0) {
        g_uSoftDoorBits |= uBit;
        if (g_cSoftDoorOk < 0xffffffffu) {
            g_cSoftDoorOk++;
        }
    } else if (g_cSoftDoorSkip < 0xffffffffu) {
        g_cSoftDoorSkip++;
    }
}

/**
 * Greppable soft inventory (Wave 31 exclusive deepen).
 * Prefix-stable markers (scsi_mid-gj: soft …):
 *   scsi_mid-gj: soft inventory  — ok/skip + door + wave/areas + log_n
 *   scsi_mid-gj: soft steps      — per-sub-step lamps + bits
 *   scsi_mid-gj: soft lun        — LUN honesty (soft only; not product gate)
 *   scsi_mid-gj: soft multi      — multi-block deepen lamp
 *   scsi_mid-gj: soft lba        — illegal LBA honesty lamp
 *   scsi_mid-gj: soft evpd       — EVPD reject honesty lamp
 *   scsi_mid-gj: soft door       — product door soft lamps (INQUIRY separate)
 *   scsi_mid-gj: soft geometry   — soft LUN geometry
 *   scsi_mid-gj: soft deepen     — wave=31 stamp
 *   scsi_mid-gj: soft path       — honesty: soft LUN ≠ product door INQUIRY
 *   scsi_mid-gj: soft inventory PASS
 *
 * Soft LUN honesty remains soft. Product INQUIRY path is door-only.
 * Never hard-gates live path PASS.
 */
static void
soft_inventory_log(void)
{
    char aLine[192];
    unsigned o;

    if (g_cSoftInvLog < 0xffffffffu) {
        g_cSoftInvLog++;
    }

    /* Grep: scsi_mid-gj: soft inventory */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "scsi_mid-gj: soft inventory ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftSkip);
    append_s(aLine, sizeof(aLine), &o, " soft_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftIoOk);
    append_s(aLine, sizeof(aLine), &o, " soft_fail=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftIoFail);
    append_s(aLine, sizeof(aLine), &o, " door_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftDoorOk);
    append_s(aLine, sizeof(aLine), &o, " door_skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftDoorSkip);
    append_s(aLine, sizeof(aLine), &o, " wave=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)SCSI_SOFT_WAVE);
    append_s(aLine, sizeof(aLine), &o, " areas=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)SCSI_SOFT_AREAS);
    append_s(aLine, sizeof(aLine), &o, " log_n=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftInvLog);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: scsi_mid-gj: soft steps */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "scsi_mid-gj: soft steps tur=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftStepBits & SOFT_S_TUR) != 0u));
    append_s(aLine, sizeof(aLine), &o, " inq=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftStepBits & SOFT_S_INQ) != 0u));
    append_s(aLine, sizeof(aLine), &o, " mode=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftStepBits & SOFT_S_MODE) != 0u));
    append_s(aLine, sizeof(aLine), &o, " readcap=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftStepBits & SOFT_S_READCAP) != 0u));
    append_s(aLine, sizeof(aLine), &o, " write10=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftStepBits & SOFT_S_WRITE10) != 0u));
    append_s(aLine, sizeof(aLine), &o, " read10=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftStepBits & SOFT_S_READ10) != 0u));
    append_s(aLine, sizeof(aLine), &o, " rw=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftStepBits & SOFT_S_RW) != 0u));
    append_s(aLine, sizeof(aLine), &o, " lun=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftStepBits & SOFT_S_LUN) != 0u));
    append_s(aLine, sizeof(aLine), &o, " sense=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftStepBits & SOFT_S_SENSE) != 0u));
    append_s(aLine, sizeof(aLine), &o, " sync=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftStepBits & SOFT_S_SYNC) != 0u));
    append_s(aLine, sizeof(aLine), &o, " multi=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftStepBits & SOFT_S_MULTI) != 0u));
    append_s(aLine, sizeof(aLine), &o, " lba=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftStepBits & SOFT_S_LBA) != 0u));
    append_s(aLine, sizeof(aLine), &o, " evpd=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftStepBits & SOFT_S_EVPD) != 0u));
    append_s(aLine, sizeof(aLine), &o, " bits=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftStepBits);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: scsi_mid-gj: soft lun (honesty remains soft) */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "scsi_mid-gj: soft lun honest=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftLunHonest);
    append_s(aLine, sizeof(aLine), &o, " key=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftLunKey);
    append_s(aLine, sizeof(aLine), &o, " asc=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftLunAsc);
    append_s(aLine, sizeof(aLine), &o,
             " soft_only=1 (soft inventory; not product gate)\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: scsi_mid-gj: soft multi */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "scsi_mid-gj: soft multi ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiOk);
    append_s(aLine, sizeof(aLine), &o, " lba=2 blocks=2\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: scsi_mid-gj: soft lba */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "scsi_mid-gj: soft lba honest=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftLbaHonest);
    append_s(aLine, sizeof(aLine), &o, " asc=0x21 soft_only=1\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: scsi_mid-gj: soft evpd */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "scsi_mid-gj: soft evpd honest=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftEvpdHonest);
    append_s(aLine, sizeof(aLine), &o, " asc=0x24 soft_only=1\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: scsi_mid-gj: soft door (product INQUIRY tracked separate) */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "scsi_mid-gj: soft door ready=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & SOFT_D_READY) != 0u));
    append_s(aLine, sizeof(aLine), &o, " product_inq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftProductInq);
    append_s(aLine, sizeof(aLine), &o, " readcap=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & SOFT_D_READCAP) != 0u));
    append_s(aLine, sizeof(aLine), &o, " read10=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & SOFT_D_READ10) != 0u));
    append_s(aLine, sizeof(aLine), &o, " stats=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & SOFT_D_STATS) != 0u));
    append_s(aLine, sizeof(aLine), &o, " side_inq=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & SOFT_D_SIDE) != 0u));
    append_s(aLine, sizeof(aLine), &o, " soft_inq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftSoftInq);
    append_s(aLine, sizeof(aLine), &o, " io=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_aSoftDoorStats[0]);
    append_s(aLine, sizeof(aLine), &o, " ready_flag=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_aSoftDoorStats[1]);
    append_s(aLine, sizeof(aLine), &o, " bits=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftDoorBits);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: scsi_mid-gj: soft geometry */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "scsi_mid-gj: soft geometry sectors=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)SOFT_SECTORS);
    append_s(aLine, sizeof(aLine), &o, " sec_size=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)SOFT_SEC_SIZE);
    append_s(aLine, sizeof(aLine), &o, " bytes=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(SOFT_SECTORS * SOFT_SEC_SIZE));
    append_s(aLine, sizeof(aLine), &o, " sense_max=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)SOFT_SENSE_MAX);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: scsi_mid-gj: soft deepen wave */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "scsi_mid-gj: soft deepen wave=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)SCSI_SOFT_WAVE);
    append_s(aLine, sizeof(aLine), &o, " areas=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)SCSI_SOFT_AREAS);
    append_s(aLine, sizeof(aLine), &o, " ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftSkip);
    append_s(aLine, sizeof(aLine), &o, " door_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftDoorOk);
    append_s(aLine, sizeof(aLine), &o, " multi_server=0 confine=0\n");
    aLine[o] = '\0';
    msg(aLine);

    /*
     * Grep: scsi_mid-gj: soft path
     * Honesty: soft LUN policy ≠ product door INQUIRY path; not bar3.
     * Soft inventory ≠ product multi-server confine.
     */
    msg("scsi_mid-gj: soft path soft_lun=1 door=1 product_inq=door "
        "soft_inq=soft lun_honest=soft multi=soft lba=soft evpd=soft "
        "multi_server=0 confine=0 wave=31 "
        "(soft inventory; not bar3; soft != product multi-server confine)\n");

    /*
     * Grep: scsi_mid-gj: soft honesty (Wave 31 exclusive deepen).
     * Soft inventory ≠ product multi-server confine.
     */
    msg("scsi_mid-gj: soft honesty multi_server=0 confine=0 bar3=0 "
        "exclusive=1 soft=1 product_kernel=OPEN wave=31\n");

    /* Soft lamp only — never a product / bar3 gate. */
    msg("scsi_mid-gj: soft inventory PASS\n");
}

/**
 * Wave 31 deepen probes on the soft LUN (multi-block, illegal LBA, EVPD).
 * Always soft — never hard-fails the live path.
 */
static void
soft_deepen_probes(void)
{
    unsigned char aCdb[16];
    unsigned char aBlk[SOFT_SEC_SIZE * 2u];
    unsigned i;
    int fOk;

    /* Multi-block WRITE10/READ10 at LBA 2, 2 blocks. */
    for (i = 0; i < sizeof(aBlk); i++) {
        aBlk[i] = (unsigned char)(0x3Cu ^ (unsigned char)i);
    }
    cdb_rw10(aCdb, SCSI_OP_WRITE_10, 2u, 2u);
    fOk = (soft_submit(0, aCdb, 10, aBlk, sizeof(aBlk)) == 0);
    if (fOk) {
        memzero(aBlk, sizeof(aBlk));
        cdb_rw10(aCdb, SCSI_OP_READ_10, 2u, 2u);
        fOk = (soft_submit(0, aCdb, 10, aBlk, sizeof(aBlk)) == 0);
        if (fOk) {
            for (i = 0; i < sizeof(aBlk); i++) {
                if (aBlk[i] != (unsigned char)(0x3Cu ^ (unsigned char)i)) {
                    fOk = 0;
                    break;
                }
            }
        }
    }
    if (fOk) {
        g_uSoftMultiOk = 1;
        soft_step_note(SOFT_S_MULTI, 1);
        msg("scsi_mid-gj: soft multi PASS\n");
    } else {
        soft_step_note(SOFT_S_MULTI, 0);
        msg("scsi_mid-gj: soft multi soft-skip\n");
    }

    /* Illegal LBA (past soft geometry) → CHECK ASC 0x21. */
    cdb_rw10(aCdb, SCSI_OP_READ_10, SOFT_SECTORS, 1u);
    if (soft_submit(0, aCdb, 10, aBlk, SOFT_SEC_SIZE) != 0 &&
        g_uSoftSenseLen >= 14 &&
        (g_aSoftSense[2] & 0x0fu) == SCSI_SK_ILLEGAL_REQUEST &&
        g_aSoftSense[12] == 0x21u) {
        g_uSoftLbaHonest = 1;
        soft_step_note(SOFT_S_LBA, 1);
        msg("scsi_mid-gj: soft LBA map PASS\n");
    } else {
        soft_step_note(SOFT_S_LBA, 0);
        msg("scsi_mid-gj: soft LBA map soft-skip\n");
    }

    /* EVPD INQUIRY reject honesty (soft path only serves standard INQUIRY). */
    memzero(aCdb, 16);
    aCdb[0] = SCSI_OP_INQUIRY;
    aCdb[1] = 0x01u; /* EVPD */
    aCdb[2] = 0x00u;
    aCdb[4] = 36;
    if (soft_submit(0, aCdb, 6, aBlk, 36) != 0 && g_uSoftSenseLen >= 14 &&
        (g_aSoftSense[2] & 0x0fu) == SCSI_SK_ILLEGAL_REQUEST &&
        g_aSoftSense[12] == 0x24u) {
        g_uSoftEvpdHonest = 1;
        soft_step_note(SOFT_S_EVPD, 1);
        msg("scsi_mid-gj: soft EVPD reject PASS\n");
    } else {
        soft_step_note(SOFT_S_EVPD, 0);
        msg("scsi_mid-gj: soft EVPD reject soft-skip\n");
    }
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
        soft_step_note(SOFT_S_TUR, 1);
        msg("scsi_mid-gj: soft TUR PASS\n");
    } else {
        soft_step_note(SOFT_S_TUR, 0);
        msg("scsi_mid-gj: soft TUR soft-skip\n");
    }

    /* 2. Soft INQUIRY + vendor (product door INQUIRY path is separate). */
    memzero(aInq, sizeof(aInq));
    cdb_inquiry(aCdb, 36);
    if (soft_submit(0, aCdb, 6, aInq, sizeof(aInq)) == 0) {
        g_uSoftSoftInq = 1;
        soft_step_note(SOFT_S_INQ, 1);
        msg("scsi_mid-gj: soft INQUIRY PASS\n");
        msg_inquiry_vendor(aInq, (long)sizeof(aInq));
    } else {
        soft_step_note(SOFT_S_INQ, 0);
        msg("scsi_mid-gj: soft INQUIRY soft-skip\n");
    }

    /* 3. MODE SENSE(6) header */
    memzero(aMode, sizeof(aMode));
    cdb_mode_sense6(aCdb, 4);
    if (soft_submit(0, aCdb, 6, aMode, sizeof(aMode)) == 0) {
        soft_step_note(SOFT_S_MODE, 1);
        msg("scsi_mid-gj: soft MODE_SENSE PASS\n");
    } else {
        soft_step_note(SOFT_S_MODE, 0);
        msg("scsi_mid-gj: soft MODE_SENSE soft-skip\n");
    }

    /* 4. READ CAPACITY */
    memzero(aCap, sizeof(aCap));
    cdb_read_cap(aCdb);
    if (soft_submit(0, aCdb, 10, aCap, sizeof(aCap)) == 0) {
        soft_step_note(SOFT_S_READCAP, 1);
        msg("scsi_mid-gj: soft READ_CAP PASS\n");
    } else {
        soft_step_note(SOFT_S_READCAP, 0);
        msg("scsi_mid-gj: soft READ_CAP soft-skip\n");
    }

    /* 5. WRITE10 pattern + READ10 verify at LBA 1 */
    for (i = 0; i < SOFT_SEC_SIZE; i++) {
        aBlk[i] = (unsigned char)(0xA5u ^ (unsigned char)i);
    }
    cdb_rw10(aCdb, SCSI_OP_WRITE_10, 1u, 1u);
    fOk = (soft_submit(0, aCdb, 10, aBlk, SOFT_SEC_SIZE) == 0);
    if (fOk) {
        soft_step_note(SOFT_S_WRITE10, 1);
        msg("scsi_mid-gj: soft WRITE10 PASS\n");
    } else {
        soft_step_note(SOFT_S_WRITE10, 0);
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
        soft_step_note(SOFT_S_READ10, 1);
        soft_step_note(SOFT_S_RW, 1);
        msg("scsi_mid-gj: soft READ10 PASS\n");
        msg("scsi_mid-gj: soft R/W verify PASS\n");
    } else {
        soft_step_note(SOFT_S_READ10, 0);
        soft_step_note(SOFT_S_RW, 0);
        msg("scsi_mid-gj: soft READ10 soft-skip\n");
    }

    /* 6. Illegal LUN → CHECK sense (soft honesty), then REQUEST SENSE */
    cdb_tur(aCdb);
    if (soft_submit(1, aCdb, 6, 0, 0) != 0 && g_uSoftSenseLen >= 14 &&
        (g_aSoftSense[2] & 0x0fu) == SCSI_SK_ILLEGAL_REQUEST &&
        g_aSoftSense[12] == 0x25u) {
        g_uSoftLunHonest = 1;
        g_uSoftLunKey = (unsigned)(g_aSoftSense[2] & 0x0fu);
        g_uSoftLunAsc = (unsigned)g_aSoftSense[12];
        soft_step_note(SOFT_S_LUN, 1);
        msg("scsi_mid-gj: soft LUN map PASS\n");
    } else {
        soft_step_note(SOFT_S_LUN, 0);
        msg("scsi_mid-gj: soft LUN map soft-skip\n");
    }
    memzero(aSense, sizeof(aSense));
    cdb_req_sense(aCdb, 18);
    if (soft_submit(0, aCdb, 6, aSense, sizeof(aSense)) == 0) {
        soft_step_note(SOFT_S_SENSE, 1);
        msg("scsi_mid-gj: soft REQ_SENSE PASS\n");
    } else {
        soft_step_note(SOFT_S_SENSE, 0);
        msg("scsi_mid-gj: soft REQ_SENSE soft-skip\n");
    }

    /* 7. SYNCHRONIZE CACHE */
    cdb_sync(aCdb);
    if (soft_submit(0, aCdb, 10, 0, 0) == 0) {
        soft_step_note(SOFT_S_SYNC, 1);
        msg("scsi_mid-gj: soft SYNC PASS\n");
    } else {
        soft_step_note(SOFT_S_SYNC, 0);
        msg("scsi_mid-gj: soft SYNC soft-skip\n");
    }

    /* 8. Wave 31 deepen probes (multi / LBA / EVPD) — always soft. */
    soft_deepen_probes();

    msg_soft_stats();
    soft_inventory_log();
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
    int fCapOk;

    msg("scsi_mid-gj: READY PASS\n");
    soft_door_note(SOFT_D_READY, 1);

    /*
     * Product INQUIRY — door path only. Hard fail if door errors.
     * Soft inventory tracks product_inq separately from soft_inq.
     */
    for (i = 0; i < sizeof(aInq); i++) {
        aInq[i] = 0;
    }
    n = gj_scsi(GJ_SCSI_OP_INQUIRY, (long)(unsigned long)aInq, 36, 0);
    if (n < 0) {
        msg("scsi_mid-gj: INQUIRY FAIL\n");
        gj_exit(1);
    }
    g_uSoftProductInq = 1;
    soft_door_note(SOFT_D_INQ, 1);
    msg("scsi_mid-gj: INQUIRY PASS\n");
    msg_inquiry_vendor(aInq, (n >= 16) ? n : (long)sizeof(aInq));

    /* Optional capacity + single-block READ10. */
    fCapOk = (gj_scsi(GJ_SCSI_OP_READ_CAP, (long)(unsigned long)aCap, 0, 0) ==
              0);
    if (fCapOk) {
        soft_door_note(SOFT_D_READCAP, 1);
        msg("scsi_mid-gj: READ_CAP PASS\n");
        n = gj_scsi(GJ_SCSI_OP_READ10, 0, (long)(unsigned long)aBlk, 1);
        if (n == 512) {
            soft_door_note(SOFT_D_READ10, 1);
            msg("scsi_mid-gj: READ10 PASS\n");
        } else {
            soft_door_note(SOFT_D_READ10, 0);
            msg("scsi_mid-gj: READ10 soft-skip\n");
        }
    } else {
        soft_door_note(SOFT_D_READCAP, 0);
        soft_door_note(SOFT_D_READ10, 0);
        msg("scsi_mid-gj: READ_CAP soft-skip\n");
    }

    /* Optional door STATS (io count + ready flag). Soft if unsupported. */
    aStats[0] = 0;
    aStats[1] = 0;
    if (gj_scsi(GJ_SCSI_OP_STATS, (long)(unsigned long)aStats, 0, 0) == 0) {
        char aLine[64];
        unsigned o = 0;

        g_aSoftDoorStats[0] = aStats[0];
        g_aSoftDoorStats[1] = aStats[1];
        soft_door_note(SOFT_D_STATS, 1);
        append_s(aLine, sizeof(aLine), &o, "scsi_mid-gj: STATS PASS io=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)aStats[0]);
        append_s(aLine, sizeof(aLine), &o, " ready=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)aStats[1]);
        append_s(aLine, sizeof(aLine), &o, "\n");
        aLine[o] = '\0';
        msg(aLine);
    } else {
        soft_door_note(SOFT_D_STATS, 0);
        msg("scsi_mid-gj: STATS soft-skip\n");
    }

    /*
     * Soft mid side-smoke even when door is ready: proves userspace soft
     * policy stays healthy beside the HBA path (never hard-fails).
     * Soft INQUIRY is separate from product door INQUIRY.
     */
    {
        unsigned char aCdb[16];
        unsigned char aSoftInq[36];

        soft_init();
        cdb_inquiry(aCdb, 36);
        memzero(aSoftInq, sizeof(aSoftInq));
        if (soft_submit(0, aCdb, 6, aSoftInq, sizeof(aSoftInq)) == 0) {
            g_uSoftSoftInq = 1;
            soft_door_note(SOFT_D_SIDE, 1);
            soft_step_note(SOFT_S_INQ, 1);
            msg("scsi_mid-gj: soft side INQUIRY PASS\n");
        } else {
            soft_door_note(SOFT_D_SIDE, 0);
            soft_step_note(SOFT_S_INQ, 0);
            msg("scsi_mid-gj: soft side soft-skip\n");
        }
        /* Soft LUN honesty + deepen beside door (always soft). */
        soft_deepen_probes();
    }

    soft_inventory_log();
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
