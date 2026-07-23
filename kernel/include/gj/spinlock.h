/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Ticket-free test-and-set spinlock for SMP (G-SMP). UP-safe.
 * Pure C11 atomics + arch cpu_relax / IRQ mask for irqsave variants.
 *
 * Rules (SECURITY_CORE_DESIGN §4):
 *   - Hold only for short non-blocking critical sections
 *   - Never block / schedule while holding a spinlock
 *   - Use irqsave when the same lock is touched from IRQ (e.g. futex timer)
 */
#pragma once

#include <gj/types.h>

struct gj_spinlock {
    u32 u32Locked; /* mutated only via __atomic_*; not plain loads/stores */
};

#define GJ_SPINLOCK_INIT { 0 }

/** Brief pause / yield for contended spin (portability over raw `pause`). */
static inline void
gj_cpu_relax(void)
{
#if defined(__aarch64__)
    __asm__ volatile ("yield" ::: "memory");
#elif defined(__x86_64__) || defined(__i386__)
    __asm__ volatile ("pause" ::: "memory");
#else
    __asm__ volatile ("" ::: "memory");
#endif
}

/**
 * Disable local IRQs and return prior flags for gj_irq_restore.
 * Nested use is caller-managed (save/restore pairs must nest LIFO).
 */
static inline u64
gj_irq_save(void)
{
    u64 u64Flags;

#if defined(__x86_64__)
    __asm__ volatile ("pushfq; popq %0; cli" : "=r"(u64Flags) :: "memory", "cc");
#elif defined(__i386__)
    __asm__ volatile ("pushfl; popl %0; cli" : "=r"(u64Flags) :: "memory", "cc");
#elif defined(__aarch64__)
    __asm__ volatile ("mrs %0, daif" : "=r"(u64Flags));
    /* I bit: mask IRQs (DAIFSet #2). */
    __asm__ volatile ("msr daifset, #2" ::: "memory");
#else
    u64Flags = 0;
#endif
    return u64Flags;
}

/** Restore IRQ flags from a matching gj_irq_save. */
static inline void
gj_irq_restore(u64 u64Flags)
{
#if defined(__x86_64__)
    __asm__ volatile ("pushq %0; popfq" :: "r"(u64Flags) : "memory", "cc");
#elif defined(__i386__)
    __asm__ volatile ("pushl %0; popfl" :: "r"(u64Flags) : "memory", "cc");
#elif defined(__aarch64__)
    __asm__ volatile ("msr daif, %0" :: "r"(u64Flags) : "memory");
#else
    (void)u64Flags;
#endif
}

static inline void
gj_spin_init(struct gj_spinlock *pLock)
{
    if (pLock != NULL) {
        __atomic_store_n(&pLock->u32Locked, 0u, __ATOMIC_RELAXED);
    }
}

static inline void
gj_spin_lock(struct gj_spinlock *pLock)
{
    u32 u32Expect;

    if (pLock == NULL) {
        return;
    }
    for (;;) {
        u32Expect = 0u;
        if (__atomic_compare_exchange_n(&pLock->u32Locked, &u32Expect, 1u,
                                        0 /* strong */, __ATOMIC_ACQUIRE,
                                        __ATOMIC_RELAXED)) {
            return;
        }
        while (__atomic_load_n(&pLock->u32Locked, __ATOMIC_RELAXED) != 0u) {
            gj_cpu_relax();
        }
    }
}

static inline int
gj_spin_trylock(struct gj_spinlock *pLock)
{
    u32 u32Expect;

    if (pLock == NULL) {
        return 0;
    }
    u32Expect = 0u;
    return __atomic_compare_exchange_n(&pLock->u32Locked, &u32Expect, 1u, 0,
                                       __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)
               ? 1
               : 0;
}

static inline void
gj_spin_unlock(struct gj_spinlock *pLock)
{
    if (pLock != NULL) {
        __atomic_store_n(&pLock->u32Locked, 0u, __ATOMIC_RELEASE);
    }
}

/**
 * Disable local IRQs then acquire lock. Returns flags for
 * gj_spin_unlock_irqrestore. Use when the lock is shared with IRQ paths.
 */
static inline u64
gj_spin_lock_irqsave(struct gj_spinlock *pLock)
{
    u64 u64Flags = gj_irq_save();

    gj_spin_lock(pLock);
    return u64Flags;
}

/** Release lock then restore IRQ flags from gj_spin_lock_irqsave. */
static inline void
gj_spin_unlock_irqrestore(struct gj_spinlock *pLock, u64 u64Flags)
{
    gj_spin_unlock(pLock);
    gj_irq_restore(u64Flags);
}
