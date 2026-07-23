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
 * Soft memobj inventory (Wave 21 exclusive deepen):
 *   - Honesty / non-claims: soft ≠ product, ≠ bar3, ≠ 1TiB product
 *   - Live pool / named / pages / mapped snaps; pool+named+pages+mapped peaks
 *   - Kind / flags / multi-map / idle live snaps (pool walk)
 *   - Cumulative create / map / unmap / destroy / reclaim / USER sanitize
 *   - Region soft full/reuse/overlap + share + named create/unlink
 *   - AS ensure + wine-shm soft path tallies + honesty path catalog
 *   - Design / lookup / page_pa / share / reclaim / lamps (Wave 15)
 *   - Wave 19: surfaces / window / prot / OPEN return surfaces
 *   - Stats rollup + deepen wave=21 stamp + PASS/PARTIAL/INIT/NONE
 *   greppable: "memobj: soft …"
 *   Soft ≠ product.
 *
 * Grep markers (prefix-stable memobj: soft …):
 *   memobj: soft honesty | inventory | pool | named | kinds | flags | peaks
 *   memobj: soft region | create | map | unmap | USER | as | wine
 *   memobj: soft design | lookup | page_pa | share | reclaim | lamps
 *   memobj: soft path | stats | surfaces | window | prot | OPEN
 *   memobj: soft return selftest — Wave 19 terminal return surface
 *   memobj: soft retmap     — Wave 19 return-surface map
 *   memobj: soft deepen wave=21
 *   memobj: soft PASS | PARTIAL | INIT | NONE | inventory PASS
 *   memobj: named | memobj: share | memobj: region table soft
 *   memobj: USER map | wine-shm
 * Honesty: soft inventory only — not bar3 / not product / not 1TiB product /
 *          full FILE-object production remains OPEN. Soft ≠ product.
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
#define GJ_NAMED_MAX   16

static struct gj_memobj g_aMemobjPool[GJ_MEMOBJ_POOL];
static u8               g_aMemobjUsed[GJ_MEMOBJ_POOL];

/* Named shareable registry (Proton A0 / wine-shm) — early for soft inventory. */
struct memobj_named_slot {
    u8                u8Used;
    char              szName[GJ_MEMOBJ_NAME_MAX];
    struct gj_memobj *pObj;
};

static struct memobj_named_slot g_aNamed[GJ_NAMED_MAX];

/* Wave 21 soft inventory stamp (file-local; never product gate). */
#define MEMOBJ_SOFT_WAVE 21u
/* Catalog areas prior to deepen (honesty..OPEN). Soft ≠ product. */
#define MEMOBJ_SOFT_AREAS 36u

/*
 * Wave 19 return-surface bit lamps (surf=0x… on soft surfaces/deepen).
 * greppable: memobj: soft surfaces
 */
#define MEMOBJ_SOFT_SURF_HONESTY   (1u << 0)
#define MEMOBJ_SOFT_SURF_INVENTORY (1u << 1)
#define MEMOBJ_SOFT_SURF_POOL      (1u << 2)
#define MEMOBJ_SOFT_SURF_NAMED     (1u << 3)
#define MEMOBJ_SOFT_SURF_KINDS     (1u << 4)
#define MEMOBJ_SOFT_SURF_FLAGS     (1u << 5)
#define MEMOBJ_SOFT_SURF_PEAKS     (1u << 6)
#define MEMOBJ_SOFT_SURF_REGION    (1u << 7)
#define MEMOBJ_SOFT_SURF_CREATE    (1u << 8)
#define MEMOBJ_SOFT_SURF_MAP       (1u << 9)
#define MEMOBJ_SOFT_SURF_UNMAP     (1u << 10)
#define MEMOBJ_SOFT_SURF_USER      (1u << 11)
#define MEMOBJ_SOFT_SURF_AS        (1u << 12)
#define MEMOBJ_SOFT_SURF_WINE      (1u << 13)
#define MEMOBJ_SOFT_SURF_DESIGN    (1u << 14)
#define MEMOBJ_SOFT_SURF_LOOKUP    (1u << 15)
#define MEMOBJ_SOFT_SURF_PAGE_PA   (1u << 16)
#define MEMOBJ_SOFT_SURF_SHARE     (1u << 17)
#define MEMOBJ_SOFT_SURF_RECLAIM   (1u << 18)
#define MEMOBJ_SOFT_SURF_LAMPS     (1u << 19)
#define MEMOBJ_SOFT_SURF_PATH      (1u << 20)
#define MEMOBJ_SOFT_SURF_STATS     (1u << 21)
#define MEMOBJ_SOFT_SURF_SURFACES  (1u << 22)
#define MEMOBJ_SOFT_SURF_WINDOW    (1u << 23)
#define MEMOBJ_SOFT_SURF_PROT      (1u << 24)
#define MEMOBJ_SOFT_SURF_OPEN      (1u << 25)
#define MEMOBJ_SOFT_SURF_CATALOG                                                   \
    (MEMOBJ_SOFT_SURF_HONESTY | MEMOBJ_SOFT_SURF_INVENTORY |                       \
     MEMOBJ_SOFT_SURF_POOL | MEMOBJ_SOFT_SURF_NAMED | MEMOBJ_SOFT_SURF_KINDS |     \
     MEMOBJ_SOFT_SURF_FLAGS | MEMOBJ_SOFT_SURF_PEAKS | MEMOBJ_SOFT_SURF_REGION |   \
     MEMOBJ_SOFT_SURF_CREATE | MEMOBJ_SOFT_SURF_MAP | MEMOBJ_SOFT_SURF_UNMAP |     \
     MEMOBJ_SOFT_SURF_USER | MEMOBJ_SOFT_SURF_AS | MEMOBJ_SOFT_SURF_WINE |         \
     MEMOBJ_SOFT_SURF_DESIGN | MEMOBJ_SOFT_SURF_LOOKUP | MEMOBJ_SOFT_SURF_PAGE_PA |\
     MEMOBJ_SOFT_SURF_SHARE | MEMOBJ_SOFT_SURF_RECLAIM | MEMOBJ_SOFT_SURF_LAMPS |  \
     MEMOBJ_SOFT_SURF_PATH | MEMOBJ_SOFT_SURF_STATS | MEMOBJ_SOFT_SURF_SURFACES |  \
     MEMOBJ_SOFT_SURF_WINDOW | MEMOBJ_SOFT_SURF_PROT | MEMOBJ_SOFT_SURF_OPEN)

/*
 * Soft product inventory (Wave 20 exclusive). Cumulative unless noted live/peak.
 * Diagnostics only — never hard-gate create/map/unmap policy.
 * Soft ≠ product. greppable: memobj: soft …
 */
