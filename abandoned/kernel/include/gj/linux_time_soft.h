/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft Linux time / delay / IRQ-request seed (clean-room).
 * Dual MIT OR Apache-2.0. No GPL / no Linux source.
 *
 * Purpose
 * -------
 * Soft bodies for jiffies / jiffies_64, ktime_get / ktime_get_ns /
 * ktime_get_mono_fast_ns, msleep/mdelay/udelay, usleep_range,
 * schedule_timeout*, msecs/usecs/nsecs_to_jiffies + inverses, soft
 * timer_list residual (mod_timer / del_timer* / timer_setup), soft
 * completion residual (complete_star/wait_star), request_irq /
 * request_threaded_irq / free_irq / enable_irq / disable_irq*,
 * _printk / __warn_printk / _dev_err so soft-loaded modules (r8169.ko path,
 * usb-storage leaf timeouts) and F2 linux_ksym can resolve something
 * observable (counters + greppable logs + jiffies bumps). Complements stub
 * entries in linux_ksym inventory: this TU owns real soft implementations
 * for the time/IRQ/printk/completion class. Soft!=product. G-AC-1.
 *
 * Soft != product: no live PIC/APIC delivery, no real wall-clock sleep that
 * blocks product bring-up forever (spin budgets are capped). Soft !=
 * ABI-stable handler calling conventions beyond "store the pointer".
 * Soft != product timer ABI (no hrtimer, no CLOCK_MONOTONIC fidelity claim).
 * Soft timer_list residual never fires callbacks (no IRQ0 / APIC hook).
 * Soft completion residual never blocks forever (fail-closed cap).
 * linux_time_soft_irq_inject is an explicit soft path for freestanding later.
 * Fail-closed product=0 - greppable soft lamps never claim product DoD.
 *
 * Non-destructive to freestanding wire (hybrid 4a): soft IRQ table never
 * programs APIC/MSI, never steals 10ec:8168 freestanding vectors. Delays and
 * schedule_timeout are spin-capped (dual DoD lab safe). Soft!=product.
 * Grep: linux_time_soft: soft hybrid zero-touch PASS
 *
 * Lean residual (this pass; Soft!=product; G-AC-1; C0; no version stamp;
 * no storm):
 *   - hot helpers stay silent (no kprintf; no stamp storms; no version stamp)
 *   - jiffies + jiffies_64 + get_jiffies_64 data residual
 *   - ktime_get_mono_fast_ns rebind to soft mono (hot; ksym re-register)
 *   - soft timer_list: mod_timer / mod_timer_pending / add_timer* /
 *     del_timer* / timer_setup / timer_pending (never fires)
 *   - soft completion void* table: init/reinit/complete_star/wait_star/try_wait
 *   - ns<->jiffy + ktime ms/us + round_jiffies* pure math (hot)
 *   - mdelay / ndelay + disable_irq_nosync / synchronize_irq companions
 *   - soft irq-request table + inject (no APIC/MSI; hybrid zero-touch)
 *   - once residual lean PASS lamp at init only
 * Grep: linux_time_soft: soft residual lean PASS
 *
 * Greppable markers (keep stable; prefix linux_time_soft: soft ...):
 *   linux_time_soft: soft init PASS
 *   linux_time_soft: soft ksym register PASS|SKIP
 *   linux_time_soft: soft hybrid zero-touch PASS
 *   linux_time_soft: soft residual lean PASS
 *   linux_time_soft: soft irq bind irq=...
 *   linux_time_soft: soft irq free irq=...
 *   linux_time_soft: soft irq inject name=...
 *   linux_time_soft: soft timer mod|del|setup ...
 *   linux_time_soft: soft completion ...
 *   linux_time_soft: soft msleep ...
 *   linux_time_soft: soft schedule_timeout ...
 *   linux_time_soft: soft msecs_to_jiffies ...
 *   linux_time_soft: soft _printk ...
 *   linux_time_soft: soft __warn_printk ...
 *   linux_time_soft: soft _dev_err ...
 *   linux_time_soft: soft ktime_get residual ...
 *
 * Hot helpers (cheap; no kprintf; no IRQ-path work):
 *   ktime_get / ktime_get_ns / ktime_to_ms / ktime_to_us / ms_to_ktime,
 *   msecs_to_jiffies / __msecs_to_jiffies, jiffies_to_msecs,
 *   usecs_to_jiffies, jiffies_to_usecs, nsecs_to_jiffies, jiffies_to_nsecs,
 *   round_jiffies / round_jiffies_relative / round_jiffies_up,
 *   lts_jiffies_sync / poll / timer_pending / try_wait_for_completion /
 *   completion_done.
 *   ktime_get prefers timer_mono_nsec_soft when timer_ready (soft residual).
 *
 * Residual (honesty; Soft!=product):
 *   - no real schedule / preemption / TASK_* sleep states
 *   - soft timer_list bookkeeping only - never product fire path
 *   - soft completion bookkeeping only - fail-closed wait cap
 *   - no hrtimer / delayed_work product path
 *   - schedule_timeout* fail-closed (cap + jiffies bump + return 0)
 *   - dual DoD A/B OPEN; soft timeouts != stick / != :22 host banner
 *   - advanced eng surface: jiffy convert + schedule_timeout* + soft mono ktime
 *
 * See docs/LINUX_ABI_HYBRID.md | docs/UDX_LINUX_PORTER.md.
 */
