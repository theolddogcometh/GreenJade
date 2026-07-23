/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Memory objects own frames; regions are views (G-MO-*).
 * Maps are USER and confined to the product user VA window (G-MAP-2).
 * Named shareable maps: one object, many process views (G-MO-3 / wine-shm).
 *
 * Soft product surface:
 *   region table soft — fixed GJ_PROC_REGION_MAX; full/reuse/overlap markers
 *   USER map flags    — memobj_sanitize_user_prot always forces U
 *   named lifecycle   — publish/unlink independent of last map
 *
 * Grep markers:
 *   memobj: named
 *   memobj: share
 *   memobj: region table soft
 *   memobj: USER map
 *   wine-shm
 */
#include <gj/cap.h>
#include <gj/config.h>
#include <gj/cpu.h>
#include <gj/klog.h>
#include <gj/memobj.h>
#include <gj/pmm.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/user_access.h>
#include <gj/vmm.h>

#define GJ_MEMOBJ_POOL 32

static struct gj_memobj g_aMemobjPool[GJ_MEMOBJ_POOL];
static u8               g_aMemobjUsed[GJ_MEMOBJ_POOL];

/* Soft observability counters (never hard-fail product). */
static u32 g_cSoftRegionFull;
static u32 g_cSoftRegionReuse;
static u32 g_cSoftRegionOverlap;
static u32 g_cSoftShareMaps;
static u32 g_cSoftNamedCreate;
static u32 g_cSoftNamedUnlink;

void
memobj_init(void)
{
    memset(g_aMemobjPool, 0, sizeof(g_aMemobjPool));
    memset(g_aMemobjUsed, 0, sizeof(g_aMemobjUsed));
    g_cSoftRegionFull = 0;
    g_cSoftRegionReuse = 0;
    g_cSoftRegionOverlap = 0;
    g_cSoftShareMaps = 0;
    g_cSoftNamedCreate = 0;
    g_cSoftNamedUnlink = 0;
    kprintf("memobj: init pool=%u named_max soft region_table=%u\n",
            (unsigned)GJ_MEMOBJ_POOL, (unsigned)GJ_PROC_REGION_MAX);
}

static struct gj_memobj *
pool_alloc(void)
{
    u32 iSlot;

    for (iSlot = 0; iSlot < GJ_MEMOBJ_POOL; iSlot++) {
        if (!g_aMemobjUsed[iSlot]) {
            g_aMemobjUsed[iSlot] = 1;
            memset(&g_aMemobjPool[iSlot], 0, sizeof(g_aMemobjPool[iSlot]));
            gj_obj_hdr_init(&g_aMemobjPool[iSlot].hdr);
            return &g_aMemobjPool[iSlot];
        }
    }
    return NULL;
}

static void
pool_free(struct gj_memobj *pObj)
{
    u32 iSlot;

    if (pObj == NULL) {
        return;
    }
    for (iSlot = 0; iSlot < GJ_MEMOBJ_POOL; iSlot++) {
        if (&g_aMemobjPool[iSlot] == pObj) {
            g_aMemobjUsed[iSlot] = 0;
            return;
        }
    }
}

/**
 * Zero a freshly allocated frame.
 * Prefer HHDM (shared kernel half — safe under any CR3). Without HHDM,
 * switch to kernel CR3 so low-PA identity maps are present.
 */
static void
memobj_zero_frame(gj_paddr_t pa)
{
    void *pVa;

    if (hhdm_ready()) {
        pVa = (void *)hhdm_to_virt(pa);
        memset(pVa, 0, GJ_PAGE_SIZE);
        return;
    }
    {
        u64 u64Saved = cpu_read_cr3();

        cpu_load_cr3(vmm_kernel_cr3());
        pVa = (void *)(gj_vaddr_t)pa;
        memset(pVa, 0, GJ_PAGE_SIZE);
        cpu_load_cr3(u64Saved);
    }
}

