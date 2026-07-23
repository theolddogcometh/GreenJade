/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * PIT tick + monotonic clock for idle HLT and futex timeouts (G-FUT / G-SCHED).
 * After APIC calibrate, mono/jiffies may switch to the local APIC timer.
 *
 * Soft mono + quantum preempt counters are boot/telemetry surfaces — not
 * hot-path locks. Coarse mono (timer_mono_nsec) stays jiffy-stable for
 * deadlines; soft mono may interpolate within the current APIC period.
 */
#pragma once

#include <gj/types.h>

/** Hz of periodic tick (PIT channel 0 / APIC target). */
#define GJ_TIMER_HZ 100u

/** Soft mono source tags (timer_mono_soft_snapshot). */
#define GJ_TIMER_SRC_NONE  0u
#define GJ_TIMER_SRC_PIT   1u
#define GJ_TIMER_SRC_APIC  2u

/**
 * Soft snapshot of mono clock state (telemetry; NULL-safe fill).
 * u64MonoNsec is the coarse product clock (jiffies * nsec/tick).
 * u64MonoSoftNsec may be finer when APIC is source (clamped >= coarse).
 */
struct gj_timer_mono_soft {
    u64 u64Jiffies;
    u64 u64MonoNsec;
    u64 u64MonoSoftNsec;
    u64 u64NsecPerTick;
    u32 u32Source;       /* GJ_TIMER_SRC_* */
    u32 u32Ready;
    u64 u64PitTicks;     /* mono advances while source=PIT */
    u64 u64ApicTicks;    /* mono advances while source=APIC */
    u64 u64SourceSwitch; /* PIT→APIC handoffs observed */
};

/**
 * Soft snapshot of quantum / soft-preempt counters.
 * Preempt path: quantum_tick → yield request; preempt_check may consume.
 */
struct gj_timer_preempt_soft {
    u32 u32Quantum;          /* configured slice length (ticks) */
    u32 u32SliceLeft;        /* remaining ticks in current slice */
    u64 u64Preempts;         /* quantum expired (yield requested) */
    u64 u64YieldRequests;    /* thread_yield_request calls from timer */
    u64 u64PreemptChecks;    /* timer_preempt_check invocations */
    u64 u64PreemptCheckHits; /* checks that consumed a pending yield */
    u64 u64PreemptCheckYields; /* checks that actually called yield */
    u64 u64QuantumSets;      /* timer_set_quantum_ticks calls */
    u64 u64QuantumTicks;     /* quantum_tick invocations (all mono ticks) */
};

void timer_init(void);

/** Monotonic nanoseconds since timer_init (jiffies * period). Coarse/stable. */
u64  timer_mono_nsec(void);

/**
 * Soft finer mono (APIC current-count interpolate when source=APIC).
 * Never reports less than timer_mono_nsec(); falls back to coarse otherwise.
 */
u64  timer_mono_nsec_soft(void);

/** Nanoseconds accounted per tick (PIT or APIC period). */
u64  timer_nsec_per_tick(void);

/** Raw tick counter. */
u64  timer_jiffies(void);

/** Called from IRQ0 (PIT) stub path. */
void timer_tick(void);

/**
 * Called from APIC timer IRQ when APIC is the mono source.
 * Same side effects as timer_tick without PIC EOI.
 */
void timer_tick_apic(void);

/**
 * After APIC calibration: drive jiffies/mono from APIC, mask PIT IRQ0.
 * u64NsecPerTick: nanoseconds per APIC period (e.g. 10_000_000 for 100 Hz).
 */
void timer_set_apic_source(u64 u64NsecPerTick);

/** 1 if mono comes from APIC, 0 if PIT. */
int  timer_apic_source(void);

/** Busy/HLT sleep until deadline (mono nsec) or forever if 0 and idle. */
void timer_sleep_until(u64 u64DeadlineMonoNsec);

/** Non-zero if timer IRQs are live. */
int  timer_ready(void);

/** Preemption quantum (ticks); default GJ_TIMER_HZ/20 (~50ms @ 100Hz). */
void timer_set_quantum_ticks(u32 u32Ticks);
u32  timer_quantum_ticks(void);
/** Remaining ticks in the current quantum slice. */
u32  timer_slice_left(void);
/** Times quantum expired (scheduler may force yield). */
u64  timer_preempt_count(void);
/**
 * Call from idle/sched: if soft yield is pending (quantum expired), yield.
 * Deepens soft-preempt observability (check / hit / yield counters).
 */
void timer_preempt_check(void);

/** Soft mono clock snapshot (NULL-safe). */
void timer_mono_soft_snapshot(struct gj_timer_mono_soft *pOut);
/** Soft quantum/preempt counter snapshot (NULL-safe). */
void timer_preempt_soft_snapshot(struct gj_timer_preempt_soft *pOut);
/**
 * Greppable soft log:
 *   timer: mono soft ...
 *   timer: preempt soft ...
 */
void timer_soft_log(void);
