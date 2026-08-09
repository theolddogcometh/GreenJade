/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft DDI bind surface for freestanding GreenJade driver hosts.
 *
 * Soft != product: numbers and shapes seed the userspace <-> kernel contract
 * for cap-gated PCI attach. Live multi-server devmgr + VT-d mint is OPEN.
 * Dual MIT OR Apache-2.0 only; no GPL; no Linux .ko product (G-AC-1).
 *
 * ---------------------------------------------------------------------------
 * Soft API honesty (Dual DoD A/B OPEN; Soft!=product; G-AC-1)
 * ---------------------------------------------------------------------------
 * soft path (this header + GJ_SYS_DDI door residual):
 *   SCAN / GET inventory note
 *   OPEN soft handle (table id only; not a CNode cap)
 *   CFG_READ soft config dword snap (no silicon program claim)
 *   MAP_BAR soft map-grant note (UC VA / user-AS residual; not MMIO_FRAME)
 *   MAP_REMAP residual = second MAP_BAR on same handle+BAR (idempotent
 *     grant slot honesty; NOT a distinct opcode — still UDX_DDI_OP_MAP_BAR)
 *   DMA_NOTE soft window inventory (not DMA window / CNode mint)
 *   IRQ_BIND soft handle→badge note (not IRQ Notification cap mint)
 *   DMA_BUF_ALLOC|FREE|MAP soft page residual (not IOMMU window mint)
 *   CLOSE soft handle free / grant forget (not product Phase-A revoke)
 *
 * product path (OPEN — not claimed by soft residual or this catalog):
 *   Dual DoD A = UDX USB  — xhci_udx    (8086:a12f) datapath / BOT-MSC
 *   Dual DoD B = UDX NIC  — rtl8168_udx (10ec:8168) TX/RX
 *   mint OPEN: MMIO_FRAME cap · IRQ→Notification cap · DMA window cap
 *   Soft SCAN/GET/OPEN/MAP_BAR lamps + life= mask never close Dual DoD A/B.
 *   Compile-time residual: UDX_DDI_DOD_A_OPEN=1 UDX_DDI_DOD_B_OPEN=1
 *     UDX_DDI_CAP_MINT=0 UDX_DDI_WINDOW_MINT=0 dual_dod_open=1.
 *
 * Freestanding path (no UDX_HOST_LIBC / GJ_FREESTANDING):
 *   udx_host_bind_scan() / udx_host_bind_by_id() -> GJ_SYS_DDI (103)
 *   residual chain (product hosts rtl8168_udx / xhci_udx):
 *     SCAN → GET → OPEN → CFG_READ → MAP_BAR* (preferred) → MAP_REMAP
 *     → DMA_NOTE → IRQ_BIND → window_register → install_granted → probe
 *   Core greppable chain remains SCAN,GET,OPEN,MAP_BAR (side residuals
 *   deepen honesty; handle is retained — product hosts do NOT CLOSE).
 *
 * Host-linux path (UDX_HOST_LIBC): inject remains the lab soft path when
 * the DDI syscall is unavailable (soft SKIP). Soft fallback != product.
 *
 * Product direction: freestanding kernel class drivers SKIP; product =
 * Linux-shaped userspace UDX hosts (rtl8168_udx / xhci_udx) over Cap
 * MMIO/IRQ/DMA grants (soft honesty: mint OPEN today). product=UDX+ABI.
 * Never freestanding rtl/usb class re-enable from this catalog
 * (no GJ_RTL8168_PROBE / GJ_XHCI_MSC_PROBE toggle).
 *
 * Product residual (C2; Dual DoD B OPEN; stamp-free bar v2026.08.04.75):
 *   product=UDX+sshd+stack — DDI bind residual seeds rtl8168_udx so
 *   netstackd (stack) + sshd (:22) can product-serve when Cap mint lands.
 *   Soft SCAN/GET/OPEN/MAP_BAR != wire / Dual DoD B close (agent!=close).
 * greppable: udx: soft product residual product=UDX+sshd+stack
 *
 * G752 first bind targets (inventory / HCL; Dual DoD seed IDs):
 *   10ec:8168  Realtek RTL8111/8168 — preferred BARs 0 + 2  (DoD B seed)
 *   8086:a12f  Intel 100 Series / C230 USB 3.0 xHCI — BAR 0  (DoD A seed)
 * Soft bind of these IDs = inventory residual only; Dual DoD A/B stay OPEN.
 *
 * Greppable:
 *   udx: soft ddi bind PASS
 *   udx: soft ddi bind SKIP
 *   udx: soft ddi bind residual
 *   udx: soft ddi bind residual path
 *   udx: soft ddi residual MAP_BAR
 *   udx: soft ddi residual CFG
 *   udx: soft ddi residual MAP_REMAP
 *   udx: soft ddi residual DMA_NOTE
 *   udx: soft ddi residual IRQ_BIND
 *   udx: soft residual freestanding class SKIP
 *   udx: soft residual product=UDX+ABI
 *   udx: soft product residual product=UDX+sshd+stack
 *   udx: soft ddi api honesty
 *   Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod_open=1
 *   cap_mint=0 window_mint=0 (OPEN) product_mint=OPEN
 *   handle_retain=1 close=0
 *   product_hosts=UDX never_fs_rtl_usb=1 fs_class=SKIP
 *   multi_server=0 confine=0 G-AC-1
 *
 * Law: dual MIT OR Apache-2.0; Soft!=product; G-AC-1; Dual DoD A/B OPEN;
 * no freestanding class re-enable; no version stamp; no stamp storms.
 * Bar honesty v2026.08.04.75 stamp-free. NEVER bump GJ_IMAGE_VERSION.
 */
