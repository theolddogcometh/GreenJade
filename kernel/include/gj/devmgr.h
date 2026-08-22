/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft DDI / devmgr surface (ABI-first pivot).
 * Clean-room pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL.
 *
 * Scope (soft only - not full product devmgr; Soft!=product; G-AC-1):
 *   - Soft BDF / BAR / PCI function / DMA window grant types
 *   - Soft init + real type-0 CF8/CFC PCI inventory (buses 0..7)
 *   - Soft table helpers: bdf_register, note_window_grant, find_by_id, count
 *   - Real soft PCI scan residual deepen for DDI (G752 dual-DoD seeds):
 *       10ec:8168 (DoD B / rtl8168_udx) + 8086:a12f (DoD A / xhci_udx)
 *     IRQ line/pin, cmd/status, subsystem, cap ptr, rev, MF, BAR mem/io counts
 *     dual-DoD class_ok + match_graph=OPEN honesty; mint remains OPEN
 *   - No userspace cap mint, no product match graph, no live IOMMU / VT-d
 *   - Lean residual: dual-DoD HIT/MISS + BAR geom seed for DDI/UDX hosts
 *
 * Product direction (DDI/UDX userspace; not freestanding class drivers):
 *   inventory -> find_by_id -> window_note -> ddi_door SCAN/GET/OPEN/MAP_BAR
 *   -> UDX hosts (rtl8168_udx / xhci_udx) -> MMIO/IRQ/DMA cap mint OPEN
 * Soft inventory seeds that path only. Dual DoD A close is host USB path;
 * B close is interactive SSH login. Soft lamps / bind proof do not close. G-AC-1: no Linux .ko product.
 * Product hosts = UDX. Match graph = OPEN (soft residual != product match).
 * See docs/DDI_SOFT.md, UDX_LINUX_PORTER.md, ABI_FIRST_PIVOT.md,
 * SECURITY_CORE_DESIGN.md (caps / IOMMU / IRQ Notification).
 *
 * Greppable markers (keep stable):
 *   devmgr: soft init PASS
 *   devmgr: soft pci scan PASS n=N
 *   devmgr: soft found 10ec:8168   (HIT | MISS; DoD B seed; not TX/RX)
 *   devmgr: soft found 8086:a12f   (HIT | MISS; DoD A seed; not BOT/MSC)
 *   devmgr: soft dma window note
 *   devmgr: soft residual lean
 *   devmgr: soft pci scan residual deepen
 *   Soft!=product
 */
#pragma once

#include <gj/types.h>

/* Soft table bounds (inventory; laptop host density). */
#define GJ_DEVMGR_SOFT_BDF_MAX  64u
#define GJ_DEVMGR_SOFT_BAR_MAX  6u /* PCI type-0 BARs 0..5 */
#define GJ_DEVMGR_SOFT_WIN_MAX  16u

/* G752VT dual-DoD soft inventory target IDs (DDI/UDX seed only). */
#define GJ_DEVMGR_VID_REALTEK   0x10ecu
#define GJ_DEVMGR_DID_RTL8168   0x8168u
#define GJ_DEVMGR_VID_INTEL     0x8086u
#define GJ_DEVMGR_DID_XHCI_A12F 0xa12fu

/*
 * Soft dual-DoD match residual bits (inventory look only).
 * Product match graph remains OPEN - these bits are soft seed honesty,
 * not personality attach / Dual DoD A/B close.
 */
#define GJ_DEVMGR_SOFT_MATCH_RTL8168    0x1u
#define GJ_DEVMGR_SOFT_MATCH_XHCI_A12F  0x2u

/* Expected class codes for G752 dual-DoD soft class_ok residual. */
#define GJ_DEVMGR_CLASS_NET         0x02u /* Network controller */
#define GJ_DEVMGR_CLASS_SERIAL      0x0cu /* Serial bus */
#define GJ_DEVMGR_SUBCLASS_ETH      0x00u /* Ethernet */
#define GJ_DEVMGR_SUBCLASS_USB      0x03u /* USB controller */
#define GJ_DEVMGR_PROGIF_XHCI       0x30u /* xHCI */

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
 * Filled by devmgr_soft_pci_scan(); Soft!=product (no cap mint).
 * Seeds ddi_door SCAN/GET/OPEN and UDX host match (mint OPEN later).
 *
 * Residual deepen (real CF8 snapshot for DDI seed):
 *   rev / int line+pin / cmd+status / subsystem / cap ptr / MF bit /
 *   BAR mem+io counts. Match graph product remains OPEN.
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
    /* ---- soft PCI scan residual deepen (DDI seed; Soft!=product) ---- */
    u8  u8RevId;       /* config 0x08 low byte */
    u8  u8IntLine;     /* config 0x3c (legacy IRQ line; MSI/MSI-X later) */
    u8  u8IntPin;      /* config 0x3d (INTA..=0x01..; 0 = none) */
    u8  u8CapPtr;      /* config 0x34 (capability list head; 0 if none) */
    u16 u16Cmd;        /* config 0x04 command */
    u16 u16Status;     /* config 0x06 status */
    u16 u16SubsysVend; /* config 0x2c subsystem vendor */
    u16 u16SubsysId;   /* config 0x2e subsystem device */
    u8  u8MultiFunc;   /* non-zero if header type bit7 (MF) set on this fn */
    u8  u8BarMemN;     /* count of MEM BARs with non-zero size */
    u8  u8BarIoN;      /* count of I/O BARs with non-zero size */
    u8  u8Pad1;
};

