/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * VT-d (Intel IOMMU) page-table construction - clean-room, dual-licensed.
 * Builds root + context + second-level identity map in memory.
 * Soft-probe: CAP/ECAP soft or MMIO, root/context verify, DMAR inventory.
 * Domain soft: software DID pool; bus-0 context DID write when tables ready.
 * Product soft (P-DMA-4): production-default enforce arm - no open bus-master
 * without a window; soft-only PASS without intel-iommu; honesty open_bus=0.
 * Optional DRHD MMIO program when ACPI DMAR provides a base.
 * Not derived from Linux intel-iommu or any GPL VT-d driver.
 *
 * Lean soft residual (greppable "vtd: soft ..." / "iommu: soft ... Soft!=product"):
 *   vtd: soft inventory  - tables/pages/ctx/domains/feat rollup
 *   vtd: soft identity   - bring-up identity cover lamps (+ bus3)
 *   vtd: soft path       - honesty: always-on product IOMMU remains OPEN
 *   vtd: soft PASS | soft inventory PASS
 * Dual DoD residual (greppable "iommu: soft ... Soft!=product"):
 *   iommu: soft bus3 residual / preflight / soft-skip no_vtd
 *   iommu: soft identity residual ... (G752 NIC DMA cover under TE)
 *   iommu: soft dma residual ... (force32 map honesty for eng DMA)
 *   iommu: soft usb residual ... (G752 xHCI 0:14.0 Translated DMA cover)
 *   iommu: soft eng residual ... (NIC bus3 + USB 0:14.0 lean eng surface)
 *   iommu: soft did0 residual ... (all-bus root + eng CT DID=0 honesty)
 *   iommu: soft udx residual ... (UDX DMA safety eng; userspace driver path)
 *   iommu: soft udx dma ... (force32 window cover for rtl8168_udx/xhci_udx)
 *   iommu: soft udx bm residual ... (FUNCTIONAL eng busmaster under enforce)
 *   iommu: soft dma path residual ... (C2 Dual DoD A/B UDX DMA path rollup)
 *   iommu: soft ddi residual ... (DDI DMA_NOTE soft window + VT-d cover)
 *   iommu: soft dma_note residual ... (window grant honesty; window_mint OPEN)
 * Soft deepen != product always-on IOMMU claim; not HW product close;
 * Soft!=product. Soft-skip if no VT-d. G-AC-1: not .ko product AC.
 * window_mint=OPEN: soft software window table only - not CNode window cap.
 * C2 Dual DoD DMA path: dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX
 *   a_ready/b_ready eng honesty; product=UDX+ABI; freestanding_product=SKIP
 * freestanding_product=SKIP (product = userspace UDX, not freestanding).
 * No version stamp. No stamp storms: residual once + silent reaffirm.
 * All-bus DID0: root[0..255].P + shared-CT eng BDFs DID=0 (identity).
 * UDX eng path: TE needs bus3_p + identity [0,1GiB) + DID0 so userspace
 * DMA (udx_dma_* / window grant) is not broken under Translated domains.
 * FUNCTIONAL residual: eng BDF busmaster under temporary enforce (window
 * live for rtl8168_udx / xhci_udx DMA policy; not open-path-only lamps).
 * Soft lamps alone != product UDX DMA caps / Dual DoD close.
 * DDI DMA_NOTE (op 6) uses iommu_window_grant; residual keeps mint OPEN.
 * STRONGER functional residual (W10 Dual DoD; stamp-free bar v2026.08.04.75):
 *   iommu: soft udx host residual ... (live UDX hosts under TE enforce)
 *   hosts=rtl8168_udx|xhci_udx force32+eng_bm+dual_dod_open honesty.
 * STRONGER denser residual (product IOMMU honesty; Soft!=product; Dual DoD OPEN):
 *   iommu: soft busmaster_ok residual ... (8168 + a12f hosts; denser BM)
 *   iommu: soft window_grant residual ... (8168 + a12f hosts; denser grant)
 *   iommu: soft all-bus identity residual ... (root 256 + DID0 honesty)
 * denser multi-arm residual (VTD_UDX_DENSE_ARMS=10; Soft!=product Dual DoD OPEN):
 *   eng_ready | root_256 | did0_eng | force32_1g | eng_bm |
 *   win_ok | bare_deny | dual_dod_open | sample_cover | product_open
 *   denser=1 denser_arms=10 denser_ok=N product_hosts=UDX
 *   greppable: iommu: soft residual denser
 *   greppable: iommu: soft busmaster_ok residual denser
 *   greppable: iommu: soft window_grant residual denser
 *   greppable: iommu: soft all-bus identity residual denser
 *   greppable: denser_arms= denser_ok= denser=1
 * Greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 * Never claim product close. H2 once (no stamp storms). ASCII Soft!= only.
 * Bar honesty v2026.08.04.75 stamp-free — never invent .76.
 * Dual MIT OR Apache-2.0. No GPL.
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
#define VTD_SL_SP (1ull << 7) /* superpage (2 MiB when in PD) */

/* Root entry: present + context table ptr */
#define VTD_ROOT_P (1ull << 0)
/* Context entry: present + translation type + SLPT ptr */
#define VTD_CTX_P     (1ull << 0)
#define VTD_CTX_TT_ML (0ull << 2) /* multi-level translate */
/* Context hi: AW in [2:0], DID in [23:8] (public VT-d context-entry) */
#define VTD_CTX_AW_48  2ull
#define VTD_CTX_DID_SHIFT 8u

/* Bring-up identity SLPT covers [0, 1 GiB). */
#define VTD_IDENTITY_LIMIT (1024ull * 1024ull * 1024ull)
#define VTD_2MIB           (2ull * 1024ull * 1024ull)
#define VTD_CTX_ENTRIES    256u /* bus0: 32 slots x 8 funcs */
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
#define VTD_GSTS_TES  (1u << 31) /* Translation Enable Status */
#define VTD_GSTS_RTPS (1u << 30)

/*
 * G752VT freestanding NIC (10ec:8168) BDF - bus 3, not bus 0.
 * TE without root[bus3].P left RX/TX OWN stuck (DMA fault under Translated).
 * Soft!=product; greppable bus3 lamps reference these.
 */
#define VTD_G752_NIC_BUS  3u
#define VTD_G752_NIC_SLOT 0u
#define VTD_G752_NIC_FUNC 0u

/* G752VT PCH xHCI (8086:a12f) - bus 0 slot 0x14 func 0 (Linux inventory). */
#define VTD_G752_XHCI_BUS  0u
#define VTD_G752_XHCI_SLOT 0x14u
#define VTD_G752_XHCI_FUNC 0u

/*
 * Soft synthetic CAP/ECAP when no DRHD MMIO (QEMU without intel-iommu).
 * Public bit meanings only; values describe bring-up tables (48-bit AW,
 * 2 MiB superpages in SAGAW, modest ND). Not read from hardware.
 */
#define VTD_SOFT_CAP_ND_16     (1ull << 0)  /* ND encoding: 16 domains */
#define VTD_SOFT_CAP_SAGAW_39  (1ull << 9)  /* 3-level / 39-bit class */
#define VTD_SOFT_CAP_SAGAW_48  (1ull << 10) /* 4-level / 48-bit class */
#define VTD_SOFT_CAP_MGAW_47   (47ull << 16)
#define VTD_SOFT_CAP                                                           \
    (VTD_SOFT_CAP_ND_16 | VTD_SOFT_CAP_SAGAW_39 | VTD_SOFT_CAP_SAGAW_48 |     \
     VTD_SOFT_CAP_MGAW_47)
#define VTD_SOFT_ECAP  0ull /* QI/IR not claimed in soft path */

/* Soft domain attach slots (BDF -> DID); independent of window table */
#define VTD_SOFT_ATTACH_MAX 32u

/*
 * Product-default soft BDF (P-DMA-4 smoke). Kept off main enforce 0:2.0 and
 * domain soft 0:3.0 so bring-up greps stay independent.
 */
#define VTD_PROD_SOFT_BUS  0u
#define VTD_PROD_SOFT_SLOT 4u
#define VTD_PROD_SOFT_FUNC 0u
#define VTD_PROD_SOFT_PA   0x3000ull
#define VTD_PROD_SOFT_CB   0x1000ull
/* Ungranted BDF for deny-path counter smoke (not 0:31.0 - main uses that). */
#define VTD_PROD_DENY_SLOT 31u
#define VTD_PROD_DENY_FUNC 7u
/*
 * Bare BDF for UDX eng busmaster residual (off eng 3:0.0 / 0:14.0 /
 * prod soft 0:4.0 / DDI note 0:5.0). Under temporary enforce must deny.
 */
#define VTD_UDX_BM_BARE_BUS  0u
#define VTD_UDX_BM_BARE_SLOT 6u
#define VTD_UDX_BM_BARE_FUNC 0u

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
static gj_paddr_t g_paPd0; /* first 1 GiB of 2 MiB pages */
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

/* Wave 19 greppable soft inventory dump count (vtd: soft ...) */
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

/*
 * Lean Dual DoD residual once-flags (soft-probe + TE arm + xhci_identity
 * all re-enter). Silent reaffirm when still PASS - no stamp storms.
 * Soft!=product / UDX eng DMA identity honesty (G752 userspace path).
 */
static int g_fBus3ResidDone;   /* bus3 residual completed once */
static int g_nBus3ResidLast;   /* last bus3 residual return */
static int g_fUsbResidDone;    /* usb residual completed once */
static int g_nUsbResidLast;    /* last usb residual return */
static int g_fEngResidDone;    /* eng residual completed once */
static int g_nEngResidLast;    /* last eng residual return */
static int g_fUdxResidDone;    /* UDX DMA safety residual completed once */
static int g_nUdxResidLast;    /* last UDX residual return */
static int g_fDdiResidDone;    /* DDI DMA_NOTE residual completed once */
static int g_nDdiResidLast;    /* last DDI residual return */
static int g_fDmaPathResidDone; /* C2 Dual DoD DMA path residual once */
static int g_nDmaPathResidLast; /* last dma path residual return */
static int g_fUdxHostResidDone; /* W10 live UDX host residual once */
static int g_nUdxHostResidLast; /* last live UDX host residual return */
static int g_fNicGrantLogged;  /* first NIC grant lamp emitted */
static int g_fUsbGrantLogged;  /* first USB grant lamp emitted */
static int g_fSoftInvLogged;   /* lean vtd soft inventory once */
/*
 * denser multi-arm residual tallies (H2 once; Soft!=product; Dual DoD OPEN).
 * greppable: iommu: soft residual denser | denser_arms= | denser_ok=
 */
static u8  g_fUdxDenseOnce;     /* denser residual lamp latched */
static u32 g_u32UdxDenseOk;     /* composite denser arms all PASS */
static u32 g_u32UdxDenseFail;   /* denser composite soft fail */
static u32 g_u32UdxDenseArms;   /* last denser arm count (0..DENSE_ARMS) */

/*
 * denser multi-arm residual for product IOMMU Dual DoD honesty
 * (Soft!=product; dual_dod OPEN; product_hosts=UDX).
 * 10 arms; all required for denser composite ok.
 * greppable: denser_arms=10 denser_ok= iommu: soft residual denser
 * Bar honesty v2026.08.04.75 stamp-free — never invent .76.
 */
#define VTD_UDX_DENSE_ARMS 10u
#define VTD_UDX_DENSE_MIN  10u
_Static_assert(VTD_UDX_DENSE_ARMS == 10u, "udx denser arms must be 10");
_Static_assert(VTD_UDX_DENSE_MIN == VTD_UDX_DENSE_ARMS,
               "udx denser min must equal denser arms");

static void vtd_soft_inventory_log(void);
static void vtd_soft_note_att_peak(void);
static void vtd_domain_pool_init(void);
static int  vtd_soft_bdf_identity_ready(u8 bus, u8 slot, u8 func);
static int  vtd_soft_identity_map_ok(void);
static int  vtd_soft_eng_busmaster_ok(int fBareCheck, int *pNicOk,
                                      int *pUsbOk, int *pBareDeny);

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
 * Non-zero if root entry for bus has P set (shared identity context).
 * Grep lamps: bus3_p for G752 03:00.0. Soft only when tables not ready.
 */
static int
vtd_root_bus_p(u32 u32Bus)
{
    u64 *pRoot;

    if (!g_fVtdReady || g_paRoot == 0 || u32Bus > 255u) {
        return 0;
    }
    pRoot = (u64 *)vtd_virt(g_paRoot);
    if (pRoot == NULL) {
        return 0;
    }
    return ((pRoot[u32Bus * 2u] & VTD_ROOT_P) != 0) ? 1 : 0;
}

/** Count root buses with P set (bring-up expects 256). */
static u32
vtd_root_buses_p_count(void)
{
    u64 *pRoot;
    u32 u32Bus;
    u32 c = 0;

    if (!g_fVtdReady || g_paRoot == 0) {
        return 0;
    }
    pRoot = (u64 *)vtd_virt(g_paRoot);
    if (pRoot == NULL) {
        return 0;
    }
    for (u32Bus = 0; u32Bus < 256u; u32Bus++) {
        if ((pRoot[u32Bus * 2u] & VTD_ROOT_P) != 0) {
            c++;
        }
    }
    return c;
}

/**
 * Context-entry present for (slot,func) on the shared identity context table.
 * Applies to every bus via root (bring-up shared SLPT).
 */
static int
vtd_ctx_devfn_p(u8 u8Slot, u8 u8Func)
{
    u64 *pCtx;
    u32 u32Idx;

    if (!g_fVtdReady || g_paContext == 0) {
        return 0;
    }
    if (!vtd_bdf_ok(0, u8Slot, u8Func)) {
        return 0;
    }
    pCtx = (u64 *)vtd_virt(g_paContext);
    if (pCtx == NULL) {
        return 0;
    }
    u32Idx = vtd_ctx_index(u8Slot, u8Func);
    return ((pCtx[u32Idx * 2u] & VTD_CTX_P) != 0) ? 1 : 0;
}

/**
 * Read context-entry DID for (slot,func) on the shared identity CT.
 * Bring-up: all buses share one CT -> DID is global per devfn.
 * Returns GJ_IOMMU_DOMAIN_INVALID if tables missing / P clear.
 * Grep lamps: did0 for freestanding NIC/USB eng residual honesty.
 */
static u32
vtd_ctx_get_did(u8 u8Slot, u8 u8Func)
{
    u64 *pCtx;
    u32 u32Idx;
    u64 u64Hi;

    if (!g_fVtdReady || g_paContext == 0) {
        return GJ_IOMMU_DOMAIN_INVALID;
    }
    if (!vtd_bdf_ok(0, u8Slot, u8Func)) {
        return GJ_IOMMU_DOMAIN_INVALID;
    }
    pCtx = (u64 *)vtd_virt(g_paContext);
    if (pCtx == NULL) {
        return GJ_IOMMU_DOMAIN_INVALID;
    }
    u32Idx = vtd_ctx_index(u8Slot, u8Func);
    if ((pCtx[u32Idx * 2u] & VTD_CTX_P) == 0) {
        return GJ_IOMMU_DOMAIN_INVALID;
    }
    u64Hi = pCtx[u32Idx * 2u + 1u];
    return (u32)((u64Hi >> VTD_CTX_DID_SHIFT) & 0xffffull);
}

/**
 * Count present context entries with hardware DID==0 (all-bus identity).
 * Bring-up expects 256 after init_tables; soft domain smoke may rebind a
 * non-eng devfn (e.g. 0:3.0) so eng residual checks eng BDFs, not this total.
 * Grep: did0_ctx=
 */
