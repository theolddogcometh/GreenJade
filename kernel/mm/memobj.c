/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Memory objects own frames; regions are views (G-MO-*).
 * Maps are USER and confined to the product user VA window (G-MAP-2).
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

void
memobj_init(void)
{
    memset(g_aMemobjPool, 0, sizeof(g_aMemobjPool));
    memset(g_aMemobjUsed, 0, sizeof(g_aMemobjUsed));
    kprintf("memobj: init pool=%u\n", (unsigned)GJ_MEMOBJ_POOL);
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

void
memobj_destroy(struct gj_memobj *pObj)
{
    u32 iPage;

    if (pObj == NULL) {
        return;
    }
    for (iPage = 0; iPage < pObj->cPages; iPage++) {
        if (pObj->aPa[iPage] != 0) {
            pmm_free(pObj->aPa[iPage]);
            pObj->aPa[iPage] = 0;
        }
    }
    pObj->cPages = 0;
    pObj->cMapped = 0;
    pool_free(pObj);
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

gj_vaddr_t
memobj_map_anon(struct gj_process *pProc, u64 u64Hint, size_t cbLen,
                u32 u32Prot, int fFixed)
{
    size_t cbAligned;
    u32 cPages;
    u32 iPage;
    u32 iReg;
    struct gj_memobj *pObj;
    gj_vaddr_t vaBase;
    gj_status_t st;
    u64 u64SavedCr3;
    u32 u32MapProt;

    if (pProc == NULL || cbLen == 0) {
        return 0;
    }
    st = process_as_ensure(pProc);
    if (st != GJ_OK) {
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
    iReg = region_alloc_idx(pProc);
    if (iReg >= GJ_PROC_REGION_MAX) {
        memobj_destroy(pObj);
        return 0;
    }

    vaBase = memobj_pick_va(pProc, u64Hint, cbAligned, fFixed);
    if (!memobj_user_va_ok(vaBase, cbAligned)) {
        region_release_idx(pProc, iReg);
        memobj_destroy(pObj);
        return 0;
    }

    /* Product ring-3 maps always set U (G-MAP-2). */
    u32MapProt = u32Prot | GJ_VMM_PROT_USER;

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
            memobj_destroy(pObj);
            memobj_restore_cr3(pProc, u64SavedCr3);
            return 0;
        }
    }

    pProc->aRegions[iReg].vaBase = vaBase;
    pProc->aRegions[iReg].cbLen = cbAligned;
    pProc->aRegions[iReg].u32Prot = u32MapProt;
    pProc->aRegions[iReg].u64ObjOff = 0;
    pProc->aRegions[iReg].pObj = pObj;
    pObj->cMapped = 1;

    memobj_restore_cr3(pProc, u64SavedCr3);
    return vaBase;
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
                if (pObj->cMapped == 0) {
                    memobj_destroy(pObj);
                }
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

/* ---- Named shareable objects (Proton A0) -------------------------------- */

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

struct gj_memobj *
memobj_create_named(const char *szName, u32 cPages)
{
    struct gj_memobj *pObj;
    u32 iSlot;

    if (szName == NULL || szName[0] == '\0') {
        return NULL;
    }
    if (memobj_lookup_named(szName) != NULL) {
        return NULL; /* EEXIST */
    }
    pObj = memobj_create_anon(cPages);
    if (pObj == NULL) {
        return NULL;
    }
    for (iSlot = 0; iSlot < GJ_NAMED_MAX; iSlot++) {
        if (!g_aNamed[iSlot].u8Used) {
            g_aNamed[iSlot].u8Used = 1;
            name_copy(g_aNamed[iSlot].szName, szName);
            g_aNamed[iSlot].pObj = pObj;
            kprintf("memobj: named \"%s\" pages=%u\n", g_aNamed[iSlot].szName,
                    cPages);
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

    if (szName == NULL) {
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

gj_vaddr_t
memobj_map_named(struct gj_process *pProc, const char *szName, u64 u64Hint,
                 u32 u32Prot)
{
    struct gj_memobj *pObj;
    gj_vaddr_t vaBase;
    u32 iPage;
    u32 iReg;
    u64 u64Saved;
    size_t cbLen;
    u32 u32MapProt;

    pObj = memobj_lookup_named(szName);
    if (pObj == NULL || pProc == NULL || pObj->cPages == 0) {
        return 0;
    }
    if (process_as_ensure(pProc) != GJ_OK) {
        return 0;
    }
    iReg = region_alloc_idx(pProc);
    if (iReg >= GJ_PROC_REGION_MAX) {
        return 0;
    }

    cbLen = (size_t)pObj->cPages * (size_t)GJ_PAGE_SIZE;
    if (u64Hint != 0) {
        if ((u64Hint & (u64)(GJ_PAGE_SIZE - 1)) != 0) {
            region_release_idx(pProc, iReg);
            return 0;
        }
        vaBase = (gj_vaddr_t)u64Hint;
    } else {
        if (pProc->u64AnonNext == 0) {
            pProc->u64AnonNext = 0x0000000040000000ull;
        }
        vaBase = (gj_vaddr_t)pProc->u64AnonNext;
        pProc->u64AnonNext = (u64)vaBase + (u64)cbLen;
    }
    if (!memobj_user_va_ok(vaBase, cbLen)) {
        region_release_idx(pProc, iReg);
        return 0;
    }

    /* Match map_anon: always USER for product ring-3 (G-MAP-2). */
    u32MapProt = u32Prot | GJ_VMM_PROT_USER;

    u64Saved = cpu_read_cr3();
    process_as_activate(pProc);
    for (iPage = 0; iPage < pObj->cPages; iPage++) {
        if (vmm_map_page(vaBase + (gj_vaddr_t)iPage * GJ_PAGE_SIZE,
                         pObj->aPa[iPage], u32MapProt) != GJ_OK) {
            while (iPage > 0) {
                iPage--;
                (void)vmm_unmap_page(vaBase +
                                     (gj_vaddr_t)iPage * GJ_PAGE_SIZE);
            }
            region_release_idx(pProc, iReg);
            memobj_restore_cr3(pProc, u64Saved);
            return 0;
        }
    }
    pProc->aRegions[iReg].u8Used = 1;
    pProc->aRegions[iReg].vaBase = vaBase;
    pProc->aRegions[iReg].cbLen = cbLen;
    pProc->aRegions[iReg].u32Prot = u32MapProt;
    pProc->aRegions[iReg].u64ObjOff = 0;
    pProc->aRegions[iReg].pObj = pObj;
    pObj->cMapped++;
    memobj_restore_cr3(pProc, u64Saved);
    kprintf("memobj: map named \"%s\" va=0x%lx pages=%u\n", szName,
            (unsigned long)vaBase, pObj->cPages);
    return vaBase;
}
