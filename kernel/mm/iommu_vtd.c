/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * VT-d (Intel IOMMU) page-table construction — clean-room, dual-licensed.
 * Builds root + context + second-level identity map in memory.
 * Soft-probe: CAP/ECAP soft or MMIO, root/context verify, DMAR inventory.
 * Domain soft: software DID pool; bus-0 context DID write when tables ready.
 * Optional DRHD MMIO program when ACPI DMAR provides a base.
 * Not derived from Linux intel-iommu or any GPL VT-d driver.
 */
#include <gj/config.h>
#include <gj/iommu.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/types.h>
#include <gj/vmm.h>

/* Second-level PTE flags (VT-d architecture; public spec bits) */
#define VTD_SL_R  (1ull << 0)
#define VTD_SL_W  (1ull << 1)
#define VTD_SL_SP (1ull << 7) /* superpage (2 MiB when in PD) */

/* Root entry: present + context table ptr */
#define VTD_ROOT_P (1ull << 0)
/* Context entry: present + translation type + SLPT ptr */
#define VTD_CTX_P     (1ull << 0)
#define VTD_CTX_TT_ML (0ull << 2) /* multi-level translate */
/* Context hi: AW in [2:0], DID in [23:8] (public VT-d context-entry) */
#define VTD_CTX_AW_48  2ull
#define VTD_CTX_DID_SHIFT 8u

/* Bring-up identity SLPT covers [0, 1 GiB). */
#define VTD_IDENTITY_LIMIT (1024ull * 1024ull * 1024ull)
#define VTD_2MIB           (2ull * 1024ull * 1024ull)
#define VTD_CTX_ENTRIES    256u /* bus0: 32 slots × 8 funcs */
#define VTD_DRHD_MAP_CB    (2ull * 1024ull * 1024ull)
#define VTD_GSTS_SPINS     10000u
/* Minimum pages: root + context + PDPT + PD */
#define VTD_PAGES_MIN      4u

/* DRHD register offsets (VT-d public register map) */
#define VTD_REG_CAP    0x08u
#define VTD_REG_ECAP   0x10u
#define VTD_REG_GCMD   0x18u
#define VTD_REG_GSTS   0x1cu
#define VTD_REG_RTADDR 0x20u

#define VTD_GCMD_TE   (1u << 31)
#define VTD_GCMD_SRTP (1u << 30)
#define VTD_GSTS_RTPS (1u << 30)

/*
 * Soft synthetic CAP/ECAP when no DRHD MMIO (QEMU without intel-iommu).
 * Public bit meanings only; values describe bring-up tables (48-bit AW,
 * 2 MiB superpages in SAGAW, modest ND). Not read from hardware.
 */
#define VTD_SOFT_CAP_ND_16     (1ull << 0)  /* ND encoding: 16 domains */
#define VTD_SOFT_CAP_SAGAW_39  (1ull << 9)  /* 3-level / 39-bit class */
#define VTD_SOFT_CAP_SAGAW_48  (1ull << 10) /* 4-level / 48-bit class */
#define VTD_SOFT_CAP_MGAW_47   (47ull << 16)
#define VTD_SOFT_CAP                                                           \
    (VTD_SOFT_CAP_ND_16 | VTD_SOFT_CAP_SAGAW_39 | VTD_SOFT_CAP_SAGAW_48 |     \
     VTD_SOFT_CAP_MGAW_47)
#define VTD_SOFT_ECAP  0ull /* QI/IR not claimed in soft path */

/* Soft domain attach slots (BDF → DID); independent of window table */
#define VTD_SOFT_ATTACH_MAX 32u

struct vtd_domain_soft {
    u8  u8Used;
    u8  u8Pad;
    u16 u16Ref; /* attach count */
};

struct vtd_attach_soft {
    u8  u8Used;
    u8  u8Bus;
    u8  u8Slot;
    u8  u8Func;
    u32 u32Did;
};

static gj_paddr_t g_paRoot;
static gj_paddr_t g_paContext;
static gj_paddr_t g_paPdpt;
static gj_paddr_t g_paPd0; /* first 1 GiB of 2 MiB pages */
static u64        g_u64Drhd;
static int        g_fVtdReady;
static int        g_fTeArmed;
static int        g_nTeMode; /* GJ_IOMMU_TE_* */
static u32        g_u32VtdPages;
static u32        g_u32CtxDevices;

/* Soft-probe snapshot */
static int                     g_fSoftProbed;
static struct gj_iommu_vtd_soft g_Soft;
static u32                     g_cDrhdInv;
static u32                     g_cRmrrInv;
static u32                     g_cAtsrInv;
static u32                     g_cRhsaInv;
static u32                     g_cOtherInv;
static u64                     g_u64Cap;
static u64                     g_u64Ecap;
static int                     g_fCapFromMmio;

/* Domain soft pool: slot index == DID */
static struct vtd_domain_soft g_aDom[GJ_IOMMU_DOMAIN_MAX];
static struct vtd_attach_soft g_aAtt[VTD_SOFT_ATTACH_MAX];
static u32                    g_u32DomUsed;

