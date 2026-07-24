/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * storaged — host POSIX software-image smoke (A1 CI path).
 *
 * Soft inventory (Wave 66 exclusive deepen — greppable "storaged: soft …"):
 *   storaged: soft inventory … / soft deepen wave=66 …
 *   multi_server=0 confine=0; soft ≠ product multi-server confine; not bar3.
 *
 * Exercises door-shaped sector R/W against an in-process 32 KiB image so CI
 * can green without the kernel store door. The freestanding product path is
 * storaged_gj.c (CLAIM / soft door / sector smoke / UDX ring / RELEASE over
 * GJ_SYS_STORE).
 *
 * Compatibility notes (keep aligned with storaged_gj + store_door):
 *   - Sector size 512 B; transfers are sector-multiple only.
 *   - Bounds: reject null, zero length, non-sector-multiple, LBA overflow.
 *   - Soft multi-sector, CAP, QUEUE, FLUSH-shaped probes deepen the surface.
 *   - Host markers are independent of ring-map (ring is freestanding-only).
 *
 * Smoke markers (prefix-stable for scripts/smoke-all.sh — do not rename):
 *   storaged: PASS
 *
 *   make storaged && ./build/storaged
 *
 * Pure C11 (hosted). Dual-licensed MIT OR Apache-2.0 (no GPL).
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define GJ_STORE_SECTOR 512u
#define GJ_STORE_SECTS  64u /* 32 KiB software image */
#define GJ_STORE_XFER_MAX 4096u
/* Soft multi-sector: LBA 3, 2 sectors — mirrors freestanding SOFT_LBA. */
#define SOFT_LBA        3u
#define SOFT_SECTS      2u
#define SOFT_BYTES      (SOFT_SECTS * GJ_STORE_SECTOR)

static uint8_t g_aImg[GJ_STORE_SECTOR * GJ_STORE_SECTS];
static uint32_t g_u32Io;
static uint32_t g_u32DoorRw;
static uint32_t g_u32Owner; /* 0 = free, non-zero = claimed token */
static uint32_t g_u32Calls;

/*
 * Door-shaped write: sector-multiple only, LBA range checked.
 * Returns 0 on success, -1 on defensive reject (no partial write).
 */
static int
store_write(uint32_t u32Lba, const void *pBuf, uint32_t cbBytes)
{
    uint32_t cSectors;

    g_u32Calls++;
    if (pBuf == NULL || cbBytes == 0u ||
        (cbBytes % GJ_STORE_SECTOR) != 0u ||
        cbBytes > GJ_STORE_XFER_MAX) {
        return -1;
    }
    cSectors = cbBytes / GJ_STORE_SECTOR;
    /* Overflow-safe range: lba + count must not wrap or exceed image. */
    if (u32Lba >= GJ_STORE_SECTS || cSectors > GJ_STORE_SECTS ||
        u32Lba + cSectors > GJ_STORE_SECTS) {
        return -1;
    }
    memcpy(g_aImg + (size_t)u32Lba * GJ_STORE_SECTOR, pBuf, cbBytes);
    g_u32Io++;
    g_u32DoorRw++;
    return 0;
}

/*
 * Door-shaped read: same contract as store_write.
 * Returns 0 on success, -1 on defensive reject (no partial read).
 */
static int
store_read(uint32_t u32Lba, void *pBuf, uint32_t cbBytes)
{
    uint32_t cSectors;

    g_u32Calls++;
    if (pBuf == NULL || cbBytes == 0u ||
        (cbBytes % GJ_STORE_SECTOR) != 0u ||
        cbBytes > GJ_STORE_XFER_MAX) {
        return -1;
    }
    cSectors = cbBytes / GJ_STORE_SECTOR;
    if (u32Lba >= GJ_STORE_SECTS || cSectors > GJ_STORE_SECTS ||
        u32Lba + cSectors > GJ_STORE_SECTS) {
        return -1;
    }
    memcpy(pBuf, g_aImg + (size_t)u32Lba * GJ_STORE_SECTOR, cbBytes);
    g_u32Io++;
    g_u32DoorRw++;
    return 0;
}

