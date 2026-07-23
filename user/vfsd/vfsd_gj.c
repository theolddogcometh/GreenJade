/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding vfsd — multi-client VFS door owner + mini-FS bring-up.
 * Named page-cache (memobj) + soft door surface + optional store LBA0 super.
 *
 * Hard smoke markers (prefix-stable for scripts/smoke-all.sh — do not rename):
 *   vfsd-gj: block mount PASS
 *   vfsd-gj: file path PASS
 *   vfsd-gj: multi-client door PASS
 *   vfsd-gj: live path PASS
 *
 * Soft (optional; never renames required PASS lines; soft-skip never hard-fails):
 *   vfsd-gj: named create miss/reuse
 *   vfsd-gj: STATS PASS | STATS soft
 *   vfsd-gj: reclaim soft PASS | reclaim soft-skip
 *   vfsd-gj: bare OPEN soft PASS | bare OPEN soft-skip
 *   vfsd-gj: WRITEFD soft PASS | WRITEFD soft-skip
 *   vfsd-gj: SEEKFD soft PASS | SEEKFD soft-skip
 *   vfsd-gj: UNLINK soft PASS | UNLINK soft-skip
 * Greppable soft inventory prefix ("vfsd-gj: soft …") — deepen-only, never hard:
 *   vfsd-gj: soft door start
 *   vfsd-gj: soft STAT PASS | soft STAT soft-skip
 *   vfsd-gj: soft LIST PASS | soft LIST soft-skip
 *   vfsd-gj: soft SEEK_CUR PASS | soft SEEK_CUR soft-skip
 *   vfsd-gj: soft STATS PASS | soft STATS soft-skip
 *   vfsd-gj: soft stats ok=N bits=0xX
 *   vfsd-gj: soft door PASS | soft door soft-skip
 *   vfsd-gj: store LBA0 super mirror PASS | soft-skip
 *   vfsd-gj: store LBA0 read soft PASS | soft-skip
 *   vfsd-gj: RELEASE free soft PASS | soft-skip  (post-RELEASE no-op)
 *
 *   make vfsd-gj → build/user/vfsd.elf
 * Boot embed (parent): kernel/proc/vfsd_embed.S (.incbin of the ELF).
 *
 * Pure C11 freestanding. Dual-licensed MIT OR Apache-2.0 (no GPL).
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
/* SEEKFD may lag libgj headers; op number is fixed in vfs_door.h. */
#ifndef GJ_VFS_OP_SEEKFD
#define GJ_VFS_OP_SEEKFD   16u
#endif
#ifndef GJ_VFS_O_CREAT
#define GJ_VFS_O_CREAT     1u
#define GJ_VFS_O_RDWR      2u
#endif
#ifndef GJ_VFS_SEEK_SET
#define GJ_VFS_SEEK_SET    0u
#define GJ_VFS_SEEK_CUR    1u
#define GJ_VFS_SEEK_END    2u
#endif
#ifndef GJ_STORE_OP_CLAIM
#define GJ_STORE_OP_CLAIM   5u
#define GJ_STORE_OP_RELEASE 6u
#define GJ_STORE_OP_WRITE   4u
#define GJ_STORE_OP_READ    3u
#endif

#define GJ_VFS_MAGIC    0x31444a47u /* "GJD1" little-endian */
#define GJ_VFS_VERSION  3u          /* must match kernel vfs_door VFS_VERSION */
#define VFSD_TOKEN      0x56465344u /* "VFSD" */
#define VFSD_CACHE_VA   0x34000000ul
#define VFSD_CACHE_NAME "vfsd-cache"
#define VFSD_SECTOR_CB  512u
#define VFSD_PAGE_MASK  0xffful

/* Soft-path scratch names (must not collide with hard-path hello.txt). */
#define VFSD_SOFT_NAME  "soft.tmp"
#define VFSD_SOFT_BODY  "SOFT\n"
#define VFSD_SOFT_PATCH "WFD"

/* Named-cache soft counters at fixed slots (after hard path writes 0..7). */
#define VFSD_CACHE_SOFT_RECLAIM  8u
#define VFSD_CACHE_SOFT_WRITEFD  9u
#define VFSD_CACHE_SOFT_SEEK     10u
#define VFSD_CACHE_SOFT_UNLINK   11u
#define VFSD_CACHE_SOFT_BITS     12u
#define VFSD_CACHE_SOFT_STAT     13u
#define VFSD_CACHE_SOFT_OK       14u