/** Non-zero if [va, va+cb) lies fully in the product user window. */
static int
memobj_user_va_ok(gj_vaddr_t va, size_t cb)
{
    u64 u64End;

    if (cb == 0) {
        return 0;
    }
    if ((u64)va < GJ_USER_VA_BASE) {
        return 0;
    }
    u64End = (u64)va + (u64)cb;
    if (u64End < (u64)va) {
        return 0;
    }
    if (u64End > GJ_USER_VA_END) {
        return 0;
    }
    return 1;
}

/* ---- USER map flags (G-MAP-2) ------------------------------------------- */

u32
memobj_sanitize_user_prot(u32 u32Prot)
{
    u32 u32Out;

    /* Keep only arch-meaningful rights; force USER for product maps. */
    u32Out = u32Prot & (GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_EXEC);
    if (u32Out == 0) {
        /* Soft default: readable user page (never supervisor-only product). */
        u32Out = GJ_VMM_PROT_READ;
    }
    u32Out |= GJ_VMM_PROT_USER;
    return u32Out;
}

/* ---- Region table soft (fixed GJ_PROC_REGION_MAX) ----------------------- */

u32
memobj_region_slots_free(const struct gj_process *pProc)
{
    u32 iReg;
    u32 cFree = 0;

    if (pProc == NULL) {
        return 0;
    }
    for (iReg = 0; iReg < GJ_PROC_REGION_MAX; iReg++) {
        if (!pProc->aRegions[iReg].u8Used) {
            cFree++;
        }
    }
    return cFree;
}

u32
memobj_region_find(const struct gj_process *pProc, gj_vaddr_t va)
{
    u32 iReg;

    if (pProc == NULL) {
        return GJ_PROC_REGION_MAX;
    }
    for (iReg = 0; iReg < GJ_PROC_REGION_MAX; iReg++) {
        if (pProc->aRegions[iReg].u8Used &&
            pProc->aRegions[iReg].vaBase == va) {
            return iReg;
        }
    }
    return GJ_PROC_REGION_MAX;
}

static u32
region_alloc_idx(struct gj_process *pProc)
{
    u32 iReg;

    for (iReg = 0; iReg < GJ_PROC_REGION_MAX; iReg++) {
        if (!pProc->aRegions[iReg].u8Used) {
            memset(&pProc->aRegions[iReg], 0, sizeof(pProc->aRegions[iReg]));
            pProc->aRegions[iReg].u8Used = 1;
            return iReg;
        }
    }
    /* Greppable: memobj: region table soft FULL */
    g_cSoftRegionFull++;
    kprintf("memobj: region table soft FULL max=%u free=0 (soft #%u)\n",
            (unsigned)GJ_PROC_REGION_MAX, (unsigned)g_cSoftRegionFull);
    return GJ_PROC_REGION_MAX;
}

static void
region_release_idx(struct gj_process *pProc, u32 iReg)
{
    if (pProc == NULL || iReg >= GJ_PROC_REGION_MAX) {
        return;
    }
    memset(&pProc->aRegions[iReg], 0, sizeof(pProc->aRegions[iReg]));
}

/**
 * Soft: exact reuse — same object already mapped at [vaBase, cbLen).
 * Returns region index or GJ_PROC_REGION_MAX.
 */
static u32
region_find_exact(const struct gj_process *pProc, gj_vaddr_t vaBase,
                  size_t cbLen, const struct gj_memobj *pObj)
{
    u32 iReg;

    if (pProc == NULL) {
        return GJ_PROC_REGION_MAX;
    }
    for (iReg = 0; iReg < GJ_PROC_REGION_MAX; iReg++) {
        if (pProc->aRegions[iReg].u8Used &&
            pProc->aRegions[iReg].vaBase == vaBase &&
            pProc->aRegions[iReg].cbLen == cbLen &&
            pProc->aRegions[iReg].pObj == (void *)pObj) {
            return iReg;
        }
    }
    return GJ_PROC_REGION_MAX;
}

/**
 * Soft: non-zero if [vaBase, cbLen) overlaps any used region.
 * G-MO region table soft — refuse clobber without hard panic.
 */