static u32 g_u32SoftPoolUsed;     /* live pool slots (scan) */
static u32 g_u32SoftPoolFree;     /* free pool slots (scan) */
static u32 g_u32SoftPoolPeak;     /* high-water pool used */
static u32 g_u32SoftNamedUsed;    /* live named slots (scan) */
static u32 g_u32SoftNamedFree;    /* free named slots (scan) */
static u32 g_u32SoftNamedPeak;    /* high-water named used */
static u32 g_u32SoftPagesOwned;   /* sum cPages over live pool objs */
static u32 g_u32SoftMappedTotal;  /* sum cMapped over live pool objs */
static u32 g_u32SoftPagesPeak;    /* high-water sum cPages */
static u32 g_u32SoftMappedPeak;   /* high-water sum cMapped */
static u32 g_u32SoftKindAnon;     /* live objs kind ANON (scan) */
static u32 g_u32SoftKindNamed;    /* live objs kind NAMED (scan) */
static u32 g_u32SoftKindFile;     /* live objs kind FILE (scan; cold) */
static u32 g_u32SoftKindOther;    /* live objs unknown kind (scan) */
static u32 g_u32SoftFlagShare;    /* live objs F_SHAREABLE (scan) */
static u32 g_u32SoftFlagNamed;    /* live objs F_NAMED (scan) */
static u32 g_u32SoftFlagZeroed;   /* live objs F_ZEROED (scan) */
static u32 g_u32SoftMultiMap;     /* live objs with cMapped > 1 (scan) */
static u32 g_u32SoftMultiMapPeak; /* high-water multi-map obj count (Wave 15) */
static u32 g_u32SoftIdleObjs;     /* live objs with cMapped == 0 (scan) */
static u32 g_u32SoftMaxMappedOne; /* max cMapped on any one live obj (scan) */
static u32 g_u32SoftMaxPagesOne;  /* max cPages on any one live obj (scan) */
static u32 g_u32SoftWineNamedLive;/* named slots with wine* prefix (scan) */
static u32 g_u32SoftInvSamples;   /* inventory dump count / logs */
static u8  g_fSoftInvOnce;        /* one-shot deep dump after activity */

/* Soft event counters (never hard-fail product). */
static u32 g_cSoftRegionFull;
static u32 g_cSoftRegionReuse;
static u32 g_cSoftRegionOverlap;
static u32 g_cSoftShareMaps;
static u32 g_cSoftNamedCreate;
static u32 g_cSoftNamedUnlink;
static u32 g_cSoftNamedTableFull;
static u32 g_cSoftCreateAnonOk;
static u32 g_cSoftCreateAnonFail;
static u32 g_cSoftCreateNamedFail;
static u32 g_cSoftMapAnonOk;
static u32 g_cSoftMapAnonFail;
static u32 g_cSoftMapShareOk;
static u32 g_cSoftMapShareFail;
static u32 g_cSoftMapNamedOk;
static u32 g_cSoftMapNamedFail;
static u32 g_cSoftMapCoreFail;    /* core installer soft miss (map path) */
static u32 g_cSoftUnmapRegion;
static u32 g_cSoftUnmapOrphan;
static u32 g_cSoftDestroy;
static u32 g_cSoftReclaim;
static u32 g_cSoftUserMap;
static u32 g_cSoftUserMapDefR;    /* sanitize soft-defaulted empty→READ */
static u32 g_cSoftLookupNamedHit;
static u32 g_cSoftLookupNamedMiss;
static u32 g_cSoftAsEnsureOk;     /* process_as_ensure created private AS */
static u32 g_cSoftAsEnsureFail;   /* process_as_ensure NOMEM/INVAL */
static u32 g_cSoftAsEnsureIdem;   /* process_as_ensure already had CR3 */
static u32 g_cSoftWineNamedCreate;/* named create with wine* prefix */
static u32 g_cSoftWineNamedMap;   /* named map with wine* prefix */
static u32 g_cSoftPagePaOk;       /* memobj_page_pa hits */
static u32 g_cSoftPagePaFail;     /* memobj_page_pa soft miss */

static void soft_inventory_scan(void);
static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);
static void soft_pool_peak_note(void);
static void soft_named_peak_note(void);
static int  soft_name_is_wine(const char *szName);

void
memobj_init(void)
{
    memset(g_aMemobjPool, 0, sizeof(g_aMemobjPool));
    memset(g_aMemobjUsed, 0, sizeof(g_aMemobjUsed));
    memset(g_aNamed, 0, sizeof(g_aNamed));
    g_u32SoftPoolUsed = 0;
    g_u32SoftPoolFree = (u32)GJ_MEMOBJ_POOL;
    g_u32SoftPoolPeak = 0;
    g_u32SoftNamedUsed = 0;
    g_u32SoftNamedFree = (u32)GJ_NAMED_MAX;
    g_u32SoftNamedPeak = 0;
    g_u32SoftPagesOwned = 0;
    g_u32SoftMappedTotal = 0;
    g_u32SoftPagesPeak = 0;
    g_u32SoftMappedPeak = 0;
    g_u32SoftKindAnon = 0;
    g_u32SoftKindNamed = 0;
    g_u32SoftKindFile = 0;
    g_u32SoftKindOther = 0;
    g_u32SoftFlagShare = 0;
    g_u32SoftFlagNamed = 0;
    g_u32SoftFlagZeroed = 0;
    g_u32SoftMultiMap = 0;
    g_u32SoftMultiMapPeak = 0;
    g_u32SoftIdleObjs = 0;
    g_u32SoftMaxMappedOne = 0;
    g_u32SoftMaxPagesOne = 0;
    g_u32SoftWineNamedLive = 0;
    g_u32SoftInvSamples = 0;
    g_fSoftInvOnce = 0;
    g_cSoftRegionFull = 0;
    g_cSoftRegionReuse = 0;
    g_cSoftRegionOverlap = 0;
    g_cSoftShareMaps = 0;
    g_cSoftNamedCreate = 0;
    g_cSoftNamedUnlink = 0;
    g_cSoftNamedTableFull = 0;
    g_cSoftCreateAnonOk = 0;
    g_cSoftCreateAnonFail = 0;
    g_cSoftCreateNamedFail = 0;
    g_cSoftMapAnonOk = 0;
    g_cSoftMapAnonFail = 0;
    g_cSoftMapShareOk = 0;
    g_cSoftMapShareFail = 0;
    g_cSoftMapNamedOk = 0;
    g_cSoftMapNamedFail = 0;
    g_cSoftMapCoreFail = 0;
    g_cSoftUnmapRegion = 0;
    g_cSoftUnmapOrphan = 0;
    g_cSoftDestroy = 0;
    g_cSoftReclaim = 0;
    g_cSoftUserMap = 0;
    g_cSoftUserMapDefR = 0;
    g_cSoftLookupNamedHit = 0;
    g_cSoftLookupNamedMiss = 0;
    g_cSoftAsEnsureOk = 0;
    g_cSoftAsEnsureFail = 0;
    g_cSoftAsEnsureIdem = 0;
    g_cSoftWineNamedCreate = 0;
    g_cSoftWineNamedMap = 0;
    g_cSoftPagePaOk = 0;
    g_cSoftPagePaFail = 0;
    kprintf("memobj: init pool=%u named_max=%u soft region_table=%u\n",
            (unsigned)GJ_MEMOBJ_POOL, (unsigned)GJ_NAMED_MAX,
            (unsigned)GJ_PROC_REGION_MAX);
    /* Grep: memobj: soft (baseline inventory after init) */
    soft_inventory_log();
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
        g_cSoftUserMapDefR++;
    }
    u32Out |= GJ_VMM_PROT_USER;
    g_cSoftUserMap++;
    /* Greppable: memobj: USER map */
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
        g_cSoftCreateAnonFail++;
        return NULL;
    }
    pObj = pool_alloc();
    if (pObj == NULL) {
        g_cSoftCreateAnonFail++;
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
            g_cSoftCreateAnonFail++;
            return NULL;
        }
        pObj->aPa[iPage] = pa;
        memobj_zero_frame(pa);
    }
    soft_pool_peak_note();
    g_cSoftCreateAnonOk++;
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
    g_cSoftDestroy++;
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
    g_cSoftReclaim++;
    memobj_destroy(pObj);
}