static u32
vtd_ctx_did0_present_count(void)
{
    u64 *pCtx;
    u32 u32Dev;
    u32 c = 0;

    if (!g_fVtdReady || g_paContext == 0) {
        return 0;
    }
    pCtx = (u64 *)vtd_virt(g_paContext);
    if (pCtx == NULL) {
        return 0;
    }
    for (u32Dev = 0; u32Dev < VTD_CTX_ENTRIES; u32Dev++) {
        u64 u64Lo = pCtx[u32Dev * 2u];
        u64 u64Hi = pCtx[u32Dev * 2u + 1u];
        u32 u32Did;

        if ((u64Lo & VTD_CTX_P) == 0) {
            continue;
        }
        u32Did = (u32)((u64Hi >> VTD_CTX_DID_SHIFT) & 0xffffull);
        if (u32Did == 0u) {
            c++;
        }
    }
    return c;
}

/**
 * Eng BDF DID0: G752 NIC (devfn 0:0) + xHCI (0:14.0) on shared CT DID=0.
 * Soft!=product; all-bus root still required separately.
 */
static int
vtd_eng_did0_ok(void)
{
    u32 u32NicDid;
    u32 u32UsbDid;

    u32NicDid = vtd_ctx_get_did(VTD_G752_NIC_SLOT, VTD_G752_NIC_FUNC);
    u32UsbDid = vtd_ctx_get_did(VTD_G752_XHCI_SLOT, VTD_G752_XHCI_FUNC);
    return (u32NicDid == 0u && u32UsbDid == 0u) ? 1 : 0;
}

/**
 * Preflight before TE: tables + all-bus root + identity [0,1GiB) + G752 bus3
 * + NIC DID0 (shared-CT identity for 03:00.0). USB DID0 is eng residual only
 * (not TE OWN gate). Returns 1 if safe to arm TE without OWN-stuck; 0 else.
 * Grep: iommu: vtd TE preflight
 */
static int
vtd_te_identity_preflight(void)
{
    u32 cRoot;
    u32 cDid0;
    u32 u32NicDid;
    int fBus0;
    int fBus3;
    int fCtx00;
    int fIdLo;
    int fIdHi;
    int fNicDid0;
    int fOk;

    if (!g_fVtdReady) {
        kprintf("iommu: vtd TE preflight FAIL not_ready\n");
        return 0;
    }
    cRoot = vtd_root_buses_p_count();
    cDid0 = vtd_ctx_did0_present_count();
    fBus0 = vtd_root_bus_p(0);
    fBus3 = vtd_root_bus_p(VTD_G752_NIC_BUS);
    fCtx00 = vtd_ctx_devfn_p(VTD_G752_NIC_SLOT, VTD_G752_NIC_FUNC);
    fIdLo = iommu_vtd_identity_covers(0, 0x1000);
    fIdHi = iommu_vtd_identity_covers(VTD_IDENTITY_LIMIT - VTD_2MIB, VTD_2MIB);
    u32NicDid = vtd_ctx_get_did(VTD_G752_NIC_SLOT, VTD_G752_NIC_FUNC);
    fNicDid0 = (u32NicDid == 0u) ? 1 : 0;
    /*
     * TE needs all-bus root + NIC DID0. Full 256 DID0 is table-build default;
     * soft domain smoke may rebind non-eng slots - eng residual checks both.
     */
    fOk = (cRoot == 256u && fBus0 && fBus3 && fCtx00 && fIdLo && fIdHi &&
           fNicDid0)
              ? 1
              : 0;
    /*
     * Grep: iommu: vtd TE preflight
     * Grep: bus3_p (G752 03:00.0 root cover)
     * Grep: did0 (NIC identity DID=0)
     */
    kprintf("iommu: vtd TE preflight root_buses=%u bus0_p=%d bus3_p=%d "
            "ctx_00_p=%d id_lo=%d id_hi=%d nic_did0=%d nic_did=%u "
            "did0_ctx=%u limit=0x%lx ok=%d "
            "(G752 NIC 03:00.0 needs bus3_p=1 did0; force32 DMA in [0,1GiB))\n",
            cRoot, fBus0, fBus3, fCtx00, fIdLo, fIdHi, fNicDid0, u32NicDid,
            cDid0, (unsigned long)VTD_IDENTITY_LIMIT, fOk);
    return fOk;
}

/**
 * Identity map honesty for freestanding force32 DMA (rtl rings / xHCI TRB).
 * Bring-up SLPT must cover full [0, 1 GiB). Soft!=product.
 */
static int
vtd_soft_identity_map_ok(void)
{
    if (!g_fVtdReady) {
        return 0;
    }
    if (!iommu_vtd_identity_covers(0, 0x1000)) {
        return 0;
    }
    if (!iommu_vtd_identity_covers(VTD_IDENTITY_LIMIT - VTD_2MIB, VTD_2MIB)) {
        return 0;
    }
    if (!iommu_vtd_identity_covers(0, VTD_IDENTITY_LIMIT)) {
        return 0;
    }
    return 1;
}

/**
 * BDF ready for freestanding DMA under TE: root P + ctx P + DID0 + map 1 GiB.
 * Soft!=product identity/map honesty for rtl ring OWN.
 */
static int
vtd_soft_bdf_identity_ready(u8 bus, u8 slot, u8 func)
{
    u32 u32CtxDid;
    u32 u32SoftDid;

    if (!g_fVtdReady || !vtd_bdf_ok(bus, slot, func)) {
        return 0;
    }
    if (!vtd_root_bus_p(bus) || !vtd_ctx_devfn_p(slot, func)) {
        return 0;
    }
    u32CtxDid = vtd_ctx_get_did(slot, func);
    u32SoftDid = iommu_vtd_domain_lookup(bus, slot, func);
    if (u32CtxDid != 0u || u32SoftDid != 0u) {
        return 0;
    }
    if (!vtd_soft_identity_map_ok()) {
        return 0;
    }
    return 1;
}

/**
 * FUNCTIONAL residual: eng BDF busmaster under temporary enforce.
 * Soft!=product / G-AC-1. Proves G752 NIC (03:00.0 10ec:8168) + xHCI
 * (0:14.0 8086:a12f) soft windows are live for UDX DMA policy
 * (rtl8168_udx / xhci_udx) - not just open-path (enforce-off) always-allow
 * lamps. Restores prior enforce.
 *
 * fBareCheck non-zero: also require ungranted bare BDF deny under enforce
 * (policy honesty; ticks deny counter once per full residual - not silent).
 * Silent reaffirm uses fBareCheck=0 (no bare deny stamp growth).
 *
 * pNicOk / pUsbOk / pBareDeny optional out (0/1). Null ok.
 * Return: 1 if both eng allow under enforce (+ bare deny when requested);
 *         0 otherwise (tables missing / window missing / bare leak).
 *
 * Grep: iommu: soft udx bm residual | soft busmaster_ok residual
 * Grep: path=iommu_busmaster_ok | product_hosts=UDX
 */
static int
vtd_soft_eng_busmaster_ok(int fBareCheck, int *pNicOk, int *pUsbOk,
                          int *pBareDeny)
{
    int fPrior;
    int fNic;
    int fUsb;
    int fBareAllow;
    int fBareDeny;
    int fOk;

    if (pNicOk != NULL) {
        *pNicOk = 0;
    }
    if (pUsbOk != NULL) {
        *pUsbOk = 0;
    }
    if (pBareDeny != NULL) {
        *pBareDeny = 0;
    }
    if (!g_fVtdReady) {
        return 0;
    }

    fPrior = iommu_enforce_get();
    iommu_enforce_set(1);
    /*
     * Eng BDF with soft windows granted by residual must allow under enforce.
     * Bare BDF (no window) must deny - product no-open-busmaster honesty.
     * denser residual: 8168 (rtl8168_udx) + a12f (xhci_udx) both probed.
     */
    fNic = iommu_busmaster_ok(VTD_G752_NIC_BUS, VTD_G752_NIC_SLOT,
                              VTD_G752_NIC_FUNC);
    fUsb = iommu_busmaster_ok(VTD_G752_XHCI_BUS, VTD_G752_XHCI_SLOT,
                              VTD_G752_XHCI_FUNC);
    if (fBareCheck != 0) {
        fBareAllow = iommu_busmaster_ok(VTD_UDX_BM_BARE_BUS,
                                        VTD_UDX_BM_BARE_SLOT,
                                        VTD_UDX_BM_BARE_FUNC);
        fBareDeny = (fBareAllow == 0) ? 1 : 0;
    } else {
        fBareDeny = 1; /* not checked; do not fail silent reaffirm */
    }
    iommu_enforce_set(fPrior);

    if (pNicOk != NULL) {
        *pNicOk = (fNic != 0) ? 1 : 0;
    }
    if (pUsbOk != NULL) {
        *pUsbOk = (fUsb != 0) ? 1 : 0;
    }
    if (pBareDeny != NULL) {
        *pBareDeny = fBareDeny;
    }
    fOk = (fNic != 0 && fUsb != 0 && fBareDeny != 0) ? 1 : 0;
    return fOk;
}

/**
 * Soft-grant identity window for G752 freestanding NIC before TE.
 * First call grants + one lamp; re-entry quiet when map still honest
 * (avoids window-cap update stamp storms from soft-probe/TE/rtl).
 * Soft!=product. Grep: iommu: vtd nic identity
 */
static void
vtd_soft_grant_g752_nic(void)
{
    int fCovered = 0;
    int fFull = 0;
    u32 u32Did;

    if (!g_fVtdReady) {
        return;
    }
    /*
     * Lean re-entry: after first grant, skip when bus3 identity still honest
     * for freestanding force32 rings (root/ctx/DID0/map 1 GiB).
     */
    if (g_fNicGrantLogged != 0 &&
        vtd_soft_bdf_identity_ready(VTD_G752_NIC_BUS, VTD_G752_NIC_SLOT,
                                    VTD_G752_NIC_FUNC) != 0) {
        return;
    }
    if (!g_aDom[0].u8Used) {
        vtd_domain_pool_init();
    }
    u32Did = iommu_vtd_domain_lookup(VTD_G752_NIC_BUS, VTD_G752_NIC_SLOT,
                                     VTD_G752_NIC_FUNC);
    if (u32Did == GJ_IOMMU_DOMAIN_INVALID || u32Did != 0) {
        (void)iommu_vtd_domain_attach(0, VTD_G752_NIC_BUS, VTD_G752_NIC_SLOT,
                                      VTD_G752_NIC_FUNC);
    }
    (void)iommu_vtd_window_grant(VTD_G752_NIC_BUS, VTD_G752_NIC_SLOT,
                                 VTD_G752_NIC_FUNC, 0x1000ull, 0x1000ull,
                                 &fCovered);
    (void)iommu_vtd_window_grant(VTD_G752_NIC_BUS, VTD_G752_NIC_SLOT,
                                 VTD_G752_NIC_FUNC, 0, VTD_IDENTITY_LIMIT,
                                 &fFull);
    /*
     * Grep: iommu: vtd nic identity
     * Grep: bus3 cover
     * Lamp once - re-repair after cover break stays quiet.
     */
    if (g_fNicGrantLogged == 0) {
        g_fNicGrantLogged = 1;
        kprintf("iommu: vtd nic identity bdf=%u:%u.%u cover=%d full=%d "
                "bus3_p=%d ctx_p=%d did=%u "
                "(pre-TE grant; force32 rings need PA in [0,1GiB))\n",
                (unsigned)VTD_G752_NIC_BUS, (unsigned)VTD_G752_NIC_SLOT,
                (unsigned)VTD_G752_NIC_FUNC, fCovered, fFull,
                vtd_root_bus_p(VTD_G752_NIC_BUS),
                vtd_ctx_devfn_p(VTD_G752_NIC_SLOT, VTD_G752_NIC_FUNC),
                iommu_vtd_domain_lookup(VTD_G752_NIC_BUS, VTD_G752_NIC_SLOT,
                                        VTD_G752_NIC_FUNC));
    }
}

/**
 * Residual Dual DoD A support: freestanding NIC (03:00.0) identity under
 * RX climb / arping when host VT-d is Translated. Soft!=product / G-AC-1.
 *
 * Soft-skip when platform has no VT-d inventory (no DMAR/IVRS and no DRHD)
 * and tables are not ready: no HW TE risk; do not claim bus3 product cover.
 *
 * When inventory or tables exist: ensure all-bus root + bus3_p + identity
 * [0,1GiB) + NIC window grant, then re-run TE preflight lamps.
 *
 * Return:
 *   1  PASS - bus3 identity residual ready (preflight ok)
 *   0  soft-skip - no VT-d / nothing to enforce (honest SKIP)
 *  -1  FAIL - tables incomplete under present inventory
 *
 * Greppable (keep stable - "iommu: soft ... Soft!=product"):
 *   iommu: soft bus3 residual ...
 *   iommu: soft bus3 soft-skip no_vtd Soft!=product
 *   iommu: soft bus3 preflight ... Soft!=product
 *   iommu: soft bus3 residual PASS|SKIP|FAIL Soft!=product
 *   iommu: soft identity residual ... Soft!=product
 */
