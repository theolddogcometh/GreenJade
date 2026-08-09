/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product PCI capability scan: MSI/MSI-X presence + table programming.
 * Clean-room pure C11 freestanding (PCI Local Bus Spec). Dual MIT OR
 * Apache-2.0. No GPL source. Soft!=product. G-AC-1.
 *
 * Hardware path:
 *   Scan config space for MSI-X (and plain MSI) caps; optional enable of
 *   Message Control; program table entry 0 when table PA is resolvable
 *   (message address/data + unmask) for GJ_MSIX_IRQ_VEC delivery.
 *
 * Soft MSI-X table (always available; no MMIO required):
 *   Software shadow of Message Address/Data/Vector Control when MMIO is
 *   absent or for smokes (mask, PBA sticky, soft fire -> notify via
 *   irq_msix when ready). Depth GJ_MSIX_SOFT_TBL (bounded; not full
 *   device Table Size).
 * greppable: MSI-X table soft path
 *
 * Soft fire contract:
 *   pci_msix_soft_fire: sticky PBA bit; if unmasked + not function-masked
 *   and irq_msix ready, pulse Notification with GJ_MSIX_BADGE_TBL(idx).
 *   Masked entries set PBA but do not deliver (PCI-shaped).
 *
 * Lean soft residual (Soft!=product; freestanding rtl poll-mode first):
 *   Unmask with sticky PBA: deliver once (UDX enable_irq latch shape).
 *   Soft Function Mask: Message Control bit14-shaped global hold;
 *   clear with sticky PBA delivers unmasked programmed entries once.
 *   Soft badge mask / entry badge: GJ_MSIX_BADGE_TBL(i) scaffolding for
 *   userspace NOTIFY_WAIT masks (later UDX IRQ hosts). Soft table only -
 *   UDX freestanding maps badge bit N -> IRQ line N independently.
 *   UDX IRQ foundation path (soft only): soft_tbl -> irq_msix inject ->
 *   notify_msix_global -> NOTIFY_WAIT. product_notify_mint=OPEN (no
 *   per-host IRQ Notification CNode mint; soft_shadow only).
 *   Never forces freestanding rtl off poll-mode (hw_force_rtl=0).
 *   Hazard H1: never force IRQ eth poll; never call net_eth_poll here.
 * greppable: pci: soft residual
 * greppable: msix: soft residual
 * greppable: pci: soft residual lean
 * greppable: msix: soft residual lean
 * greppable: pci: soft residual lean PASS
 * greppable: msix: soft residual lean PASS
 * greppable: product_notify_mint=OPEN
 * greppable: force_irq_eth_poll=0 poll_mode_first=1
 * greppable: net_eth_poll=run_loop_only net_eth_irq=0
 * greppable: net_eth_poll_from_msix=0
 *
 * Greppable product markers (keep stable):
 *   MSI-X table soft path
 *   pci_msix soft table exercise PASS / probe_log product PASS markers
 */
#pragma once

#include <gj/types.h>

/** Soft shadow table depth (bounded; not full device Table Size). */
#define GJ_MSIX_SOFT_TBL 8u

/** Vector Control bit 0: Mask (PCI MSI-X). */
#define GJ_MSIX_VECCTL_MASK 1u

/**
 * Lean residual honesty constants (Soft!=product; Hazard H1; G-AC-1).
 * Soft MSI-X table residual scaffolds later UDX userspace IRQ hosts only.
 * Freestanding net may stay poll-mode; this unit never forces IRQ eth poll
 * and never calls net_eth_poll (run-loop / scheduler_run owns eth poll).
 * Compile-time locked: force=0, poll_mode_first=1 (H1 review to flip).
 * greppable: force_irq_eth_poll=0 poll_mode_first=1
 * greppable: net_eth_poll=run_loop_only net_eth_irq=0
 * greppable: net_eth_poll_from_msix=0 hw_force_rtl=0
 */
