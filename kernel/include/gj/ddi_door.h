/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * DDI door - native soft surface for userspace Linux-shaped driver hosts.
 * Pure C11 freestanding TU. Dual MIT OR Apache-2.0 only. No GPL / no Linux .ko.
 *
 * Lean residual (this TU exclusive; Soft!=product dual license / G-AC-1):
 *   Product path: UDX class hosts (user/udx udx_host_bind_*; rtl8168_udx;
 *     xhci_udx) - dual-license userspace, never in-kernel .ko product.
 *   Bind residual: SCAN -> GET -> OPEN -> MAP_BAR
 *   Lifecycle residual (same dual-license hosts; not .ko):
 *     IRQ_BIND; DMA_NOTE; DMA_BUF_ALLOC|FREE|MAP; CLOSE; CFG_READ|WRITE
 *   Soft inventory + soft handle + soft UC map-grant note only.
 *   Cap/window honesty: MMIO_FRAME / IRQ->Notification / DMA window caps
 *     mint into host CNode remains OPEN (soft residual != product mint).
 *   Never freestanding rtl/usb class re-enable from this door
 *     (no GJ_RTL8168_PROBE / GJ_XHCI_MSC_PROBE toggle; product = UDX hosts).
 *   Dual license: MIT OR Apache-2.0. Soft!=product. Product mint OPEN.
 *   Once-lamp lean residual: opcode/bounds/layout/cap honesty + PASS|FAIL.
 *   No version stamp. No stamp storms. stamp_storm=0.
 *
 * Functional residual (preferred over soft lamps; lean toward UDX product):
 *   - MAP_BAR records a soft map-grant slot per handle+BAR (idempotent
 *     re-map returns prior VA; grant_live table recount; CLOSE forgets;
 *     share count on grant; multi-host share_forget leaves peer grants;
 *     multi-BAR residual per handle; arg3 dual-use: user map_note* /
 *     user VA hint / kernel-smoke note; not_sole_owner multi-host grants;
 *     no product MMIO_FRAME mint - mint OPEN).
 *   - IRQ_BIND stores badge on the soft handle; CLOSE uses
 *     irq_msix_soft_user_unbind(handle) (multi-slot safe; never clears
 *     other hosts; never bind(0) clear-all from door; live-before/after
 *     exact one-slot drop; multi_host_safe sync; unbind_safe residual).
 *     No product IRQ Notification cap mint - mint OPEN.
 *   - DMA_BUF_ALLOC/FREE/MAP prefer dma_buf_ddi_* (UDX DDI residual path)
 *     (force32 multi-page via ddi_alloc; map records bus cookie;
 *     window_ok / is_low / vtd_identity honesty; sync_pa residual;
 *     FREE/CLOSE unmap+sync then ddi_free; map-live + slot live counters);
 *     optional BDF window note via DMA_NOTE. No DMA window cap mint - mint OPEN.
 *   - Cap kprintf hard: residual lean / path / OPEN / MAP grant / DMA /
 *     IRQ / multi-host IRQ / CFG reject once; SCAN count once; no stamp storms;
 *     no version stamp.
 *
 * Dispatched via GJ_SYS_DDI (arg0 = opcode). Soft surface only:
 *   - PCI inventory note via devmgr_soft_pci_scan (D1 soft table)
 *   - Soft open handle + BAR map note (kernel UC VA when vmm_map_device_uc)
 *   - Soft config R/W notes; CFG_WRITE is carefully soft (no reckless live)
 *   - DMA window note -> iommu_window_grant / devmgr_soft_dma_window_note
 *   - DMA page alloc/free/map via dma_buf_* (soft residual for UDX hosts)
 *
 * Soft!=product: no live MMIO_FRAME / IRQ Notification / DMA window caps
 * mint to host CNodes (docs/DDI_SOFT.md, SECURITY_CORE_DESIGN.md ss8-9.5).
 * Soft IRQ note (DDI_OP_IRQ_BIND) records handle->badge only; delivery is
 * still the global MSI-X Notification + GJ_SYS_NOTIFY_WAIT.
 *
 * Dispatch contract:
 *   ddi_door_syscall(op, arg1, arg2, arg3) -> i64
 *   Success: 0, positive counts / handle ids / soft VA bits
 *   Errors:  negative GJ_ERR_* (INVAL / NOENT / NODEV / FAULT / NOSUPPORT / PERM)
 *   User pointers: user_range_ok + copy_to_user (kernel smoke HHDM ok).
 *
 * Greppable markers (keep stable; sparse - no stamp storms; no version stamp):
 *   ddi_door: soft residual lean
 *   ddi_door: soft residual lean PASS
 *   ddi_door: soft residual
 *   ddi_door: soft residual UDX product path
 *   ddi_door: soft residual MAP/IRQ/DMA
 *   ddi_door: soft product surface PASS
 *   ddi: soft irq note PASS
 *   ddi_door: soft irq multi-host
 *   ddi_door: soft map grant
 *   ddi_door: soft dma_buf
 *   ddi_door: soft scan count=
 *   ddi_door: soft close
 *   never_fs_rtl_usb=1
 *   product_hosts=UDX
 *   cap_mint=OPEN
 *   window_mint=OPEN
 *   never_clear_all=1
 *   not_sole_owner=1
 *   ddi_door: soft ...
 *
 * Law: dual MIT OR Apache-2.0; Soft!=product; G-AC-1. Product mint OPEN.
 * Never freestanding rtl/usb class re-enable. Product path = UDX class hosts.
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
 * OPEN: arg1=index -> soft handle id (>0) for MAP/CFG/DMA ops.
 * Soft table only; no live cap mint. Product CNode mint remains OPEN.
 */
