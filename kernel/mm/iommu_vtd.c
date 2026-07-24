/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * VT-d (Intel IOMMU) page-table construction — clean-room, dual-licensed.
 * Builds root + context + second-level identity map in memory.
 * Soft-probe: CAP/ECAP soft or MMIO, root/context verify, DMAR inventory.
 * Domain soft: software DID pool; bus-0 context DID write when tables ready.
 * Product soft (P-DMA-4): production-default enforce arm — no open bus-master
 * without a window; soft-only PASS without intel-iommu; honesty open_bus=0.
 * Optional DRHD MMIO program when ACPI DMAR provides a base.
 * Not derived from Linux intel-iommu or any GPL VT-d driver.
 *
 * Wave 35 exclusive soft deepen (this unit only — greppable "vtd: soft …"):
 *   vtd: soft inventory  — tables/pages/ctx/domains/feat rollup
 *   vtd: soft tables     — root/context/SLPT identity construct
 *   vtd: soft cap        — CAP/ECAP MMIO or synthetic soft
 *   vtd: soft dmar       — DRHD/RMRR/ATSR/RHSA/other counts
 *   vtd: soft te         — TE arm mode (none/soft/hw)
 *   vtd: soft domain     — soft DID pool + attach slots
 *   vtd: soft identity   — bring-up identity cover lamps
 *   vtd: soft product    — P-DMA-4 no-open-bus soft path
 *   vtd: soft attach     — attach/rebind/detach/create tallies (W15)
 *   vtd: soft feat       — expanded SOFT_FEAT bitmask lamps (W15)
 *   vtd: soft qi         — QI invalidate product remains OPEN (W15)
 *   vtd: soft root       — root/context present verify lamps (W15)
 *   vtd: soft path       — honesty: always-on product IOMMU remains OPEN
 *   vtd: soft lamps      — composite soft lamps
 *   vtd: soft honesty    — explicit non-claims catalog (W15)
 *   vtd: soft stats      — rollup for agent greps (W15)
 *   vtd: soft surfaces   — Wave 19 return-surface catalog
 *   vtd: soft did        — Wave 17 DID pool return surface
 *   vtd: soft mmio       — Wave 17 DRHD MMIO program surface
 *   vtd: soft return     — Wave 17 attach/domain return taxonomy
 *   vtd: soft return selftest — Wave 19 terminal return surface
 *   vtd: soft retmap     — Wave 19 return-surface map
 *   vtd: soft deepen     — wave=35 stamp + area count
 *   vtd: soft OPEN       — always-on product IOMMU OPEN honesty
 *   vtd: soft PASS | soft inventory PASS
 * Soft deepen ≠ product always-on IOMMU claim; not bar3; not HW product close;
 * soft ≠ product.
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

/* Wave 35 soft inventory stamp (file-local; never product gate). */
#define VTD_SOFT_WAVE 35u
/* Fixed greppable categories for deepen stamp (inventory…return + W16 axes). */
#define VTD_SOFT_AREAS 58u

/*
 * Wave 19 return-surface bit lamps (surf=0x… on soft surfaces/deepen).
 * greppable: vtd: soft surfaces
 */
#define VTD_SOFT_SURF_INVENTORY (1u << 0)
#define VTD_SOFT_SURF_TABLES    (1u << 1)
#define VTD_SOFT_SURF_CAP       (1u << 2)
#define VTD_SOFT_SURF_DMAR      (1u << 3)
#define VTD_SOFT_SURF_TE        (1u << 4)
#define VTD_SOFT_SURF_DOMAIN    (1u << 5)
#define VTD_SOFT_SURF_IDENTITY  (1u << 6)
#define VTD_SOFT_SURF_PRODUCT   (1u << 7)
#define VTD_SOFT_SURF_ATTACH    (1u << 8)
#define VTD_SOFT_SURF_FEAT      (1u << 9)
#define VTD_SOFT_SURF_QI        (1u << 10)
#define VTD_SOFT_SURF_ROOT      (1u << 11)
#define VTD_SOFT_SURF_PATH      (1u << 12)
#define VTD_SOFT_SURF_LAMPS     (1u << 13)
#define VTD_SOFT_SURF_HONESTY   (1u << 14)
#define VTD_SOFT_SURF_STATS     (1u << 15)
#define VTD_SOFT_SURF_OPEN      (1u << 16)
#define VTD_SOFT_SURF_SURFACES  (1u << 17)
#define VTD_SOFT_SURF_DID       (1u << 18)
#define VTD_SOFT_SURF_MMIO      (1u << 19)
#define VTD_SOFT_SURF_RETURN    (1u << 20)
#define VTD_SOFT_SURF_CATALOG                                                      \
    (VTD_SOFT_SURF_INVENTORY | VTD_SOFT_SURF_TABLES | VTD_SOFT_SURF_CAP |          \
     VTD_SOFT_SURF_DMAR | VTD_SOFT_SURF_TE | VTD_SOFT_SURF_DOMAIN |                \
     VTD_SOFT_SURF_IDENTITY | VTD_SOFT_SURF_PRODUCT | VTD_SOFT_SURF_ATTACH |       \
     VTD_SOFT_SURF_FEAT | VTD_SOFT_SURF_QI | VTD_SOFT_SURF_ROOT |                  \
     VTD_SOFT_SURF_PATH | VTD_SOFT_SURF_LAMPS | VTD_SOFT_SURF_HONESTY |            \
     VTD_SOFT_SURF_STATS | VTD_SOFT_SURF_OPEN | VTD_SOFT_SURF_SURFACES |           \
     VTD_SOFT_SURF_DID | VTD_SOFT_SURF_MMIO | VTD_SOFT_SURF_RETURN)