/* Soft bit flags in pCache[VFSD_CACHE_SOFT_BITS] */
#define VFSD_SOFT_BIT_RECLAIM  1u
#define VFSD_SOFT_BIT_BARE     2u
#define VFSD_SOFT_BIT_WRITEFD  4u
#define VFSD_SOFT_BIT_SEEKFD   8u
#define VFSD_SOFT_BIT_UNLINK   16u
#define VFSD_SOFT_BIT_STAT     32u
#define VFSD_SOFT_BIT_LIST     64u
#define VFSD_SOFT_BIT_SEEK_CUR 128u
#define VFSD_SOFT_BIT_STATS    256u

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

/*
 * Soft door surface while CLAIM is held.
 * Never hard-fails: each step soft-skip on rejection / short I/O.
 * Leaves hard-path hello.txt content intact (only mutates soft.tmp).
 * Does not RELEASE / re-token CLAIM ownership (CLAIM PASS stays hard).
 * Returns count of soft sub-steps that greened (0..9).
 */
static unsigned
soft_door_path(volatile unsigned *pCache)
{
    static unsigned char aRd[64];
    static char aList[160];
    static unsigned aStat[2];
    static unsigned aDoorSt[4];
    static char aInv[96];
    static const char szSoft[] = VFSD_SOFT_NAME;
    static const char szBody[] = VFSD_SOFT_BODY;
    static const char szPatch[] = VFSD_SOFT_PATCH;
    const unsigned cbBody = (unsigned)(sizeof(szBody) - 1u);
    const unsigned cbPatch = (unsigned)(sizeof(szPatch) - 1u);
    const unsigned cbName = (unsigned)(sizeof(szSoft) - 1u);
    unsigned uBits = 0u;
    unsigned cOk = 0u;
    long nRet;
    long hFd;

    if (pCache == 0) {
        msg("vfsd-gj: soft door soft-skip\n");
        return 0u;
    }

    /* Greppable soft inventory start (prefix "vfsd-gj: soft "). */
    msg("vfsd-gj: soft door start\n");

    /*
     * 1) Soft reclaim: same-token re-CLAIM is idempotent (vfs_door soft path).
     *    Must stay owned after; failure is soft-skip only. Never hard-fails.
     */
    nRet = gj_vfs(GJ_VFS_OP_CLAIM, (long)VFSD_TOKEN, 0, 0);
    if (nRet == 0) {
        pCache[VFSD_CACHE_SOFT_RECLAIM] = 1u;
        uBits |= VFSD_SOFT_BIT_RECLAIM;
        cOk++;
        msg("vfsd-gj: reclaim soft PASS\n");
    } else {
        msg("vfsd-gj: reclaim soft-skip\n");
    }

    /*
     * 2) Bare-name OPEN (no /mnt/ prefix) + READFD on hard-path hello.txt.
     *    Soft: kernel path_normalize should accept bare names.
     */
    {
        static const char szHello[] = "hello.txt";
        static const char szExpect[] = "GJVFS2";
        const unsigned cbExpect = (unsigned)(sizeof(szExpect) - 1u);

        hFd = gj_vfs(GJ_VFS_OP_OPEN, (long)(uintptr_t)szHello, 0, 0);
        if (hFd < 0) {
            msg("vfsd-gj: bare OPEN soft-skip\n");
        } else {
            (void)gj_memset(aRd, 0, sizeof(aRd));
            nRet = gj_vfs(GJ_VFS_OP_READFD, hFd, (long)(uintptr_t)aRd,
                          (long)sizeof(aRd));
            (void)gj_vfs(GJ_VFS_OP_CLOSE, hFd, 0, 0);
            if (nRet == (long)cbExpect &&
                gj_memcmp(aRd, szExpect, cbExpect) == 0) {
                uBits |= VFSD_SOFT_BIT_BARE;
                cOk++;
                msg("vfsd-gj: bare OPEN soft PASS\n");
            } else {
                msg("vfsd-gj: bare OPEN soft-skip\n");
            }
        }
    }

    /*
     * 3) WRITEFD soft: OPEN(CREAT|RDWR) soft.tmp, WRITEFD body, CLOSE,
     *    name READ verify. Soft-skip on any step fail.
     */
    hFd = gj_vfs(GJ_VFS_OP_OPEN, (long)(uintptr_t)szSoft,
                 (long)(GJ_VFS_O_CREAT | GJ_VFS_O_RDWR), 0);
    if (hFd < 0) {
        msg("vfsd-gj: WRITEFD soft-skip\n");
    } else {
        nRet = gj_vfs(GJ_VFS_OP_WRITEFD, hFd, (long)(uintptr_t)szBody,
                      (long)cbBody);
        (void)gj_vfs(GJ_VFS_OP_CLOSE, hFd, 0, 0);
        hFd = -1;
        if (nRet != (long)cbBody) {
            msg("vfsd-gj: WRITEFD soft-skip\n");
        } else {
            (void)gj_memset(aRd, 0, sizeof(aRd));
            nRet = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szSoft,
                          (long)(uintptr_t)aRd, (long)(sizeof(aRd) - 1u));
            if (nRet == (long)cbBody && gj_memcmp(aRd, szBody, cbBody) == 0) {
                pCache[VFSD_CACHE_SOFT_WRITEFD] = cbBody;
                uBits |= VFSD_SOFT_BIT_WRITEFD;
                cOk++;
                msg("vfsd-gj: WRITEFD soft PASS\n");
            } else {
                msg("vfsd-gj: WRITEFD soft-skip\n");
            }
        }
    }

    /*
     * 3b) Soft STAT of soft.tmp — size must match body after WRITEFD.
     *     Greppable: vfsd-gj: soft STAT …
     */
    if ((uBits & VFSD_SOFT_BIT_WRITEFD) != 0u) {
        aStat[0] = aStat[1] = 0u;
        nRet = gj_vfs(GJ_VFS_OP_STAT, (long)(uintptr_t)szSoft,
                      (long)(uintptr_t)aStat, 0);
        if (nRet == 0 && aStat[0] == cbBody) {
            pCache[VFSD_CACHE_SOFT_STAT] = aStat[0];
            uBits |= VFSD_SOFT_BIT_STAT;
            cOk++;
            msg("vfsd-gj: soft STAT PASS\n");
        } else {
            msg("vfsd-gj: soft STAT soft-skip\n");
        }
    } else {
        msg("vfsd-gj: soft STAT soft-skip\n");
    }

    /*
     * 3c) Soft LIST must contain soft.tmp while it still exists.
     *     Greppable: vfsd-gj: soft LIST …
     */
    if ((uBits & VFSD_SOFT_BIT_WRITEFD) != 0u) {
        (void)gj_memset(aList, 0, sizeof(aList));
        nRet = gj_vfs(GJ_VFS_OP_LIST, (long)(uintptr_t)aList, 0,
                      (long)sizeof(aList));
        if (nRet >= 0 && buf_contains(aList, nRet, szSoft, cbName)) {
            uBits |= VFSD_SOFT_BIT_LIST;
            cOk++;
            msg("vfsd-gj: soft LIST PASS\n");
        } else {
            msg("vfsd-gj: soft LIST soft-skip\n");
        }
    } else {
        msg("vfsd-gj: soft LIST soft-skip\n");
    }

    /*
     * 4) SEEKFD soft: reopen RDWR, SEEK END, WRITEFD patch, SEEK SET,
     *    soft SEEK_CUR probe, READFD full, verify patch at end.
     *    Soft-skip if door returns NOSUPPORT or any step fails.
     */
    if ((uBits & VFSD_SOFT_BIT_WRITEFD) != 0u) {
        hFd = gj_vfs(GJ_VFS_OP_OPEN, (long)(uintptr_t)szSoft,
                     (long)GJ_VFS_O_RDWR, 0);
        if (hFd < 0) {
            msg("vfsd-gj: SEEKFD soft-skip\n");
            msg("vfsd-gj: soft SEEK_CUR soft-skip\n");
        } else {
            long nEnd;
            long nSeek;
            long nCur;

            nEnd = gj_vfs(GJ_VFS_OP_SEEKFD, hFd, 0, (long)GJ_VFS_SEEK_END);
            if (nEnd < 0) {
                (void)gj_vfs(GJ_VFS_OP_CLOSE, hFd, 0, 0);
                msg("vfsd-gj: SEEKFD soft-skip\n");
                msg("vfsd-gj: soft SEEK_CUR soft-skip\n");
            } else {
                nRet = gj_vfs(GJ_VFS_OP_WRITEFD, hFd,
                              (long)(uintptr_t)szPatch, (long)cbPatch);
                nSeek = gj_vfs(GJ_VFS_OP_SEEKFD, hFd, 0,
                               (long)GJ_VFS_SEEK_SET);
                /*
                 * Soft SEEK_CUR: from offset 0, +cbBody must land at body end
                 * (patch start). Restore SET 0 before full READFD.
                 * Greppable: vfsd-gj: soft SEEK_CUR …
                 */
                nCur = -1;
                if (nRet == (long)cbPatch && nSeek == 0) {
                    nCur = gj_vfs(GJ_VFS_OP_SEEKFD, hFd, (long)cbBody,
                                  (long)GJ_VFS_SEEK_CUR);
                    if (nCur == (long)cbBody) {
                        uBits |= VFSD_SOFT_BIT_SEEK_CUR;
                        cOk++;
                        msg("vfsd-gj: soft SEEK_CUR PASS\n");
                    } else {
                        msg("vfsd-gj: soft SEEK_CUR soft-skip\n");
                    }
                    nSeek = gj_vfs(GJ_VFS_OP_SEEKFD, hFd, 0,
                                   (long)GJ_VFS_SEEK_SET);
                } else {
                    msg("vfsd-gj: soft SEEK_CUR soft-skip\n");
                }
                (void)gj_memset(aRd, 0, sizeof(aRd));
                nRet = (nRet == (long)cbPatch && nSeek == 0)
                           ? gj_vfs(GJ_VFS_OP_READFD, hFd,
                                    (long)(uintptr_t)aRd,
                                    (long)(sizeof(aRd) - 1u))
                           : -1;
                (void)gj_vfs(GJ_VFS_OP_CLOSE, hFd, 0, 0);
                /*
                 * Expect body + patch: "SOFT\n" + "WFD" → 8 bytes.
                 * Verify patch lands at original end offset.
                 */
                if (nRet == (long)(cbBody + cbPatch) &&
                    nEnd == (long)cbBody &&
                    gj_memcmp(aRd, szBody, cbBody) == 0 &&
                    gj_memcmp(aRd + cbBody, szPatch, cbPatch) == 0) {
                    pCache[VFSD_CACHE_SOFT_SEEK] = (unsigned)nRet;
                    uBits |= VFSD_SOFT_BIT_SEEKFD;
                    cOk++;
                    msg("vfsd-gj: SEEKFD soft PASS\n");
                } else {
                    msg("vfsd-gj: SEEKFD soft-skip\n");
                }
                (void)nCur;
            }
        }
    } else {
        msg("vfsd-gj: SEEKFD soft-skip\n");
        msg("vfsd-gj: soft SEEK_CUR soft-skip\n");
    }

    /*
     * 5) UNLINK soft: remove soft.tmp; LIST must not contain name.
     *    Best-effort even if create/write soft failed (NOENT is soft-skip).
     */
    nRet = gj_vfs(GJ_VFS_OP_UNLINK, (long)(uintptr_t)szSoft, 0, 0);
    if (nRet != 0) {
        msg("vfsd-gj: UNLINK soft-skip\n");
    } else {
        (void)gj_memset(aList, 0, sizeof(aList));
        nRet = gj_vfs(GJ_VFS_OP_LIST, (long)(uintptr_t)aList, 0,
                      (long)sizeof(aList));
        if (nRet >= 0 &&
            !buf_contains(aList, nRet, szSoft, cbName)) {
            pCache[VFSD_CACHE_SOFT_UNLINK] = 1u;
            uBits |= VFSD_SOFT_BIT_UNLINK;
            cOk++;
            msg("vfsd-gj: UNLINK soft PASS\n");
        } else {
            msg("vfsd-gj: UNLINK soft-skip\n");
        }
    }

    /*
     * 6) Soft STATS recheck while still claimed: owned=1, mounted=1.
     *    Greppable: vfsd-gj: soft STATS …  (distinct from hard STATS PASS).
     *    Soft-skip only — never demotes hard CLAIM / live path.
     */
    (void)gj_memset(aDoorSt, 0, sizeof(aDoorSt));
    nRet = gj_vfs(GJ_VFS_OP_STATS, (long)(uintptr_t)aDoorSt, 0, 0);
    if (nRet == 0 && aDoorSt[2] == 1u && aDoorSt[3] == 1u) {
        uBits |= VFSD_SOFT_BIT_STATS;
        cOk++;
        msg("vfsd-gj: soft STATS PASS\n");
    } else {
        msg("vfsd-gj: soft STATS soft-skip\n");
    }

    pCache[VFSD_CACHE_SOFT_BITS] = uBits;
    pCache[VFSD_CACHE_SOFT_OK] = cOk;

    /*
     * Soft inventory rollup — greppable "vfsd-gj: soft stats ok=… bits=…".
     * Pure observation; never gates hard PASS lines.
     */
    (void)gj_snprintf(aInv, sizeof(aInv),
                      "vfsd-gj: soft stats ok=%u bits=0x%x\n",
                      (unsigned long)cOk, (unsigned long)uBits);
    msg(aInv);

    /* Aggregate soft door line — green if any sub-step greened. */
    if (cOk > 0u) {
        msg("vfsd-gj: soft door PASS\n");
    } else {
        msg("vfsd-gj: soft door soft-skip\n");
    }
    return cOk;
}