static int
region_overlaps(const struct gj_process *pProc, gj_vaddr_t vaBase, size_t cbLen)
{
    u32 iReg;
    u64 u64A0, u64A1, u64B0, u64B1;

    if (pProc == NULL || cbLen == 0) {
        return 0;
    }
    u64A0 = (u64)vaBase;
    u64A1 = u64A0 + (u64)cbLen;
    for (iReg = 0; iReg < GJ_PROC_REGION_MAX; iReg++) {
        if (!pProc->aRegions[iReg].u8Used) {
            continue;
        }
        u64B0 = (u64)pProc->aRegions[iReg].vaBase;
        u64B1 = u64B0 + (u64)pProc->aRegions[iReg].cbLen;
        if (u64A0 < u64B1 && u64B0 < u64A1) {
            return 1;
        }
    }
    return 0;
}

/**
 * Pick map base: fixed uses hint; otherwise honor in-window page-aligned hints.
 * Low hints (classic PE 0x400000) collide with kernel BSS — ignore them.
 */
static gj_vaddr_t
memobj_pick_va(struct gj_process *pProc, u64 u64Hint, size_t cbAligned,
               int fFixed)
{
    gj_vaddr_t vaBase;

    if (fFixed) {
        return (gj_vaddr_t)u64Hint;
    }
    if (pProc->u64AnonNext == 0) {
        pProc->u64AnonNext = 0x0000000040000000ull;
    }
    vaBase = (gj_vaddr_t)pProc->u64AnonNext;
    if (u64Hint >= GJ_USER_VA_BASE && u64Hint < GJ_USER_VA_END &&
        (u64Hint & (u64)(GJ_PAGE_SIZE - 1)) == 0) {
        vaBase = (gj_vaddr_t)u64Hint;
    }
    pProc->u64AnonNext = (u64)vaBase + (u64)cbAligned;
    return vaBase;
}

/** Restore CR3 after mapping: keep process AS if that was already active. */
static void
memobj_restore_cr3(struct gj_process *pProc, u64 u64SavedCr3)
{
    if ((u64SavedCr3 & ~0xfffull) == (pProc->u64Cr3 & ~0xfffull)) {
        process_as_activate(pProc);
    } else {
        cpu_load_cr3(u64SavedCr3);
    }
}

/* ---- Object create / destroy -------------------------------------------- */

struct gj_memobj *
memobj_create_anon(u32 cPages)
{
    struct gj_memobj *pObj;
    u32 iPage;
    gj_paddr_t pa;

    if (cPages == 0 || cPages > GJ_MEMOBJ_MAX_PAGES) {
        return NULL;
    }
    pObj = pool_alloc();
    if (pObj == NULL) {
        return NULL;
    }
    pObj->u32Kind = (u32)GJ_MEMOBJ_ANON;
    pObj->cPages = cPages;
    pObj->cMapped = 0;
    pObj->u32Flags = GJ_MEMOBJ_F_ZEROED;
    for (iPage = 0; iPage < cPages; iPage++) {
        pa = pmm_alloc();
        if (pa == 0) {
            while (iPage > 0) {
                iPage--;
                pmm_free(pObj->aPa[iPage]);
                pObj->aPa[iPage] = 0;
            }
            pool_free(pObj);
            return NULL;
        }
        pObj->aPa[iPage] = pa;
        memobj_zero_frame(pa);
    }
    return pObj;
}

/**
 * Soft: drop named publish if this object still occupies a name slot.
 * Prevents dangling wine-shm registry entries after destroy.
 */
static void
named_clear_obj(struct gj_memobj *pObj);

void
memobj_destroy(struct gj_memobj *pObj)
{
    u32 iPage;

    if (pObj == NULL) {
        return;
    }
    /* Soft clear named registry first so wine-shm lookup cannot UAF. */
    named_clear_obj(pObj);
    for (iPage = 0; iPage < pObj->cPages; iPage++) {
        if (pObj->aPa[iPage] != 0) {
            pmm_free(pObj->aPa[iPage]);
            pObj->aPa[iPage] = 0;
        }
    }
    pObj->cPages = 0;
    pObj->cMapped = 0;
    pObj->u32Flags = 0;
    pObj->u32Kind = 0;
    pool_free(pObj);
}

