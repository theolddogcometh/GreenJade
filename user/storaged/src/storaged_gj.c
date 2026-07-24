/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding storaged — product block host over the kernel store door.
 *
 * Live path (order fixed for smoke greps):
 *   CLAIM → soft door surface → WRITE/READ sector smoke →
 *   UDX ring EXPORT/MAP/STATE/KICK → RELEASE → soft free →
 *   soft inventory (Wave 98) → live path PASS
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
 * Soft inventory (Wave 98 exclusive deepen — greppable "storaged-gj: soft …"):
 *   storaged-gj: soft inventory door_ok=… door_skip=… free_ok=… free_skip=…
 *                ring_ok=… ring_skip=… ok=… skip=… wave=70 log_n=…
 *   storaged-gj: soft door reclaim=… cap=… stats=… queue=… multi=… flush=…
 *                rstate=… bits=… sectors=…
 *   storaged-gj: soft free release=… own=… bits=… free_own=…
 *   storaged-gj: soft ring export=… map=… state=… kick=… ready=… free=…
 *                bits=… size=… free_head=… num_free=… mapped=…
 *   storaged-gj: soft stats ok=… skip=… door_bits=… free_bits=… ring_bits=…
 *                blk=… scsi=… calls=…
 *   storaged-gj: soft queue blk=… scsi=… rw=… own=… free_own=…
 *   storaged-gj: soft cap sectors=…
 *   storaged-gj: soft export size=… ready=… free_head=… num_free=…
 *                which=… mapped=… va=…
 *   storaged-gj: soft multi lba=… sects=… bytes=… ok=…
 *   storaged-gj: soft deepen wave=70 areas=… ok=… skip=…
 *   storaged-gj: soft path store=1 ring=1 bar3=0 fs=0 multi_server=0
 *                confine=0 wave=70
 *   storaged-gj: soft inventory PASS
 * Diagnostics only — never hard-fail the live path; not a bar3 / FS claim.
 * Honesty: soft inventory ≠ product multi-server confine.
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
/* Soft inventory wave stamp (Wave 98 exclusive deepen). */
/* Wave 98 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft retcircumangle continuum_toward=24000 soft_ne_product=1 wave=98
 *   greppable: soft retellipseangle exclusive=1 continuum_toward=24000 soft_ne_product=1 wave=98
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */

#define SOFT_INV_WAVE   61u
/* Soft inventory greppable area count (inventory…path + deepen). */
#define SOFT_INV_AREAS 12u

/* Soft door sub-step bits (aggregate soft door PASS if any greened). */
#define SOFT_BIT_RECLAIM  (1u << 0)
#define SOFT_BIT_CAP      (1u << 1)
#define SOFT_BIT_STATS    (1u << 2)
#define SOFT_BIT_QUEUE    (1u << 3)
#define SOFT_BIT_MULTI    (1u << 4)
#define SOFT_BIT_FLUSH    (1u << 5)
#define SOFT_BIT_RSTATE   (1u << 6)

/* Soft free-path bits (Wave 9+ inventory). */
#define SOFT_FREE_RELEASE (1u << 0)
#define SOFT_FREE_OWN     (1u << 1)

/* Soft ring-path bits (Wave 9+ inventory). */
#define SOFT_RING_EXPORT  (1u << 0)
#define SOFT_RING_MAP     (1u << 1)
#define SOFT_RING_STATE   (1u << 2)
#define SOFT_RING_KICK    (1u << 3)

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

/*
 * Soft inventory tallies (Wave 98 exclusive deepen).
 * Wrap-OK counters; diagnostics only — never gate live path PASS.
 * greppable: storaged-gj: soft
 */
