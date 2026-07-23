/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding vfsd — multi-client VFS door owner + mini-FS bring-up.
 * Named page-cache (memobj) + optional store LBA0 super mirror.
 *
 * Smoke markers (must stay prefix-stable for scripts/smoke-all.sh):
 *   vfsd-gj: block mount PASS
 *   vfsd-gj: file path PASS
 *   vfsd-gj: multi-client door PASS
 *   vfsd-gj: live path PASS
 *
 * Soft (optional; never renames required PASS lines):
 *   vfsd-gj: named create miss/reuse
 *   vfsd-gj: STATS PASS | STATS soft
 *   vfsd-gj: store LBA0 super mirror PASS | soft-skip
 *
 *   make vfsd-gj → build/user/vfsd.elf
 * Boot embed (parent): kernel/proc/vfsd_embed.S (.incbin of the ELF).
 */
#include <gj/string.h>
#include <gj/syscalls.h>

/*
 * Prefer libgj GJ_VFS_OP_* / GJ_STORE_OP_* from syscalls.h.
 * Local fallbacks are self-describing only — values must match
 * kernel/include/gj/vfs_door.h (do not invent opcodes here).
 */
#ifndef GJ_VFS_OP_CLAIM
#define GJ_VFS_OP_CLAIM    1u
#define GJ_VFS_OP_RELEASE  2u
#define GJ_VFS_OP_FORMAT   3u
#define GJ_VFS_OP_MOUNT    4u
#define GJ_VFS_OP_CREATE   5u
#define GJ_VFS_OP_READ     6u
#define GJ_VFS_OP_WRITE    7u
#define GJ_VFS_OP_UNLINK   8u
#define GJ_VFS_OP_STAT     9u
#define GJ_VFS_OP_LIST     10u
#define GJ_VFS_OP_STATS    11u
#define GJ_VFS_OP_OPEN     12u
#define GJ_VFS_OP_CLOSE    13u
#define GJ_VFS_OP_READFD   14u
#define GJ_VFS_OP_WRITEFD  15u
#endif
#ifndef GJ_STORE_OP_CLAIM
#define GJ_STORE_OP_CLAIM   5u
#define GJ_STORE_OP_RELEASE 6u
#define GJ_STORE_OP_WRITE   4u
#endif

#define GJ_VFS_MAGIC    0x31444a47u /* "GJD1" little-endian */
#define GJ_VFS_VERSION  3u          /* must match kernel vfs_door VFS_VERSION */
#define VFSD_TOKEN      0x56465344u /* "VFSD" */
#define VFSD_CACHE_VA   0x34000000ul
#define VFSD_CACHE_NAME "vfsd-cache"
#define VFSD_SECTOR_CB  512u
#define VFSD_PAGE_MASK  0xffful

static unsigned g_uToken;

static void
msg(const char *sz)
{
    if (sz == 0) {
        return;
    }
    (void)gj_debug_log(sz, (long)gj_strlen(sz));
}

static void
fail_exit(const char *szWhy)
{
    msg(szWhy);
    if (g_uToken != 0u) {
        (void)gj_vfs(GJ_VFS_OP_RELEASE, (long)g_uToken, 0, 0);
        g_uToken = 0u;
    }
    gj_exit(1);
}

/* True if needle appears as a contiguous substring of hay (no NUL in needle). */
static int
buf_contains(const char *pHay, long cbHay, const char *szNeedle, unsigned cbNeedle)
{
    long i;
    unsigned j;

    if (pHay == 0 || szNeedle == 0 || cbHay <= 0 || cbNeedle == 0u) {
        return 0;
    }
    if ((long)cbNeedle > cbHay) {
        return 0;
    }
    for (i = 0; i <= cbHay - (long)cbNeedle; i++) {
        for (j = 0; j < cbNeedle; j++) {
            if (pHay[i + (long)j] != szNeedle[j]) {
                break;
            }
        }
        if (j == cbNeedle) {
            return 1;
        }
    }
    return 0;
}

