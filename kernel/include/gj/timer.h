/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * PIT tick + monotonic clock for idle HLT and futex timeouts (G-FUT / G-SCHED).
 *
 * Scope
 * -----
 * Arch-owned early mono clock and soft quantum / preempt telemetry. Product
 * deadlines (futex_wait, door timeouts, nanosleep-class sleeps) must use
 * timer_mono_nsec — the coarse, jiffy-stable product clock. Soft mono and
 * quantum counters are boot/telemetry surfaces, not hot-path locks.
 *
 * Tick sources (x86_64 product path)
 * ----------------------------------
 *   Early boot:  PIT channel 0 @ GJ_TIMER_HZ, remapped PIC IRQ0 → vector 32.
 *                timer_tick() advances jiffies when not yet APIC-sourced and
 *                always EOIs the PIC.
 *   After cal:   Local APIC periodic timer (typically vector 48). Call
 *                timer_set_apic_source(nsec_per_period) once; mono/jiffies
 *                then advance only from timer_tick_apic(); PIT IRQ0 is masked.
 *   aarch64:     Arch timer.o supplies the same public symbols; Hz/tags match.
 *
 * Coarse vs soft mono
 * -------------------
 *   timer_mono_nsec()      = jiffies * nsec_per_tick  (stable for deadlines)
 *   timer_mono_nsec_soft() = coarse + LAPIC CUR-count interpolate when source
 *                            is APIC; never reports less than coarse (wrap /
 *                            IRQ race clamp). Falls back to coarse on PIT or
 *                            when INIT count is unusable.
 *
 * Soft preempt path (G-SCHED observability)
 * -----------------------------------------
 *   quantum_tick (every mono tick) → may call thread_yield_request()
 *   timer_preempt_check (idle/sched) → may consume pending yield → thread_yield
 *   Counters in gj_timer_preempt_soft deepen check / hit / yield / set stats.
 *
 * Coupling
 * --------
 *   timer_tick / timer_tick_apic always call futex_timer_check() so absolute
 *   mono deadlines on futex waiters reaped with -ETIMEDOUT (G-FUT-2).
 *   timer_sleep_until uses STI+HLT / yield with a spin guard so a stuck mono
 *   clock cannot hang forever (e.g. after user #PF paths).
 *
 * Soft product surface (this header / timer.c)
 * --------------------------------------------
 *   G-TIMER-MONO       — jiffies + coarse mono nsec
 *   G-TIMER-SOFT-MONO  — APIC-interpolated soft mono + snapshot
 *   G-TIMER-APIC-SRC   — PIT → APIC source handoff
 *   G-TIMER-PREEMPT    — quantum slice + soft preempt_check counters
 *
 * greppable: timer: mono soft
 * greppable: timer: preempt soft
 * greppable: G-TIMER-MONO G-TIMER-SOFT-MONO G-TIMER-APIC-SRC G-TIMER-PREEMPT
 * greppable: G-FUT G-SCHED
 */
#pragma once

#include <gj/types.h>

/** Periodic tick rate (PIT channel 0 / APIC target after calibrate). */
#define GJ_TIMER_HZ 100u

/**
 * Soft mono source tags for timer_mono_soft_snapshot.u32Source.
 * NONE = not ready; PIT = early IRQ0 path; APIC = post-handoff.
 * greppable: GJ_TIMER_SRC_
 */
#define GJ_TIMER_SRC_NONE  0u
#define GJ_TIMER_SRC_PIT   1u
#define GJ_TIMER_SRC_APIC  2u

/**
 * Soft snapshot of mono clock state (telemetry; NULL-safe fill).
 *
 * Fields
 * ------
 *   u64Jiffies       Raw tick counter (same as timer_jiffies).
 *   u64MonoNsec      Coarse product clock: jiffies * nsec_per_tick.
 *   u64MonoSoftNsec  Finer soft mono (APIC interpolate); always ≥ coarse.
 *   u64NsecPerTick   Accounted period (PIT divisor or APIC programmed period).
 *   u32Source        GJ_TIMER_SRC_* (which hardware advances mono).
 *   u32Ready         1 after timer_init and IRQs live.
 *   u64PitTicks      Mono advances observed while source=PIT.
 *   u64ApicTicks     Mono advances observed while source=APIC.
 *   u64SourceSwitch  PIT→APIC handoffs observed (timer_set_apic_source).
 *
 * Not a lock-free consistency snapshot under IRQ; for boot logs / soft PASS.
 * greppable: G-TIMER-SOFT-MONO
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
 *
 * Preempt path (one slice):
 *   quantum_tick on each mono tick decrements slice; at 0 → preempts++,
 *   yield_requests++, thread_yield_request(), re-arm slice.
 *   timer_preempt_check may then consume the pending yield (checks / hits /
 *   check_yields) from idle or sched helpers.
 *
 * greppable: G-TIMER-PREEMPT
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

/**
 * Program PIT (or arch equivalent), install IRQ stub, mark ready, STI.
 * Idempotent enough for bring-up; resets soft counters on x86 product path.
 */
void timer_init(void);

/**
 * Monotonic nanoseconds since timer_init (jiffies * period).
 * Coarse/stable product clock — use for futex and sleep deadlines.
 * greppable: G-TIMER-MONO
 */
u64  timer_mono_nsec(void);

/**
 * Soft finer mono (APIC current-count interpolate when source=APIC).
 * Never reports less than timer_mono_nsec(); falls back to coarse otherwise.
 * Not for product deadlines (interpolation can race the tick).
 * greppable: G-TIMER-SOFT-MONO
 */
u64  timer_mono_nsec_soft(void);

/** Nanoseconds accounted per tick (PIT divisor period or APIC period). */
u64  timer_nsec_per_tick(void);

/** Raw tick counter advanced by the active mono source. */
u64  timer_jiffies(void);

/**
 * Called from IRQ0 (PIT) stub path.
 * Advances mono only while source is still PIT; always PIC EOI.
 * Invokes futex_timer_check + quantum_tick (+ deferred revoke hygiene).
 */
void timer_tick(void);

/**
 * Called from APIC timer IRQ when APIC is the mono source.
 * Same mono / futex / quantum side effects as timer_tick without PIC EOI.
 * greppable: G-TIMER-APIC-SRC
 */
void timer_tick_apic(void);

/**
 * After APIC calibration: drive jiffies/mono from APIC, mask PIT IRQ0.
 * u64NsecPerTick: nanoseconds per APIC period (e.g. 10_000_000 for 100 Hz).
 * No-op if u64NsecPerTick == 0. Increments source-switch soft counter once.
 * greppable: G-TIMER-APIC-SRC
 */
void timer_set_apic_source(u64 u64NsecPerTick);

/** 1 if mono comes from APIC, 0 if PIT (or not yet handed off). */
int  timer_apic_source(void);

/**
 * Busy/HLT sleep until deadline (absolute mono nsec) or forever if 0 and idle.
 * When deadline non-zero: STI + yield/HLT loop with a finite guard so a stuck
 * mono clock cannot hang forever; soft-preempt check deepens quantum consume
 * on sleep paths. No-op if timer not ready.
 */
void timer_sleep_until(u64 u64DeadlineMonoNsec);

/** Non-zero if timer IRQs are live (timer_init completed). */
int  timer_ready(void);

/**
 * Preemption quantum length in ticks; default ~GJ_TIMER_HZ/20 (~50 ms @ 100 Hz).
 * Zero is clamped to 1. Resets the current slice to the new length.
 */
void timer_set_quantum_ticks(u32 u32Ticks);

/** Configured quantum length in ticks. */
u32  timer_quantum_ticks(void);

/** Remaining ticks in the current quantum slice. */
u32  timer_slice_left(void);

/** Times quantum expired (scheduler may force yield). */
u64  timer_preempt_count(void);

/**
 * Call from idle/sched: if soft yield is pending (quantum expired), yield.
 * Deepens soft-preempt observability (check / hit / yield counters).
 * May also re-arm a yield request if slice is already zero without a flag.
 * greppable: G-TIMER-PREEMPT
 */
void timer_preempt_check(void);

/** Soft mono clock snapshot (NULL-safe; telemetry only). */
void timer_mono_soft_snapshot(struct gj_timer_mono_soft *pOut);

/** Soft quantum/preempt counter snapshot (NULL-safe; telemetry only). */
void timer_preempt_soft_snapshot(struct gj_timer_preempt_soft *pOut);

/**
 * Greppable soft log (kprintf):
 *   timer: mono soft ...
 *   timer: preempt soft ...
 *   timer: mono soft PASS|READY|FAIL
 *   timer: preempt soft PASS|FAIL
 */
void timer_soft_log(void);