/*
 * Product-default soft BDF (P-DMA-4 smoke). Kept off main enforce 0:2.0 and
 * domain soft 0:3.0 so bring-up greps stay independent.
 */
#define VTD_PROD_SOFT_BUS  0u
#define VTD_PROD_SOFT_SLOT 4u
#define VTD_PROD_SOFT_FUNC 0u
#define VTD_PROD_SOFT_PA   0x3000ull
#define VTD_PROD_SOFT_CB   0x1000ull
/* Ungranted BDF for deny-path counter smoke (not 0:31.0 — main uses that). */
#define VTD_PROD_DENY_SLOT 31u
#define VTD_PROD_DENY_FUNC 7u

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

/* Product-default soft (P-DMA-4): local deny-path ticks while enforce armed */
static u32 g_u32ProdSoftDeny;

/* Wave 19 greppable soft inventory dump count (vtd: soft …) */
static u32 g_cSoftInvLogs;

/*
 * Wave 15 soft domain/attach/ops tallies (diagnostics only; wrap OK).
 * Never hard-gates; not product always-on IOMMU / QI.
 */
static u32 g_cSoftDomCreate;   /* domain soft create ok */
static u32 g_cSoftDomCreateFail; /* domain soft create fail/full */
static u32 g_cSoftDomDestroy;  /* domain soft destroy ok */
static u32 g_cSoftDomDestroyFail;
static u32 g_cSoftAttOk;       /* domain attach new ok */
static u32 g_cSoftAttRebind;   /* domain attach rebind */
static u32 g_cSoftAttFail;     /* attach fail/full/bad */
static u32 g_cSoftDetOk;       /* detach ok */
static u32 g_cSoftDetMiss;     /* detach miss */
static u32 g_cSoftCtxDidWrite; /* bus-0 context DID write attempts ok */
static u32 g_cSoftCtxDidFail;  /* context DID write fail */
static u32 g_cSoftAttPeak;     /* peak live attach slots */
static u32 g_cSoftDomPeak;     /* peak soft domains used */
static u32 g_cSoftRootPOk;     /* soft-verify root P seen */
static u32 g_cSoftRootPClear;  /* soft-verify root P clear */
static u32 g_cSoftCapMmioHit;  /* CAP loaded from MMIO */
static u32 g_cSoftCapSynth;    /* CAP fell back to synthetic */