/**
 * Reclaim object only when no maps remain and not published by name.
 * Named wine-shm objects survive last unmap until unlink/destroy.
 */
static void
memobj_maybe_reclaim(struct gj_memobj *pObj)
{
    if (pObj == NULL) {
        return;
    }
    if (pObj->cMapped > 0) {
        return;
    }
    if ((pObj->u32Flags & GJ_MEMOBJ_F_NAMED) != 0) {
        return; /* still published — G-MO-3 / wine-shm sticky */
    }
    memobj_destroy(pObj);
}

gj_status_t
process_as_ensure(struct gj_process *pProc)
{
    u64 u64Cr3;

    if (pProc == NULL) {
        return GJ_ERR_INVAL;
    }
    if (pProc->u64Cr3 != 0) {
        return GJ_OK;
    }
    u64Cr3 = vmm_as_create();
    if (u64Cr3 == 0) {
        return GJ_ERR_NOMEM;
    }
    pProc->u64Cr3 = u64Cr3;
    if (pProc->u64AnonNext == 0) {
        pProc->u64AnonNext = 0x0000000040000000ull;
    }
    kprintf("process: as_create cr3=0x%lx\n", (unsigned long)u64Cr3);
    return GJ_OK;
}

void
process_as_activate(struct gj_process *pProc)
{
    if (pProc != NULL && pProc->u64Cr3 != 0) {
        cpu_load_cr3(pProc->u64Cr3);
        vmm_set_anon_cursor(&pProc->u64AnonNext);
    } else {
        cpu_load_cr3(vmm_kernel_cr3());
        vmm_set_anon_cursor(NULL);
    }
}

/**
 * Core map installer: object frames → process AS + region row.
 * fShare marks G-MO-3 shareable path (soft counter). Returns VA or 0.
 */