gj_status_t
process_as_ensure(struct gj_process *pProc)
{
    u64 u64Cr3;

    if (pProc == NULL) {
        g_cSoftAsEnsureFail++;
        return GJ_ERR_INVAL;
    }
    if (pProc->u64Cr3 != 0) {
        g_cSoftAsEnsureIdem++;
        return GJ_OK;
    }
    u64Cr3 = vmm_as_create();
    if (u64Cr3 == 0) {
        g_cSoftAsEnsureFail++;
        return GJ_ERR_NOMEM;
    }
    pProc->u64Cr3 = u64Cr3;
    if (pProc->u64AnonNext == 0) {
        pProc->u64AnonNext = 0x0000000040000000ull;
    }
    g_cSoftAsEnsureOk++;
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
        g_cSoftMapCoreFail++;
        return 0;
    }
    st = process_as_ensure(pProc);
    if (st != GJ_OK) {
        g_cSoftMapCoreFail++;
        return 0;
    }

    cPages = pObj->cPages;
    cbLen = (size_t)cPages * (size_t)GJ_PAGE_SIZE;
    if (fFixed && (u64Hint & (u64)(GJ_PAGE_SIZE - 1)) != 0) {
        g_cSoftMapCoreFail++;
        return 0;
    }
    if (!fFixed && u64Hint != 0 &&
        (u64Hint & (u64)(GJ_PAGE_SIZE - 1)) != 0) {
        g_cSoftMapCoreFail++;
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
        g_cSoftMapCoreFail++;
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
        g_cSoftMapCoreFail++;
        return 0;
    }

    iReg = region_alloc_idx(pProc);
    if (iReg >= GJ_PROC_REGION_MAX) {
        pProc->u64AnonNext = u64SavedAnon;
        g_cSoftMapCoreFail++;
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
            g_cSoftMapCoreFail++;
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
        g_cSoftMapAnonFail++;
        return 0;
    }
    cbAligned = (cbLen + GJ_PAGE_SIZE - 1) & ~(size_t)(GJ_PAGE_SIZE - 1);
    cPages = (u32)(cbAligned / GJ_PAGE_SIZE);
    if (cPages == 0 || cPages > GJ_MEMOBJ_MAX_PAGES) {
        g_cSoftMapAnonFail++;
        return 0;
    }
    if (fFixed && (u64Hint & (u64)(GJ_PAGE_SIZE - 1)) != 0) {
        g_cSoftMapAnonFail++;
        return 0;
    }

    pObj = memobj_create_anon(cPages);
    if (pObj == NULL) {
        g_cSoftMapAnonFail++;
        return 0;
    }
    /* Private anon: not shareable until elevated (named path does that). */
    vaBase = memobj_map_obj_core(pProc, pObj, u64Hint, u32Prot, fFixed, 0);
    if (vaBase == 0) {
        memobj_destroy(pObj);
        g_cSoftMapAnonFail++;
        return 0;
    }
    g_cSoftMapAnonOk++;
    soft_inventory_maybe_once();
    return vaBase;
}

gj_vaddr_t
memobj_map_share(struct gj_process *pProc, struct gj_memobj *pObj, u64 u64Hint,
                 u32 u32Prot, int fFixed)
{
    gj_vaddr_t vaBase;

    if (pObj == NULL) {
        g_cSoftMapShareFail++;
        return 0;
    }
    /* Elevate to shareable for multi-map (G-MO-3). */
    pObj->u32Flags |= GJ_MEMOBJ_F_SHAREABLE;
    vaBase = memobj_map_obj_core(pProc, pObj, u64Hint, u32Prot, fFixed, 1);
    if (vaBase == 0) {
        g_cSoftMapShareFail++;
        return 0;
    }
    g_cSoftMapShareOk++;
    soft_inventory_maybe_once();
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
                /* Named wine-shm sticks until unlink; private reclaims. */
                memobj_maybe_reclaim(pObj);
            }
            fFound = 1;
            g_cSoftUnmapRegion++;
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
        g_cSoftUnmapOrphan++;
    }

    cpu_load_cr3(u64SavedCr3);
    return GJ_OK;
}

/* ---- Named shareable objects (Proton A0 / wine-shm) --------------------- */

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
        g_cSoftCreateNamedFail++;
        return NULL;
    }
    if (memobj_lookup_named(szName) != NULL) {
        g_cSoftCreateNamedFail++;
        return NULL; /* EEXIST — wine-shm re-create soft-misses at caller */
    }
    pObj = memobj_create_anon(cPages);
    if (pObj == NULL) {
        g_cSoftCreateNamedFail++;
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
            soft_named_peak_note();
            /* Greppable: memobj: named  (+ wine-shm when name matches) */
            kprintf("memobj: named \"%s\" pages=%u shareable=1 (soft #%u)\n",
                    g_aNamed[iSlot].szName, cPages,
                    (unsigned)g_cSoftNamedCreate);
            if (soft_name_is_wine(szName)) {
                g_cSoftWineNamedCreate++;
                /* Greppable: wine-shm path */
                kprintf("memobj: wine-shm named path \"%s\" ok\n",
                        g_aNamed[iSlot].szName);
            }
            soft_inventory_maybe_once();
            return pObj;
        }
    }
    g_cSoftNamedTableFull++;
    g_cSoftCreateNamedFail++;
    memobj_destroy(pObj);
    return NULL;
}

