/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Local APIC timer (product path; PIT used for calibration then fallback).
 * Soft cal observability is boot telemetry — not hot-path locks.
 */
#pragma once

#include <gj/types.h>

/** Soft calibration outcome tags (sticky last status + counters). */
#define GJ_APIC_CAL_NONE       0u /* never attempted */
#define GJ_APIC_CAL_OK         1u /* bus_hz accepted */
#define GJ_APIC_CAL_FAIL_RDY   2u /* APIC or PIT not ready */
#define GJ_APIC_CAL_FAIL_PIT   3u /* PIT not ticking / align timeout */
#define GJ_APIC_CAL_FAIL_CUR   4u /* countdown did not advance */
#define GJ_APIC_CAL_FAIL_HZ    5u /* bus_hz below floor */
#define GJ_APIC_CAL_FAIL_WIN   6u /* measurement window degenerate */

/**
 * Soft snapshot of APIC timer calibration + programmed period.
 * Filled by apic_cal_soft_snapshot(); zeroed fields mean "not yet".
 */
struct gj_apic_cal_soft {
    u32 u32Status;         /* GJ_APIC_CAL_* last outcome */
    u32 u32Attempts;       /* apic_calibrate() calls */
    u32 u32Ok;             /* successful calibrations */
    u32 u32Fail;           /* failed calibrations */
    u32 u32Samples;        /* last successful multi-sample count (1..N) */
    u32 u32PitTicks;       /* last window: PIT jiffies delta */
    u32 u32Elapsed;        /* last window: LAPIC counts elapsed (div16) */
    u32 u32AlignSpins;     /* last align-to-tick spin count */
    u32 u32WaitSpins;      /* last wait-window spin count */
    u64 u64BusHz;          /* accepted bus counts/sec (div16 domain) */
    u64 u64BusHzSample0;   /* first sample (0 if unused) */
    u64 u64BusHzSample1;   /* second sample (0 if single-sample) */
    u64 u64NsecPerTick;    /* programmed period nsec (after timer_hz) */
    u32 u32PeriodInit;     /* LAPIC INIT count for periodic mode */
    u32 u32HzProgrammed;   /* last apic_timer_hz argument */
    u64 u64BspTicks;       /* BSP mono-driving APIC IRQ count */
    u32 u32Calibrated;     /* 1 after successful calibrate */
    u32 u32Ready;          /* apic_ready() */
};

/** Detect and enable local APIC; optional one-shot/periodic timer. */
void apic_init(void);
int  apic_ready(void);

/**
 * Calibrate APIC timer against PIT (must call after timer_init + STI).
 * Multi-sample (up to 2 windows) when PIT is stable; records soft telemetry.
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

/** Current LAPIC timer count (countdown); 0 if not ready. Soft mono uses this. */
u32  apic_timer_cur_count(void);

/** Nanoseconds per APIC tick (after calibrate + timer_hz); 0 if unknown. */
u64  apic_nsec_per_tick(void);

/** Soft calibration snapshot (NULL-safe). */
void apic_cal_soft_snapshot(struct gj_apic_cal_soft *pOut);

/**
 * Greppable soft log:
 *   apic: cal soft ...
 *   apic: cal soft PASS|FAIL|NONE
 */
void apic_cal_soft_log(void);

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