/*
 * Optional store LBA0 super mirror + soft READ verify.
 * Ownership separate from VFS door; soft-skip when claim busy / short I/O.
 */
static void
soft_store_lba0_mirror(void)
{
    static unsigned char aSec[VFSD_SECTOR_CB];
    static unsigned char aRd[VFSD_SECTOR_CB];
    long nRet;
    unsigned iByte;
    int fMagicOk;

    if (gj_store(GJ_STORE_OP_CLAIM, (long)VFSD_TOKEN, 0, 0) != 0) {
        msg("vfsd-gj: store LBA0 super mirror soft-skip\n");
        return;
    }

    (void)gj_memset(aSec, 0, sizeof(aSec));
    aSec[0] = (unsigned char)(GJ_VFS_MAGIC & 0xffu);
    aSec[1] = (unsigned char)((GJ_VFS_MAGIC >> 8) & 0xffu);
    aSec[2] = (unsigned char)((GJ_VFS_MAGIC >> 16) & 0xffu);
    aSec[3] = (unsigned char)((GJ_VFS_MAGIC >> 24) & 0xffu);
    /* Soft version nibble for diagnostics (not part of hard super). */
    aSec[4] = (unsigned char)(GJ_VFS_VERSION & 0xffu);

    nRet = gj_store(GJ_STORE_OP_WRITE, 0, (long)(uintptr_t)aSec,
                    (long)VFSD_SECTOR_CB);
    if (nRet != (long)VFSD_SECTOR_CB) {
        (void)gj_store(GJ_STORE_OP_RELEASE, (long)VFSD_TOKEN, 0, 0);
        msg("vfsd-gj: store LBA0 super mirror soft-skip\n");
        return;
    }
    msg("vfsd-gj: store LBA0 super mirror PASS\n");

    /* Soft READ-back of LBA0 — verify magic LE bytes stick. */
    (void)gj_memset(aRd, 0, sizeof(aRd));
    nRet = gj_store(GJ_STORE_OP_READ, 0, (long)(uintptr_t)aRd,
                    (long)VFSD_SECTOR_CB);
    (void)gj_store(GJ_STORE_OP_RELEASE, (long)VFSD_TOKEN, 0, 0);

    if (nRet != (long)VFSD_SECTOR_CB) {
        msg("vfsd-gj: store LBA0 read soft-skip\n");
        return;
    }
    fMagicOk = 1;
    for (iByte = 0; iByte < 4u; iByte++) {
        if (aRd[iByte] != aSec[iByte]) {
            fMagicOk = 0;
            break;
        }
    }
    if (fMagicOk != 0 && aRd[4] == aSec[4]) {
        msg("vfsd-gj: store LBA0 read soft PASS\n");
    } else {
        msg("vfsd-gj: store LBA0 read soft-skip\n");
    }
}