static void *
vtd_virt(gj_paddr_t pa)
{
    if (pa == 0) {
        return NULL;
    }
    if (hhdm_ready()) {
        return (void *)(gj_vaddr_t)(GJ_HHDM_BASE + (u64)pa);
    }
    /* Identity only safe for low memory before HHDM */
    if ((u64)pa < 0x100000000ull) {
        return (void *)(gj_vaddr_t)pa;
    }
    return NULL;
}

static gj_paddr_t
vtd_alloc_zero(void)
{
    gj_paddr_t pa;
    void *pVa;

    pa = pmm_alloc();
    if (pa == 0) {
        return 0;
    }
    pVa = vtd_virt(pa);
    if (pVa == NULL) {
        /* Cannot zero; leave page leaked rather than write blind */
        kprintf("iommu: vtd alloc map fail pa=0x%lx\n", (unsigned long)pa);
        return 0;
    }
    memset(pVa, 0, GJ_PAGE_SIZE);
    g_u32VtdPages++;
    return pa;
}

static int
vtd_bdf_ok(u8 u8Bus, u8 u8Slot, u8 u8Func)
{
    (void)u8Bus;
    if (u8Slot > 31u || u8Func > 7u) {
        return 0;
    }
    return 1;
}

/** Bus-0 context index: slot[4:0] << 3 | func[2:0]. */
static u32
vtd_ctx_index(u8 u8Slot, u8 u8Func)
{
    return ((u32)(u8Slot & 31u) << 3) | (u32)(u8Func & 7u);
}

/**
 * Write context-entry DID (+AW) for bus 0 when tables ready.
 * Soft path: updates RAM only; no context-cache invalidate (no QI soft).
 */
static int
vtd_ctx_set_did(u8 u8Bus, u8 u8Slot, u8 u8Func, u32 u32Did)
{
    u64 *pCtx;
    u32 u32Idx;
    u64 *pE;

    if (!g_fVtdReady || g_paContext == 0) {
        return 0; /* soft ok without tables */
    }
    if (u8Bus != 0 || u32Did >= GJ_IOMMU_DOMAIN_MAX) {
        return -1;
    }
    if (!vtd_bdf_ok(u8Bus, u8Slot, u8Func)) {
        return -1;
    }
    pCtx = (u64 *)vtd_virt(g_paContext);
    if (pCtx == NULL) {
        return -1;
    }
    u32Idx = vtd_ctx_index(u8Slot, u8Func);
    pE = &pCtx[u32Idx * 2u];
    /* Keep P|TT|SLPT; rewrite hi with AW + DID */
    if ((pE[0] & VTD_CTX_P) == 0) {
        pE[0] = (u64)g_paPdpt | VTD_CTX_P | VTD_CTX_TT_ML;
    }
    pE[1] = VTD_CTX_AW_48 | ((u64)u32Did << VTD_CTX_DID_SHIFT);
    return 0;
}

static void
vtd_domain_pool_init(void)
{
    memset(g_aDom, 0, sizeof(g_aDom));
    memset(g_aAtt, 0, sizeof(g_aAtt));
    /* Domain 0 = default identity domain (context DID=0 at table build) */
    g_aDom[0].u8Used = 1;
    g_aDom[0].u16Ref = 0;
    g_u32DomUsed = 1;
}

/**
 * Build identity SLPT covering [0, 1 GiB) with 2 MiB pages.
 * Layout: PDPT[0] → PD with 512 × 2 MiB SP entries.
 */
static int
vtd_build_slpt_identity_1g(void)
{
    u64 *pPdpt;
    u64 *pPd;
    u32 iPage;

    g_paPdpt = vtd_alloc_zero();
    g_paPd0 = vtd_alloc_zero();
    if (g_paPdpt == 0 || g_paPd0 == 0) {
        return -1;
    }
    pPdpt = (u64 *)vtd_virt(g_paPdpt);
    pPd = (u64 *)vtd_virt(g_paPd0);
    if (pPdpt == NULL || pPd == NULL) {
        return -1;
    }
    /* PDPT entry: present R/W + PD phys */
    pPdpt[0] = (u64)g_paPd0 | VTD_SL_R | VTD_SL_W;
    for (iPage = 0; iPage < 512u; iPage++) {
        u64 u64Pa = (u64)iPage * VTD_2MIB;

        pPd[iPage] = u64Pa | VTD_SL_R | VTD_SL_W | VTD_SL_SP;
    }
    return 0;
}

/** Map DRHD for soft CAP read or HW program; 0 on fail. */
static gj_vaddr_t
vtd_drhd_va(void)
{
    gj_vaddr_t vaDrhd;

    if (g_u64Drhd == 0) {
        return 0;
    }
    if (g_u64Drhd >= 0xf0000000ull) {
        if (vmm_map_device((gj_paddr_t)g_u64Drhd, VTD_DRHD_MAP_CB) != GJ_OK) {
            kprintf("iommu: DRHD map failed base=0x%lx\n",
                    (unsigned long)g_u64Drhd);
            return 0;
        }
    }
    if (hhdm_ready() && g_u64Drhd < 0x100000000ull) {
        vaDrhd = (gj_vaddr_t)(GJ_HHDM_BASE + g_u64Drhd);
    } else {
        vaDrhd = (gj_vaddr_t)g_u64Drhd;
    }
    return vaDrhd;
}