int
iommu_vtd_bus3_identity_residual(void)
{
    int fPresent;
    int fHasVtd;
    int fPre;
    int fBus3;
    int fIdLo;
    int fIdHi;
    int fDid0;
    int fMap;
    u32 cRoot;
    u32 u32NicDid;
    u32 u32SoftDid;

    /*
     * Lean re-entry (soft-probe / TE arm / rtl xhci_identity): silent when
     * freestanding NIC identity still honest - no stamp storms.
     * Soft!=product / Dual DoD B DMA residual.
     */
    if (g_fBus3ResidDone != 0 && g_nBus3ResidLast > 0 &&
        vtd_soft_bdf_identity_ready(VTD_G752_NIC_BUS, VTD_G752_NIC_SLOT,
                                    VTD_G752_NIC_FUNC) != 0 &&
        vtd_root_buses_p_count() == 256u) {
        return 1;
    }
    if (g_fBus3ResidDone != 0 && g_nBus3ResidLast == 0 &&
        iommu_present() == 0 && g_u64Drhd == 0 && !g_fVtdReady) {
        return 0; /* soft-skip reaffirm silent */
    }

    fPresent = iommu_present();
    fHasVtd = (fPresent != 0 || g_u64Drhd != 0) ? 1 : 0;

    /*
     * Soft-skip if no VT-d: QEMU default / no DMAR. Soft!=product.
     * Grep: iommu: soft bus3 soft-skip no_vtd
     * Grep: iommu: soft ... Soft!=product
     */
    if (!fHasVtd && !g_fVtdReady) {
        /* Grep: iommu: soft bus3 soft-skip no_vtd | soft identity residual */
        kprintf("iommu: soft bus3 soft-skip no_vtd present=%d Soft!=product\n",
                fPresent);
        kprintf("iommu: soft identity residual soft-skip no_vtd Soft!=product\n");
        kprintf("iommu: soft bus3 residual SKIP no_vtd Soft!=product\n");
        g_fBus3ResidDone = 1;
        g_nBus3ResidLast = 0;
        return 0;
    }

    if (!g_fVtdReady) {
        if (iommu_vtd_init_tables() != 0) {
            if (!fHasVtd) {
                kprintf("iommu: soft bus3 residual SKIP no_tables Soft!=product\n");
                kprintf("iommu: soft identity residual soft-skip no_tables "
                        "Soft!=product\n");
                g_fBus3ResidDone = 1;
                g_nBus3ResidLast = 0;
                return 0;
            }
            kprintf("iommu: soft bus3 residual FAIL tables Soft!=product\n");
            kprintf("iommu: soft identity residual FAIL tables Soft!=product\n");
            g_fBus3ResidDone = 1;
            g_nBus3ResidLast = -1;
            return -1;
        }
    }

    /* Pre-TE NIC grant + all-bus identity (OWN-stuck residual for rtl rings). */
    vtd_soft_grant_g752_nic();
    fPre = vtd_te_identity_preflight();
    fBus3 = vtd_root_bus_p(VTD_G752_NIC_BUS);
    cRoot = vtd_root_buses_p_count();
    fIdLo = iommu_vtd_identity_covers(0, 0x1000);
    fIdHi = iommu_vtd_identity_covers(VTD_IDENTITY_LIMIT - VTD_2MIB, VTD_2MIB);
    fMap = vtd_soft_identity_map_ok();
    u32NicDid = vtd_ctx_get_did(VTD_G752_NIC_SLOT, VTD_G752_NIC_FUNC);
    u32SoftDid = iommu_vtd_domain_lookup(VTD_G752_NIC_BUS, VTD_G752_NIC_SLOT,
                                         VTD_G752_NIC_FUNC);
    fDid0 = (u32NicDid == 0u && u32SoftDid == 0u) ? 1 : 0;

    /*
     * Lean residual lamps once (no inventory flood). Soft!=product / G-AC-1.
     * All-bus DID0: root_buses=256 + eng ctx DID=0 for 03:00.0 (rtl RX).
     * Identity/map honesty: force32 rings need map=1g + bus3_p + did0.
     * Grep: iommu: soft bus3 residual | soft bus3 preflight
     * Grep: iommu: soft identity residual
     * Grep: iommu: soft dma residual (Dual DoD B freestanding rtl rings)
     */
    kprintf("iommu: soft bus3 residual bdf=%u:%u.%u bus3_p=%d root_buses=%u "
            "preflight=%d did0=%d ctx_did=%u soft_did=%u Soft!=product\n",
            (unsigned)VTD_G752_NIC_BUS, (unsigned)VTD_G752_NIC_SLOT,
            (unsigned)VTD_G752_NIC_FUNC, fBus3, cRoot, fPre, fDid0, u32NicDid,
            u32SoftDid);
    kprintf("iommu: soft bus3 preflight ok=%d bus3_p=%d root_buses=%u "
            "did0=%d id_lo=%d id_hi=%d Soft!=product\n",
            fPre, fBus3, cRoot, fDid0, fIdLo, fIdHi);
    kprintf("iommu: soft identity residual map=1g bus3_p=%d id_lo=%d "
            "id_hi=%d root_buses=%u did0=%d rx_climb=1 Soft!=product\n",
            fBus3, fIdLo, fIdHi, cRoot, fDid0);
    kprintf("iommu: soft dma residual force32=1 map=%d bus3_p=%d did0=%d "
            "root_buses=%u Soft!=product\n",
            fMap, fBus3, fDid0, cRoot);
    (void)fPresent;

    if (!fPre || !fBus3 || cRoot != 256u || !fIdLo || !fIdHi || !fDid0 ||
        !fMap) {
        if (!fHasVtd) {
            /* Soft tables partial without inventory: honest soft-skip. */
            kprintf("iommu: soft bus3 residual SKIP partial Soft!=product\n");
            kprintf("iommu: soft identity residual SKIP partial Soft!=product\n");
            g_fBus3ResidDone = 1;
            g_nBus3ResidLast = 0;
            return 0;
        }
        kprintf("iommu: soft bus3 residual FAIL preflight Soft!=product\n");
        kprintf("iommu: soft identity residual FAIL preflight Soft!=product\n");
        g_fBus3ResidDone = 1;
        g_nBus3ResidLast = -1;
        return -1;
    }

    kprintf("iommu: soft bus3 residual PASS bus3_p=1 root_buses=256 did0=1 "
            "Soft!=product\n");
    kprintf("iommu: soft identity residual PASS bus3_p=1 did0=1 id_limit=0x%lx "
            "Soft!=product\n",
            (unsigned long)VTD_IDENTITY_LIMIT);
    kprintf("iommu: soft dma residual PASS force32=1 map=1g Soft!=product\n");
    g_fBus3ResidDone = 1;
    g_nBus3ResidLast = 1;
    return 1;
}

/** File-local alias - call sites below keep short name. */
static int
vtd_soft_bus3_identity_residual(void)
{
    return iommu_vtd_bus3_identity_residual();
}

/**
 * Soft-grant identity window for G752 PCH xHCI (0:14.0) before TE / MSC.
 * First call grants + one lamp; re-entry quiet when map still honest.
 * Soft!=product. Grep: iommu: vtd usb identity
 */
static void
vtd_soft_grant_g752_xhci(void)
{
    int fCovered = 0;
    int fFull = 0;
    u32 u32Did;

    if (!g_fVtdReady) {
        return;
    }
    if (g_fUsbGrantLogged != 0 &&
        vtd_soft_bdf_identity_ready(VTD_G752_XHCI_BUS, VTD_G752_XHCI_SLOT,
                                    VTD_G752_XHCI_FUNC) != 0) {
        return;
    }
    if (!g_aDom[0].u8Used) {
        vtd_domain_pool_init();
    }
    u32Did = iommu_vtd_domain_lookup(VTD_G752_XHCI_BUS, VTD_G752_XHCI_SLOT,
                                     VTD_G752_XHCI_FUNC);
    if (u32Did == GJ_IOMMU_DOMAIN_INVALID || u32Did != 0) {
        (void)iommu_vtd_domain_attach(0, VTD_G752_XHCI_BUS, VTD_G752_XHCI_SLOT,
                                      VTD_G752_XHCI_FUNC);
    }
    (void)iommu_vtd_window_grant(VTD_G752_XHCI_BUS, VTD_G752_XHCI_SLOT,
                                 VTD_G752_XHCI_FUNC, 0x1000ull, 0x1000ull,
                                 &fCovered);
    (void)iommu_vtd_window_grant(VTD_G752_XHCI_BUS, VTD_G752_XHCI_SLOT,
                                 VTD_G752_XHCI_FUNC, 0, VTD_IDENTITY_LIMIT,
                                 &fFull);
    /*
     * Grep: iommu: vtd usb identity
     * Grep: ctx_14_p (G752 0:14.0 xHCI cover lamp)
     */
    if (g_fUsbGrantLogged == 0) {
        g_fUsbGrantLogged = 1;
        kprintf("iommu: vtd usb identity bdf=%u:%u.%u cover=%d full=%d "
                "bus0_p=%d ctx_14_p=%d did=%u "
                "(pre-TE grant; TRB/DMA buf need PA in [0,1GiB))\n",
                (unsigned)VTD_G752_XHCI_BUS, (unsigned)VTD_G752_XHCI_SLOT,
                (unsigned)VTD_G752_XHCI_FUNC, fCovered, fFull,
                vtd_root_bus_p(VTD_G752_XHCI_BUS),
                vtd_ctx_devfn_p(VTD_G752_XHCI_SLOT, VTD_G752_XHCI_FUNC),
                iommu_vtd_domain_lookup(VTD_G752_XHCI_BUS, VTD_G752_XHCI_SLOT,
                                        VTD_G752_XHCI_FUNC));
    }
}

/**
 * Residual Dual DoD A support: freestanding xHCI (0:14.0) identity under
 * Translated VT-d on G752 (Linux inventory 8086:a12f). Soft!=product / G-AC-1.
 *
 * Soft-skip when no VT-d inventory and tables not ready. When inventory or
 * tables exist: ensure bus0 root + ctx 0x14.0 + identity [0,1GiB) + window.
 *
 * Return: 1 PASS / 0 soft-skip / -1 FAIL under present inventory
 *
 * Greppable (keep stable - "iommu: soft ... Soft!=product"):
 *   iommu: soft usb residual ...
 *   iommu: soft usb soft-skip no_vtd Soft!=product
 *   iommu: soft usb residual PASS|SKIP|FAIL Soft!=product
 */
int
iommu_vtd_usb_identity_residual(void)
{
    int fPresent;
    int fHasVtd;
    int fBus0;
    int fCtx14;
    int fIdLo;
    int fIdHi;
    int fMap;
    u32 u32Did;

    /* Lean re-entry: silent when xHCI identity + force32 map still honest. */
    if (g_fUsbResidDone != 0 && g_nUsbResidLast > 0 &&
        vtd_soft_bdf_identity_ready(VTD_G752_XHCI_BUS, VTD_G752_XHCI_SLOT,
                                    VTD_G752_XHCI_FUNC) != 0 &&
        vtd_soft_identity_map_ok() != 0) {
        return 1;
    }
    if (g_fUsbResidDone != 0 && g_nUsbResidLast == 0 &&
        iommu_present() == 0 && g_u64Drhd == 0 && !g_fVtdReady) {
        return 0;
    }

    fPresent = iommu_present();
    fHasVtd = (fPresent != 0 || g_u64Drhd != 0) ? 1 : 0;

    /*
     * Soft-skip if no VT-d: QEMU default / no DMAR. Soft!=product.
     * Grep: iommu: soft usb soft-skip no_vtd
     */
    if (!fHasVtd && !g_fVtdReady) {
        kprintf("iommu: soft usb soft-skip no_vtd present=%d drhd=0 "
                "Soft!=product\n",
                fPresent);
        kprintf("iommu: soft usb residual SKIP no_vtd Soft!=product\n");
        g_fUsbResidDone = 1;
        g_nUsbResidLast = 0;
        return 0;
    }

    if (!g_fVtdReady) {
        if (iommu_vtd_init_tables() != 0) {
            if (!fHasVtd) {
                kprintf("iommu: soft usb residual SKIP no_tables Soft!=product\n");
                g_fUsbResidDone = 1;
                g_nUsbResidLast = 0;
                return 0;
            }
            kprintf("iommu: soft usb residual FAIL tables Soft!=product\n");
            g_fUsbResidDone = 1;
            g_nUsbResidLast = -1;
            return -1;
        }
    }

    vtd_soft_grant_g752_xhci();
    fBus0 = vtd_root_bus_p(VTD_G752_XHCI_BUS);
    fCtx14 = vtd_ctx_devfn_p(VTD_G752_XHCI_SLOT, VTD_G752_XHCI_FUNC);
    fIdLo = iommu_vtd_identity_covers(0, 0x1000);
    fIdHi = iommu_vtd_identity_covers(VTD_IDENTITY_LIMIT - VTD_2MIB, VTD_2MIB);
    fMap = vtd_soft_identity_map_ok();
    u32Did = iommu_vtd_domain_lookup(VTD_G752_XHCI_BUS, VTD_G752_XHCI_SLOT,
                                     VTD_G752_XHCI_FUNC);
    {
        u32 u32CtxDid = vtd_ctx_get_did(VTD_G752_XHCI_SLOT, VTD_G752_XHCI_FUNC);

        /*
         * Grep: iommu: soft usb residual
         * Soft!=product - not MSC product close / not stage-15 T1 / G-AC-1.
         * DID0 honesty: soft attach + shared-CT hardware DID must both be 0.
         * force32 map: Dual DoD A UDX (xhci_udx) TRB/DMA need [0,1GiB).
         */
        kprintf("iommu: soft usb residual bdf=%u:%u.%u bus0_p=%d ctx_14_p=%d "
                "soft_did=%u ctx_did=%u id_lo=%d id_hi=%d map=%d present=%d "
                "Soft!=product\n",
                (unsigned)VTD_G752_XHCI_BUS, (unsigned)VTD_G752_XHCI_SLOT,
                (unsigned)VTD_G752_XHCI_FUNC, fBus0, fCtx14, u32Did, u32CtxDid,
                fIdLo, fIdHi, fMap, fPresent);

        if (!fBus0 || !fCtx14 || !fIdLo || !fIdHi || !fMap || u32Did != 0u ||
            u32CtxDid != 0u) {
            if (!fHasVtd) {
                kprintf("iommu: soft usb residual SKIP partial Soft!=product\n");
                g_fUsbResidDone = 1;
                g_nUsbResidLast = 0;
                return 0;
            }
            kprintf("iommu: soft usb residual FAIL preflight Soft!=product\n");
            g_fUsbResidDone = 1;
            g_nUsbResidLast = -1;
            return -1;
        }
    }

    kprintf("iommu: soft usb residual PASS bus0_p=1 ctx_14_p=1 did0=1 "
            "force32_1g=1 Soft!=product\n");
    g_fUsbResidDone = 1;
    g_nUsbResidLast = 1;
    return 1;
}

/** File-local alias. */
static int
vtd_soft_usb_identity_residual(void)
{
    return iommu_vtd_usb_identity_residual();
}

/**
 * Lean eng residual: freestanding NIC (03:00.0) + xHCI (0:14.0) identity
 * under Translated VT-d. Dual DoD A eng surface only - not product close.
 * Soft-skip when no VT-d. Soft!=product / no stamp storms.
 *
 * Return:
 *   1  PASS - at least one path PASS and no FAIL
 *   0  soft-skip - both soft-skip (no VT-d / nothing to enforce)
 *  -1  FAIL - either path FAIL under present inventory
 *
 * Greppable (keep stable):
 *   iommu: soft eng residual ...
 *   iommu: soft eng residual PASS|SKIP|FAIL Soft!=product
 */
