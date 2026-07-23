/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Ticket-free test-and-set spinlock for SMP (G-SMP). UP-safe.
 * Pure C11 atomics + arch cpu_relax / IRQ mask for irqsave variants.
 * Header-only deepen (no spinlock.c) — soft probes + aligned pad + irqsave try.
 *
 * Rules (SECURITY_CORE_DESIGN §4):
 *   - Hold only for short non-blocking critical sections
 *   - Never block / schedule while holding a spinlock
 *   - Use irqsave when the same lock is touched from IRQ (e.g. futex timer)
 *   - Lock order published by subsystem; no reverse acquire under debug
 *
 * Soft product surface (header-only):
 *   SPIN_TAS              — ticket-free test-and-set + read-spin
 *   SPIN_IRQSAVE          — local IRQ mask around acquire/release
 *   SPIN_SOFT_IS_LOCKED   — relaxed probe (debug / soft asserts)
 *   SPIN_TRY_IRQSAVE      — non-blocking irqsave acquire
 *   SPIN_ALIGNED          — cacheline-padded lock (false-sharing pad)
 *   SPIN_SOFT_IRQ_PROBE   — soft “IRQs disabled?” probe
 *
 * greppable: SPIN_TAS SPIN_IRQSAVE SPIN_SOFT_IS_LOCKED SPIN_TRY_IRQSAVE
 * greppable: SPIN_ALIGNED SPIN_SOFT_IRQ_PROBE G-SMP
 */
#pragma once

#include <gj/config.h>
#include <gj/types.h>

/**
 * Ticket-free TAS word. Mutate only via __atomic_* (not plain loads/stores
 * that form the lock protocol — soft is_locked uses relaxed atomic load).
 */
struct gj_spinlock {
    u32 u32Locked;
};

/**
 * Cacheline-padded lock for hot shared data (avoids false sharing with
 * adjacent counters / next object). Product plain locks stay sizeof(u32).
 * greppable: SPIN_ALIGNED
 */
struct gj_spinlock_aligned {
    struct gj_spinlock lock;
#if GJ_CACHELINE_SIZE > 4u
    u8 aPad[GJ_CACHELINE_SIZE - 4u];
#endif
} __attribute__((aligned(GJ_CACHELINE_SIZE)));

#define GJ_SPINLOCK_INIT { 0 }
#if GJ_CACHELINE_SIZE > 4u
#define GJ_SPINLOCK_ALIGNED_INIT \
    {                            \
        { 0 }, { 0 }             \
    }
#else
#define GJ_SPINLOCK_ALIGNED_INIT \
    {                            \
        { 0 }                    \
    }
#endif

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
 * Full compiler barrier (ordering for non-atomic side effects around locks).
 * Acquire/release atomics already order the lock word; this is for soft
 * critical-section fencing when mixing with arch MMIO or asm.
 */
static inline void
gj_compiler_barrier(void)
{
    __asm__ volatile ("" ::: "memory");
}

/**
 * Disable local IRQs and return prior flags for gj_irq_restore.
 * Nested use is caller-managed (save/restore pairs must nest LIFO).
 * greppable: SPIN_IRQSAVE
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

/**
 * Soft probe: 1 if local IRQs appear disabled on this CPU, else 0.
 * Architecture best-effort; unknown arch returns 0.
 * greppable: SPIN_SOFT_IRQ_PROBE
 */
static inline int
gj_irq_disabled_soft(void)
{
#if defined(__x86_64__) || defined(__i386__)
    u64 u64Flags;

#if defined(__x86_64__)
    __asm__ volatile ("pushfq; popq %0" : "=r"(u64Flags) :: "memory", "cc");
#else
    __asm__ volatile ("pushfl; popl %0" : "=r"(u64Flags) :: "memory", "cc");
#endif
    /* RFLAGS/EFLAGS bit 9 = IF (interrupts enabled when set). */
    return ((u64Flags & (1ull << 9)) == 0ull) ? 1 : 0;
#elif defined(__aarch64__)
    u64 u64Daif;

    __asm__ volatile ("mrs %0, daif" : "=r"(u64Daif));
    /* DAIF I bit is bit 7 of the DAIF field as presented in the register. */
    return ((u64Daif & (1ull << 7)) != 0ull) ? 1 : 0;
#else
    return 0;
#endif
}

static inline void
gj_spin_init(struct gj_spinlock *pLock)
{
    if (pLock != NULL) {
        __atomic_store_n(&pLock->u32Locked, 0u, __ATOMIC_RELAXED);
    }
}

/** Soft destroy: force unlocked. Symmetry for init; not a refcount. */
static inline void
gj_spin_destroy(struct gj_spinlock *pLock)
{
    if (pLock != NULL) {
        __atomic_store_n(&pLock->u32Locked, 0u, __ATOMIC_RELAXED);
    }
}

/**
 * Soft probe: non-zero if lock word is held (relaxed). Not a substitute for
 * ownership; debug / soft assert only.
 * greppable: SPIN_SOFT_IS_LOCKED
 */