static gj_vaddr_t
memobj_map_obj_core(struct gj_process *pProc, struct gj_memobj *pObj,
                    u64 u64Hint, u32 u32Prot, int fFixed, int fShare)
{
    size_t cbLen;
    u32 cPages;
    u32 iPage;
    u32 iReg;
    u32 iExact;
    gj_vaddr_t vaBase;
    gj_status_t st;
    u64 u64SavedCr3;
    u32 u32MapProt;
    u64 u64SavedAnon;

    if (pProc == NULL || pObj == NULL || pObj->cPages == 0) {
        return 0;
    }
    st = process_as_ensure(pProc);
    if (st != GJ_OK) {
        return 0;
    }

    cPages = pObj->cPages;
    cbLen = (size_t)cPages * (size_t)GJ_PAGE_SIZE;
    if (fFixed && (u64Hint & (u64)(GJ_PAGE_SIZE - 1)) != 0) {
        return 0;
    }
    if (!fFixed && u64Hint != 0 &&
        (u64Hint & (u64)(GJ_PAGE_SIZE - 1)) != 0) {
        return 0;
    }

    /* Product ring-3 maps always set U (G-MAP-2 / USER map). */
    u32MapProt = memobj_sanitize_user_prot(u32Prot);
    /* Greppable: memobj: USER map */
    (void)u32MapProt;

    /*
     * Soft VA pick without yet committing anon cursor (rollback on fail).
     * Fixed / explicit in-window hint used as-is; else anon cursor.
     */
    u64SavedAnon = pProc->u64AnonNext;
    if (fFixed || (u64Hint != 0 && u64Hint >= GJ_USER_VA_BASE &&
                   u64Hint < GJ_USER_VA_END &&
                   (u64Hint & (u64)(GJ_PAGE_SIZE - 1)) == 0)) {
        vaBase = (gj_vaddr_t)u64Hint;
        if (!fFixed) {
            /* Advance cursor past this map when using a soft hint. */
            pProc->u64AnonNext = (u64)vaBase + (u64)cbLen;
        }
    } else {
        vaBase = memobj_pick_va(pProc, 0, cbLen, 0);
    }

    if (!memobj_user_va_ok(vaBase, cbLen)) {
        pProc->u64AnonNext = u64SavedAnon;
        return 0;
    }

    /* Soft idempotent reuse: same view already installed. */
    iExact = region_find_exact(pProc, vaBase, cbLen, pObj);
    if (iExact < GJ_PROC_REGION_MAX) {
        g_cSoftRegionReuse++;
        /* Greppable: memobj: region table soft reuse */
        kprintf("memobj: region table soft reuse va=0x%lx idx=%u (soft #%u)\n",
                (unsigned long)vaBase, (unsigned)iExact,
                (unsigned)g_cSoftRegionReuse);
        pProc->u64AnonNext = u64SavedAnon; /* no new reservation */
        pProc->aRegions[iExact].u32Prot = u32MapProt;
        return vaBase;
    }

    if (region_overlaps(pProc, vaBase, cbLen)) {
        g_cSoftRegionOverlap++;
        /* Greppable: memobj: region table soft overlap */
        kprintf("memobj: region table soft overlap va=0x%lx cb=%lu (soft #%u)\n",
                (unsigned long)vaBase, (unsigned long)cbLen,
                (unsigned)g_cSoftRegionOverlap);
        pProc->u64AnonNext = u64SavedAnon;
        return 0;
    }

    iReg = region_alloc_idx(pProc);
    if (iReg >= GJ_PROC_REGION_MAX) {
        pProc->u64AnonNext = u64SavedAnon;
        return 0;
    }

    u64SavedCr3 = cpu_read_cr3();
    process_as_activate(pProc);

    for (iPage = 0; iPage < cPages; iPage++) {
        st = vmm_map_page(vaBase + (gj_vaddr_t)iPage * GJ_PAGE_SIZE,
                          pObj->aPa[iPage], u32MapProt);
        if (st != GJ_OK) {
            while (iPage > 0) {
                iPage--;
                (void)vmm_unmap_page(vaBase +
                                     (gj_vaddr_t)iPage * GJ_PAGE_SIZE);
            }
            region_release_idx(pProc, iReg);
            pProc->u64AnonNext = u64SavedAnon;
            memobj_restore_cr3(pProc, u64SavedCr3);
            return 0;
        }
    }

    pProc->aRegions[iReg].vaBase = vaBase;
    pProc->aRegions[iReg].cbLen = cbLen;
    pProc->aRegions[iReg].u32Prot = u32MapProt;
    pProc->aRegions[iReg].u64ObjOff = 0;
    pProc->aRegions[iReg].pObj = pObj;
    pObj->cMapped++;
    if (fShare || (pObj->u32Flags & GJ_MEMOBJ_F_SHAREABLE) != 0) {
        g_cSoftShareMaps++;
        /* Greppable: memobj: share */
        kprintf("memobj: share map va=0x%lx pages=%u cMapped=%u (soft #%u)\n",
                (unsigned long)vaBase, (unsigned)cPages,
                (unsigned)pObj->cMapped, (unsigned)g_cSoftShareMaps);
    }

    memobj_restore_cr3(pProc, u64SavedCr3);
    return vaBase;
}

gj_vaddr_t
memobj_map_anon(struct gj_process *pProc, u64 u64Hint, size_t cbLen,
                u32 u32Prot, int fFixed)
{
    size_t cbAligned;
    u32 cPages;
    struct gj_memobj *pObj;
    gj_vaddr_t vaBase;

    if (pProc == NULL || cbLen == 0) {
        return 0;
    }
    cbAligned = (cbLen + GJ_PAGE_SIZE - 1) & ~(size_t)(GJ_PAGE_SIZE - 1);
    cPages = (u32)(cbAligned / GJ_PAGE_SIZE);
    if (cPages == 0 || cPages > GJ_MEMOBJ_MAX_PAGES) {
        return 0;
    }
    if (fFixed && (u64Hint & (u64)(GJ_PAGE_SIZE - 1)) != 0) {
        return 0;
    }

    pObj = memobj_create_anon(cPages);
    if (pObj == NULL) {
        return 0;
    }
    /* Private anon: not shareable until elevated (named path does that). */
    vaBase = memobj_map_obj_core(pProc, pObj, u64Hint, u32Prot, fFixed, 0);
    if (vaBase == 0) {
        memobj_destroy(pObj);
        return 0;
    }
    return vaBase;
}