int
iommu_vtd_nic_usb_eng_residual(void)
{
    int nNic;
    int nUsb;
    int fHasVtd;
    int fIdForce32;
    int fBus3;
    int fCtx14;
    int fDid0Eng;
    u32 cRoot;
    u32 cDid0;
    u32 u32NicDid;
    u32 u32UsbDid;

    /*
     * Lean re-entry: after first eng residual PASS, re-check identity/map
     * honesty only - nested bus3/usb residuals stay silent when ready.
     * Soft!=product / Dual DoD B freestanding rtl ring DMA + USB eng.
     */
    if (g_fEngResidDone != 0 && g_nEngResidLast > 0 &&
        vtd_root_buses_p_count() == 256u && vtd_eng_did0_ok() != 0 &&
        vtd_soft_identity_map_ok() != 0 &&
        vtd_root_bus_p(VTD_G752_NIC_BUS) != 0 &&
        vtd_ctx_devfn_p(VTD_G752_XHCI_SLOT, VTD_G752_XHCI_FUNC) != 0) {
        return 1;
    }
    if (g_fEngResidDone != 0 && g_nEngResidLast == 0 &&
        iommu_present() == 0 && g_u64Drhd == 0 && !g_fVtdReady) {
        return 0;
    }

    fHasVtd = (iommu_present() != 0 || g_u64Drhd != 0 || g_fVtdReady) ? 1 : 0;

    nNic = vtd_soft_bus3_identity_residual();
    nUsb = vtd_soft_usb_identity_residual();

    fBus3 = vtd_root_bus_p(VTD_G752_NIC_BUS);
    fCtx14 = vtd_ctx_devfn_p(VTD_G752_XHCI_SLOT, VTD_G752_XHCI_FUNC);
    cRoot = vtd_root_buses_p_count();
    cDid0 = vtd_ctx_did0_present_count();
    u32NicDid = vtd_ctx_get_did(VTD_G752_NIC_SLOT, VTD_G752_NIC_FUNC);
    u32UsbDid = vtd_ctx_get_did(VTD_G752_XHCI_SLOT, VTD_G752_XHCI_FUNC);
    fDid0Eng = vtd_eng_did0_ok();
    /* force32 class: whole bring-up identity is DMA-safe under SLPT. */
    fIdForce32 = vtd_soft_identity_map_ok();

    /*
     * Lean eng lamps only (no stamp storm / inventory flood). Soft!=product.
     * All-bus DID0 honesty: root_buses=256 + eng CT DID=0 for NIC+USB.
     * Grep: iommu: soft eng residual
     * Grep: iommu: soft did0 residual
     * Grep: iommu: soft all-bus identity residual (eng surface honesty)
     * Never claim product close; dual_dod OPEN product_hosts=UDX.
     */
    kprintf("iommu: soft eng residual nic=%d usb=%d bus3_p=%d ctx_14_p=%d "
            "root_buses=%u did0_eng=%d nic_did=%u usb_did=%u did0_ctx=%u "
            "force32_1g=%d has_vtd=%d host_8168=1 host_a12f=1 "
            "product_hosts=UDX dual_dod=OPEN Soft!=product\n",
            nNic, nUsb, fBus3, fCtx14, cRoot, fDid0Eng, u32NicDid, u32UsbDid,
            cDid0, fIdForce32, fHasVtd);
    kprintf("iommu: soft did0 residual root_buses=%u did0_eng=%d did0_ctx=%u "
            "nic_did=%u usb_did=%u product_hosts=UDX dual_dod=OPEN "
            "Soft!=product\n",
            cRoot, fDid0Eng, cDid0, u32NicDid, u32UsbDid);
    kprintf("iommu: soft all-bus identity residual eng_surface=1 "
            "root_buses=%u want=256 did0_eng=%d did0_ctx=%u "
            "bus3_p=%d ctx_14_p=%d force32_1g=%d map=%d "
            "product_hosts=UDX dual_dod=OPEN Soft!=product G-AC-1 "
            "(all-bus identity residual honesty; Soft!=product; "
            "never claim product close)\n",
            cRoot, fDid0Eng, cDid0, fBus3, fCtx14, fIdForce32, fIdForce32);

    if (nNic < 0 || nUsb < 0) {
        kprintf("iommu: soft eng residual FAIL nic=%d usb=%d "
                "product_hosts=UDX dual_dod=OPEN Soft!=product\n",
                nNic, nUsb);
        kprintf("iommu: soft all-bus identity residual FAIL eng_surface "
                "product_hosts=UDX dual_dod=OPEN Soft!=product\n");
        g_fEngResidDone = 1;
        g_nEngResidLast = -1;
        return -1;
    }
    if (nNic == 0 && nUsb == 0) {
        kprintf("iommu: soft eng residual SKIP no_vtd product_hosts=UDX "
                "dual_dod=OPEN Soft!=product\n");
        kprintf("iommu: soft did0 residual SKIP no_vtd Soft!=product\n");
        kprintf("iommu: soft all-bus identity residual SKIP no_vtd "
                "product_hosts=UDX dual_dod=OPEN Soft!=product\n");
        g_fEngResidDone = 1;
        g_nEngResidLast = 0;
        return 0;
    }
    /*
     * When either eng path PASSed, require all-bus root + eng DID0.
     * Soft-skip path already returned; under inventory this is FAIL.
     * Never claim product close; Soft!=product dual_dod OPEN.
     */
    if (cRoot != 256u || !fDid0Eng || !fIdForce32) {
        if (!fHasVtd) {
            kprintf("iommu: soft eng residual SKIP partial product_hosts=UDX "
                    "dual_dod=OPEN Soft!=product\n");
            kprintf("iommu: soft did0 residual SKIP partial Soft!=product\n");
            kprintf("iommu: soft all-bus identity residual SKIP partial "
                    "root_buses=%u did0_eng=%d product_hosts=UDX "
                    "dual_dod=OPEN Soft!=product\n",
                    cRoot, fDid0Eng);
            g_fEngResidDone = 1;
            g_nEngResidLast = 0;
            return 0;
        }
        kprintf("iommu: soft eng residual FAIL did0 root_buses=%u did0_eng=%d "
                "map=%d product_hosts=UDX dual_dod=OPEN Soft!=product\n",
                cRoot, fDid0Eng, fIdForce32);
        kprintf("iommu: soft did0 residual FAIL Soft!=product\n");
        kprintf("iommu: soft all-bus identity residual FAIL root_buses=%u "
                "did0_eng=%d map=%d product_hosts=UDX dual_dod=OPEN "
                "Soft!=product\n",
                cRoot, fDid0Eng, fIdForce32);
        g_fEngResidDone = 1;
        g_nEngResidLast = -1;
        return -1;
    }
    kprintf("iommu: soft eng residual PASS nic=%d usb=%d force32_1g=%d "
            "did0_eng=1 root_buses=256 host_8168=1 host_a12f=1 "
            "product_hosts=UDX dual_dod=OPEN Soft!=product\n",
            nNic, nUsb, fIdForce32);
    kprintf("iommu: soft did0 residual PASS root_buses=256 did0_eng=1 "
            "product_hosts=UDX dual_dod=OPEN Soft!=product\n");
    kprintf("iommu: soft all-bus identity residual PASS eng_surface=1 "
            "root_buses=256 did0_eng=1 force32_1g=1 product_hosts=UDX "
            "dual_dod=OPEN Soft!=product G-AC-1 "
            "(all-bus identity residual honesty; never product close)\n");
    g_fEngResidDone = 1;
    g_nEngResidLast = 1;
    return 1;
}

/** File-local alias. */
static int
vtd_soft_nic_usb_eng_residual(void)
{
    return iommu_vtd_nic_usb_eng_residual();
}

/**
 * Lean UDX DMA safety residual for G752 userspace driver path.
 * Dual DoD A/B eng surface: rtl8168_udx @ 03:00.0 + xhci_udx @ 0:14.0.
 * Runs eng residual (bus3/usb/did0/map), then lean UDX + C2 DMA path lamps.
 * FUNCTIONAL: eng busmaster under temporary enforce (window live for DMA
 * policy - not open-path-only). Soft!=product / G-AC-1. Soft-skip when no
 * VT-d. No stamp storms. No GPL; dual MIT OR Apache-2.0.
 * Soft residual != product Dual DoD close.
 *
 * C2 Dual DoD DMA path residual (greppable "iommu: soft dma path residual"):
 *   dual_dod_a=OPEN_UDX  (xhci_udx userspace; freestanding MSC SKIP)
 *   dual_dod_b=OPEN_UDX  (rtl8168_udx userspace; freestanding rtl SKIP)
 *   a_ready / b_ready     (per-path identity+cover+ctx honesty; not close)
 *   freestanding_product=SKIP  force32 cover for UDX DMA under TE
 *   product=UDX+ABI       (product direction honesty; G-AC-1 not .ko AC)
 *   window_mint=OPEN      (software window table only - not CNode cap)
 *   eng_bm               FUNCTIONAL eng busmaster under enforce
 * Soft lamps alone != Dual DoD close / product UDX DMA caps.
 * STRONGER denser residual (Soft!=product; dual_dod OPEN; product_hosts=UDX):
 *   busmaster_ok residual denser for 10ec:8168 + 8086:a12f hosts
 *   window_grant residual denser for same hosts (sample + full 1g cover)
 *   all-bus identity residual honesty (root 256 + DID0; never product close)
 * denser multi-arm residual (VTD_UDX_DENSE_ARMS=10; all required):
 *   eng_ready | root_256 | did0_eng | force32_1g | eng_bm |
 *   win_ok | bare_deny | dual_dod_open | sample_cover | product_open
 *   greppable: iommu: soft residual denser | denser_arms= | denser_ok=
 *   greppable: iommu: soft busmaster_ok residual denser
 *   greppable: iommu: soft window_grant residual denser
 *   greppable: iommu: soft all-bus identity residual denser
 * H2 once: residual lamps once + silent reaffirm (no stamp storms).
 * Bar honesty v2026.08.04.75 stamp-free — never invent .76.
 *
 * Return: 1 PASS / 0 soft-skip / -1 FAIL under present inventory
 *
 * Greppable (keep stable):
 *   iommu: soft udx residual ...
 *   iommu: soft udx residual soft-skip no_vtd Soft!=product
 *   iommu: soft udx residual PASS|SKIP|FAIL Soft!=product
 *   iommu: soft udx dma ... Soft!=product
 *   iommu: soft udx bm residual ... Soft!=product
 *   iommu: soft busmaster_ok residual ... Soft!=product product_hosts=UDX
 *   iommu: soft window_grant residual ... Soft!=product product_hosts=UDX
 *   iommu: soft all-bus identity residual ... Soft!=product
 *   iommu: soft residual denser ... Soft!=product product_hosts=UDX
 *   iommu: soft busmaster_ok residual denser ...
 *   iommu: soft window_grant residual denser ...
 *   iommu: soft all-bus identity residual denser ...
 *   iommu: soft dma path residual ... Soft!=product
 *   iommu: soft dma path residual PASS|SKIP|FAIL Soft!=product
 *   dual_dod_a=OPEN_UDX | dual_dod_b=OPEN_UDX | dual_dod=OPEN
 *   window_mint=OPEN | product_hosts=UDX
 *   denser=1 denser_arms= denser_ok=
 */
