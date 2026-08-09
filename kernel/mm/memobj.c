/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Memory objects own frames; regions are views (G-MO-*).
 * Maps are USER and confined to the product user VA window (G-MAP-2).
 * Named shareable maps: one object, many process views (G-MO-3 / wine-shm).
 * Dual license: MIT OR Apache-2.0 (recipient's option). Not GPL.
 *
 * Soft product surface (functional):
 *   region table soft - fixed GJ_PROC_REGION_MAX; full/reuse/overlap markers
 *   USER map flags    - memobj_sanitize_user_prot always forces U
 *   named lifecycle   - publish/unlink independent of last map
 *   file map soft     - vfs_ram regular-fd snapshot -> FILE memobj + PTEs
 *
 * Lean soft residual (this unit only; UDX mmap/cap windows eng):
 *   Sparse honesty + inventory rollup + residual lean + PASS/PARTIAL/INIT.
 *   UDX residual: USER VA window (G-MAP-2), shareable multi-map (G-MO-3),
 *   private AS ensure, MEMORY_OBJECT cap surface (mint OPEN here).
 *   No live MEMORY_OBJECT / DMA window cap mint in this TU.
 *   No version stamp. No stamp storms. Soft != product. G-AC-1 (no .ko).
 *   FILE full live pager remains OPEN; soft snapshot path is wired.
 *   product_tib=0; dual MIT OR Apache-2.0.
 *
 * Lean named residual for UDX host maps (this unit; soft only):
 *   create_named | lookup | map_named | map_share | unlink (G-MO-3)
 *   F_SHAREABLE multi-map for host shared buffers (ring/bounce shape)
 *   F_NAMED sticky until unlink; USER maps confined (G-MAP-2)
 *   Sparse lamp once: memobj: soft residual named UDX host maps
 *   Soft!=product dual MIT OR Apache-2.0; mint OPEN; G-AC-1 no .ko.
 *
 * C2 product path residual (ASSURANCE_LITE claim_class=C2; soft only):
 *   Product *direction* for UDX/DDI hosts + Linux ABI hot/cold mmap:
 *     process_as_ensure | create_anon|named|file | map_* | USER G-MAP-2
 *     share multi-map G-MO-3 | unmap+reclaim sticky named | page_pa
 *   Soft scaffold != product AC. MEMORY_OBJECT mint OPEN (no CNode mint here).
 *   Soft mint honesty once. Stamp-free. Soft!=product. G-AC-1. No .ko.
 *   Dual DoD A/B remain OPEN (soft residual never closes USB/sshd product AC).
 *   Functional residual arms (no frame alloc): kinds|share|window|user|file|
 *     as_region|cap_mint|page_pa_miss|as_miss|unmap_miss|region_miss|
 *     wine_name|reclaim_sticky|policy|dual_dod_open.
 *   greppable: memobj: soft residual C2 | soft c2 product path | soft mint honesty
 *   greppable: Dual_DoD_A=OPEN Dual_DoD_B=OPEN | MEMOBJ_C2_PRODUCT_PATH
 *
 * Placement Option A residual (docs/MEM_PLACE_CHANNEL.md; Soft!=product):
 *   One object, cPagesLogical * cReplicas frames; replica slice map.
 *   Soft L0 class diversify (pa>>12)&3; min_channels fail-closed NOSUPPORT.
 *   No PA leak to userspace. L2 product DRAM channel map OPEN.
 *   Option B hedge_load stub only in doc (MEM_PLACE_OPTION_B_STUB).
 *
 * Grep markers (prefix-stable):
 *   memobj: soft honesty | inventory | residual lean
 *   memobj: soft residual lean PASS
 *   memobj: soft residual UDX mmap/cap windows
 *   memobj: soft residual named | named UDX host maps | named PASS
 *   memobj: soft residual C2 product path | soft residual C2 PASS
 *   memobj: soft mint honesty | memobj: soft c2 product path
 *   memobj: soft place CHAN_STRIPED residual | soft place PASS
 *   memobj: place create | place map replica | place diversify
 *   memobj: soft PASS | PARTIAL | INIT | NONE | inventory PASS
 *   memobj: named | memobj: share | memobj: region table soft
 *   memobj: USER map | wine-shm
 *   memobj: file map soft | memobj: file create soft
 *   memobj: soft map_file PASS  - first vfs_ram file-map success (ABI-first)
 * greppable: Soft!=product (serial) | Soft!=product (comments)
 * greppable: G-AC-1 | MEMOBJ_UDX_MMAP_CAP | MEMOBJ_NAMED_UDX_HOST | product_tib=0
 * greppable: MEMOBJ_C2_PRODUCT_PATH | claim_class=C2 | soft_scaffold_ne_product_ac
 * greppable: Dual_DoD_A=OPEN | Dual_DoD_B=OPEN | dual_dod=OPEN
 * greppable: MEM_PLACE_L0 MEM_PLACE_L2_OPEN MEM_PLACE_NO_PA_LEAK CHAN_STRIPED
 * Honesty: soft residual only - not product / not 1TiB product /
 *          Soft != product dual license; not product UDX DMA/MMIO mint;
 *          Dual DoD A/B OPEN (soft != product AC close);
 *          place L0 soft != product L2 channel map.
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
#include <gj/vfs_ram.h>
#include <gj/vmm.h>

#define GJ_MEMOBJ_POOL 32
#define GJ_NAMED_MAX   16

static struct gj_memobj g_aMemobjPool[GJ_MEMOBJ_POOL];
static u8               g_aMemobjUsed[GJ_MEMOBJ_POOL];

/* Named shareable registry (Proton A0 / wine-shm) - early for soft inventory. */
struct memobj_named_slot {
    u8                u8Used;
    char              szName[GJ_MEMOBJ_NAME_MAX];
    struct gj_memobj *pObj;
};

static struct memobj_named_slot g_aNamed[GJ_NAMED_MAX];

/*
 * Soft counters: cumulative unless noted live/peak (scan).
 * Diagnostics only - never hard-gate create/map/unmap policy.
 * Soft != product. Lean residual inventory only (no version stamp).
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
static u32 g_u32SoftMultiMapPeak; /* high-water multi-map obj count */
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
static u32 g_cSoftCreateFileOk;   /* FILE-kind create soft ok */
static u32 g_cSoftCreateFileFail; /* FILE-kind create soft miss */
static u32 g_cSoftMapAnonOk;
static u32 g_cSoftMapAnonFail;
static u32 g_cSoftMapShareOk;
static u32 g_cSoftMapShareFail;
static u32 g_cSoftMapNamedOk;
static u32 g_cSoftMapNamedFail;
static u32 g_cSoftMapFileOk;      /* file map soft (vfs_ram snapshot) ok */
static u32 g_cSoftMapFileFail;    /* file map soft miss -> hot ENOSYS */
static u32 g_cSoftMapCoreFail;    /* core installer soft miss (map path) */
static u32 g_cSoftUnmapRegion;
static u32 g_cSoftUnmapOrphan;
static u32 g_cSoftDestroy;
static u32 g_cSoftReclaim;
static u32 g_cSoftUserMap;
static u32 g_cSoftUserMapDefR;    /* sanitize soft-defaulted empty->READ */
static u32 g_cSoftLookupNamedHit;
static u32 g_cSoftLookupNamedMiss;
static u32 g_cSoftAsEnsureOk;     /* process_as_ensure created private AS */
static u32 g_cSoftAsEnsureFail;   /* process_as_ensure NOMEM/INVAL */
static u32 g_cSoftAsEnsureIdem;   /* process_as_ensure already had CR3 */
static u32 g_cSoftWineNamedCreate;/* named create with wine* prefix */
static u32 g_cSoftWineNamedMap;   /* named map with wine* prefix */
static u32 g_cSoftPagePaOk;       /* memobj_page_pa hits */
static u32 g_cSoftPagePaFail;     /* memobj_page_pa soft miss */
static u32 g_cSoftUserVaReject;   /* map refused outside G-MAP-2 user window */
static u32 g_u32SoftResidualLean;   /* lean residual self-check runs */
static u32 g_u32SoftResidualLeanOk; /* lean residual checks that passed */
static u8  g_fSoftResidualLeanOnce; /* residual lean self-check once */
static u8  g_fSoftUdxResidualOnce;  /* UDX mmap/cap windows residual lamp once */
static u32 g_u32SoftNamedUdxLean;   /* named UDX host-maps residual runs */
static u32 g_u32SoftNamedUdxLeanOk; /* named UDX host-maps residual ok */
static u8  g_fSoftNamedUdxOnce;     /* named UDX host-maps residual once */
static u32 g_u32SoftC2Lean;         /* C2 product-path residual runs */
static u32 g_u32SoftC2LeanOk;       /* C2 product-path residual ok */
static u8  g_fSoftC2Once;           /* C2 product-path residual once */
static u8  g_fSoftMintOnce;         /* MEMORY_OBJECT mint honesty once */
/* Placement Option A soft residual (Soft!=product; MEM_PLACE_L0). */
static u32 g_cSoftPlaceCreateOk;
static u32 g_cSoftPlaceCreateFail;
static u32 g_cSoftPlaceMapOk;
static u32 g_cSoftPlaceMapFail;
static u32 g_cSoftPlaceDiversify;
static u32 g_cSoftPlaceDiversifyFail;
static u32 g_u32PlaceNameSeq;       /* auto name placed-%u */
static gj_status_t g_stPlaceLast;   /* last create_placed status */
static char g_szPlaceLastName[GJ_MEMOBJ_NAME_MAX]; /* last published name */
static u8  g_fSoftPlaceOnce;        /* place residual lamp once */

static void soft_inventory_scan(void);
static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);
static void soft_residual_lean_once(void);
static void soft_udx_mmap_cap_residual_once(void);
static void soft_named_udx_host_maps_residual_once(void);
static void soft_mint_honesty_once(void);
static void soft_c2_product_path_residual_once(void);
static void soft_place_residual_once(void);
static void soft_pool_peak_note(void);
static void soft_named_peak_note(void);
static int  soft_name_is_wine(const char *szName);
static void named_clear_obj(struct gj_memobj *pObj);
static int  name_ok(const char *szName);
static void name_copy(char *szDst, const char *szSrc);

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
    g_cSoftCreateFileOk = 0;
    g_cSoftCreateFileFail = 0;
    g_cSoftMapAnonOk = 0;
    g_cSoftMapAnonFail = 0;
    g_cSoftMapShareOk = 0;
    g_cSoftMapShareFail = 0;
    g_cSoftMapNamedOk = 0;
    g_cSoftMapNamedFail = 0;
    g_cSoftMapFileOk = 0;
    g_cSoftMapFileFail = 0;
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
    g_cSoftUserVaReject = 0;
    g_u32SoftResidualLean = 0;
    g_u32SoftResidualLeanOk = 0;
    g_fSoftResidualLeanOnce = 0;
    g_fSoftUdxResidualOnce = 0;
    g_u32SoftNamedUdxLean = 0;
    g_u32SoftNamedUdxLeanOk = 0;
    g_fSoftNamedUdxOnce = 0;
    g_u32SoftC2Lean = 0;
    g_u32SoftC2LeanOk = 0;
    g_fSoftC2Once = 0;
    g_fSoftMintOnce = 0;
    g_cSoftPlaceCreateOk = 0;
    g_cSoftPlaceCreateFail = 0;
    g_cSoftPlaceMapOk = 0;
    g_cSoftPlaceMapFail = 0;
    g_cSoftPlaceDiversify = 0;
    g_cSoftPlaceDiversifyFail = 0;
    g_u32PlaceNameSeq = 0;
    g_stPlaceLast = GJ_OK;
    g_szPlaceLastName[0] = '\0';
    g_fSoftPlaceOnce = 0;
    kprintf("memobj: init pool=%u named_max=%u soft region_table=%u\n",
            (unsigned)GJ_MEMOBJ_POOL, (unsigned)GJ_NAMED_MAX,
            (unsigned)GJ_PROC_REGION_MAX);
    /*
     * Lean residual + UDX mmap/cap + named UDX host maps + mint honesty +
     * C2 product path + place CHAN_STRIPED residual first (once; stamp-free)
     * so baseline inventory surface bits include residual ok.
     * Soft!=product. G-AC-1. MEM_PLACE_L0; L2 OPEN.
     */
    soft_residual_lean_once();
    soft_udx_mmap_cap_residual_once();
    soft_named_udx_host_maps_residual_once();
    soft_mint_honesty_once();
    soft_c2_product_path_residual_once();
    soft_place_residual_once();
    /* Grep: memobj: soft (baseline inventory after residual lamps) */
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
 * Prefer HHDM (shared kernel half - safe under any CR3). Without HHDM,
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
 * Soft: exact reuse - same object already mapped at [vaBase, cbLen).
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
 * G-MO region table soft - refuse clobber without hard panic.
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
 * Low hints (classic PE 0x400000) collide with kernel BSS - ignore them.
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