#pragma once

#include <udx/types.h>

/*
 * Native GJ_SYS_DDI — matches D2 / next free after GJ_SYS_SCSI (102).
 * Kernel may soft-stub until product dispatch is wired; UDX soft-handles
 * unknown / -ENOSYS as SKIP. Soft syscall success != Dual DoD A/B close.
 */
#ifndef GJ_SYS_DDI
#define GJ_SYS_DDI 103
#endif

/* UDX local alias (freestanding helpers; same number). */
#ifndef UDX_GJ_SYS_DDI
#define UDX_GJ_SYS_DDI GJ_SYS_DDI
#endif

/*
 * Soft DDI opcodes (arg0) — MUST match kernel/include/gj/ddi_door.h DDI_OP_*.
 * Do not renumber. Layout / number honesty is the ABI seed for product hosts.
 *
 * UDX freestanding bind residual walks:
 *   SCAN → GET → OPEN → MAP_BAR (preferred BARs for product hosts)
 *   + soft side residual: CFG_READ / MAP_REMAP / DMA_NOTE / IRQ_BIND.
 * MAP_REMAP is residual honesty only: re-issue MAP_BAR (same opcode 4) on
 * the first mapped preferred BAR; door grant slot is idempotent. There is
 * no UDX_DDI_OP_MAP_REMAP number.
 *
 * Lifecycle residual opcodes seed product UDX hosts; soft mint OPEN.
 * Product hosts retain the OPEN handle (no CLOSE on install path).
 * Every grant-ish op below: soft table / note only — cap_mint=OPEN.
 *
 * Dispatch honesty (matches ddi_door_syscall):
 *   Success: 0, positive counts / handle ids / soft VA bits when they fit i64
 *   Errors:  negative (INVAL / NOENT / NODEV / FAULT / NOSUPPORT / PERM)
 *   Soft SKIP when door absent / -ENOSYS (host-linux inject path).
 */
#define UDX_DDI_OP_SCAN          1u /* → soft device count (inventory note) */
#define UDX_DDI_OP_GET           2u /* arg1=index arg2=user udx_ddi_dev_info* */
#define UDX_DDI_OP_OPEN          3u /* arg1=index → soft handle id (>0); no CNode mint */
#define UDX_DDI_OP_MAP_BAR       4u /* arg1=handle arg2=bar arg3=map_note*|VA hint; mint OPEN */
#define UDX_DDI_OP_CFG_READ      5u /* arg1=handle arg2=offset → u32 dword (soft snap) */
#define UDX_DDI_OP_DMA_NOTE      6u /* arg1=handle arg2=pa arg3=cb soft window note; mint OPEN */
#define UDX_DDI_OP_INVENTORY     7u /* once-lamp product surface residual; mint OPEN */
#define UDX_DDI_OP_CLOSE         8u /* soft handle free / map-grant forget; not Phase-A */
#define UDX_DDI_OP_IRQ_BIND      9u /* soft irq note handle→badge; Notification mint OPEN */
#define UDX_DDI_OP_DMA_BUF_ALLOC 10u /* arg1=handle arg2=cPages arg3=flags; window mint OPEN */
#define UDX_DDI_OP_DMA_BUF_FREE  11u /* arg1=handle arg2=pa arg3=cPages; soft free only */
#define UDX_DDI_OP_DMA_BUF_MAP   12u /* arg1=handle arg2=pa arg3=cb; bus cookie residual */
/* 13..15 reserved for future product grants (leave sparse; do not renumber) */
#define UDX_DDI_OP_CFG_WRITE     16u /* careful soft; reject unsafe identity / BAR poke */