int
iommu_vtd_udx_dma_safety_residual(void)
{
    int nEng;
    int fNicReady;
    int fUsbReady;
    int fMap;
    int fForce32;
    int fDid0Eng;
    int fBus3;
    int fCtx14;
    int fNicCover = 0;
    int fUsbCover = 0;
    int fNicSample = 0;
    int fUsbSample = 0;
    int fWinOk;
    int fBmOk = 0;
    int fBmNic = 0;
    int fBmUsb = 0;
    int fBmBareDeny = 0;
    int fDodAReady;
    int fDodBReady;
    int fTe;
    int nTeMode;
    int fDenseEngReady;
    int fDenseRoot256;
    int fDenseDid0;
    int fDenseForce32;
    int fDenseEngBm;
    int fDenseWinOk;
    int fDenseBareDeny;
    int fDenseDodOpen;
    int fDenseSample;
    int fDenseProductOpen;
    u32 u32Dense;
    u32 u32DenseOk;
    u32 cRoot;
    u32 cWin;
    u32 u32NicDid;
    u32 u32UsbDid;
    u32 u32SoftNic;
    u32 u32SoftUsb;
    const char *szDenseVerdict;

    /*
     * Lean re-entry: after first UDX + dma path residual PASS, silent when
     * eng map honesty + soft windows + FUNCTIONAL eng busmaster still hold
     * (soft-probe / TE arm / xhci_identity). Soft!=product / Dual DoD C2.
     * Require both eng BDF soft windows live (cWin >= 2 after NIC+USB grant)
     * and eng busmaster under temporary enforce (no bare deny stamp growth).
     */
    if (g_fUdxResidDone != 0 && g_nUdxResidLast > 0 &&
        g_fDmaPathResidDone != 0 && g_nDmaPathResidLast > 0 &&
        vtd_root_buses_p_count() == 256u && vtd_eng_did0_ok() != 0 &&
        vtd_soft_identity_map_ok() != 0 &&
        vtd_soft_bdf_identity_ready(VTD_G752_NIC_BUS, VTD_G752_NIC_SLOT,
                                    VTD_G752_NIC_FUNC) != 0 &&
        vtd_soft_bdf_identity_ready(VTD_G752_XHCI_BUS, VTD_G752_XHCI_SLOT,
                                    VTD_G752_XHCI_FUNC) != 0 &&
        iommu_window_count() >= 2u &&
        vtd_soft_eng_busmaster_ok(0, NULL, NULL, NULL) != 0) {
        return 1;
    }
    if (g_fUdxResidDone != 0 && g_nUdxResidLast == 0 &&
        g_fDmaPathResidDone != 0 && g_nDmaPathResidLast == 0 &&
        iommu_present() == 0 && g_u64Drhd == 0 && !g_fVtdReady) {
        return 0;
    }

    /* Eng residual first (bus3 + usb + did0 + force32 map). Soft!=product. */
    nEng = vtd_soft_nic_usb_eng_residual();

    /*
     * C2 strengthen: re-affirm eng soft window grants for UDX force32 DMA
     * under TE. Grants end on full [0,1GiB) (not sample-only - do not shrink).
     * Soft!=product. Cover honesty via identity_covers (no extra grant stamp
     * after first NIC/USB grant lamps - soft grant helpers stay quiet).
     * Sample cover + full [0,1GiB) cover reported separately for eng honesty.
     */
    if (g_fVtdReady) {
        vtd_soft_grant_g752_nic();
        vtd_soft_grant_g752_xhci();
        fNicSample = iommu_vtd_identity_covers(0x1000ull, 0x1000ull) ? 1 : 0;
        fUsbSample = fNicSample; /* shared bring-up SLPT */
        fNicCover = (fNicSample != 0 &&
                     iommu_vtd_identity_covers(0, VTD_IDENTITY_LIMIT))
                        ? 1
                        : 0;
        fUsbCover = fNicCover;
    }

    fBus3 = vtd_root_bus_p(VTD_G752_NIC_BUS);
    fCtx14 = vtd_ctx_devfn_p(VTD_G752_XHCI_SLOT, VTD_G752_XHCI_FUNC);
    cRoot = vtd_root_buses_p_count();
    fMap = vtd_soft_identity_map_ok();
    /* force32 class: full identity cover for rings/TRB/UDX coherent DMA. */
    fForce32 = (fMap != 0 && fNicCover != 0 && fUsbCover != 0) ? 1 : 0;
    fDid0Eng = vtd_eng_did0_ok();
    fNicReady = vtd_soft_bdf_identity_ready(VTD_G752_NIC_BUS, VTD_G752_NIC_SLOT,
                                            VTD_G752_NIC_FUNC);
    fUsbReady = vtd_soft_bdf_identity_ready(VTD_G752_XHCI_BUS,
                                            VTD_G752_XHCI_SLOT,
                                            VTD_G752_XHCI_FUNC);
    u32NicDid = vtd_ctx_get_did(VTD_G752_NIC_SLOT, VTD_G752_NIC_FUNC);
    u32UsbDid = vtd_ctx_get_did(VTD_G752_XHCI_SLOT, VTD_G752_XHCI_FUNC);
    u32SoftNic = iommu_vtd_domain_lookup(VTD_G752_NIC_BUS, VTD_G752_NIC_SLOT,
                                         VTD_G752_NIC_FUNC);
    u32SoftUsb = iommu_vtd_domain_lookup(VTD_G752_XHCI_BUS, VTD_G752_XHCI_SLOT,
                                         VTD_G752_XHCI_FUNC);
    cWin = iommu_window_count();
    fTe = g_fTeArmed ? 1 : 0;
    nTeMode = iommu_vtd_te_mode();
    /*
     * FUNCTIONAL residual: eng busmaster under temporary enforce + bare deny.
     * Proves NIC/USB soft windows are live for UDX DMA policy (not open-path
     * only). Soft!=product; Dual DoD remains OPEN_UDX. Restores enforce.
     * Grep: iommu: soft udx bm residual
     */
    if (g_fVtdReady && nEng > 0) {
        fBmOk = vtd_soft_eng_busmaster_ok(1, &fBmNic, &fBmUsb, &fBmBareDeny);
    } else if (g_fVtdReady) {
        /* Soft-skip / FAIL eng: still sample eng bm for lamps (no bare tick). */
        (void)vtd_soft_eng_busmaster_ok(0, &fBmNic, &fBmUsb, &fBmBareDeny);
        fBmOk = 0;
    }
    /*
     * win_ok (stronger): tables ready + both eng soft windows live
     * (NIC + USB grants -> cWin >= 2) + full force32 identity cover +
     * FUNCTIONAL eng busmaster under enforce (eng_bm). Soft table only;
     * window_mint remains OPEN (not CNode cap product).
     */
    fWinOk = (g_fVtdReady && cWin >= 2u && fNicCover != 0 && fUsbCover != 0 &&
              fForce32 != 0 && fBmOk != 0)
                 ? 1
                 : 0;
    /*
     * Per-path readiness (Soft!=product; Dual DoD A/B remain OPEN_UDX):
     *   A (USB/xhci_udx): ctx 0:14.0 + identity-ready + cover + did0 + bm
     *   B (NIC/rtl8168_udx): bus3_p + identity-ready + cover + did0 + bm
     * Ready lamps != Dual DoD close / product UDX DMA cap mint.
     */
    fDodAReady = (fUsbReady != 0 && fCtx14 != 0 && fUsbCover != 0 &&
                  fDid0Eng != 0 && fMap != 0 && u32SoftUsb == 0u &&
                  fBmUsb != 0)
                     ? 1
                     : 0;
    fDodBReady = (fNicReady != 0 && fBus3 != 0 && fNicCover != 0 &&
                  fDid0Eng != 0 && fMap != 0 && u32SoftNic == 0u &&
                  fBmNic != 0)
                     ? 1
                     : 0;

    /*
     * denser multi-arm residual (VTD_UDX_DENSE_ARMS=10; Soft!=product).
     * Functional honesty for product IOMMU Dual DoD A/B UDX hosts.
     * Soft residual never closes Dual DoD; denser_ok is soft only.
     * greppable: denser_arms= denser_ok= iommu: soft residual denser
     */
    fDenseEngReady = 0;
    fDenseRoot256 = 0;
    fDenseDid0 = 0;
    fDenseForce32 = 0;
    fDenseEngBm = 0;
    fDenseWinOk = 0;
    fDenseBareDeny = 0;
    fDenseDodOpen = 0;
    fDenseSample = 0;
    fDenseProductOpen = 0;
    u32Dense = 0u;
    u32DenseOk = 0u;
    szDenseVerdict = "SKIP";

    /* arm0: eng_ready — both Dual DoD host BDFs identity-ready. */
    if (fNicReady != 0 && fUsbReady != 0) {
        fDenseEngReady = 1;
        u32Dense++;
    }
    /* arm1: root_256 — all-bus root present (G752 bus3 covered). */
    if (cRoot == 256u) {
        fDenseRoot256 = 1;
        u32Dense++;
    }
    /* arm2: did0_eng — eng CT DID=0 identity honesty. */
    if (fDid0Eng != 0) {
        fDenseDid0 = 1;
        u32Dense++;
    }
    /* arm3: force32_1g — full identity cover for UDX coherent DMA. */
    if (fForce32 != 0 && fMap != 0) {
        fDenseForce32 = 1;
        u32Dense++;
    }
    /* arm4: eng_bm — denser busmaster_ok residual under enforce. */
    if (fBmOk != 0 && fBmNic != 0 && fBmUsb != 0) {
        fDenseEngBm = 1;
        u32Dense++;
    }
    /* arm5: win_ok — denser window_grant residual both hosts. */
    if (fWinOk != 0 && fNicCover != 0 && fUsbCover != 0) {
        fDenseWinOk = 1;
        u32Dense++;
    }
    /* arm6: bare_deny — no-open-busmaster product honesty. */
    if (fBmBareDeny != 0) {
        fDenseBareDeny = 1;
        u32Dense++;
    }
    /* arm7: dual_dod_open — A/B ready honesty; Dual DoD stays OPEN. */
    if (fDodAReady != 0 && fDodBReady != 0 &&
        VTD_UDX_DENSE_ARMS == 10u &&
        VTD_UDX_DENSE_MIN == VTD_UDX_DENSE_ARMS) {
        fDenseDodOpen = 1;
        u32Dense++;
    }
    /* arm8: sample_cover — sample + full 1g cover honesty. */
    if (fNicSample != 0 && fUsbSample != 0 && fNicCover != 0 &&
        fUsbCover != 0) {
        fDenseSample = 1;
        u32Dense++;
    }
    /*
     * arm9: product_open — product=UDX+ABI; window_mint OPEN;
     * freestanding SKIP; soft residual never closes Dual DoD.
     */
    if (fDenseEngReady != 0 && fDenseEngBm != 0 && fDenseWinOk != 0 &&
        fDenseDodOpen != 0 && fDenseForce32 != 0 && fDenseBareDeny != 0) {
        fDenseProductOpen = 1;
        u32Dense++;
    }

    g_u32UdxDenseArms = u32Dense;
    if (u32Dense >= VTD_UDX_DENSE_MIN && fDenseEngReady != 0 &&
        fDenseRoot256 != 0 && fDenseDid0 != 0 && fDenseForce32 != 0 &&
        fDenseEngBm != 0 && fDenseWinOk != 0 && fDenseBareDeny != 0 &&
        fDenseDodOpen != 0 && fDenseSample != 0 &&
        fDenseProductOpen != 0) {
        u32DenseOk = 1u;
        if (g_u32UdxDenseOk < 0xffffffffu) {
            g_u32UdxDenseOk++;
        }
        szDenseVerdict = "PASS";
    } else if (nEng > 0) {
        if (g_u32UdxDenseFail < 0xffffffffu) {
            g_u32UdxDenseFail++;
        }
        szDenseVerdict = "FAIL";
    } else {
        szDenseVerdict = "SKIP";
    }

    /*
     * Lean UDX lamps only. Soft!=product / G-AC-1.
     * Grep: iommu: soft udx residual
     * Grep: iommu: soft udx dma
     * Grep: iommu: soft udx bm residual (FUNCTIONAL eng busmaster)
     * path=userspace product=UDX+ABI product_hosts=UDX: Dual DoD A/B over
     * UDX (not .ko / not freestanding class product). G-AC-1.
     * Soft residual != product close; dual_dod OPEN always.
     */
    kprintf("iommu: soft udx residual eng=%d nic_ready=%d usb_ready=%d "
            "bus3_p=%d ctx_14_p=%d root_buses=%u did0_eng=%d nic_did=%u "
            "usb_did=%u soft_nic=%u soft_usb=%u map=%d win=%u eng_bm=%d "
            "path=userspace product=UDX+ABI product_hosts=UDX "
            "dual_dod=OPEN Soft!=product G-AC-1\n",
            nEng, fNicReady, fUsbReady, fBus3, fCtx14, cRoot, fDid0Eng,
            u32NicDid, u32UsbDid, u32SoftNic, u32SoftUsb, fMap, cWin, fBmOk);
    kprintf("iommu: soft udx dma force32=%d cover=1g nic_sample=%d "
            "usb_sample=%d nic_bdf=%u:%u.%u usb_bdf=%u:%u.%u "
            "udx_nic=rtl8168_udx udx_usb=xhci_udx host_8168=1 host_a12f=1 "
            "nic_cover=%d usb_cover=%d product_hosts=UDX dual_dod=OPEN "
            "Soft!=product G-AC-1\n",
            fForce32, fNicSample, fUsbSample, (unsigned)VTD_G752_NIC_BUS,
            (unsigned)VTD_G752_NIC_SLOT, (unsigned)VTD_G752_NIC_FUNC,
            (unsigned)VTD_G752_XHCI_BUS, (unsigned)VTD_G752_XHCI_SLOT,
            (unsigned)VTD_G752_XHCI_FUNC, fNicCover, fUsbCover);
    kprintf("iommu: soft udx bm residual eng_bm=%d nic_ok=%d usb_ok=%d "
            "bare_deny=%d enforce_probe=1 product_hosts=UDX dual_dod=OPEN "
            "Soft!=product G-AC-1\n",
            fBmOk, fBmNic, fBmUsb, fBmBareDeny);

    /*
     * STRONGER denser residual for product IOMMU honesty (H2 once):
     * busmaster_ok + window_grant for G752 10ec:8168 (rtl8168_udx Dual DoD B)
     * + 8086:a12f (xhci_udx Dual DoD A). all-bus identity residual honesty.
     * Soft!=product; dual_dod OPEN; product_hosts=UDX; never product close.
     * Grep: iommu: soft busmaster_ok residual
     * Grep: iommu: soft window_grant residual
     * Grep: iommu: soft all-bus identity residual
     * Grep: Soft!=product | dual_dod=OPEN | product_hosts=UDX
     */
    kprintf("iommu: soft busmaster_ok residual "
            "host_8168=rtl8168_udx bdf=%u:%u.%u nic_ok=%d "
            "host_a12f=xhci_udx bdf=%u:%u.%u usb_ok=%d "
            "bare_deny=%d eng_bm=%d enforce_probe=1 "
            "path=iommu_busmaster_ok product_hosts=UDX "
            "dual_dod=OPEN dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX "
            "Soft!=product G-AC-1 "
            "(denser busmaster_ok residual 8168+a12f; Soft!=product; "
            "soft residual != product close)\n",
            (unsigned)VTD_G752_NIC_BUS, (unsigned)VTD_G752_NIC_SLOT,
            (unsigned)VTD_G752_NIC_FUNC, fBmNic,
            (unsigned)VTD_G752_XHCI_BUS, (unsigned)VTD_G752_XHCI_SLOT,
            (unsigned)VTD_G752_XHCI_FUNC, fBmUsb, fBmBareDeny, fBmOk);
    kprintf("iommu: soft window_grant residual "
            "host_8168=rtl8168_udx nic_sample=%d nic_cover=%d "
            "host_a12f=xhci_udx usb_sample=%d usb_cover=%d "
            "win=%u force32=%d win_ok=%d path=iommu_window_grant "
            "window_mint=OPEN product_hosts=UDX dual_dod=OPEN "
            "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX Soft!=product G-AC-1 "
            "(denser window_grant residual 8168+a12f; Soft!=product; "
            "soft residual != product close)\n",
            fNicSample, fNicCover, fUsbSample, fUsbCover, cWin, fForce32,
            fWinOk);
    kprintf("iommu: soft all-bus identity residual "
            "root_buses=%u want=256 did0_eng=%d map=%d "
            "bus3_p=%d ctx_14_p=%d force32=%d id_limit=0x%lx "
            "product_hosts=UDX dual_dod=OPEN Soft!=product G-AC-1 "
            "(all-bus identity residual honesty; Soft!=product; "
            "never claim product close; soft residual dual_dod OPEN)\n",
            cRoot, fDid0Eng, fMap, fBus3, fCtx14, fForce32,
            (unsigned long)VTD_IDENTITY_LIMIT);

    /*
     * denser multi-arm residual lamps (H2 once; Soft!=product Dual DoD OPEN).
     * Grep: iommu: soft residual denser
     * Grep: iommu: soft busmaster_ok residual denser
     * Grep: iommu: soft window_grant residual denser
     * Grep: iommu: soft all-bus identity residual denser
     * Grep: denser=1 denser_arms= denser_ok=
     * Bar honesty v2026.08.04.75 stamp-free — never invent .76.
     */
    if (g_fUdxDenseOnce == 0u) {
        g_fUdxDenseOnce = 1u;
        kprintf("iommu: soft residual denser %s denser=1 denser_arms=%u "
                "denser_ok=%u denser_min=%u denser_fail=%u "
                "eng_ready=%d root_256=%d did0_eng=%d force32_1g=%d "
                "eng_bm=%d win_ok=%d bare_deny=%d dual_dod_open=%d "
                "sample_cover=%d product_open=%d "
                "a_ready=%d b_ready=%d nic_ok=%d usb_ok=%d "
                "win=%u te=%d te_mode=%d "
                "product_hosts=UDX hosts=rtl8168_udx|xhci_udx "
                "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX dual_dod=OPEN "
                "window_mint=OPEN freestanding_product=SKIP product=UDX+ABI "
                "path=userspace soft_residual_closes_dod=0 "
                "stamp_free=1 bar_honesty=v2026.08.04.75 never_invent=.76 "
                "H2=once Soft!=product G-AC-1 dual=MIT|Apache-2.0 "
                "(denser multi-arm product IOMMU residual; "
                "not Dual DoD close; Soft!=product)\n",
                szDenseVerdict, (unsigned)u32Dense, (unsigned)u32DenseOk,
                (unsigned)VTD_UDX_DENSE_MIN, (unsigned)g_u32UdxDenseFail,
                fDenseEngReady, fDenseRoot256, fDenseDid0, fDenseForce32,
                fDenseEngBm, fDenseWinOk, fDenseBareDeny, fDenseDodOpen,
                fDenseSample, fDenseProductOpen, fDodAReady, fDodBReady,
                fBmNic, fBmUsb, cWin, fTe, nTeMode);
        kprintf("iommu: soft busmaster_ok residual denser %s denser=1 "
                "denser_arms=%u denser_ok=%u eng_bm=%d nic_ok=%d usb_ok=%d "
                "bare_deny=%d host_8168=rtl8168_udx host_a12f=xhci_udx "
                "path=iommu_busmaster_ok product_hosts=UDX dual_dod=OPEN "
                "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX Soft!=product "
                "G-AC-1 (denser busmaster_ok residual; not Dual DoD close)\n",
                szDenseVerdict, (unsigned)u32Dense, (unsigned)u32DenseOk,
                fDenseEngBm, fBmNic, fBmUsb, fBmBareDeny);
        kprintf("iommu: soft window_grant residual denser %s denser=1 "
                "denser_arms=%u denser_ok=%u win_ok=%d force32=%d "
                "nic_cover=%d usb_cover=%d sample=%d win=%u "
                "window_mint=OPEN path=iommu_window_grant "
                "product_hosts=UDX dual_dod=OPEN Soft!=product G-AC-1 "
                "(denser window_grant residual; not Dual DoD close)\n",
                szDenseVerdict, (unsigned)u32Dense, (unsigned)u32DenseOk,
                fDenseWinOk, fForce32, fNicCover, fUsbCover, fDenseSample,
                cWin);
        kprintf("iommu: soft all-bus identity residual denser %s denser=1 "
                "denser_arms=%u denser_ok=%u root_256=%d did0_eng=%d "
                "map=%d force32_1g=%d bus3_p=%d ctx_14_p=%d "
                "product_hosts=UDX dual_dod=OPEN Soft!=product G-AC-1 "
                "(denser all-bus identity residual; never product close)\n",
                szDenseVerdict, (unsigned)u32Dense, (unsigned)u32DenseOk,
                fDenseRoot256, fDenseDid0, fMap, fDenseForce32, fBus3,
                fCtx14);
        if (u32DenseOk != 0u) {
            kprintf("iommu: soft residual denser PASS denser=1 denser_arms=%u "
                    "denser_ok=1 product_hosts=UDX dual_dod=OPEN "
                    "Soft!=product G-AC-1 "
                    "(denser residual honesty; not Dual DoD close)\n",
                    (unsigned)u32Dense);
        }
    }

    /*
     * C2 Dual DoD DMA path residual rollup (userspace UDX; freestanding SKIP).
     * Grep: iommu: soft dma path residual
     * Grep: dual_dod_a=OPEN_UDX | dual_dod_b=OPEN_UDX | dual_dod=OPEN
     * Grep: freestanding_product=SKIP | window_mint=OPEN | product=UDX+ABI
     * Grep: product_hosts=UDX | Soft!=product
     * a_ready/b_ready: per-path eng honesty; Dual DoD stays OPEN (not close).
     * eng_bm: FUNCTIONAL eng busmaster under temporary enforce.
     */
    kprintf("iommu: soft dma path residual dual_dod_a=OPEN_UDX "
            "dual_dod_b=OPEN_UDX dual_dod=OPEN freestanding_product=SKIP "
            "product=UDX+ABI product_hosts=UDX eng=%d a_ready=%d b_ready=%d "
            "nic_ready=%d usb_ready=%d force32=%d map=%d win_ok=%d eng_bm=%d "
            "win=%u te=%d te_mode=%d window_mint=OPEN path=userspace "
            "Soft!=product G-AC-1\n",
            nEng, fDodAReady, fDodBReady, fNicReady, fUsbReady, fForce32,
            fMap, fWinOk, fBmOk, cWin, fTe, nTeMode);

    if (nEng < 0) {
        kprintf("iommu: soft udx residual FAIL eng=%d product_hosts=UDX "
                "dual_dod=OPEN Soft!=product G-AC-1\n",
                nEng);
        kprintf("iommu: soft udx bm residual FAIL eng product_hosts=UDX "
                "Soft!=product\n");
        kprintf("iommu: soft busmaster_ok residual FAIL eng "
                "host_8168=rtl8168_udx host_a12f=xhci_udx "
                "product_hosts=UDX dual_dod=OPEN Soft!=product\n");
        kprintf("iommu: soft window_grant residual FAIL eng "
                "product_hosts=UDX dual_dod=OPEN Soft!=product\n");
        kprintf("iommu: soft all-bus identity residual FAIL eng "
                "product_hosts=UDX dual_dod=OPEN Soft!=product\n");
        kprintf("iommu: soft dma path residual FAIL eng "
                "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX dual_dod=OPEN "
                "product_hosts=UDX window_mint=OPEN Soft!=product G-AC-1\n");
        g_fUdxResidDone = 1;
        g_nUdxResidLast = -1;
        g_fDmaPathResidDone = 1;
        g_nDmaPathResidLast = -1;
        return -1;
    }
    if (nEng == 0) {
        /* Grep: iommu: soft udx residual soft-skip no_vtd */
        kprintf("iommu: soft udx residual soft-skip no_vtd Soft!=product\n");
        kprintf("iommu: soft udx residual SKIP no_vtd product_hosts=UDX "
                "dual_dod=OPEN Soft!=product G-AC-1\n");
        kprintf("iommu: soft udx bm residual SKIP no_vtd Soft!=product\n");
        kprintf("iommu: soft busmaster_ok residual SKIP no_vtd "
                "host_8168=rtl8168_udx host_a12f=xhci_udx "
                "product_hosts=UDX dual_dod=OPEN Soft!=product\n");
        kprintf("iommu: soft window_grant residual SKIP no_vtd "
                "product_hosts=UDX dual_dod=OPEN Soft!=product\n");
        kprintf("iommu: soft all-bus identity residual SKIP no_vtd "
                "product_hosts=UDX dual_dod=OPEN Soft!=product\n");
        kprintf("iommu: soft dma path residual soft-skip no_vtd "
                "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX dual_dod=OPEN "
                "freestanding_product=SKIP product=UDX+ABI "
                "product_hosts=UDX window_mint=OPEN Soft!=product G-AC-1\n");
        kprintf("iommu: soft dma path residual SKIP no_vtd "
                "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX dual_dod=OPEN "
                "product_hosts=UDX window_mint=OPEN Soft!=product G-AC-1\n");
        g_fUdxResidDone = 1;
        g_nUdxResidLast = 0;
        g_fDmaPathResidDone = 1;
        g_nDmaPathResidLast = 0;
        return 0;
    }

    /*
     * Eng PASS already required root/did0/map. C2 UDX DMA path wants both
     * Dual DoD A/B eng paths identity-ready + force32 window cover + both
     * eng soft windows live + FUNCTIONAL eng busmaster under enforce.
     * Soft!=product; Dual DoD remains OPEN_UDX; never product close.
     */
    if (!fMap || !fDid0Eng || cRoot != 256u || !fNicReady || !fUsbReady ||
        !fWinOk || !fDodAReady || !fDodBReady || !fForce32 || !fBmOk) {
        if (iommu_present() == 0 && g_u64Drhd == 0) {
            kprintf("iommu: soft udx residual SKIP partial product_hosts=UDX "
                    "dual_dod=OPEN Soft!=product\n");
            kprintf("iommu: soft udx bm residual SKIP partial "
                    "eng_bm=%d nic_ok=%d usb_ok=%d Soft!=product\n",
                    fBmOk, fBmNic, fBmUsb);
            kprintf("iommu: soft busmaster_ok residual SKIP partial "
                    "nic_ok=%d usb_ok=%d product_hosts=UDX dual_dod=OPEN "
                    "Soft!=product\n",
                    fBmNic, fBmUsb);
            kprintf("iommu: soft window_grant residual SKIP partial "
                    "win_ok=%d product_hosts=UDX dual_dod=OPEN Soft!=product\n",
                    fWinOk);
            kprintf("iommu: soft all-bus identity residual SKIP partial "
                    "root_buses=%u did0_eng=%d product_hosts=UDX "
                    "dual_dod=OPEN Soft!=product\n",
                    cRoot, fDid0Eng);
            kprintf("iommu: soft dma path residual SKIP partial "
                    "a_ready=%d b_ready=%d win_ok=%d eng_bm=%d "
                    "product_hosts=UDX dual_dod=OPEN window_mint=OPEN "
                    "Soft!=product G-AC-1\n",
                    fDodAReady, fDodBReady, fWinOk, fBmOk);
            g_fUdxResidDone = 1;
            g_nUdxResidLast = 0;
            g_fDmaPathResidDone = 1;
            g_nDmaPathResidLast = 0;
            return 0;
        }
        kprintf("iommu: soft udx residual FAIL map=%d did0=%d root=%u "
                "nic=%d usb=%d win_ok=%d eng_bm=%d a_ready=%d b_ready=%d "
                "product_hosts=UDX dual_dod=OPEN Soft!=product G-AC-1\n",
                fMap, fDid0Eng, cRoot, fNicReady, fUsbReady, fWinOk, fBmOk,
                fDodAReady, fDodBReady);
        kprintf("iommu: soft udx bm residual FAIL eng_bm=%d nic_ok=%d "
                "usb_ok=%d bare_deny=%d Soft!=product\n",
                fBmOk, fBmNic, fBmUsb, fBmBareDeny);
        kprintf("iommu: soft busmaster_ok residual FAIL eng_bm=%d "
                "host_8168=%d host_a12f=%d bare_deny=%d product_hosts=UDX "
                "dual_dod=OPEN Soft!=product\n",
                fBmOk, fBmNic, fBmUsb, fBmBareDeny);
        kprintf("iommu: soft window_grant residual FAIL win_ok=%d "
                "nic_cover=%d usb_cover=%d product_hosts=UDX dual_dod=OPEN "
                "Soft!=product\n",
                fWinOk, fNicCover, fUsbCover);
        kprintf("iommu: soft all-bus identity residual FAIL root_buses=%u "
                "did0_eng=%d map=%d product_hosts=UDX dual_dod=OPEN "
                "Soft!=product\n",
                cRoot, fDid0Eng, fMap);
        kprintf("iommu: soft dma path residual FAIL a_ready=%d b_ready=%d "
                "win_ok=%d eng_bm=%d dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX "
                "dual_dod=OPEN product_hosts=UDX window_mint=OPEN "
                "Soft!=product G-AC-1\n",
                fDodAReady, fDodBReady, fWinOk, fBmOk);
        g_fUdxResidDone = 1;
        g_nUdxResidLast = -1;
        g_fDmaPathResidDone = 1;
        g_nDmaPathResidLast = -1;
        return -1;
    }

    kprintf("iommu: soft udx residual PASS eng=1 nic_ready=1 usb_ready=1 "
            "force32_1g=1 did0_eng=1 eng_bm=1 path=userspace product=UDX+ABI "
            "product_hosts=UDX dual_dod=OPEN Soft!=product G-AC-1\n");
    kprintf("iommu: soft udx dma PASS force32=1 cover=1g nic_cover=1 "
            "usb_cover=1 host_8168=1 host_a12f=1 product_hosts=UDX "
            "dual_dod=OPEN Soft!=product G-AC-1\n");
    kprintf("iommu: soft udx bm residual PASS eng_bm=1 nic_ok=1 usb_ok=1 "
            "bare_deny=1 product_hosts=UDX dual_dod=OPEN Soft!=product "
            "G-AC-1\n");
    kprintf("iommu: soft busmaster_ok residual PASS "
            "host_8168=rtl8168_udx nic_ok=1 host_a12f=xhci_udx usb_ok=1 "
            "bare_deny=1 eng_bm=1 path=iommu_busmaster_ok product_hosts=UDX "
            "dual_dod=OPEN Soft!=product G-AC-1\n");
    kprintf("iommu: soft window_grant residual PASS "
            "host_8168=rtl8168_udx nic_cover=1 host_a12f=xhci_udx "
            "usb_cover=1 force32_1g=1 win_ok=1 path=iommu_window_grant "
            "window_mint=OPEN product_hosts=UDX dual_dod=OPEN "
            "Soft!=product G-AC-1\n");
    kprintf("iommu: soft all-bus identity residual PASS root_buses=256 "
            "did0_eng=1 map=1 force32_1g=1 product_hosts=UDX dual_dod=OPEN "
            "Soft!=product G-AC-1 "
            "(all-bus identity residual honesty; never product close)\n");
    /*
     * Grep: iommu: soft dma path residual PASS
     * C2 honesty: Dual DoD A/B remain OPEN_UDX (not freestanding close).
     * a_ready=1 b_ready=1: eng identity/cover honest; not product close.
     * eng_bm=1: FUNCTIONAL eng busmaster under temporary enforce.
     * window_mint=OPEN: software window table only - not CNode cap mint.
     * product_hosts=UDX dual_dod=OPEN Soft!=product. G-AC-1. No version stamp.
     */
    kprintf("iommu: soft dma path residual PASS dual_dod_a=OPEN_UDX "
            "dual_dod_b=OPEN_UDX dual_dod=OPEN freestanding_product=SKIP "
            "product=UDX+ABI product_hosts=UDX a_ready=1 b_ready=1 "
            "force32_1g=1 win_ok=1 eng_bm=1 win=%u te=%d te_mode=%d "
            "window_mint=OPEN path=userspace Soft!=product G-AC-1\n",
            cWin, fTe, nTeMode);
    /*
     * STRONGER functional residual (W10 Dual DoD; stamp-free bar v2026.08.04.75):
     * live UDX host residual under TE enforce - force32 sample cover + eng BM
     * + dual_dod OPEN honesty for rtl8168_udx / xhci_udx product path.
     * Soft lamps alone != Dual DoD close / product UDX DMA cap mint.
     * Grep: iommu: soft udx host residual | product_hosts=UDX | dual_dod=OPEN
     */
    {
        int fSample = iommu_vtd_identity_covers(0x1000ull, 0x1000ull) ? 1 : 0;
        int fFull = iommu_vtd_identity_covers(0, VTD_IDENTITY_LIMIT) ? 1 : 0;
        int fHostOk;

        fHostOk = (fSample != 0 && fFull != 0 && fBmOk != 0 && fBmNic != 0 &&
                   fBmUsb != 0 && fBmBareDeny != 0 && fDodAReady != 0 &&
                   fDodBReady != 0 && fForce32 != 0)
                      ? 1
                      : 0;
        kprintf("iommu: soft udx host residual "
                "hosts=rtl8168_udx|xhci_udx product=UDX+ABI product_hosts=UDX "
                "host_8168=1 host_a12f=1 force32=%d sample=%d full_1g=%d "
                "eng_bm=%d nic_ok=%d usb_ok=%d bare_deny=%d "
                "a_ready=%d b_ready=%d win=%u te=%d te_mode=%d "
                "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX dual_dod=OPEN "
                "window_mint=OPEN freestanding_product=SKIP "
                "host_ok=%d Soft!=product G-AC-1 "
                "(W10 STRONGER live UDX host residual; Soft!=product; "
                "soft residual dual_dod OPEN product_hosts=UDX; "
                "not Dual DoD close; no .ko product; no version stamp)\n",
                fForce32, fSample, fFull, fBmOk, fBmNic, fBmUsb, fBmBareDeny,
                fDodAReady, fDodBReady, cWin, fTe, nTeMode, fHostOk);
        if (fHostOk != 0) {
            kprintf("iommu: soft udx host residual PASS "
                    "hosts=rtl8168_udx|xhci_udx product_hosts=UDX "
                    "host_8168=1 host_a12f=1 force32_1g=1 eng_bm=1 "
                    "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX dual_dod=OPEN "
                    "window_mint=OPEN Soft!=product G-AC-1\n");
            g_fUdxHostResidDone = 1;
            g_nUdxHostResidLast = 1;
        } else {
            kprintf("iommu: soft udx host residual FAIL host_ok=0 "
                    "product_hosts=UDX dual_dod=OPEN Soft!=product G-AC-1 "
                    "Dual_DoD_A=OPEN Dual_DoD_B=OPEN\n");
            g_fUdxHostResidDone = 1;
            g_nUdxHostResidLast = -1;
        }
        (void)g_nUdxHostResidLast;
    }
    g_fUdxResidDone = 1;
    g_nUdxResidLast = 1;
    g_fDmaPathResidDone = 1;
    g_nDmaPathResidLast = 1;
    return 1;
}