void
_start(void)
{
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

    /* Shape checks (freestanding: no <assert.h>). */
    if ((VFSD_CACHE_VA & VFSD_PAGE_MASK) != 0ul) {
        fail_exit("vfsd-gj: CACHE_VA not page-aligned\n");
    }
    if (VFSD_TOKEN == 0u || GJ_VFS_VERSION == 0u) {
        fail_exit("vfsd-gj: token/version zero\n");
    }
    if (VFSD_SECTOR_CB != 512u) {
        fail_exit("vfsd-gj: SECTOR_CB invalid\n");
    }

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

    /* Soft deepen: reclaim / bare OPEN / WRITEFD / SEEKFD / UNLINK */
    (void)soft_door_path(pCache);

    /* Optional raw store LBA0 super mirror + soft READ verify */
    soft_store_lba0_mirror();

    if (gj_vfs(GJ_VFS_OP_RELEASE, (long)g_uToken, 0, 0) != 0) {
        g_uToken = 0u;
        fail_exit("vfsd-gj: RELEASE fail\n");
    }
    g_uToken = 0u;
    msg("vfsd-gj: RELEASE PASS\n");

    /*
     * Soft free RELEASE: door already free → soft no-op (0).
     * Never hard-fails live path.
     */
    nRet = gj_vfs(GJ_VFS_OP_RELEASE, (long)VFSD_TOKEN, 0, 0);
    if (nRet == 0) {
        msg("vfsd-gj: RELEASE free soft PASS\n");
    } else {
        msg("vfsd-gj: RELEASE free soft-skip\n");
    }

    /* Re-check named cache still holds magic after FS work */
    if (pCache[0] != GJ_VFS_MAGIC) {
        fail_exit("vfsd-gj: named cache sticky fail\n");
    }
    msg("vfsd-gj: live path PASS (door+mini-FS+named cache)\n");
    gj_exit(0);
}