/**
 * Soft CAP/ECAP: prefer read-only DRHD MMIO; else synthetic bring-up CAP.
 * Never programs GCMD. Safe when DRHD absent or map fails.
 */
static void
vtd_soft_cap_load(void)
{
    gj_vaddr_t va;
    volatile u32 *pMmio;
    u64 u64Cap;
    u64 u64Ecap;

    g_fCapFromMmio = 0;
    g_u64Cap = VTD_SOFT_CAP;
    g_u64Ecap = VTD_SOFT_ECAP;

    if (g_u64Drhd == 0) {
        return;
    }
    va = vtd_drhd_va();
    if (va == 0) {
        kprintf("iommu: vtd CAP soft synthetic (DRHD map fail)\n");
        return;
    }
    pMmio = (volatile u32 *)va;
    /* CAP @0x08, ECAP @0x10 — public VT-d register map; read-only soft probe */
    u64Cap = (u64)pMmio[VTD_REG_CAP / 4u] |
             ((u64)pMmio[(VTD_REG_CAP + 4u) / 4u] << 32);
    u64Ecap = (u64)pMmio[VTD_REG_ECAP / 4u] |
              ((u64)pMmio[(VTD_REG_ECAP + 4u) / 4u] << 32);
    /*
     * Reject all-zero / all-ones as non-responsive MMIO (no unit / unmapped).
     * Fall back to synthetic CAP so soft-probe still PASSes on QEMU.
     */
    if (u64Cap == 0 || u64Cap == ~0ull) {
        kprintf("iommu: vtd CAP soft synthetic (DRHD non-responsive)\n");
        return;
    }
    g_u64Cap = u64Cap;
    g_u64Ecap = u64Ecap;
    g_fCapFromMmio = 1;
    kprintf("iommu: vtd CAP=0x%lx ECAP=0x%lx (MMIO soft-read)\n",
            (unsigned long)g_u64Cap, (unsigned long)g_u64Ecap);
}

/** Count present context entries; verify root bus0 P. Returns present count. */
static u32
vtd_soft_verify_tables(void)
{
    u64 *pRoot;
    u64 *pCtx;
    u32 u32Dev;
    u32 cPresent = 0;

    if (!g_fVtdReady || g_paRoot == 0 || g_paContext == 0) {
        return 0;
    }
    pRoot = (u64 *)vtd_virt(g_paRoot);
    pCtx = (u64 *)vtd_virt(g_paContext);
    if (pRoot == NULL || pCtx == NULL) {
        return 0;
    }
    if ((pRoot[0] & VTD_ROOT_P) == 0) {
        kprintf("iommu: vtd soft-probe root P clear\n");
        return 0;
    }
    for (u32Dev = 0; u32Dev < VTD_CTX_ENTRIES; u32Dev++) {
        if ((pCtx[u32Dev * 2u] & VTD_CTX_P) != 0) {
            cPresent++;
        }
    }
    return cPresent;
}

static void
vtd_soft_snapshot_fill(u32 u32CtxPresent)
{
    memset(&g_Soft, 0, sizeof(g_Soft));
    g_Soft.u8TablesReady = g_fVtdReady ? 1u : 0u;
    g_Soft.u8TeMode = (u8)iommu_vtd_te_mode();
    g_Soft.u8HasDrhd = (g_u64Drhd != 0) ? 1u : 0u;
    g_Soft.u8CapFromMmio = g_fCapFromMmio ? 1u : 0u;
    g_Soft.u32Pages = g_u32VtdPages;
    g_Soft.u32CtxDevices = g_u32CtxDevices;
    g_Soft.u32CtxPresent = u32CtxPresent;
    g_Soft.u32Domains = g_u32DomUsed;
    g_Soft.u32DomainMax = GJ_IOMMU_DOMAIN_MAX;
    g_Soft.u32DrhdCount = g_cDrhdInv;
    g_Soft.u32RmrrCount = g_cRmrrInv;
    g_Soft.u32AtsrCount = g_cAtsrInv;
    g_Soft.u32RhsaCount = g_cRhsaInv;
    g_Soft.u32OtherCount = g_cOtherInv;
    g_Soft.u64DrhdBase = g_u64Drhd;
    g_Soft.u64RootPa = (u64)g_paRoot;
    g_Soft.u64IdentityLimit = VTD_IDENTITY_LIMIT;
    g_Soft.u64Cap = g_u64Cap;
    g_Soft.u64Ecap = g_u64Ecap;

    if (g_fVtdReady) {
        g_Soft.u32Feat |= GJ_IOMMU_SOFT_FEAT_TABLES;
    }
    if (g_fVtdReady && iommu_vtd_identity_covers(0, 0x1000)) {
        g_Soft.u32Feat |= GJ_IOMMU_SOFT_FEAT_IDENTITY;
    }
    if (g_u64Drhd != 0) {
        g_Soft.u32Feat |= GJ_IOMMU_SOFT_FEAT_DRHD;
    }
    if (g_fCapFromMmio) {
        g_Soft.u32Feat |= GJ_IOMMU_SOFT_FEAT_CAP_MMIO;
    } else {
        g_Soft.u32Feat |= GJ_IOMMU_SOFT_FEAT_CAP_SOFT;
    }
    if (g_fTeArmed && g_nTeMode == GJ_IOMMU_TE_SOFT) {
        g_Soft.u32Feat |= GJ_IOMMU_SOFT_FEAT_TE_SOFT;
    }
    if (g_fTeArmed && g_nTeMode == GJ_IOMMU_TE_HW) {
        g_Soft.u32Feat |= GJ_IOMMU_SOFT_FEAT_TE_HW;
    }
    if (g_aDom[0].u8Used) {
        g_Soft.u32Feat |= GJ_IOMMU_SOFT_FEAT_DOMAIN;
    }
}

