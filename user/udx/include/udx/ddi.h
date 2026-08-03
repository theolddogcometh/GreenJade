/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft DDI bind surface for freestanding GreenJade driver hosts.
 *
 * Soft ≠ product: numbers and shapes seed the userspace ↔ kernel contract
 * for cap-gated PCI attach. Live multi-server devmgr + VT-d mint is OPEN.
 * Dual MIT OR Apache-2.0 only; no GPL; no Linux .ko product.
 *
 * Freestanding path (no UDX_HOST_LIBC / GJ_FREESTANDING):
 *   udx_host_bind_scan() / udx_host_bind_by_id() → GJ_SYS_DDI (103)
 *   open → map BAR0 → register window → ioremap(granted PA)
 *
 * Host-linux path (UDX_HOST_LIBC): inject remains the lab soft path when
 * the DDI syscall is unavailable (soft SKIP). Soft fallback ≠ product.
 *
 * G752 first bind targets (inventory / HCL):
 *   10ec:8168  Realtek RTL8111/8168 Gigabit Ethernet
 *   8086:a12f  Intel 100 Series / C230 USB 3.0 xHCI (00:14.0)
 *
 * Greppable:
 *   udx: soft ddi bind PASS
 *   udx: soft ddi bind SKIP
 */
#pragma once

#include <udx/types.h>

/*
 * Native GJ_SYS_DDI — matches D2 / next free after GJ_SYS_SCSI (102).
 * Kernel may soft-stub until product dispatch is wired; UDX soft-handles
 * unknown / -ENOSYS as SKIP.
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
 * Do not renumber. UDX freestanding bind walks SCAN→GET→OPEN→MAP_BAR.
 */
#define UDX_DDI_OP_SCAN     1u /* → device count */
#define UDX_DDI_OP_GET      2u /* arg1=index arg2=user gj_ddi_dev_info* */
#define UDX_DDI_OP_OPEN     3u /* arg1=index → soft handle id */
#define UDX_DDI_OP_MAP_BAR  4u /* arg1=handle arg2=bar arg3=map_note* or va */
#define UDX_DDI_OP_CFG_READ 5u
#define UDX_DDI_OP_DMA_NOTE 6u
#define UDX_DDI_OP_INVENTORY 7u
#define UDX_DDI_OP_CLOSE    8u /* soft reserved / not in kernel yet */

/* Scan buffer bound (soft). */
#define UDX_DDI_SCAN_MAX    32u

/* G752 first bind targets (soft lab / inventory). */
#define UDX_DDI_G752_RTL8168_VEND  0x10ecu
#define UDX_DDI_G752_RTL8168_DEV   0x8168u
#define UDX_DDI_G752_XHCI_VEND     0x8086u
#define UDX_DDI_G752_XHCI_DEV      0xa12fu

/**
 * Packed device info — layout matches kernel struct gj_ddi_dev_info
 * (DDI_OP_GET). UDX never walks CF8/CFC itself.
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
 */
struct udx_ddi_map_note {
    u64 u64Va;
    u64 u64Cb;
    u64 u64Pa;
    u32 u32Bar;
    u32 u32Handle;
    u8  u8Ok;
    u8  u8Pad[3];
};

/* Legacy aliases for older soft notes. */
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
 * This header is the soft ABI shape + G752 target catalog only.
 */