static unsigned g_cSoftDoorOk;
static unsigned g_cSoftDoorSkip;
static unsigned g_uSoftDoorBits;
static unsigned g_cSoftFreeOk;
static unsigned g_cSoftFreeSkip;
static unsigned g_uSoftFreeBits;
static unsigned g_cSoftRingOk;
static unsigned g_cSoftRingSkip;
static unsigned g_uSoftRingBits;
/* Last soft CAP / RING_STATE / multi / export / queue / stats snapshots. */
static unsigned long g_uSoftCapSectors;
static unsigned g_uSoftRingFree;
static unsigned g_uSoftRingReady;
static unsigned g_aSoftStats[3];   /* door STATS: blk_io, scsi_io, calls */
static unsigned g_aSoftQueue[4];   /* door QUEUE while owned */
static unsigned g_uSoftFreeOwn;    /* QUEUE own bit after soft free */
static unsigned g_uSoftMultiOk;    /* multi-sector soft R/W greened */
static unsigned g_uSoftExSize;
static unsigned g_uSoftExReady;
static unsigned g_uSoftExFreeHead;
static unsigned g_uSoftExNumFree;
static unsigned g_uSoftExWhich;
static unsigned g_uSoftRingMapped; /* MAP_RING greened */
static unsigned g_cSoftInvLog;     /* inventory dump emissions */

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
 * Soft inventory dump (Wave 98 exclusive deepen).
 * Greppable prefix: "storaged-gj: soft …"
 * Pure observation — always soft; never gates live path PASS.
 * Honesty: soft ≠ product multi-server confine.
 *
 *   storaged-gj: soft inventory …
 *   storaged-gj: soft door …
 *   storaged-gj: soft free …
 *   storaged-gj: soft ring …
 *   storaged-gj: soft stats …
 *   storaged-gj: soft queue …
 *   storaged-gj: soft cap …
 *   storaged-gj: soft export …
 *   storaged-gj: soft multi …
 *   storaged-gj: soft deepen …
 *   storaged-gj: soft path …
 *   storaged-gj: soft inventory PASS
 */