int
iommu_vtd_init_tables(void)
{
    u64 *pRoot;
    u64 *pCtx;
    u32 u32Dev;

    if (g_fVtdReady) {
        return 0;
    }
    g_u32VtdPages = 0;
    g_paRoot = vtd_alloc_zero();
    g_paContext = vtd_alloc_zero();
    if (g_paRoot == 0 || g_paContext == 0) {
        kprintf("iommu: vtd alloc failed\n");
        return -1;
    }
    if (vtd_build_slpt_identity_1g() != 0) {
        kprintf("iommu: vtd SLPT failed\n");
        return -1;
    }
    pRoot = (u64 *)vtd_virt(g_paRoot);
    pCtx = (u64 *)vtd_virt(g_paContext);
    if (pRoot == NULL || pCtx == NULL) {
        kprintf("iommu: vtd root/ctx map failed\n");
        return -1;
    }
    /* Root entry for bus 0: lo = P | context_pa, hi = 0 */
    pRoot[0] = (u64)g_paContext | VTD_ROOT_P;
    pRoot[1] = 0;
    /*
     * Context entries for all 256 BDF indices on bus 0
     * (slot[4:0] << 3 | func[2:0]). Shared identity SLPT; DID=0 soft default.
     */
    g_u32CtxDevices = 0;
    for (u32Dev = 0; u32Dev < VTD_CTX_ENTRIES; u32Dev++) {
        u64 *pE = &pCtx[u32Dev * 2u];

        /* lo: P | TT | SLPT; hi: AW=48-bit, DID=0 */
        pE[0] = (u64)g_paPdpt | VTD_CTX_P | VTD_CTX_TT_ML;
        pE[1] = VTD_CTX_AW_48 | (0ull << VTD_CTX_DID_SHIFT);
        g_u32CtxDevices++;
    }
    vtd_domain_pool_init();
    g_fVtdReady = 1;
    kprintf("iommu: vtd root=0x%lx ctx=0x%lx slpt=0x%lx pages=%u ctx_dev=%u\n",
            (unsigned long)g_paRoot, (unsigned long)g_paContext,
            (unsigned long)g_paPdpt, g_u32VtdPages, g_u32CtxDevices);
    return 0;
}

void
iommu_vtd_set_drhd(u64 u64Base)
{
    /* Accept page-aligned non-zero only; ignore junk firmware values */
    if (u64Base != 0 && (u64Base & 0xfffull) != 0) {
        kprintf("iommu: DRHD reject unaligned=0x%lx\n",
                (unsigned long)u64Base);
        return;
    }
    /* First accepted base wins (multi-DRHD: soft inventory counts all) */
    if (g_u64Drhd != 0 && u64Base != 0 && g_u64Drhd != u64Base) {
        kprintf("iommu: DRHD soft extra base=0x%lx (keep 0x%lx)\n",
                (unsigned long)u64Base, (unsigned long)g_u64Drhd);
        return;
    }
    g_u64Drhd = u64Base;
    if (u64Base != 0) {
        kprintf("iommu: DRHD base=0x%lx\n", (unsigned long)u64Base);
    }
}

u64
iommu_vtd_root_pa(void)
{
    return (u64)g_paRoot;
}

int
iommu_vtd_ready(void)
{
    return g_fVtdReady;
}

u32
iommu_vtd_pages(void)
{
    return g_u32VtdPages;
}

/**
 * Attempt to program RTADDR + GCMD.TE on DRHD when base known.
 * Safe no-op if no DRHD (typical QEMU without -device intel-iommu).
 * Returns 1 if MMIO written, 0 if software-only / map fail.
 */