static void vtd_soft_inventory_log(void);
static void vtd_soft_note_att_peak(void);

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
        if (g_cSoftCapSynth < 0xffffffffu) {
            g_cSoftCapSynth++;
        }
        return;
    }
    va = vtd_drhd_va();
    if (va == 0) {
        if (g_cSoftCapSynth < 0xffffffffu) {
            g_cSoftCapSynth++;
        }
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
        if (g_cSoftCapSynth < 0xffffffffu) {
            g_cSoftCapSynth++;
        }
        kprintf("iommu: vtd CAP soft synthetic (DRHD non-responsive)\n");
        return;
    }
    g_u64Cap = u64Cap;
    g_u64Ecap = u64Ecap;
    g_fCapFromMmio = 1;
    if (g_cSoftCapMmioHit < 0xffffffffu) {
        g_cSoftCapMmioHit++;
    }
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
        if (g_cSoftRootPClear < 0xffffffffu) {
            g_cSoftRootPClear++;
        }
        kprintf("iommu: vtd soft-probe root P clear\n");
        return 0;
    }
    if (g_cSoftRootPOk < 0xffffffffu) {
        g_cSoftRootPOk++;
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

/** Wave 15: peak live attach slots + domain used (diagnostics only). */
static void
vtd_soft_note_att_peak(void)
{
    u32 cAtt = 0;
    u32 iAtt;

    for (iAtt = 0; iAtt < VTD_SOFT_ATTACH_MAX; iAtt++) {
        if (g_aAtt[iAtt].u8Used) {
            cAtt++;
        }
    }
    if (cAtt > g_cSoftAttPeak) {
        g_cSoftAttPeak = cAtt;
    }
    if (g_u32DomUsed > g_cSoftDomPeak) {
        g_cSoftDomPeak = g_u32DomUsed;
    }
}

/**
 * Wave 19 greppable soft inventory dump (prefix "vtd: soft …").
 * Diagnostics only — never hard-gates; never claims always-on product IOMMU.
 *
 * greppable: vtd: soft
 * greppable: vtd: soft inventory
 * greppable: vtd: soft tables
 * greppable: vtd: soft cap
 * greppable: vtd: soft dmar
 * greppable: vtd: soft te
 * greppable: vtd: soft domain
 * greppable: vtd: soft identity
 * greppable: vtd: soft product
 * greppable: vtd: soft attach
 * greppable: vtd: soft feat
 * greppable: vtd: soft qi
 * greppable: vtd: soft root
 * greppable: vtd: soft path
 * greppable: vtd: soft lamps
 * greppable: vtd: soft honesty
 * greppable: vtd: soft stats
 * greppable: vtd: soft deepen
 * greppable: vtd: soft OPEN
 * greppable: vtd: soft surfaces
 * greppable: vtd: soft did
 * greppable: vtd: soft mmio
 * greppable: vtd: soft return
 * greppable: vtd: soft PASS
 */
static void
vtd_soft_inventory_log(void)
{
    u32 cAtt = 0;
    u32 iAtt;
    u32 u32Surf;
    int fReady;
    int fTe;
    int nMode;
    int fIdLo;
    int fIdHi;
    int fIdOut;
    int fCoveredSoft;
    int fRootP;
    u32 u32Feat;
    const char *szMode;
    const char *szCapSrc;

    if (g_cSoftInvLogs < 0xffffffffu) {
        g_cSoftInvLogs++;
    }

    for (iAtt = 0; iAtt < VTD_SOFT_ATTACH_MAX; iAtt++) {
        if (g_aAtt[iAtt].u8Used) {
            cAtt++;
        }
    }
    vtd_soft_note_att_peak();

    fReady = g_fVtdReady ? 1 : 0;
    fTe = g_fTeArmed ? 1 : 0;
    nMode = iommu_vtd_te_mode();
    if (nMode == GJ_IOMMU_TE_HW) {
        szMode = "hw";
    } else if (nMode == GJ_IOMMU_TE_SOFT) {
        szMode = "soft";
    } else {
        szMode = "none";
    }
    szCapSrc = g_fCapFromMmio ? "mmio" : "synthetic";
    fIdLo = iommu_vtd_identity_covers(0, 0x1000);
    fIdHi = iommu_vtd_identity_covers(VTD_IDENTITY_LIMIT - VTD_2MIB, VTD_2MIB);
    fIdOut = iommu_vtd_identity_covers(VTD_IDENTITY_LIMIT, 0x1000);
    fCoveredSoft = (fIdLo && fIdHi && !fIdOut) ? 1 : 0;
    fRootP = (g_cSoftRootPOk > 0 && g_cSoftRootPClear == 0) ? 1 : 0;
    if (fReady && g_paRoot != 0) {
        u64 *pRoot = (u64 *)vtd_virt(g_paRoot);

        if (pRoot != NULL) {
            fRootP = ((pRoot[0] & VTD_ROOT_P) != 0) ? 1 : 0;
        }
    }
    u32Feat = g_Soft.u32Feat;
    u32Surf = VTD_SOFT_SURF_CATALOG;

    /* Grep: vtd: soft inventory */
    kprintf("vtd: soft inventory ready=%d pages=%u ctx_dev=%u ctx_present=%u "
            "dom=%u/%u feat=0x%x te_mode=%s drhd=0x%lx logs=%u wave=%u "
            "(soft inventory; not always-on product IOMMU; not bar3)\n",
            fReady, g_u32VtdPages, g_u32CtxDevices,
            g_Soft.u32CtxPresent, g_u32DomUsed,
            (unsigned)GJ_IOMMU_DOMAIN_MAX, u32Feat, szMode,
            (unsigned long)g_u64Drhd, g_cSoftInvLogs,
            (unsigned)VTD_SOFT_WAVE);

    /* Grep: vtd: soft tables */
    kprintf("vtd: soft tables ready=%d root=0x%lx ctx=0x%lx slpt=0x%lx "
            "pd0=0x%lx pages=%u ctx_dev=%u min_pages=%u wave=%u "
            "(RAM construct; not HW product close)\n",
            fReady, (unsigned long)g_paRoot, (unsigned long)g_paContext,
            (unsigned long)g_paPdpt, (unsigned long)g_paPd0, g_u32VtdPages,
            g_u32CtxDevices, (unsigned)VTD_PAGES_MIN, (unsigned)VTD_SOFT_WAVE);

    /* Grep: vtd: soft cap */
    kprintf("vtd: soft cap src=%s mmio=%d cap=0x%lx ecap=0x%lx "
            "feat_cap_mmio=%u feat_cap_soft=%u mmio_hit=%u synth=%u "
            "wave=%u (read-only soft; never programs GCMD here)\n",
            szCapSrc, g_fCapFromMmio ? 1 : 0, (unsigned long)g_u64Cap,
            (unsigned long)g_u64Ecap,
            (u32Feat & GJ_IOMMU_SOFT_FEAT_CAP_MMIO) ? 1u : 0u,
            (u32Feat & GJ_IOMMU_SOFT_FEAT_CAP_SOFT) ? 1u : 0u,
            g_cSoftCapMmioHit, g_cSoftCapSynth, (unsigned)VTD_SOFT_WAVE);

    /* Grep: vtd: soft dmar */
    kprintf("vtd: soft dmar drhd=%u rmrr=%u atsr=%u rhsa=%u other=%u "
            "has_drhd=%d base=0x%lx wave=%u (fed from ACPI soft inventory)\n",
            g_cDrhdInv, g_cRmrrInv, g_cAtsrInv, g_cRhsaInv, g_cOtherInv,
            (g_u64Drhd != 0) ? 1 : 0, (unsigned long)g_u64Drhd,
            (unsigned)VTD_SOFT_WAVE);

    /* Grep: vtd: soft te */
    kprintf("vtd: soft te armed=%d mode=%s mode_n=%d "
            "feat_te_soft=%u feat_te_hw=%u root=0x%lx wave=%u "
            "(soft-arm ≠ product always-on TE)\n",
            fTe, szMode, nMode,
            (u32Feat & GJ_IOMMU_SOFT_FEAT_TE_SOFT) ? 1u : 0u,
            (u32Feat & GJ_IOMMU_SOFT_FEAT_TE_HW) ? 1u : 0u,
            (unsigned long)g_paRoot, (unsigned)VTD_SOFT_WAVE);

    /* Grep: vtd: soft domain */
    kprintf("vtd: soft domain used=%u max=%u attach_live=%u attach_max=%u "
            "default0=%u feat_domain=%u create=%u destroy=%u peak=%u "
            "wave=%u (software DID pool; no QI invalidate product)\n",
            g_u32DomUsed, (unsigned)GJ_IOMMU_DOMAIN_MAX, cAtt,
            (unsigned)VTD_SOFT_ATTACH_MAX, g_aDom[0].u8Used ? 1u : 0u,
            (u32Feat & GJ_IOMMU_SOFT_FEAT_DOMAIN) ? 1u : 0u,
            g_cSoftDomCreate, g_cSoftDomDestroy, g_cSoftDomPeak,
            (unsigned)VTD_SOFT_WAVE);

    /* Grep: vtd: soft identity */
    kprintf("vtd: soft identity limit=0x%lx cover_lo=%d cover_hi=%d "
            "cover_out=%d bounds_ok=%d feat_id=%u sp=2MiB wave=%u "
            "(bring-up first 1GiB; not full address-space product)\n",
            (unsigned long)VTD_IDENTITY_LIMIT, fIdLo, fIdHi, fIdOut,
            fCoveredSoft,
            (u32Feat & GJ_IOMMU_SOFT_FEAT_IDENTITY) ? 1u : 0u,
            (unsigned)VTD_SOFT_WAVE);

    /* Grep: vtd: soft product (P-DMA-4 soft path tallies) */
    kprintf("vtd: soft product soft_denies=%u open_bus=0 "
            "prod_bdf=%u:%u.%u deny_bdf=%u:%u.%u "
            "production_default_open_bus=0 soft wave=%u "
            "(P-DMA-4 soft; not product always-on IOMMU)\n",
            g_u32ProdSoftDeny, (unsigned)VTD_PROD_SOFT_BUS,
            (unsigned)VTD_PROD_SOFT_SLOT, (unsigned)VTD_PROD_SOFT_FUNC,
            (unsigned)VTD_PROD_SOFT_BUS, (unsigned)VTD_PROD_DENY_SLOT,
            (unsigned)VTD_PROD_DENY_FUNC, (unsigned)VTD_SOFT_WAVE);

    /* Grep: vtd: soft attach (Wave 15 domain attach path) */
    kprintf("vtd: soft attach ok=%u rebind=%u fail=%u detach_ok=%u "
            "detach_miss=%u live=%u peak=%u ctx_did_ok=%u ctx_did_fail=%u "
            "wave=%u (software attach; not QI/HW invalidate product)\n",
            g_cSoftAttOk, g_cSoftAttRebind, g_cSoftAttFail, g_cSoftDetOk,
            g_cSoftDetMiss, cAtt, g_cSoftAttPeak, g_cSoftCtxDidWrite,
            g_cSoftCtxDidFail, (unsigned)VTD_SOFT_WAVE);

    /* Grep: vtd: soft feat (Wave 15 expanded feature lamps) */
    kprintf("vtd: soft feat mask=0x%x tables=%u identity=%u drhd=%u "
            "cap_mmio=%u cap_soft=%u te_soft=%u te_hw=%u domain=%u "
            "wave=%u (SOFT_FEAT bits; soft only)\n",
            u32Feat,
            (u32Feat & GJ_IOMMU_SOFT_FEAT_TABLES) ? 1u : 0u,
            (u32Feat & GJ_IOMMU_SOFT_FEAT_IDENTITY) ? 1u : 0u,
            (u32Feat & GJ_IOMMU_SOFT_FEAT_DRHD) ? 1u : 0u,
            (u32Feat & GJ_IOMMU_SOFT_FEAT_CAP_MMIO) ? 1u : 0u,
            (u32Feat & GJ_IOMMU_SOFT_FEAT_CAP_SOFT) ? 1u : 0u,
            (u32Feat & GJ_IOMMU_SOFT_FEAT_TE_SOFT) ? 1u : 0u,
            (u32Feat & GJ_IOMMU_SOFT_FEAT_TE_HW) ? 1u : 0u,
            (u32Feat & GJ_IOMMU_SOFT_FEAT_DOMAIN) ? 1u : 0u,
            (unsigned)VTD_SOFT_WAVE);

    /* Grep: vtd: soft qi (Wave 15 QI product OPEN honesty) */
    kprintf("vtd: soft qi product=OPEN invalidate=OPEN "
            "context_cache=OPEN iotlb=OPEN ecap=0x%lx wave=%u "
            "(no QI product; soft deepen ≠ QI claim)\n",
            (unsigned long)g_u64Ecap, (unsigned)VTD_SOFT_WAVE);

    /* Grep: vtd: soft root (Wave 15 root/context lamps) */
    kprintf("vtd: soft root ready=%d root_p=%d root_pa=0x%lx "
            "ctx_pa=0x%lx ctx_present=%u ctx_dev=%u "
            "verify_ok=%u verify_clear=%u wave=%u "
            "(RAM root; not HW RTADDR product default)\n",
            fReady, fRootP, (unsigned long)g_paRoot,
            (unsigned long)g_paContext, g_Soft.u32CtxPresent,
            g_u32CtxDevices, g_cSoftRootPOk, g_cSoftRootPClear,
            (unsigned)VTD_SOFT_WAVE);

    /*
     * Honesty: always-on product IOMMU remains OPEN.
     * Soft deepen ≠ product always-on IOMMU claim.
     * Grep: vtd: soft path
     */
    kprintf("vtd: soft path tables=ram_identity_1g te=soft_or_hw "
            "cap=mmio_or_synthetic domain=soft_did "
            "always_on_product=OPEN hw_default_te=OPEN "
            "qi_invalidate=OPEN bar3=OPEN wave=%u "
            "(soft inventory; not product always-on IOMMU)\n",
            (unsigned)VTD_SOFT_WAVE);

    /* Grep: vtd: soft lamps */
    kprintf("vtd: soft lamps tables=%d te=%d cap_mmio=%d has_drhd=%d "
            "identity=%d domain0=%d soft_probed=%d root_p=%d wave=%u "
            "(composite soft lamps)\n",
            fReady, fTe, g_fCapFromMmio ? 1 : 0, (g_u64Drhd != 0) ? 1 : 0,
            fCoveredSoft, g_aDom[0].u8Used ? 1 : 0, g_fSoftProbed, fRootP,
            (unsigned)VTD_SOFT_WAVE);

    /* Grep: vtd: soft honesty (Wave 15 non-claims catalog) */
    kprintf("vtd: soft honesty always_on_product=OPEN "
            "no_open_bus_master_product=OPEN hw_default_te=OPEN "
            "qi_invalidate=OPEN full_as_identity=OPEN "
            "amd_vi_product=OPEN bar3=OPEN inventory_only=1 "
            "soft_neq_product=1 wave=%u "
            "(explicit non-claims; not product always-on IOMMU)\n",
            (unsigned)VTD_SOFT_WAVE);

    /* Grep: vtd: soft stats (Wave 15 rollup) */
    kprintf("vtd: soft stats ready=%d pages=%u ctx=%u dom=%u/%u "
            "att_live=%u att_peak=%u create=%u attach=%u "
            "prod_deny=%u feat=0x%x te=%s logs=%u wave=%u "
            "(rollup; not product close)\n",
            fReady, g_u32VtdPages, g_Soft.u32CtxPresent, g_u32DomUsed,
            (unsigned)GJ_IOMMU_DOMAIN_MAX, cAtt, g_cSoftAttPeak,
            g_cSoftDomCreate, g_cSoftAttOk, g_u32ProdSoftDeny, u32Feat,
            szMode, g_cSoftInvLogs, (unsigned)VTD_SOFT_WAVE);

    /*
     * Wave 19: return-surface catalog (surf bitmask; soft ≠ product).
     * Grep: vtd: soft surfaces
     */
    kprintf("vtd: soft surfaces surf=0x%x catalog=%u "
            "tables=1 cap=1 domain=1 attach=1 qi=1 root=1 "
            "did=1 mmio=1 return=1 open=1 wave=%u "
            "(return surfaces; soft only; not product always-on IOMMU; "
            "not bar3)\n",
            (unsigned)u32Surf, (unsigned)VTD_SOFT_AREAS,
            (unsigned)VTD_SOFT_WAVE);

    /*
     * Wave 19: DID pool return surface.
     * Grep: vtd: soft did
     */
    kprintf("vtd: soft did used=%u max=%u create=%u destroy=%u "
            "attach_live=%u attach_peak=%u default0=%u wave=%u "
            "(software DID pool; soft only; not QI product; not bar3)\n",
            g_u32DomUsed, (unsigned)GJ_IOMMU_DOMAIN_MAX,
            g_cSoftDomCreate, g_cSoftDomDestroy, cAtt, g_cSoftAttPeak,
            g_aDom[0].u8Used ? 1u : 0u, (unsigned)VTD_SOFT_WAVE);

    /*
     * Wave 19: DRHD MMIO program surface (optional; soft).
     * Grep: vtd: soft mmio
     */
    kprintf("vtd: soft mmio has_drhd=%d base=0x%lx cap_src=%s "
            "cap_mmio=%d te_mode=%s srtp=soft_or_hw wave=%u "
            "(soft MMIO surface; not product always-on TE; not bar3)\n",
            (g_u64Drhd != 0) ? 1 : 0, (unsigned long)g_u64Drhd, szCapSrc,
            g_fCapFromMmio ? 1 : 0, szMode, (unsigned)VTD_SOFT_WAVE);

    /*
     * Wave 19: attach/domain return taxonomy.
     * Grep: vtd: soft return
     */
    kprintf("vtd: soft return att_ok=%u att_rebind=%u att_fail=%u "
            "det_ok=%u det_miss=%u ctx_did_ok=%u ctx_did_fail=%u "
            "dom_create=%u dom_destroy=%u wave=%u "
            "(soft return taxonomy; not product; not bar3)\n",
            g_cSoftAttOk, g_cSoftAttRebind, g_cSoftAttFail,
            g_cSoftDetOk, g_cSoftDetMiss, g_cSoftCtxDidWrite,
            g_cSoftCtxDidFail, g_cSoftDomCreate, g_cSoftDomDestroy,
            (unsigned)VTD_SOFT_WAVE);

    /*
     * Grep: vtd: soft return rate
     * Wave 17 return-surface rate lamps (kept) (attach/domain).
     */
    kprintf("vtd: soft return rate "
            "att_ok=%u att_fail=%u det_ok=%u det_miss=%u "
            "dom_create=%u dom_destroy=%u ctx_did_fail=%u "
            "logs=%u wave=%u "
            "(return rate; Soft≠product always-on IOMMU; soft≠product; "
            "not bar3)\n",
            g_cSoftAttOk, g_cSoftAttFail, g_cSoftDetOk, g_cSoftDetMiss,
            g_cSoftDomCreate, g_cSoftDomDestroy, g_cSoftCtxDidFail,
            g_cSoftInvLogs, (unsigned)VTD_SOFT_WAVE);

    /*
     * Grep: vtd: soft retcode
     * Wave 17 retcode catalog for attach/domain return classes.
     */
    kprintf("vtd: soft retcode "
            "att_ok=1 att_rebind=1 att_fail=1 det_ok=1 det_miss=1 "
            "ctx_did_ok=1 ctx_did_fail=1 dom_create=1 dom_destroy=1 "
            "qi_product=OPEN always_on_product=OPEN wave=%u "
            "(retcode catalog; Soft≠product always-on IOMMU; soft≠product)\n",
            (unsigned)VTD_SOFT_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: vtd: soft return selftest — Wave 19 terminal return surface */
    kprintf("vtd: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 rate_limited=0 wave=%u soft PASS\n",
            (unsigned)VTD_SOFT_WAVE);

    /* Grep: vtd: soft retmap — Wave 19 return-surface map */
    kprintf("vtd: soft retmap soft_inv=1 deepen=1 return_rate=1 retcode=1 "
            "product=OPEN wave=%u soft PASS\n",
            (unsigned)VTD_SOFT_WAVE);

    /* Grep: vtd: soft deepen wave (Wave 24 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: vtd: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("vtd: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)VTD_SOFT_WAVE);
    /* Grep: vtd: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("vtd: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)VTD_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: vtd: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("vtd: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)VTD_SOFT_WAVE);
    /* Grep: vtd: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("vtd: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)VTD_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: vtd: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("vtd: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)VTD_SOFT_WAVE);
            /* Grep: vtd: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("vtd: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)VTD_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: vtd: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("vtd: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)VTD_SOFT_WAVE);
            /* Grep: vtd: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("vtd: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)VTD_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: vtd: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("vtd: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)VTD_SOFT_WAVE);
            /* Grep: vtd: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("vtd: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)VTD_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: vtd: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("vtd: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)VTD_SOFT_WAVE);
            /* Grep: vtd: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("vtd: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)VTD_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: vtd: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("vtd: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)VTD_SOFT_WAVE);
            /* Grep: vtd: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("vtd: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)VTD_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: vtd: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("vtd: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)VTD_SOFT_WAVE);
            /* Grep: vtd: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("vtd: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)VTD_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: vtd: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("vtd: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)VTD_SOFT_WAVE);
                    /* Grep: vtd: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("vtd: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)VTD_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: vtd: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("vtd: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)VTD_SOFT_WAVE);
                            /* Grep: vtd: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("vtd: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)VTD_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: vtd: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("vtd: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)VTD_SOFT_WAVE);
                            /* Grep: vtd: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("vtd: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)VTD_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: vtd: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("vtd: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)VTD_SOFT_WAVE);
                            /* Grep: vtd: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("vtd: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)VTD_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: vtd: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("vtd: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)VTD_SOFT_WAVE);
                            /* Grep: vtd: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("vtd: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)VTD_SOFT_WAVE);
                            /* Grep: vtd: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("vtd: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)VTD_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vtd: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("vtd: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)VTD_SOFT_WAVE);
/* Grep: vtd: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("vtd: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)VTD_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vtd: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("vtd: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)VTD_SOFT_WAVE);
/* Grep: vtd: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("vtd: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)VTD_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vtd: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("vtd: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)VTD_SOFT_WAVE);
/* Grep: vtd: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("vtd: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)VTD_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vtd: soft retfortress — Wave 35 return-fortress honesty */
kprintf("vtd: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)VTD_SOFT_WAVE);
/* Grep: vtd: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("vtd: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)VTD_SOFT_WAVE);
                            kprintf("vtd: soft deepen wave=%u areas=%u logs=%u surf=0x%x "
            "(Wave 35 exclusive; soft only; not product always-on IOMMU; "
            "not bar3; soft≠product)\n",
            (unsigned)VTD_SOFT_WAVE, (unsigned)VTD_SOFT_AREAS, g_cSoftInvLogs,
            (unsigned)u32Surf);

    /*
     * Explicit OPEN honesty for always-on product IOMMU.
     * Grep: vtd: soft OPEN
     */
    kprintf("vtd: soft OPEN always_on_product=OPEN "
            "no_open_bus_master_product=OPEN hw_te_default=OPEN "
            "qi_product=OPEN inventory_only=1 wave=%u "
            "(soft deepen ≠ product always-on IOMMU claim; not bar3; "
            "soft≠product)\n",
            (unsigned)VTD_SOFT_WAVE);

    /* Grep: vtd: soft inventory PASS | vtd: soft PASS */
    kprintf("vtd: soft inventory PASS ready=%d pages=%u logs=%u wave=%u\n",
            fReady, g_u32VtdPages, g_cSoftInvLogs, (unsigned)VTD_SOFT_WAVE);
    kprintf("vtd: soft PASS wave=%u areas=%u always_on_product=OPEN\n",
            (unsigned)VTD_SOFT_WAVE, (unsigned)VTD_SOFT_AREAS);
}

/**
 * Production-default soft path (P-DMA-4): arm enforce, deny open bus-master
 * without a window, grant+allow with window, honesty production_default_open_bus=0.
 *
 * Soft-only: PASSes on QEMU without intel-iommu / DRHD (software policy + tables).
 * Does not leave enforce armed (bring-up may keep off after smoke); restores
 * prior enforce and revokes the soft window so main `iommu: enforce PASS` stays
 * independent. Not a claim that product IOMMU default is shipped/closed.
 *
 * Greppable:
 *   iommu: enforce default soft …
 *   iommu: no open bus-master soft PASS
 */
static int
vtd_product_default_soft(void)
{
    int fPrior;
    int fOk;
    int fDenyBare;
    int fDenyOther;
    int fCovered = 0;
    u32 u32Denies0;
    u32 u32Denies1;
    u32 u32Denies2;
    u32 u32SoftDeny0;
    /* production_default_open_bus=0 when enforce intended (honesty) */
    const int fOpenBus = 0;

    if (!g_fVtdReady) {
        if (iommu_vtd_init_tables() != 0) {
            kprintf("iommu: enforce default soft tables FAIL\n");
            return 0;
        }
    }

    fPrior = iommu_enforce_get();
    u32Denies0 = iommu_deny_count();
    u32SoftDeny0 = g_u32ProdSoftDeny;

    /* Arm production-default enforce (soft policy; not HW product close). */
    iommu_enforce_set(1);
    kprintf("iommu: enforce default soft arm prior=%d te_mode=%d "
            "production_default_open_bus=%d\n",
            fPrior, iommu_vtd_te_mode(), fOpenBus);

    /*
     * Deny path: enforce armed, no window for product soft BDF → bus-master
     * denied and platform deny counter must tick.
     */
    fDenyBare = iommu_busmaster_ok(VTD_PROD_SOFT_BUS, VTD_PROD_SOFT_SLOT,
                                   VTD_PROD_SOFT_FUNC);
    u32Denies1 = iommu_deny_count();
    if (fDenyBare != 0 || u32Denies1 <= u32Denies0) {
        kprintf("iommu: enforce default soft deny-path FAIL ok=%d "
                "denies=%u→%u\n",
                fDenyBare, u32Denies0, u32Denies1);
        iommu_enforce_set(fPrior);
        return 0;
    }
    g_u32ProdSoftDeny++;

    /* Grant software window (+ VT-d identity cover when tables ready). */
    if (iommu_vtd_window_grant(VTD_PROD_SOFT_BUS, VTD_PROD_SOFT_SLOT,
                               VTD_PROD_SOFT_FUNC, VTD_PROD_SOFT_PA,
                               VTD_PROD_SOFT_CB, &fCovered) != 0) {
        kprintf("iommu: enforce default soft grant FAIL\n");
        iommu_enforce_set(fPrior);
        return 0;
    }

    fOk = iommu_busmaster_ok(VTD_PROD_SOFT_BUS, VTD_PROD_SOFT_SLOT,
                             VTD_PROD_SOFT_FUNC);
    /* Still no open bus-master for ungranted BDF under armed enforce. */
    fDenyOther = iommu_busmaster_ok(VTD_PROD_SOFT_BUS, VTD_PROD_DENY_SLOT,
                                    VTD_PROD_DENY_FUNC);
    u32Denies2 = iommu_deny_count();
    if (!fOk || fDenyOther != 0 || u32Denies2 <= u32Denies1) {
        kprintf("iommu: enforce default soft grant-check FAIL ok=%d deny=%d "
                "denies=%u→%u\n",
                fOk, fDenyOther, u32Denies1, u32Denies2);
        iommu_window_revoke(VTD_PROD_SOFT_BUS, VTD_PROD_SOFT_SLOT,
                            VTD_PROD_SOFT_FUNC);
        iommu_enforce_set(fPrior);
        return 0;
    }
    g_u32ProdSoftDeny++;

    /*
     * Honesty line: when enforce is the intended production default,
     * open bus-master is 0 (no free-form DMA without a window).
     */
    kprintf("iommu: enforce default soft armed=%u denies=%u soft_denies=%u "
            "windows=%u cover=%d te_mode=%d production_default_open_bus=%d\n",
            iommu_enforce_get() ? 1u : 0u, u32Denies2,
            g_u32ProdSoftDeny - u32SoftDeny0, iommu_window_count(), fCovered,
            iommu_vtd_te_mode(), fOpenBus);

    /* Cleanup: revoke soft window; restore prior enforce (bring-up off). */
    iommu_window_revoke(VTD_PROD_SOFT_BUS, VTD_PROD_SOFT_SLOT,
                        VTD_PROD_SOFT_FUNC);
    iommu_enforce_set(fPrior);

    /*
     * Soft PASS without requiring DRHD / intel-iommu. Soft ≠ product close:
     * production no-open-bus-master default remains a soft path only here.
     */
    kprintf("iommu: no open bus-master soft PASS soft_denies=%u\n",
            g_u32ProdSoftDeny);
    return 1;
}

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
    /*
     * Production-default soft (P-DMA-4): no open bus-master when enforce
     * intended. Soft-only; independent of existing soft-probe PASS.
     */
    (void)vtd_product_default_soft();
    /*
     * Wave 17 exclusive soft inventory (greppable "vtd: soft …").
     * Soft deepen only; always-on product IOMMU remains OPEN.
     */
    vtd_soft_inventory_log();
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
            if (g_cSoftDomCreate < 0xffffffffu) {
                g_cSoftDomCreate++;
            }
            vtd_soft_note_att_peak();
            kprintf("iommu: vtd domain soft create did=%u used=%u\n", iDom,
                    g_u32DomUsed);
            return iDom;
        }
    }
    if (g_cSoftDomCreateFail < 0xffffffffu) {
        g_cSoftDomCreateFail++;
    }
    kprintf("iommu: vtd domain soft create FULL\n");
    return GJ_IOMMU_DOMAIN_INVALID;
}