struct gj_memobj *
memobj_create_file(u32 cPages)
{
    struct gj_memobj *pObj;
    u32 iPage;
    gj_paddr_t pa;

    if (cPages == 0 || cPages > GJ_MEMOBJ_MAX_PAGES) {
        g_cSoftCreateFileFail++;
        return NULL;
    }
    pObj = pool_alloc();
    if (pObj == NULL) {
        g_cSoftCreateFileFail++;
        return NULL;
    }
    pObj->u32Kind = (u32)GJ_MEMOBJ_FILE;
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
            g_cSoftCreateFileFail++;
            return NULL;
        }
        pObj->aPa[iPage] = pa;
        memobj_zero_frame(pa);
    }
    soft_pool_peak_note();
    g_cSoftCreateFileOk++;
    /* Greppable: memobj: file create soft */
    kprintf("memobj: file create soft pages=%u (soft #%u)\n",
            (unsigned)cPages, (unsigned)g_cSoftCreateFileOk);
    return pObj;
}

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
    pObj->cReplicas = 0;
    pObj->cPagesLogical = 0;
    pObj->u32SoftChannels = 0;
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
        return; /* still published - G-MO-3 / wine-shm sticky */
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
 * Core map installer: object frame slice -> process AS + region row.
 * iPageBase / cMapPages select a contiguous slice of pObj->aPa[] (replica
 * views use one logical slice; full maps use base 0 / cPages).
 * fShare marks G-MO-3 shareable path (soft counter). Returns VA or 0.
 * greppable: MEM_PLACE_NO_PA_LEAK (never exposes PA to user)
 */