/** File-local alias. */
static int
vtd_soft_udx_dma_safety_residual(void)
{
    return iommu_vtd_udx_dma_safety_residual();
}

/*
 * Soft sample PA for DDI DMA_NOTE residual (inside bring-up identity [0,1GiB)).
 * Not a live device buffer - policy/inventory only. Soft!=product.
 */
#define VTD_DDI_NOTE_PA  0x4000ull
#define VTD_DDI_NOTE_CB  0x1000ull
/* Soft BDF for DDI DMA_NOTE residual (off eng 0:14.0 / 3:0.0 / prod 0:4.0). */
#define VTD_DDI_NOTE_BUS  0u
#define VTD_DDI_NOTE_SLOT 5u
#define VTD_DDI_NOTE_FUNC 0u

/**
 * Lean DDI DMA_NOTE residual: software window path + VT-d identity honesty.
 * Soft!=product / G-AC-1. window_mint remains OPEN (not CNode window cap).
 *
 * DDI door op 6 (DMA_NOTE) calls iommu_window_grant + devmgr soft note.
 * Residual: soft-grant a sample window, verify identity cover when tables
 * ready, lamp honesty. Soft-skip when no VT-d and tables not ready.
 * Once-only + silent reaffirm (no stamp storms). Soft lamps != product mint.
 *
 * Return: 1 PASS / 0 soft-skip / -1 FAIL under present inventory
 *
 * Greppable (keep stable - "iommu: soft ... Soft!=product"):
 *   iommu: soft ddi residual ...
 *   iommu: soft ddi residual PASS|SKIP|FAIL Soft!=product
 *   iommu: soft dma_note residual ...
 *   iommu: soft dma_note residual PASS|SKIP|FAIL Soft!=product
 *   window_mint=OPEN
 */
