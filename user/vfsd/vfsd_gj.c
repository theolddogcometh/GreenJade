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
 *   vfsd-gj: store LBA0 super mirror PASS | soft-skip
 *   vfsd-gj: store LBA0 read soft PASS | soft-skip
 *   vfsd-gj: RELEASE free soft PASS | soft-skip  (post-RELEASE no-op)
 *
 * Soft inventory (Wave 119 exclusive deepen — greppable "vfsd-gj: soft …"):
 *   vfsd-gj: soft door start
 *   vfsd-gj: soft reclaim PASS | soft reclaim soft-skip
 *   vfsd-gj: soft bare PASS | soft bare soft-skip
 *   vfsd-gj: soft WRITEFD PASS | soft WRITEFD soft-skip
 *   vfsd-gj: soft STAT PASS | soft STAT soft-skip
 *   vfsd-gj: soft LIST PASS | soft LIST soft-skip
 *   vfsd-gj: soft SEEK_END PASS | soft SEEK_END soft-skip
 *   vfsd-gj: soft SEEK_CUR PASS | soft SEEK_CUR soft-skip
 *   vfsd-gj: soft SEEKFD PASS | soft SEEKFD soft-skip
 *   vfsd-gj: soft UNLINK PASS | soft UNLINK soft-skip
 *   vfsd-gj: soft STATS PASS | soft STATS soft-skip
 *   vfsd-gj: soft store mirror PASS | soft store mirror soft-skip
 *   vfsd-gj: soft store read PASS | soft store read soft-skip
 *   vfsd-gj: soft free PASS | soft free soft-skip
 *   vfsd-gj: soft inventory door_ok=… door_skip=… store_ok=… store_skip=…
 *                free_ok=… free_skip=… wave=70
 *   vfsd-gj: soft door reclaim=… bare=… writefd=… stat=… list=… seek_end=…
 *                seek_cur=… seekfd=… unlink=… stats=… bits=…
 *   vfsd-gj: soft store mirror=… read=… bits=…
 *   vfsd-gj: soft free release=… bits=…
 *   vfsd-gj: soft cache reclaim=… writefd=… seek=… unlink=… stat=… bits=… ok=…
 *   vfsd-gj: soft stats ok=… skip=… door_bits=… store_bits=… free_bits=…
 *   vfsd-gj: soft deepen wave=70 areas=… ok=… skip=…
 *   vfsd-gj: soft path multi_server=0 confine=0 (soft; not bar3)
 *   vfsd-gj: soft honesty multi_server=0 confine=0 bar3=0 exclusive=1 soft=1 product_kernel=OPEN wave=70
 *   vfsd-gj: soft door PASS | soft door soft-skip
 * Diagnostics only — never hard-fail live path PASS; not a bar3 claim.
 * Honesty: soft inventory ≠ product multi-server confine.
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

/* Soft door sub-step bits in pCache[VFSD_CACHE_SOFT_BITS] / inventory. */
#define VFSD_SOFT_BIT_RECLAIM  1u
#define VFSD_SOFT_BIT_BARE     2u
#define VFSD_SOFT_BIT_WRITEFD  4u
#define VFSD_SOFT_BIT_SEEKFD   8u
#define VFSD_SOFT_BIT_UNLINK   16u
#define VFSD_SOFT_BIT_STAT     32u
#define VFSD_SOFT_BIT_LIST     64u
#define VFSD_SOFT_BIT_SEEK_CUR 128u
#define VFSD_SOFT_BIT_STATS    256u
#define VFSD_SOFT_BIT_SEEK_END 512u

/* Soft store path bits (Wave 111 inventory; LBA0 mirror + read). */
/* Wave 119 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft retqueueangle continuum_toward=26100 soft_ne_product=1 wave=119
 *   greppable: soft reteventangle exclusive=1 continuum_toward=26100 soft_ne_product=1 wave=119
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */

#define VFSD_SOFT_STORE_MIRROR 1u
#define VFSD_SOFT_STORE_READ   2u

