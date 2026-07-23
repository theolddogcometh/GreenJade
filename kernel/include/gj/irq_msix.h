/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product MSI-X vector → Notification bind (hard IRQ signals only).
 * Clean-room pure C. No GPL source.
 */
#pragma once

#include <gj/types.h>

/** Product MSI-X delivery vector (IDT). Matches pci_msix_probe_log default. */
#define GJ_MSIX_IRQ_VEC 0x41u

/** Install IDT gate + bind global Notification. Call after idt_init + APIC. */
void irq_msix_init(void);

/** C handler for MSI-X vector (from isr stub); signals Notification + EOI. */
void irq_msix_handler(void);

/** Software inject (smoke / host inject without device fire). */
void irq_msix_soft_inject(u64 u64Badge);

/**
 * Hw-sim pulse: Notification-side of a hardware IRQ without self-IPI/EOI
 * (early kmain self-IPI risks #DF). Returns cumulative hw pulse count.
 * Live device IRQs use the installed IDT gate → irq_msix_handler.
 */
u32 irq_msix_hw_pulse(void);

u32 irq_msix_count(void);
u32 irq_msix_soft_count(void);
u32 irq_msix_hw_count(void);
int irq_msix_ready(void);