/*
 * Soft table bounds (match kernel GJ_DDI_SOFT_*; Soft!=product quota).
 * Diagnostics / residual depth only — not product confine limits.
 */
#define UDX_DDI_SCAN_MAX       32u /* soft; matches GJ_DDI_SOFT_DEV_MAX */
#define UDX_DDI_HANDLE_MAX     16u /* soft; matches GJ_DDI_SOFT_HANDLE_MAX */
#define UDX_DDI_BAR_MAX         6u /* PCI type-0 BAR indices 0..5 */
#define UDX_DDI_DMA_SLOT_MAX   32u /* soft; matches GJ_DDI_SOFT_DMA_SLOT_MAX */
#define UDX_DDI_PREF_BAR_MAX    4u /* preferred BAR residual depth (RTL uses 0+2) */

/*
 * DMA_BUF_ALLOC arg3 bit0 (match kernel DDI_DMA_BUF_F_FORCE32 /
 * UDX_DMA_BUF_F_FORCE32 in dma.h). Geometric / VT-d identity prefer note
 * only — Soft!=product; does not mint a DMA window cap.
 */
#define UDX_DDI_DMA_BUF_F_FORCE32  1u

/* G752 first bind targets (soft lab / inventory → product UDX hosts). */
#define UDX_DDI_G752_RTL8168_VEND  0x10ecu
#define UDX_DDI_G752_RTL8168_DEV   0x8168u
#define UDX_DDI_G752_XHCI_VEND     0x8086u
#define UDX_DDI_G752_XHCI_DEV      0xa12fu

/*
 * Dual DoD A/B catalog (API honesty; Soft residual != close).
 *
 *   Dual DoD A (UDX USB): xhci_udx    @ UDX_DDI_G752_XHCI_*    — OPEN
 *   Dual DoD B (UDX NIC): rtl8168_udx @ UDX_DDI_G752_RTL8168_* — OPEN
 *
 * Close criteria live on product userspace UDX datapath + live cap mint
 * (MMIO_FRAME / IRQ Notification / DMA window), not soft door notes,
 * not freestanding class stages, not life= residual masks.
 * Lamps / LIFE_* / bind PASS never flip UDX_DDI_DOD_*_OPEN to closed.
 * greppable: Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod_open=1
 */
#define UDX_DDI_DOD_A_HOST_NAME  "xhci_udx"
#define UDX_DDI_DOD_B_HOST_NAME  "rtl8168_udx"

/*
 * Soft residual Cap / Dual DoD honesty constants (eng only; Soft!=product).
 * Always-0 mint/server/confine flags match host soft residual lean emit.
 * Dual DoD A/B remain OPEN (1) until DUT proof closes them — soft door
 * residual, life= masks, and bind PASS lamps alone never flip these.
 * Product hosts retain OPEN handle (close=0 on bind PASS).
 * never freestanding rtl/usb class re-enable (G-AC-1; no .ko product).
 * Grep: UDX_DDI_CAP_MINT / UDX_DDI_DOD_A_OPEN / UDX_DDI_DOD_B_OPEN /
 *       dual_dod_open=1 / handle_retain=1 / never_fs_rtl_usb=1
 */