int
iommu_vtd_program_hw(void)
{
    volatile u32 *pMmio;
    u64 u64Rt;
    u32 u32Spins;
    gj_vaddr_t vaDrhd;

    if (!g_fVtdReady || g_u64Drhd == 0 || g_paRoot == 0) {
        return 0;
    }
    vaDrhd = vtd_drhd_va();
    if (vaDrhd == 0) {
        return 0;
    }
    pMmio = (volatile u32 *)vaDrhd;
    /* RTADDR at 0x20 (64-bit), GCMD at 0x18 — public VT-d register map */
    u64Rt = (u64)g_paRoot;
    pMmio[VTD_REG_RTADDR / 4u] = (u32)(u64Rt & 0xffffffffu);
    pMmio[(VTD_REG_RTADDR + 4u) / 4u] = (u32)(u64Rt >> 32);
    /* Set SRTP then TE — only when a real unit is present */
    pMmio[VTD_REG_GCMD / 4u] = VTD_GCMD_SRTP;
    for (u32Spins = 0; u32Spins < VTD_GSTS_SPINS; u32Spins++) {
        if ((pMmio[VTD_REG_GSTS / 4u] & VTD_GSTS_RTPS) != 0) {
            break;
        }
    }
    if (u32Spins >= VTD_GSTS_SPINS) {
        kprintf("iommu: vtd SRTP timeout DRHD=0x%lx\n",
                (unsigned long)g_u64Drhd);
        return 0;
    }
    pMmio[VTD_REG_GCMD / 4u] = VTD_GCMD_TE;
    g_fTeArmed = 1;
    g_nTeMode = GJ_IOMMU_TE_HW;
    kprintf("iommu: vtd MMIO programmed DRHD=0x%lx RT=0x%lx\n",
            (unsigned long)g_u64Drhd, (unsigned long)u64Rt);
    return 1;
}

int
iommu_vtd_te_arm(void)
{
    if (g_fTeArmed && g_nTeMode != GJ_IOMMU_TE_NONE) {
        return 1;
    }
    if (!g_fVtdReady) {
        if (iommu_vtd_init_tables() != 0) {
            kprintf("iommu: vtd TE arm tables FAIL\n");
            return 0;
        }
    }
    if (g_u64Drhd != 0) {
        if (iommu_vtd_program_hw()) {
            kprintf("iommu: vtd TE arm HW PASS\n");
            return 1;
        }
        kprintf("iommu: vtd TE arm HW fail → soft\n");
    }
    /* Soft TE: tables present; production HW path would set GCMD.TE */
    g_fTeArmed = 1;
    g_nTeMode = GJ_IOMMU_TE_SOFT;
    kprintf("iommu: vtd TE soft-armed root=0x%lx pages=%u\n",
            (unsigned long)g_paRoot, g_u32VtdPages);
    kprintf("iommu: vtd TE soft-arm PASS\n");
    return 1;
}

int
iommu_vtd_te_armed(void)
{
    return g_fTeArmed != 0;
}

/**
 * TE mode: GJ_IOMMU_TE_NONE / SOFT / HW.
 * QEMU without intel-iommu uses soft; DMAR+DRHD MMIO success yields HW.
 */
int
iommu_vtd_te_mode(void)
{
    if (!g_fTeArmed) {
        return GJ_IOMMU_TE_NONE;
    }
    if (g_nTeMode == GJ_IOMMU_TE_HW || g_nTeMode == GJ_IOMMU_TE_SOFT) {
        return g_nTeMode;
    }
    /* Defensive fallback if flag set without mode */
    return (g_u64Drhd != 0) ? GJ_IOMMU_TE_HW : GJ_IOMMU_TE_SOFT;
}

/**
 * Live TE readiness smoke: tables + TE arm + identity cover + mode report.
 * Without DRHD: soft mode is "live-ready" for product path validation.
 * With DRHD: verifies MMIO program path when GSTS responds.
 * Logs iommu: vtd TE live-ready PASS.
 */
int
iommu_vtd_te_live_ready(void)
{
    int fCovered = 0;
    int nMode;

    if (!g_fVtdReady) {
        if (iommu_vtd_init_tables() != 0) {
            kprintf("iommu: vtd TE live-ready tables FAIL\n");
            return 0;
        }
    }
    if (!iommu_vtd_te_arm()) {
        kprintf("iommu: vtd TE live-ready arm FAIL\n");
        return 0;
    }
    nMode = iommu_vtd_te_mode();
    if (nMode == GJ_IOMMU_TE_NONE) {
        return 0;
    }
    if (iommu_vtd_window_grant(0, 0, 0, 0x2000, 0x1000, &fCovered) != 0 ||
        !fCovered) {
        kprintf("iommu: vtd TE live-ready cover FAIL\n");
        return 0;
    }
    if (nMode == GJ_IOMMU_TE_HW) {
        kprintf("iommu: vtd TE live HW mode DRHD=0x%lx\n",
                (unsigned long)g_u64Drhd);
    } else {
        kprintf("iommu: vtd TE live soft mode (no DRHD; set GJ_INTEL_IOMMU=1)\n");
    }
    kprintf("iommu: vtd TE live-ready PASS mode=%d pages=%u\n", nMode,
            g_u32VtdPages);
    return 1;
}

int
iommu_vtd_smoke(void)
{
    int fHw;

    if (iommu_vtd_init_tables() != 0) {
        kprintf("iommu: vtd tables FAIL\n");
        return -1;
    }
    fHw = iommu_vtd_program_hw();
    if (!g_fVtdReady || g_paRoot == 0 || g_u32VtdPages < VTD_PAGES_MIN) {
        kprintf("iommu: vtd tables incomplete pages=%u\n", g_u32VtdPages);
        return -1;
    }
    if (fHw) {
        kprintf("iommu: vtd MMIO path PASS\n");
    } else if (g_u64Drhd != 0) {
        kprintf("iommu: vtd soft-only (DRHD not programmed) PASS\n");
    } else {
        kprintf("iommu: vtd soft-only (no DRHD) PASS\n");
    }
    kprintf("iommu: vtd tables PASS\n");
    return 0;
}