#pragma once

#include <gj/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Soft IRQ table size (line index == irq when irq < max). */
#define LINUX_TIME_SOFT_IRQ_MAX  32u

/**
 * Soft timer_list residual slots (void* key + expires + pending + pfn).
 * Soft!=product: never fires; no APIC/IRQ0 delivery.
 */
#define LINUX_TIME_SOFT_TIMER_MAX  16u

/**
 * Soft completion residual slots (void* key + done count).
 * Soft!=product: no real wait queue; fail-closed spin cap.
 */
#define LINUX_TIME_SOFT_COMPLETION_MAX  16u

/**
 * Soft cap for schedule_timeout / msleep / wait_for_completion jiffy budgets.
 * Soft!=product; never hang freestanding bring-up.
 */
#define LINUX_TIME_SOFT_TIMEOUT_JIFFIES_MAX  200ul

/**
 * Soft cap for mdelay busy-budget ms (bring-up safe). Soft!=product.
 */
#define LINUX_TIME_SOFT_MDELAY_MS_MAX  50ul

/**
 * Soft cap for ndelay nsec spin budget (bring-up safe). Soft!=product.
 */
#define LINUX_TIME_SOFT_NDELAY_NS_MAX  100000ul

/**
 * Linux-shaped jiffies data symbol (soft).
 * Updated from timer_jiffies() when the mono clock is live, else bumped by
 * delay helpers / linux_time_soft_poll(). ksym registers &jiffies as "jiffies".
 * Soft!=product timer ABI.
 */
extern volatile unsigned long jiffies;

/**
 * Soft jiffies_64 data residual (Linux-shaped 64-bit tick). Mirrored with
 * jiffies on sync/bump. Soft!=product (no seqlock / timekeeping core).
 */
extern volatile u64 jiffies_64;

/**
 * Soft get_jiffies_64: snapshot of soft jiffies_64 (hot; no kprintf).
 * Soft!=product.
 */
u64 get_jiffies_64(void);

/**
 * Soft init: idempotent ready lamp + optional linux_ksym_register of bodies
 * (jiffies data, delays, schedule_timeout*, jiffy convert, soft timer_list,
 * soft completion complete_star/wait_star, IRQ bind, real memcpy/memset
 * addresses). Fail-closed product=0. Soft!=product. G-AC-1.
 * Grep: linux_time_soft: soft init PASS
 * Grep: linux_time_soft: soft residual lean PASS
 */
void linux_time_soft_init(void);

/** Non-zero after successful soft init. */
int  linux_time_soft_ready(void);

/**
 * Soft poll: sync jiffies from timer_jiffies() when available, else +1.
 * Safe cold-path / soft bring-up helper.
 */
void linux_time_soft_poll(void);

/* ---- Soft ktime (Linux C names; r8169 / net und) ------------------------ */

/**
 * Soft ktime_get: return freestanding mono nsec as i64 (Linux ktime_t shape).
 * Uses timer_mono_nsec_soft when timer is live; syncs soft jiffies.
 * Hot helper: no kprintf. Never programs hardware clocks; never touches wire.
 * Soft!=product PHC / CLOCK_MONOTONIC fidelity.
 * Grep (cold init): linux_time_soft: soft ktime_get residual
 */
i64 ktime_get(void);

