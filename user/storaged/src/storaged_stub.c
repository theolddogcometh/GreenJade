/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * storaged — host POSIX software-image smoke (A1 CI path).
 *
 * Exercises door-shaped sector R/W against an in-process 32 KiB image so CI
 * can green without the kernel store door. The freestanding product path is
 * storaged_gj.c (CLAIM / sector smoke / UDX ring / RELEASE over GJ_SYS_STORE).
 *
 * Compatibility notes (keep aligned with storaged_gj + store_door):
 *   - Sector size 512 B; transfers are sector-multiple only.
 *   - Bounds: reject null, zero length, non-sector-multiple, LBA overflow.
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

static uint8_t g_aImg[GJ_STORE_SECTOR * GJ_STORE_SECTS];
static uint32_t g_u32Io;

/*
 * Door-shaped write: sector-multiple only, LBA range checked.
 * Returns 0 on success, -1 on defensive reject (no partial write).
 */
static int
store_write(uint32_t u32Lba, const void *pBuf, uint32_t cbBytes)
{
    uint32_t cSectors;

    if (pBuf == NULL || cbBytes == 0u ||
        (cbBytes % GJ_STORE_SECTOR) != 0u) {
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

    if (pBuf == NULL || cbBytes == 0u ||
        (cbBytes % GJ_STORE_SECTOR) != 0u) {
        return -1;
    }
    cSectors = cbBytes / GJ_STORE_SECTOR;
    if (u32Lba >= GJ_STORE_SECTS || cSectors > GJ_STORE_SECTS ||
        u32Lba + cSectors > GJ_STORE_SECTS) {
        return -1;
    }
    memcpy(pBuf, g_aImg + (size_t)u32Lba * GJ_STORE_SECTOR, cbBytes);
    g_u32Io++;
    return 0;
}

/* Reject path probe — must stay -1 (CI / API contract). */
static int
store_reject_probe(void)
{
    static uint8_t aTmp[GJ_STORE_SECTOR];
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
    return nFails == 0 ? 0 : -1;
}

int
main(void)
{
    static uint8_t aSec[GJ_STORE_SECTOR];
    static uint8_t aRd[GJ_STORE_SECTOR];
    static const char szMark[] = "GreenJade-storaged\n";
    uint32_t iByte;
    size_t cbMark;

    printf("storaged: start (software image until scsi door)\n");

    memset(g_aImg, 0, sizeof(g_aImg));
    g_u32Io = 0u;

    /* Defensive reject paths before any successful I/O. */
    if (store_reject_probe() != 0) {
        printf("storaged: reject-path FAIL\n");
        return 1;
    }

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

    printf("storaged: door-shaped multi-lba ok sectors=%u io=%u\n",
           (unsigned)GJ_STORE_SECTS, (unsigned)g_u32Io);
    printf("storaged: PASS\n");
    return 0;
}