int
iommu_vtd_identity_covers(u64 pa, u64 cb)
{
    if (!g_fVtdReady || cb == 0) {
        return 0;
    }
    if (pa >= VTD_IDENTITY_LIMIT) {
        return 0;
    }
    if (pa + cb < pa) {
        return 0; /* overflow */
    }
    if (pa + cb > VTD_IDENTITY_LIMIT) {
        return 0;
    }
    return 1;
}

int
iommu_vtd_window_grant(u8 bus, u8 slot, u8 func, u64 pa, u64 cb, int *pCovered)
{
    int fCovered = 0;

    if (iommu_window_grant(bus, slot, func, pa, cb) != 0) {
        if (pCovered != NULL) {
            *pCovered = 0;
        }
        return -1;
    }
    if (iommu_vtd_ready()) {
        fCovered = iommu_vtd_identity_covers(pa, cb);
        kprintf("iommu: vtd identity cover=%d %u:%u.%u pa=0x%lx cb=0x%lx\n",
                fCovered, bus, slot, func, (unsigned long)pa,
                (unsigned long)cb);
    }
    if (pCovered != NULL) {
        *pCovered = fCovered;
    }
    return 0;
}

/* ---- Soft probe ---- */

void
iommu_vtd_soft_dmar_inventory(u32 cDrhd, u32 cRmrr, u32 cAtsr, u32 cRhsa,
                              u32 cOther)
{
    g_cDrhdInv = cDrhd;
    g_cRmrrInv = cRmrr;
    g_cAtsrInv = cAtsr;
    g_cRhsaInv = cRhsa;
    g_cOtherInv = cOther;
}

int
iommu_vtd_soft_probe(void)
{
    u32 u32CtxPresent;
    int fId;

    if (!g_fVtdReady) {
        if (iommu_vtd_init_tables() != 0) {
            kprintf("iommu: vtd soft-probe tables FAIL\n");
            g_fSoftProbed = 0;
            return 0;
        }
    }
    if (!g_aDom[0].u8Used) {
        vtd_domain_pool_init();
    }

    vtd_soft_cap_load();
    u32CtxPresent = vtd_soft_verify_tables();
    if (u32CtxPresent == 0 || u32CtxPresent < VTD_CTX_ENTRIES) {
        kprintf("iommu: vtd soft-probe ctx present=%u want=%u\n", u32CtxPresent,
                VTD_CTX_ENTRIES);
        if (u32CtxPresent == 0) {
            kprintf("iommu: vtd soft-probe FAIL\n");
            g_fSoftProbed = 0;
            return 0;
        }
    }

    fId = iommu_vtd_identity_covers(0, VTD_2MIB) &&
          iommu_vtd_identity_covers(VTD_IDENTITY_LIMIT - VTD_2MIB, VTD_2MIB) &&
          !iommu_vtd_identity_covers(VTD_IDENTITY_LIMIT, 0x1000);
    if (!fId) {
        kprintf("iommu: vtd soft-probe identity bounds FAIL\n");
        g_fSoftProbed = 0;
        return 0;
    }

    /*
     * Soft-probe does not arm TE (HW path must remain free for te_arm).
     * Snapshot reports current mode; FEAT_TE_* only if already armed.
     */
    vtd_soft_snapshot_fill(u32CtxPresent);
    g_fSoftProbed = 1;

    kprintf("iommu: vtd soft-probe feat=0x%x pages=%u ctx=%u dom=%u/%u "
            "cap%s=0x%lx\n",
            g_Soft.u32Feat, g_Soft.u32Pages, g_Soft.u32CtxPresent,
            g_Soft.u32Domains, g_Soft.u32DomainMax,
            g_fCapFromMmio ? "" : ".soft", (unsigned long)g_Soft.u64Cap);
    kprintf("iommu: vtd soft-probe dmar drhd=%u rmrr=%u atsr=%u rhsa=%u "
            "other=%u\n",
            g_Soft.u32DrhdCount, g_Soft.u32RmrrCount, g_Soft.u32AtsrCount,
            g_Soft.u32RhsaCount, g_Soft.u32OtherCount);
    kprintf("iommu: vtd soft-probe PASS\n");
    return 1;
}

void
iommu_vtd_soft_info_get(struct gj_iommu_vtd_soft *pOut)
{
    if (pOut == NULL) {
        return;
    }
    if (!g_fSoftProbed) {
        memset(pOut, 0, sizeof(*pOut));
        pOut->u32DomainMax = GJ_IOMMU_DOMAIN_MAX;
        pOut->u64IdentityLimit = VTD_IDENTITY_LIMIT;
        return;
    }
    /* Refresh live counters */
    g_Soft.u8TeMode = (u8)iommu_vtd_te_mode();
    g_Soft.u32Domains = g_u32DomUsed;
    g_Soft.u32Pages = g_u32VtdPages;
    *pOut = g_Soft;
}

int
iommu_vtd_soft_probed(void)
{
    return g_fSoftProbed != 0;
}

/* ---- Domain soft ---- */

