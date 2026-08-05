/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft Linux time / delay / IRQ-request seed (clean-room).
 * Dual MIT OR Apache-2.0. No GPL / no Linux source.
 *
 * Purpose
 * -------
 * Soft bodies for jiffies, msleep/udelay/usleep_range, request_irq, and
 * _printk / _dev_err so soft-loaded modules and F2 linux_ksym can resolve
 * something observable (counters + greppable logs + jiffies bumps).
 * Complements stub entries in linux_ksym_soft_inventory: this TU owns real
 * soft implementations.
 *
 * Soft ≠ product: no live PIC/APIC delivery, no real wall-clock sleep that
 * blocks product bring-up forever (spin budgets are capped). Soft ≠
 * ABI-stable handler calling conventions beyond "store the pointer".
 * linux_time_soft_irq_inject is an explicit soft path for freestanding later.
 *
 * Greppable markers (keep stable):
 *   linux_time_soft: soft init PASS
 *   linux_time_soft: soft ksym register PASS|SKIP
 *   linux_time_soft: soft irq bind irq=…
 *   linux_time_soft: soft irq free irq=…
 *   linux_time_soft: soft irq inject name=…
 *   linux_time_soft: soft msleep …
 *   linux_time_soft: soft _printk …
 *   linux_time_soft: soft _dev_err …
 *
 * See docs/LINUX_ABI_HYBRID.md · docs/UDX_LINUX_PORTER.md.
 */
#pragma once

#include <gj/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Soft IRQ table size (line index == irq when irq < max). */
#define LINUX_TIME_SOFT_IRQ_MAX  32u

/**
 * Linux-shaped jiffies data symbol (soft).
 * Updated from timer_jiffies() when the mono clock is live, else bumped by
 * delay helpers / linux_time_soft_poll(). ksym registers &jiffies as "jiffies".
 */
extern volatile unsigned long jiffies;

/**
 * Soft init: idempotent ready lamp + optional linux_ksym_register of bodies
 * (jiffies data, delays, IRQ bind, real memcpy/memset addresses).
 * Grep: linux_time_soft: soft init PASS
 */
void linux_time_soft_init(void);

/** Non-zero after successful soft init. */
int  linux_time_soft_ready(void);

/**
 * Soft poll: sync jiffies from timer_jiffies() when available, else +1.
 * Safe cold-path / soft bring-up helper.
 */
void linux_time_soft_poll(void);

/* ---- Soft delay bodies (Linux C names) ---------------------------------- */

/** Soft msleep: spin on timer_jiffies budget; always bumps soft jiffies. */
void msleep(unsigned int msecs);

/**
 * Soft __const_udelay: capped busy-spin on xloops (Linux loops scale; soft
 * only burns a bounded count so bring-up never hangs).
 */
void __const_udelay(unsigned long xloops);

/** Soft udelay: maps usecs to a small __const_udelay budget. */
void udelay(unsigned long usecs);

/**
 * Soft usleep_range: uses min (max ignored for soft scheduling); short spin
 * + jiffies bump. Soft≠product sleep.
 */
void usleep_range(unsigned long min, unsigned long max);

/**
 * Soft usleep_range_state: same as usleep_range; state ignored (no real
 * TASK_* sleep states). Post-probe r8169 delay path. Soft≠product.
 */
void usleep_range_state(unsigned long min, unsigned long max, unsigned state);

/* ---- Soft IRQ request table (Linux C names) ----------------------------- */

/**
 * Soft request_irq: store handler in table[irq] when irq < 32.
 * Logs: linux_time_soft: soft irq bind irq=…
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
 * Soft IRQ inject by name recorded in request_irq / request_threaded_irq.
 * When the slot is bound, not disabled, and has a handler, calls
 * handler(irq, dev) (and thread_fn if primary returns soft wake-thread=2).
 * Callable from freestanding later. Soft≠product (no APIC delivery).
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

#ifdef __cplusplus
}
#endif