#define PCI_MSIX_FORCE_IRQ_ETH_POLL 0u
#define PCI_MSIX_POLL_MODE_FIRST    1u
/** H1: this unit never owns eth poll (run-loop only). Soft!=product. */
#define PCI_MSIX_NET_ETH_IRQ        0u
/** H1: net_eth_poll never called from pci_msix.* (soft or HW program). */
#define PCI_MSIX_NET_ETH_POLL_FROM  0u
/** Residual: never force freestanding rtl off poll-mode. Soft!=product. */
#define PCI_MSIX_HW_FORCE_RTL       0u

#if PCI_MSIX_FORCE_IRQ_ETH_POLL != 0u
#error "H1: PCI_MSIX_FORCE_IRQ_ETH_POLL must be 0"
#endif
#if PCI_MSIX_POLL_MODE_FIRST != 1u
#error "H1: PCI_MSIX_POLL_MODE_FIRST must be 1"
#endif
#if PCI_MSIX_NET_ETH_IRQ != 0u
#error "H1: PCI_MSIX_NET_ETH_IRQ must be 0"
#endif
#if PCI_MSIX_NET_ETH_POLL_FROM != 0u
#error "H1: PCI_MSIX_NET_ETH_POLL_FROM must be 0 (never call net_eth_poll)"
#endif
#if PCI_MSIX_HW_FORCE_RTL != 0u
#error "H1: PCI_MSIX_HW_FORCE_RTL must be 0 (poll-mode first)"
#endif

/**
 * MSI-X capability inventory for one device (scan/probe fill).
 * u16TableSize is Message Control N+1 entries. u64TablePa is 0 if BAR
 * resolution failed (soft path still usable).
 */
struct gj_pci_msix_info {
    u8  u8Present;
    u8  u8Enabled;
    u16 u16TableSize;  /* N table entries (Message Control + 1) */
    u8  u8Bus;
    u8  u8Slot;
    u8  u8Func;
    u8  u8TableBir;    /* BAR index for MSI-X table */
    u32 u32TableOff;   /* offset within BAR */
    u8  u8PbaBir;
    u8  u8Pad;
    u32 u32PbaOff;
    u64 u64TablePa;    /* resolved table base PA (0 if unknown) */
    u32 u32Programmed; /* non-zero if entry 0 written this boot */
};

/**
 * Soft MSI-X table entry (Message Address/Data + Vector Control).
 * Layout mirrors a 16-byte MSI-X table entry (spec dword order).
 */
struct gj_pci_msix_soft_entry {
    u32 u32MsgAddrLo;
    u32 u32MsgAddrHi;
    u32 u32MsgData;  /* low 8: vector when fixed delivery */
    u32 u32VecCtl;   /* bit0 = mask */
    u8  u8Programmed;
    u8  u8SoftFire;  /* non-zero if soft-fired at least once */
    u8  u8Pad[2];
};

/**
 * Scan PCI for MSI-X (and plain MSI) capabilities; fill up to u32Max.
 * Returns number of devices written into pOut (0 if none / null).
 */
u32 pci_msix_scan(struct gj_pci_msix_info *pOut, u32 u32Max);

/**
 * Inventory + soft enable/program; logs greppable product PASS markers.
 * Safe when no MSI-X devices are present.
 */
void pci_msix_probe_log(void);

/**
 * Software-enable MSI-X Message Control on first N devices.
 * Does not program table entries. Returns how many enabled.
 */
u32 pci_msix_enable_first(u32 u32Max);

/**
 * Program MSI-X table entry 0 on first device with a resolvable table PA:
 * message address/data + unmask. u8Vector: IDT vector (e.g. GJ_MSIX_IRQ_VEC).
 * Mirrors into the soft table. Returns 1 on success, 0 if none programmed.
 */
u32 pci_msix_program_first(u8 u8Vector);

/** How many devices had entry 0 programmed this boot (HW path). */
u32 pci_msix_programmed_count(void);

