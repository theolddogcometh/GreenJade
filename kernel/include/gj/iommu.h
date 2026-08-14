/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * IOMMU (VT-d / AMD-Vi) presence + enforce windows (M7 / A2 partial).
 * Clean-room GreenJade policy/API - not a Linux/GPL VT-d driver.
 * Pure C11 freestanding, dual MIT OR Apache-2.0.
 *
 * Layers:
 *   1) Inventory: ACPI DMAR (Intel) / IVRS (AMD) presence + unit counts
 *   2) Enforce windows: software BDF -> PA range grants; bus-master deny
 *      under enforce (devmgr-shaped); inventory still records when off
 *   3) VT-d tables: root/context/SLPT identity cover (bring-up first 1 GiB)
 *   4) TE arm: SOFT (tables ready, no DRHD MMIO) or HW (RTADDR + GCMD.TE)
 *   5) Soft domain pool: software DIDs; optional context DID write on bus-0
 *
 * Soft vs hard:
 *   Soft-probe and soft-only TE stay green without DRHD (QEMU default).
 *   HW TE when DMAR provides a programmed DRHD (e.g. GJ_INTEL_IOMMU=1).
 *   CAP/ECAP may be synthetic when MMIO is absent (SOFT_FEAT_CAP_SOFT).
 *
 * Greppable product markers (keep stable):
 *   iommu: probe PASS / enforce PASS
 *   iommu: vtd tables PASS / vtd soft-only ... PASS / vtd soft-probe PASS
 *   iommu: vtd identity grant PASS / vtd TE soft-arm/path PASS
 *   iommu: vtd TE live-ready PASS / vtd domain soft PASS
 *   iommu: xhci identity ... PASS | SKIP | FAIL
 */
#pragma once

#include <gj/types.h>

/* Vendor codes in gj_iommu_info.u8Vendor */
#define GJ_IOMMU_VENDOR_NONE  0u
#define GJ_IOMMU_VENDOR_INTEL 1u /* ACPI DMAR */
#define GJ_IOMMU_VENDOR_AMD   2u /* ACPI IVRS */

/* TE arm mode (iommu_vtd_te_mode) */
#define GJ_IOMMU_TE_NONE 0 /* not armed */
#define GJ_IOMMU_TE_SOFT 1 /* tables ready; no DRHD MMIO */
#define GJ_IOMMU_TE_HW   2 /* DRHD RTADDR + GCMD.TE programmed */

/* Soft domain pool (software DID; no HW invalidate required) */
#define GJ_IOMMU_DOMAIN_MAX       8u
#define GJ_IOMMU_DOMAIN_INVALID   0xffffffffu

/* Soft-probe feature bits (gj_iommu_vtd_soft.u32Feat) */
#define GJ_IOMMU_SOFT_FEAT_TABLES   (1u << 0)
#define GJ_IOMMU_SOFT_FEAT_IDENTITY (1u << 1)
#define GJ_IOMMU_SOFT_FEAT_DRHD     (1u << 2)
#define GJ_IOMMU_SOFT_FEAT_CAP_MMIO (1u << 3) /* CAP/ECAP read from DRHD */
#define GJ_IOMMU_SOFT_FEAT_CAP_SOFT (1u << 4) /* synthetic CAP when no MMIO */
#define GJ_IOMMU_SOFT_FEAT_TE_SOFT  (1u << 5)
#define GJ_IOMMU_SOFT_FEAT_TE_HW    (1u << 6)
#define GJ_IOMMU_SOFT_FEAT_DOMAIN   (1u << 7)

/**
 * Platform IOMMU inventory (probe + enforce counters).
 * u8Enforce: production policy armed (no open bus-master without window).
 */
struct gj_iommu_info {
    u8  u8Present;   /* RSDP/DMAR or IVRS found */
    u8  u8Vendor;    /* GJ_IOMMU_VENDOR_* */
    u8  u8Enforce;   /* production policy armed */
    u8  u8Pad;
    u32 u32Units;    /* DMAR/IVRS tables counted */
    u32 u32Windows;  /* granted DMA windows */
    u32 u32Denies;   /* bus-master denials under enforce */
};

