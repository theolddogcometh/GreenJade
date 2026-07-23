/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product MSI-X vector → Notification bind (hard IRQ + soft pulse paths).
 * Clean-room pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL source.
 *
 * Delivery model:
 *   IDT gate @ GJ_MSIX_IRQ_VEC (0x41) → irq_msix_handler →
 *   notify_pulse on global MSI-X Notification (notify_msix_global) + EOI.
 *   Soft paths inject the same Notification without self-IPI during early
 *   kmain (self-IPI risks #DF before APIC/IDT fully settled).
 *
 * Soft pulse path: inject / hw-sim / table-soft fire → badge OR on the
 * global MSI-X Notification. Path tags attribute last pulse for stats.
 * greppable: MSI-X soft pulse path
 *
 * Badge bits (OR into Notification pending):
 *   GJ_MSIX_BADGE_SOFT  — soft inject (bit 0)
 *   GJ_MSIX_BADGE_HW    — hw-sim pulse (bit 1)
 *   GJ_MSIX_BADGE_TBL(i)— soft table entry i (bit 2+i%61) so smoke masks
 *                         0x7 still observe table-soft fire of entry 0
 *
 * Order: call irq_msix_init after idt_init + APIC (+ notify_msix_init).
 * pci_msix soft table fire routes through this path when ready.
 *
 * Greppable product markers (keep stable):
 *   MSI-X soft pulse path
 *   notify: MSI-X IRQ PASS (with notify bind)
 *   irq_msix soft path exercise PASS
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

/**
 * Install IDT gate + bind global Notification.
 * Call after idt_init + APIC (+ notify_msix_init). Idempotent soft.
 */
void irq_msix_init(void);

/**
 * C handler for MSI-X vector (from isr stub); signals Notification + EOI.
 * Live device IRQs enter here; soft paths do not.
 */
void irq_msix_handler(void);

/**
 * Software inject (smoke / host inject without device fire).
 * OR @u64Badge into the MSI-X Notification; updates soft count / last path.
 */
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

/** Lifetime hard IRQ handler entries (stats). */
u32 irq_msix_count(void);
/** Lifetime soft injects (stats). */
u32 irq_msix_soft_count(void);
/** Lifetime hw-sim pulses (stats). */
u32 irq_msix_hw_count(void);
/** Lifetime soft pulse-path calls (stats). */
u32 irq_msix_soft_pulse_path_count(void);
/** Lifetime soft table pulses (stats). */
u32 irq_msix_table_pulse_count(void);
/** Last pulsed badge word (stats). */
u64 irq_msix_last_badge(void);
/** Last path tag GJ_MSIX_PATH_* (stats). */
u32 irq_msix_last_path(void);
/** Non-zero after successful irq_msix_init. */
int irq_msix_ready(void);