#define UDX_DDI_CAP_MINT          0u /* no MMIO/IRQ/DMA CNode mint today */
#define UDX_DDI_WINDOW_MINT       0u /* no DMA window / VT-d mint today */
#define UDX_DDI_DOD_A_OPEN        1u /* xhci_udx Dual DoD A still OPEN */
#define UDX_DDI_DOD_B_OPEN        1u /* rtl8168_udx Dual DoD B still OPEN */
#define UDX_DDI_HANDLE_RETAIN     1u /* install residual keeps OPEN handle */
#define UDX_DDI_CLOSE_ON_BIND     0u /* no CLOSE residual on bind PASS */
#define UDX_DDI_FS_RTL_USB        0u /* never freestanding rtl/usb re-enable */
#define UDX_DDI_MULTI_SERVER      0u /* soft residual; not product multi-server */
#define UDX_DDI_CONFINE           0u /* soft residual; not product confine */
/*
 * Product residual seed (Soft!=product; Dual DoD B OPEN; stamp-free).
 * product=UDX+sshd+stack — DDI residual binds rtl8168_udx for stack+sshd.
 * greppable: udx: soft product residual product=UDX+sshd+stack
 * greppable: UDX_DDI_PRODUCT_UDX_SSHD_STACK Dual_DoD_B=OPEN
 */
#define UDX_DDI_PRODUCT_UDX_SSHD_STACK  1u /* catalog honesty only; never close */
#define UDX_DDI_PRODUCT_CHAIN_SSHD      1u /* sshd :22 product residual seed */
#define UDX_DDI_PRODUCT_CHAIN_STACK     1u /* netstackd residual seed */

/*
 * Product-host preferred BAR residual (Soft!=product; matches main/ddi_host):
 *   rtl8168_udx (10ec:8168) Dual DoD B seed: BAR0 (DDI grant) + BAR2 (silicon MMIO)
 *   xhci_udx    (8086:a12f) Dual DoD A seed: BAR0 (HC MMIO)
 * Other IDs: BAR0 only until product host names them.
 * Soft MAP of preferred BARs never claims Dual DoD A/B closed
 * (UDX_DDI_DOD_A_OPEN / UDX_DDI_DOD_B_OPEN stay 1).
 */
#define UDX_DDI_RTL_PREF_BAR0  0u
#define UDX_DDI_RTL_PREF_BAR2  2u
#define UDX_DDI_XHCI_PREF_BAR0 0u
/* Preferred BAR counts for Dual DoD seed hosts (soft residual depth). */
#define UDX_DDI_RTL_PREF_BAR_N   2u /* BAR0 + BAR2 */
#define UDX_DDI_XHCI_PREF_BAR_N  1u /* BAR0 only */

/*
 * Soft residual CFG offsets (PCI type-0; soft snap only — no product poke).
 * Identity dword0 + command/status dword1 deepen OPEN residual honesty.
 * CFG_WRITE (op 16) carefully soft-rejects unsafe identity / BAR writes.
 */
#define UDX_DDI_CFG_OFF_IDENT  0x00u
#define UDX_DDI_CFG_OFF_CMDST  0x04u

/*
 * Soft MSI-X badge residual (matches kernel GJ_MSIX_BADGE_SOFT = bit 0).
 * IRQ_BIND with badge=0 also selects soft default; UDX passes bit0 explicitly
 * for greppable residual lamps. Soft!=product Notification mint OPEN.
 * Product wait residual after soft fire: NOTIFY_WAIT which=0 mask=badge.
 */
#define UDX_DDI_IRQ_BADGE_SOFT  1ull

/*
 * Soft residual life bitmask (functional residual vs door life=).
 * Bits set when corresponding soft surface soft-ok (UDX_DDI_CAP_MINT=0).
 * Core product-host chain: SCAN|GET|OPEN|MAP (+ WIN|INSTALL on PASS).
 * Side residual: CFG_R|REMAP|DMA_NOTE|IRQ (deepen honesty only).
 * Side + core never claim Dual DoD A/B close (UDX_DDI_DOD_*_OPEN=1).
 * life= peak is diagnostics only — Soft!=product, Soft!=Dual DoD closed.
 * greppable: life=0x... core_ok= dual_dod_open=1 on residual path lamps.
 */
#define UDX_DDI_LIFE_SCAN      (1u << 0)
#define UDX_DDI_LIFE_GET       (1u << 1)
#define UDX_DDI_LIFE_OPEN      (1u << 2)
#define UDX_DDI_LIFE_CFG_R     (1u << 3)
#define UDX_DDI_LIFE_MAP       (1u << 4)
#define UDX_DDI_LIFE_REMAP     (1u << 5) /* second MAP_BAR residual (idempotent) */
#define UDX_DDI_LIFE_DMA_NOTE  (1u << 6)
#define UDX_DDI_LIFE_IRQ       (1u << 7)
#define UDX_DDI_LIFE_WIN       (1u << 8)
#define UDX_DDI_LIFE_INSTALL   (1u << 9)
/* Core SCAN→GET→OPEN→MAP_BAR residual (product host install path). */
#define UDX_DDI_LIFE_CORE \
    (UDX_DDI_LIFE_SCAN | UDX_DDI_LIFE_GET | UDX_DDI_LIFE_OPEN | \
     UDX_DDI_LIFE_MAP | UDX_DDI_LIFE_WIN | UDX_DDI_LIFE_INSTALL)
