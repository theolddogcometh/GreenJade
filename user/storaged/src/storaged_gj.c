/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding storaged — product block host over the kernel store door.
 *
 * Live path (order fixed for smoke greps):
 *   CLAIM → WRITE/READ sector smoke → UDX ring EXPORT/MAP/STATE/KICK → RELEASE
 *
 * Store-door ops used here (must match kernel/include/gj/store_door.h and
 * the GJ_STORE_OP_* subset in user/libgj/include/gj/syscalls.h):
 *   READ=3  WRITE=4  CLAIM=5  RELEASE=6
 *   EXPORT_RING=9  KICK=10  RING_STATE=11  MAP_RING=12
 *
 * Ring steps soft-skip when virtio-blk is not ready so host/QEMU without blk
 * still green on non-hard markers. Multiboot smoke with a ready ring expects
 * the exact substring "ring map PASS".
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
 *   storaged-gj: ring EXPORT soft-skip
 *   storaged-gj: ring MAP fail (soft)
 *   storaged-gj: ring soft-skip (no virtio-blk)
 *
 *   make storaged-gj → build/user/storaged.elf
 * Boot embed (parent tree): kernel/proc/storaged_embed.S (.incbin of the ELF).
 *
 * Pure C11 freestanding. Dual-licensed MIT OR Apache-2.0 (no GPL).
 */
#include <gj/string.h>
#include <gj/syscalls.h>

/* Local fallbacks if libgj headers lag the door surface (keep op numbers fixed). */
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
/* Store-door ownership token (storaged product claim; non-zero). */
#define STORE_TOKEN   0x510e0002u

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

/*
 * Hard failure: best-effort RELEASE if we still own the door, then exit 1.
 * Soft ring skips must never call this.
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
    msg("storaged-gj: live path PASS\n");
    gj_exit(0);
}