void
_start(void)
{
    static unsigned char aSec[VFSD_SECTOR_CB];
    static unsigned char aRd[64];
    static unsigned aInfo[4];
    static unsigned aStat[2];
    static unsigned aDoorSt[4];
    static char aList[128];
    static const char szHello[] = "hello.txt";
    static const char szBody[] = "GJVFS\n";
    static const char szBody2[] = "GJVFS2";
    static const char szNote[] = "note";
    static const char szNoteBody[] = "ok";
    static const char szMnt[] = "/mnt/hello.txt";
    const unsigned cbBody = (unsigned)(sizeof(szBody) - 1u);
    const unsigned cbBody2 = (unsigned)(sizeof(szBody2) - 1u);
    const unsigned cbNoteBody = (unsigned)(sizeof(szNoteBody) - 1u);
    long nRet;
    long i64Va;
    long hFd;
    volatile unsigned *pCache;

    g_uToken = 0u;
    msg("vfsd-gj: start\n");

    /* Named shareable page-cache (create may miss if already present) */
    if (gj_memobj_create_named(VFSD_CACHE_NAME, 1) != 0) {
        msg("vfsd-gj: named create miss/reuse\n");
    }
    i64Va = gj_memobj_map_named(VFSD_CACHE_NAME, VFSD_CACHE_VA, 3);
    if (i64Va <= 0) {
        fail_exit("vfsd-gj: named map fail\n");
    }
    /* Defensive: reject low / unaligned VA before dereference */
    if ((unsigned long)i64Va < 0x1000ul ||
        ((unsigned long)i64Va & VFSD_PAGE_MASK) != 0ul) {
        fail_exit("vfsd-gj: named map VA bad\n");
    }
    pCache = (volatile unsigned *)(uintptr_t)i64Va;
    pCache[0] = GJ_VFS_MAGIC;
    if (pCache[0] != GJ_VFS_MAGIC) {
        fail_exit("vfsd-gj: named cache write verify fail\n");
    }
    msg("vfsd-gj: named cache PASS (memobj map+magic)\n");

    /* Claim multi-client VFS door + format/mount */
    if (gj_vfs(GJ_VFS_OP_CLAIM, (long)VFSD_TOKEN, 0, 0) != 0) {
        fail_exit("vfsd-gj: CLAIM fail\n");
    }
    g_uToken = VFSD_TOKEN;
    msg("vfsd-gj: CLAIM PASS\n");

    /* Always format to current on-disk layout version */
    if (gj_vfs(GJ_VFS_OP_FORMAT, 0, 0, 0) != 0) {
        fail_exit("vfsd-gj: FORMAT fail\n");
    }
    (void)gj_memset(aInfo, 0, sizeof(aInfo));
    if (gj_vfs(GJ_VFS_OP_MOUNT, (long)(uintptr_t)aInfo, 0, 0) != 0) {
        fail_exit("vfsd-gj: MOUNT fail\n");
    }
    /* Mount info: [0]=magic [1]=version [2]=files [3]=data_lba0 */
    if (aInfo[0] != GJ_VFS_MAGIC || aInfo[1] != GJ_VFS_VERSION) {
        fail_exit("vfsd-gj: MOUNT super verify fail\n");
    }
    pCache[1] = aInfo[0];
    pCache[2] = aInfo[1];
    pCache[3] = aInfo[3];
    msg("vfsd-gj: block mount PASS (format+mount)\n");

    /* File create/read/overwrite via door (any client may call these) */
    nRet = gj_vfs(GJ_VFS_OP_CREATE, (long)(uintptr_t)szHello,
                  (long)(uintptr_t)szBody, (long)cbBody);
    if (nRet < 0) {
        fail_exit("vfsd-gj: CREATE fail\n");
    }
    (void)gj_memset(aRd, 0, sizeof(aRd));
    nRet = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szHello,
                  (long)(uintptr_t)aRd, (long)(sizeof(aRd) - 1u));
    if (nRet != (long)cbBody || gj_memcmp(aRd, szBody, cbBody) != 0) {
        fail_exit("vfsd-gj: file read fail\n");
    }
    nRet = gj_vfs(GJ_VFS_OP_WRITE, (long)(uintptr_t)szNote,
                  (long)(uintptr_t)szNoteBody, (long)cbNoteBody);
    if (nRet != (long)cbNoteBody) {
        fail_exit("vfsd-gj: WRITE note fail\n");
    }
    nRet = gj_vfs(GJ_VFS_OP_WRITE, (long)(uintptr_t)szHello,
                  (long)(uintptr_t)szBody2, (long)cbBody2);
    if (nRet != (long)cbBody2) {
        fail_exit("vfsd-gj: overwrite fail\n");
    }
    (void)gj_memset(aRd, 0, sizeof(aRd));
    nRet = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szHello,
                  (long)(uintptr_t)aRd, 8);
    if (nRet != (long)cbBody2 || gj_memcmp(aRd, szBody2, cbBody2) != 0) {
        fail_exit("vfsd-gj: overwrite verify fail\n");
    }
    (void)gj_memset(aStat, 0, sizeof(aStat));
    if (gj_vfs(GJ_VFS_OP_STAT, (long)(uintptr_t)szHello,
               (long)(uintptr_t)aStat, 0) != 0 ||
        aStat[0] != cbBody2) {
        fail_exit("vfsd-gj: STAT fail\n");
    }
    (void)gj_memset(aList, 0, sizeof(aList));
    nRet = gj_vfs(GJ_VFS_OP_LIST, (long)(uintptr_t)aList, 0,
                  (long)sizeof(aList));
    if (nRet <= 0 || nRet > (long)sizeof(aList)) {
        fail_exit("vfsd-gj: LIST fail\n");
    }
    /* LIST is newline-separated names; require hello.txt present */
    if (!buf_contains(aList, nRet, szHello,
                      (unsigned)(sizeof(szHello) - 1u))) {
        fail_exit("vfsd-gj: LIST missing hello.txt\n");
    }
    pCache[4] = aStat[0];
    pCache[5] = (unsigned)nRet;

    /* OPEN / READFD / CLOSE with /mnt/ path namespace */
    hFd = gj_vfs(GJ_VFS_OP_OPEN, (long)(uintptr_t)szMnt, 0, 0);
    if (hFd < 0) {
        fail_exit("vfsd-gj: OPEN fail\n");
    }
    (void)gj_memset(aRd, 0, sizeof(aRd));
    nRet = gj_vfs(GJ_VFS_OP_READFD, hFd, (long)(uintptr_t)aRd, 16);
    if (gj_vfs(GJ_VFS_OP_CLOSE, hFd, 0, 0) != 0) {
        fail_exit("vfsd-gj: CLOSE fail\n");
    }
    hFd = -1;
    if (nRet != (long)cbBody2 || gj_memcmp(aRd, szBody2, cbBody2) != 0) {
        fail_exit("vfsd-gj: READFD fail\n");
    }
    msg("vfsd-gj: file path PASS (create/read/write/stat/list)\n");
    msg("vfsd-gj: multi-client door PASS (CLAIM owned)\n");
    msg("vfsd-gj: openfd PASS (/mnt + READFD)\n");

    /*
     * Soft STATS while still claimed: [0]=calls [1]=files [2]=owned [3]=mounted.
     * Soft-skip if door rejects; hard-fail if counters contradict ownership.
     */
    (void)gj_memset(aDoorSt, 0, sizeof(aDoorSt));
    nRet = gj_vfs(GJ_VFS_OP_STATS, (long)(uintptr_t)aDoorSt, 0, 0);
    if (nRet != 0) {
        msg("vfsd-gj: STATS soft\n");
    } else if (aDoorSt[2] != 1u || aDoorSt[3] != 1u) {
        fail_exit("vfsd-gj: STATS owned/mounted fail\n");
    } else {
        pCache[6] = aDoorSt[0];
        pCache[7] = aDoorSt[1];
        msg("vfsd-gj: STATS PASS (owned+mounted)\n");
    }

    /* Optional raw store LBA0 super mirror (ownership separate from VFS door) */
    if (gj_store(GJ_STORE_OP_CLAIM, (long)VFSD_TOKEN, 0, 0) == 0) {
        (void)gj_memset(aSec, 0, sizeof(aSec));
        aSec[0] = (unsigned char)(GJ_VFS_MAGIC & 0xffu);
        aSec[1] = (unsigned char)((GJ_VFS_MAGIC >> 8) & 0xffu);
        aSec[2] = (unsigned char)((GJ_VFS_MAGIC >> 16) & 0xffu);
        aSec[3] = (unsigned char)((GJ_VFS_MAGIC >> 24) & 0xffu);
        nRet = gj_store(GJ_STORE_OP_WRITE, 0, (long)(uintptr_t)aSec,
                        (long)VFSD_SECTOR_CB);
        (void)gj_store(GJ_STORE_OP_RELEASE, (long)VFSD_TOKEN, 0, 0);
        if (nRet == (long)VFSD_SECTOR_CB) {
            msg("vfsd-gj: store LBA0 super mirror PASS\n");
        } else {
            msg("vfsd-gj: store LBA0 super mirror soft-skip\n");
        }
    } else {
        msg("vfsd-gj: store LBA0 super mirror soft-skip\n");
    }

    if (gj_vfs(GJ_VFS_OP_RELEASE, (long)g_uToken, 0, 0) != 0) {
        g_uToken = 0u;
        fail_exit("vfsd-gj: RELEASE fail\n");
    }
    g_uToken = 0u;
    msg("vfsd-gj: RELEASE PASS\n");

    /* Re-check named cache still holds magic after FS work */
    if (pCache[0] != GJ_VFS_MAGIC) {
        fail_exit("vfsd-gj: named cache sticky fail\n");
    }
    msg("vfsd-gj: live path PASS (door+mini-FS+named cache)\n");
    gj_exit(0);
}