gj_vaddr_t
memobj_map_share(struct gj_process *pProc, struct gj_memobj *pObj, u64 u64Hint,
                 u32 u32Prot, int fFixed)
{
    if (pObj == NULL) {
        return 0;
    }
    /* Elevate to shareable for multi-map (G-MO-3). */
    pObj->u32Flags |= GJ_MEMOBJ_F_SHAREABLE;
    return memobj_map_obj_core(pProc, pObj, u64Hint, u32Prot, fFixed, 1);
}

gj_status_t
memobj_unmap(struct gj_process *pProc, gj_vaddr_t va, size_t cbLen)
{
    u32 iReg;
    u32 iPage;
    size_t cPages;
    size_t cbAligned;
    struct gj_memobj *pObj;
    u64 u64SavedCr3;
    int fFound = 0;

    if (pProc == NULL || cbLen == 0) {
        return GJ_ERR_INVAL;
    }
    if ((va & (gj_vaddr_t)(GJ_PAGE_SIZE - 1)) != 0) {
        return GJ_ERR_INVAL;
    }
    cbAligned = (cbLen + GJ_PAGE_SIZE - 1) & ~(size_t)(GJ_PAGE_SIZE - 1);
    cPages = cbAligned / GJ_PAGE_SIZE;

    u64SavedCr3 = cpu_read_cr3();
    if (pProc->u64Cr3 != 0) {
        process_as_activate(pProc);
    }

    for (iReg = 0; iReg < GJ_PROC_REGION_MAX; iReg++) {
        if (pProc->aRegions[iReg].u8Used &&
            pProc->aRegions[iReg].vaBase == va &&
            pProc->aRegions[iReg].cbLen == cbAligned) {
            pObj = (struct gj_memobj *)pProc->aRegions[iReg].pObj;
            for (iPage = 0; iPage < (u32)cPages; iPage++) {
                (void)vmm_unmap_page(va + (gj_vaddr_t)iPage * GJ_PAGE_SIZE);
            }
            region_release_idx(pProc, iReg);
            if (pObj != NULL) {
                if (pObj->cMapped > 0) {
                    pObj->cMapped--;
                }
                /* Named wine-shm sticks until unlink; private reclaims. */
                memobj_maybe_reclaim(pObj);
            }
            fFound = 1;
            break;
        }
    }

    /*
     * Fallback: unmap orphan PTEs and free private frames (no region book-
     * keeping). Fail closed on bad VA; still return OK so munmap is idempotent
     * for product Linux ABI paths that unmap twice.
     */
    if (!fFound) {
        for (iPage = 0; iPage < (u32)cPages; iPage++) {
            gj_paddr_t pa = vmm_virt_to_phys(va +
                                             (gj_vaddr_t)iPage * GJ_PAGE_SIZE);

            (void)vmm_unmap_page(va + (gj_vaddr_t)iPage * GJ_PAGE_SIZE);
            if (pa) {
                pmm_free(pa & ~(gj_paddr_t)(GJ_PAGE_SIZE - 1));
            }
        }
    }

    cpu_load_cr3(u64SavedCr3);
    return GJ_OK;
}

/* ---- Named shareable objects (Proton A0 / wine-shm) --------------------- */

#define GJ_NAMED_MAX 16

struct named_slot {
    u8                u8Used;
    char              szName[GJ_MEMOBJ_NAME_MAX];
    struct gj_memobj *pObj;
};

static struct named_slot g_aNamed[GJ_NAMED_MAX];