/**
 * Soft DMA window grant result (devmgr-shaped note; not a live cap).
 * u8Ok: non-zero if the note was accepted into soft inventory.
 * u32WinId: soft window index (or 0xffffffff if rejected / full).
 * BDF + PA range mirror iommu_window_grant ingredients for UDX DMA seed.
 * Product: DMA window cap mint into host CNode remains OPEN.
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
 * No VT-d program. Soft!=product. Seeds DDI/UDX product path only.
 * See docs/DDI_SOFT.md, UDX_LINUX_PORTER.md.
 */
void devmgr_soft_init(void);

/** Non-zero after successful soft init (ready() query). */
int  devmgr_soft_ready(void);

/**
 * Soft PCI enumerate via type-0 CF8/CFC config walk.
 * Scans bus 0..7, slot 0..31; MF-aware funcs (func0 always; 1..7 when MF).
 * Skips vendor 0xffff/0x0000. Fills soft PCI inventory (vendor/device/
 * class/BARs + residual deepen fields); BAR size via classic write-all-1s
 * probe with restore. Idempotent after first fill.
 * If pOut non-NULL, copies up to cMax BDFs from the inventory.
 * Returns number of functions in soft inventory (not just copied count).
 * Once-lamps: scan PASS, dual-DoD found HIT|MISS, residual lean,
 * residual deepen (DDI/UDX product-direction seed). Soft!=product; G-AC-1.
 * Product hosts = UDX; match graph = OPEN; mint OPEN.
 */
u32  devmgr_soft_pci_scan(struct gj_devmgr_bdf *pOut, u32 cMax);

/**
 * Soft BDF register (inventory only - no config-space access).
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
 * Soft inventory index of first function matching vendor:device.
 * Returns >=0 index on hit; -1 if not found. Scan-first like find_by_id.
 * DDI residual: index feeds ddi_door OPEN/GET without a second walk.
 */
int  devmgr_soft_index_by_id(u16 u16Vend, u16 u16Dev);

/**
 * Soft dual-DoD hit residual bits after inventory fill.
 * OR of GJ_DEVMGR_SOFT_MATCH_RTL8168 / GJ_DEVMGR_SOFT_MATCH_XHCI_A12F.
 * Soft inventory look only - not product match graph (graph remains OPEN).
 * Soft!=product; does not close Dual DoD A/B.
 */
u32  devmgr_soft_dual_dod_hits(void);

/**
 * First MEM BAR index (u8Mem!=0, non-zero size) on a soft inventory fn.
 * Returns bar index 0..5 or -1. Soft snapshot only - not sole BAR owner.
 * Product BAR/MMIO ownership is UDX/DDI host grant (mint OPEN).
 */
int  devmgr_soft_fn_first_mem_bar(const struct gj_devmgr_pci_fn *pFn);

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
 * program VT-d). Soft seed for UDX DMA path; product window cap mint
 * into host CNode remains OPEN (docs/DDI_SOFT.md).
 * Returns 0 if noted, -1 on bad args / not ready / soft table full.
 * pGrantOut may be NULL; when non-NULL, filled with soft result.
 */
int  devmgr_soft_dma_window_note(const struct gj_devmgr_bdf *pBdf,
                                 u64 u64PaBase, u64 u64Cb, u8 u8Rw,
                                 struct gj_devmgr_window_grant *pGrantOut);

/**
 * Soft note_window_grant helper (same soft table as dma_window_note).
 * Alias-shaped name for ABI-first DDI surface; Soft!=product.
 * Product path: ddi_door DMA_NOTE / UDX dma_* over live window caps.
 * See docs/DDI_SOFT.md, UDX_LINUX_PORTER.md.
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