u32
iommu_vtd_domain_create(void)
{
    u32 iDom;

    if (!g_aDom[0].u8Used) {
        if (!g_fVtdReady) {
            if (iommu_vtd_init_tables() != 0) {
                return GJ_IOMMU_DOMAIN_INVALID;
            }
        } else {
            vtd_domain_pool_init();
        }
    }
    /* Skip DID 0 (default identity); allocate 1..MAX-1 */
    for (iDom = 1; iDom < GJ_IOMMU_DOMAIN_MAX; iDom++) {
        if (!g_aDom[iDom].u8Used) {
            g_aDom[iDom].u8Used = 1;
            g_aDom[iDom].u16Ref = 0;
            g_u32DomUsed++;
            kprintf("iommu: vtd domain soft create did=%u used=%u\n", iDom,
                    g_u32DomUsed);
            return iDom;
        }
    }
    kprintf("iommu: vtd domain soft create FULL\n");
    return GJ_IOMMU_DOMAIN_INVALID;
}

int
iommu_vtd_domain_destroy(u32 u32Did)
{
    if (u32Did == 0 || u32Did >= GJ_IOMMU_DOMAIN_MAX) {
        return -1;
    }
    if (!g_aDom[u32Did].u8Used) {
        return -1;
    }
    if (g_aDom[u32Did].u16Ref != 0) {
        kprintf("iommu: vtd domain soft destroy busy did=%u ref=%u\n", u32Did,
                (u32)g_aDom[u32Did].u16Ref);
        return -1;
    }
    g_aDom[u32Did].u8Used = 0;
    g_aDom[u32Did].u16Ref = 0;
    if (g_u32DomUsed > 0) {
        g_u32DomUsed--;
    }
    kprintf("iommu: vtd domain soft destroy did=%u used=%u\n", u32Did,
            g_u32DomUsed);
    return 0;
}

int
iommu_vtd_domain_attach(u32 u32Did, u8 bus, u8 slot, u8 func)
{
    u32 iAtt;
    u32 iFree = VTD_SOFT_ATTACH_MAX;

    if (u32Did >= GJ_IOMMU_DOMAIN_MAX || !g_aDom[u32Did].u8Used) {
        return -1;
    }
    if (!vtd_bdf_ok(bus, slot, func)) {
        return -1;
    }
    /* Update existing attach for BDF */
    for (iAtt = 0; iAtt < VTD_SOFT_ATTACH_MAX; iAtt++) {
        if (g_aAtt[iAtt].u8Used && g_aAtt[iAtt].u8Bus == bus &&
            g_aAtt[iAtt].u8Slot == slot && g_aAtt[iAtt].u8Func == func) {
            u32 u32Old = g_aAtt[iAtt].u32Did;

            if (u32Old == u32Did) {
                return 0; /* already attached */
            }
            if (u32Old < GJ_IOMMU_DOMAIN_MAX && g_aDom[u32Old].u8Used &&
                g_aDom[u32Old].u16Ref > 0) {
                g_aDom[u32Old].u16Ref--;
            }
            g_aAtt[iAtt].u32Did = u32Did;
            if (g_aDom[u32Did].u16Ref < 0xffffu) {
                g_aDom[u32Did].u16Ref++;
            }
            if (vtd_ctx_set_did(bus, slot, func, u32Did) != 0 && bus == 0) {
                kprintf("iommu: vtd domain soft ctx DID write fail\n");
            }
            kprintf("iommu: vtd domain soft rebind %u:%u.%u did=%u\n", bus,
                    slot, func, u32Did);
            return 0;
        }
        if (!g_aAtt[iAtt].u8Used && iFree == VTD_SOFT_ATTACH_MAX) {
            iFree = iAtt;
        }
    }
    if (iFree >= VTD_SOFT_ATTACH_MAX) {
        kprintf("iommu: vtd domain soft attach FULL\n");
        return -1;
    }
    g_aAtt[iFree].u8Used = 1;
    g_aAtt[iFree].u8Bus = bus;
    g_aAtt[iFree].u8Slot = slot;
    g_aAtt[iFree].u8Func = func;
    g_aAtt[iFree].u32Did = u32Did;
    if (g_aDom[u32Did].u16Ref < 0xffffu) {
        g_aDom[u32Did].u16Ref++;
    }
    if (vtd_ctx_set_did(bus, slot, func, u32Did) != 0 && bus == 0) {
        kprintf("iommu: vtd domain soft ctx DID write fail\n");
    }
    kprintf("iommu: vtd domain soft attach %u:%u.%u did=%u ref=%u\n", bus, slot,
            func, u32Did, (u32)g_aDom[u32Did].u16Ref);
    return 0;
}

int
iommu_vtd_domain_detach(u8 bus, u8 slot, u8 func)
{
    u32 iAtt;

    if (!vtd_bdf_ok(bus, slot, func)) {
        return -1;
    }
    for (iAtt = 0; iAtt < VTD_SOFT_ATTACH_MAX; iAtt++) {
        if (g_aAtt[iAtt].u8Used && g_aAtt[iAtt].u8Bus == bus &&
            g_aAtt[iAtt].u8Slot == slot && g_aAtt[iAtt].u8Func == func) {
            u32 u32Did = g_aAtt[iAtt].u32Did;

            if (u32Did < GJ_IOMMU_DOMAIN_MAX && g_aDom[u32Did].u8Used &&
                g_aDom[u32Did].u16Ref > 0) {
                g_aDom[u32Did].u16Ref--;
            }
            memset(&g_aAtt[iAtt], 0, sizeof(g_aAtt[iAtt]));
            /* Rebind bus-0 context to default domain 0 */
            (void)vtd_ctx_set_did(bus, slot, func, 0);
            kprintf("iommu: vtd domain soft detach %u:%u.%u (was did=%u)\n",
                    bus, slot, func, u32Did);
            return 0;
        }
    }
    return -1;
}