#define DDI_OP_OPEN        3u

/**
 * MAP_BAR: arg1=handle arg2=bar_index arg3 dual-use:
 *   - user ptr to struct gj_ddi_map_note (user_range_ok) -> fill out-note
 *   - user VA hint in user band -> preferred map install VA
 *   - non-zero kernel/smoke buffer -> trusted note out via ddi_copy_out
 * Soft map note: prefer vmm_map_user_device when process current; else
 *   vmm_map_device_uc for a kernel UC VA.
 * Functional residual: soft map-grant slot per handle+BAR (idempotent);
 *   grant_live table recount / share residual / multi-BAR residual
 *   (not_sole_owner multi-host honesty).
 * Honesty: does not install a product MMIO_FRAME cap (mint OPEN).
 * Returns 0 on success (high UC VAs do not fit positive i64Ret), or low VA.
 * Soft-return of kva+length for same-AS smoke: serial greppable line
 * carries kva/cb; length also from prior GET bar*_cb; kva is typically
 * GJ_DEVICE_MMIO_BASE+pa after a successful map_device_uc.
 * Host may use that kernel UC VA only if sharing the window - not product
 * isolation. Product user map is follow-on.
 */
#define DDI_OP_MAP_BAR     4u

/**
 * CFG_READ: arg1=handle arg2=offset -> u32 config dword (aligned soft).
 */
#define DDI_OP_CFG_READ    5u

/**
 * DMA_NOTE: arg1=handle arg2=pa arg3=cb.
 * Record DMA window via iommu_window_grant if available and
 * devmgr_soft_dma_window_note (soft inventory). Not a DMA window cap mint.
 */
#define DDI_OP_DMA_NOTE    6u

/**
 * INVENTORY: kprintf once "ddi_door: soft product surface PASS"
 * (+ UDX product path residual lean once). Soft!=product dual license.
 * Cap/window mint OPEN; never freestanding rtl/usb class re-enable.
 * Returns 0; subsequent calls soft 0 without re-printing.
 */
#define DDI_OP_INVENTORY   7u

/**
 * CLOSE: arg1=handle.
 * Soft residual lifecycle: free soft handle, forget map-grant slots
 * (grant_live recount; grant_forget++; share_forget when peers retain PA),
 * multi-slot IRQ unbind via irq_msix_soft_user_unbind (this handle only;
 * never clear-all; exact one-slot drop + multi-host unbind_safe residual),
 * free soft DMA_BUF slots (unmap+sync then dma_buf_ddi_free; map-live/slot--).
 * Product revoke/Phase-A remains OPEN.
 * Opcode 8 (matches UDX UDX_DDI_OP_CLOSE).
 * greppable: ddi_door: soft close | ddi_door: soft irq multi-host
 */
#define DDI_OP_CLOSE       8u