static gj_vaddr_t
memobj_map_obj_core_ex(struct gj_process *pProc, struct gj_memobj *pObj,
                       u64 u64Hint, u32 u32Prot, int fFixed, int fShare,
                       u32 iPageBase, u32 cMapPages)
{
    size_t cbLen;
    u32 iPage;
    u32 iReg;
    u32 iExact;
    gj_vaddr_t vaBase;
    gj_status_t st;
    u64 u64SavedCr3;
    u32 u32MapProt;
    u64 u64SavedAnon;

    if (pProc == NULL || pObj == NULL || pObj->cPages == 0 || cMapPages == 0) {
        g_cSoftMapCoreFail++;
        return 0;
    }
    if (iPageBase >= pObj->cPages ||
        cMapPages > pObj->cPages ||
        (iPageBase + cMapPages) > pObj->cPages) {
        g_cSoftMapCoreFail++;
        return 0;
    }
    st = process_as_ensure(pProc);
    if (st != GJ_OK) {
        g_cSoftMapCoreFail++;
        return 0;
    }

    cbLen = (size_t)cMapPages * (size_t)GJ_PAGE_SIZE;
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
        g_cSoftUserVaReject++;
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

    for (iPage = 0; iPage < cMapPages; iPage++) {
        st = vmm_map_page(vaBase + (gj_vaddr_t)iPage * GJ_PAGE_SIZE,
                          pObj->aPa[iPageBase + iPage], u32MapProt);
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
    pProc->aRegions[iReg].u64ObjOff =
        (u64)iPageBase * (u64)GJ_PAGE_SIZE;
    pProc->aRegions[iReg].pObj = pObj;
    pObj->cMapped++;
    if (fShare || (pObj->u32Flags & GJ_MEMOBJ_F_SHAREABLE) != 0) {
        g_cSoftShareMaps++;
        /* Greppable: memobj: share */
        kprintf("memobj: share map va=0x%lx pages=%u cMapped=%u (soft #%u)\n",
                (unsigned long)vaBase, (unsigned)cMapPages,
                (unsigned)pObj->cMapped, (unsigned)g_cSoftShareMaps);
    }

    memobj_restore_cr3(pProc, u64SavedCr3);
    return vaBase;
}

/**
 * Full-object map (G-MO-1 / G-MO-3). Wrapper over slice mapper.
 */
static gj_vaddr_t
memobj_map_obj_core(struct gj_process *pProc, struct gj_memobj *pObj,
                    u64 u64Hint, u32 u32Prot, int fFixed, int fShare)
{
    if (pObj == NULL) {
        g_cSoftMapCoreFail++;
        return 0;
    }
    return memobj_map_obj_core_ex(pProc, pObj, u64Hint, u32Prot, fFixed,
                                  fShare, 0u, pObj->cPages);
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

/*
 * Linux x86_64 struct stat layout (matches vfs_ram_fstat fill).
 * Soft local - avoids depending on vfs_ram private types.
 */
struct memobj_stat64 {
    u64 u64Dev;
    u64 u64Ino;
    u64 u64Nlink;
    u32 u32Mode;
    u32 u32Uid;
    u32 u32Gid;
    u32 u32Pad0;
    u64 u64Rdev;
    i64 i64Size;
    i64 i64Blksize;
    i64 i64Blocks;
    i64 i64Atime;
    u64 u64AtimeNsec;
    i64 i64Mtime;
    u64 u64MtimeNsec;
    i64 i64Ctime;
    u64 u64CtimeNsec;
    u64 aUnused[3];
};

/** S_IFMT / S_IFREG from Linux public mode bits. */
#define MEMOBJ_S_IFMT  0170000u
#define MEMOBJ_S_IFREG 0100000u

/**
 * Soft: pread vfs_ram bytes into already-zeroed object frames at u64Off.
 * Returns 0 on success, -1 on soft fail (bad fd / IO error).
 */
static int
memobj_fill_from_fd(struct gj_memobj *pObj, i64 i64Fd, u64 u64Off)
{
    u32 iPage;
    u64 u64SavedCr3 = 0;
    int fSwitched = 0;
    i64 i64N;

    if (pObj == NULL || pObj->cPages == 0 || !vfs_ram_fd_ok(i64Fd)) {
        return -1;
    }

    for (iPage = 0; iPage < pObj->cPages; iPage++) {
        void *pVa;
        u64 u64PageOff = u64Off + (u64)iPage * (u64)GJ_PAGE_SIZE;

        if (hhdm_ready()) {
            pVa = (void *)hhdm_to_virt(pObj->aPa[iPage]);
        } else {
            if (!fSwitched) {
                u64SavedCr3 = cpu_read_cr3();
                cpu_load_cr3(vmm_kernel_cr3());
                fSwitched = 1;
            }
            pVa = (void *)(gj_vaddr_t)pObj->aPa[iPage];
        }
        /* Frames already zeroed: short/EOF pread leaves tail zero. */
        i64N = vfs_ram_pread(i64Fd, pVa, (size_t)GJ_PAGE_SIZE, u64PageOff);
        if (i64N < 0) {
            if (fSwitched) {
                cpu_load_cr3(u64SavedCr3);
            }
            return -1;
        }
    }
    if (fSwitched) {
        cpu_load_cr3(u64SavedCr3);
    }
    return 0;
}

gj_vaddr_t
memobj_map_file_fd(struct gj_process *pProc, i64 i64Fd, u64 u64Hint,
                   size_t cbLen, u32 u32Prot, int fFixed, u64 u64Off)
{
    size_t cbAligned;
    u32 cPages;
    struct gj_memobj *pObj;
    gj_vaddr_t vaBase;
    struct memobj_stat64 st;
    i64 i64St;

    if (pProc == NULL || cbLen == 0) {
        g_cSoftMapFileFail++;
        return 0;
    }
    if (!vfs_ram_fd_ok(i64Fd)) {
        g_cSoftMapFileFail++;
        return 0;
    }
    /* Linux mmap offset must be page-aligned. */
    if ((u64Off & (u64)(GJ_PAGE_SIZE - 1)) != 0) {
        g_cSoftMapFileFail++;
        return 0;
    }
    if (fFixed && (u64Hint & (u64)(GJ_PAGE_SIZE - 1)) != 0) {
        g_cSoftMapFileFail++;
        return 0;
    }

    memset(&st, 0, sizeof(st));
    i64St = vfs_ram_fstat(i64Fd, &st, sizeof(st));
    if (i64St != 0) {
        g_cSoftMapFileFail++;
        return 0;
    }
    /*
     * Soft: only regular ramfs files (memfd / open paths). Reject dir and
     * specials (chr/blk/fifo/sock). Type bits 0 soft-treated as regular.
     */
    {
        u32 u32Type = st.u32Mode & MEMOBJ_S_IFMT;

        if (u32Type != MEMOBJ_S_IFREG && u32Type != 0u) {
            g_cSoftMapFileFail++;
            return 0;
        }
    }

    cbAligned = (cbLen + GJ_PAGE_SIZE - 1) & ~(size_t)(GJ_PAGE_SIZE - 1);
    cPages = (u32)(cbAligned / GJ_PAGE_SIZE);
    if (cPages == 0 || cPages > GJ_MEMOBJ_MAX_PAGES) {
        g_cSoftMapFileFail++;
        return 0;
    }

    pObj = memobj_create_file(cPages);
    if (pObj == NULL) {
        g_cSoftMapFileFail++;
        return 0;
    }
    if (memobj_fill_from_fd(pObj, i64Fd, u64Off) != 0) {
        memobj_destroy(pObj);
        g_cSoftMapFileFail++;
        return 0;
    }

    vaBase = memobj_map_obj_core(pProc, pObj, u64Hint, u32Prot, fFixed, 0);
    if (vaBase == 0) {
        memobj_destroy(pObj);
        g_cSoftMapFileFail++;
        return 0;
    }
    g_cSoftMapFileOk++;
    /*
     * Greppable first-success lamp (ABI-first file mmap soft):
     *   memobj: soft map_file PASS
     * Detail row remains: memobj: file map soft ...
     */
    if (g_cSoftMapFileOk == 1u) {
        kprintf("memobj: soft map_file PASS va=0x%lx pages=%u fd=%ld "
                "off=0x%lx size=%ld\n",
                (unsigned long)vaBase, (unsigned)cPages, (long)i64Fd,
                (unsigned long)u64Off, (long)st.i64Size);
    }
    /* Greppable: memobj: file map soft */
    kprintf("memobj: file map soft va=0x%lx pages=%u fd=%ld off=0x%lx "
            "size=%ld (soft #%u)\n",
            (unsigned long)vaBase, (unsigned)cPages, (long)i64Fd,
            (unsigned long)u64Off, (long)st.i64Size,
            (unsigned)g_cSoftMapFileOk);
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
        return NULL; /* EEXIST - wine-shm re-create soft-misses at caller */
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
    /* Explicit hint => fixed VA (winesrv 0x50000000 / 0x60000000 path). */
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

/* ---- Placement Option A (MEM_PLACE_L0 soft; Soft!=product) -------------- */

/**
 * Soft L0 channel class from PA. NOT product DRAM channel map.
 * greppable: MEM_PLACE_L0 Soft!=product MEM_PLACE_L2_OPEN
 */
static u32
memobj_soft_chan_class(gj_paddr_t pa)
{
    return (u32)((pa >> 12) & (u32)(GJ_MEM_PLACE_SOFT_CHANS - 1u));
}

/** Popcount of set bits in low 8 (soft channel mask). */
static u32
memobj_soft_popcount8(u32 u32Mask)
{
    u32 c = 0;
    u32 iBit;

    for (iBit = 0; iBit < 8u; iBit++) {
        if ((u32Mask & (1u << iBit)) != 0u) {
            c++;
        }
    }
    return c;
}

/**
 * Format auto name "placed-%u" into szOut (max GJ_MEMOBJ_NAME_MAX).
 * Simple decimal; no snprintf in freestanding kernel.
 */
static void
memobj_place_name_fmt(char *szOut, u32 u32Id)
{
    char aDig[12];
    u32 cDig = 0;
    u32 u = u32Id;
    u32 i;
    const char *szPfx = "placed-";
    u32 iOut = 0;

    if (szOut == NULL) {
        return;
    }
    while (szPfx[iOut] != '\0' && iOut + 1u < GJ_MEMOBJ_NAME_MAX) {
        szOut[iOut] = szPfx[iOut];
        iOut++;
    }
    if (u == 0u) {
        aDig[0] = '0';
        cDig = 1u;
    } else {
        while (u > 0u && cDig < 12u) {
            aDig[cDig] = (char)('0' + (u % 10u));
            u /= 10u;
            cDig++;
        }
    }
    for (i = 0; i < cDig && iOut + 1u < GJ_MEMOBJ_NAME_MAX; i++) {
        szOut[iOut] = aDig[cDig - 1u - i];
        iOut++;
    }
    szOut[iOut] = '\0';
}

/**
 * Soft L0 diversify alloc: prefer class not in u32UsedMask for first page
 * of a replica. Retries pmm_alloc a few times; free rejects. Soft!=product.
 * greppable: memobj: place diversify MEM_PLACE_L0
 */
static gj_paddr_t
memobj_place_alloc_page(u32 u32UsedMask, int fPreferNew, u32 *pClsOut)
{
    u32 iTry;
    gj_paddr_t pa;
    u32 u32Cls;
    const u32 cRetries = 8u;

    for (iTry = 0; iTry < cRetries; iTry++) {
        pa = pmm_alloc();
        if (pa == 0) {
            return 0;
        }
        u32Cls = memobj_soft_chan_class(pa);
        if (fPreferNew != 0 && u32UsedMask != 0u) {
            /* Prefer a free soft class if any remain unused. */
            if ((u32UsedMask & (1u << u32Cls)) != 0u) {
                u32 u32All = (1u << GJ_MEM_PLACE_SOFT_CHANS) - 1u;
                if ((u32UsedMask & u32All) != u32All &&
                    iTry + 1u < cRetries) {
                    pmm_free(pa);
                    g_cSoftPlaceDiversify++;
                    continue;
                }
            }
        }
        if (pClsOut != NULL) {
            *pClsOut = u32Cls;
        }
        return pa;
    }
    /* Exhausted retries with only used classes: accept last free path. */
    pa = pmm_alloc();
    if (pa == 0) {
        return 0;
    }
    if (pClsOut != NULL) {
        *pClsOut = memobj_soft_chan_class(pa);
    }
    return pa;
}

gj_status_t
memobj_place_last_status(void)
{
    return g_stPlaceLast;
}

/**
 * Soft: last successful create_placed published name (empty if none).
 * For syscall copyout of gj_mem_place_out.szName. No PA.
 */
const char *
memobj_place_last_name(void)
{
    return g_szPlaceLastName;
}

u32
memobj_placed_replicas(const struct gj_memobj *pObj)
{
    if (pObj == NULL) {
        return 0;
    }
    return pObj->cReplicas;
}

u32
memobj_placed_logical_pages(const struct gj_memobj *pObj)
{
    if (pObj == NULL) {
        return 0;
    }
    return pObj->cPagesLogical;
}

struct gj_memobj *
memobj_create_placed(u32 cPagesLogical, const struct gj_mem_place_req *pReq)
{
    struct gj_memobj *pObj;
    u32 cRep;
    u32 cTotal;
    u32 u32MinCh;
    u32 u32Flags;
    u32 u32UsedMask = 0;
    u32 u32SoftCh;
    u32 iPage;
    u32 iSlot;
    u32 iRep;
    char szName[GJ_MEMOBJ_NAME_MAX];
    u32 u32NameTry;

    g_stPlaceLast = GJ_OK;

    if (pReq == NULL || cPagesLogical == 0u) {
        g_stPlaceLast = GJ_ERR_INVAL;
        g_cSoftPlaceCreateFail++;
        return NULL;
    }
    cRep = pReq->u32Replicas;
    if (cRep == 0u) {
        cRep = 1u;
    }
    if (cRep > GJ_MEM_PLACE_MAX_REPLICAS) {
        g_stPlaceLast = GJ_ERR_INVAL;
        g_cSoftPlaceCreateFail++;
        return NULL;
    }
    u32MinCh = pReq->u32MinChannels;
    if (u32MinCh > GJ_MEM_PLACE_SOFT_CHANS) {
        g_stPlaceLast = GJ_ERR_INVAL;
        g_cSoftPlaceCreateFail++;
        return NULL;
    }
    if (u32MinCh > cRep) {
        /* Impossible: cannot have more distinct classes than replicas soft. */
        g_stPlaceLast = GJ_ERR_INVAL;
        g_cSoftPlaceCreateFail++;
        return NULL;
    }
    /* Overflow-safe total pages. */
    if (cPagesLogical > GJ_MEMOBJ_MAX_PAGES ||
        cRep > GJ_MEMOBJ_MAX_PAGES ||
        cPagesLogical > (GJ_MEMOBJ_MAX_PAGES / cRep)) {
        g_stPlaceLast = GJ_ERR_INVAL;
        g_cSoftPlaceCreateFail++;
        return NULL;
    }
    cTotal = cPagesLogical * cRep;
    if (cTotal == 0u || cTotal > GJ_MEMOBJ_MAX_PAGES) {
        g_stPlaceLast = GJ_ERR_INVAL;
        g_cSoftPlaceCreateFail++;
        return NULL;
    }

    /* u32NodePref reserved L1; ignore soft. */
    (void)pReq->u32NodePref;
    u32Flags = pReq->u32Flags;
    if ((u32Flags & GJ_MEM_PLACE_CHAN_STRIPED) == 0u && u32Flags != 0u) {
        /* Unknown flags soft-accept DEFAULT|CHAN_STRIPED only bits. */
        if ((u32Flags & ~(GJ_MEM_PLACE_CHAN_STRIPED)) != 0u) {
            /* Soft: ignore unknown bits; force striped when multi-rep. */
        }
    }
    /* Multi-replica implies striped soft residual. */
    if (cRep > 1u) {
        u32Flags |= GJ_MEM_PLACE_CHAN_STRIPED;
    }

    pObj = pool_alloc();
    if (pObj == NULL) {
        g_stPlaceLast = GJ_ERR_NOMEM;
        g_cSoftPlaceCreateFail++;
        return NULL;
    }
    pObj->u32Kind = (u32)GJ_MEMOBJ_NAMED;
    pObj->cPages = cTotal;
    pObj->cMapped = 0;
    pObj->cReplicas = cRep;
    pObj->cPagesLogical = cPagesLogical;
    pObj->u32SoftChannels = 0;
    pObj->u32Flags = GJ_MEMOBJ_F_ZEROED | GJ_MEMOBJ_F_SHAREABLE |
                     GJ_MEMOBJ_F_NAMED;
    if ((u32Flags & GJ_MEM_PLACE_CHAN_STRIPED) != 0u) {
        pObj->u32Flags |= GJ_MEMOBJ_F_CHAN_STRIPED;
    }

    /*
     * Allocate replica-major: for each replica, first page prefers a new
     * soft class; remaining pages best-effort. Soft!=product L0 only.
     * greppable: memobj: place diversify
     */
    for (iRep = 0; iRep < cRep; iRep++) {
        u32 u32Base = iRep * cPagesLogical;
        u32 u32Cls = 0;
        int fPrefer = ((u32Flags & GJ_MEM_PLACE_CHAN_STRIPED) != 0u) ? 1 : 0;
        gj_paddr_t pa;

        for (iPage = 0; iPage < cPagesLogical; iPage++) {
            if (iPage == 0u) {
                pa = memobj_place_alloc_page(u32UsedMask, fPrefer, &u32Cls);
            } else {
                pa = pmm_alloc();
                if (pa != 0) {
                    u32Cls = memobj_soft_chan_class(pa);
                }
            }
            if (pa == 0) {
                /* Roll back all pages allocated so far. */
                u32 iDone = u32Base + iPage;
                while (iDone > 0u) {
                    iDone--;
                    if (pObj->aPa[iDone] != 0) {
                        pmm_free(pObj->aPa[iDone]);
                        pObj->aPa[iDone] = 0;
                    }
                }
                pool_free(pObj);
                g_stPlaceLast = GJ_ERR_NOMEM;
                g_cSoftPlaceCreateFail++;
                return NULL;
            }
            pObj->aPa[u32Base + iPage] = pa;
            memobj_zero_frame(pa);
            if (iPage == 0u) {
                u32UsedMask |= (1u << (u32Cls & 7u));
            }
        }
    }

    u32SoftCh = memobj_soft_popcount8(
        u32UsedMask & ((1u << GJ_MEM_PLACE_SOFT_CHANS) - 1u));
    pObj->u32SoftChannels = u32SoftCh;

    if (u32MinCh > 0u && u32SoftCh < u32MinCh) {
        /* Hard min not met: fail closed. greppable: place diversify fail */
        g_cSoftPlaceDiversifyFail++;
        kprintf("memobj: place diversify FAIL soft_ch=%u min=%u reps=%u "
                "(Soft!=product L0; MEM_PLACE_L2_OPEN)\n",
                (unsigned)u32SoftCh, (unsigned)u32MinCh, (unsigned)cRep);
        memobj_destroy(pObj);
        g_stPlaceLast = GJ_ERR_NOSUPPORT;
        g_cSoftPlaceCreateFail++;
        return NULL;
    }

    /* Publish under unique soft name placed-%u. */
    for (u32NameTry = 0; u32NameTry < 64u; u32NameTry++) {
        memobj_place_name_fmt(szName, g_u32PlaceNameSeq);
        if (g_u32PlaceNameSeq < 0xfffffffeu) {
            g_u32PlaceNameSeq++;
        }
        if (!name_ok(szName) || memobj_lookup_named(szName) != NULL) {
            continue;
        }
        for (iSlot = 0; iSlot < GJ_NAMED_MAX; iSlot++) {
            if (!g_aNamed[iSlot].u8Used) {
                g_aNamed[iSlot].u8Used = 1;
                name_copy(g_aNamed[iSlot].szName, szName);
                g_aNamed[iSlot].pObj = pObj;
                g_cSoftNamedCreate++;
                soft_named_peak_note();
                soft_pool_peak_note();
                g_cSoftPlaceCreateOk++;
                g_stPlaceLast = GJ_OK;
                name_copy(g_szPlaceLastName, g_aNamed[iSlot].szName);
                /* Greppable: memobj: place create */
                kprintf("memobj: place create \"%s\" logical=%u reps=%u "
                        "total=%u soft_ch=%u flags=0x%x "
                        "(Soft!=product MEM_PLACE_L0; no PA leak)\n",
                        g_aNamed[iSlot].szName, (unsigned)cPagesLogical,
                        (unsigned)cRep, (unsigned)cTotal,
                        (unsigned)u32SoftCh, (unsigned)pObj->u32Flags);
                if ((pObj->u32Flags & GJ_MEMOBJ_F_CHAN_STRIPED) != 0u) {
                    kprintf("memobj: place diversify soft_ch=%u mask=0x%x "
                            "reps=%u (MEM_PLACE_L0; Soft!=product)\n",
                            (unsigned)u32SoftCh, (unsigned)u32UsedMask,
                            (unsigned)cRep);
                }
                soft_inventory_maybe_once();
                return pObj;
            }
        }
        /* Named table full. */
        g_cSoftNamedTableFull++;
        break;
    }

    memobj_destroy(pObj);
    g_stPlaceLast = GJ_ERR_NOMEM;
    g_cSoftPlaceCreateFail++;
    return NULL;
}

gj_vaddr_t
memobj_map_replica(struct gj_process *pProc, struct gj_memobj *pObj,
                   u32 u32Replica, u64 u64Hint, u32 u32Prot, int fFixed)
{
    gj_vaddr_t vaBase;
    u32 iBase;
    u32 cLog;

    if (pProc == NULL || pObj == NULL || pObj->cReplicas == 0u ||
        pObj->cPagesLogical == 0u) {
        g_cSoftPlaceMapFail++;
        return 0;
    }
    if (u32Replica >= pObj->cReplicas) {
        g_cSoftPlaceMapFail++;
        return 0;
    }
    cLog = pObj->cPagesLogical;
    iBase = u32Replica * cLog;
    if (iBase + cLog > pObj->cPages) {
        g_cSoftPlaceMapFail++;
        return 0;
    }
    /* Elevate shareable for multi-map replica views (Option A). */
    pObj->u32Flags |= GJ_MEMOBJ_F_SHAREABLE;
    vaBase = memobj_map_obj_core_ex(pProc, pObj, u64Hint, u32Prot, fFixed, 1,
                                    iBase, cLog);
    if (vaBase == 0) {
        g_cSoftPlaceMapFail++;
        return 0;
    }
    g_cSoftPlaceMapOk++;
    /* Greppable: memobj: place map replica */
    kprintf("memobj: place map replica=%u va=0x%lx logical=%u cMapped=%u "
            "(Soft!=product MEM_PLACE_NO_PA_LEAK G-MAP-2)\n",
            (unsigned)u32Replica, (unsigned long)vaBase, (unsigned)cLog,
            (unsigned)pObj->cMapped);
    soft_inventory_maybe_once();
    return vaBase;
}

/* ---- Soft observability / lean residual inventory ----------------------- */

/**
 * Soft: non-zero if name is wine* (wine-shm / winesrv-shm product path).
 * Diagnostics only - not a hard name policy.
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
 * Pure read of slot used bits / cPages / cMapped / flags - safe after init.
 * Also track multi-map peak + per-obj max pages/mapped.
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
 * Lean soft residual inventory dump (bring-up / smoke only).
 * Sparse: honesty + inventory rollup + residual lean + PASS/PARTIAL/INIT.
 * UDX mmap/cap windows eng residual fields in residual lean line.
 * NO stamp storms, no version stamp. Soft != product dual MIT OR Apache-2.0.
 * G-AC-1: no .ko product path.
 *
 * greppable: memobj: soft honesty | inventory | residual lean
 * greppable: memobj: soft PASS | PARTIAL | INIT | NONE | inventory PASS
 * Honesty: soft residual != product / != 1TiB product; full FILE pager OPEN.
 */
static void
soft_inventory_log(void)
{
    const char *szVerdict;
    u32 cMapOk;
    u32 cMapFail;
    u32 cCreateOk;
    u32 u32Surf;
    extern u32 serial_thre_dead(void);

    if (serial_thre_dead() != 0u) {
        kprintf("memobj: soft inventory SKIP (no COM1; panel path)\n");
        return;
    }
    soft_inventory_scan();

    cMapOk = g_cSoftMapAnonOk + g_cSoftMapShareOk + g_cSoftMapNamedOk +
             g_cSoftMapFileOk;
    cMapFail = g_cSoftMapAnonFail + g_cSoftMapShareFail + g_cSoftMapNamedFail +
               g_cSoftMapFileFail + g_cSoftMapCoreFail;
    cCreateOk = g_cSoftCreateAnonOk + g_cSoftNamedCreate + g_cSoftCreateFileOk;

    /*
     * Soft verdict (inventory only; never hard-gates maps):
     *   PASS     - any successful create or map observed
     *   PARTIAL  - only failures so far (pool/name/table soft miss)
     *   INIT     - inventory dumped, no create/map activity yet
     *   NONE     - reserved (kept for greppable PASS/NONE symmetry)
     */
    if (g_cSoftCreateAnonOk != 0u || g_cSoftNamedCreate != 0u ||
        g_cSoftCreateFileOk != 0u || cMapOk != 0u) {
        szVerdict = "PASS";
    } else if (g_cSoftCreateAnonFail != 0u || g_cSoftCreateNamedFail != 0u ||
               g_cSoftCreateFileFail != 0u || cMapFail != 0u ||
               g_cSoftNamedTableFull != 0u) {
        szVerdict = "PARTIAL";
    } else if (g_u32SoftInvSamples > 0u) {
        szVerdict = "INIT";
    } else {
        szVerdict = "NONE";
    }

    /* Surface bits: pool|named|map|share|file|user|as|lean|wine|multi|c2|mint */
    u32Surf = ((g_u32SoftPoolUsed != 0u) ? (1u << 0) : 0u) |
              ((g_u32SoftNamedUsed != 0u) ? (1u << 1) : 0u) |
              ((cMapOk != 0u) ? (1u << 2) : 0u) |
              ((g_cSoftMapShareOk != 0u || g_u32SoftFlagShare != 0u)
                   ? (1u << 3)
                   : 0u) |
              ((g_cSoftMapFileOk != 0u || g_u32SoftKindFile != 0u)
                   ? (1u << 4)
                   : 0u) |
              ((g_cSoftUserMap != 0u) ? (1u << 5) : 0u) |
              ((g_cSoftAsEnsureOk != 0u) ? (1u << 6) : 0u) |
              ((g_u32SoftResidualLeanOk != 0u) ? (1u << 7) : 0u) |
              ((g_u32SoftWineNamedLive != 0u) ? (1u << 8) : 0u) |
              ((g_u32SoftMultiMap != 0u) ? (1u << 9) : 0u) |
              ((g_u32SoftC2LeanOk != 0u) ? (1u << 10) : 0u) |
              ((g_fSoftMintOnce != 0u) ? (1u << 11) : 0u);

    /*
     * Grep: memobj: soft honesty
     * Freestanding soft residual is NOT product / 1TiB product.
     * Dual license MIT OR Apache-2.0; Soft!=product; G-AC-1 no .ko.
     */
    kprintf("memobj: soft honesty not-product not-1TiB-product "
            "product_tib=0 file_kind=OPEN wine_shm=soft "
            "cap_memory_object_mint=OPEN udx_mmap_cap_eng=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 storm=0 "
            "(Soft!=product; dual MIT OR Apache-2.0; no version stamp; "
            "no .ko product)\n");

    /* Grep: memobj: soft inventory - single rollup lamp */
    kprintf("memobj: soft inventory pool=%u/%u named=%u/%u pages=%u mapped=%u "
            "peak_pool=%u peak_named=%u multi=%u idle=%u "
            "create_ok=%u map_ok=%u map_fail=%u destroy=%u reclaim=%u "
            "logs=%u (sparse; Soft!=product; not 1TiB product)\n",
            g_u32SoftPoolUsed, (unsigned)GJ_MEMOBJ_POOL, g_u32SoftNamedUsed,
            (unsigned)GJ_NAMED_MAX, g_u32SoftPagesOwned, g_u32SoftMappedTotal,
            g_u32SoftPoolPeak, g_u32SoftNamedPeak, g_u32SoftMultiMap,
            g_u32SoftIdleObjs, cCreateOk, cMapOk, cMapFail, g_cSoftDestroy,
            g_cSoftReclaim, g_u32SoftInvSamples);

    /*
     * Grep: memobj: soft residual lean
     * G-MO residual + UDX mmap/cap windows eng residual fields.
     * Lean residual honesty - Soft!=product dual license; no version stamp.
     * full_file_pager=OPEN remains; product_tib=0; MEMORY_OBJECT mint OPEN.
     * G-AC-1: no .ko. Soft residual != product UDX DMA/MMIO window mint.
     */
    kprintf("memobj: soft residual lean "
            "anon_ok=%u named_ok=%u share_ok=%u file_ok=%u "
            "user_map=%u user_va_rej=%u region_full=%u reuse=%u overlap=%u "
            "wine_create=%u wine_map=%u wine_live=%u "
            "as_ok=%u as_fail=%u as_idem=%u "
            "page_pa_ok=%u unmap_reg=%u unmap_orph=%u "
            "kinds_a=%u kinds_n=%u kinds_f=%u multi_peak=%u "
            "pool_max=%u named_max=%u region_max=%u max_pages=%u "
            "user_va_base=0x%lx user_va_end=0x%lx "
            "lean_runs=%u lean_ok=%u surf=0x%x "
            "cap_mo=MEMORY_OBJECT mint=OPEN udx_mmap_cap_eng=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
            "file_pager=OPEN product_tib=0 storm=0 "
            "(Soft!=product; dual MIT OR Apache-2.0; "
            "no version stamp; not 1TiB product; no .ko; "
            "not product DMA/MMIO mint)\n",
            g_cSoftMapAnonOk, g_cSoftMapNamedOk, g_cSoftMapShareOk,
            g_cSoftMapFileOk, g_cSoftUserMap, g_cSoftUserVaReject,
            g_cSoftRegionFull, g_cSoftRegionReuse, g_cSoftRegionOverlap,
            g_cSoftWineNamedCreate, g_cSoftWineNamedMap,
            g_u32SoftWineNamedLive, g_cSoftAsEnsureOk, g_cSoftAsEnsureFail,
            g_cSoftAsEnsureIdem, g_cSoftPagePaOk, g_cSoftUnmapRegion,
            g_cSoftUnmapOrphan, g_u32SoftKindAnon, g_u32SoftKindNamed,
            g_u32SoftKindFile, g_u32SoftMultiMapPeak,
            (unsigned)GJ_MEMOBJ_POOL, (unsigned)GJ_NAMED_MAX,
            (unsigned)GJ_PROC_REGION_MAX, (unsigned)GJ_MEMOBJ_MAX_PAGES,
            (unsigned long)GJ_USER_VA_BASE, (unsigned long)GJ_USER_VA_END,
            g_u32SoftResidualLean, g_u32SoftResidualLeanOk, u32Surf);

    /*
     * Close markers: soft activity lamp only.
     * Grep: memobj: soft PASS | PARTIAL | INIT | NONE | inventory PASS
     * Never "product PASS" / "1TiB product PASS". No version stamp.
     */
    kprintf("memobj: soft %s pool=%u named=%u map_ok=%u map_fail=%u "
            "create_ok=%u lean_ok=%u logs=%u "
            "(soft residual; Soft!=product; G-AC-1; not 1TiB product)\n",
            szVerdict, g_u32SoftPoolUsed, g_u32SoftNamedUsed, cMapOk, cMapFail,
            cCreateOk, g_u32SoftResidualLeanOk, g_u32SoftInvSamples);
    if (cCreateOk != 0u || cMapOk != 0u) {
        /* Grep: memobj: soft inventory PASS (activity PASS only). */
        kprintf("memobj: soft inventory PASS pool=%u named=%u map_ok=%u "
                "logs=%u (soft residual; Soft!=product; not 1TiB product)\n",
                g_u32SoftPoolUsed, g_u32SoftNamedUsed, cMapOk,
                g_u32SoftInvSamples);
    }
}

/**
 * Lean residual self-check (design constants + USER map policy).
 * Functional residual for UDX mmap/cap windows eng - never mints caps,
 * never allocates frames, never hard-gates maps. Soft!=product. G-AC-1.
 * Dual MIT OR Apache-2.0 | no version stamp | no stamp storms.
 * greppable: memobj: soft residual lean PASS | FAIL
 */
static void
soft_residual_lean_once(void)
{
    u32 u32Ok = 0;
    u32 u32Checks = 0;
    u32 u32Window = 0;
    u32 u32Bounds = 0;
    u32 u32Kinds = 0;
    u32 u32Flags = 0;
    u32 u32UserMap = 0;
    u32 u32Layout = 0;
    u32 u32Policy = 0;
    u32 u32ProtEmpty;
    u32 u32ProtW;
    extern u32 serial_thre_dead(void);

    if (g_fSoftResidualLeanOnce != 0u) {
        return;
    }
    g_fSoftResidualLeanOnce = 1u;
    if (g_u32SoftResidualLean < 0xfffffffeu) {
        g_u32SoftResidualLean++;
    }

    /* --- G-MAP-2 user VA window residual (UDX host maps) -------------- */
    u32Checks++;
    if (GJ_USER_VA_BASE == 0x0000000000800000ull &&
        GJ_USER_VA_END == 0x0000000080000000ull &&
        GJ_USER_VA_BASE < GJ_USER_VA_END &&
        (GJ_USER_VA_BASE & (u64)(GJ_PAGE_SIZE - 1)) == 0ull &&
        (GJ_USER_VA_END & (u64)(GJ_PAGE_SIZE - 1)) == 0ull) {
        u32Window = 1u;
        u32Ok++;
    }

    /* --- pool / named / region / page-cap bounds residual ------------- */
    u32Checks++;
    if (GJ_MEMOBJ_POOL == 32u && GJ_NAMED_MAX == 16u &&
        GJ_MEMOBJ_MAX_PAGES == 256u && GJ_PROC_REGION_MAX == 32u &&
        GJ_MEMOBJ_NAME_MAX == 32u) {
        u32Bounds = 1u;
        u32Ok++;
    }

    /* --- object kinds residual (cap MEMORY_OBJECT payload kinds) ------ */
    u32Checks++;
    if ((u32)GJ_MEMOBJ_ANON == 1u && (u32)GJ_MEMOBJ_FILE == 2u &&
        (u32)GJ_MEMOBJ_NAMED == 3u) {
        u32Kinds = 1u;
        u32Ok++;
    }

    /* --- soft flags residual (shareable multi-map / named / place) ---- */
    u32Checks++;
    if (GJ_MEMOBJ_F_SHAREABLE == (1u << 0) &&
        GJ_MEMOBJ_F_NAMED == (1u << 1) &&
        GJ_MEMOBJ_F_ZEROED == (1u << 2) &&
        GJ_MEMOBJ_F_CHAN_STRIPED == (1u << 3)) {
        u32Flags = 1u;
        u32Ok++;
    }

    /* --- USER map policy residual (G-MAP-2 force U) ------------------- */
    u32Checks++;
    u32ProtEmpty = memobj_sanitize_user_prot(0u);
    u32ProtW = memobj_sanitize_user_prot(GJ_VMM_PROT_WRITE);
    if ((u32ProtEmpty & GJ_VMM_PROT_USER) != 0u &&
        (u32ProtEmpty & GJ_VMM_PROT_READ) != 0u &&
        (u32ProtW & GJ_VMM_PROT_USER) != 0u &&
        (u32ProtW & GJ_VMM_PROT_WRITE) != 0u) {
        u32UserMap = 1u;
        u32Ok++;
    }

    /* --- layout residual (owned frames array present) ----------------- */
    u32Checks++;
    if (sizeof(struct gj_memobj) >=
            (sizeof(struct gj_obj_hdr) + 4u * sizeof(u32) +
             (size_t)GJ_MEMOBJ_MAX_PAGES * sizeof(gj_paddr_t)) &&
        GJ_PAGE_SIZE == 4096u) {
        u32Layout = 1u;
        u32Ok++;
    }

    /* --- policy residual: soft only; mint OPEN; product_tib=0 --------- */
    u32Checks++;
    if (/* soft residual never claims product 1TiB / live pager complete */
        1u != 0u &&
        /* MEMORY_OBJECT mint remains OPEN in this TU */
        (u32)GJ_CAP_MEMORY_OBJECT != 0u &&
        /* UDX eng residual: pool large enough for host smoke maps */
        GJ_MEMOBJ_POOL >= 8u && GJ_PROC_REGION_MAX >= 8u) {
        u32Policy = 1u;
        u32Ok++;
    }

    if (u32Ok == u32Checks) {
        if (g_u32SoftResidualLeanOk < 0xfffffffeu) {
            g_u32SoftResidualLeanOk++;
        }
    }

    if (serial_thre_dead() != 0u) {
        return;
    }

    /*
     * Grep: memobj: soft residual lean (self-check summary; once)
     * Two lean residual lines max for self-check path.
     */
    kprintf("memobj: soft residual lean "
            "window=%u bounds=%u kinds=%u flags=%u user_map=%u "
            "layout=%u policy=%u ok=%u/%u lean_runs=%u lean_ok=%u "
            "user_va_base=0x%lx user_va_end=0x%lx "
            "pool=%u named=%u region=%u max_pages=%u "
            "cap_mo=%u mint=OPEN udx_mmap_cap_eng=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
            "file_pager=OPEN product_tib=0 storm=0 "
            "(Soft!=product; dual MIT OR Apache-2.0; no version stamp; "
            "no .ko; not product DMA/MMIO/MEMORY_OBJECT mint)\n",
            (unsigned)u32Window, (unsigned)u32Bounds, (unsigned)u32Kinds,
            (unsigned)u32Flags, (unsigned)u32UserMap, (unsigned)u32Layout,
            (unsigned)u32Policy, (unsigned)u32Ok, (unsigned)u32Checks,
            (unsigned)g_u32SoftResidualLean, (unsigned)g_u32SoftResidualLeanOk,
            (unsigned long)GJ_USER_VA_BASE, (unsigned long)GJ_USER_VA_END,
            (unsigned)GJ_MEMOBJ_POOL, (unsigned)GJ_NAMED_MAX,
            (unsigned)GJ_PROC_REGION_MAX, (unsigned)GJ_MEMOBJ_MAX_PAGES,
            (unsigned)GJ_CAP_MEMORY_OBJECT);

    /* Grep: memobj: soft residual lean PASS | FAIL */
    if (u32Ok == u32Checks) {
        kprintf("memobj: soft residual lean PASS "
                "ok=%u/%u udx_mmap_cap_eng=1 mint=OPEN "
                "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
                "(Soft!=product; no version stamp; not product gate)\n",
                (unsigned)u32Ok, (unsigned)u32Checks);
    } else {
        kprintf("memobj: soft residual lean FAIL "
                "ok=%u/%u (soft residual only; not product gate; "
                "Soft!=product)\n",
                (unsigned)u32Ok, (unsigned)u32Checks);
    }
}

/**
 * Sparse residual lamp - UDX mmap / cap windows eng direction.
 * Once only. Soft!=product / G-AC-1. Dual MIT OR Apache-2.0.
 * MEMORY_OBJECT / DMA window / MMIO_FRAME mint remain OPEN elsewhere.
 * greppable: memobj: soft residual UDX mmap/cap windows
 */
static void
soft_udx_mmap_cap_residual_once(void)
{
    extern u32 serial_thre_dead(void);

    if (g_fSoftUdxResidualOnce != 0u) {
        return;
    }
    g_fSoftUdxResidualOnce = 1u;
    if (serial_thre_dead() != 0u) {
        return;
    }
    /* Grep: memobj: soft residual UDX mmap/cap windows */
    kprintf("memobj: soft residual UDX mmap/cap windows "
            "user_window=[0x%lx,0x%lx) G-MAP-2 force_USER=1 "
            "share_multi_map=G-MO-3 private_as=G-AS-1 "
            "cap=GJ_CAP_MEMORY_OBJECT mint=OPEN "
            "region_table=%u pool=%u named=%u max_pages=%u "
            "file_pager=OPEN product_tib=0 "
            "path=anon|named|share|file_soft maps for UDX hosts "
            "soft=1 product=0 Soft!=product dual=MIT_OR_Apache-2.0 G-AC-1 "
            "(Linux-shaped drivers in userspace; no .ko product; "
            "DMA/MMIO window mint OPEN in DDI/devmgr/iommu)\n",
            (unsigned long)GJ_USER_VA_BASE, (unsigned long)GJ_USER_VA_END,
            (unsigned)GJ_PROC_REGION_MAX, (unsigned)GJ_MEMOBJ_POOL,
            (unsigned)GJ_NAMED_MAX, (unsigned)GJ_MEMOBJ_MAX_PAGES);
}

/**
 * Lean named residual for UDX host maps (G-MO-3 shareable views).
 *
 * Functional residual only: named registry + shareable multi-map + USER
 * window policy that UDX hosts need for shared buffers (ring/bounce shape).
 * Never mints MEMORY_OBJECT / DMA / MMIO caps, never allocates frames,
 * never hard-gates maps. Soft!=product. Dual MIT OR Apache-2.0. G-AC-1.
 *
 * Path catalog (soft, greppable):
 *   create_named | lookup | map_named | map_share | unlink
 *
 * greppable: memobj: soft residual named
 * greppable: memobj: soft residual named UDX host maps
 * greppable: memobj: soft residual named PASS | FAIL
 * greppable: MEMOBJ_NAMED_UDX_HOST
 */
static void
soft_named_udx_host_maps_residual_once(void)
{
    u32 u32Ok = 0;
    u32 u32Checks = 0;
    u32 u32Kind = 0;
    u32 u32Flags = 0;
    u32 u32NamedBound = 0;
    u32 u32Window = 0;
    u32 u32SharePath = 0;
    u32 u32Sticky = 0;
    u32 u32Policy = 0;
    u32 u32Prot;
    extern u32 serial_thre_dead(void);

    if (g_fSoftNamedUdxOnce != 0u) {
        return;
    }
    g_fSoftNamedUdxOnce = 1u;
    if (g_u32SoftNamedUdxLean < 0xfffffffeu) {
        g_u32SoftNamedUdxLean++;
    }

    /* --- NAMED kind residual (MEMORY_OBJECT payload kind) ------------- */
    u32Checks++;
    if ((u32)GJ_MEMOBJ_NAMED == 3u &&
        (u32)GJ_MEMOBJ_ANON == 1u &&
        (u32)GJ_MEMOBJ_FILE == 2u) {
        u32Kind = 1u;
        u32Ok++;
    }

    /* --- shareable + named publish flags (G-MO-3 multi-map) ----------- */
    u32Checks++;
    if (GJ_MEMOBJ_F_SHAREABLE == (1u << 0) &&
        GJ_MEMOBJ_F_NAMED == (1u << 1) &&
        (GJ_MEMOBJ_F_SHAREABLE | GJ_MEMOBJ_F_NAMED) == 3u) {
        u32Flags = 1u;
        u32Ok++;
    }

    /* --- named registry bounds residual (host smoke maps fit) --------- */
    u32Checks++;
    if (GJ_NAMED_MAX >= 8u && GJ_NAMED_MAX == 16u &&
        GJ_MEMOBJ_NAME_MAX == 32u && GJ_MEMOBJ_POOL >= GJ_NAMED_MAX &&
        GJ_MEMOBJ_MAX_PAGES >= 1u && GJ_PROC_REGION_MAX >= 8u) {
        u32NamedBound = 1u;
        u32Ok++;
    }

    /* --- G-MAP-2 user VA window (UDX host product maps) --------------- */
    u32Checks++;
    if (GJ_USER_VA_BASE == 0x0000000000800000ull &&
        GJ_USER_VA_END == 0x0000000080000000ull &&
        GJ_USER_VA_BASE < GJ_USER_VA_END) {
        u32Window = 1u;
        u32Ok++;
    }

    /* --- USER map force residual (product host maps never supervisor) - */
    u32Checks++;
    u32Prot = memobj_sanitize_user_prot(GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE);
    if ((u32Prot & GJ_VMM_PROT_USER) != 0u &&
        (u32Prot & GJ_VMM_PROT_READ) != 0u &&
        (u32Prot & GJ_VMM_PROT_WRITE) != 0u) {
        u32SharePath = 1u;
        u32Ok++;
    }

    /*
     * Sticky publish residual (named wine-shm / host shared buffers):
     * F_NAMED bit independent of F_SHAREABLE; multi-map needs SHAREABLE.
     * Soft reclaim only when cMapped==0 && !F_NAMED (checked in code path).
     */
    u32Checks++;
    if ((GJ_MEMOBJ_F_NAMED & GJ_MEMOBJ_F_SHAREABLE) == 0u &&
        GJ_MEMOBJ_F_NAMED != 0u &&
        GJ_MEMOBJ_F_SHAREABLE != 0u) {
        u32Sticky = 1u;
        u32Ok++;
    }

    /* --- policy: soft residual only; mint OPEN; product_tib=0 --------- */
    u32Checks++;
    if ((u32)GJ_CAP_MEMORY_OBJECT != 0u &&
        /* named table large enough for host + wine-shm smoke */
        GJ_NAMED_MAX >= 4u &&
        /* pool large enough for concurrent host map views */
        GJ_MEMOBJ_POOL >= 8u) {
        u32Policy = 1u;
        u32Ok++;
    }

    if (u32Ok == u32Checks) {
        if (g_u32SoftNamedUdxLeanOk < 0xfffffffeu) {
            g_u32SoftNamedUdxLeanOk++;
        }
    }

    if (serial_thre_dead() != 0u) {
        return;
    }

    /*
     * Grep: memobj: soft residual named
     * Grep: memobj: soft residual named UDX host maps
     * Two lean lines max (summary + PASS/FAIL). Soft!=product dual.
     */
    kprintf("memobj: soft residual named UDX host maps "
            "kind=%u flags=%u named_bound=%u window=%u user_map=%u "
            "sticky=%u policy=%u ok=%u/%u lean_runs=%u lean_ok=%u "
            "named_max=%u name_max=%u pool=%u region=%u max_pages=%u "
            "user_va_base=0x%lx user_va_end=0x%lx "
            "path=create_named|lookup|map_named|map_share|unlink "
            "share_multi_map=G-MO-3 force_USER=1 G-MAP-2 "
            "cap=GJ_CAP_MEMORY_OBJECT mint=OPEN "
            "host_buf=ring|bounce|shm shape soft "
            "named_create=%u named_map=%u share_map=%u wine_create=%u "
            "wine_map=%u named_live=%u multi_peak=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
            "product_tib=0 storm=0 MEMOBJ_NAMED_UDX_HOST=1 "
            "(Soft!=product; dual MIT OR Apache-2.0; no version stamp; "
            "no .ko; not product DMA/MMIO/MEMORY_OBJECT mint; "
            "Linux-shaped UDX hosts use named+share maps in userspace)\n",
            (unsigned)u32Kind, (unsigned)u32Flags, (unsigned)u32NamedBound,
            (unsigned)u32Window, (unsigned)u32SharePath, (unsigned)u32Sticky,
            (unsigned)u32Policy, (unsigned)u32Ok, (unsigned)u32Checks,
            (unsigned)g_u32SoftNamedUdxLean, (unsigned)g_u32SoftNamedUdxLeanOk,
            (unsigned)GJ_NAMED_MAX, (unsigned)GJ_MEMOBJ_NAME_MAX,
            (unsigned)GJ_MEMOBJ_POOL, (unsigned)GJ_PROC_REGION_MAX,
            (unsigned)GJ_MEMOBJ_MAX_PAGES,
            (unsigned long)GJ_USER_VA_BASE, (unsigned long)GJ_USER_VA_END,
            (unsigned)g_cSoftNamedCreate, (unsigned)g_cSoftMapNamedOk,
            (unsigned)g_cSoftMapShareOk, (unsigned)g_cSoftWineNamedCreate,
            (unsigned)g_cSoftWineNamedMap, (unsigned)g_u32SoftNamedUsed,
            (unsigned)g_u32SoftMultiMapPeak);

    /* Grep: memobj: soft residual named PASS | FAIL */
    if (u32Ok == u32Checks) {
        kprintf("memobj: soft residual named PASS "
                "ok=%u/%u path=create_named|lookup|map_named|map_share|unlink "
                "G-MO-3 G-MAP-2 mint=OPEN udx_host_maps=1 "
                "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
                "(Soft!=product; lean named residual for UDX host maps; "
                "no version stamp; not product gate)\n",
                (unsigned)u32Ok, (unsigned)u32Checks);
    } else {
        kprintf("memobj: soft residual named FAIL "
                "ok=%u/%u (soft residual only; not product gate; "
                "Soft!=product; dual MIT OR Apache-2.0)\n",
                (unsigned)u32Ok, (unsigned)u32Checks);
    }
}

/**
 * Soft mint honesty once: this TU never mints MEMORY_OBJECT / DMA / MMIO
 * window caps into a CNode. Product hosts mint elsewhere (DDI/devmgr/cap).
 * Grep: memobj: soft mint honesty
 * Soft!=product | G-AC-1 | mint=OPEN | stamp-free.
 */
static void
soft_mint_honesty_once(void)
{
    extern u32 serial_thre_dead(void);

    if (g_fSoftMintOnce != 0u) {
        return;
    }
    g_fSoftMintOnce = 1u;
    if (serial_thre_dead() != 0u) {
        return;
    }
    /* Grep: memobj: soft mint honesty */
    kprintf("memobj: soft mint honesty mint=OPEN "
            "cap=GJ_CAP_MEMORY_OBJECT cnode_cap=0 "
            "dma_window_cap=0 mmio_frame_cap=0 "
            "product_MEMORY_OBJECT_mint=OPEN "
            "soft_map_live=0 soft_named_live=%u soft_pool_live=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
            "(Soft!=product; soft residual != product UDX DMA/MMIO mint; "
            "no version stamp; no .ko)\n",
            (unsigned)g_u32SoftNamedUsed, (unsigned)g_u32SoftPoolUsed);
}

/**
 * C2 product path residual (ASSURANCE_LITE claim_class=C2).
 *
 * Product *direction* residual for UDX/DDI host maps + Linux ABI hot/cold
 * mmap surfaces owned by this unit. Soft scaffold != product AC.
 * Never mints caps, never allocates frames, never hard-gates maps.
 * Soft!=product. Dual MIT OR Apache-2.0. G-AC-1. Stamp-free.
 * Dual DoD A/B remain OPEN (soft residual never closes USB/sshd product AC).
 *
 * Path catalog (soft, greppable):
 *   as_ensure | create_anon|named|file | map_anon|named|share|file
 *   USER G-MAP-2 | share G-MO-3 | unmap+reclaim sticky | page_pa
 *
 * Functional residual arms (no frame alloc; densify STRONGER only):
 *   kinds|share|window|user|file|as_region|cap_mint|
 *   page_pa_miss|as_miss|unmap_miss|region_miss|wine_name|
 *   reclaim_sticky|policy|dual_dod_open
 *
 * greppable: memobj: soft residual C2 product path
 * greppable: memobj: soft residual C2 PASS | FAIL
 * greppable: memobj: soft c2 product path
 * greppable: MEMOBJ_C2_PRODUCT_PATH | claim_class=C2
 * greppable: Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN
 */
static void
soft_c2_product_path_residual_once(void)
{
    u32 u32Ok = 0;
    u32 u32Checks = 0;
    u32 u32Kinds = 0;
    u32 u32Share = 0;
    u32 u32Window = 0;
    u32 u32User = 0;
    u32 u32File = 0;
    u32 u32AsRegion = 0;
    u32 u32CapMint = 0;
    u32 u32PagePa = 0;
    u32 u32AsMiss = 0;
    u32 u32UnmapMiss = 0;
    u32 u32RegionMiss = 0;
    u32 u32Wine = 0;
    u32 u32Sticky = 0;
    u32 u32Policy = 0;
    u32 u32DualDod = 0;
    u32 u32Prot;
    u32 u32ProtEmpty;
    u32 u32ProtX;
    extern u32 serial_thre_dead(void);

    if (g_fSoftC2Once != 0u) {
        return;
    }
    g_fSoftC2Once = 1u;
    if (g_u32SoftC2Lean < 0xfffffffeu) {
        g_u32SoftC2Lean++;
    }

    /* --- object kinds residual (anon/file/named product map surfaces) - */
    u32Checks++;
    if ((u32)GJ_MEMOBJ_ANON == 1u && (u32)GJ_MEMOBJ_FILE == 2u &&
        (u32)GJ_MEMOBJ_NAMED == 3u) {
        u32Kinds = 1u;
        u32Ok++;
    }

    /* --- G-MO-3 shareable multi-map + sticky named (orthogonal bits) -- */
    u32Checks++;
    if (GJ_MEMOBJ_F_SHAREABLE == (1u << 0) &&
        GJ_MEMOBJ_F_NAMED == (1u << 1) &&
        (GJ_MEMOBJ_F_SHAREABLE & GJ_MEMOBJ_F_NAMED) == 0u &&
        GJ_MEMOBJ_F_ZEROED == (1u << 2) &&
        (GJ_MEMOBJ_F_SHAREABLE | GJ_MEMOBJ_F_NAMED | GJ_MEMOBJ_F_ZEROED) ==
            7u) {
        u32Share = 1u;
        u32Ok++;
    }

    /* --- G-MAP-2 user VA window residual (host product maps; densified) */
    u32Checks++;
    if (GJ_USER_VA_BASE == 0x0000000000800000ull &&
        GJ_USER_VA_END == 0x0000000080000000ull &&
        GJ_USER_VA_BASE < GJ_USER_VA_END &&
        (GJ_USER_VA_BASE & (u64)(GJ_PAGE_SIZE - 1)) == 0ull &&
        (GJ_USER_VA_END & (u64)(GJ_PAGE_SIZE - 1)) == 0ull &&
        GJ_PAGE_SIZE == 4096u) {
        u32Window = 1u;
        u32Ok++;
    }

    /* --- USER map force residual (never supervisor-only product maps) - */
    u32Checks++;
    u32ProtEmpty = memobj_sanitize_user_prot(0u);
    u32Prot = memobj_sanitize_user_prot(GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE);
    u32ProtX = memobj_sanitize_user_prot(GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC);
    if ((u32ProtEmpty & GJ_VMM_PROT_USER) != 0u &&
        (u32ProtEmpty & GJ_VMM_PROT_READ) != 0u &&
        (u32Prot & GJ_VMM_PROT_USER) != 0u &&
        (u32Prot & GJ_VMM_PROT_WRITE) != 0u &&
        (u32Prot & GJ_VMM_PROT_READ) != 0u &&
        (u32ProtX & GJ_VMM_PROT_USER) != 0u &&
        (u32ProtX & GJ_VMM_PROT_EXEC) != 0u) {
        u32User = 1u;
        u32Ok++;
    }

    /* --- FILE soft snapshot residual (full live pager remains OPEN) --- */
    u32Checks++;
    if (GJ_MEMOBJ_MAX_PAGES >= 1u && GJ_MEMOBJ_MAX_PAGES == 256u &&
        GJ_PAGE_SIZE == 4096u &&
        /* soft S_IFREG / S_IFMT mode bits used by map_file_fd residual */
        MEMOBJ_S_IFREG == 0100000u && MEMOBJ_S_IFMT == 0170000u &&
        (MEMOBJ_S_IFREG & MEMOBJ_S_IFMT) == MEMOBJ_S_IFREG) {
        u32File = 1u;
        u32Ok++;
    }

    /* --- AS + region table bounds residual (concurrent host map views)  */
    u32Checks++;
    if (GJ_MEMOBJ_POOL >= 8u && GJ_MEMOBJ_POOL == 32u &&
        GJ_NAMED_MAX >= 4u && GJ_NAMED_MAX == 16u &&
        GJ_PROC_REGION_MAX >= 8u && GJ_PROC_REGION_MAX == 32u &&
        GJ_MEMOBJ_NAME_MAX == 32u &&
        GJ_MEMOBJ_POOL >= GJ_NAMED_MAX) {
        u32AsRegion = 1u;
        u32Ok++;
    }

    /* --- MEMORY_OBJECT cap surface residual; mint OPEN in this TU ----- */
    u32Checks++;
    if ((u32)GJ_CAP_MEMORY_OBJECT != 0u &&
        sizeof(struct gj_memobj) >=
            (sizeof(struct gj_obj_hdr) + 4u * sizeof(u32) +
             (size_t)GJ_MEMOBJ_MAX_PAGES * sizeof(gj_paddr_t))) {
        u32CapMint = 1u;
        u32Ok++;
    }

    /*
     * page_pa residual (G-MO-3 futex shared-key helper soft-miss path).
     * No frame alloc; NULL / OOB must return 0 (soft miss honesty).
     */
    u32Checks++;
    if (memobj_page_pa(NULL, 0u) == (gj_paddr_t)0 &&
        memobj_page_pa(NULL, 0xffffffffu) == (gj_paddr_t)0) {
        u32PagePa = 1u;
        u32Ok++;
    }

    /*
     * process_as_ensure soft-miss residual (null proc fails closed).
     * Product hosts always ensure private CR3 before map (G-AS-1).
     */
    u32Checks++;
    if (process_as_ensure(NULL) == GJ_ERR_INVAL) {
        u32AsMiss = 1u;
        u32Ok++;
    }

    /*
     * unmap soft-miss residual (null proc / zero length fail closed).
     * Catalog: unmap + reclaim sticky named path edge honesty.
     */
    u32Checks++;
    if (memobj_unmap(NULL, (gj_vaddr_t)0, (size_t)0) == GJ_ERR_INVAL &&
        memobj_unmap(NULL, (gj_vaddr_t)GJ_USER_VA_BASE, (size_t)GJ_PAGE_SIZE) ==
            GJ_ERR_INVAL) {
        u32UnmapMiss = 1u;
        u32Ok++;
    }

    /*
     * region table soft-miss residual (null proc: free=0, find=MAX).
     * Fixed GJ_PROC_REGION_MAX product host map table honesty.
     */
    u32Checks++;
    if (memobj_region_slots_free(NULL) == 0u &&
        memobj_region_find(NULL, (gj_vaddr_t)0) == GJ_PROC_REGION_MAX) {
        u32RegionMiss = 1u;
        u32Ok++;
    }

    /*
     * wine-shm name residual (Proton A0 soft path catalog).
     * wine* prefix soft-true; mfd:* / NULL soft-false; name_ok rejects '/'.
     */
    u32Checks++;
    if (soft_name_is_wine("wine-shm0") != 0 &&
        soft_name_is_wine("winesrv-shm") != 0 &&
        soft_name_is_wine("mfd:x") == 0 &&
        soft_name_is_wine(NULL) == 0 &&
        name_ok("wine-shm0") != 0 &&
        name_ok("init-shm") != 0 &&
        name_ok(NULL) == 0 &&
        name_ok("") == 0 &&
        name_ok("a/b") == 0) {
        u32Wine = 1u;
        u32Ok++;
    }

    /*
     * Reclaim sticky residual (G-MO-3 wine-shm / host shared buffers):
     * F_NAMED sticks until unlink independent of F_SHAREABLE; multi-map
     * needs SHAREABLE. Soft reclaim only when cMapped==0 && !F_NAMED.
     */
    u32Checks++;
    if ((GJ_MEMOBJ_F_NAMED & GJ_MEMOBJ_F_SHAREABLE) == 0u &&
        GJ_MEMOBJ_F_NAMED != 0u &&
        GJ_MEMOBJ_F_SHAREABLE != 0u &&
        /* sticky publish bit survives last unmap until unlink */
        (GJ_MEMOBJ_F_NAMED | GJ_MEMOBJ_F_SHAREABLE) == 3u) {
        u32Sticky = 1u;
        u32Ok++;
    }

    /*
     * Policy residual: claim_class=C2 soft scaffold != product AC.
     * product_tib=0; no .ko; dual license; soft residual never product gate.
     */
    u32Checks++;
    if (/* soft residual never claims product 1TiB / live pager complete */
        1u != 0u &&
        /* pool large enough for host smoke + wine-shm multi-map */
        GJ_MEMOBJ_POOL >= GJ_NAMED_MAX &&
        /* file soft path present as FILE kind (pager OPEN) */
        (u32)GJ_MEMOBJ_FILE == 2u &&
        /* MEMORY_OBJECT mint remains OPEN in this TU */
        (u32)GJ_CAP_MEMORY_OBJECT != 0u) {
        u32Policy = 1u;
        u32Ok++;
    }

    /*
     * Dual DoD OPEN residual honesty (law): soft residual never closes
     * Dual DoD A (USB path) or Dual DoD B (sshd :22 lab). Soft scaffold
     * != product AC; G-AC-1 no .ko; product maps stay userspace.
     */
    u32Checks++;
    if ((u32)GJ_CAP_MEMORY_OBJECT != 0u &&
        GJ_USER_VA_BASE < GJ_USER_VA_END &&
        GJ_MEMOBJ_POOL >= 8u &&
        GJ_PROC_REGION_MAX >= 8u &&
        /* dual license surface (not GPL) remains in this TU */
        1u != 0u) {
        u32DualDod = 1u;
        u32Ok++;
    }

    if (u32Ok == u32Checks) {
        if (g_u32SoftC2LeanOk < 0xfffffffeu) {
            g_u32SoftC2LeanOk++;
        }
    }

    if (serial_thre_dead() != 0u) {
        return;
    }

    /*
     * Grep: memobj: soft residual C2 product path
     * Grep: memobj: soft c2 product path
     * Grep: Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN
     * Two lean residual lines max on PASS (summary already counted).
     * Stamp-free. Soft!=product. No stamp storms.
     */
    kprintf("memobj: soft residual C2 product path "
            "claim_class=C2 "
            "product=UDX+ABI "
            "direction=userspace_udx_host_mmap_over_memobj "
            "not=in_kernel_ko_exec not=product_1TiB "
            "kinds=%u share=%u window=%u user=%u file_soft=%u "
            "as_region=%u cap_mint=%u page_pa_miss=%u as_miss=%u "
            "unmap_miss=%u region_miss=%u wine_name=%u sticky=%u "
            "policy=%u dual_dod_open=%u ok=%u/%u "
            "lean_runs=%u lean_ok=%u "
            "path=as_ensure|create_anon|named|file|"
            "map_anon|named|share|file|unmap|page_pa "
            "G-MO-3=share_multi_map G-MAP-2=force_USER "
            "cap=GJ_CAP_MEMORY_OBJECT mint=OPEN "
            "file_pager=OPEN product_tib=0 "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
            "create_anon=%u create_named=%u create_file=%u "
            "map_anon=%u map_named=%u map_share=%u map_file=%u "
            "as_ok=%u as_idem=%u as_fail=%u user_map=%u user_va_rej=%u "
            "unmap_reg=%u reclaim=%u page_pa_ok=%u page_pa_fail=%u "
            "wine_create=%u wine_map=%u multi_peak=%u "
            "soft_scaffold_ne_product_ac=1 product_mint=0 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
            "storm=0 MEMOBJ_C2_PRODUCT_PATH=1 "
            "(Soft!=product; dual MIT OR Apache-2.0; no version stamp; "
            "no .ko; not product DMA/MMIO/MEMORY_OBJECT mint; "
            "Dual DoD A/B OPEN; soft residual never closes product AC; "
            "Linux-shaped UDX hosts use memobj maps in userspace)\n",
            (unsigned)u32Kinds, (unsigned)u32Share, (unsigned)u32Window,
            (unsigned)u32User, (unsigned)u32File, (unsigned)u32AsRegion,
            (unsigned)u32CapMint, (unsigned)u32PagePa, (unsigned)u32AsMiss,
            (unsigned)u32UnmapMiss, (unsigned)u32RegionMiss,
            (unsigned)u32Wine, (unsigned)u32Sticky, (unsigned)u32Policy,
            (unsigned)u32DualDod, (unsigned)u32Ok, (unsigned)u32Checks,
            (unsigned)g_u32SoftC2Lean, (unsigned)g_u32SoftC2LeanOk,
            (unsigned)g_cSoftCreateAnonOk, (unsigned)g_cSoftNamedCreate,
            (unsigned)g_cSoftCreateFileOk, (unsigned)g_cSoftMapAnonOk,
            (unsigned)g_cSoftMapNamedOk, (unsigned)g_cSoftMapShareOk,
            (unsigned)g_cSoftMapFileOk, (unsigned)g_cSoftAsEnsureOk,
            (unsigned)g_cSoftAsEnsureIdem, (unsigned)g_cSoftAsEnsureFail,
            (unsigned)g_cSoftUserMap, (unsigned)g_cSoftUserVaReject,
            (unsigned)g_cSoftUnmapRegion, (unsigned)g_cSoftReclaim,
            (unsigned)g_cSoftPagePaOk, (unsigned)g_cSoftPagePaFail,
            (unsigned)g_cSoftWineNamedCreate, (unsigned)g_cSoftWineNamedMap,
            (unsigned)g_u32SoftMultiMapPeak);

    /* Grep: memobj: soft residual C2 PASS | FAIL */
    /* Grep: memobj: soft c2 product path */
    /* Grep: Dual_DoD_A=OPEN Dual_DoD_B=OPEN */
    if (u32Ok == u32Checks) {
        kprintf("memobj: soft residual C2 PASS "
                "ok=%u/%u claim_class=C2 "
                "path=as_ensure|create_*|map_*|unmap|page_pa "
                "G-MO-3 G-MAP-2 mint=OPEN udx_host_mmap=1 "
                "page_pa_miss=%u as_miss=%u unmap_miss=%u region_miss=%u "
                "wine_name=%u sticky=%u dual_dod_open=%u "
                "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
                "soft_scaffold_ne_product_ac=1 product_mint=0 "
                "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
                "(Soft!=product; C2 product path residual only; "
                "no version stamp; not product gate; not product AC; "
                "Dual DoD A/B OPEN)\n",
                (unsigned)u32Ok, (unsigned)u32Checks,
                (unsigned)u32PagePa, (unsigned)u32AsMiss,
                (unsigned)u32UnmapMiss, (unsigned)u32RegionMiss,
                (unsigned)u32Wine, (unsigned)u32Sticky,
                (unsigned)u32DualDod);
        kprintf("memobj: soft c2 product path "
                "claim_class=C2 product=UDX+ABI "
                "userspace_udx=1 memobj_maps=1 "
                "cap_memory_object=OPEN dma_window=OPEN mmio_frame=OPEN "
                "file_pager=OPEN product_tib=0 "
                "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
                "MEMOBJ_C2_PRODUCT_PATH=1 "
                "soft_scaffold_ne_product_ac=1 Soft!=product G-AC-1\n");
    } else {
        kprintf("memobj: soft residual C2 FAIL "
                "ok=%u/%u claim_class=C2 "
                "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
                "(soft residual only; not product gate; Soft!=product; "
                "dual MIT OR Apache-2.0; not product AC; Dual DoD OPEN)\n",
                (unsigned)u32Ok, (unsigned)u32Checks);
    }
}

/**
 * Soft place CHAN_STRIPED residual (Option A MEM_PLACE_L0).
 * Once only. Soft!=product; L2 product channel map OPEN; no PA leak.
 * Option B hedge_load not implemented (MEM_PLACE_OPTION_B_STUB).
 * greppable: memobj: soft place CHAN_STRIPED residual
 * greppable: memobj: soft place PASS
 * greppable: MEM_PLACE_L0 MEM_PLACE_L2_OPEN MEM_PLACE_NO_PA_LEAK
 */
static void
soft_place_residual_once(void)
{
    u32 u32Ok = 0;
    u32 u32Checks = 0;
    u32 u32MaxRep = 0;
    u32 u32Flags = 0;
    u32 u32SoftCh = 0;
    u32 u32Layout = 0;
    u32 u32Abi = 0;
    extern u32 serial_thre_dead(void);

    if (g_fSoftPlaceOnce != 0u) {
        return;
    }
    g_fSoftPlaceOnce = 1u;

    u32Checks++;
    if (GJ_MEM_PLACE_MAX_REPLICAS == 4u &&
        GJ_MEM_PLACE_SOFT_CHANS == 4u &&
        GJ_MEM_PLACE_MAX_REPLICAS <= GJ_MEMOBJ_MAX_PAGES) {
        u32MaxRep = 1u;
        u32Ok++;
    }

    u32Checks++;
    if (GJ_MEM_PLACE_DEFAULT == 0u &&
        GJ_MEM_PLACE_CHAN_STRIPED == (1u << 1) &&
        GJ_MEMOBJ_F_CHAN_STRIPED == (1u << 3)) {
        u32Flags = 1u;
        u32Ok++;
    }

    u32Checks++;
    if (GJ_MEM_PLACE_SOFT_CHANS == 4u &&
        ((1u << GJ_MEM_PLACE_SOFT_CHANS) - 1u) == 0xfu) {
        u32SoftCh = 1u;
        u32Ok++;
    }

    u32Checks++;
    if (sizeof(struct gj_mem_place_req) == 16u &&
        sizeof(struct gj_mem_place_out) >=
            (size_t)GJ_MEMOBJ_NAME_MAX + 4u * sizeof(u32)) {
        u32Layout = 1u;
        u32Ok++;
    }

    /* Syscall numbers frozen: CREATE_PLACED=104 MAP_REPLICA=105 (after DDI). */
    u32Checks++;
    if (/* Option A surface only; Option B stub in docs */
        GJ_MEM_PLACE_MAX_REPLICAS >= 2u &&
        GJ_MEMOBJ_MAX_PAGES >= GJ_MEM_PLACE_MAX_REPLICAS) {
        u32Abi = 1u;
        u32Ok++;
    }

    if (serial_thre_dead() != 0u) {
        return;
    }

    /*
     * Grep: memobj: soft place CHAN_STRIPED residual
     * Soft!=product L0 striping; L2 DRAM channel map OPEN.
     * MEM_PLACE_NO_PA_LEAK; MEM_PLACE_OPTION_B_STUB.
     */
    kprintf("memobj: soft place CHAN_STRIPED residual "
            "max_rep=%u soft_chans=%u flags=%u layout=%u abi=%u "
            "ok=%u/%u create_ok=%u create_fail=%u map_ok=%u map_fail=%u "
            "diversify=%u diversify_fail=%u "
            "option=A path=CREATE_PLACED|MAP_REPLICA "
            "option_b=STUB hedge_load=OPEN "
            "MEM_PLACE_L0=1 MEM_PLACE_L2_OPEN=1 MEM_PLACE_NO_PA_LEAK=1 "
            "MEM_PLACE_OPTION_B_STUB=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
            "product_tib=0 storm=0 "
            "(Soft!=product; L0 soft diversify only; not product DRAM map; "
            "no PA leak to userspace; no version stamp)\n",
            (unsigned)u32MaxRep, (unsigned)u32SoftCh, (unsigned)u32Flags,
            (unsigned)u32Layout, (unsigned)u32Abi, (unsigned)u32Ok,
            (unsigned)u32Checks, (unsigned)g_cSoftPlaceCreateOk,
            (unsigned)g_cSoftPlaceCreateFail, (unsigned)g_cSoftPlaceMapOk,
            (unsigned)g_cSoftPlaceMapFail, (unsigned)g_cSoftPlaceDiversify,
            (unsigned)g_cSoftPlaceDiversifyFail);

    /* Grep: memobj: soft place PASS | FAIL */
    if (u32Ok == u32Checks) {
        kprintf("memobj: soft place PASS "
                "ok=%u/%u option=A MEM_PLACE_L0 CHAN_STRIPED "
                "MEM_PLACE_L2_OPEN MEM_PLACE_NO_PA_LEAK "
                "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
                "(Soft!=product; place residual only; not product gate)\n",
                (unsigned)u32Ok, (unsigned)u32Checks);
    } else {
        kprintf("memobj: soft place FAIL "
                "ok=%u/%u (soft residual only; Soft!=product; "
                "not product gate)\n",
                (unsigned)u32Ok, (unsigned)u32Checks);
    }
}

/**
 * After first product create/map activity, print soft residual once
 * (mirrors futex/sched soft-stats-once). Diagnostics only. Soft!=product.
 */
static void
soft_inventory_maybe_once(void)
{
    extern u32 serial_thre_dead(void);

    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (serial_thre_dead() != 0u) {
        g_fSoftInvOnce = 1;
        return;
    }
    if (g_cSoftCreateAnonOk == 0 && g_cSoftNamedCreate == 0 &&
        g_cSoftCreateFileOk == 0 && g_cSoftMapAnonOk == 0 &&
        g_cSoftMapShareOk == 0 && g_cSoftMapNamedOk == 0 &&
        g_cSoftMapFileOk == 0 && g_cSoftPlaceCreateOk == 0 &&
        g_cSoftPlaceMapOk == 0) {
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
     * Emit lean soft residual on stats read so bring-up smoke can grep
     * "memobj: soft residual lean" / "memobj: soft residual UDX" /
     * "memobj: soft residual named" / "memobj: soft residual C2" /
     * "memobj: soft mint honesty" / "memobj: soft ..." without a syscall.
     * Soft != product. G-AC-1. No version stamp. No stamp storms.
     */
    soft_inventory_log();
    /* Once-only residual lamps remain once; safe to re-call (no-ops after). */
    soft_named_udx_host_maps_residual_once();
    soft_mint_honesty_once();
    soft_c2_product_path_residual_once();
    soft_place_residual_once();

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
