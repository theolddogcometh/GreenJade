/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * PIT tick + monotonic clock for idle HLT and futex timeouts (G-FUT / G-SCHED).
 * After APIC calibrate, mono/jiffies may switch to the local APIC timer.
 */
#pragma once

#include <gj/types.h>

/** Hz of periodic tick (PIT channel 0 / APIC target). */
#define GJ_TIMER_HZ 100u

void timer_init(void);

/** Monotonic nanoseconds since timer_init (jiffies * period). */
u64  timer_mono_nsec(void);

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
/** Times quantum expired (scheduler may force yield). */
u64  timer_preempt_count(void);
/** Call from idle/sched: if quantum expired for current thread, yield. */
void timer_preempt_check(void);