/**
 * Soft ktime_get_ns: mono nsec as u64 (hot; no kprintf). Soft!=product.
 * ksym also rebinds "ktime_get_mono_fast_ns" to this body (hot residual;
 * Soft!=product; sibling leaf C names may still return 0).
 */
u64 ktime_get_ns(void);

/**
 * Soft ktime_to_ms / ktime_to_us / ms_to_ktime: pure unit math (hot).
 * Soft!=product (no overflow table fidelity).
 */
i64  ktime_to_ms(i64 i64Kt);
i64  ktime_to_us(i64 i64Kt);
i64  ms_to_ktime(i64 i64Ms);

/* ---- Soft jiffy conversion (Linux C names; HZ = GJ_TIMER_HZ) ------------ */

/**
 * Soft msecs_to_jiffies: convert ms -> jiffies at GJ_TIMER_HZ.
 * Hot helper: pure math, no kprintf. Soft!=product (no MAX_JIFFY_OFFSET).
 * Grep (cold init): linux_time_soft: soft msecs_to_jiffies
 * __msecs_to_jiffies is weak here if a leaf TU also defines it.
 */
unsigned long msecs_to_jiffies(const unsigned int m);
unsigned long __msecs_to_jiffies(const unsigned int m);

/** Soft jiffies_to_msecs: inverse of soft msecs_to_jiffies (capped math). */
unsigned int jiffies_to_msecs(const unsigned long j);

/** Soft usecs_to_jiffies / jiffies_to_usecs (soft unit scale). */
unsigned long usecs_to_jiffies(const unsigned int u);
unsigned int jiffies_to_usecs(const unsigned long j);

/**
 * Soft nsecs_to_jiffies / jiffies_to_nsecs (hot pure math). Soft!=product.
 */
unsigned long nsecs_to_jiffies(u64 u64Nsec);
u64 jiffies_to_nsecs(const unsigned long j);

/**
 * Soft round_jiffies* : round absolute/relative jiffy to next HZ second.
 * Hot pure math; no kprintf. Soft!=product (not Linux exact round table).
 */
unsigned long round_jiffies(unsigned long uJ);
unsigned long round_jiffies_relative(unsigned long uJ);
unsigned long round_jiffies_up(unsigned long uJ);
unsigned long round_jiffies_up_relative(unsigned long uJ);

/* ---- Soft delay bodies (Linux C names) ---------------------------------- */

/** Soft msleep: spin on timer_jiffies budget; always bumps soft jiffies. */
void msleep(unsigned int msecs);

/**
 * Soft msleep_interruptible: same budget as msleep; soft never signal-wakes.
 * Returns 0 (full sleep; Soft!=product interruptible).
 */
unsigned long msleep_interruptible(unsigned int msecs);

/**
 * Soft ssleep: seconds -> capped msleep chain. Soft!=product.
 */
void ssleep(unsigned int seconds);

/**
 * Soft mdelay: capped busy-style ms delay (maps to soft msleep budget).
 * Replaces empty ksym mdelay stub. Soft!=product (not calibrated LPJ).
 */
void mdelay(unsigned long msecs);

/**
 * Soft __const_udelay: capped busy-spin on xloops (Linux loops scale; soft
 * only burns a bounded count so bring-up never hangs).
 */
void __const_udelay(unsigned long xloops);

/** Soft udelay: maps usecs to a small __const_udelay budget. */
void udelay(unsigned long usecs);

/**
 * Soft ndelay: capped busy spin for nsec budget (maps to tiny udelay-ish).
 * Soft!=product (not calibrated LPJ). Cap LINUX_TIME_SOFT_NDELAY_NS_MAX.
 */
void ndelay(unsigned long nsecs);

/**
 * Soft usleep_range: uses min (max ignored for soft scheduling); short spin
 * + jiffies bump. Soft!=product sleep.
 */
void usleep_range(unsigned long min, unsigned long max);

/**
 * Soft usleep_range_state: same as usleep_range; state ignored (no real
 * TASK_* sleep states). Post-probe r8169 delay path. Soft!=product.
 */
void usleep_range_state(unsigned long min, unsigned long max, unsigned state);

/* ---- Soft schedule_timeout (Linux C names; USB/net module waits) -------- */

