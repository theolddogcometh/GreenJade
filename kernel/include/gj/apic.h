/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Local APIC timer (product path; PIT used for calibration then fallback).
 */
#pragma once

#include <gj/types.h>

/** Detect and enable local APIC; optional one-shot/periodic timer. */
void apic_init(void);
int  apic_ready(void);

/**
 * Calibrate APIC timer against PIT (must call after timer_init + STI).
 * Measures bus ticks / second with divide-by-16. Returns 0 on success.
 */
int  apic_calibrate(void);

/** Non-zero after successful calibrate. */
int  apic_calibrated(void);

/** Bus ticks per second (after calibrate; 0 if not). */
u64  apic_bus_hz(void);

/**
 * Program periodic timer at u32Hz using calibration (or best-effort guess).
 * On success, APIC becomes the preferred mono/jiffies source via timer_set_apic.
 * Only BSP advances global mono; APs use apic_timer_start_local().
 */
void apic_timer_hz(u32 u32Hz);

/** Start this CPU's local APIC timer (AP path; no mono switch). */
void apic_timer_start_local(void);

u64  apic_timer_ticks(void);
u64  apic_timer_ticks_cpu(u32 u32Cpu);
u32  apic_timer_init_count(void);

/** Nanoseconds per APIC tick (after calibrate + timer_hz); 0 if unknown. */
u64  apic_nsec_per_tick(void);

/**
 * Send fixed IPI to destination local APIC id (xAPIC ICR).
 * u8Vector: interrupt vector; fAssert for INIT/SIPI sequencing use ICR mode helpers.
 */
void apic_send_ipi(u32 u32ApicId, u8 u8Vector);

/** INIT level-deassert + SIPI startup (vector = trampoline_pa >> 12). */
void apic_send_init_sipi(u32 u32ApicId, u8 u8StartupVector);

/** Vector for reschedule/wake IPI (must match IDT gate). */
#define GJ_APIC_IPI_RESCHED_VEC 49u
/** Vector for TLB shootdown IPI (G-AS-4). */
#define GJ_APIC_IPI_TLB_VEC     50u

/** Install IPI resched gate (call once from BSP after idt_init). */
void apic_ipi_init(void);

/** Write LAPIC/x2APIC EOI (device IRQ completion). */
void apic_eoi(void);

/** Deliver fixed IPI to self (for MSI-X path self-test). */
void apic_send_self_ipi(u8 u8Vector);

/** C handler for IPI resched (EOI only). */
void apic_ipi_resched_irq(void);

/** C handler for TLB shootdown IPI. */
void apic_ipi_tlb_irq(void);

/**
 * Broadcast TLB shootdown for one page (or full CR3 reload if va==0).
 * Other online CPUs invlpg / reload; local CPU already invalidated.
 */
void tlb_shootdown(gj_vaddr_t va);

/** Send reschedule IPI to a CPU slot (wake HLT / nudge schedule). */
void apic_send_resched(u32 u32CpuSlot);