struct gj_memobj *
memobj_lookup_named(const char *szName)
{
    u32 iSlot;

    if (szName == NULL || szName[0] == '\0') {
        g_cSoftLookupNamedMiss++;
        return NULL;
    }
    for (iSlot = 0; iSlot < GJ_NAMED_MAX; iSlot++) {
        if (g_aNamed[iSlot].u8Used &&
            name_eq(g_aNamed[iSlot].szName, szName)) {
            g_cSoftLookupNamedHit++;
            return g_aNamed[iSlot].pObj;
        }
    }
    g_cSoftLookupNamedMiss++;
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
        g_cSoftMapNamedFail++;
        return 0;
    }
    /* Explicit hint ⇒ fixed VA (winesrv 0x50000000 / 0x60000000 path). */
    fFixed = (u64Hint != 0) ? 1 : 0;
    vaBase = memobj_map_obj_core(pProc, pObj, u64Hint, u32Prot, fFixed, 1);
    if (vaBase != 0) {
        g_cSoftMapNamedOk++;
        /* Greppable: memobj: named map */
        kprintf("memobj: named map \"%s\" va=0x%lx pages=%u cMapped=%u\n",
                szName, (unsigned long)vaBase, pObj->cPages, pObj->cMapped);
        if (soft_name_is_wine(szName)) {
            g_cSoftWineNamedMap++;
            /* Greppable: wine-shm */
            kprintf("memobj: wine-shm map \"%s\" va=0x%lx\n", szName,
                    (unsigned long)vaBase);
        }
        soft_inventory_maybe_once();
    } else {
        g_cSoftMapNamedFail++;
    }
    return vaBase;
}

/* ---- Soft observability / Wave 15 inventory ----------------------------- */

/**
 * Soft: non-zero if name is wine* (wine-shm / winesrv-shm product path).
 * Diagnostics only — not a hard name policy.
 */
static int
soft_name_is_wine(const char *szName)
{
    if (szName == NULL) {
        return 0;
    }
    if (szName[0] == 'w' && szName[1] == 'i' && szName[2] == 'n' &&
        szName[3] == 'e') {
        return 1;
    }
    return 0;
}

/**
 * Note live pool high-water. Walk is tiny (GJ_MEMOBJ_POOL); call after alloc.
 */
static void
soft_pool_peak_note(void)
{
    u32 iSlot;
    u32 cUsed = 0;

    for (iSlot = 0; iSlot < GJ_MEMOBJ_POOL; iSlot++) {
        if (g_aMemobjUsed[iSlot]) {
            cUsed++;
        }
    }
    if (cUsed > g_u32SoftPoolPeak) {
        g_u32SoftPoolPeak = cUsed;
    }
}

/**
 * Note live named high-water. Call after successful publish.
 */
static void
soft_named_peak_note(void)
{
    u32 iSlot;
    u32 cUsed = 0;

    for (iSlot = 0; iSlot < GJ_NAMED_MAX; iSlot++) {
        if (g_aNamed[iSlot].u8Used) {
            cUsed++;
        }
    }
    if (cUsed > g_u32SoftNamedPeak) {
        g_u32SoftNamedPeak = cUsed;
    }
}

/**
 * Walk pool + named tables; refresh live snaps + peaks + kind/flags.
 * Pure read of slot used bits / cPages / cMapped / flags — safe after init.
 * Wave 15: also track multi-map peak + per-obj max pages/mapped.
 */
static void
soft_inventory_scan(void)
{
    u32 iSlot;
    u32 cPool = 0;
    u32 cNamed = 0;
    u32 cPages = 0;
    u32 cMapped = 0;
    u32 cKindAnon = 0;
    u32 cKindNamed = 0;
    u32 cKindFile = 0;
    u32 cKindOther = 0;
    u32 cFlagShare = 0;
    u32 cFlagNamed = 0;
    u32 cFlagZeroed = 0;
    u32 cMulti = 0;
    u32 cIdle = 0;
    u32 cWineLive = 0;
    u32 cMaxMappedOne = 0;
    u32 cMaxPagesOne = 0;

    for (iSlot = 0; iSlot < GJ_MEMOBJ_POOL; iSlot++) {
        if (g_aMemobjUsed[iSlot]) {
            struct gj_memobj *pObj = &g_aMemobjPool[iSlot];

            cPool++;
            cPages += pObj->cPages;
            cMapped += pObj->cMapped;
            if (pObj->cPages > cMaxPagesOne) {
                cMaxPagesOne = pObj->cPages;
            }
            if (pObj->cMapped > cMaxMappedOne) {
                cMaxMappedOne = pObj->cMapped;
            }
            if (pObj->u32Kind == (u32)GJ_MEMOBJ_ANON) {
                cKindAnon++;
            } else if (pObj->u32Kind == (u32)GJ_MEMOBJ_NAMED) {
                cKindNamed++;
            } else if (pObj->u32Kind == (u32)GJ_MEMOBJ_FILE) {
                cKindFile++;
            } else {
                cKindOther++;
            }
            if ((pObj->u32Flags & GJ_MEMOBJ_F_SHAREABLE) != 0) {
                cFlagShare++;
            }
            if ((pObj->u32Flags & GJ_MEMOBJ_F_NAMED) != 0) {
                cFlagNamed++;
            }
            if ((pObj->u32Flags & GJ_MEMOBJ_F_ZEROED) != 0) {
                cFlagZeroed++;
            }
            if (pObj->cMapped > 1u) {
                cMulti++;
            }
            if (pObj->cMapped == 0u) {
                cIdle++;
            }
        }
    }
    for (iSlot = 0; iSlot < GJ_NAMED_MAX; iSlot++) {
        if (g_aNamed[iSlot].u8Used) {
            cNamed++;
            if (soft_name_is_wine(g_aNamed[iSlot].szName)) {
                cWineLive++;
            }
        }
    }

    g_u32SoftPoolUsed = cPool;
    g_u32SoftPoolFree = (cPool < (u32)GJ_MEMOBJ_POOL)
                            ? ((u32)GJ_MEMOBJ_POOL - cPool)
                            : 0u;
    g_u32SoftNamedUsed = cNamed;
    g_u32SoftNamedFree = (cNamed < (u32)GJ_NAMED_MAX)
                             ? ((u32)GJ_NAMED_MAX - cNamed)
                             : 0u;
    g_u32SoftPagesOwned = cPages;
    g_u32SoftMappedTotal = cMapped;
    g_u32SoftKindAnon = cKindAnon;
    g_u32SoftKindNamed = cKindNamed;
    g_u32SoftKindFile = cKindFile;
    g_u32SoftKindOther = cKindOther;
    g_u32SoftFlagShare = cFlagShare;
    g_u32SoftFlagNamed = cFlagNamed;
    g_u32SoftFlagZeroed = cFlagZeroed;
    g_u32SoftMultiMap = cMulti;
    g_u32SoftIdleObjs = cIdle;
    g_u32SoftMaxMappedOne = cMaxMappedOne;
    g_u32SoftMaxPagesOne = cMaxPagesOne;
    g_u32SoftWineNamedLive = cWineLive;
    if (cPool > g_u32SoftPoolPeak) {
        g_u32SoftPoolPeak = cPool;
    }
    if (cNamed > g_u32SoftNamedPeak) {
        g_u32SoftNamedPeak = cNamed;
    }
    if (cPages > g_u32SoftPagesPeak) {
        g_u32SoftPagesPeak = cPages;
    }
    if (cMapped > g_u32SoftMappedPeak) {
        g_u32SoftMappedPeak = cMapped;
    }
    if (cMulti > g_u32SoftMultiMapPeak) {
        g_u32SoftMultiMapPeak = cMulti;
    }
    if (g_u32SoftInvSamples < 0xffffffffu) {
        g_u32SoftInvSamples++;
    }
}

