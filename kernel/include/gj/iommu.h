/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * IOMMU (VT-d / AMD-Vi) presence + enforce windows (M7).
 * Clean-room GreenJade policy/API — not a Linux/GPL VT-d driver.
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

struct gj_iommu_info {
    u8  u8Present;   /* RSDP/DMAR or IVRS found */
    u8  u8Vendor;    /* GJ_IOMMU_VENDOR_* */
    u8  u8Enforce;   /* production policy armed */
    u8  u8Pad;
    u32 u32Units;    /* DMAR/IVRS tables counted */
    u32 u32Windows;  /* granted DMA windows */
    u32 u32Denies;   /* bus-master denials under enforce */
};

#define GJ_IOMMU_MAX_WINDOWS 16u

struct gj_iommu_window {
    u8  u8Bus;
    u8  u8Slot;
    u8  u8Func;
    u8  u8Used;
    u64 u64PaBase;
    u64 u64Cb;
};

void iommu_probe(void);
int  iommu_present(void);
void iommu_info_get(struct gj_iommu_info *pOut);

/** Arm/disarm production enforce (no open bus-master without window). */
void iommu_enforce_set(int fOn);
int  iommu_enforce_get(void);

/**
 * Grant a DMA window for BDF (devmgr-shaped). Returns 0 or -1.
 * Rejects cb==0, PCI slot/func out of range, and pa+cb overflow.
 * When enforce is off, still records for inventory.
 */
int  iommu_window_grant(u8 bus, u8 slot, u8 func, u64 pa, u64 cb);
/** Revoke all windows for BDF. */
void iommu_window_revoke(u8 bus, u8 slot, u8 func);

/**
 * Bus-master check: 1 if allowed.
 * Enforce off → always allow (dev/QEMU). Enforce on → need matching window.
 */
int  iommu_busmaster_ok(u8 bus, u8 slot, u8 func);
u32  iommu_window_count(void);
u32  iommu_deny_count(void);

/* ---- VT-d hardware page tables (software construct; optional MMIO) ---- */
/** Build root/context/SLPT identity tables in RAM. */
int  iommu_vtd_init_tables(void);
/** Record DRHD register base from DMAR (0 if none / clear). */
void iommu_vtd_set_drhd(u64 u64Base);
u64  iommu_vtd_root_pa(void);
int  iommu_vtd_ready(void);
u32  iommu_vtd_pages(void);
/** Program DRHD RTADDR+TE if base known; else soft-only. Returns 1 if MMIO. */
int  iommu_vtd_program_hw(void);
/** Smoke: tables built → PASS log. */
int  iommu_vtd_smoke(void);
/**
 * Non-zero if VT-d identity SLPT covers [pa, pa+cb) (bring-up: first 1 GiB).
 * Used when software window grant integrates with VT-d tables.
 */
int  iommu_vtd_identity_covers(u64 pa, u64 cb);
/**
 * Grant software window and verify identity SLPT coverage when tables ready.
 * Returns 0 on grant ok; -1 on grant fail; covers reported via *pCovered (0/1).
 */
int  iommu_vtd_window_grant(u8 bus, u8 slot, u8 func, u64 pa, u64 cb,
                            int *pCovered);
/**
 * Soft-arm translation enable (TE) when tables ready.
 * With DRHD: programs MMIO (mode=HW). Without / on MMIO fail: soft policy
 * (mode=SOFT). Returns 1 if TE considered armed, 0 otherwise.
 */
int  iommu_vtd_te_arm(void);
/** Non-zero if TE soft-armed or MMIO programmed. */
int  iommu_vtd_te_armed(void);
/** GJ_IOMMU_TE_NONE / GJ_IOMMU_TE_SOFT / GJ_IOMMU_TE_HW. */
int  iommu_vtd_te_mode(void);
/**
 * Product live-ready check: tables + TE + identity window.
 * Soft mode when no DRHD; HW when DMAR provides a programmed DRHD.
 */
int  iommu_vtd_te_live_ready(void);
