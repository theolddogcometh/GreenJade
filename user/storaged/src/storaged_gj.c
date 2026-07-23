/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding storaged — product block host over the kernel store door.
 *
 * Live path (order fixed for smoke greps):
 *   CLAIM → soft door surface → WRITE/READ sector smoke → soft multi-I/O →
 *   UDX ring EXPORT/MAP/STATE/KICK → RELEASE → soft free → live path PASS
 *
 * Store-door ops used here (must match kernel/include/gj/store_door.h and
 * the GJ_STORE_OP_* subset in user/libgj/include/gj/syscalls.h):
 *   STATS=1  CAP=2  READ=3  WRITE=4  CLAIM=5  RELEASE=6
 *   QUEUE_INFO=7  FLUSH=8
 *   EXPORT_RING=9  KICK=10  RING_STATE=11  MAP_RING=12
 *
 * Soft steps never hard-fail the live path. Ring steps soft-skip when
 * virtio-blk is not ready so host/QEMU without blk still green on non-hard
 * markers. Multiboot smoke with a ready ring expects the exact substring
 * "ring map PASS".
 *
 * Smoke markers (prefix-stable for scripts/smoke-all.sh — do not rename):
 *   storaged-gj: CLAIM PASS
 *   storaged-gj: WRITE PASS
 *   storaged-gj: READ PASS
 *   storaged-gj: ring map PASS
 *   storaged-gj: RELEASE PASS
 *   storaged-gj: live path PASS
 *
 * Soft / non-hard lines (not required alone by smoke-all):
 *   storaged-gj: reclaim soft PASS | reclaim soft-skip
 *   storaged-gj: CAP soft …
 *   storaged-gj: STATS soft …
 *   storaged-gj: QUEUE soft …
 *   storaged-gj: multi-sector soft PASS | multi-sector soft-skip
 *   storaged-gj: FLUSH soft PASS | FLUSH soft-skip
 *   storaged-gj: RING_STATE soft …
 *   storaged-gj: soft door PASS | soft door soft-skip
 *   storaged-gj: ring EXPORT soft-skip
 *   storaged-gj: ring MAP fail (soft)
 *   storaged-gj: ring soft-skip (no virtio-blk)
 *   storaged-gj: free soft PASS | free soft-skip
 *
 *   make storaged-gj → build/user/storaged.elf
 * Boot embed (parent tree): kernel/proc/storaged_embed.S (.incbin of the ELF).
 *
 * Pure C11 freestanding. Dual-licensed MIT OR Apache-2.0 (no GPL).
 */
#include <gj/string.h>
#include <gj/syscalls.h>

/* Local fallbacks if libgj headers lag the door surface (keep op numbers fixed). */
#ifndef GJ_STORE_OP_STATS
#define GJ_STORE_OP_STATS      1u
#define GJ_STORE_OP_CAP        2u
#define GJ_STORE_OP_READ       3u
#define GJ_STORE_OP_WRITE      4u
#define GJ_STORE_OP_CLAIM      5u
#define GJ_STORE_OP_RELEASE    6u
#define GJ_STORE_OP_QUEUE_INFO 7u
#define GJ_STORE_OP_FLUSH      8u
#endif
#ifndef GJ_STORE_OP_EXPORT_RING
#define GJ_STORE_OP_EXPORT_RING 9u
#define GJ_STORE_OP_KICK        10u
#define GJ_STORE_OP_RING_STATE  11u
#define GJ_STORE_OP_MAP_RING    12u
#endif

/* User VA for MAP_RING — page-aligned; must stay stable for UDX / kernel map. */
#define RING_VA       0x32000000ul
#define PAGE_BYTES    4096u
#define SECTOR_BYTES  512u
#define SMOKE_LBA     2u
/* Soft multi-sector: LBA 3, 2 sectors (≤ GJ_STORE_XFER_MAX 4096; avoid LBA0/2). */
#define SOFT_LBA      3u
#define SOFT_SECTS    2u
#define SOFT_BYTES    (SOFT_SECTS * SECTOR_BYTES)
/* Store-door ownership token (storaged product claim; non-zero). */
#define STORE_TOKEN   0x510e0002u