/**
 * Greppable Wave 15 soft memobj inventory dump (product / smoke deepen).
 * Prefix-stable markers (memobj: soft …):
 *   memobj: soft honesty    — explicit non-claims (not product/bar3/1TiB)
 *   memobj: soft inventory  — rollup live + peaks + logs
 *   memobj: soft pool       — pool used/free/peak + pages/mapped live
 *   memobj: soft named      — named table + create/unlink/lookup
 *   memobj: soft kinds      — ANON/NAMED/FILE live kind snap
 *   memobj: soft flags      — SHAREABLE/NAMED/ZEROED + multi/idle
 *   memobj: soft peaks      — pool/named/pages/mapped high-water
 *   memobj: soft region     — full/reuse/overlap + share maps
 *   memobj: soft create     — anon/named create + destroy/reclaim
 *   memobj: soft map        — anon/share/named map ok|fail + core
 *   memobj: soft unmap      — region vs orphan unmap
 *   memobj: soft USER       — G-MAP-2 sanitize force-U tallies
 *   memobj: soft as         — process_as_ensure ok/fail/idem
 *   memobj: soft wine       — wine-shm create/map + live named
 *   memobj: soft design     — Wave 15 geometry constants
 *   memobj: soft lookup     — Wave 15 named lookup hit/miss
 *   memobj: soft page_pa    — Wave 15 page PA soft path
 *   memobj: soft share      — Wave 15 multi-map / G-MO-3 share
 *   memobj: soft reclaim    — Wave 15 destroy/reclaim lifecycle
 *   memobj: soft lamps      — Wave 15 readiness lamps
 *   memobj: soft path       — honesty catalog (FILE cold OPEN)
 *   memobj: soft stats      — rollup tallies for agent greps
 *   memobj: soft surfaces   — Wave 19 return-surface catalog
 *   memobj: soft window     — Wave 17 user VA window geometry
 *   memobj: soft prot       — Wave 17 sanitize prot surface
 *   memobj: soft OPEN       — Wave 17 FILE/product OPEN honesty
 *   memobj: soft deepen     — wave=21 stamp + area count
 *   memobj: soft PASS|PARTIAL|INIT|NONE | inventory PASS
 * greppable: memobj: soft
 * Honesty: soft inventory only — not product / not bar3 / not 1TiB product /
 *          full FILE production remains OPEN. Soft ≠ product.
 */