/**
 * VT-d soft-probe inventory (tables + DMAR struct counts + CAP soft).
 * Safe without DRHD; CAP/ECAP may be synthetic.
 * u64IdentityLimit: bring-up SLPT cover end (1 GiB).
 */
struct gj_iommu_vtd_soft {
    u8  u8TablesReady;
    u8  u8TeMode;      /* GJ_IOMMU_TE_* */
    u8  u8HasDrhd;
    u8  u8CapFromMmio; /* 1 if CAP/ECAP read from DRHD */
    u32 u32Feat;       /* GJ_IOMMU_SOFT_FEAT_* */
    u32 u32Pages;
    u32 u32CtxDevices;
    u32 u32CtxPresent; /* context entries with P set (soft verify) */
    u32 u32Domains;    /* soft domains in use (incl. default 0 when ready) */
    u32 u32DomainMax;  /* GJ_IOMMU_DOMAIN_MAX */
    u32 u32DrhdCount;  /* DMAR type-0 structures */
    u32 u32RmrrCount;  /* DMAR type-1 */
    u32 u32AtsrCount;  /* DMAR type-2 */
    u32 u32RhsaCount;  /* DMAR type-3 */
    u32 u32OtherCount; /* other remapping structure types */
    u64 u64DrhdBase;
    u64 u64RootPa;
    u64 u64IdentityLimit; /* bring-up SLPT cover end (1 GiB) */
    u64 u64Cap;        /* CAP or soft synthetic */
    u64 u64Ecap;       /* ECAP or soft synthetic */
};

#define GJ_IOMMU_MAX_WINDOWS 16u

/** Software DMA window grant (BDF + PA range). */
struct gj_iommu_window {
    u8  u8Bus;
    u8  u8Slot;
    u8  u8Func;
    u8  u8Used;
    u64 u64PaBase;
    u64 u64Cb;
};

/** Walk ACPI for DMAR/IVRS; fill presence inventory. Safe without IOMMU HW. */
void iommu_probe(void);

/** Non-zero if DMAR or IVRS was found (inventory present). */
int  iommu_present(void);

/** Copy inventory snapshot; null pOut is a no-op. */
void iommu_info_get(struct gj_iommu_info *pOut);

/**
 * Arm/disarm production enforce (no open bus-master without window).
 * fOn non-zero -> enforce on. QEMU/dev typically leave off until product.
 */
void iommu_enforce_set(int fOn);
int  iommu_enforce_get(void);

/**
 * Grant a DMA window for BDF (devmgr-shaped). Returns 0 or -1.
 * Rejects cb==0, PCI slot/func out of range, and pa+cb overflow.
 * When enforce is off, still records for inventory.
 */
int  iommu_window_grant(u8 bus, u8 slot, u8 func, u64 pa, u64 cb);

/** Revoke all windows for BDF (no-op if none). */
void iommu_window_revoke(u8 bus, u8 slot, u8 func);

/**
 * Bus-master check: 1 if allowed.
 * Enforce off -> always allow (dev/QEMU). Enforce on -> need matching window.
 */
int  iommu_busmaster_ok(u8 bus, u8 slot, u8 func);

/** Active software window count / lifetime deny count (stats). */
u32  iommu_window_count(void);
u32  iommu_deny_count(void);

/* ---- VT-d hardware page tables (software construct; optional MMIO) ---- */

/** Build root/context/SLPT identity tables in RAM. Returns 0 or -1. */
int  iommu_vtd_init_tables(void);

/** Record DRHD register base from DMAR (0 if none / clear). */
void iommu_vtd_set_drhd(u64 u64Base);

/** Root table physical address (0 if tables not built). */
u64  iommu_vtd_root_pa(void);