/* Side residual honesty only — never Dual DoD close / cap mint. */
#define UDX_DDI_LIFE_SIDE \
    (UDX_DDI_LIFE_CFG_R | UDX_DDI_LIFE_REMAP | \
     UDX_DDI_LIFE_DMA_NOTE | UDX_DDI_LIFE_IRQ)

/**
 * Packed device info — layout matches kernel struct gj_ddi_dev_info
 * (DDI_OP_GET). UDX never walks CF8/CFC itself.
 * Soft fields may be zero when scan is deferred or BAR unresolved.
 * Soft inventory only — GET success != Dual DoD A/B close / cap mint.
 *
 * Layout honesty (packed soft ABI; keep stable):
 *   bus/slot/func + pad0 + vend/dev + class + 6×(bar_pa, bar_cb)
 *   sizeof residual target: 112 bytes on LP64 (8-byte aligned u64 bars).
 */
struct udx_ddi_dev_info {
    u8  u8Bus;
    u8  u8Slot;
    u8  u8Func;
    u8  u8Pad0;
    u16 u16Vend;
    u16 u16Dev;
    u32 u32Class;
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
 * MAP_BAR out-note — layout matches kernel struct gj_ddi_map_note.
 * After success, UDX registers the window so udx_ioremap(phys,len) works.
 * u8User: non-zero when kernel installed a user-AS UC map residual
 * (vmm_map_user_device path); Soft!=product MMIO_FRAME mint OPEN.
 *
 * Honesty:
 *   u64Va is soft kernel UC VA or user-AS VA — not a product MMIO_FRAME map.
 *   arg3 dual-use (door): map_note* | user VA hint | kernel-smoke note.
 *   Re-MAP same handle+BAR (MAP_REMAP residual) is idempotent grant slot;
 *   prior VA match deepens honesty (remap_idem) without claiming mint.
 *   sizeof residual target: 40 bytes on LP64.
 */
struct udx_ddi_map_note {
    u64 u64Va;
    u64 u64Cb;
    u64 u64Pa;
    u32 u32Bar;
    u32 u32Handle;
    u8  u8Ok;
    u8  u8User;  /* non-zero when user-AS map residual installed */
    u8  u8Pad[2];
};

/* Legacy aliases for older soft notes (host inject / skeleton residual). */
struct udx_ddi_map_out {
    u64 u64Phys;
    u64 u64Len;
    u64 u64Va;
};

struct udx_ddi_open_out {
    u32 u32Handle;
    u32 u32Irq;
    u16 u16Vendor;
    u16 u16Device;
    u8  u8Bus;
    u8  u8Devfn;
    u8  u8Pad[2];
    u64 aBarPhys[6];
    u64 aBarLen[6];
    u8  aBarIsMem[6];
    u8  u8Pad2[2];
};

/*
 * Bind APIs live in host.h (udx_host_bind_scan / udx_host_bind_by_id).
 * This header is the soft ABI shape + G752 / product-host target catalog +
 * Dual DoD A/B OPEN honesty for Cap MMIO/IRQ/DMA mint.
 *
 * greppable: udx: soft ddi api honesty
 *   soft=SCAN,GET,OPEN,MAP_BAR,CFG,DMA_NOTE,IRQ_BIND,DMA_BUF,CLOSE
 *   chain=SCAN,GET,OPEN,MAP_BAR side=CFG,MAP_REMAP,DMA_NOTE,IRQ_BIND
 *   product_mint=OPEN Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod_open=1
 *   hosts=rtl8168_udx,xhci_udx product=UDX+ABI fs_class=SKIP
 *   cap_mint=0 window_mint=0 (OPEN) handle_retain=1 close=0
 *   never_fs_rtl_usb=1 multi_server=0 confine=0 G-AC-1 Soft!=product
 */
