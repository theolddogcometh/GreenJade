/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product PCI capability scan: MSI/MSI-X presence + table programming.
 * Clean-room pure C (PCI Local Bus Spec). No GPL source.
 */
#pragma once

#include <gj/types.h>

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
 * Returns 1 on success, 0 if none programmed.
 */
u32 pci_msix_program_first(u8 u8Vector);

/** How many devices had entry 0 programmed this boot. */
u32 pci_msix_programmed_count(void);