/** Non-zero when identity tables are ready. */
int  iommu_vtd_ready(void);

/** Pages allocated for root/context/SLPT construct (stats). */
u32  iommu_vtd_pages(void);

/**
 * Program DRHD RTADDR+TE if base known; else soft-only.
 * Returns 1 if MMIO programmed, 0 if soft / fail.
 */
int  iommu_vtd_program_hw(void);

/** Smoke: tables built -> PASS log. Returns 1 on PASS, 0 on FAIL. */
int  iommu_vtd_smoke(void);

/**
 * Non-zero if VT-d identity SLPT covers [pa, pa+cb) (bring-up: first 1 GiB).
 * Used when software window grant integrates with VT-d tables.
 */
int  iommu_vtd_identity_covers(u64 pa, u64 cb);

/**
 * Grant software window and verify identity SLPT coverage when tables ready.
 * Returns 0 on grant ok; -1 on grant fail; covers reported via *pCovered (0/1)
 * when pCovered non-NULL.
 */
int  iommu_vtd_window_grant(u8 bus, u8 slot, u8 func, u64 pa, u64 cb,
                            int *pCovered);

/**
 * Soft-arm translation enable (TE) when tables ready.
 * With DRHD: programs MMIO (mode=HW) only after identity preflight
 * (all 256 root buses + bus3 for G752 03:00.0 + identity [0,1GiB)).
 * HW sequence: clear firmware TES → RTADDR/SRTP → context+IOTLB inv → TE.
 * Contexts: TT=multi_level + SLPTPTR identity [0,1GiB) + DID0 + AW=39.
 * Without DRHD / on MMIO fail: soft policy (mode=SOFT) and **disarm any
 * live firmware TES** so soft-arm does not leave foreign remapping (Own).
 * Pre-runs UDX DMA safety eng residual (bus3 + usb + did0 + map) and
 * DDI DMA_NOTE residual (soft window + cover; window_mint OPEN) so TE
 * does not leave UDX force32 DMA OWN-stuck. Soft!=product / G-AC-1.
 * Soft-skip residual when no VT-d (no DMAR/DRHD, no tables).
 * Returns 1 if TE considered armed, 0 otherwise.
 * Once-pins persist STATUS hold2 TE/identity lamp (G752 no-COM1 glass):
 *   TE mode=hw|soft|none tes= tt=ML slpt= rdy bus3 id1g
 * Never hold0/6/13/14/15. Soft!=product dual_dod_b=OPEN.
 * Grep: iommu: vtd TE preflight | bus3_p | TE arm HW/soft-arm PASS
 * Grep: iommu: vtd ctx TT=multi_level | iommu: vtd inv | TE firmware
 * Grep: iommu: vtd TE hold2
 * Grep: iommu: soft udx residual | soft bus3 residual Soft!=product
 * Grep: iommu: soft ddi residual | soft dma_note residual | window_mint=OPEN
 */
int  iommu_vtd_te_arm(void);

/** Non-zero if TE soft-armed or MMIO programmed. */
int  iommu_vtd_te_armed(void);

/** GJ_IOMMU_TE_NONE / GJ_IOMMU_TE_SOFT / GJ_IOMMU_TE_HW. */
int  iommu_vtd_te_mode(void);

/**
 * Dual DoD B dig: clear DRHD GSTS.TES whenever mapped (firmware residual
 * included) and drop software TE flags. Glass gap: soft-arm mode used to
 * skip GCMD writes so firmware TES stayed live. Soft!=product.
 * Returns 1 if TE now off, 0 if HW TES clear timed out.
 * Once-updates persist STATUS hold2 with tes=0 te_disarm (not UDX hold14).
 * greppable: iommu: vtd TE disarm | PLATFORM_INFO op9 te_disarm
 * greppable: iommu: vtd TE hold2
 */
int  iommu_vtd_te_disarm(void);

/**
 * Product live-ready check: tables + TE + identity window.
 * Soft mode when no DRHD; HW when DMAR provides a programmed DRHD.
 */
