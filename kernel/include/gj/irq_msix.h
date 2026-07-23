/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product MSI-X vector → Notification bind (hard IRQ + soft pulse paths).
 * Clean-room pure C. No GPL source.
 *
 * Soft pulse path: inject / hw-sim / table-soft fire → badge OR on the
 * global MSI-X Notification (no self-IPI during early kmain).
 * greppable: MSI-X soft pulse path
 */
#pragma once

#include <gj/types.h>

/** Product MSI-X delivery vector (IDT). Matches pci_msix_probe_log default. */
#define GJ_MSIX_IRQ_VEC 0x41u

/** Soft-inject default badge bit (bit 0 = any / soft). */
#define GJ_MSIX_BADGE_SOFT (1ull << 0)
/** Hw-sim pulse badge bit (distinct from soft). */
#define GJ_MSIX_BADGE_HW (1ull << 1)
/**
 * Soft table entry → badge bit. Entry 0 uses bit 2 so smoke masks 0x7
 * (bits 0..2) still observe table-soft fire of entry 0.
 */
#define GJ_MSIX_BADGE_TBL(idx) (1ull << (2u + ((u32)(idx) % 61u)))

/** Path tags for last-pulse attribution (stats only). */
#define GJ_MSIX_PATH_NONE  0u
#define GJ_MSIX_PATH_SOFT  1u
#define GJ_MSIX_PATH_HW    2u
#define GJ_MSIX_PATH_IRQ   3u
#define GJ_MSIX_PATH_TBL   4u

/** Install IDT gate + bind global Notification. Call after idt_init + APIC. */
void irq_msix_init(void);

/** C handler for MSI-X vector (from isr stub); signals Notification + EOI. */
void irq_msix_handler(void);

/** Software inject (smoke / host inject without device fire). */
void irq_msix_soft_inject(u64 u64Badge);

/**
 * Soft pulse path: inject badge on the MSI-X Notification, track last badge
 * and path tag, return cumulative soft pulse-path count (0 if not ready).
 * greppable: MSI-X soft pulse path
 */
u32 irq_msix_soft_pulse_path(u64 u64Badge);

/**
 * Hw-sim pulse: Notification-side of a hardware IRQ without self-IPI/EOI
 * (early kmain self-IPI risks #DF). Returns cumulative hw pulse count.
 * Live device IRQs use the installed IDT gate → irq_msix_handler.
 */
u32 irq_msix_hw_pulse(void);

/**
 * Soft table → Notification pulse: fire soft MSI-X entry u16Idx via
 * pci_msix_soft_fire (PBA + mask gate) then confirm path stats.
 * Returns 1 if delivery attempted, 0 otherwise.
 */
u32 irq_msix_soft_table_pulse(u16 u16Idx);

/**
 * Soft path exercise: multi-badge OR inject + pending poll + table soft fire.
 * Greppable PASS. Call after irq_msix_init (or no-ops cleanly if not ready).
 * Returns non-zero on success.
 */
u32 irq_msix_soft_path_exercise(void);

u32 irq_msix_count(void);
u32 irq_msix_soft_count(void);
u32 irq_msix_hw_count(void);
u32 irq_msix_soft_pulse_path_count(void);
u32 irq_msix_table_pulse_count(void);
u64 irq_msix_last_badge(void);
u32 irq_msix_last_path(void);
int irq_msix_ready(void);