/**
 * IRQ_BIND (soft note): arg1=handle arg2=badge_mask (0 -> GJ_MSIX_BADGE_SOFT).
 * Records soft irq->handle note via irq_msix_soft_user_bind (multi-slot).
 * Handle 0 rejected (door never clear-all). Rebind updates badge only.
 * Multi-host residual when other slots already live.
 * No hard IRQ in userspace; no product IRQ Notification cap mint
 * (Soft!=product; mint OPEN). CLOSE unbinds this handle only.
 * Host wait after MSI-X soft fire:
 *   GJ_SYS_NOTIFY_WAIT which=0 mask=badge_mask block=0|1
 * greppable: ddi: soft irq note PASS | ddi_door: soft irq multi-host
 * Opcode 9 (matches residual bind; 8 = CLOSE).
 */
#define DDI_OP_IRQ_BIND    9u

/**
 * DMA_BUF_ALLOC: arg1=handle arg2=cPages (0 -> 1) arg3=flags
 *   (bit0 = force32 / VT-d identity prefer).
 * Functional residual: prefer dma_buf_ddi_alloc (UDX DDI-shaped path);
 * window_ok / is_low / in_vtd_identity honesty. Tracks soft slot under
 * handle for CLOSE/FREE (live slot counter). Returns physical base as i64
 * when PA fits positive range; else 0 with soft table only. Not a DMA
 * window / CNode cap mint (product OPEN). greppable (sparse):
 * ddi_door: soft dma_buf
 */
#define DDI_OP_DMA_BUF_ALLOC 10u

/**
 * DMA_BUF_FREE: arg1=handle arg2=pa arg3=cPages (0 -> 1).
 * Soft unmap+sync residual (if mapped) then dma_buf_ddi_free for the slot
 * from DMA_BUF_ALLOC. Live slot-- / map-live recount. Not product IOMMU
 * revoke.
 */
#define DDI_OP_DMA_BUF_FREE  11u

/**
 * DMA_BUF_MAP: arg1=handle arg2=pa arg3=cb (dir=BIDIR soft).
 * Soft residual: window_ok precheck when force32; dma_buf_ddi_map; records
 * bus cookie on owned slot (idempotent re-map; re-MAP different cb unmaps
 * prior); dma_buf_sync_pa_for_device residual; optional devmgr/iommu window
 * note for the handle BDF (same ingredients as DMA_NOTE). Returns bus
 * address (soft identity PA) or GJ_ERR_*. Not product IOMMU window cap mint.
 */
#define DDI_OP_DMA_BUF_MAP   12u

/**
 * CFG_WRITE: arg1=handle arg2=offset arg3=u32 value.
 * Opcode 16 (sparse; leave 13..15 for future product grants).
 * Careful soft only if safe: soft-note path; no reckless live config poke
 * of identity / BARs. See ddi_door.c honesty comments.
 */
#define DDI_OP_CFG_WRITE   16u

/** Soft inventory / handle table bounds (diagnostics; not product depth). */
#define GJ_DDI_SOFT_DEV_MAX     32u
#define GJ_DDI_SOFT_HANDLE_MAX  16u
#define GJ_DDI_SOFT_BAR_MAX      6u
/** Soft DMA_BUF residual slots (handle-owned pages; not product quota). */
#define GJ_DDI_SOFT_DMA_SLOT_MAX 32u

/**
 * Packed device info for DDI_OP_GET (userspace / kernel smoke).
 * Soft fields may be zero when scan is deferred or BAR unresolved.
 * Layout must stay aligned with user/udx udx_ddi_dev_info.
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
 * Honesty: u64Va is a kernel UC VA when vmm_map_device_uc succeeds, or a
 * user-AS VA when vmm_map_user_device succeeds - not a granted product
 * MMIO_FRAME map (mint OPEN).
 * Layout must stay aligned with user/udx udx_ddi_map_note.
 */
struct gj_ddi_map_note {
    u64 u64Va;     /* kernel UC VA, user VA, or 0 */
    u64 u64Cb;     /* mapped / BAR length */
    u64 u64Pa;     /* BAR physical base */
    u32 u32Bar;    /* BAR index 0..5 */
    u32 u32Handle; /* soft handle */
    u8  u8Ok;
    u8  u8User;    /* non-zero when user-AS map residual installed */
    u8  u8Pad[2];
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