static inline int
gj_spin_is_locked(const struct gj_spinlock *pLock)
{
    if (pLock == NULL) {
        return 0;
    }
    return __atomic_load_n(&pLock->u32Locked, __ATOMIC_RELAXED) != 0u ? 1 : 0;
}

/**
 * Soft assert helper: 1 if locked (or !GJ_DEBUG), 0 if unlocked under debug.
 * No panic dependency — callers may trap/log on 0.
 */
static inline int
gj_spin_assert_locked(const struct gj_spinlock *pLock)
{
#if GJ_DEBUG
    return gj_spin_is_locked(pLock);
#else
    (void)pLock;
    return 1;
#endif
}

/**
 * Soft assert helper: 1 if free (or !GJ_DEBUG), 0 if held under debug.
 */
static inline int
gj_spin_assert_unlocked(const struct gj_spinlock *pLock)
{
#if GJ_DEBUG
    return gj_spin_is_locked(pLock) ? 0 : 1;
#else
    (void)pLock;
    return 1;
#endif
}

static inline void
gj_spin_lock(struct gj_spinlock *pLock)
{
    u32 u32Expect;
    u32 u32Relax;
    u32 u32Iters;

    if (pLock == NULL) {
        return;
    }
    for (;;) {
        /* greppable: SPIN_TAS */
        u32Expect = 0u;
        if (__atomic_compare_exchange_n(&pLock->u32Locked, &u32Expect, 1u,
                                        0 /* strong */, __ATOMIC_ACQUIRE,
                                        __ATOMIC_RELAXED)) {
            gj_compiler_barrier();
            return;
        }
        /* Read-spin while held: reduces store traffic on contended cache line. */
        u32Iters = GJ_SPIN_RELAX_ITERS;
        if (u32Iters == 0u) {
            u32Iters = 1u;
        }
        while (__atomic_load_n(&pLock->u32Locked, __ATOMIC_RELAXED) != 0u) {
            for (u32Relax = 0u; u32Relax < u32Iters; u32Relax++) {
                gj_cpu_relax();
            }
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
    if (__atomic_compare_exchange_n(&pLock->u32Locked, &u32Expect, 1u, 0,
                                    __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) {
        gj_compiler_barrier();
        return 1;
    }
    return 0;
}

static inline void
gj_spin_unlock(struct gj_spinlock *pLock)
{
    if (pLock != NULL) {
        gj_compiler_barrier();
        __atomic_store_n(&pLock->u32Locked, 0u, __ATOMIC_RELEASE);
    }
}

/**
 * Disable local IRQs then acquire lock. Returns flags for
 * gj_spin_unlock_irqrestore. Use when the lock is shared with IRQ paths.
 * greppable: SPIN_IRQSAVE
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

/**
 * Non-blocking irqsave acquire: IRQs masked only for the attempt.
 * On success returns 1 and *pFlags holds restore word for
 * gj_spin_unlock_irqrestore. On failure restores IRQs and returns 0.
 * greppable: SPIN_TRY_IRQSAVE
 */
static inline int
gj_spin_trylock_irqsave(struct gj_spinlock *pLock, u64 *pFlags)
{
    u64 u64Flags;

    if (pLock == NULL || pFlags == NULL) {
        return 0;
    }
    u64Flags = gj_irq_save();
    if (gj_spin_trylock(pLock) != 0) {
        *pFlags = u64Flags;
        return 1;
    }
    gj_irq_restore(u64Flags);
    return 0;
}

/* ---- Aligned-lock shims (operate on .lock member) --------------------- */

static inline void
gj_spin_aligned_init(struct gj_spinlock_aligned *pLock)
{
    if (pLock != NULL) {
        gj_spin_init(&pLock->lock);
    }
}

static inline void
gj_spin_aligned_lock(struct gj_spinlock_aligned *pLock)
{
    if (pLock != NULL) {
        gj_spin_lock(&pLock->lock);
    }
}

static inline int
gj_spin_aligned_trylock(struct gj_spinlock_aligned *pLock)
{
    if (pLock == NULL) {
        return 0;
    }
    return gj_spin_trylock(&pLock->lock);
}

static inline void
gj_spin_aligned_unlock(struct gj_spinlock_aligned *pLock)
{
    if (pLock != NULL) {
        gj_spin_unlock(&pLock->lock);
    }
}

static inline u64
gj_spin_aligned_lock_irqsave(struct gj_spinlock_aligned *pLock)
{
    if (pLock == NULL) {
        return gj_irq_save();
    }
    return gj_spin_lock_irqsave(&pLock->lock);
}

static inline void
gj_spin_aligned_unlock_irqrestore(struct gj_spinlock_aligned *pLock,
                                  u64 u64Flags)
{
    if (pLock != NULL) {
        gj_spin_unlock(&pLock->lock);
    }
    gj_irq_restore(u64Flags);
}

/* Layout contracts (header-only deepen). */
_Static_assert(sizeof(struct gj_spinlock) == 4u, "gj_spinlock TAS word");
_Static_assert((GJ_CACHELINE_SIZE % sizeof(struct gj_spinlock)) == 0u ||
                   GJ_CACHELINE_SIZE >= sizeof(struct gj_spinlock),
               "aligned pad geometry");
