/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft DDI / devmgr surface (ABI-first pivot).
 * Clean-room pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL.
 *
 * Scope (soft only — not full product devmgr):
 *   - Soft BDF / BAR / PCI function / DMA window grant types
 *   - Soft init + real type-0 CF8/CFC PCI inventory (buses 0..7)
 *   - Soft table helpers: bdf_register, note_window_grant, find_by_id, count
 *   - No userspace cap mint, no match graph, no live IOMMU / VT-d program
 *
 * Product path later: devmgr holds device untyped / IOMMU authority and
 * mints MMIO / IRQ / DMA window caps to userspace driver hosts
 * (docs/DDI_SOFT.md, SECURITY_CORE_DESIGN.md §8–9.5, ABI_FIRST_PIVOT.md).
 *
 * Greppable markers (keep stable):
 *   devmgr: soft init PASS
 *   devmgr: soft pci scan PASS n=N
 *   devmgr: soft found 10ec:8168   (G752VT Realtek NIC, if present)
 *   devmgr: soft found 8086:a12f   (G752VT Intel xHCI, if present)
 *   devmgr: soft dma window note
 */
#pragma once

#include <gj/types.h>

/* Soft table bounds (inventory; laptop host density). */
#define GJ_DEVMGR_SOFT_BDF_MAX  64u
#define GJ_DEVMGR_SOFT_BAR_MAX  6u /* PCI type-0 BARs 0..5 */
#define GJ_DEVMGR_SOFT_WIN_MAX  16u

/** Soft PCI BDF identity (bus/device/function). */
struct gj_devmgr_bdf {
    u8  u8Bus;
    u8  u8Slot; /* device number on bus */
    u8  u8Func;
    u8  u8Pad;
};

/**
 * Soft BAR snapshot (PA + size when known).
 * u8Mem: non-zero = memory BAR; zero = I/O BAR (soft inventory only).
 * u8Prefetch: non-zero when prefetchable (memory BARs).
 * u8Is64: non-zero when 64-bit memory BAR (consumes next index as high dword).
 * u64Pa / u64Cb: 0 if empty / unresolved.
 */
struct gj_devmgr_bar {
    u8  u8Index;    /* 0..5 */
    u8  u8Mem;
    u8  u8Prefetch;
    u8  u8Is64;
    u64 u64Pa;
    u64 u64Cb;
};

/**
 * Soft PCI function inventory record (type-0 walk result).
 * Filled by devmgr_soft_pci_scan(); Soft≠product (no cap mint).
 */
struct gj_devmgr_pci_fn {
    struct gj_devmgr_bdf bdf;
    u16 u16Vendor;
    u16 u16Device;
    u8  u8Class;
    u8  u8Subclass;
    u8  u8ProgIf;
    u8  u8HdrType; /* bits 6:0 only (no MF bit) */
    struct gj_devmgr_bar aBar[GJ_DEVMGR_SOFT_BAR_MAX];
};

/**
 * Soft DMA window grant result (devmgr-shaped note; not a live cap).
 * u8Ok: non-zero if the note was accepted into soft inventory.
 * u32WinId: soft window index (or 0xffffffff if rejected / full).
 * BDF + PA range mirror iommu_window_grant ingredients.
 */
struct gj_devmgr_window_grant {
    struct gj_devmgr_bdf bdf;
    u64 u64PaBase;
    u64 u64Cb;
    u8  u8Ok;
    u8  u8Rw; /* bit0 R, bit1 W (soft flags; product uses cap rights) */
    u8  u8Pad[2];
    u32 u32WinId;
};

/**
 * Soft init: idempotent ready lamp.
 * First call clears soft tables and may defer the CF8/CFC bus walk
 * (devmgr_soft_pci_scan performs the real inventory fill).
 * No VT-d program. Soft≠product.
 * See docs/DDI_SOFT.md.
 */
void devmgr_soft_init(void);

/** Non-zero after successful soft init (ready() query). */
int  devmgr_soft_ready(void);

/**
 * Soft PCI enumerate via type-0 CF8/CFC config walk.
 * Scans bus 0..7, slot 0..31, func 0..7; skips vendor 0xffff/0x0000.
 * Fills soft PCI inventory (vendor/device/class/BARs); BAR size via
 * classic write-all-1s probe with restore. Idempotent after first fill.
 * If pOut non-NULL, copies up to cMax BDFs from the inventory.
 * Returns number of functions in soft inventory (not just copied count).
 * Logs: "devmgr: soft pci scan PASS n=N" and target ID "found" lamps.
 */
u32  devmgr_soft_pci_scan(struct gj_devmgr_bdf *pOut, u32 cMax);

/**
 * Soft BDF register (inventory only — no config-space access).
 * Records bus:slot.func in the soft BDF table for later match/grant.
 * Returns soft index (>=0) on success, -1 on bad args / full table.
 */
int  devmgr_soft_bdf_register(const struct gj_devmgr_bdf *pBdf);

/**
 * Find first soft inventory function matching vendor:device.
 * Returns 0 and fills *pOut on hit; -1 if not found / bad args / empty.
 * Scan first if inventory not yet filled (ensures find works after init).
 */
int  devmgr_soft_find_by_id(u16 u16Vend, u16 u16Dev,
                            struct gj_devmgr_pci_fn *pOut);

/**
 * Soft inventory device count (scanned PCI functions).
 * Distinct from devmgr_soft_bdf_count (manual bdf_register tallies).
 */
u32  devmgr_soft_count(void);

/**
 * Copy soft inventory record at index iIdx into *pOut.
 * Returns 0 on success, -1 if out of range / bad args.
 */
int  devmgr_soft_get(u32 iIdx, struct gj_devmgr_pci_fn *pOut);

/**
 * Soft DMA window grant note (records intent; does not mint caps or
 * program VT-d). Optional mirror via existing iommu soft tables is a
 * later product step — this API stays soft-only.
 * Returns 0 if noted, -1 on bad args / not ready / soft table full.
 * pGrantOut may be NULL; when non-NULL, filled with soft result.
 */
int  devmgr_soft_dma_window_note(const struct gj_devmgr_bdf *pBdf,
                                 u64 u64PaBase, u64 u64Cb, u8 u8Rw,
                                 struct gj_devmgr_window_grant *pGrantOut);

/**
 * Soft note_window_grant helper (same soft table as dma_window_note).
 * Alias-shaped name for ABI-first DDI surface; Soft≠product.
 * See docs/DDI_SOFT.md.
 */
int  devmgr_soft_note_window_grant(const struct gj_devmgr_bdf *pBdf,
                                   u64 u64PaBase, u64 u64Cb, u8 u8Rw,
                                   struct gj_devmgr_window_grant *pGrantOut);

/** Soft inventory tallies (diagnostics). */
u32  devmgr_soft_pci_scan_calls(void);
u32  devmgr_soft_bdf_count(void);
u32  devmgr_soft_bdf_rejects(void);
u32  devmgr_soft_window_notes(void);
u32  devmgr_soft_window_rejects(void);