int  iommu_vtd_te_live_ready(void);

/* ---- VT-d soft probe (deep inventory; no commit to HW required) ---- */

/**
 * Feed DMAR remapping-structure inventory from ACPI walk (probe path).
 * Counts are soft-only; safe with zeros when no DMAR.
 */
void iommu_vtd_soft_dmar_inventory(u32 cDrhd, u32 cRmrr, u32 cAtsr, u32 cRhsa,
                                   u32 cOther);

/**
 * Soft-probe: ensure tables, verify root/context, CAP/ECAP soft or MMIO,
 * domain pool defaults, DMAR inventory. Logs `iommu: vtd soft-probe PASS`.
 * Also runs UDX DMA safety eng residual + DDI DMA_NOTE residual
 * (pulls bus3/usb/did0; soft window grant; soft-skip if no VT-d).
 * Soft!=product. G-AC-1. window_mint=OPEN.
 * Grep: iommu: soft udx residual | soft bus3 residual | soft ddi residual
 * Grep: iommu: soft dma_note residual | window_mint=OPEN | vtd soft-probe PASS
 * Returns 1 on soft-ready, 0 on hard fail (no tables).
 */
int  iommu_vtd_soft_probe(void);

/** Copy last soft-probe snapshot (zeros if never probed). Null is no-op. */
void iommu_vtd_soft_info_get(struct gj_iommu_vtd_soft *pOut);

/** Non-zero after a successful soft-probe. */
int  iommu_vtd_soft_probed(void);

/* ---- VT-d domain soft (software DID; optional context DID write) ---- */

/**
 * Create a soft domain. Returns DID (0..DOMAIN_MAX-1) or INVALID.
 * Domain 0 is the default identity domain after tables init.
 */
u32  iommu_vtd_domain_create(void);

/** Destroy soft domain (not 0). Fails if still attached. Returns 0 or -1. */
int  iommu_vtd_domain_destroy(u32 u32Did);

/**
 * Attach BDF to soft domain. Updates bus-0 context DID when tables ready.
 * Returns 0 or -1.
 */
int  iommu_vtd_domain_attach(u32 u32Did, u8 bus, u8 slot, u8 func);

/** Detach BDF (rebind bus-0 context to domain 0 when tables ready). */
int  iommu_vtd_domain_detach(u8 bus, u8 slot, u8 func);

/** Lookup soft DID for BDF; INVALID if none / default unbound. */
u32  iommu_vtd_domain_lookup(u8 bus, u8 slot, u8 func);

/** Soft domains currently in use (stats). */
u32  iommu_vtd_domain_count(void);

/**
 * Soft domain smoke: create -> attach -> lookup -> detach -> destroy.
 * Logs `iommu: vtd domain soft PASS`. Returns 1 on success, 0 on fail.
 */
int  iommu_vtd_domain_soft_smoke(void);

/* ---- Soft xHCI / freestanding DMA identity (VT-d Translated hosts) ---- */

/**
 * Common PCH xHCI BDF on many Intel laptops (e.g. G752VT class 0:14.0).
 * Callers may pass any valid BDF; these are conveniences for parent wiring.
 */
#define GJ_IOMMU_XHCI_BUS_DEFAULT  0u
#define GJ_IOMMU_XHCI_SLOT_DEFAULT 0x14u
#define GJ_IOMMU_XHCI_FUNC_DEFAULT 0u

/**
 * G752VT freestanding Realtek NIC (10ec:8168) BDF - bus 3, not bus 0.
 * TE without root[bus3].P left RX/TX OWN stuck under Translated domains.
 * Soft!=product residual; greppable bus3 lamps.
 */
#define GJ_IOMMU_G752_NIC_BUS  3u
#define GJ_IOMMU_G752_NIC_SLOT 0u
#define GJ_IOMMU_G752_NIC_FUNC 0u