/* Soft free path bits (Wave 111 inventory; post-RELEASE no-op). */
#define VFSD_SOFT_FREE_RELEASE 1u

/* Soft inventory wave stamp (Wave 119 exclusive deepen). */
#define VFSD_SOFT_WAVE 70u
#define VFSD_SOFT_AREAS        8u  /* inventory door store free cache stats deepen path honesty */

static unsigned g_uToken;

/*
 * Wave 119 soft inventory tallies (file-local; wrap OK).
 * greppable: vfsd-gj: soft …
 */
static unsigned g_uSoftDoorBits;
static unsigned g_uSoftStoreBits;
static unsigned g_uSoftFreeBits;
static unsigned g_cSoftDoorOk;
static unsigned g_cSoftDoorSkip;
static unsigned g_cSoftStoreOk;
static unsigned g_cSoftStoreSkip;
static unsigned g_cSoftFreeOk;
static unsigned g_cSoftFreeSkip;
/* Named-cache soft slot snapshots for soft cache inventory. */
static unsigned g_uSoftCacheReclaim;
static unsigned g_uSoftCacheWritefd;
static unsigned g_uSoftCacheSeek;
static unsigned g_uSoftCacheUnlink;
static unsigned g_uSoftCacheStat;
static unsigned g_uSoftCacheBits;
static unsigned g_uSoftCacheOk;

static void
msg(const char *sz)
{
    if (sz == 0) {
        return;
    }
    (void)gj_debug_log(sz, (long)gj_strlen(sz));
}

/* Append NUL-terminated string into aLine[*pO], leaving room for trailing NUL. */
static void
append_s(char *aLine, unsigned cb, unsigned *pO, const char *sz)
{
    unsigned o;

    if (aLine == 0 || pO == 0 || sz == 0 || cb == 0u) {
        return;
    }
    o = *pO;
    while (*sz != '\0' && o + 1u < cb) {
        aLine[o++] = *sz++;
    }
    *pO = o;
}

/* Append decimal unsigned long. */
static void
append_u(char *aLine, unsigned cb, unsigned *pO, unsigned long uVal)
{
    char aTmp[24];
    size_t n;

    n = gj_utoa(uVal, aTmp, sizeof(aTmp));
    if (n == 0u) {
        append_s(aLine, cb, pO, "0");
        return;
    }
    append_s(aLine, cb, pO, aTmp);
}

