/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * DDI door — native soft surface for userspace Linux-shaped driver hosts.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux .ko.
 *
 * Dispatched via GJ_SYS_DDI (arg0 = opcode). Soft product only:
 *   - PCI inventory note via devmgr_soft_pci_scan (D1 soft table)
 *   - Soft open handle + BAR map note (kernel UC VA when vmm_map_device_uc)
 *   - Soft config R/W notes; CFG_WRITE is carefully soft (no reckless live)
 *   - DMA window note → iommu_window_grant / devmgr_soft_dma_window_note
 *
 * Soft ≠ product: no live MMIO_FRAME / IRQ Notification / DMA window caps
 * mint to host CNodes (docs/DDI_SOFT.md, SECURITY_CORE_DESIGN.md §8–9.5).
 * Soft IRQ note (DDI_OP_IRQ_BIND) records handle→badge only; delivery is
 * still the global MSI-X Notification + GJ_SYS_NOTIFY_WAIT.
 *
 * Dispatch contract:
 *   ddi_door_syscall(op, arg1, arg2, arg3) → i64
 *   Success: 0, positive counts / handle ids / soft VA bits
 *   Errors:  negative GJ_ERR_* (INVAL / NOENT / NODEV / FAULT / NOSUPPORT / PERM)
 *   User pointers: user_range_ok + copy_to_user (kernel smoke HHDM ok).
 *
 * Greppable markers (keep stable):
 *   ddi_door: soft product surface PASS
 *   ddi: soft irq note PASS
 *   ddi_door: soft …
 *
 * Law: MIT/Apache only; soft≠product.
 */
#pragma once

#include <gj/types.h>

/* ---- door opcodes (GJ_SYS_DDI arg0) ------------------------------------ */

/**
 * SCAN: trigger soft PCI inventory via devmgr_soft_pci_scan.
 * Returns device count (soft table / deferred scan).
 */
#define DDI_OP_SCAN        1u

/**
 * GET: arg1=index, arg2=user ptr to struct gj_ddi_dev_info (packed).
 * Fills BDF, vend, dev, class, bar0_pa/cb (+ further soft BARs).
 * copy_to_user; ret 0 or GJ_ERR_*.
 */
#define DDI_OP_GET         2u

/**
 * OPEN: arg1=index → soft handle id (>0) for MAP/CFG/DMA ops.
 * Soft table only; no live cap mint.
 */
#define DDI_OP_OPEN        3u

/**
 * MAP_BAR: arg1=handle arg2=bar_index arg3=user_va_hint (product slot;
 *   soft records only; not a true userspace map install).
 * Soft map note: prefer vmm_map_device_uc for a kernel UC VA.
 * Honesty: does not install a true userspace map into the host AS.
 * Returns 0 on success (high UC VAs do not fit positive i64Ret).
 * Soft-return of kva+length for same-AS smoke: serial greppable line
 * carries kva/cb; length also from prior GET bar*_cb; kva is typically
 * GJ_DEVICE_MMIO_BASE+pa after a successful map_device_uc.
 * Host may use that kernel UC VA only if sharing the window — not product
 * isolation. Product user map is follow-on.
 */
#define DDI_OP_MAP_BAR     4u

/**
 * CFG_READ: arg1=handle arg2=offset → u32 config dword (aligned soft).
 */
#define DDI_OP_CFG_READ    5u

/**
 * DMA_NOTE: arg1=handle arg2=pa arg3=cb.
 * Record DMA window via iommu_window_grant if available and
 * devmgr_soft_dma_window_note (soft inventory).
 */
#define DDI_OP_DMA_NOTE    6u

/**
 * INVENTORY: kprintf once "ddi_door: soft product surface PASS".
 * Returns 0; subsequent calls soft 0 without re-printing.
 */
#define DDI_OP_INVENTORY   7u

/**
 * IRQ_BIND (soft note): arg1=handle arg2=badge_mask (0 → GJ_MSIX_BADGE_SOFT).
 * Records soft irq→handle note via irq_msix_soft_user_bind. No hard IRQ in
 * userspace; no product IRQ Notification cap mint (soft≠product).
 * Host wait after MSI-X soft fire:
 *   GJ_SYS_NOTIFY_WAIT which=0 mask=badge_mask block=0|1
 * greppable: ddi: soft irq note PASS
 * Opcode 9 (8 reserved / soft CLOSE slot in UDX headers).
 */
#define DDI_OP_IRQ_BIND    9u

/**
 * CFG_WRITE: arg1=handle arg2=offset arg3=u32 value.
 * Opcode 16 (sparse; leave 8..15 for future product grants).
 * Careful soft only if safe: soft-note path; no reckless live config poke
 * of identity / BARs. See ddi_door.c honesty comments.
 */
#define DDI_OP_CFG_WRITE   16u

/** Soft inventory / handle table bounds (diagnostics; not product depth). */
#define GJ_DDI_SOFT_DEV_MAX     32u
#define GJ_DDI_SOFT_HANDLE_MAX  16u
#define GJ_DDI_SOFT_BAR_MAX      6u

/**
 * Packed device info for DDI_OP_GET (userspace / kernel smoke).
 * Soft fields may be zero when scan is deferred or BAR unresolved.
 */
struct gj_ddi_dev_info {
    u8  u8Bus;
    u8  u8Slot; /* device number on bus */
    u8  u8Func;
    u8  u8Pad0;
    u16 u16Vend;
    u16 u16Dev;
    u32 u32Class; /* bits: rev | progif<<8 | subclass<<16 | class<<24 soft */
    u64 u64Bar0Pa;
    u64 u64Bar0Cb;
    u64 u64Bar1Pa;
    u64 u64Bar1Cb;
    u64 u64Bar2Pa;
    u64 u64Bar2Cb;
    u64 u64Bar3Pa;
    u64 u64Bar3Cb;
    u64 u64Bar4Pa;
    u64 u64Bar4Cb;
    u64 u64Bar5Pa;
    u64 u64Bar5Cb;
};

/**
 * Soft MAP_BAR result note (optional diagnostics / future out-arg).
 * Honesty: u64Va is a kernel UC VA when vmm_map_device_uc succeeds —
 * not a granted userspace MMIO_FRAME map.
 */
struct gj_ddi_map_note {
    u64 u64Va;     /* kernel UC VA or 0 */
    u64 u64Cb;     /* mapped / BAR length */
    u64 u64Pa;     /* BAR physical base */
    u32 u32Bar;    /* BAR index 0..5 */
    u32 u32Handle; /* soft handle */
    u8  u8Ok;
    u8  u8Pad[3];
};

/**
 * Init soft door tables. Safe to re-call (idempotent).
 * Does not claim product DDI authority.
 */
void ddi_door_init(void);

/**
 * Dispatch DDI opcode (GJ_SYS_DDI).
 * arg layout matches opcode docs above.
 */
i64  ddi_door_syscall(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3);

/** Non-zero after ddi_door_init. */
int  ddi_door_ready(void);

/** Soft call count (diagnostics). */
u32  ddi_door_call_count(void);