/**
 * Soft schedule_timeout: fail-closed soft wait of up to `timeout` jiffies.
 * Caps at LINUX_TIME_SOFT_TIMEOUT_JIFFIES_MAX; spins on mono when live else
 * bumps jiffies. Always returns 0 (full timeout elapsed) so soft waiters
 * do not hang dual DoD lab. Never real schedule / TASK_*.
 * Soft!=product timer ABI. Grep: linux_time_soft: soft schedule_timeout
 *
 * Linux shape: long schedule_timeout(long timeout);
 * Special: timeout <= 0 -> return 0; huge/MAX_SCHEDULE_TIMEOUT -> hard cap.
 */
long schedule_timeout(long timeout);

/**
 * Soft schedule_timeout_interruptible / uninterruptible / killable / idle:
 * same fail-closed body as schedule_timeout (no signal / freezer).
 * Grep: linux_time_soft: soft schedule_timeout
 */
long schedule_timeout_interruptible(long timeout);
long schedule_timeout_uninterruptible(long timeout);
long schedule_timeout_killable(long timeout);
long schedule_timeout_idle(long timeout);

/**
 * Soft io_schedule_timeout: same soft budget as schedule_timeout
 * (no real block-layer io_schedule). Soft!=product.
 */
long io_schedule_timeout(long timeout);

/* ---- Soft timer_list residual (Linux C names; void* key) ---------------- */

/**
 * Soft timer_setup: record pTimer key + optional callback pointer + flags.
 * Never arms; never fires. Soft!=product.
 * Grep: linux_time_soft: soft timer setup
 * Linux shape: void timer_setup(struct timer_list *, void (*)(struct timer_list *), unsigned);
 */
void timer_setup(void *pTimer, void *pfnCallback, unsigned int uFlags);

/**
 * Soft mod_timer: record expires for pTimer key; mark pending.
 * Returns 1 if timer was already pending, 0 if newly armed / first seen.
 * Soft!=product: never programs IRQ0/APIC; never fires callback.
 * Grep: linux_time_soft: soft timer mod
 * Linux shape: int mod_timer(struct timer_list *timer, unsigned long expires);
 */
int  mod_timer(void *pTimer, unsigned long uExpires);

/**
 * Soft mod_timer_pending: arm only when already pending; else return 0.
 * Soft!=product. Hot-ish; rate-cap lamp only on cold full path via mod_timer.
 */
int  mod_timer_pending(void *pTimer, unsigned long uExpires);

/**
 * Soft add_timer: arm if not pending (uses stored expires or jiffies+1).
 * Returns void (Linux void). Soft!=product.
 */
void add_timer(void *pTimer);

/**
 * Soft add_timer_on: ignore CPU id; same as add_timer. Soft!=product.
 */
void add_timer_on(void *pTimer, int nCpu);

/**
 * Soft del_timer: clear pending; return 1 if was pending, else 0.
 * Soft!=product.
 */
int  del_timer(void *pTimer);

/**
 * Soft del_timer_sync: same as del_timer (no real IRQ race). Soft!=product.
 * Grep: linux_time_soft: soft timer del
 */
int  del_timer_sync(void *pTimer);

/**
 * Soft del_singleshot_timer_sync: alias of del_timer_sync. Soft!=product.
 */
int  del_singleshot_timer_sync(void *pTimer);

/**
 * Soft timer_pending: 1 if soft table has pTimer pending. Hot helper.
 * Soft!=product.
 */
int  timer_pending(const void *pTimer);

/* ---- Soft completion residual (Linux C names; void* key) ---------------- */

/**
 * Soft init_completion: bind pComp key; done=0. Soft!=product.
 * Grep: linux_time_soft: soft completion
 */
void init_completion(void *pComp);

/** Soft reinit_completion: done=0 when key known. Soft!=product. */
void reinit_completion(void *pComp);

/**
 * Soft complete: increment done for pComp. Soft!=product (no waiter wake).
 * Grep: linux_time_soft: soft completion
 */
void complete(void *pComp);

/** Soft complete_all: set done to a large soft count. Soft!=product. */
void complete_all(void *pComp);

/**
 * Soft wait_for_completion: fail-closed spin/cap; auto-done if still empty.
 * Never hangs bring-up. Soft!=product.
 */
void wait_for_completion(void *pComp);

/**
 * Soft wait_for_completion_timeout: cap at min(timeout, TIMEOUT_JIFFIES_MAX).
 * Returns remaining soft jiffies (0 if full wait / fail-closed auto-done).
 * Soft!=product. Grep: linux_time_soft: soft completion
 */
