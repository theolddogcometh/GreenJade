/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * x2APIC capability + mode (product path).
 */
#pragma once

#include <gj/types.h>

/** CPUID leaf 1 ECX bit 21 = x2APIC. Non-zero if supported. */
int  x2apic_supported(void);

/** Non-zero after successful x2apic_enable(). */
int  x2apic_enabled(void);

/**
 * Switch local APIC to x2APIC mode (IA32_APIC_BASE bit 10).
 * Requires apic_init first. Returns 0 on success.
 */
int  x2apic_enable(void);

/** Detect and enable when supported. */
void x2apic_probe(void);

/** Fixed IPI via x2APIC ICR MSR (only when x2apic_enabled). */
void x2apic_send_ipi(u32 u32ApicId, u8 u8Vector);
/** Self-IPI (x2APIC MSR 0x83F). */
void x2apic_send_self_ipi(u8 u8Vector);

/** Raw 64-bit ICR write (INIT/SIPI delivery modes). */
void x2apic_send_ipi_raw(u64 u64Icr);

/** EOI via x2APIC MSR. */
void x2apic_eoi(void);