/* Soft sub-step bits (aggregate soft door PASS if any greened). */
#define SOFT_BIT_RECLAIM  (1u << 0)
#define SOFT_BIT_CAP      (1u << 1)
#define SOFT_BIT_STATS    (1u << 2)
#define SOFT_BIT_QUEUE    (1u << 3)
#define SOFT_BIT_MULTI    (1u << 4)
#define SOFT_BIT_FLUSH    (1u << 5)
#define SOFT_BIT_RSTATE   (1u << 6)

/*
 * Layout mirrors kernel gj_virtq_export / udx_virtq_export.
 * Field order and widths are ABI with store_door EXPORT/MAP_RING — do not
 * reorder without coordinating kernel/fs/store_door.c and UDX.
 */
struct vq_export {
    unsigned short which;
    unsigned short size;
    unsigned short queue_idx;
    unsigned short notify_off;
    unsigned long  pa_desc;
    unsigned long  pa_avail;
    unsigned long  pa_used;
    unsigned       notify_mult;
    unsigned       ready;
    unsigned       off_desc;
    unsigned       off_avail;
    unsigned       off_used;
    unsigned short free_head;
    unsigned short num_free;
};

/* Non-zero while CLAIM holds the door; cleared on RELEASE or fail_exit. */
static unsigned g_uToken;

static void
msg(const char *sz)
{
    if (sz == 0) {
        return;
    }
    (void)gj_debug_log(sz, (long)gj_strlen(sz));
}

/* Append a NUL-terminated string into aLine; o is write cursor. */
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

/*
 * Hard failure: best-effort RELEASE if we still own the door, then exit 1.
 * Soft ring / soft door skips must never call this.
 */
static void
fail_exit(const char *szWhy)
{
    if (szWhy != 0) {
        msg(szWhy);
    }
    if (g_uToken != 0u) {
        (void)gj_store(GJ_STORE_OP_RELEASE, (long)g_uToken, 0, 0);
        g_uToken = 0u;
    }
    gj_exit(1);
}

/* True when export looks ready for MAP_RING (virtio-blk present). */
static int
export_ready(const struct vq_export *pEx)
{
    if (pEx == 0) {
        return 0;
    }
    if (pEx->ready == 0u || pEx->size == 0u) {
        return 0;
    }
    /* Defensive: queue size should be a power of two in virtio; tolerate others. */
    if (pEx->size > 4096u) {
        return 0;
    }
    return 1;
}

