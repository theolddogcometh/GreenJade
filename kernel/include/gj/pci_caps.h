/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product PCI capability scan: MSI/MSI-X presence + table programming.
 * Clean-room pure C (PCI Local Bus Spec). No GPL source.
 *
 * Soft MSI-X table: software shadow of Message Address/Data/Vector Control
 * when MMIO is absent or for smokes (mask, PBA sticky, soft fire → notify).
 * greppable: MSI-X table soft path
 */
#pragma once

#include <gj/types.h>

/** Soft shadow table depth (bounded; not full device Table Size). */
#define GJ_MSIX_SOFT_TBL 8u

/** Vector Control bit 0: Mask (PCI MSI-X). */
#define GJ_MSIX_VECCTL_MASK 1u

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

/** Scan PCI for MSI-X (and plain MSI) capabilities; fill up to u32Max. */
u32 pci_msix_scan(struct gj_pci_msix_info *pOut, u32 u32Max);

/** Inventory + soft enable/program; logs greppable product PASS markers. */
void pci_msix_probe_log(void);

/**
 * Software-enable MSI-X Message Control on first N devices.
 * Does not program table entries.
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
 * u32Mask: non-zero → masked (VecCtl bit0). Returns 1 on success.
 */
u32 pci_msix_soft_program(u16 u16Idx, u32 u32AddrLo, u32 u32Data, u32 u32Mask);

/** Set/clear mask on soft entry. Returns 1 if entry was programmed. */
u32 pci_msix_soft_mask(u16 u16Idx, u32 u32Mask);

/** Readback soft entry; returns 1 if programmed and pOut filled. */
u32 pci_msix_soft_read(u16 u16Idx, struct gj_pci_msix_soft_entry *pOut);

/**
 * Soft-fire entry u16Idx: sticky PBA bit; if unmasked and irq_msix ready,
 * pulse Notification badge bit (idx). Returns 1 if delivery attempted
 * (unmasked + programmed), 0 if masked/invalid.
 */
u32 pci_msix_soft_fire(u16 u16Idx);

/** Soft Pending Bit Array (bit per soft entry). */
u64 pci_msix_soft_pba(void);

/** Clear soft PBA bits in u64Mask; returns previous PBA & mask. */
u64 pci_msix_soft_pba_clear(u64 u64Mask);

u32 pci_msix_soft_programmed_count(void);
u32 pci_msix_soft_fire_count(void);
int pci_msix_soft_ready(void);

/**
 * End-to-end soft table exercise: program / mask-hold / unmask / fire /
 * readback. Greppable PASS. Safe before or after irq_msix_init.
 * Returns non-zero on soft path success.
 */
u32 pci_msix_soft_table_exercise(void);