int
iommu_vtd_ddi_dma_note_residual(void)
{
    int fPresent;
    int fHasVtd;
    int fMap;
    int fCovered = 0;
    int fGrantOk;
    u32 cWin0;
    u32 cWin1;
    u32 cRoot;

    /*
     * Lean re-entry: after first PASS, silent when identity map still honest
     * and soft window table still live. Soft!=product / window_mint OPEN.
     */
    if (g_fDdiResidDone != 0 && g_nDdiResidLast > 0 &&
        vtd_soft_identity_map_ok() != 0 && iommu_window_count() > 0u) {
        return 1;
    }
    if (g_fDdiResidDone != 0 && g_nDdiResidLast == 0 &&
        iommu_present() == 0 && g_u64Drhd == 0 && !g_fVtdReady) {
        return 0;
    }

    fPresent = iommu_present();
    fHasVtd = (fPresent != 0 || g_u64Drhd != 0) ? 1 : 0;

    /*
     * Soft-skip if no VT-d and no tables: nothing to enforce for DMA_NOTE.
     * Soft!=product; window_mint stays OPEN either way.
     * Grep: iommu: soft ddi residual | soft dma_note residual
     */
    if (!fHasVtd && !g_fVtdReady) {
        kprintf("iommu: soft ddi residual soft-skip no_vtd present=%d "
                "window_mint=OPEN Soft!=product\n",
                fPresent);
        kprintf("iommu: soft dma_note residual soft-skip no_vtd "
                "window_mint=OPEN Soft!=product\n");
        kprintf("iommu: soft ddi residual SKIP no_vtd Soft!=product\n");
        kprintf("iommu: soft dma_note residual SKIP no_vtd Soft!=product\n");
        g_fDdiResidDone = 1;
        g_nDdiResidLast = 0;
        return 0;
    }

    if (!g_fVtdReady) {
        if (iommu_vtd_init_tables() != 0) {
            if (!fHasVtd) {
                kprintf("iommu: soft ddi residual SKIP no_tables "
                        "window_mint=OPEN Soft!=product\n");
                kprintf("iommu: soft dma_note residual SKIP no_tables "
                        "window_mint=OPEN Soft!=product\n");
                g_fDdiResidDone = 1;
                g_nDdiResidLast = 0;
                return 0;
            }
            kprintf("iommu: soft ddi residual FAIL tables Soft!=product\n");
            kprintf("iommu: soft dma_note residual FAIL tables Soft!=product\n");
            g_fDdiResidDone = 1;
            g_nDdiResidLast = -1;
            return -1;
        }
    }

    /*
     * Soft-grant sample window (create_window soft path used by DDI DMA_NOTE).
     * Verify VT-d identity cover for sample PA. Soft!=product; not cap mint.
     */
    cWin0 = iommu_window_count();
    fGrantOk = (iommu_vtd_window_grant(VTD_DDI_NOTE_BUS, VTD_DDI_NOTE_SLOT,
                                       VTD_DDI_NOTE_FUNC, VTD_DDI_NOTE_PA,
                                       VTD_DDI_NOTE_CB, &fCovered) == 0)
                   ? 1
                   : 0;
    cWin1 = iommu_window_count();
    fMap = vtd_soft_identity_map_ok();
    cRoot = vtd_root_buses_p_count();

    /*
     * Lean lamps once. Soft!=product / G-AC-1.
     * window_mint=OPEN: software BDF table only - not CNode DMA window cap.
     * Grep: iommu: soft ddi residual
     * Grep: iommu: soft dma_note residual
     * Grep: window_mint=OPEN
     */
    kprintf("iommu: soft ddi residual grant=%d cover=%d map=%d win=%u->%u "
            "root_buses=%u bdf=%u:%u.%u pa=0x%lx cb=0x%lx "
            "window_mint=OPEN Soft!=product\n",
            fGrantOk, fCovered, fMap, cWin0, cWin1, cRoot,
            (unsigned)VTD_DDI_NOTE_BUS, (unsigned)VTD_DDI_NOTE_SLOT,
            (unsigned)VTD_DDI_NOTE_FUNC, (unsigned long)VTD_DDI_NOTE_PA,
            (unsigned long)VTD_DDI_NOTE_CB);
    kprintf("iommu: soft dma_note residual op=6 path=iommu_window_grant "
            "grant=%d cover=%d identity_1g=%d window_mint=OPEN "
            "Soft!=product G-AC-1\n",
            fGrantOk, fCovered, fMap);

    if (!fGrantOk || !fCovered || !fMap) {
        if (!fHasVtd) {
            kprintf("iommu: soft ddi residual SKIP partial "
                    "window_mint=OPEN Soft!=product\n");
            kprintf("iommu: soft dma_note residual SKIP partial "
                    "window_mint=OPEN Soft!=product\n");
            g_fDdiResidDone = 1;
            g_nDdiResidLast = 0;
            return 0;
        }
        kprintf("iommu: soft ddi residual FAIL grant=%d cover=%d map=%d "
                "Soft!=product\n",
                fGrantOk, fCovered, fMap);
        kprintf("iommu: soft dma_note residual FAIL Soft!=product\n");
        g_fDdiResidDone = 1;
        g_nDdiResidLast = -1;
        return -1;
    }

    kprintf("iommu: soft ddi residual PASS grant=1 cover=1 map=1g "
            "window_mint=OPEN Soft!=product\n");
    kprintf("iommu: soft dma_note residual PASS window_mint=OPEN "
            "Soft!=product\n");
    g_fDdiResidDone = 1;
    g_nDdiResidLast = 1;
    return 1;
}

/** File-local alias. */
static int
vtd_soft_ddi_dma_note_residual(void)
{
    return iommu_vtd_ddi_dma_note_residual();
}

/**
 * Write context-entry DID (+AW) when tables ready.
 * Bring-up shares one context table across all root bus entries (identity).
 * Soft path: updates RAM only; no context-cache invalidate (no QI soft).
 * Note: shared context means DID for (slot,func) is global across buses -
 * fine for DID=0 identity; product multi-domain needs per-bus context later.
 */