/* Door-shaped CAP: capacity in 512-byte sectors. */
static int
store_cap(uint64_t *pCap)
{
    g_u32Calls++;
    if (pCap == NULL) {
        return -1;
    }
    *pCap = (uint64_t)GJ_STORE_SECTS;
    return 0;
}

/* Door-shaped STATS: {blk_io, scsi_io=0, calls}. */
static int
store_stats(uint32_t aSt[3])
{
    g_u32Calls++;
    if (aSt == NULL) {
        return -1;
    }
    aSt[0] = g_u32Io;
    aSt[1] = 0u;
    aSt[2] = g_u32Calls;
    return 0;
}

/* Door-shaped QUEUE_INFO: {blk_io, scsi_io, door_rw, owned}. */
static int
store_queue_info(uint32_t aQ[4])
{
    g_u32Calls++;
    if (aQ == NULL) {
        return -1;
    }
    aQ[0] = g_u32Io;
    aQ[1] = 0u;
    aQ[2] = g_u32DoorRw;
    aQ[3] = g_u32Owner ? 1u : 0u;
    return 0;
}

/* Door-shaped FLUSH: fsync-shaped success while image is live. */
static int
store_flush(void)
{
    g_u32Calls++;
    return 0;
}

/* Soft CLAIM / RELEASE (token non-zero; reclaim idempotent). */
static int
store_claim(uint32_t u32Tok)
{
    g_u32Calls++;
    if (u32Tok == 0u) {
        return -1;
    }
    if (g_u32Owner != 0u && g_u32Owner != u32Tok) {
        return -1; /* BUSY */
    }
    g_u32Owner = u32Tok;
    return 0;
}

static int
store_release(uint32_t u32Tok)
{
    g_u32Calls++;
    /* Soft free: already unowned → 0. */
    if (g_u32Owner == 0u) {
        return 0;
    }
    if (u32Tok != g_u32Owner) {
        return -1;
    }
    g_u32Owner = 0u;
    return 0;
}

/* Reject path probe — must stay -1 (CI / API contract). */
static int
store_reject_probe(void)
{
    static uint8_t aTmp[GJ_STORE_SECTOR];
    static uint8_t aBig[GJ_STORE_XFER_MAX + GJ_STORE_SECTOR];
    int nFails = 0;

    /* Null buffer */
    if (store_write(0, NULL, GJ_STORE_SECTOR) == 0) {
        nFails++;
    }
    if (store_read(0, NULL, GJ_STORE_SECTOR) == 0) {
        nFails++;
    }
    /* Zero length */
    if (store_write(0, aTmp, 0) == 0) {
        nFails++;
    }
    /* Non-sector-multiple */
    if (store_write(0, aTmp, GJ_STORE_SECTOR - 1u) == 0) {
        nFails++;
    }
    /* LBA past end */
    if (store_write(GJ_STORE_SECTS, aTmp, GJ_STORE_SECTOR) == 0) {
        nFails++;
    }
    /* Range overflow past image end */
    if (store_write(GJ_STORE_SECTS - 1u, aTmp, GJ_STORE_SECTOR * 2u) == 0) {
        nFails++;
    }
    /* Over XFER_MAX (door ceiling) */
    if (store_write(0, aBig, (uint32_t)sizeof(aBig)) == 0) {
        nFails++;
    }
    /* Null CAP / STATS / QUEUE */
    if (store_cap(NULL) == 0) {
        nFails++;
    }
    if (store_stats(NULL) == 0) {
        nFails++;
    }
    if (store_queue_info(NULL) == 0) {
        nFails++;
    }
    /* Zero-token CLAIM */
    if (store_claim(0) == 0) {
        nFails++;
    }
    return nFails == 0 ? 0 : -1;
}