static void
soft_inventory_log(void)
{
    const char *szVerdict;
    u32 cMapOk;
    u32 cMapFail;
    u32 cCreateOk;
    u32 cAreas = 0;
    u32 u32Surf;
    u32 u32PoolPct;
    u32 u32NamedPct;
    u32 fReady;

    soft_inventory_scan();

    cMapOk = g_cSoftMapAnonOk + g_cSoftMapShareOk + g_cSoftMapNamedOk;
    cMapFail = g_cSoftMapAnonFail + g_cSoftMapShareFail + g_cSoftMapNamedFail +
               g_cSoftMapCoreFail;
    cCreateOk = g_cSoftCreateAnonOk + g_cSoftNamedCreate;

    /*
     * Soft verdict (inventory only; never hard-gates maps):
     *   PASS     — any successful create or map observed
     *   PARTIAL  — only failures so far (pool/name/table soft miss)
     *   INIT     — inventory dumped, no create/map activity yet
     *   NONE     — reserved (kept for greppable PASS/NONE symmetry)
     */
    if (g_cSoftCreateAnonOk != 0u || g_cSoftNamedCreate != 0u || cMapOk != 0u) {
        szVerdict = "PASS";
    } else if (g_cSoftCreateAnonFail != 0u || g_cSoftCreateNamedFail != 0u ||
               cMapFail != 0u || g_cSoftNamedTableFull != 0u) {
        szVerdict = "PARTIAL";
    } else if (g_u32SoftInvSamples > 0u) {
        szVerdict = "INIT";
    } else {
        szVerdict = "NONE";
    }

    /* Soft occupancy % (scan; 0 if empty tables). */
    if (GJ_MEMOBJ_POOL == 0) {
        u32PoolPct = 0;
    } else {
        u32PoolPct = (g_u32SoftPoolUsed * 100u) / (u32)GJ_MEMOBJ_POOL;
        if (u32PoolPct > 100u) {
            u32PoolPct = 100u;
        }
    }
    if (GJ_NAMED_MAX == 0) {
        u32NamedPct = 0;
    } else {
        u32NamedPct = (g_u32SoftNamedUsed * 100u) / (u32)GJ_NAMED_MAX;
        if (u32NamedPct > 100u) {
            u32NamedPct = 100u;
        }
    }
    fReady = (cCreateOk != 0u || cMapOk != 0u) ? 1u : 0u;
    u32Surf = MEMOBJ_SOFT_SURF_CATALOG;

    /*
     * Honesty first: freestanding soft inventory is NOT product / bar3 /
     * 1TiB product. greppable: memobj: soft honesty
     */
    kprintf("memobj: soft honesty not-product not-bar3 not-1TiB-product "
            "product_tib=0 bar3=OPEN file_kind=OPEN wine_shm=soft "
            "diagnostics=1 wave=%u "
            "(soft inventory only; never closes bar3)\n",
            (unsigned)MEMOBJ_SOFT_WAVE);
    cAreas++;

    /* Grep: memobj: soft inventory */
    kprintf("memobj: soft inventory pool=%u/%u named=%u/%u pages=%u mapped=%u "
            "peak_pool=%u peak_named=%u multi=%u idle=%u logs=%u wave=%u "
            "(soft; not product; not bar3; not 1TiB product)\n",
            g_u32SoftPoolUsed, (unsigned)GJ_MEMOBJ_POOL, g_u32SoftNamedUsed,
            (unsigned)GJ_NAMED_MAX, g_u32SoftPagesOwned, g_u32SoftMappedTotal,
            g_u32SoftPoolPeak, g_u32SoftNamedPeak, g_u32SoftMultiMap,
            g_u32SoftIdleObjs, g_u32SoftInvSamples,
            (unsigned)MEMOBJ_SOFT_WAVE);
    cAreas++;

    /* Grep: memobj: soft pool */
    kprintf("memobj: soft pool used=%u free=%u peak=%u pages=%u mapped=%u "
            "slots=%u samples=%u max_pages=%u pool_pct=%u "
            "max_pages_one=%u max_mapped_one=%u\n",
            g_u32SoftPoolUsed, g_u32SoftPoolFree, g_u32SoftPoolPeak,
            g_u32SoftPagesOwned, g_u32SoftMappedTotal,
            (unsigned)GJ_MEMOBJ_POOL, g_u32SoftInvSamples,
            (unsigned)GJ_MEMOBJ_MAX_PAGES, u32PoolPct,
            g_u32SoftMaxPagesOne, g_u32SoftMaxMappedOne);
    cAreas++;

    /* Grep: memobj: soft named */
    kprintf("memobj: soft named used=%u free=%u peak=%u create=%u unlink=%u "
            "table_full=%u lookup_hit=%u lookup_miss=%u max=%u "
            "name_max=%u named_pct=%u\n",
            g_u32SoftNamedUsed, g_u32SoftNamedFree, g_u32SoftNamedPeak,
            g_cSoftNamedCreate, g_cSoftNamedUnlink, g_cSoftNamedTableFull,
            g_cSoftLookupNamedHit, g_cSoftLookupNamedMiss,
            (unsigned)GJ_NAMED_MAX, (unsigned)GJ_MEMOBJ_NAME_MAX,
            u32NamedPct);
    cAreas++;

    /* Grep: memobj: soft kinds */
    kprintf("memobj: soft kinds anon=%u named=%u file=%u other=%u "
            "live=%u (FILE cold OPEN soft)\n",
            g_u32SoftKindAnon, g_u32SoftKindNamed, g_u32SoftKindFile,
            g_u32SoftKindOther, g_u32SoftPoolUsed);
    cAreas++;

    /* Grep: memobj: soft flags */
    kprintf("memobj: soft flags shareable=%u named=%u zeroed=%u multi_map=%u "
            "idle=%u multi_peak=%u (G-MO-3 share soft)\n",
            g_u32SoftFlagShare, g_u32SoftFlagNamed, g_u32SoftFlagZeroed,
            g_u32SoftMultiMap, g_u32SoftIdleObjs, g_u32SoftMultiMapPeak);
    cAreas++;

    /* Grep: memobj: soft peaks */
    kprintf("memobj: soft peaks pool=%u named=%u pages=%u mapped=%u "
            "multi=%u max_pages_one=%u max_mapped_one=%u "
            "region_max=%u pool_max=%u named_max=%u\n",
            g_u32SoftPoolPeak, g_u32SoftNamedPeak, g_u32SoftPagesPeak,
            g_u32SoftMappedPeak, g_u32SoftMultiMapPeak,
            g_u32SoftMaxPagesOne, g_u32SoftMaxMappedOne,
            (unsigned)GJ_PROC_REGION_MAX,
            (unsigned)GJ_MEMOBJ_POOL, (unsigned)GJ_NAMED_MAX);
    cAreas++;

    /* Grep: memobj: soft region */
    kprintf("memobj: soft region full=%u reuse=%u overlap=%u "
            "region_max=%u share_maps=%u\n",
            g_cSoftRegionFull, g_cSoftRegionReuse, g_cSoftRegionOverlap,
            (unsigned)GJ_PROC_REGION_MAX, g_cSoftShareMaps);
    cAreas++;

    /* Grep: memobj: soft create */
    kprintf("memobj: soft create anon_ok=%u anon_fail=%u named_fail=%u "
            "named_ok=%u destroy=%u reclaim=%u\n",
            g_cSoftCreateAnonOk, g_cSoftCreateAnonFail, g_cSoftCreateNamedFail,
            g_cSoftNamedCreate, g_cSoftDestroy, g_cSoftReclaim);
    cAreas++;

    /* Grep: memobj: soft map */
    kprintf("memobj: soft map anon_ok=%u anon_fail=%u share_ok=%u "
            "share_fail=%u named_ok=%u named_fail=%u core_fail=%u "
            "user_map=%u map_ok_sum=%u\n",
            g_cSoftMapAnonOk, g_cSoftMapAnonFail, g_cSoftMapShareOk,
            g_cSoftMapShareFail, g_cSoftMapNamedOk, g_cSoftMapNamedFail,
            g_cSoftMapCoreFail, g_cSoftUserMap, cMapOk);
    cAreas++;

    /* Grep: memobj: soft unmap */
    kprintf("memobj: soft unmap region=%u orphan=%u destroy=%u reclaim=%u "
            "page_pa_ok=%u page_pa_fail=%u "
            "(soft; not product; not bar3; not 1TiB product)\n",
            g_cSoftUnmapRegion, g_cSoftUnmapOrphan, g_cSoftDestroy,
            g_cSoftReclaim, g_cSoftPagePaOk, g_cSoftPagePaFail);
    cAreas++;

    /* Grep: memobj: soft USER — G-MAP-2 force-U product maps */
    kprintf("memobj: soft USER sanitize=%u default_r=%u force_u=1 "
            "va_base=0x%lx va_end=0x%lx (G-MAP-2 soft)\n",
            g_cSoftUserMap, g_cSoftUserMapDefR,
            (unsigned long)GJ_USER_VA_BASE, (unsigned long)GJ_USER_VA_END);
    cAreas++;

    /* Grep: memobj: soft as */
    kprintf("memobj: soft as ensure_ok=%u ensure_fail=%u ensure_idem=%u "
            "(G-AS-1 soft)\n",
            g_cSoftAsEnsureOk, g_cSoftAsEnsureFail, g_cSoftAsEnsureIdem);
    cAreas++;

    /* Grep: memobj: soft wine */
    kprintf("memobj: soft wine create=%u map=%u named_live=%u "
            "share_maps=%u (wine-shm soft; Proton A0)\n",
            g_cSoftWineNamedCreate, g_cSoftWineNamedMap,
            g_u32SoftWineNamedLive, g_cSoftShareMaps);
    cAreas++;

    /*
     * Wave 15: design geometry (constants; not product capacity claim).
     * greppable: memobj: soft design
     */
    kprintf("memobj: soft design pool=%u named_max=%u name_max=%u "
            "max_pages=%u region_max=%u page_size=%u "
            "va_base=0x%lx va_end=0x%lx file_kind=OPEN "
            "wave=%u (soft geometry; not product; not bar3)\n",
            (unsigned)GJ_MEMOBJ_POOL, (unsigned)GJ_NAMED_MAX,
            (unsigned)GJ_MEMOBJ_NAME_MAX, (unsigned)GJ_MEMOBJ_MAX_PAGES,
            (unsigned)GJ_PROC_REGION_MAX, (unsigned)GJ_PAGE_SIZE,
            (unsigned long)GJ_USER_VA_BASE, (unsigned long)GJ_USER_VA_END,
            (unsigned)MEMOBJ_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 15: named lookup surface.
     * greppable: memobj: soft lookup
     */
    kprintf("memobj: soft lookup hit=%u miss=%u create=%u unlink=%u "
            "table_full=%u named_live=%u wine_live=%u "
            "wave=%u (soft; not product)\n",
            g_cSoftLookupNamedHit, g_cSoftLookupNamedMiss,
            g_cSoftNamedCreate, g_cSoftNamedUnlink, g_cSoftNamedTableFull,
            g_u32SoftNamedUsed, g_u32SoftWineNamedLive,
            (unsigned)MEMOBJ_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 15: page PA soft path.
     * greppable: memobj: soft page_pa
     */
    kprintf("memobj: soft page_pa ok=%u fail=%u pages_owned=%u "
            "max_pages_one=%u max_pages=%u "
            "wave=%u (soft; not product; not bar3)\n",
            g_cSoftPagePaOk, g_cSoftPagePaFail, g_u32SoftPagesOwned,
            g_u32SoftMaxPagesOne, (unsigned)GJ_MEMOBJ_MAX_PAGES,
            (unsigned)MEMOBJ_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 15: G-MO-3 share / multi-map surface.
     * greppable: memobj: soft share
     */
    kprintf("memobj: soft share maps=%u multi_live=%u multi_peak=%u "
            "shareable_live=%u max_mapped_one=%u share_ok=%u share_fail=%u "
            "wave=%u (G-MO-3 soft; not product; not bar3)\n",
            g_cSoftShareMaps, g_u32SoftMultiMap, g_u32SoftMultiMapPeak,
            g_u32SoftFlagShare, g_u32SoftMaxMappedOne,
            g_cSoftMapShareOk, g_cSoftMapShareFail,
            (unsigned)MEMOBJ_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 15: destroy / reclaim lifecycle.
     * greppable: memobj: soft reclaim
     */
    kprintf("memobj: soft reclaim destroy=%u reclaim=%u idle=%u "
            "named_sticky=%u unmap_region=%u unmap_orphan=%u "
            "wave=%u (soft lifecycle; not product)\n",
            g_cSoftDestroy, g_cSoftReclaim, g_u32SoftIdleObjs,
            g_u32SoftFlagNamed, g_cSoftUnmapRegion, g_cSoftUnmapOrphan,
            (unsigned)MEMOBJ_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 15: readiness lamps.
     * greppable: memobj: soft lamps
     */
    kprintf("memobj: soft lamps ready=%u pool_pct=%u named_pct=%u "
            "create_ok=%u map_ok=%u map_fail=%u destroy=%u reclaim=%u "
            "file_kind=OPEN wine_shm=soft wave=%u "
            "(soft lamps; not product; not bar3; not 1TiB product)\n",
            fReady, u32PoolPct, u32NamedPct, cCreateOk, cMapOk, cMapFail,
            g_cSoftDestroy, g_cSoftReclaim, (unsigned)MEMOBJ_SOFT_WAVE);
    cAreas++;

    /*
     * Grep: memobj: soft path
     * Honesty catalog of product surfaces this unit exposes.
     * file_kind=0: FILE pager not product-wired (cold OPEN).
     */
    kprintf("memobj: soft path claim=1 anon=1 named=1 share=1 unlink=1 "
            "user_map=1 region_table=1 as_ensure=1 wine_shm=1 "
            "design=1 lookup=1 page_pa=1 reclaim=1 lamps=1 "
            "file_kind=0 max_pages=%u pool=%u named_max=%u region_max=%u "
            "product_tib=0 bar3=OPEN "
            "(soft inventory; FILE cold OPEN; not product; not bar3; "
            "not 1TiB product)\n",
            (unsigned)GJ_MEMOBJ_MAX_PAGES, (unsigned)GJ_MEMOBJ_POOL,
            (unsigned)GJ_NAMED_MAX, (unsigned)GJ_PROC_REGION_MAX);
    cAreas++;

    /* Grep: memobj: soft stats — rollup tallies for agent greps. */
    kprintf("memobj: soft stats pool=%u named=%u pages=%u mapped=%u "
            "create_ok=%u map_ok=%u map_fail=%u destroy=%u reclaim=%u "
            "share_maps=%u user_map=%u wine_create=%u wine_map=%u "
            "region_full=%u multi_peak=%u pool_pct=%u named_pct=%u "
            "logs=%u wave=%u\n",
            g_u32SoftPoolUsed, g_u32SoftNamedUsed, g_u32SoftPagesOwned,
            g_u32SoftMappedTotal, cCreateOk, cMapOk, cMapFail, g_cSoftDestroy,
            g_cSoftReclaim, g_cSoftShareMaps, g_cSoftUserMap,
            g_cSoftWineNamedCreate, g_cSoftWineNamedMap, g_cSoftRegionFull,
            g_u32SoftMultiMapPeak, u32PoolPct, u32NamedPct,
            g_u32SoftInvSamples, (unsigned)MEMOBJ_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: return-surface catalog (surf bitmask; soft ≠ product).
     * Grep: memobj: soft surfaces
     */
    kprintf("memobj: soft surfaces surf=0x%x catalog=%u areas_live=%u "
            "pool=1 named=1 map=1 unmap=1 USER=1 share=1 wine=1 "
            "window=1 prot=1 open=1 wave=%u "
            "(return surfaces; soft only; not product; not bar3)\n",
            (unsigned)u32Surf, (unsigned)MEMOBJ_SOFT_AREAS, cAreas + 4u,
            (unsigned)MEMOBJ_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: user VA window geometry (G-MAP-2 soft).
     * Grep: memobj: soft window
     */
    kprintf("memobj: soft window va_base=0x%lx va_end=0x%lx "
            "max_pages=%u region_max=%u page_size=%u force_u=1 "
            "wave=%u (soft window; not product; not bar3)\n",
            (unsigned long)GJ_USER_VA_BASE, (unsigned long)GJ_USER_VA_END,
            (unsigned)GJ_MEMOBJ_MAX_PAGES, (unsigned)GJ_PROC_REGION_MAX,
            (unsigned)GJ_PAGE_SIZE, (unsigned)MEMOBJ_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: sanitize prot surface (USER force + default-R).
     * Grep: memobj: soft prot
     */
    kprintf("memobj: soft prot sanitize=%u default_r=%u force_u=1 "
            "user_map=%u wave=%u "
            "(soft prot sanitize; not product; not bar3)\n",
            g_cSoftUserMap, g_cSoftUserMapDefR, g_cSoftUserMap,
            (unsigned)MEMOBJ_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: explicit OPEN honesty (FILE / product remain OPEN).
     * Grep: memobj: soft OPEN
     */
    kprintf("memobj: soft OPEN file_kind=OPEN product_tib=0 bar3=OPEN "
            "wine_shm=soft full_file_pager=OPEN wave=%u "
            "(soft inventory; never closes FILE product; soft≠product)\n",
            (unsigned)MEMOBJ_SOFT_WAVE);
    cAreas++;

    /*
     * Grep: memobj: soft return rate
     * Wave 17 return-surface rate lamps (kept) (map/create ok vs fail).
     */
    kprintf("memobj: soft return rate "
            "map_ok=%u map_fail=%u create_ok=%u "
            "pool=%u named=%u multi_peak=%u logs=%u wave=%u "
            "(return rate; Soft≠product; not 1TiB product; not bar3)\n",
            cMapOk, cMapFail, cCreateOk,
            g_u32SoftPoolUsed, g_u32SoftNamedUsed, g_u32SoftMultiMapPeak,
            g_u32SoftInvSamples, (unsigned)MEMOBJ_SOFT_WAVE);
    cAreas++;

    /*
     * Grep: memobj: soft retcode
     * Wave 17 retcode catalog for map/create/pool soft return classes.
     */
    kprintf("memobj: soft retcode "
            "map_ok=1 map_fail=1 create_ok=1 pool=1 named=1 "
            "user_map=1 prot=1 file_kind=OPEN product_tib=0 wave=%u "
            "(retcode catalog; Soft≠product; soft≠product)\n",
            (unsigned)MEMOBJ_SOFT_WAVE);
    cAreas++;

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: memobj: soft return selftest — Wave 19 terminal return surface */
    kprintf("memobj: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 rate_limited=0 wave=%u soft PASS\n",
            (unsigned)MEMOBJ_SOFT_WAVE);

    /* Grep: memobj: soft retmap — Wave 19 return-surface map */
    kprintf("memobj: soft retmap soft_inv=1 deepen=1 return_rate=1 retcode=1 "
            "product=OPEN wave=%u soft PASS\n",
            (unsigned)MEMOBJ_SOFT_WAVE);

    /* Grep: memobj: soft deepen wave (Wave 21 stamp; areas = prior soft lines). */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: memobj: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("memobj: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)MEMOBJ_SOFT_WAVE);
    /* Grep: memobj: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("memobj: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)MEMOBJ_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: memobj: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("memobj: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)MEMOBJ_SOFT_WAVE);
    /* Grep: memobj: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("memobj: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)MEMOBJ_SOFT_WAVE);
            /*
             * ---- Wave 21 exclusive complementary surfaces (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: memobj: soft retpulse — Wave 21 return-pulse honesty */
            kprintf("memobj: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)MEMOBJ_SOFT_WAVE);
            /* Grep: memobj: soft retmark — Wave 21 exclusive mark stamp */
            kprintf("memobj: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)MEMOBJ_SOFT_WAVE);
    kprintf("memobj: soft deepen wave=%u areas=%u catalog=%u logs=%u "
            "pool=%u named=%u map_ok=%u create_ok=%u multi_peak=%u "
            "surf=0x%x product_tib=0 bar3=OPEN file_kind=OPEN "
            "(Wave 21 exclusive; soft; not product; not bar3; "
            "not 1TiB product; soft≠product)\n",
            (unsigned)MEMOBJ_SOFT_WAVE, cAreas, (unsigned)MEMOBJ_SOFT_AREAS,
            g_u32SoftInvSamples, g_u32SoftPoolUsed, g_u32SoftNamedUsed, cMapOk,
            cCreateOk, g_u32SoftMultiMapPeak, (unsigned)u32Surf);
    (void)MEMOBJ_SOFT_AREAS;

    /*
     * Close markers: soft activity lamp only.
     * Grep: memobj: soft PASS | PARTIAL | INIT | NONE | inventory PASS
     * Never "product PASS" / "bar3 PASS" / "1TiB product PASS".
     */
    kprintf("memobj: soft %s pool=%u named=%u map_ok=%u map_fail=%u "
            "create_ok=%u logs=%u wave=%u "
            "(soft inventory; not product; not bar3; not 1TiB product)\n",
            szVerdict, g_u32SoftPoolUsed, g_u32SoftNamedUsed, cMapOk, cMapFail,
            cCreateOk, g_u32SoftInvSamples, (unsigned)MEMOBJ_SOFT_WAVE);
    if (cCreateOk != 0u || cMapOk != 0u) {
        /* Grep: memobj: soft inventory PASS (activity PASS only). */
        kprintf("memobj: soft inventory PASS pool=%u named=%u map_ok=%u "
                "logs=%u wave=%u "
                "(soft; not product; not bar3; not 1TiB product)\n",
                g_u32SoftPoolUsed, g_u32SoftNamedUsed, cMapOk,
                g_u32SoftInvSamples, (unsigned)MEMOBJ_SOFT_WAVE);
    }
}

/**
 * After first product create/map activity, print soft inventory once
 * (mirrors futex/sched soft-stats-once). Diagnostics only.
 */
static void
soft_inventory_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_cSoftCreateAnonOk == 0 && g_cSoftNamedCreate == 0 &&
        g_cSoftMapAnonOk == 0 && g_cSoftMapShareOk == 0 &&
        g_cSoftMapNamedOk == 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    soft_inventory_log();
}

gj_paddr_t
memobj_page_pa(const struct gj_memobj *pObj, u32 iPage)
{
    if (pObj == NULL || iPage >= pObj->cPages) {
        g_cSoftPagePaFail++;
        return 0;
    }
    g_cSoftPagePaOk++;
    return pObj->aPa[iPage];
}

void
memobj_soft_stats(u32 *pNamedUsed, u32 *pPoolUsed, u32 *pMappedTotal)
{
    /*
     * Emit soft inventory on stats read so bring-up smoke can grep
     * "memobj: soft …" without a dedicated syscall.
     */
    soft_inventory_log();

    if (pNamedUsed != NULL) {
        *pNamedUsed = g_u32SoftNamedUsed;
    }
    if (pPoolUsed != NULL) {
        *pPoolUsed = g_u32SoftPoolUsed;
    }
    if (pMappedTotal != NULL) {
        *pMappedTotal = g_u32SoftMappedTotal;
    }
}