static int
name_eq(const char *szA, const char *szB)
{
    u32 iCh;

    if (szA == NULL || szB == NULL) {
        return 0;
    }
    for (iCh = 0; iCh < GJ_MEMOBJ_NAME_MAX; iCh++) {
        if (szA[iCh] != szB[iCh]) {
            return 0;
        }
        if (szA[iCh] == '\0') {
            return 1;
        }
    }
    return 1;
}

static void
name_copy(char *szDst, const char *szSrc)
{
    u32 iCh;

    for (iCh = 0; iCh + 1 < GJ_MEMOBJ_NAME_MAX; iCh++) {
        if (szSrc == NULL || szSrc[iCh] == '\0') {
            break;
        }
        szDst[iCh] = szSrc[iCh];
    }
    szDst[iCh] = '\0';
}

/**
 * Soft name validation: non-empty, no '/', printable-ish ASCII.
 * Keeps wine-shm0 / winesrv-shm / mfd:* / init-shm / vfsd-cache valid.
 */
static int
name_ok(const char *szName)
{
    u32 iCh;
    u32 cLen = 0;

    if (szName == NULL || szName[0] == '\0') {
        return 0;
    }
    for (iCh = 0; iCh < GJ_MEMOBJ_NAME_MAX; iCh++) {
        char ch = szName[iCh];

        if (ch == '\0') {
            break;
        }
        if (ch == '/' || (u8)ch < 0x20u || (u8)ch > 0x7eu) {
            return 0;
        }
        cLen++;
    }
    return cLen > 0 && cLen < GJ_MEMOBJ_NAME_MAX;
}

static void
named_clear_obj(struct gj_memobj *pObj)
{
    u32 iSlot;

    if (pObj == NULL) {
        return;
    }
    for (iSlot = 0; iSlot < GJ_NAMED_MAX; iSlot++) {
        if (g_aNamed[iSlot].u8Used && g_aNamed[iSlot].pObj == pObj) {
            g_aNamed[iSlot].u8Used = 0;
            g_aNamed[iSlot].pObj = NULL;
            g_aNamed[iSlot].szName[0] = '\0';
            pObj->u32Flags &= (u32)~GJ_MEMOBJ_F_NAMED;
        }
    }
}

struct gj_memobj *
memobj_create_named(const char *szName, u32 cPages)
{
    struct gj_memobj *pObj;
    u32 iSlot;

    if (!name_ok(szName)) {
        return NULL;
    }
    if (memobj_lookup_named(szName) != NULL) {
        return NULL; /* EEXIST — wine-shm re-create soft-misses at caller */
    }
    pObj = memobj_create_anon(cPages);
    if (pObj == NULL) {
        return NULL;
    }
    pObj->u32Kind = (u32)GJ_MEMOBJ_NAMED;
    pObj->u32Flags |= GJ_MEMOBJ_F_SHAREABLE | GJ_MEMOBJ_F_NAMED;

    for (iSlot = 0; iSlot < GJ_NAMED_MAX; iSlot++) {
        if (!g_aNamed[iSlot].u8Used) {
            g_aNamed[iSlot].u8Used = 1;
            name_copy(g_aNamed[iSlot].szName, szName);
            g_aNamed[iSlot].pObj = pObj;
            g_cSoftNamedCreate++;
            /* Greppable: memobj: named  (+ wine-shm when name matches) */
            kprintf("memobj: named \"%s\" pages=%u shareable=1 (soft #%u)\n",
                    g_aNamed[iSlot].szName, cPages,
                    (unsigned)g_cSoftNamedCreate);
            if (szName[0] == 'w' && szName[1] == 'i' && szName[2] == 'n' &&
                szName[3] == 'e') {
                /* Greppable: wine-shm path */
                kprintf("memobj: wine-shm named path \"%s\" ok\n",
                        g_aNamed[iSlot].szName);
            }
            return pObj;
        }
    }
    memobj_destroy(pObj);
    return NULL;
}