int
main(void)
{
    static uint8_t aSec[GJ_STORE_SECTOR];
    static uint8_t aRd[GJ_STORE_SECTOR];
    static uint8_t aSoftW[SOFT_BYTES];
    static uint8_t aSoftR[SOFT_BYTES];
    static const char szMark[] = "GreenJade-storaged\n";
    static const uint32_t uTok = 0x510e0002u;
    uint32_t iByte;
    uint32_t aSt[3];
    uint32_t aQ[4];
    uint64_t u64Cap;
    size_t cbMark;
    unsigned cSoft = 0u;

    printf("storaged: start (software image until scsi door)\n");

    memset(g_aImg, 0, sizeof(g_aImg));
    g_u32Io = 0u;
    g_u32DoorRw = 0u;
    g_u32Owner = 0u;
    g_u32Calls = 0u;

    /* Defensive reject paths before any successful I/O. */
    if (store_reject_probe() != 0) {
        printf("storaged: reject-path FAIL\n");
        return 1;
    }

    /* Soft CLAIM + reclaim (idempotent same token). */
    if (store_claim(uTok) != 0) {
        printf("storaged: CLAIM FAIL\n");
        return 1;
    }
    if (store_claim(uTok) != 0) {
        printf("storaged: reclaim FAIL\n");
        return 1;
    }
    cSoft++;
    printf("storaged: reclaim soft ok token=0x%x\n", (unsigned)uTok);

    /* CAP soft */
    u64Cap = 0ull;
    if (store_cap(&u64Cap) != 0 || u64Cap != (uint64_t)GJ_STORE_SECTS) {
        printf("storaged: CAP FAIL\n");
        return 1;
    }
    cSoft++;
    printf("storaged: CAP soft sectors=%llu\n",
           (unsigned long long)u64Cap);

    /* Sector 0: magic banner (mirrors product identity string). */
    memset(aSec, 0, sizeof(aSec));
    cbMark = strlen(szMark);
    if (cbMark >= sizeof(aSec)) {
        printf("storaged: mark too long FAIL\n");
        return 1;
    }
    memcpy(aSec, szMark, cbMark);

    if (store_write(0, aSec, GJ_STORE_SECTOR) != 0) {
        printf("storaged: write FAIL\n");
        return 1;
    }
    memset(aRd, 0, sizeof(aRd));
    if (store_read(0, aRd, GJ_STORE_SECTOR) != 0) {
        printf("storaged: read FAIL\n");
        return 1;
    }
    if (memcmp(aSec, aRd, GJ_STORE_SECTOR) != 0) {
        printf("storaged: verify FAIL\n");
        return 1;
    }
    /* Print first 9 identity bytes only (bounds-safe). */
    printf("storaged: sector0 ok io=%u magic=\"%c%c%c%c%c%c%c%c%c\"\n",
           (unsigned)g_u32Io, aRd[0], aRd[1], aRd[2], aRd[3], aRd[4], aRd[5],
           aRd[6], aRd[7], aRd[8]);

    /*
     * Multi-LBA (door-shaped CAP + R/W): pattern at LBA 1, full-sector
     * verify — same sector size / multiple rules as freestanding WRITE/READ.
     */
    for (iByte = 0; iByte < GJ_STORE_SECTOR; iByte++) {
        aSec[iByte] = (uint8_t)(0x50u + (iByte & 0x0fu));
    }
    memset(aRd, 0, sizeof(aRd));
    if (store_write(1, aSec, GJ_STORE_SECTOR) != 0 ||
        store_read(1, aRd, GJ_STORE_SECTOR) != 0 ||
        memcmp(aSec, aRd, GJ_STORE_SECTOR) != 0) {
        printf("storaged: lba1 FAIL\n");
        return 1;
    }
    /* Sector 0 must remain intact after LBA 1 traffic. */
    memset(aRd, 0, sizeof(aRd));
    if (store_read(0, aRd, GJ_STORE_SECTOR) != 0 ||
        memcmp(szMark, aRd, cbMark) != 0) {
        printf("storaged: sector0 clobber FAIL\n");
        return 1;
    }

    /*
     * Soft multi-sector at LBA 3 (2 × 512 B) — freestanding soft path shape.
     * Must not clobber sector 0 / LBA 1.
     */
    for (iByte = 0; iByte < SOFT_BYTES; iByte++) {
        aSoftW[iByte] = (uint8_t)(0xA5u ^ (uint8_t)(iByte & 0xffu));
    }
    memset(aSoftR, 0, sizeof(aSoftR));
    if (store_write(SOFT_LBA, aSoftW, SOFT_BYTES) != 0 ||
        store_read(SOFT_LBA, aSoftR, SOFT_BYTES) != 0 ||
        memcmp(aSoftW, aSoftR, SOFT_BYTES) != 0) {
        printf("storaged: multi-sector FAIL\n");
        return 1;
    }
    cSoft++;
    /* Neighbours intact. */
    memset(aRd, 0, sizeof(aRd));
    if (store_read(0, aRd, GJ_STORE_SECTOR) != 0 ||
        memcmp(szMark, aRd, cbMark) != 0) {
        printf("storaged: multi-sector clobber FAIL\n");
        return 1;
    }
    printf("storaged: multi-sector soft ok lba=%u bytes=%u\n",
           (unsigned)SOFT_LBA, (unsigned)SOFT_BYTES);

    /* FLUSH soft */
    if (store_flush() != 0) {
        printf("storaged: FLUSH FAIL\n");
        return 1;
    }
    cSoft++;
    printf("storaged: FLUSH soft ok\n");

    /* STATS + QUEUE soft while owned. */
    aSt[0] = aSt[1] = aSt[2] = 0u;
    if (store_stats(aSt) != 0 || aSt[0] == 0u) {
        printf("storaged: STATS FAIL\n");
        return 1;
    }
    cSoft++;
    printf("storaged: STATS soft blk=%u scsi=%u calls=%u\n",
           (unsigned)aSt[0], (unsigned)aSt[1], (unsigned)aSt[2]);

    aQ[0] = aQ[1] = aQ[2] = aQ[3] = 0u;
    if (store_queue_info(aQ) != 0 || aQ[3] != 1u) {
        printf("storaged: QUEUE FAIL\n");
        return 1;
    }
    cSoft++;
    printf("storaged: QUEUE soft blk=%u scsi=%u rw=%u own=%u\n",
           (unsigned)aQ[0], (unsigned)aQ[1], (unsigned)aQ[2],
           (unsigned)aQ[3]);

    /* RELEASE + free soft (idempotent unowned RELEASE). */
    if (store_release(uTok) != 0) {
        printf("storaged: RELEASE FAIL\n");
        return 1;
    }
    if (store_release(uTok) != 0) {
        printf("storaged: free RELEASE FAIL\n");
        return 1;
    }
    aQ[0] = aQ[1] = aQ[2] = aQ[3] = 0u;
    if (store_queue_info(aQ) != 0 || aQ[3] != 0u) {
        printf("storaged: free own FAIL\n");
        return 1;
    }
    cSoft++;
    printf("storaged: free soft ok soft_steps=%u\n", cSoft);

    /* Grep: storaged: soft inventory (Wave 66 exclusive deepen) */
/* Wave 66 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft retflankangle continuum_toward=20800 soft_ne_product=1
 *   greppable: soft retfaceangle exclusive=1 continuum_toward=20800
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */

    printf("storaged: soft inventory soft_steps=%u sectors=%u io=%u "
           "wave=66 multi_server=0 confine=0\n",
           cSoft, (unsigned)GJ_STORE_SECTS, (unsigned)g_u32Io);
    printf("storaged: soft deepen product_kernel=OPEN wave=66 areas=1 multi_server=0 "
           "confine=0 bar3=0\n");
    printf("storaged: soft honesty multi_server=0 confine=0 bar3=0 "
           "exclusive=1 soft=1 product_kernel=OPEN wave=66\n");

    printf("storaged: door-shaped multi-lba ok sectors=%u io=%u\n",
           (unsigned)GJ_STORE_SECTS, (unsigned)g_u32Io);
    printf("storaged: PASS\n");
    return 0;
}