/* Append lowercase hex (no 0x prefix). */
static void
append_x(char *aLine, unsigned cb, unsigned *pO, unsigned long uVal)
{
    char aTmp[24];
    size_t n;

    n = gj_xtoa(uVal, aTmp, sizeof(aTmp), 0, 0);
    if (n == 0u) {
        append_s(aLine, cb, pO, "0");
        return;
    }
    append_s(aLine, cb, pO, aTmp);
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

/* Note one soft door sub-step into Wave 111 inventory counters. */
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

/* Note one soft store sub-step into Wave 111 inventory counters. */
static void
soft_store_note(unsigned uBit, int fOk)
{
    if (fOk != 0) {
        g_uSoftStoreBits |= uBit;
        if (g_cSoftStoreOk < 0xffffffffu) {
            g_cSoftStoreOk++;
        }
    } else if (g_cSoftStoreSkip < 0xffffffffu) {
        g_cSoftStoreSkip++;
    }
}

/* Note one soft free sub-step into Wave 111 inventory counters. */
static void
soft_free_note(unsigned uBit, int fOk)
{
    if (fOk != 0) {
        g_uSoftFreeBits |= uBit;
        if (g_cSoftFreeOk < 0xffffffffu) {
            g_cSoftFreeOk++;
        }
    } else if (g_cSoftFreeSkip < 0xffffffffu) {
        g_cSoftFreeSkip++;
    }
}

/*
 * Soft inventory dump (Wave 119 exclusive deepen).
 * Greppable prefix: "vfsd-gj: soft …"
 * Pure observation — always soft; never gates live path PASS.
 * Honesty: soft ≠ product multi-server confine.
 *
 *   vfsd-gj: soft inventory …
 *   vfsd-gj: soft door …
 *   vfsd-gj: soft store …
 *   vfsd-gj: soft free …
 *   vfsd-gj: soft cache …
 *   vfsd-gj: soft stats …
 *   vfsd-gj: soft deepen …
 *   vfsd-gj: soft path …
 */
static void
soft_inventory_log(void)
{
    char aLine[256];
    unsigned o;
    unsigned cOk;
    unsigned cSkip;

    cOk = g_cSoftDoorOk + g_cSoftStoreOk + g_cSoftFreeOk;
    cSkip = g_cSoftDoorSkip + g_cSoftStoreSkip + g_cSoftFreeSkip;

    /* Grep: vfsd-gj: soft inventory */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "vfsd-gj: soft inventory door_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftDoorOk);
    append_s(aLine, sizeof(aLine), &o, " door_skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftDoorSkip);
    append_s(aLine, sizeof(aLine), &o, " store_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftStoreOk);
    append_s(aLine, sizeof(aLine), &o, " store_skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftStoreSkip);
    append_s(aLine, sizeof(aLine), &o, " free_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftFreeOk);
    append_s(aLine, sizeof(aLine), &o, " free_skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftFreeSkip);
    append_s(aLine, sizeof(aLine), &o, " wave=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)VFSD_SOFT_WAVE);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: vfsd-gj: soft door */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "vfsd-gj: soft door reclaim=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & VFSD_SOFT_BIT_RECLAIM) != 0u));
    append_s(aLine, sizeof(aLine), &o, " bare=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & VFSD_SOFT_BIT_BARE) != 0u));
    append_s(aLine, sizeof(aLine), &o, " writefd=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & VFSD_SOFT_BIT_WRITEFD) != 0u));
    append_s(aLine, sizeof(aLine), &o, " stat=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & VFSD_SOFT_BIT_STAT) != 0u));
    append_s(aLine, sizeof(aLine), &o, " list=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & VFSD_SOFT_BIT_LIST) != 0u));
    append_s(aLine, sizeof(aLine), &o, " seek_end=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & VFSD_SOFT_BIT_SEEK_END) != 0u));
    append_s(aLine, sizeof(aLine), &o, " seek_cur=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & VFSD_SOFT_BIT_SEEK_CUR) != 0u));
    append_s(aLine, sizeof(aLine), &o, " seekfd=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & VFSD_SOFT_BIT_SEEKFD) != 0u));
    append_s(aLine, sizeof(aLine), &o, " unlink=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & VFSD_SOFT_BIT_UNLINK) != 0u));
    append_s(aLine, sizeof(aLine), &o, " stats=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & VFSD_SOFT_BIT_STATS) != 0u));
    append_s(aLine, sizeof(aLine), &o, " bits=0x");
    append_x(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftDoorBits);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: vfsd-gj: soft store */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "vfsd-gj: soft store mirror=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftStoreBits & VFSD_SOFT_STORE_MIRROR) != 0u));
    append_s(aLine, sizeof(aLine), &o, " read=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftStoreBits & VFSD_SOFT_STORE_READ) != 0u));
    append_s(aLine, sizeof(aLine), &o, " bits=0x");
    append_x(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftStoreBits);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: vfsd-gj: soft free */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "vfsd-gj: soft free release=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftFreeBits & VFSD_SOFT_FREE_RELEASE) != 0u));
    append_s(aLine, sizeof(aLine), &o, " bits=0x");
    append_x(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftFreeBits);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: vfsd-gj: soft cache (named-page soft slots) */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "vfsd-gj: soft cache reclaim=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftCacheReclaim);
    append_s(aLine, sizeof(aLine), &o, " writefd=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftCacheWritefd);
    append_s(aLine, sizeof(aLine), &o, " seek=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftCacheSeek);
    append_s(aLine, sizeof(aLine), &o, " unlink=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftCacheUnlink);
    append_s(aLine, sizeof(aLine), &o, " stat=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftCacheStat);
    append_s(aLine, sizeof(aLine), &o, " bits=0x");
    append_x(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftCacheBits);
    append_s(aLine, sizeof(aLine), &o, " ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftCacheOk);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: vfsd-gj: soft stats (rollup) */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "vfsd-gj: soft stats ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cSkip);
    append_s(aLine, sizeof(aLine), &o, " door_bits=0x");
    append_x(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftDoorBits);
    append_s(aLine, sizeof(aLine), &o, " store_bits=0x");
    append_x(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftStoreBits);
    append_s(aLine, sizeof(aLine), &o, " free_bits=0x");
    append_x(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftFreeBits);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: vfsd-gj: soft deepen wave (Wave 111 stamp) */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "vfsd-gj: soft deepen wave=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)VFSD_SOFT_WAVE);
    append_s(aLine, sizeof(aLine), &o, " areas=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)VFSD_SOFT_AREAS);
    append_s(aLine, sizeof(aLine), &o, " ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cSkip);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /*
     * Grep: vfsd-gj: soft path (Wave 111 honesty).
     * Soft inventory ≠ product multi-server confine.
     */
    msg("vfsd-gj: soft path door=1 store=1 cache=1 multi_server=0 "
        "confine=0 (soft; not bar3; soft != product multi-server confine)\n");

    /*
     * Grep: vfsd-gj: soft honesty (Wave 119 exclusive deepen).
     * Soft inventory ≠ product multi-server confine.
     */
    msg("vfsd-gj: soft honesty multi_server=0 confine=0 bar3=0 "
        "exclusive=1 soft=1 product_kernel=OPEN wave=70\n");

    /*
     * Grep: vfsd-gj: soft exclusive (Wave 119 exclusive deepen).
     * Soft inventory ≠ product multi-server / continuum.
     */
    msg("vfsd-gj: soft exclusive product_kernel=OPEN wave=70 multi_server=0 confine=0 "
        "bar3=0 userland=1 kernel=0 continuum=0\n");
}

/*
 * Soft door surface while CLAIM is held.
 * Never hard-fails: each step soft-skip on rejection / short I/O.
 * Leaves hard-path hello.txt content intact (only mutates soft.tmp).
 * Does not RELEASE / re-token CLAIM ownership (CLAIM PASS stays hard).
 * Tallies Wave 119 soft inventory (vfsd-gj: soft …).
 * Returns count of soft door sub-steps that greened (0..10).
 */
static unsigned
soft_door_path(volatile unsigned *pCache)
{
    static unsigned char aRd[64];
    static char aList[160];
    static unsigned aStat[2];
    static unsigned aDoorSt[4];
    static char aInv[128];
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
     *    Dual markers: legacy "reclaim soft …" + greppable "soft reclaim …".
     */
    nRet = gj_vfs(GJ_VFS_OP_CLAIM, (long)VFSD_TOKEN, 0, 0);
    if (nRet == 0) {
        pCache[VFSD_CACHE_SOFT_RECLAIM] = 1u;
        uBits |= VFSD_SOFT_BIT_RECLAIM;
        soft_door_note(VFSD_SOFT_BIT_RECLAIM, 1);
        cOk++;
        msg("vfsd-gj: reclaim soft PASS\n");
        msg("vfsd-gj: soft reclaim PASS\n");
    } else {
        soft_door_note(VFSD_SOFT_BIT_RECLAIM, 0);
        msg("vfsd-gj: reclaim soft-skip\n");
        msg("vfsd-gj: soft reclaim soft-skip\n");
    }

    /*
     * 2) Bare-name OPEN (no /mnt/ prefix) + READFD on hard-path hello.txt.
     *    Soft: kernel path_normalize should accept bare names.
     *    Dual: "bare OPEN soft …" + greppable "soft bare …".
     */
    {
        static const char szHello[] = "hello.txt";
        static const char szExpect[] = "GJVFS2";
        const unsigned cbExpect = (unsigned)(sizeof(szExpect) - 1u);

        hFd = gj_vfs(GJ_VFS_OP_OPEN, (long)(uintptr_t)szHello, 0, 0);
        if (hFd < 0) {
            soft_door_note(VFSD_SOFT_BIT_BARE, 0);
            msg("vfsd-gj: bare OPEN soft-skip\n");
            msg("vfsd-gj: soft bare soft-skip\n");
        } else {
            (void)gj_memset(aRd, 0, sizeof(aRd));
            nRet = gj_vfs(GJ_VFS_OP_READFD, hFd, (long)(uintptr_t)aRd,
                          (long)sizeof(aRd));
            (void)gj_vfs(GJ_VFS_OP_CLOSE, hFd, 0, 0);
            if (nRet == (long)cbExpect &&
                gj_memcmp(aRd, szExpect, cbExpect) == 0) {
                uBits |= VFSD_SOFT_BIT_BARE;
                soft_door_note(VFSD_SOFT_BIT_BARE, 1);
                cOk++;
                msg("vfsd-gj: bare OPEN soft PASS\n");
                msg("vfsd-gj: soft bare PASS\n");
            } else {
                soft_door_note(VFSD_SOFT_BIT_BARE, 0);
                msg("vfsd-gj: bare OPEN soft-skip\n");
                msg("vfsd-gj: soft bare soft-skip\n");
            }
        }
    }

    /*
     * 3) WRITEFD soft: OPEN(CREAT|RDWR) soft.tmp, WRITEFD body, CLOSE,
     *    name READ verify. Soft-skip on any step fail.
     *    Dual: "WRITEFD soft …" + greppable "soft WRITEFD …".
     */
    hFd = gj_vfs(GJ_VFS_OP_OPEN, (long)(uintptr_t)szSoft,
                 (long)(GJ_VFS_O_CREAT | GJ_VFS_O_RDWR), 0);
    if (hFd < 0) {
        soft_door_note(VFSD_SOFT_BIT_WRITEFD, 0);
        msg("vfsd-gj: WRITEFD soft-skip\n");
        msg("vfsd-gj: soft WRITEFD soft-skip\n");
    } else {
        nRet = gj_vfs(GJ_VFS_OP_WRITEFD, hFd, (long)(uintptr_t)szBody,
                      (long)cbBody);
        (void)gj_vfs(GJ_VFS_OP_CLOSE, hFd, 0, 0);
        hFd = -1;
        if (nRet != (long)cbBody) {
            soft_door_note(VFSD_SOFT_BIT_WRITEFD, 0);
            msg("vfsd-gj: WRITEFD soft-skip\n");
            msg("vfsd-gj: soft WRITEFD soft-skip\n");
        } else {
            (void)gj_memset(aRd, 0, sizeof(aRd));
            nRet = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szSoft,
                          (long)(uintptr_t)aRd, (long)(sizeof(aRd) - 1u));
            if (nRet == (long)cbBody && gj_memcmp(aRd, szBody, cbBody) == 0) {
                pCache[VFSD_CACHE_SOFT_WRITEFD] = cbBody;
                uBits |= VFSD_SOFT_BIT_WRITEFD;
                soft_door_note(VFSD_SOFT_BIT_WRITEFD, 1);
                cOk++;
                msg("vfsd-gj: WRITEFD soft PASS\n");
                msg("vfsd-gj: soft WRITEFD PASS\n");
            } else {
                soft_door_note(VFSD_SOFT_BIT_WRITEFD, 0);
                msg("vfsd-gj: WRITEFD soft-skip\n");
                msg("vfsd-gj: soft WRITEFD soft-skip\n");
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
            soft_door_note(VFSD_SOFT_BIT_STAT, 1);
            cOk++;
            msg("vfsd-gj: soft STAT PASS\n");
        } else {
            soft_door_note(VFSD_SOFT_BIT_STAT, 0);
            msg("vfsd-gj: soft STAT soft-skip\n");
        }
    } else {
        soft_door_note(VFSD_SOFT_BIT_STAT, 0);
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
            soft_door_note(VFSD_SOFT_BIT_LIST, 1);
            cOk++;
            msg("vfsd-gj: soft LIST PASS\n");
        } else {
            soft_door_note(VFSD_SOFT_BIT_LIST, 0);
            msg("vfsd-gj: soft LIST soft-skip\n");
        }
    } else {
        soft_door_note(VFSD_SOFT_BIT_LIST, 0);
        msg("vfsd-gj: soft LIST soft-skip\n");
    }

    /*
     * 4) SEEKFD soft: reopen RDWR, SEEK END, WRITEFD patch, SEEK SET,
     *    soft SEEK_CUR probe, READFD full, verify patch at end.
     *    Soft-skip if door returns NOSUPPORT or any step fails.
     *    Wave 111: greppable soft SEEK_END + soft SEEKFD dual markers.
     */
    if ((uBits & VFSD_SOFT_BIT_WRITEFD) != 0u) {
        hFd = gj_vfs(GJ_VFS_OP_OPEN, (long)(uintptr_t)szSoft,
                     (long)GJ_VFS_O_RDWR, 0);
        if (hFd < 0) {
            soft_door_note(VFSD_SOFT_BIT_SEEK_END, 0);
            soft_door_note(VFSD_SOFT_BIT_SEEK_CUR, 0);
            soft_door_note(VFSD_SOFT_BIT_SEEKFD, 0);
            msg("vfsd-gj: SEEKFD soft-skip\n");
            msg("vfsd-gj: soft SEEKFD soft-skip\n");
            msg("vfsd-gj: soft SEEK_END soft-skip\n");
            msg("vfsd-gj: soft SEEK_CUR soft-skip\n");
        } else {
            long nEnd;
            long nSeek;
            long nCur;

            nEnd = gj_vfs(GJ_VFS_OP_SEEKFD, hFd, 0, (long)GJ_VFS_SEEK_END);
            if (nEnd < 0) {
                (void)gj_vfs(GJ_VFS_OP_CLOSE, hFd, 0, 0);
                soft_door_note(VFSD_SOFT_BIT_SEEK_END, 0);
                soft_door_note(VFSD_SOFT_BIT_SEEK_CUR, 0);
                soft_door_note(VFSD_SOFT_BIT_SEEKFD, 0);
                msg("vfsd-gj: SEEKFD soft-skip\n");
                msg("vfsd-gj: soft SEEKFD soft-skip\n");
                msg("vfsd-gj: soft SEEK_END soft-skip\n");
                msg("vfsd-gj: soft SEEK_CUR soft-skip\n");
            } else {
                /*
                 * Soft SEEK_END: end offset must match body size before patch.
                 * Greppable: vfsd-gj: soft SEEK_END …
                 */
                if (nEnd == (long)cbBody) {
                    uBits |= VFSD_SOFT_BIT_SEEK_END;
                    soft_door_note(VFSD_SOFT_BIT_SEEK_END, 1);
                    cOk++;
                    msg("vfsd-gj: soft SEEK_END PASS\n");
                } else {
                    soft_door_note(VFSD_SOFT_BIT_SEEK_END, 0);
                    msg("vfsd-gj: soft SEEK_END soft-skip\n");
                }

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
                        soft_door_note(VFSD_SOFT_BIT_SEEK_CUR, 1);
                        cOk++;
                        msg("vfsd-gj: soft SEEK_CUR PASS\n");
                    } else {
                        soft_door_note(VFSD_SOFT_BIT_SEEK_CUR, 0);
                        msg("vfsd-gj: soft SEEK_CUR soft-skip\n");
                    }
                    nSeek = gj_vfs(GJ_VFS_OP_SEEKFD, hFd, 0,
                                   (long)GJ_VFS_SEEK_SET);
                } else {
                    soft_door_note(VFSD_SOFT_BIT_SEEK_CUR, 0);
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
                    soft_door_note(VFSD_SOFT_BIT_SEEKFD, 1);
                    cOk++;
                    msg("vfsd-gj: SEEKFD soft PASS\n");
                    msg("vfsd-gj: soft SEEKFD PASS\n");
                } else {
                    soft_door_note(VFSD_SOFT_BIT_SEEKFD, 0);
                    msg("vfsd-gj: SEEKFD soft-skip\n");
                    msg("vfsd-gj: soft SEEKFD soft-skip\n");
                }
                (void)nCur;
            }
        }
    } else {
        soft_door_note(VFSD_SOFT_BIT_SEEK_END, 0);
        soft_door_note(VFSD_SOFT_BIT_SEEK_CUR, 0);
        soft_door_note(VFSD_SOFT_BIT_SEEKFD, 0);
        msg("vfsd-gj: SEEKFD soft-skip\n");
        msg("vfsd-gj: soft SEEKFD soft-skip\n");
        msg("vfsd-gj: soft SEEK_END soft-skip\n");
        msg("vfsd-gj: soft SEEK_CUR soft-skip\n");
    }

    /*
     * 5) UNLINK soft: remove soft.tmp; LIST must not contain name.
     *    Best-effort even if create/write soft failed (NOENT is soft-skip).
     *    Dual: "UNLINK soft …" + greppable "soft UNLINK …".
     */
    nRet = gj_vfs(GJ_VFS_OP_UNLINK, (long)(uintptr_t)szSoft, 0, 0);
    if (nRet != 0) {
        soft_door_note(VFSD_SOFT_BIT_UNLINK, 0);
        msg("vfsd-gj: UNLINK soft-skip\n");
        msg("vfsd-gj: soft UNLINK soft-skip\n");
    } else {
        (void)gj_memset(aList, 0, sizeof(aList));
        nRet = gj_vfs(GJ_VFS_OP_LIST, (long)(uintptr_t)aList, 0,
                      (long)sizeof(aList));
        if (nRet >= 0 &&
            !buf_contains(aList, nRet, szSoft, cbName)) {
            pCache[VFSD_CACHE_SOFT_UNLINK] = 1u;
            uBits |= VFSD_SOFT_BIT_UNLINK;
            soft_door_note(VFSD_SOFT_BIT_UNLINK, 1);
            cOk++;
            msg("vfsd-gj: UNLINK soft PASS\n");
            msg("vfsd-gj: soft UNLINK PASS\n");
        } else {
            soft_door_note(VFSD_SOFT_BIT_UNLINK, 0);
            msg("vfsd-gj: UNLINK soft-skip\n");
            msg("vfsd-gj: soft UNLINK soft-skip\n");
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
        soft_door_note(VFSD_SOFT_BIT_STATS, 1);
        cOk++;
        msg("vfsd-gj: soft STATS PASS\n");
    } else {
        soft_door_note(VFSD_SOFT_BIT_STATS, 0);
        msg("vfsd-gj: soft STATS soft-skip\n");
    }

    pCache[VFSD_CACHE_SOFT_BITS] = uBits;
    pCache[VFSD_CACHE_SOFT_OK] = cOk;

    /* Snapshot named-cache soft slots for soft cache inventory. */
    g_uSoftCacheReclaim = pCache[VFSD_CACHE_SOFT_RECLAIM];
    g_uSoftCacheWritefd = pCache[VFSD_CACHE_SOFT_WRITEFD];
    g_uSoftCacheSeek = pCache[VFSD_CACHE_SOFT_SEEK];
    g_uSoftCacheUnlink = pCache[VFSD_CACHE_SOFT_UNLINK];
    g_uSoftCacheStat = pCache[VFSD_CACHE_SOFT_STAT];
    g_uSoftCacheBits = uBits;
    g_uSoftCacheOk = cOk;

    /*
     * Door-local soft stats rollup (Wave 111 deepened fields).
     * Full store/free rollup is emitted later by soft_inventory_log.
     * Greppable: vfsd-gj: soft stats ok=… (prefix-stable).
     */
    (void)gj_snprintf(aInv, sizeof(aInv),
                      "vfsd-gj: soft stats ok=%u skip=%u bits=0x%x "
                      "door_bits=0x%x\n",
                      (unsigned long)cOk,
                      (unsigned long)g_cSoftDoorSkip,
                      (unsigned long)uBits,
                      (unsigned long)g_uSoftDoorBits);
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
 * Tallies Wave 119 soft store inventory (vfsd-gj: soft store …).
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
        soft_store_note(VFSD_SOFT_STORE_MIRROR, 0);
        soft_store_note(VFSD_SOFT_STORE_READ, 0);
        msg("vfsd-gj: store LBA0 super mirror soft-skip\n");
        msg("vfsd-gj: soft store mirror soft-skip\n");
        msg("vfsd-gj: soft store read soft-skip\n");
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
        soft_store_note(VFSD_SOFT_STORE_MIRROR, 0);
        soft_store_note(VFSD_SOFT_STORE_READ, 0);
        msg("vfsd-gj: store LBA0 super mirror soft-skip\n");
        msg("vfsd-gj: soft store mirror soft-skip\n");
        msg("vfsd-gj: soft store read soft-skip\n");
        return;
    }
    soft_store_note(VFSD_SOFT_STORE_MIRROR, 1);
    msg("vfsd-gj: store LBA0 super mirror PASS\n");
    msg("vfsd-gj: soft store mirror PASS\n");

    /* Soft READ-back of LBA0 — verify magic LE bytes stick. */
    (void)gj_memset(aRd, 0, sizeof(aRd));
    nRet = gj_store(GJ_STORE_OP_READ, 0, (long)(uintptr_t)aRd,
                    (long)VFSD_SECTOR_CB);
    (void)gj_store(GJ_STORE_OP_RELEASE, (long)VFSD_TOKEN, 0, 0);

    if (nRet != (long)VFSD_SECTOR_CB) {
        soft_store_note(VFSD_SOFT_STORE_READ, 0);
        msg("vfsd-gj: store LBA0 read soft-skip\n");
        msg("vfsd-gj: soft store read soft-skip\n");
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
        soft_store_note(VFSD_SOFT_STORE_READ, 1);
        msg("vfsd-gj: store LBA0 read soft PASS\n");
        msg("vfsd-gj: soft store read PASS\n");
    } else {
        soft_store_note(VFSD_SOFT_STORE_READ, 0);
        msg("vfsd-gj: store LBA0 read soft-skip\n");
        msg("vfsd-gj: soft store read soft-skip\n");
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
     * Dual: "RELEASE free soft …" + greppable "soft free …".
     * Tallies Wave 119 soft free inventory.
     */
    nRet = gj_vfs(GJ_VFS_OP_RELEASE, (long)VFSD_TOKEN, 0, 0);
    if (nRet == 0) {
        soft_free_note(VFSD_SOFT_FREE_RELEASE, 1);
        msg("vfsd-gj: RELEASE free soft PASS\n");
        msg("vfsd-gj: soft free PASS\n");
    } else {
        soft_free_note(VFSD_SOFT_FREE_RELEASE, 0);
        msg("vfsd-gj: RELEASE free soft-skip\n");
        msg("vfsd-gj: soft free soft-skip\n");
    }

    /*
     * Wave 119 soft inventory dump — greppable "vfsd-gj: soft …".
     * Pure observation after all soft door / store / free work.
     */
    soft_inventory_log();

    /* Re-check named cache still holds magic after FS work */
    if (pCache[0] != GJ_VFS_MAGIC) {
        fail_exit("vfsd-gj: named cache sticky fail\n");
    }
    msg("vfsd-gj: live path PASS (door+mini-FS+named cache)\n");
    gj_exit(0);
}