struct gj_memobj *
memobj_lookup_named(const char *szName)
{
    u32 iSlot;

    if (szName == NULL || szName[0] == '\0') {
        return NULL;
    }
    for (iSlot = 0; iSlot < GJ_NAMED_MAX; iSlot++) {
        if (g_aNamed[iSlot].u8Used &&
            name_eq(g_aNamed[iSlot].szName, szName)) {
            return g_aNamed[iSlot].pObj;
        }
    }
    return NULL;
}

gj_status_t
memobj_unlink_named(const char *szName)
{
    u32 iSlot;
    struct gj_memobj *pObj;

    if (szName == NULL || szName[0] == '\0') {
        return GJ_ERR_INVAL;
    }
    for (iSlot = 0; iSlot < GJ_NAMED_MAX; iSlot++) {
        if (g_aNamed[iSlot].u8Used &&
            name_eq(g_aNamed[iSlot].szName, szName)) {
            pObj = g_aNamed[iSlot].pObj;
            g_aNamed[iSlot].u8Used = 0;
            g_aNamed[iSlot].pObj = NULL;
            g_aNamed[iSlot].szName[0] = '\0';
            g_cSoftNamedUnlink++;
            /* Greppable: memobj: named unlink */
            kprintf("memobj: named unlink \"%s\" (soft #%u)\n", szName,
                    (unsigned)g_cSoftNamedUnlink);
            if (pObj != NULL) {
                pObj->u32Flags &= (u32)~GJ_MEMOBJ_F_NAMED;
                /* Keep SHAREABLE while maps remain; reclaim if idle. */
                memobj_maybe_reclaim(pObj);
            }
            return GJ_OK;
        }
    }
    return GJ_ERR_NOENT;
}

gj_vaddr_t
memobj_map_named(struct gj_process *pProc, const char *szName, u64 u64Hint,
                 u32 u32Prot)
{
    struct gj_memobj *pObj;
    gj_vaddr_t vaBase;
    int fFixed;

    pObj = memobj_lookup_named(szName);
    if (pObj == NULL || pProc == NULL || pObj->cPages == 0) {
        return 0;
    }
    /* Explicit hint ⇒ fixed VA (winesrv 0x50000000 / 0x60000000 path). */
    fFixed = (u64Hint != 0) ? 1 : 0;
    vaBase = memobj_map_obj_core(pProc, pObj, u64Hint, u32Prot, fFixed, 1);
    if (vaBase != 0) {
        /* Greppable: memobj: named map */
        kprintf("memobj: named map \"%s\" va=0x%lx pages=%u cMapped=%u\n",
                szName, (unsigned long)vaBase, pObj->cPages, pObj->cMapped);
        if (szName[0] == 'w' && szName[1] == 'i' && szName[2] == 'n' &&
            szName[3] == 'e') {
            /* Greppable: wine-shm */
            kprintf("memobj: wine-shm map \"%s\" va=0x%lx\n", szName,
                    (unsigned long)vaBase);
        }
    }
    return vaBase;
}

/* ---- Soft observability ------------------------------------------------- */

gj_paddr_t
memobj_page_pa(const struct gj_memobj *pObj, u32 iPage)
{
    if (pObj == NULL || iPage >= pObj->cPages) {
        return 0;
    }
    return pObj->aPa[iPage];
}

void
memobj_soft_stats(u32 *pNamedUsed, u32 *pPoolUsed, u32 *pMappedTotal)
{
    u32 iSlot;
    u32 cNamed = 0;
    u32 cPool = 0;
    u32 cMapped = 0;

    for (iSlot = 0; iSlot < GJ_NAMED_MAX; iSlot++) {
        if (g_aNamed[iSlot].u8Used) {
            cNamed++;
        }
    }
    for (iSlot = 0; iSlot < GJ_MEMOBJ_POOL; iSlot++) {
        if (g_aMemobjUsed[iSlot]) {
            cPool++;
            cMapped += g_aMemobjPool[iSlot].cMapped;
        }
    }
    if (pNamedUsed != NULL) {
        *pNamedUsed = cNamed;
    }
    if (pPoolUsed != NULL) {
        *pPoolUsed = cPool;
    }
    if (pMappedTotal != NULL) {
        *pMappedTotal = cMapped;
    }
}