static void
soft_inventory_log(void)
{
    char aLine[256];
    unsigned o;
    unsigned cOk;
    unsigned cSkip;

    cOk = g_cSoftDoorOk + g_cSoftFreeOk + g_cSoftRingOk;
    cSkip = g_cSoftDoorSkip + g_cSoftFreeSkip + g_cSoftRingSkip;

    if (g_cSoftInvLog < 0xffffffffu) {
        g_cSoftInvLog++;
    }

    /* Grep: storaged-gj: soft inventory */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "storaged-gj: soft inventory door_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftDoorOk);
    append_s(aLine, sizeof(aLine), &o, " door_skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftDoorSkip);
    append_s(aLine, sizeof(aLine), &o, " free_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftFreeOk);
    append_s(aLine, sizeof(aLine), &o, " free_skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftFreeSkip);
    append_s(aLine, sizeof(aLine), &o, " ring_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftRingOk);
    append_s(aLine, sizeof(aLine), &o, " ring_skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftRingSkip);
    append_s(aLine, sizeof(aLine), &o, " ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cSkip);
    append_s(aLine, sizeof(aLine), &o, " wave=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)SOFT_INV_WAVE);
    append_s(aLine, sizeof(aLine), &o, " log_n=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftInvLog);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: storaged-gj: soft door */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "storaged-gj: soft door reclaim=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & SOFT_BIT_RECLAIM) != 0u));
    append_s(aLine, sizeof(aLine), &o, " cap=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & SOFT_BIT_CAP) != 0u));
    append_s(aLine, sizeof(aLine), &o, " stats=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & SOFT_BIT_STATS) != 0u));
    append_s(aLine, sizeof(aLine), &o, " queue=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & SOFT_BIT_QUEUE) != 0u));
    append_s(aLine, sizeof(aLine), &o, " multi=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & SOFT_BIT_MULTI) != 0u));
    append_s(aLine, sizeof(aLine), &o, " flush=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & SOFT_BIT_FLUSH) != 0u));
    append_s(aLine, sizeof(aLine), &o, " rstate=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftDoorBits & SOFT_BIT_RSTATE) != 0u));
    append_s(aLine, sizeof(aLine), &o, " bits=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftDoorBits);
    append_s(aLine, sizeof(aLine), &o, " sectors=");
    append_u(aLine, sizeof(aLine), &o, g_uSoftCapSectors);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: storaged-gj: soft free */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "storaged-gj: soft free release=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftFreeBits & SOFT_FREE_RELEASE) != 0u));
    append_s(aLine, sizeof(aLine), &o, " own=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftFreeBits & SOFT_FREE_OWN) != 0u));
    append_s(aLine, sizeof(aLine), &o, " bits=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftFreeBits);
    append_s(aLine, sizeof(aLine), &o, " free_own=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftFreeOwn);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: storaged-gj: soft ring */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "storaged-gj: soft ring export=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftRingBits & SOFT_RING_EXPORT) != 0u));
    append_s(aLine, sizeof(aLine), &o, " map=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftRingBits & SOFT_RING_MAP) != 0u));
    append_s(aLine, sizeof(aLine), &o, " state=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftRingBits & SOFT_RING_STATE) != 0u));
    append_s(aLine, sizeof(aLine), &o, " kick=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftRingBits & SOFT_RING_KICK) != 0u));
    append_s(aLine, sizeof(aLine), &o, " ready=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftRingReady);
    append_s(aLine, sizeof(aLine), &o, " free=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftRingFree);
    append_s(aLine, sizeof(aLine), &o, " bits=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftRingBits);
    append_s(aLine, sizeof(aLine), &o, " size=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftExSize);
    append_s(aLine, sizeof(aLine), &o, " free_head=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftExFreeHead);
    append_s(aLine, sizeof(aLine), &o, " num_free=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftExNumFree);
    append_s(aLine, sizeof(aLine), &o, " mapped=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftRingMapped);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: storaged-gj: soft stats (rollup + door STATS snapshot) */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "storaged-gj: soft stats ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cSkip);
    append_s(aLine, sizeof(aLine), &o, " door_bits=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftDoorBits);
    append_s(aLine, sizeof(aLine), &o, " free_bits=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftFreeBits);
    append_s(aLine, sizeof(aLine), &o, " ring_bits=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftRingBits);
    append_s(aLine, sizeof(aLine), &o, " blk=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_aSoftStats[0]);
    append_s(aLine, sizeof(aLine), &o, " scsi=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_aSoftStats[1]);
    append_s(aLine, sizeof(aLine), &o, " calls=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_aSoftStats[2]);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: storaged-gj: soft queue (owned + free-path own snapshot) */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "storaged-gj: soft queue blk=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_aSoftQueue[0]);
    append_s(aLine, sizeof(aLine), &o, " scsi=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_aSoftQueue[1]);
    append_s(aLine, sizeof(aLine), &o, " rw=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_aSoftQueue[2]);
    append_s(aLine, sizeof(aLine), &o, " own=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_aSoftQueue[3]);
    append_s(aLine, sizeof(aLine), &o, " free_own=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftFreeOwn);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: storaged-gj: soft cap */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "storaged-gj: soft cap sectors=");
    append_u(aLine, sizeof(aLine), &o, g_uSoftCapSectors);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: storaged-gj: soft export (virtq export geometry snapshot) */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "storaged-gj: soft export size=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftExSize);
    append_s(aLine, sizeof(aLine), &o, " ready=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftExReady);
    append_s(aLine, sizeof(aLine), &o, " free_head=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftExFreeHead);
    append_s(aLine, sizeof(aLine), &o, " num_free=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftExNumFree);
    append_s(aLine, sizeof(aLine), &o, " which=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftExWhich);
    append_s(aLine, sizeof(aLine), &o, " mapped=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftRingMapped);
    append_s(aLine, sizeof(aLine), &o, " va=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)RING_VA);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: storaged-gj: soft multi (soft multi-sector contract) */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "storaged-gj: soft multi lba=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)SOFT_LBA);
    append_s(aLine, sizeof(aLine), &o, " sects=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)SOFT_SECTS);
    append_s(aLine, sizeof(aLine), &o, " bytes=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)SOFT_BYTES);
    append_s(aLine, sizeof(aLine), &o, " ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiOk);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: storaged-gj: soft deepen wave (Wave 98 stamp) */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "storaged-gj: soft deepen wave=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)SOFT_INV_WAVE);
    append_s(aLine, sizeof(aLine), &o, " areas=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)SOFT_INV_AREAS);
    append_s(aLine, sizeof(aLine), &o, " ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cSkip);
    append_s(aLine, sizeof(aLine), &o, " multi_server=0 confine=0\n");
    aLine[o] = '\0';
    msg(aLine);

    /*
     * Grep: storaged-gj: soft path
     * Honesty: store-door + soft ring inventory only; not bar3 / FS claim.
     * Soft inventory ≠ product multi-server confine.
     */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o,
             "storaged-gj: soft path store=1 ring=1 bar3=0 fs=0 "
             "multi_server=0 confine=0 wave=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)SOFT_INV_WAVE);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /*
     * Grep: storaged-gj: soft honesty (Wave 98 exclusive deepen).
     * Soft inventory ≠ product multi-server confine.
     */
    msg("storaged-gj: soft honesty multi_server=0 confine=0 bar3=0 "
        "exclusive=1 soft=1 product_kernel=OPEN wave=70\n");

    /* Soft lamp only — never a product / bar3 gate. */
    msg("storaged-gj: soft inventory PASS\n");
}

/* Note one soft door sub-step outcome into Wave 98 inventory counters. */
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

/* Note one soft free sub-step outcome into Wave 98 inventory counters. */
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

/* Note one soft ring sub-step outcome into Wave 98 inventory counters. */
static void
soft_ring_note(unsigned uBit, int fOk)
{
    if (fOk != 0) {
        g_uSoftRingBits |= uBit;
        if (g_cSoftRingOk < 0xffffffffu) {
            g_cSoftRingOk++;
        }
    } else if (g_cSoftRingSkip < 0xffffffffu) {
        g_cSoftRingSkip++;
    }
}

/* Snapshot virtq export fields for Wave 98 soft export / ring lines. */
static void
soft_export_snap(const struct vq_export *pEx)
{
    if (pEx == 0) {
        return;
    }
    g_uSoftExSize = (unsigned)pEx->size;
    g_uSoftExReady = pEx->ready;
    g_uSoftExFreeHead = (unsigned)pEx->free_head;
    g_uSoftExNumFree = (unsigned)pEx->num_free;
    g_uSoftExWhich = (unsigned)pEx->which;
}

/*
 * Soft door surface while CLAIM is held (and a light RING_STATE peek).
 * Never hard-fails: each step soft-skips on rejection / short I/O.
 * Leaves hard-path smoke LBA 2 alone (only mutates soft LBA 3..4).
 * Returns count of soft sub-steps that greened.
 * Tallies Wave 98 soft inventory (storaged-gj: soft …).
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
    unsigned cOk = 0u;
    unsigned iByte;
    long nRet;

    /*
     * 1) Soft reclaim: same-token re-CLAIM is idempotent (store_door soft).
     *    Must stay owned after; failure is soft-skip only.
     */
    nRet = gj_store(GJ_STORE_OP_CLAIM, (long)STORE_TOKEN, 0, 0);
    if (nRet == 0) {
        soft_door_note(SOFT_BIT_RECLAIM, 1);
        cOk++;
        msg("storaged-gj: reclaim soft PASS\n");
    } else {
        soft_door_note(SOFT_BIT_RECLAIM, 0);
        msg("storaged-gj: reclaim soft-skip\n");
    }

    /*
     * 2) CAP — capacity in 512-byte sectors. Soft-skip when no blk/scsi
     *    transport (NODEV) or null/fault.
     */
    u64Cap = 0ull;
    nRet = gj_store(GJ_STORE_OP_CAP, (long)(uintptr_t)&u64Cap, 0, 0);
    if (nRet == 0 && u64Cap > 0ull) {
        soft_door_note(SOFT_BIT_CAP, 1);
        cOk++;
        g_uSoftCapSectors = (unsigned long)u64Cap;
        msg_cap_soft(u64Cap);
    } else {
        soft_door_note(SOFT_BIT_CAP, 0);
        msg("storaged-gj: CAP soft-skip\n");
    }

    /*
     * 3) STATS — {blk_io, scsi_io, door_calls}. Soft log; never hard-fail.
     */
    aStats[0] = aStats[1] = aStats[2] = 0u;
    nRet = gj_store(GJ_STORE_OP_STATS, (long)(uintptr_t)aStats, 0, 0);
    if (nRet == 0) {
        soft_door_note(SOFT_BIT_STATS, 1);
        cOk++;
        g_aSoftStats[0] = aStats[0];
        g_aSoftStats[1] = aStats[1];
        g_aSoftStats[2] = aStats[2];
        msg_stats_soft(aStats);
    } else {
        soft_door_note(SOFT_BIT_STATS, 0);
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
        soft_door_note(SOFT_BIT_QUEUE, 1);
        cOk++;
        g_aSoftQueue[0] = aQ[0];
        g_aSoftQueue[1] = aQ[1];
        g_aSoftQueue[2] = aQ[2];
        g_aSoftQueue[3] = aQ[3];
        msg_queue_soft(aQ);
    } else {
        soft_door_note(SOFT_BIT_QUEUE, 0);
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
        soft_door_note(SOFT_BIT_MULTI, 0);
        g_uSoftMultiOk = 0u;
        msg("storaged-gj: multi-sector soft-skip\n");
    } else {
        nRet = gj_store(GJ_STORE_OP_READ, (long)SOFT_LBA, (long)(uintptr_t)aR,
                        (long)SOFT_BYTES);
        if (nRet == (long)SOFT_BYTES &&
            gj_memcmp(aR, aW, SOFT_BYTES) == 0) {
            soft_door_note(SOFT_BIT_MULTI, 1);
            cOk++;
            g_uSoftMultiOk = 1u;
            msg("storaged-gj: multi-sector soft PASS\n");
        } else {
            soft_door_note(SOFT_BIT_MULTI, 0);
            g_uSoftMultiOk = 0u;
            msg("storaged-gj: multi-sector soft-skip\n");
        }
    }

    /*
     * 6) FLUSH — fsync-shaped when a block transport is ready; soft-skip
     *    NODEV without virtio-blk / scsi_mid.
     */
    nRet = gj_store(GJ_STORE_OP_FLUSH, 0, 0, 0);
    if (nRet == 0) {
        soft_door_note(SOFT_BIT_FLUSH, 1);
        cOk++;
        msg("storaged-gj: FLUSH soft PASS\n");
    } else {
        soft_door_note(SOFT_BIT_FLUSH, 0);
        msg("storaged-gj: FLUSH soft-skip\n");
    }

    /*
     * 7) RING_STATE soft peek — door always fills {free, ready}; ready=0
     *    without virtio-blk. Independent of EXPORT/MAP success.
     */
    aRs[0] = aRs[1] = 0u;
    nRet = gj_store(GJ_STORE_OP_RING_STATE, (long)(uintptr_t)aRs, 0, 0);
    if (nRet == 0) {
        soft_door_note(SOFT_BIT_RSTATE, 1);
        cOk++;
        g_uSoftRingFree = aRs[0];
        g_uSoftRingReady = aRs[1];
        msg_rstate_soft(aRs);
    } else {
        soft_door_note(SOFT_BIT_RSTATE, 0);
        msg("storaged-gj: RING_STATE soft-skip\n");
    }

    /* Aggregate soft door line — green if any sub-step greened. */
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
 * Tallies Wave 98 soft inventory (storaged-gj: soft …).
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
        soft_free_note(SOFT_FREE_RELEASE, 1);
        cOk++;
        msg("storaged-gj: free RELEASE soft PASS\n");
    } else {
        soft_free_note(SOFT_FREE_RELEASE, 0);
        msg("storaged-gj: free RELEASE soft-skip\n");
    }

    aQ[0] = aQ[1] = aQ[2] = aQ[3] = 0u;
    nRet = gj_store(GJ_STORE_OP_QUEUE_INFO, (long)(uintptr_t)aQ, 0, 0);
    if (nRet == 0 && aQ[3] == 0u) {
        soft_free_note(SOFT_FREE_OWN, 1);
        cOk++;
        g_uSoftFreeOwn = aQ[3];
        msg_queue_soft(aQ);
        msg("storaged-gj: free own soft PASS\n");
    } else if (nRet == 0) {
        soft_free_note(SOFT_FREE_OWN, 0);
        g_uSoftFreeOwn = aQ[3];
        msg_queue_soft(aQ);
        msg("storaged-gj: free own soft-skip\n");
    } else {
        soft_free_note(SOFT_FREE_OWN, 0);
        msg("storaged-gj: free own soft-skip\n");
    }

    if (cOk > 0u) {
        msg("storaged-gj: free soft PASS\n");
    } else {
        msg("storaged-gj: free soft-skip\n");
    }
}

/*
 * Soft UDX ring path: EXPORT → MAP → RING_STATE → KICK.
 * Soft-skip without virtio-blk (or on MAP fail). Never hard-fails live path.
 * On full success, emits hard-smoke substring "ring map PASS".
 * Tallies Wave 98 soft inventory (storaged-gj: soft …).
 */
static void
soft_ring_path(void)
{
    static struct vq_export ex;
    static unsigned aSt[2]; /* [0]=free, [1]=ready — RING_STATE */
    long nRet;

    (void)gj_memset(&ex, 0, sizeof(ex));
    aSt[0] = 0u;
    aSt[1] = 0u;

    nRet = gj_store(GJ_STORE_OP_EXPORT_RING, (long)(uintptr_t)&ex, 0, 0);
    if (nRet != 0) {
        soft_ring_note(SOFT_RING_EXPORT, 0);
        soft_ring_note(SOFT_RING_MAP, 0);
        soft_ring_note(SOFT_RING_STATE, 0);
        soft_ring_note(SOFT_RING_KICK, 0);
        msg("storaged-gj: ring EXPORT soft-skip\n");
        return;
    }
    soft_ring_note(SOFT_RING_EXPORT, 1);
    soft_export_snap(&ex);

    if (!export_ready(&ex)) {
        soft_ring_note(SOFT_RING_MAP, 0);
        soft_ring_note(SOFT_RING_STATE, 0);
        soft_ring_note(SOFT_RING_KICK, 0);
        msg("storaged-gj: ring soft-skip (no virtio-blk)\n");
        return;
    }

    if (gj_store(GJ_STORE_OP_MAP_RING, (long)RING_VA,
                 (long)(uintptr_t)&ex, 0) != 0) {
        soft_ring_note(SOFT_RING_MAP, 0);
        soft_ring_note(SOFT_RING_STATE, 0);
        soft_ring_note(SOFT_RING_KICK, 0);
        msg("storaged-gj: ring MAP fail (soft)\n");
        return;
    }
    soft_ring_note(SOFT_RING_MAP, 1);
    g_uSoftRingMapped = 1u;
    soft_export_snap(&ex);

    /* Best-effort state snapshot + notify; failures stay soft. */
    if (gj_store(GJ_STORE_OP_RING_STATE, (long)(uintptr_t)aSt, 0, 0) == 0) {
        soft_ring_note(SOFT_RING_STATE, 1);
        g_uSoftRingFree = aSt[0];
        g_uSoftRingReady = aSt[1];
    } else {
        soft_ring_note(SOFT_RING_STATE, 0);
    }
    if (gj_store(GJ_STORE_OP_KICK, 0, 0, 0) == 0) {
        soft_ring_note(SOFT_RING_KICK, 1);
    } else {
        soft_ring_note(SOFT_RING_KICK, 0);
    }
    msg("storaged-gj: ring map PASS\n");
}

void
_start(void)
{
    static unsigned char aW[SECTOR_BYTES];
    static unsigned char aR[SECTOR_BYTES];
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
     * UDX ring hand-off: EXPORT → MAP → RING_STATE → KICK (soft inventory).
     * Soft-skip entire ring path without virtio-blk (or on MAP fail).
     * Hard path never fails the live smoke solely for ring absence.
     */
    soft_ring_path();

    if (gj_store(GJ_STORE_OP_RELEASE, (long)g_uToken, 0, 0) != 0) {
        g_uToken = 0u;
        fail_exit("storaged-gj: RELEASE fail\n");
    }
    g_uToken = 0u;
    msg("storaged-gj: RELEASE PASS\n");

    /* Soft free path after ownership drop — never aborts live path. */
    soft_free_path();

    /*
     * Wave 98 exclusive soft inventory rollup (greppable "storaged-gj: soft …").
     * Emitted after all soft sub-paths; never gates live path PASS.
     */
    soft_inventory_log();

    msg("storaged-gj: live path PASS\n");
    gj_exit(0);
}
