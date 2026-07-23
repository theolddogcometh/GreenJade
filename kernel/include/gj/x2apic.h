/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * x2APIC capability + mode (product path) + ICR soft observability.
 * Pure C11 freestanding; dual-licensed MIT OR Apache-2.0.
 *
 * -------------------------------------------------------------------------
 * Scope
 * -------------------------------------------------------------------------
 * Prefer x2APIC when CPUID + firmware allow it (P-IRQ-1, P-SMP-3):
 *   - Destination field is a full 32-bit APIC id (large logical CPU counts).
 *   - ICR / EOI / self-IPI via MSRs (no MMIO race with mapped LAPIC page).
 *
 * Product enable order (typical BSP):
 *   apic_init() → x2apic_probe() or x2apic_enable() → IPIs use MSR path.
 * APs may re-enable after their own long-mode entry (see smp AP phase X2APIC).
 *
 * Soft helpers below are boot/bring-up telemetry (counters + greppable
 * logs) — not hot-path locks. ICR soft tracks MSR 0x830 / self-IPI writes
 * so INIT/SIPI/fixed paths are visible without MMIO decode.
 *
 * Soft never hard-gates IPI delivery. Counters wrap OK.
 *
 * Cross-module: gj/apic.h (xAPIC fallback + timer + resched vectors),
 *               gj/smp.h (INIT-SIPI soft summary chains x2apic_icr_soft_log).
 *
 * greppable: x2apic: icr soft
 * greppable: GJ_X2APIC_ICR_MODE_
 * greppable: x2apic_enable x2apic_probe x2apic_send_ipi
 * greppable: MSR 0x830 0x83F P-IRQ-1
 */
#pragma once

#include <gj/types.h>

/**
 * CPUID leaf 1 ECX bit 21 = x2APIC feature.
 * Non-zero if the logical processor advertises x2APIC support.
 * Does not imply the mode is currently enabled (see x2apic_enabled).
 */
int  x2apic_supported(void);

/**
 * Non-zero after successful x2apic_enable().
 * When set, apic_send_* / EOI product paths should prefer MSR writes.
 */
int  x2apic_enabled(void);

/**
 * Switch local APIC to x2APIC mode (IA32_APIC_BASE bit 10).
 * Requires apic_init first (LAPIC base known / enabled).
 * Returns 0 on success; non-zero if unsupported or enable rejected.
 */
int  x2apic_enable(void);

/**
 * Detect (CPUID) and enable when supported.
 * Soft no-op when feature bit clear; safe to call on UP and AP.
 */
void x2apic_probe(void);

/**
 * Fixed-delivery IPI via x2APIC ICR MSR (only when x2apic_enabled).
 * u32ApicId is the full x2APIC destination id; u8Vector is the IDT vector.
 */
void x2apic_send_ipi(u32 u32ApicId, u8 u8Vector);

/**
 * Self-IPI (x2APIC MSR 0x83F).
 * Used for local vector delivery without full ICR encode (e.g. soft tests).
 */
void x2apic_send_self_ipi(u8 u8Vector);

/**
 * Raw 64-bit ICR write (INIT/SIPI and non-fixed delivery modes).
 * Caller encodes delivery mode, level, vector, and destination per SDM.
 * Soft counters decode mode/vector/dest from this value.
 */
void x2apic_send_ipi_raw(u64 u64Icr);

/** EOI via x2APIC MSR (device/IRQ completion when mode is x2APIC). */
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
 * greppable: x2apic: icr soft
 */
void x2apic_icr_soft_log(void);