unsigned long wait_for_completion_timeout(void *pComp, unsigned long uTimeout);

/**
 * Soft wait_for_completion_interruptible / _timeout / killable:
 * same fail-closed body (no signal). Soft!=product.
 */
int           wait_for_completion_interruptible(void *pComp);
long          wait_for_completion_interruptible_timeout(void *pComp,
                                                        unsigned long uTimeout);
int           wait_for_completion_killable(void *pComp);
long          wait_for_completion_killable_timeout(void *pComp,
                                                   unsigned long uTimeout);

/**
 * Soft try_wait_for_completion: 1 and consume one done if available, else 0.
 * Hot helper - no kprintf. Soft!=product.
 */
int  try_wait_for_completion(void *pComp);

/**
 * Soft completion_done: 1 if done > 0. Hot helper. Soft!=product.
 */
int  completion_done(const void *pComp);

/* ---- Soft IRQ request table (Linux C names) ----------------------------- */

/**
 * Soft request_irq: store handler in table[irq] when irq < 32.
 * Logs: linux_time_soft: soft irq bind irq=...
 * Returns 0 on bind, negative on bad args / busy / full.
 */
int  request_irq(unsigned int irq, void *handler, unsigned long flags,
                 const char *name, void *dev);

/**
 * Soft request_threaded_irq: store handler and/or thread_fn (prefer primary
 * handler; if handler is NULL, soft-bind thread_fn as the slot handler).
 */
int  request_threaded_irq(unsigned int irq, void *handler, void *thread_fn,
                          unsigned long flags, const char *name, void *dev);

/** Soft free_irq: clear slot when irq+dev match (dev NULL matches any). */
void free_irq(unsigned int irq, void *dev);

/** Soft enable_irq / disable_irq: disable-depth bookkeeping only. */
void enable_irq(unsigned int irq);
void disable_irq(unsigned int irq);

/**
 * Soft disable_irq_nosync: same disable-depth as disable_irq (no wait).
 * Soft!=product (no real IRQ race).
 */
void disable_irq_nosync(unsigned int irq);

/**
 * Soft synchronize_irq: no-op bookkeeping (no real in-flight wait).
 * Soft!=product.
 */
void synchronize_irq(unsigned int irq);

/**
 * Soft IRQ inject by name recorded in request_irq / request_threaded_irq.
 * When the slot is bound, not disabled, and has a handler, calls
 * handler(irq, dev) (and thread_fn if primary returns soft wake-thread=2).
 * Callable from freestanding later. Soft!=product (no APIC delivery).
 * Grep: linux_time_soft: soft irq inject name=
 * Returns 0 on inject, negative on unbound / disabled / no handler.
 */
int  linux_time_soft_irq_inject(const char *szName);

/** Soft inject success counter. */
u32  linux_time_soft_irq_inject_count(void);

/* ---- Soft printk bodies (Linux C names; re-register over ksym stubs) ---- */

/**
 * Soft _printk: rate-capped kprintf of the format string (args ignored).
 * Linux: int _printk(const char *fmt, ...). Soft returns 0.
 */
int  _printk(const char *szFmt, ...);

/**
 * Soft __warn_printk: r8169 und WARN_* path. Rate-capped; never raises;
 * never touches freestanding wire. Soft returns 0.
 * Grep: linux_time_soft: soft __warn_printk
 */
int  __warn_printk(const char *szFmt, ...);

/**
 * Soft _dev_err: rate-capped kprintf with dev pointer + format string.
 * Linux: void/int _dev_err(const struct device *dev, const char *fmt, ...).
 * Soft returns 0. Sibling _dev_info / _dev_warn share the same soft path.
 */
int  _dev_err(const void *pDev, const char *szFmt, ...);
int  _dev_info(const void *pDev, const char *szFmt, ...);
int  _dev_warn(const void *pDev, const char *szFmt, ...);

/** Soft diagnostics. */
u32  linux_time_soft_irq_bound_count(void);
u32  linux_time_soft_msleep_calls(void);
u32  linux_time_soft_schedule_timeout_calls(void);
u32  linux_time_soft_mod_timer_calls(void);
u32  linux_time_soft_timer_pending_count(void);
u32  linux_time_soft_completion_complete_calls(void);
u32  linux_time_soft_completion_wait_calls(void);

#ifdef __cplusplus
}
#endif