/* ---- Soft MSI-X table (always available; no MMIO required) ---- */

/** Init soft shadow table (idempotent). */
void pci_msix_soft_table_init(void);

/**
 * Program soft table entry u16Idx (0..GJ_MSIX_SOFT_TBL-1).
 * u32Mask: non-zero -> masked (VecCtl bit0). Returns 1 on success, 0 if
 * idx out of range.
 */
u32 pci_msix_soft_program(u16 u16Idx, u32 u32AddrLo, u32 u32Data, u32 u32Mask);

/**
 * Set/clear mask on soft entry. Returns 1 if entry was programmed.
 * Residual: clear (unmask) with sticky PBA set delivers once (UDX
 * enable_irq latch shape). Soft != product HW re-issue.
 */
u32 pci_msix_soft_mask(u16 u16Idx, u32 u32Mask);

/** Readback soft entry; returns 1 if programmed and pOut filled. */
u32 pci_msix_soft_read(u16 u16Idx, struct gj_pci_msix_soft_entry *pOut);

/**
 * Soft-fire entry u16Idx: sticky PBA bit; if unmasked, not function-masked,
 * and irq_msix ready, pulse Notification badge (GJ_MSIX_BADGE_TBL). Returns
 * 1 if delivery attempted (unmasked + programmed), 0 if masked/invalid.
 */
u32 pci_msix_soft_fire(u16 u16Idx);

/** Soft Pending Bit Array (bit per soft entry). */
u64 pci_msix_soft_pba(void);

/** Clear soft PBA bits in u64Mask; returns previous PBA & mask. */
u64 pci_msix_soft_pba_clear(u64 u64Mask);

/**
 * Soft residual: non-zero if sticky PBA bit is set for soft entry u16Idx.
 * UDX disable_irq latch shape (pending while masked). Soft!=product.
 */
u32 pci_msix_soft_pba_pending(u16 u16Idx);

/**
 * Soft residual Function Mask (PCI MSI-X Message Control bit 14 shape).
 * Non-zero u32Mask -> hold all soft table delivery.
 * Clear (0) with sticky PBA: deliver once per unmasked programmed entry
 * (UDX enable_irq / MC Function Mask residual shape). Soft!=product HW MC.
 * Returns 1 always after soft table ready.
 */
u32 pci_msix_soft_function_mask(u32 u32Mask);

/** Soft residual: current Function Mask state (0/1). */
u32 pci_msix_soft_function_mask_get(void);

/**
 * Soft residual: OR of GJ_MSIX_BADGE_TBL(i) for programmed soft entries.
 * Scaffolding for userspace GJ_SYS_NOTIFY_WAIT badge masks (later UDX).
 * Soft!=product per-vector Notification mint.
 */
u64 pci_msix_soft_badge_mask(void);

/**
 * Soft residual: Notification badge for soft table entry u16Idx
 * (GJ_MSIX_BADGE_TBL). 0 if idx out of range. Soft != UDX bit_N_line_N
 * product map (that lives in freestanding udx_run / irq_msix soft inject).
 */
u64 pci_msix_soft_entry_badge(u16 u16Idx);

/** Soft table programmed-entry count / lifetime soft-fire count. */
u32 pci_msix_soft_programmed_count(void);
u32 pci_msix_soft_fire_count(void);

/**
 * Soft residual: lifetime latched-pending delivers (vector unmask or
 * Function Mask clear with sticky PBA -> inject). UDX enable_irq shape.
 */
u32 pci_msix_soft_unmask_pend_count(void);

/** Non-zero after soft table init. */
int pci_msix_soft_ready(void);

/**
 * End-to-end soft table exercise: program / mask-hold / unmask-pending /
 * fire / function-mask residual / readback. Greppable PASS.
 * Lean residual inventory emits H1 honesty (poll_mode_first; no eth force).
 * Safe before or after irq_msix_init. Returns non-zero on soft path success.
 */
u32 pci_msix_soft_table_exercise(void);