/* "storaged-gj: STATS soft blk= scsi= calls=\n" — aSt[3] wire layout. */
static void
msg_stats_soft(const unsigned *aSt)
{
    char aLine[96];
    unsigned o = 0u;

    if (aSt == 0) {
        return;
    }
    append_s(aLine, sizeof(aLine), &o, "storaged-gj: STATS soft blk=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)aSt[0]);
    append_s(aLine, sizeof(aLine), &o, " scsi=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)aSt[1]);
    append_s(aLine, sizeof(aLine), &o, " calls=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)aSt[2]);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);
}

/* "storaged-gj: QUEUE soft blk= scsi= rw= own=\n" — aQ[4] wire layout. */
static void
msg_queue_soft(const unsigned *aQ)
{
    char aLine[96];
    unsigned o = 0u;

    if (aQ == 0) {
        return;
    }
    append_s(aLine, sizeof(aLine), &o, "storaged-gj: QUEUE soft blk=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)aQ[0]);
    append_s(aLine, sizeof(aLine), &o, " scsi=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)aQ[1]);
    append_s(aLine, sizeof(aLine), &o, " rw=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)aQ[2]);
    append_s(aLine, sizeof(aLine), &o, " own=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)aQ[3]);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);
}

/* "storaged-gj: CAP soft sectors=\n" */
static void
msg_cap_soft(unsigned long long u64Sectors)
{
    char aLine[72];
    unsigned o = 0u;

    append_s(aLine, sizeof(aLine), &o, "storaged-gj: CAP soft sectors=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)u64Sectors);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);
}

/* "storaged-gj: RING_STATE soft free= ready=\n" */
static void
msg_rstate_soft(const unsigned *aSt)
{
    char aLine[80];
    unsigned o = 0u;

    if (aSt == 0) {
        return;
    }
    append_s(aLine, sizeof(aLine), &o, "storaged-gj: RING_STATE soft free=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)aSt[0]);
    append_s(aLine, sizeof(aLine), &o, " ready=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)aSt[1]);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);
}

/*
 * Soft door surface while CLAIM is held (and a light RING_STATE peek).
 * Never hard-fails: each step soft-skips on rejection / short I/O.
 * Leaves hard-path smoke LBA 2 alone (only mutates soft LBA 3..4).
 * Returns count of soft sub-steps that greened.
 */
static unsigned
soft_door_path(void)
{
    static unsigned char aW[SOFT_BYTES];
    static unsigned char aR[SOFT_BYTES];
    static unsigned aStats[3];
    static unsigned aQ[4];
    static unsigned aRs[2];
    static unsigned long long u64Cap;
    unsigned uBits = 0u;
    unsigned cOk = 0u;
    unsigned iByte;
    long nRet;

    /*
     * 1) Soft reclaim: same-token re-CLAIM is idempotent (store_door soft).
     *    Must stay owned after; failure is soft-skip only.
     */
    nRet = gj_store(GJ_STORE_OP_CLAIM, (long)STORE_TOKEN, 0, 0);
    if (nRet == 0) {
        uBits |= SOFT_BIT_RECLAIM;
        cOk++;
        msg("storaged-gj: reclaim soft PASS\n");
    } else {
        msg("storaged-gj: reclaim soft-skip\n");
    }

    /*
     * 2) CAP — capacity in 512-byte sectors. Soft-skip when no blk/scsi
     *    transport (NODEV) or null/fault.
     */
    u64Cap = 0ull;
    nRet = gj_store(GJ_STORE_OP_CAP, (long)(uintptr_t)&u64Cap, 0, 0);
    if (nRet == 0 && u64Cap > 0ull) {
        uBits |= SOFT_BIT_CAP;
        cOk++;
        msg_cap_soft(u64Cap);
    } else {
        msg("storaged-gj: CAP soft-skip\n");
    }

    /*
     * 3) STATS — {blk_io, scsi_io, door_calls}. Soft log; never hard-fail.
     */
    aStats[0] = aStats[1] = aStats[2] = 0u;
    nRet = gj_store(GJ_STORE_OP_STATS, (long)(uintptr_t)aStats, 0, 0);
    if (nRet == 0) {
        uBits |= SOFT_BIT_STATS;
        cOk++;
        msg_stats_soft(aStats);
    } else {
        msg("storaged-gj: STATS soft-skip\n");
    }

    /*
     * 4) QUEUE_INFO — {blk_io, scsi_io, door_rw, owned}. Prefer owned=1
     *    while we hold CLAIM; still soft-PASS on wire success even if own
     *    bit lags (defensive).
     */
    aQ[0] = aQ[1] = aQ[2] = aQ[3] = 0u;
    nRet = gj_store(GJ_STORE_OP_QUEUE_INFO, (long)(uintptr_t)aQ, 0, 0);
    if (nRet == 0) {
        uBits |= SOFT_BIT_QUEUE;
        cOk++;
        msg_queue_soft(aQ);
    } else {
        msg("storaged-gj: QUEUE soft-skip\n");
    }

    /*
     * 5) Multi-sector soft R/W at SOFT_LBA (2 × 512 B). Distinct pattern
     *    from hard smoke. Soft-skip on short I/O / NODEV / INVAL.
     */
    for (iByte = 0; iByte < SOFT_BYTES; iByte++) {
        aW[iByte] = (unsigned char)(0xA5u ^ (unsigned char)(iByte & 0xffu));
    }
    (void)gj_memset(aR, 0, sizeof(aR));
    nRet = gj_store(GJ_STORE_OP_WRITE, (long)SOFT_LBA, (long)(uintptr_t)aW,
                    (long)SOFT_BYTES);
    if (nRet != (long)SOFT_BYTES) {
        msg("storaged-gj: multi-sector soft-skip\n");
    } else {
        nRet = gj_store(GJ_STORE_OP_READ, (long)SOFT_LBA, (long)(uintptr_t)aR,
                        (long)SOFT_BYTES);
        if (nRet == (long)SOFT_BYTES &&
            gj_memcmp(aR, aW, SOFT_BYTES) == 0) {
            uBits |= SOFT_BIT_MULTI;
            cOk++;
            msg("storaged-gj: multi-sector soft PASS\n");
        } else {
            msg("storaged-gj: multi-sector soft-skip\n");
        }
    }

    /*
     * 6) FLUSH — fsync-shaped when a block transport is ready; soft-skip
     *    NODEV without virtio-blk / scsi_mid.
     */
    nRet = gj_store(GJ_STORE_OP_FLUSH, 0, 0, 0);
    if (nRet == 0) {
        uBits |= SOFT_BIT_FLUSH;
        cOk++;
        msg("storaged-gj: FLUSH soft PASS\n");
    } else {
        msg("storaged-gj: FLUSH soft-skip\n");
    }

    /*
     * 7) RING_STATE soft peek — door always fills {free, ready}; ready=0
     *    without virtio-blk. Independent of EXPORT/MAP success.
     */
    aRs[0] = aRs[1] = 0u;
    nRet = gj_store(GJ_STORE_OP_RING_STATE, (long)(uintptr_t)aRs, 0, 0);
    if (nRet == 0) {
        uBits |= SOFT_BIT_RSTATE;
        cOk++;
        msg_rstate_soft(aRs);
    } else {
        msg("storaged-gj: RING_STATE soft-skip\n");
    }

    /* Aggregate soft door line — green if any sub-step greened. */
    (void)uBits;
    if (cOk > 0u) {
        msg("storaged-gj: soft door PASS\n");
    } else {
        msg("storaged-gj: soft door soft-skip\n");
    }
    return cOk;
}

/*
 * Soft free path after RELEASE: already-unowned RELEASE is 0; QUEUE owned
 * should drop. Never hard-fails live path.
 */
static void
soft_free_path(void)
{
    static unsigned aQ[4];
    long nRet;
    unsigned cOk = 0u;

    /* Soft free RELEASE: door returns 0 when unowned (no token match). */
    nRet = gj_store(GJ_STORE_OP_RELEASE, (long)STORE_TOKEN, 0, 0);
    if (nRet == 0) {
        cOk++;
        msg("storaged-gj: free RELEASE soft PASS\n");
    } else {
        msg("storaged-gj: free RELEASE soft-skip\n");
    }

    aQ[0] = aQ[1] = aQ[2] = aQ[3] = 0u;
    nRet = gj_store(GJ_STORE_OP_QUEUE_INFO, (long)(uintptr_t)aQ, 0, 0);
    if (nRet == 0 && aQ[3] == 0u) {
        cOk++;
        msg_queue_soft(aQ);
        msg("storaged-gj: free own soft PASS\n");
    } else if (nRet == 0) {
        msg_queue_soft(aQ);
        msg("storaged-gj: free own soft-skip\n");
    } else {
        msg("storaged-gj: free own soft-skip\n");
    }

    if (cOk > 0u) {
        msg("storaged-gj: free soft PASS\n");
    } else {
        msg("storaged-gj: free soft-skip\n");
    }
}

void
_start(void)
{
    static unsigned char aW[SECTOR_BYTES];
    static unsigned char aR[SECTOR_BYTES];
    static struct vq_export ex;
    static unsigned aSt[2]; /* [0]=free, [1]=ready — RING_STATE */
    unsigned iByte;
    long nRet;

    /* Shape checks (freestanding: no <assert.h>). */
    if ((RING_VA & (PAGE_BYTES - 1u)) != 0ul) {
        fail_exit("storaged-gj: RING_VA not page-aligned\n");
    }
    if ((SECTOR_BYTES % 512u) != 0u || SECTOR_BYTES == 0u) {
        fail_exit("storaged-gj: SECTOR_BYTES invalid\n");
    }
    if ((SOFT_BYTES % SECTOR_BYTES) != 0u || SOFT_BYTES == 0u ||
        SOFT_BYTES > 4096u) {
        fail_exit("storaged-gj: SOFT_BYTES invalid\n");
    }
    if (STORE_TOKEN == 0u) {
        fail_exit("storaged-gj: STORE_TOKEN zero\n");
    }

    g_uToken = 0u;
    msg("storaged-gj: start\n");

    if (gj_store(GJ_STORE_OP_CLAIM, (long)STORE_TOKEN, 0, 0) != 0) {
        fail_exit("storaged-gj: CLAIM fail\n");
    }
    g_uToken = STORE_TOKEN;
    msg("storaged-gj: CLAIM PASS\n");

    /* Soft door surface while owned — never aborts live path. */
    (void)soft_door_path();

    /* Sector smoke pattern — deterministic, non-zero, LBA 2 (avoid LBA0 super). */
    for (iByte = 0; iByte < SECTOR_BYTES; iByte++) {
        aW[iByte] = (unsigned char)(0x33u + (iByte & 0x0fu));
    }
    (void)gj_memset(aR, 0, sizeof(aR));

    nRet = gj_store(GJ_STORE_OP_WRITE, (long)SMOKE_LBA, (long)(uintptr_t)aW,
                    (long)SECTOR_BYTES);
    if (nRet != (long)SECTOR_BYTES) {
        fail_exit("storaged-gj: WRITE fail\n");
    }
    msg("storaged-gj: WRITE PASS\n");

    nRet = gj_store(GJ_STORE_OP_READ, (long)SMOKE_LBA, (long)(uintptr_t)aR,
                    (long)SECTOR_BYTES);
    if (nRet != (long)SECTOR_BYTES) {
        fail_exit("storaged-gj: READ/verify fail\n");
    }
    /* Full-sector verify (defensive: partial fill must not PASS). */
    for (iByte = 0; iByte < SECTOR_BYTES; iByte++) {
        if (aR[iByte] != aW[iByte]) {
            fail_exit("storaged-gj: READ/verify fail\n");
        }
    }
    msg("storaged-gj: READ PASS\n");

    /*
     * UDX ring hand-off: EXPORT → MAP → RING_STATE → KICK.
     * Soft-skip entire ring path without virtio-blk (or on MAP fail).
     * Hard path never fails the live smoke solely for ring absence.
     */
    (void)gj_memset(&ex, 0, sizeof(ex));
    aSt[0] = 0u;
    aSt[1] = 0u;

    nRet = gj_store(GJ_STORE_OP_EXPORT_RING, (long)(uintptr_t)&ex, 0, 0);
    if (nRet != 0) {
        msg("storaged-gj: ring EXPORT soft-skip\n");
    } else if (!export_ready(&ex)) {
        msg("storaged-gj: ring soft-skip (no virtio-blk)\n");
    } else if (gj_store(GJ_STORE_OP_MAP_RING, (long)RING_VA,
                        (long)(uintptr_t)&ex, 0) != 0) {
        msg("storaged-gj: ring MAP fail (soft)\n");
    } else {
        /* Best-effort state snapshot + notify; failures stay soft. */
        (void)gj_store(GJ_STORE_OP_RING_STATE, (long)(uintptr_t)aSt, 0, 0);
        (void)gj_store(GJ_STORE_OP_KICK, 0, 0, 0);
        msg("storaged-gj: ring map PASS\n");
    }

    if (gj_store(GJ_STORE_OP_RELEASE, (long)g_uToken, 0, 0) != 0) {
        g_uToken = 0u;
        fail_exit("storaged-gj: RELEASE fail\n");
    }
    g_uToken = 0u;
    msg("storaged-gj: RELEASE PASS\n");

    /* Soft free path after ownership drop — never aborts live path. */
    soft_free_path();

    msg("storaged-gj: live path PASS\n");
    gj_exit(0);
}