/**
 * Dual DoD B DMA residual: freestanding NIC (03:00.0) VT-d identity under
 * RX climb / arping so force32 rtl rings are not broken by TE.
 * Soft!=product / G-AC-1 (not .ko product AC). Dual MIT OR Apache-2.0.
 *
 * Ensures all-bus root + bus3_p + identity SLPT [0,1 GiB) + NIC window
 * grant + DID0, then TE preflight lamps. Soft-skip when no VT-d (no
 * DMAR/IVRS and no DRHD, no tables). Safe without HW TE.
 * Lean residual: once-only lamps + silent reaffirm; no version stamp;
 * no stamp storms (soft-probe / TE arm / xhci_identity re-enter quiet).
 *
 * Return:
 *   1  PASS - bus3 identity residual ready (preflight + map honesty ok)
 *   0  soft-skip / SKIP - no VT-d or partial without inventory
 *  -1  FAIL - tables/preflight incomplete under present inventory
 *
 * Greppable (keep stable - "iommu: soft ... Soft!=product"):
 *   iommu: soft bus3 residual ...
 *   iommu: soft bus3 soft-skip no_vtd Soft!=product
 *   iommu: soft bus3 preflight ... Soft!=product
 *   iommu: soft bus3 residual PASS|SKIP|FAIL Soft!=product
 *   iommu: soft identity residual ... Soft!=product
 *   iommu: soft dma residual ... Soft!=product  (force32 rtl ring map)
 *
 * Wired from soft-probe / TE arm / xhci_identity(3,0,0). Do not edit
 * rtl8168 / dma_buf / main / config for this residual.
 */
int  iommu_vtd_bus3_identity_residual(void);

/**
 * Dual DoD residual: freestanding xHCI (0:14.0) identity under Translated
 * VT-d. Soft!=product / G-AC-1. Soft-skip when no VT-d.
 *
 * Return: 1 PASS / 0 soft-skip / -1 FAIL under present inventory
 * Grep: iommu: soft usb residual ... PASS|SKIP|FAIL Soft!=product
 */
int  iommu_vtd_usb_identity_residual(void);

/**
 * Lean eng residual: G752 NIC (03:00.0) + xHCI (0:14.0) identity.
 * Eng surface for Dual DoD A/B UDX userspace path (rtl8168_udx / xhci_udx).
 * Soft!=product / once-only + silent reaffirm (no stamp storms).
 * Soft-skip when no VT-d. Requires all-bus root + eng DID0 + map 1 GiB.
 *
 * Return: 1 PASS / 0 soft-skip / -1 FAIL under present inventory
 * Grep: iommu: soft eng residual ... | iommu: soft did0 residual ...
 */
int  iommu_vtd_nic_usb_eng_residual(void);

/**
 * Lean UDX DMA safety residual (G752 userspace driver path). Soft!=product.
 * G-AC-1: not .ko product AC. Dual MIT OR Apache-2.0.
 *
 * VT-d identity honesty so UDX DMA (force32 coherent / window grant /
 * PLATFORM_INFO op5) for eng BDFs is not broken under Translated TE:
 *   - NIC 03:00.0 10ec:8168  -> rtl8168_udx Dual DoD B
 *   - xHCI 0:14.0 8086:a12f  -> xhci_udx Dual DoD A
 * Runs eng residual first (bus3 + usb + did0 + map 1 GiB), then emits
 * lean UDX lamps. Soft-skip when no VT-d. Once-only + silent reaffirm
 * (no stamp storms from soft-probe / TE arm / xhci_identity re-enter).
 * Soft lamps alone != product UDX DMA caps closed / Dual DoD close.
 *
 * Return:
 *   1  PASS - eng identity ready for UDX DMA safety eng
 *   0  soft-skip / SKIP - no VT-d or eng soft-skip
 *  -1  FAIL - eng residual FAIL under present inventory
 *
 * Greppable (keep stable - "iommu: soft ... Soft!=product"):
 *   iommu: soft udx residual ...
 *   iommu: soft udx residual soft-skip no_vtd Soft!=product
 *   iommu: soft udx residual PASS|SKIP|FAIL Soft!=product
 *   iommu: soft udx dma ... Soft!=product
 *
 * Wired from soft-probe / TE arm. Do not edit user/udx / rtl8168 /
 * dma_buf / main / config for this residual.
 */
