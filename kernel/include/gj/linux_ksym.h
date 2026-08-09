/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft Linux-style kernel symbol table for resolving .ko undefined symbols
 * (soft-loaded modules, eng) and a greppable name surface for ABI
 * hostability eng + future userspace binding (UDX / Linux-shaped hosts).
 * Clean-room dual MIT OR Apache-2.0 only. Soft!=product Linux ABI
 * completeness. Not Linux source / not GPL. Stubs are no-ops for soft
 * module-path development.
 *
 * G-AC-1: no .ko runs in kernel as product. ksym never claims a G-AC-1
 * waiver: resolve-for-eng != product .ko wire / != in-kernel .ko init.
 * freestanding_no_exec honesty: symbol resolve supports soft load under
 * peer RUN_INIT=0 / SKIP exec; never_exec_ko stance is not waived here.
 *
 * Capacity sized for base stubs + large soft seeds (usb soft seed n=191:
 * usbcore+scsi_mod+sg+msc_leaf) + freestanding msc residual generics
 * (LINUX_KSYM_USB_STORAGE_RESIDUAL) + freestanding net residual
 * (LINUX_KSYM_NET_RESIDUAL) + freestanding pci residual
 * (LINUX_KSYM_PCI_RESIDUAL) + multi-mod export headroom. Soft!=product.
 *
 * Residual lean (C0 soft eng; Soft!=product; G-AC-1; no stamp storms):
 * boot emits two lamps only - residual lean (usb/net/pci/leaf folded +
 * freestanding_no_exec honesty) + soft init PASS (n/max/free/seed only).
 * No per-class residual kprintf flood, no version stamp, no ret*angle
 * storm class. Dual MIT OR Apache-2.0.
 *
 * Greppable (implementation in kernel/mm/linux_ksym.c) - lean residual:
 *   linux_ksym: soft residual lean usb=... net=... pci=... leaf=...
 *     freestanding_no_exec=1 never_exec_ko=1 resolve_only=1 ...
 *   linux_ksym: soft init PASS n=... max=... free=... freestanding_no_exec=1 ...
 *   linux_ksym: soft inventory ... (on-demand; not boot residual flood)
 *   linux_ksym: soft has miss name=...   (first miss only; not every lookup)
 * Per-class residual sizes remain compile-time constants (usb/net/pci/leaf);
 * registrations stay; only boot lamps are lean. Soft!=product; G-AC-1;
 * g_ac1_waiver=0 always (never claim waiver).
 */
#pragma once

#include <gj/types.h>

/**
 * Fixed soft table capacity (name -> address slots).
 * Headroom: base ~280 empty stubs + leaf15 + usb residual62 + net residual48 +
 * pci residual32 + LINUX_KSYM_USB_SOFT_SEED (191) + multi-mod export / future deepen.
 * Soft!=product. MAX already 4096 - do not thrash capacity.
 */
#define LINUX_KSYM_MAX 4096u

/**
 * Large soft USB seed size (linux_usb_soft: usb=84 scsi=58 sg=11 msc_leaf=38).
 * Inventory / headroom lamps reserve this many free slots as design budget.
 * Soft!=product; not a hard gate on product paths.
 */
#define LINUX_KSYM_USB_SOFT_SEED 191u

/**
 * usb-storage soft leaf stub count (generics only; not full usbcore/scsi).
 * Folded into residual lean lamp (leaf=). Soft!=product.
 */
#define LINUX_KSYM_USB_STORAGE_LEAF 15u

/**
 * Freestanding usb-storage residual UND/FAIL KSYM class size (generics only:
 * sg_*, kthread_*, param_ops_*, wait/workqueue helpers, ...). Does NOT include
 * usb_* or scsi_* (those stay with linux_usb_soft so need=usbcore FAIL stays
 * honest). Soft!=product; leaf resolve honesty != stick datapath claim.
 * Name surface also useful for future userspace USB host binding story.
 * Dual DoD eng expand: +16 sg table/copy + wait/timer/wq (50), then +12
 * wq/completion/sg/sysfs generics for uas-adjacent eng (62 total).
 */
#define LINUX_KSYM_USB_STORAGE_RESIDUAL 62u

/**
 * Freestanding net residual UND/FAIL KSYM class size (skb / xmit / netdev
 * lifecycle generics). Peer linux_netdev_soft may replace these with soft
 * bodies when inited. Does NOT claim .ko wire ownership (G-AC-1). Sparse
 * residual only. Soft!=product; eng + future userspace net ABI name surface.
 * Dual DoD eng expand: +18 skb/queue/stats/rtnl (36), then +12 napi/skb/netdev
 * eng helpers (48 total).
 */
#define LINUX_KSYM_NET_RESIDUAL 48u

/**
 * Freestanding PCI residual UND/FAIL KSYM class size (pci config / BAR /
 * drvdata / irq vector / dma map-error helpers). Peer linux_pci_soft may
 * replace when inited. Soft!=product; UDX host + soft layout name surface.
 * Does NOT claim real BAR/MMIO ownership (G-AC-1). Sparse residual only.
 * Dual DoD eng expand: +12 msi/msix/selected-regions/dma-mask helpers (32 total).
 */
#define LINUX_KSYM_PCI_RESIDUAL 32u

/**
 * Initialize table and register starter soft stubs.
 * Idempotent. Lean residual: one residual-lean lamp (freestanding_no_exec
 * honesty + class sizes) + soft init PASS (no per-class residual stamp
 * storm; no version stamp). Soft!=product; G-AC-1; never G-AC-1 waiver.
 * Supports soft-loaded module symbol resolve for eng / ABI hostability only.
 */
void linux_ksym_init(void);

/**
 * Register (or replace) a symbol by name.
 * @return 0 on success; -1 if name/addr NULL, empty name, or table full
 *         (replace of an existing name always succeeds).
 */
int linux_ksym_register(const char *szName, void *pAddr);

/**
 * Lookup symbol address by name.
 * @return address, or NULL if missing / not inited.
 */
void *linux_ksym_lookup(const char *szName);

/**
 * Soft presence check (1 if registered non-NULL addr, else 0).
 * Soft!=product; used by soft-loaded module pre-resolve / load-order helpers
 * under freestanding_no_exec (resolve honesty; never executes .ko init).
 * Optional first-miss diagnostic: linux_ksym: soft has miss name=...
 * (rate-limited; not every lookup). Does not alter SKIP exec semantics.
 * Never claims G-AC-1 waiver.
 */
int linux_ksym_has(const char *szName);

/** Number of registered entries. */
u32 linux_ksym_count(void);

/** Remaining free slots (0 if full or not inited). */
u32 linux_ksym_slots_free(void);

/**
 * On-demand greppable soft inventory + headroom (lean single line).
 * Not part of boot residual flood - callers refresh free= after seeds.
 * Residual lean remains init-only (storm=0). No version stamp.
 * Soft!=product; freestanding_no_exec honesty; g_ac1_waiver=0.
 */
void linux_ksym_soft_inventory(void);