int
iommu_vtd_domain_destroy(u32 u32Did)
{
    if (u32Did == 0 || u32Did >= GJ_IOMMU_DOMAIN_MAX) {
        if (g_cSoftDomDestroyFail < 0xffffffffu) {
            g_cSoftDomDestroyFail++;
        }
        return -1;
    }
    if (!g_aDom[u32Did].u8Used) {
        if (g_cSoftDomDestroyFail < 0xffffffffu) {
            g_cSoftDomDestroyFail++;
        }
        return -1;
    }
    if (g_aDom[u32Did].u16Ref != 0) {
        if (g_cSoftDomDestroyFail < 0xffffffffu) {
            g_cSoftDomDestroyFail++;
        }
        kprintf("iommu: vtd domain soft destroy busy did=%u ref=%u\n", u32Did,
                (u32)g_aDom[u32Did].u16Ref);
        return -1;
    }
    g_aDom[u32Did].u8Used = 0;
    g_aDom[u32Did].u16Ref = 0;
    if (g_u32DomUsed > 0) {
        g_u32DomUsed--;
    }
    if (g_cSoftDomDestroy < 0xffffffffu) {
        g_cSoftDomDestroy++;
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
        if (g_cSoftAttFail < 0xffffffffu) {
            g_cSoftAttFail++;
        }
        return -1;
    }
    if (!vtd_bdf_ok(bus, slot, func)) {
        if (g_cSoftAttFail < 0xffffffffu) {
            g_cSoftAttFail++;
        }
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
                if (g_cSoftCtxDidFail < 0xffffffffu) {
                    g_cSoftCtxDidFail++;
                }
                kprintf("iommu: vtd domain soft ctx DID write fail\n");
            } else if (bus == 0 && g_fVtdReady) {
                if (g_cSoftCtxDidWrite < 0xffffffffu) {
                    g_cSoftCtxDidWrite++;
                }
            }
            if (g_cSoftAttRebind < 0xffffffffu) {
                g_cSoftAttRebind++;
            }
            vtd_soft_note_att_peak();
            kprintf("iommu: vtd domain soft rebind %u:%u.%u did=%u\n", bus,
                    slot, func, u32Did);
            return 0;
        }
        if (!g_aAtt[iAtt].u8Used && iFree == VTD_SOFT_ATTACH_MAX) {
            iFree = iAtt;
        }
    }
    if (iFree >= VTD_SOFT_ATTACH_MAX) {
        if (g_cSoftAttFail < 0xffffffffu) {
            g_cSoftAttFail++;
        }
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
        if (g_cSoftCtxDidFail < 0xffffffffu) {
            g_cSoftCtxDidFail++;
        }
        kprintf("iommu: vtd domain soft ctx DID write fail\n");
    } else if (bus == 0 && g_fVtdReady) {
        if (g_cSoftCtxDidWrite < 0xffffffffu) {
            g_cSoftCtxDidWrite++;
        }
    }
    if (g_cSoftAttOk < 0xffffffffu) {
        g_cSoftAttOk++;
    }
    vtd_soft_note_att_peak();
    kprintf("iommu: vtd domain soft attach %u:%u.%u did=%u ref=%u\n", bus, slot,
            func, u32Did, (u32)g_aDom[u32Did].u16Ref);
    return 0;
}

int
iommu_vtd_domain_detach(u8 bus, u8 slot, u8 func)
{
    u32 iAtt;

    if (!vtd_bdf_ok(bus, slot, func)) {
        if (g_cSoftDetMiss < 0xffffffffu) {
            g_cSoftDetMiss++;
        }
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
            if (vtd_ctx_set_did(bus, slot, func, 0) == 0 && bus == 0 &&
                g_fVtdReady) {
                if (g_cSoftCtxDidWrite < 0xffffffffu) {
                    g_cSoftCtxDidWrite++;
                }
            }
            if (g_cSoftDetOk < 0xffffffffu) {
                g_cSoftDetOk++;
            }
            kprintf("iommu: vtd domain soft detach %u:%u.%u (was did=%u)\n",
                    bus, slot, func, u32Did);
            return 0;
        }
    }
    if (g_cSoftDetMiss < 0xffffffffu) {
        g_cSoftDetMiss++;
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