int  iommu_vtd_udx_dma_safety_residual(void);

/**
 * Lean DDI DMA_NOTE residual: software window path + VT-d identity honesty.
 * Soft!=product / G-AC-1. Dual MIT OR Apache-2.0.
 *
 * DDI door op 6 (DMA_NOTE) records a soft window via iommu_window_grant
 * (+ devmgr soft note). This residual soft-grants a sample window inside
 * bring-up identity [0,1 GiB), verifies VT-d cover when tables ready, and
 * greps honesty that product window **mint** remains OPEN (software table
 * only - not CNode DMA window cap product). Soft-skip when no VT-d.
 * Once-only + silent reaffirm (soft-probe / TE arm re-enter quiet).
 * Soft lamps alone != product DMA window cap mint / Dual DoD close.
 *
 * Return:
 *   1  PASS - soft grant + identity cover honest for DMA_NOTE path
 *   0  soft-skip / SKIP - no VT-d or partial without inventory
 *  -1  FAIL - grant/cover fail under present inventory
 *
 * Greppable (keep stable - "iommu: soft ... Soft!=product"):
 *   iommu: soft ddi residual ...
 *   iommu: soft ddi residual PASS|SKIP|FAIL Soft!=product
 *   iommu: soft dma_note residual ...
 *   iommu: soft dma_note residual PASS|SKIP|FAIL Soft!=product
 *   window_mint=OPEN
 *
 * Wired from soft-probe / TE arm. Do not edit ddi_door / devmgr / main /
 * user/udx for this residual - lives here.
 */
int  iommu_vtd_ddi_dma_note_residual(void);

/**
 * Soft: grant identity-path policy for a PCI BDF (typical xHCI 0:14.0).
 *
 * Ensures VT-d identity tables (soft), attaches BDF to default domain 0,
 * and records a software DMA window inside the bring-up identity cover
 * [0, 1 GiB). Never hard-gates boot; safe when no DRHD / no DMAR.
 * Soft-skip when no VT-d (no inventory, no tables). Soft!=product.
 *
 * Return:
 *   1  PASS - tables + window + identity cover for BDF
 *   0  SKIP - soft-skip: no VT-d / no inventory and tables unavailable
 *  -1  FAIL - bad BDF, table init fail with DMAR present, or grant fail
 *
 * Greppable (keep stable):
 *   iommu: xhci identity bdf=B:S.F ... PASS|SKIP|FAIL Soft!=product
 *   iommu: soft xhci identity ... soft-skip no_vtd Soft!=product
 *   iommu: soft bus3 residual ... (when BDF is G752 03:00.0)
 *   iommu: soft udx residual ... (UDX DMA safety eng re-enter)
 *
 * Parent (main / xhci / UDX host) should call after iommu_probe(), e.g.:
 *   (void)iommu_vtd_xhci_identity(0, 0x14, 0);       // xHCI / xhci_udx
 *   (void)iommu_vtd_xhci_identity(3, 0, 0);          // 10ec:8168 @ 03:00.0
 *   (void)iommu_vtd_bus3_identity_residual();       // bus3 residual
 *   (void)iommu_vtd_udx_dma_safety_residual();      // UDX Dual DoD eng
 * Pair with dma_buf_alloc_page() / udx_dma_* so device DMA PAs stay in
 * identity cover. Root covers all 256 buses (shared identity context)
 * so non-bus-0 endpoints (G752 NIC) are not left unmapped under TE.
 * Do not edit rtl8168 / user/udx for this residual - lives here.
 */
int  iommu_vtd_xhci_identity(u8 bus, u8 slot, u8 func);