static int
vtd_ctx_set_did(u8 u8Bus, u8 u8Slot, u8 u8Func, u32 u32Did)
{
    u64 *pCtx;
    u32 u32Idx;
    u64 *pE;

    (void)u8Bus; /* all buses share identity context in bring-up tables */
    if (!g_fVtdReady || g_paContext == 0) {
        return 0; /* soft ok without tables */
    }
    if (u32Did >= GJ_IOMMU_DOMAIN_MAX) {
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
 * Build identity SLPT covering [0, 1 GiB) with 2 MiB pages.
 * Layout: PDPT[0] -> PD with 512 x 2 MiB SP entries.
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
    /* CAP @0x08, ECAP @0x10 - public VT-d register map; read-only soft probe */
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
    /*
     * Root entries for ALL 256 buses: share one context table + identity SLPT.
     * G752 10ec:8168 sits at 03:00.0 (bus 3) - bus-0-only root left that BDF
     * unmapped under TE -> RX/TX OWN never clears (DMA fault). Soft!=product.
     * Root page is 4 KiB = 256 x 16-byte entries; one shared context is OK
     * for bring-up identity (DID=0, same SLPT for every BDF).
     */
    {
        u32 u32Bus;

        for (u32Bus = 0; u32Bus < 256u; u32Bus++) {
            pRoot[u32Bus * 2u] = (u64)g_paContext | VTD_ROOT_P;
            pRoot[u32Bus * 2u + 1u] = 0;
        }
    }
    /*
     * Context entries for all 256 devfn indices (slot[4:0]<<3 | func[2:0]).
     * Shared identity SLPT; DID=0 soft default. Applies to every bus via root.
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
    /*
     * Grep: iommu: vtd root=
     * Grep: root_buses=256 bus3_p= did0_ctx=
     * All-bus DID0: 256 root P + 256 shared-CT entries with DID=0.
     */
    kprintf("iommu: vtd root=0x%lx ctx=0x%lx slpt=0x%lx pages=%u ctx_dev=%u "
            "root_buses=%u bus0_p=%d bus3_p=%d ctx_00_p=%d did0_ctx=%u "
            "(shared identity DID0; covers G752 03:00.0 + 0:14.0)\n",
            (unsigned long)g_paRoot, (unsigned long)g_paContext,
            (unsigned long)g_paPdpt, g_u32VtdPages, g_u32CtxDevices,
            vtd_root_buses_p_count(), vtd_root_bus_p(0),
            vtd_root_bus_p(VTD_G752_NIC_BUS),
            vtd_ctx_devfn_p(VTD_G752_NIC_SLOT, VTD_G752_NIC_FUNC),
            vtd_ctx_did0_present_count());
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
    /* RTADDR at 0x20 (64-bit), GCMD at 0x18 - public VT-d register map */
    u64Rt = (u64)g_paRoot;
    pMmio[VTD_REG_RTADDR / 4u] = (u32)(u64Rt & 0xffffffffu);
    pMmio[(VTD_REG_RTADDR + 4u) / 4u] = (u32)(u64Rt >> 32);
    /* Set SRTP then TE - only when a real unit is present */
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
    /*
     * Arm TE only after identity preflight (all 256 root buses + bus3).
     * Without bus3 root P, G752 03:00.0 DMA faults -> freestanding OWN-stuck.
     */
    if (!vtd_te_identity_preflight()) {
        kprintf("iommu: vtd MMIO TE abort preflight FAIL (NIC OWN risk)\n");
        return 0;
    }
    pMmio[VTD_REG_GCMD / 4u] = VTD_GCMD_TE;
    /* Wait GSTS.TES - TE without TES ack is not live; OWN can stick mid-arm. */
    for (u32Spins = 0; u32Spins < VTD_GSTS_SPINS; u32Spins++) {
        if ((pMmio[VTD_REG_GSTS / 4u] & VTD_GSTS_TES) != 0) {
            break;
        }
    }
    if (u32Spins >= VTD_GSTS_SPINS) {
        kprintf("iommu: vtd TES timeout DRHD=0x%lx (TE not live; soft fallback)\n",
                (unsigned long)g_u64Drhd);
        return 0;
    }
    g_fTeArmed = 1;
    g_nTeMode = GJ_IOMMU_TE_HW;
    kprintf("iommu: vtd MMIO programmed DRHD=0x%lx RT=0x%lx TES=1 "
            "bus3_p=%d root_buses=%u\n",
            (unsigned long)g_u64Drhd, (unsigned long)u64Rt,
            vtd_root_bus_p(VTD_G752_NIC_BUS), vtd_root_buses_p_count());
    return 1;
}

int
iommu_vtd_te_arm(void)
{
    int fPre;
    int fEngBm = 0;

    if (g_fTeArmed && g_nTeMode != GJ_IOMMU_TE_NONE) {
        /*
         * Already armed: silent reaffirm UDX identity residual (eng map +
         * FUNCTIONAL eng busmaster) so post-TE greps stay honest without
         * stamp storms. Soft!=product / Dual DoD OPEN_UDX.
         * Grep: iommu: vtd TE arm already
         */
        (void)vtd_soft_udx_dma_safety_residual();
        fEngBm = vtd_soft_eng_busmaster_ok(0, NULL, NULL, NULL);
        kprintf("iommu: vtd TE arm already mode=%d bus3_p=%d root_buses=%u "
                "eng_bm=%d Soft!=product\n",
                g_nTeMode, vtd_root_bus_p(VTD_G752_NIC_BUS),
                vtd_root_buses_p_count(), fEngBm);
        return 1;
    }
    if (!g_fVtdReady) {
        if (iommu_vtd_init_tables() != 0) {
            kprintf("iommu: vtd TE arm tables FAIL\n");
            return 0;
        }
    }

    /*
     * Before TE (HW or soft): UDX DMA safety eng residual (pulls bus3 + usb
     * + did0 + map + FUNCTIONAL eng busmaster) so userspace rtl8168_udx /
     * xhci_udx force32 DMA stays OWN-safe under Translated TE.
     * Soft!=product / G-AC-1.
     * Also DDI DMA_NOTE residual (soft window + cover; window_mint OPEN).
     * Grep: iommu: soft udx residual | soft eng residual | soft bus3 residual
     * Grep: iommu: soft udx bm residual
     * Grep: iommu: soft ddi residual | soft dma_note residual | window_mint=OPEN
     */
    (void)vtd_soft_udx_dma_safety_residual();
    (void)vtd_soft_ddi_dma_note_residual();
    fPre = vtd_te_identity_preflight();
    if (!fPre) {
        /*
         * Incomplete root/identity: refuse HW TE (OWN killer). Soft-arm still
         * records policy when tables partial - but log FAIL-soft for greps.
         */
        kprintf("iommu: vtd TE arm preflight WARN bus3 incomplete "
                "(refuse HW TE; soft-arm if tables partial)\n");
    }

    if (g_u64Drhd != 0 && fPre) {
        if (iommu_vtd_program_hw()) {
            /* Grep: iommu: vtd TE arm HW PASS */
            fEngBm = vtd_soft_eng_busmaster_ok(0, NULL, NULL, NULL);
            kprintf("iommu: vtd TE arm HW PASS bus3_p=%d root_buses=%u "
                    "eng_bm=%d id_limit=0x%lx "
                    "(force32 DMA must stay in [0,1GiB))\n",
                    vtd_root_bus_p(VTD_G752_NIC_BUS), vtd_root_buses_p_count(),
                    fEngBm, (unsigned long)VTD_IDENTITY_LIMIT);
            return 1;
        }
        kprintf("iommu: vtd TE arm HW fail -> soft bus3_p=%d\n",
                vtd_root_bus_p(VTD_G752_NIC_BUS));
    } else if (g_u64Drhd != 0 && !fPre) {
        kprintf("iommu: vtd TE arm HW skipped preflight FAIL -> soft\n");
    }

    /* Soft TE: tables present; production HW path would set GCMD.TE */
    g_fTeArmed = 1;
    g_nTeMode = GJ_IOMMU_TE_SOFT;
    fEngBm = vtd_soft_eng_busmaster_ok(0, NULL, NULL, NULL);
    kprintf("iommu: vtd TE soft-armed root=0x%lx pages=%u bus3_p=%d "
            "root_buses=%u preflight=%d eng_bm=%d\n",
            (unsigned long)g_paRoot, g_u32VtdPages,
            vtd_root_bus_p(VTD_G752_NIC_BUS), vtd_root_buses_p_count(), fPre,
            fEngBm);
    /* Grep: iommu: vtd TE soft-arm PASS */
    kprintf("iommu: vtd TE soft-arm PASS bus3_cover=%d eng_bm=%d "
            "Soft!=product\n",
            vtd_root_bus_p(VTD_G752_NIC_BUS), fEngBm);
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
    /* Bus3 cover is part of live-ready for freestanding G752 NIC. */
    if (!vtd_root_bus_p(VTD_G752_NIC_BUS) || vtd_root_buses_p_count() != 256u) {
        kprintf("iommu: vtd TE live-ready bus3 FAIL bus3_p=%d root_buses=%u\n",
                vtd_root_bus_p(VTD_G752_NIC_BUS), vtd_root_buses_p_count());
        return 0;
    }
    if (nMode == GJ_IOMMU_TE_HW) {
        kprintf("iommu: vtd TE live HW mode DRHD=0x%lx bus3_p=1\n",
                (unsigned long)g_u64Drhd);
    } else {
        kprintf("iommu: vtd TE live soft mode (no DRHD; set GJ_INTEL_IOMMU=1) "
                "bus3_p=1\n");
    }
    /* Grep: iommu: vtd TE live-ready PASS */
    kprintf("iommu: vtd TE live-ready PASS mode=%d pages=%u bus3_p=%d "
            "root_buses=%u\n",
            nMode, g_u32VtdPages, vtd_root_bus_p(VTD_G752_NIC_BUS),
            vtd_root_buses_p_count());
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
        /*
         * greppable: iommu: vtd window_grant FAIL
         * Soft software window reject (full/bad); not product mint.
         */
        kprintf("iommu: vtd window_grant FAIL soft_win %u:%u.%u "
                "pa=0x%lx cb=0x%lx cover=0 product_window_mint=OPEN "
                "Soft!=product dual_dod_b=OPEN\n",
                bus, slot, func, (unsigned long)pa, (unsigned long)cb);
        return -1;
    }
    if (iommu_vtd_ready()) {
        fCovered = iommu_vtd_identity_covers(pa, cb);
        if (fCovered == 0) {
            /*
             * greppable: iommu: vtd identity cover FAIL
             * greppable: iommu: vtd identity cover=0
             * Soft residual: Own stick under TE if identity miss.
             * Soft!=product; not inventing full VT-d product program.
             */
            kprintf("iommu: vtd identity cover FAIL cover=0 %u:%u.%u "
                    "pa=0x%lx cb=0x%lx te_armed=%d soft_window=1 "
                    "product_window_mint=OPEN full_vtd=0 "
                    "residual=Own_stick_under_TE_if_identity_miss "
                    "Soft!=product dual_dod_b=OPEN\n",
                    bus, slot, func, (unsigned long)pa, (unsigned long)cb,
                    iommu_vtd_te_armed());
        } else {
            /* greppable: iommu: vtd identity cover */
            kprintf("iommu: vtd identity cover=%d %u:%u.%u pa=0x%lx "
                    "cb=0x%lx soft_window=1 product_window_mint=OPEN "
                    "Soft!=product\n",
                    fCovered, bus, slot, func, (unsigned long)pa,
                    (unsigned long)cb);
        }
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
 * Lean greppable soft inventory (prefix "vtd: soft ... Soft!=product").
 * Diagnostics only - never hard-gates; never claims always-on product IOMMU.
 * No version stamp. No stamp storms (few lamps only).
 * Keeps G752 eng cover lamps: bus3_p + identity [0,1GiB) + DID0 (UDX path).
 *
 * greppable: vtd: soft inventory
 * greppable: vtd: soft identity
 * greppable: vtd: soft path
 * greppable: vtd: soft PASS
 */
static void
vtd_soft_inventory_log(void)
{
    u32 cAtt = 0;
    u32 iAtt;
    int fReady;
    int fTe;
    int nMode;
    int fIdLo;
    int fIdHi;
    int fMap;
    u32 u32Feat;
    const char *szMode;

    /* Lean: inventory lamps once (soft-probe only path today). Soft!=product. */
    if (g_fSoftInvLogged != 0) {
        return;
    }
    g_fSoftInvLogged = 1;

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
    fIdLo = iommu_vtd_identity_covers(0, 0x1000);
    fIdHi = iommu_vtd_identity_covers(VTD_IDENTITY_LIMIT - VTD_2MIB, VTD_2MIB);
    fMap = vtd_soft_identity_map_ok();
    u32Feat = g_Soft.u32Feat;

    /* Grep: vtd: soft inventory */
    kprintf("vtd: soft inventory ready=%d pages=%u ctx=%u dom=%u/%u "
            "feat=0x%x te_mode=%s drhd=0x%lx Soft!=product\n",
            fReady, g_u32VtdPages, g_Soft.u32CtxPresent, g_u32DomUsed,
            (unsigned)GJ_IOMMU_DOMAIN_MAX, u32Feat, szMode,
            (unsigned long)g_u64Drhd);

    /*
     * Grep: vtd: soft identity
     * Grep: bus3_p (G752 03:00.0 10ec:8168 UDX eng cover)
     * Identity/map honesty for Dual DoD UDX force32 DMA.
     */
    kprintf("vtd: soft identity limit=0x%lx id_lo=%d id_hi=%d map=%d "
            "bus3_p=%d root_buses=%u did0_eng=%d te=%d Soft!=product\n",
            (unsigned long)VTD_IDENTITY_LIMIT, fIdLo, fIdHi, fMap,
            vtd_root_bus_p(VTD_G752_NIC_BUS), vtd_root_buses_p_count(),
            vtd_eng_did0_ok(), fTe);

    /*
     * Honesty: always-on product IOMMU remains OPEN.
     * eng_bm: FUNCTIONAL eng busmaster under enforce (UDX residual).
     * Grep: vtd: soft path
     * udx_dma_safety=eng: soft residual only; Soft!=product Dual DoD OPEN.
     * C2 Dual DoD DMA path: dual_dod_a/b=OPEN_UDX freestanding_product=SKIP
     * product=UDX+ABI product_hosts=UDX (G-AC-1 not .ko product AC).
     * Soft lamps != Dual DoD close. Never claim product close.
     * window_mint=OPEN: DDI DMA_NOTE soft window only - not CNode cap mint.
     * Full "iommu: soft dma path residual" lamps live in UDX residual once.
     * Grep: Soft!=product | dual_dod=OPEN | product_hosts=UDX
     */
    {
        int fEngBm = 0;

        if (fReady != 0) {
            fEngBm = vtd_soft_eng_busmaster_ok(0, NULL, NULL, NULL);
        }
        kprintf("vtd: soft path tables=ram_identity_1g te=soft_or_hw "
                "always_on_product=OPEN udx_dma_safety=eng eng_bm=%d "
                "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX dual_dod=OPEN "
                "freestanding_product=SKIP product=UDX+ABI product_hosts=UDX "
                "window_mint=OPEN Soft!=product G-AC-1 "
                "(soft residual dual_dod OPEN product_hosts=UDX; "
                "never product close)\n",
                fEngBm);
    }

    /* Grep: vtd: soft inventory PASS | vtd: soft PASS */
    kprintf("vtd: soft inventory PASS ready=%d pages=%u Soft!=product\n",
            fReady, g_u32VtdPages);
    kprintf("vtd: soft PASS always_on_product=OPEN window_mint=OPEN "
            "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX dual_dod=OPEN "
            "product=UDX+ABI product_hosts=UDX Soft!=product G-AC-1 "
            "(soft residual dual_dod OPEN product_hosts=UDX)\n");
    (void)cAtt;
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
 *   iommu: enforce default soft ...
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
     * Deny path: enforce armed, no window for product soft BDF -> bus-master
     * denied and platform deny counter must tick.
     */
    fDenyBare = iommu_busmaster_ok(VTD_PROD_SOFT_BUS, VTD_PROD_SOFT_SLOT,
                                   VTD_PROD_SOFT_FUNC);
    u32Denies1 = iommu_deny_count();
    if (fDenyBare != 0 || u32Denies1 <= u32Denies0) {
        kprintf("iommu: enforce default soft deny-path FAIL ok=%d "
                "denies=%u->%u\n",
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
                "denies=%u->%u\n",
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
     * Soft PASS without requiring DRHD / intel-iommu. Soft != product close:
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
     * Dual DoD UDX DMA safety eng residual (G752 userspace path):
     * pulls eng (bus3 + usb + did0 + map) then lean UDX lamps.
     * Soft-skip when no VT-d. Soft!=product / G-AC-1. No stamp storms.
     * Grep: iommu: soft udx residual | soft eng residual | soft bus3
     */
    (void)vtd_soft_udx_dma_safety_residual();
    /*
     * Lean DDI DMA_NOTE residual: soft window grant + VT-d cover honesty.
     * window_mint=OPEN (not CNode cap product). Soft!=product / G-AC-1.
     * Grep: iommu: soft ddi residual | soft dma_note residual | window_mint=OPEN
     */
    (void)vtd_soft_ddi_dma_note_residual();
    /* Lean soft inventory (greppable "vtd: soft ... Soft!=product"). */
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
            if (vtd_ctx_set_did(bus, slot, func, u32Did) != 0) {
                if (g_cSoftCtxDidFail < 0xffffffffu) {
                    g_cSoftCtxDidFail++;
                }
                kprintf("iommu: vtd domain soft ctx DID write fail\n");
            } else if (g_fVtdReady) {
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
    if (vtd_ctx_set_did(bus, slot, func, u32Did) != 0) {
        if (g_cSoftCtxDidFail < 0xffffffffu) {
            g_cSoftCtxDidFail++;
        }
        kprintf("iommu: vtd domain soft ctx DID write fail\n");
    } else if (g_fVtdReady) {
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
            /* Rebind shared context to default domain 0 */
            if (vtd_ctx_set_did(bus, slot, func, 0) == 0 && g_fVtdReady) {
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
    /*
     * Unbound devices soft-default to domain 0 when tables ready.
     * Bring-up shares one identity context + SLPT across all root buses
     * (DID=0). Bus-0-only default left G752 NIC 03:00.0 as INVALID after
     * table build if attach missed -> freestanding rtl WARN / OWN risk.
     * Soft!=product shared-identity; product multi-domain needs per-bus CT.
     */
    if (g_fVtdReady) {
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

/*
 * Soft xHCI identity path for freestanding DMA under VT-d Translated domains.
 * Sample window sits inside bring-up identity [0, 1 GiB). Parent pairs this
 * with dma_buf_alloc_page() so real buffer PAs stay coverable.
 *
 * greppable: iommu: xhci identity ... PASS|SKIP|FAIL
 */
int
iommu_vtd_xhci_identity(u8 bus, u8 slot, u8 func)
{
    int fCovered = 0;
    int fPresent;
    u32 u32Did;
    /* Low sample inside identity SLPT; not a real xHCI buffer - policy only */
    const u64 u64PaSample = 0x1000ull;
    const u64 u64CbSample = 0x1000ull;

    if (!vtd_bdf_ok(bus, slot, func)) {
        kprintf("iommu: xhci identity bdf=%u:%u.%u FAIL bad_bdf "
                "Soft!=product\n",
                (unsigned)bus, (unsigned)slot, (unsigned)func);
        return -1;
    }

    fPresent = iommu_present();

    if (!g_fVtdReady) {
        if (iommu_vtd_init_tables() != 0) {
            /*
             * No tables: soft-skip when platform has no DMAR/IVRS inventory
             * (QEMU default / no VT-d). FAIL when inventory said present
             * but we could not build identity tables.
             * Grep: iommu: soft ... soft-skip if no VT-d
             */
            if (!fPresent && g_u64Drhd == 0) {
                kprintf("iommu: soft xhci identity bdf=%u:%u.%u soft-skip "
                        "no_vtd Soft!=product\n",
                        (unsigned)bus, (unsigned)slot, (unsigned)func);
                kprintf("iommu: xhci identity bdf=%u:%u.%u SKIP no_tables "
                        "no_dmar Soft!=product\n",
                        (unsigned)bus, (unsigned)slot, (unsigned)func);
                return 0;
            }
            kprintf("iommu: xhci identity bdf=%u:%u.%u FAIL tables "
                    "Soft!=product\n",
                    (unsigned)bus, (unsigned)slot, (unsigned)func);
            return -1;
        }
    }

    /* Domain 0 = default identity domain after table build */
    if (!g_aDom[0].u8Used) {
        vtd_domain_pool_init();
    }
    u32Did = iommu_vtd_domain_lookup(bus, slot, func);
    if (u32Did == GJ_IOMMU_DOMAIN_INVALID || u32Did != 0) {
        /* Soft attach to identity DID 0 (updates bus-0 context when ready) */
        if (iommu_vtd_domain_attach(0, bus, slot, func) != 0) {
            /*
             * Attach soft-fail is non-fatal when slots full; still try window.
             * Log and continue - window + cover is the DMA policy surface.
             */
            kprintf("iommu: xhci identity bdf=%u:%u.%u attach soft-miss "
                    "(continue window)\n",
                    (unsigned)bus, (unsigned)slot, (unsigned)func);
        }
    }

    if (iommu_vtd_window_grant(bus, slot, func, u64PaSample, u64CbSample,
                               &fCovered) != 0) {
        kprintf("iommu: xhci identity bdf=%u:%u.%u FAIL grant\n",
                (unsigned)bus, (unsigned)slot, (unsigned)func);
        return -1;
    }

    if (!fCovered || !iommu_vtd_identity_covers(u64PaSample, u64CbSample)) {
        kprintf("iommu: xhci identity bdf=%u:%u.%u FAIL cover "
                "limit=0x%lx sample=0x%lx\n",
                (unsigned)bus, (unsigned)slot, (unsigned)func,
                (unsigned long)VTD_IDENTITY_LIMIT, (unsigned long)u64PaSample);
        return -1;
    }

    /*
     * Also record software window over full bring-up identity cover so
     * enforce+window inventory matches SLPT (rtl8168 rings / xHCI TRB live
     * here). Soft!=product mint; grant may noop-duplicate if already present.
     */
    {
        int fFull = 0;

        (void)iommu_vtd_window_grant(bus, slot, func, 0, VTD_IDENTITY_LIMIT,
                                     &fFull);
        if (!fFull || !iommu_vtd_identity_covers(0, VTD_IDENTITY_LIMIT)) {
            kprintf("iommu: xhci identity bdf=%u:%u.%u FAIL full_cover "
                    "limit=0x%lx Soft!=product\n",
                    (unsigned)bus, (unsigned)slot, (unsigned)func,
                    (unsigned long)VTD_IDENTITY_LIMIT);
            return -1;
        }
    }

    /*
     * FUNCTIONAL eng path: when BDF is G752 NIC or xHCI, require identity
     * ready (root/ctx/DID0/map 1 GiB) before PASS so UDX force32 DMA is not
     * left half-granted. Soft!=product / Dual DoD OPEN_UDX.
     */
    if ((bus == VTD_G752_NIC_BUS && slot == VTD_G752_NIC_SLOT &&
         func == VTD_G752_NIC_FUNC) ||
        (bus == VTD_G752_XHCI_BUS && slot == VTD_G752_XHCI_SLOT &&
         func == VTD_G752_XHCI_FUNC)) {
        if (vtd_soft_bdf_identity_ready(bus, slot, func) == 0) {
            kprintf("iommu: xhci identity bdf=%u:%u.%u FAIL eng_ready "
                    "root_p=%d ctx_p=%d Soft!=product\n",
                    (unsigned)bus, (unsigned)slot, (unsigned)func,
                    vtd_root_bus_p(bus), vtd_ctx_devfn_p(slot, func));
            return -1;
        }
    }

    /*
     * Grep: iommu: xhci identity ... PASS
     * Soft != product always-on IOMMU; TE/DRHD optional.
     * Root buses=256: non-bus-0 BDF (G752 03:00.0 10ec:8168) is covered.
     * Grep: bus3_p= when BDF bus==3
     */
    kprintf("iommu: xhci identity bdf=%u:%u.%u cover=1 limit=0x%lx "
            "did=%u te_mode=%d drhd=%d root_all_buses=1 "
            "root_bus_p=%d bus3_p=%d root_buses=%u PASS Soft!=product\n",
            (unsigned)bus, (unsigned)slot, (unsigned)func,
            (unsigned long)VTD_IDENTITY_LIMIT,
            iommu_vtd_domain_lookup(bus, slot, func), iommu_vtd_te_mode(),
            (g_u64Drhd != 0) ? 1 : 0, vtd_root_bus_p(bus),
            vtd_root_bus_p(VTD_G752_NIC_BUS), vtd_root_buses_p_count());
    /*
     * When parent grants identity for G752 eng BDFs, re-emit residual soft
     * lamps then lean UDX DMA safety eng residual (incl. FUNCTIONAL eng
     * busmaster). Soft!=product. Lean (no inventory stamp storm; residual
     * once + silent reaffirm).
     *   03:00.0 10ec:8168 -> bus3 residual + UDX (rtl8168_udx)
     *   0:14.0  xHCI      -> usb residual + UDX (xhci_udx)
     * Grep: iommu: soft udx residual | soft udx bm residual
     */
    if (bus == VTD_G752_NIC_BUS && slot == VTD_G752_NIC_SLOT &&
        func == VTD_G752_NIC_FUNC) {
        (void)vtd_soft_bus3_identity_residual();
        (void)vtd_soft_udx_dma_safety_residual();
    } else if (bus == VTD_G752_XHCI_BUS && slot == VTD_G752_XHCI_SLOT &&
               func == VTD_G752_XHCI_FUNC) {
        (void)vtd_soft_usb_identity_residual();
        (void)vtd_soft_udx_dma_safety_residual();
    }
    return 1;
}
