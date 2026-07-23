/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * x2APIC capability + mode (product path) + ICR soft observability.
 *
 * Soft helpers below are boot/bring-up telemetry (counters + greppable
 * logs) — not hot-path locks. ICR soft tracks MSR 0x830 / self-IPI writes
 * so INIT/SIPI/fixed paths are visible without MMIO decode.
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

/* ------------------------------------------------------------------ */
/* Soft ICR observability (boot telemetry — not hot-path locks)         */
/* ------------------------------------------------------------------ */

/** Delivery-mode field (ICR bits 10:8) — soft decode aids. */
#define GJ_X2APIC_ICR_MODE_FIXED   0u
#define GJ_X2APIC_ICR_MODE_LOWPRI  1u
#define GJ_X2APIC_ICR_MODE_SMI     2u
#define GJ_X2APIC_ICR_MODE_NMI     4u
#define GJ_X2APIC_ICR_MODE_INIT    5u
#define GJ_X2APIC_ICR_MODE_STARTUP 6u
#define GJ_X2APIC_ICR_MODE_EXTINT  7u

/** Total x2APIC ICR MSR (0x830) writes observed since boot. */
u64  x2apic_icr_soft_writes(void);
/** Fixed-delivery ICR writes (mode 0). */
u64  x2apic_icr_soft_fixed(void);
/** INIT delivery ICR writes (mode 5) — AP bring-up. */
u64  x2apic_icr_soft_init(void);
/** STARTUP/SIPI delivery ICR writes (mode 6) — AP bring-up. */
u64  x2apic_icr_soft_sipi(void);
/** Self-IPI MSR (0x83F) writes. */
u64  x2apic_icr_soft_self(void);
/** Other / unclassified delivery modes. */
u64  x2apic_icr_soft_other(void);

/** Last ICR value written (0 if none). */
u64  x2apic_icr_soft_last(void);
/** Destination APIC id from last ICR (bits 63:32). */
u32  x2apic_icr_soft_last_dest(void);
/** Delivery mode (bits 10:8) from last ICR. */
u8   x2apic_icr_soft_last_mode(void);
/** Vector (bits 7:0) from last ICR. */
u8   x2apic_icr_soft_last_vector(void);

/**
 * Greppable ICR soft summary:
 *   x2apic: icr soft writes=… fixed=… init=… sipi=… self=… other=…
 *   x2apic: icr soft last dest=… mode=… vec=… val=0x…
 * Safe anytime; no-op-ish if no writes yet (still prints zeros).
 */
void x2apic_icr_soft_log(void);