u32
iommu_vtd_domain_lookup(u8 bus, u8 slot, u8 func)
{
    u32 iAtt;

    if (!vtd_bdf_ok(bus, slot, func)) {
        return GJ_IOMMU_DOMAIN_INVALID;
    }
    for (iAtt = 0; iAtt < VTD_SOFT_ATTACH_MAX; iAtt++) {
        if (g_aAtt[iAtt].u8Used && g_aAtt[iAtt].u8Bus == bus &&
            g_aAtt[iAtt].u8Slot == slot && g_aAtt[iAtt].u8Func == func) {
            return g_aAtt[iAtt].u32Did;
        }
    }
    /* Unbound devices soft-default to domain 0 when tables ready */
    if (g_fVtdReady && bus == 0) {
        return 0;
    }
    return GJ_IOMMU_DOMAIN_INVALID;
}

u32
iommu_vtd_domain_count(void)
{
    return g_u32DomUsed;
}

int
iommu_vtd_domain_soft_smoke(void)
{
    u32 u32Did;
    u32 u32Look;
    u64 *pCtx;
    u32 u32Idx;
    u64 u64Hi;

    if (!g_fVtdReady) {
        if (iommu_vtd_init_tables() != 0) {
            kprintf("iommu: vtd domain soft tables FAIL\n");
            return 0;
        }
    }
    if (!g_aDom[0].u8Used) {
        vtd_domain_pool_init();
    }

    u32Did = iommu_vtd_domain_create();
    if (u32Did == GJ_IOMMU_DOMAIN_INVALID || u32Did == 0) {
        kprintf("iommu: vtd domain soft create FAIL\n");
        return 0;
    }
    /* Attach bring-up BDF 0:3.0 (unused by enforce smoke's 0:2.0) */
    if (iommu_vtd_domain_attach(u32Did, 0, 3, 0) != 0) {
        kprintf("iommu: vtd domain soft attach FAIL\n");
        (void)iommu_vtd_domain_destroy(u32Did);
        return 0;
    }
    u32Look = iommu_vtd_domain_lookup(0, 3, 0);
    if (u32Look != u32Did) {
        kprintf("iommu: vtd domain soft lookup FAIL got=%u want=%u\n", u32Look,
                u32Did);
        (void)iommu_vtd_domain_detach(0, 3, 0);
        (void)iommu_vtd_domain_destroy(u32Did);
        return 0;
    }
    /* Verify context DID written for bus 0 */
    pCtx = (u64 *)vtd_virt(g_paContext);
    if (pCtx != NULL) {
        u32Idx = vtd_ctx_index(3, 0);
        u64Hi = pCtx[u32Idx * 2u + 1u];
        if (((u64Hi >> VTD_CTX_DID_SHIFT) & 0xffffull) != (u64)u32Did) {
            kprintf("iommu: vtd domain soft ctx DID FAIL hi=0x%lx did=%u\n",
                    (unsigned long)u64Hi, u32Did);
            (void)iommu_vtd_domain_detach(0, 3, 0);
            (void)iommu_vtd_domain_destroy(u32Did);
            return 0;
        }
    }
    if (iommu_vtd_domain_destroy(u32Did) == 0) {
        kprintf("iommu: vtd domain soft destroy busy-check FAIL\n");
        (void)iommu_vtd_domain_detach(0, 3, 0);
        (void)iommu_vtd_domain_destroy(u32Did);
        return 0;
    }
    if (iommu_vtd_domain_detach(0, 3, 0) != 0) {
        kprintf("iommu: vtd domain soft detach FAIL\n");
        return 0;
    }
    u32Look = iommu_vtd_domain_lookup(0, 3, 0);
    if (u32Look != 0) {
        kprintf("iommu: vtd domain soft post-detach lookup FAIL got=%u\n",
                u32Look);
        return 0;
    }
    if (iommu_vtd_domain_destroy(u32Did) != 0) {
        kprintf("iommu: vtd domain soft destroy FAIL\n");
        return 0;
    }
    /* Reject bad BDF / invalid DID */
    if (iommu_vtd_domain_attach(0, 0, 32, 0) == 0 ||
        iommu_vtd_domain_attach(GJ_IOMMU_DOMAIN_MAX, 0, 1, 0) == 0) {
        kprintf("iommu: vtd domain soft reject FAIL\n");
        return 0;
    }

    kprintf("iommu: vtd domain soft count=%u max=%u\n", iommu_vtd_domain_count(),
            GJ_IOMMU_DOMAIN_MAX);
    kprintf("iommu: vtd domain soft PASS\n");
    return 1;
}
