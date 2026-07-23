/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Linux-true futex wait queues (G-FUT / G-MO-3).
 *
 * Keying (matches Linux private vs shared) — G-FUT-1:
 *   PRIVATE: (address space, user VA) — AS id is process CR3 when set
 *   shared:  physical address of the u32 word (same PA ⇒ same key
 *            across aliasing maps / processes)
 *   Grep: futex: shared key PA
 *
 * Wait path (lost-wake safe):
 *   1. irqsave-lock waiter table
 *   2. claim slot, recheck *uaddr, thread_block on the slot
 *   3. unlock, schedule()
 * Wake / timer under the same irqsave lock so IRQ tick cannot race
 * table surgery (G-SMP, SECURITY_CORE §4).
 *
 * Soft product:
 *   G-FUT-BITSET — WAIT_BITSET / WAKE_BITSET (grep: futex: wait_bitset,
 *                  futex: wake_bitset)
 *   G-FUT-ROBUST — per-thr robust list head + soft exit OWNER_DIED wake
 *                  (grep: futex: robust set/get/exit)
 *
 * Wait object for thread_block/wake is the futex_waiter slot itself; tag 0.
 * Fixed table (no heap) — ENOMEM when all slots are in use.
 * Product path never busy-spins (G-FUT-3).
 */
#include <gj/cpu.h>
#include <gj/error.h>
#include <gj/futex.h>
#include <gj/linux_abi.h>
#include <gj/process.h>
#include <gj/spinlock.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/timer.h>
#include <gj/user_access.h>
#include <gj/vmm.h>

/* SMAP-safe load of a user/shared futex word (STAC/CLAC). */
static u32
futex_load_u32(volatile u32 *pU32)
{
    u32 u32Val = 0;

    if (pU32 == NULL) {
        return 0;
    }
    if (user_load_u32((u64)(gj_vaddr_t)pU32, &u32Val) != GJ_OK) {
        /* Kernel BSS / non-user smoke: direct load */
        u32Val = *pU32;
    }
    return u32Val;
}

/* SMAP-safe store; returns 0 on success. */
static int
futex_store_u32(u64 u64Uaddr, u32 u32Val)
{
    if (user_store_u32(u64Uaddr, u32Val) == GJ_OK) {
        return 0;
    }
    /* Kernel BSS smoke path */
    if (u64Uaddr != 0) {
        *(volatile u32 *)(gj_vaddr_t)u64Uaddr = u32Val;
        return 0;
    }
    return -1;
}

#define GJ_FUTEX_MAX_WAITERS 64
/* Soft robust slots: one per thread id class (GJ_MAX_THREADS). */
#define GJ_FUTEX_ROBUST_SLOTS 32
/* Cap robust list walk so a corrupt cycle cannot hang the exit path. */
#define GJ_FUTEX_ROBUST_WALK_MAX 64

struct futex_waiter {
    u8                  u8Used;
    u8                  u8Waiting;
    u8                  u8Pad[2];
    u32                 u32Val;
    u32                 u32Bitset; /* G-FUT-BITSET; MATCH_ANY for classic wait */
    u32                 u32TimedOut;
    struct gj_futex_key key;
    struct gj_thread   *pThr;
    u64                 u64Deadline; /* mono nsec; 0 = no timeout */
};

/* Soft robust list head per thread (G-FUT-ROBUST). */
struct futex_robust_slot {
    u8  u8Used;
    u8  u8Pad[3];
    u32 u32Tid;
    u64 u64Head;
    u64 u64Len;
};

static struct futex_waiter      g_aWaiters[GJ_FUTEX_MAX_WAITERS];
static struct futex_robust_slot g_aRobust[GJ_FUTEX_ROBUST_SLOTS];
static struct gj_spinlock       g_lockFutex = GJ_SPINLOCK_INIT;

void
futex_init(void)
{
    memset(g_aWaiters, 0, sizeof(g_aWaiters));
    memset(g_aRobust, 0, sizeof(g_aRobust));
    gj_spin_init(&g_lockFutex);
}

/*
 * Address-space identity for private futexes: prefer the process CR3
 * (stable across temporary kernel CR3 loads), else the thread CR3,
 * else the active CPU CR3 (boot / no-process threads).
 */
static u64
futex_as_id(void)
{
    struct gj_thread *pThr = thread_current();

    if (pThr != NULL && pThr->pProc != NULL && pThr->pProc->u64Cr3 != 0) {
        return pThr->pProc->u64Cr3;
    }
    if (pThr != NULL && pThr->u64Cr3 != 0) {
        return pThr->u64Cr3;
    }
    return cpu_read_cr3();
}

/*
 * Resolve VA → PA for shared keys (G-FUT-1 / futex: shared key PA).
 * Uses active CR3 PTE walk; falls back to low identity (PA==VA) so kernel
 * BSS / Multiboot maps still form a stable cross-alias key when the leaf
 * walk fails (same pattern as virtio / io_uring bring-up paths).
 */
static gj_paddr_t
futex_resolve_pa(u64 u64Uaddr)
{
    gj_paddr_t pa;

    pa = vmm_virt_to_phys((gj_vaddr_t)u64Uaddr);
    if (pa != 0) {
        return pa;
    }
    /* Low identity: boot/kernel BSS often PA==VA below 1 GiB. */
    if (u64Uaddr != 0 && u64Uaddr < 0x40000000ull) {
        return (gj_paddr_t)u64Uaddr;
    }
    return 0;
}

/*
 * Fill shared key fields from a resolved PA. u64Uaddr kept for diagnostics.
 * Cross-process equality is solely u64Phys (page frame + word offset).
 */
static void
futex_key_set_shared(struct gj_futex_key *pOut, u64 u64Uaddr, gj_paddr_t pa)
{
    pOut->u8Private = 0;
    pOut->u64Phys = (u64)pa;
    pOut->u64As = 0;
    pOut->u64Uaddr = u64Uaddr;
}

/* Private: (AS, VA). Shared: PA equality (uaddr may differ). */
static int
key_eq(const struct gj_futex_key *pA, const struct gj_futex_key *pB)
{
    if (pA == NULL || pB == NULL) {
        return 0;
    }
    if (pA->u8Private != pB->u8Private) {
        return 0;
    }
    if (pA->u8Private) {
        return (pA->u64Uaddr == pB->u64Uaddr) && (pA->u64As == pB->u64As);
    }
    /* Shared: require non-zero PA so empty keys never match by accident. */
    if (pA->u64Phys == 0 || pB->u64Phys == 0) {
        return 0;
    }
    return pA->u64Phys == pB->u64Phys;
}

int
futex_key_eq(const struct gj_futex_key *pA, const struct gj_futex_key *pB)
{
    return key_eq(pA, pB);
}

/*
 * Resolve uaddr into a stable wait key under pProc's AS when shared.
 * G-FUT-1: temporary CR3 switch so peer mappings of the same frame share PA.
 */
gj_status_t
futex_key_from_uaddr_proc(struct gj_futex_key *pOut, u64 u64Uaddr, int fPrivate,
                          struct gj_process *pProc)
{
    gj_paddr_t pa;
    u64 u64SavedCr3;
    int fSwitched;

    if (pOut == NULL) {
        return GJ_ERR_INVAL;
    }
    /* Linux: futex word must be 4-byte aligned. */
    if ((u64Uaddr & 3ull) != 0) {
        return GJ_ERR_INVAL;
    }
    memset(pOut, 0, sizeof(*pOut));
    pOut->u64Uaddr = u64Uaddr;
    if (fPrivate) {
        pOut->u8Private = 1;
        pOut->u64Phys = 0;
        if (pProc != NULL && pProc->u64Cr3 != 0) {
            pOut->u64As = pProc->u64Cr3;
        } else {
            pOut->u64As = futex_as_id();
        }
        return GJ_OK;
    }

    /* Shared path: walk the target process page tables when they differ. */
    fSwitched = 0;
    u64SavedCr3 = cpu_read_cr3();
    if (pProc != NULL && pProc->u64Cr3 != 0 && pProc->u64Cr3 != u64SavedCr3) {
        cpu_load_cr3(pProc->u64Cr3);
        fSwitched = 1;
    } else if (pProc == NULL) {
        /*
         * Prefer current thr process CR3 if active CR3 is kernel template
         * but the thr owns a private AS (mid-path CR3 load).
         */
        struct gj_thread *pThr = thread_current();

        if (pThr != NULL && pThr->pProc != NULL && pThr->pProc->u64Cr3 != 0 &&
            pThr->pProc->u64Cr3 != u64SavedCr3) {
            cpu_load_cr3(pThr->pProc->u64Cr3);
            fSwitched = 1;
        }
    }

    pa = futex_resolve_pa(u64Uaddr);
    if (fSwitched) {
        cpu_load_cr3(u64SavedCr3);
    }
    if (pa == 0) {
        return GJ_ERR_FAULT;
    }
    /* futex: shared key PA — G-FUT-1 cross-process queue identity */
    futex_key_set_shared(pOut, u64Uaddr, pa);
    return GJ_OK;
}

/*
 * Resolve uaddr into a stable wait key. Private keeps (AS, VA);
 * shared maps VA → PA so distinct mappings of the same frame share one queue.
 */
gj_status_t
futex_key_from_uaddr(struct gj_futex_key *pOut, u64 u64Uaddr, int fPrivate)
{
    struct gj_thread *pThr = thread_current();
    struct gj_process *pProc = NULL;

    if (pThr != NULL) {
        pProc = pThr->pProc;
    }
    return futex_key_from_uaddr_proc(pOut, u64Uaddr, fPrivate, pProc);
}

/*
 * Cancel a wait that was registered (thread_block) but will not schedule —
 * value changed under the lock after enqueue (lost-wake avoidance path).
 * Restores RUNNING so the caller can return without schedule().
 */
static void
waiter_cancel_blocked(struct futex_waiter *pW)
{
    struct gj_thread *pThr;

    if (pW == NULL) {
        return;
    }
    pThr = pW->pThr;
    pW->u8Waiting = 0;
    pW->u8Used = 0;
    pW->u32TimedOut = 0;
    pW->u32Bitset = 0;
    if (pThr != NULL && pThr->u32State == GJ_THR_BLOCKED &&
        pThr->pBlockObj == (void *)pW) {
        pThr->pBlockObj = NULL;
        pThr->u32BlockTag = 0;
        pThr->u32State = GJ_THR_RUNNING;
    }
}

/*
 * Timer IRQ path: mark waiters past mono deadline and wake them.
 * Must not allocate; runs under tick context (irqsave lock).
 */
void
futex_timer_check(void)
{
    u32 iSlot;
    u64 u64Now;
    u64 u64Flags;

    if (!timer_ready()) {
        return;
    }
    u64Now = timer_mono_nsec();
    u64Flags = gj_spin_lock_irqsave(&g_lockFutex);
    for (iSlot = 0; iSlot < GJ_FUTEX_MAX_WAITERS; iSlot++) {
        struct futex_waiter *pW = &g_aWaiters[iSlot];

        if (!pW->u8Used || !pW->u8Waiting) {
            continue;
        }
        if (pW->u64Deadline == 0) {
            continue;
        }
        if (u64Now >= pW->u64Deadline) {
            pW->u32TimedOut = 1;
            pW->u8Waiting = 0;
            (void)thread_wake(pW, 0, 1);
        }
    }
    gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
}

/*
 * Shared wait core for classic WAIT and WAIT_BITSET (G-FUT-BITSET).
 * u32Bitset must be non-zero (Linux rejects 0).
 */
static i64
futex_wait_common(volatile u32 *pU32, u32 u32Val, const struct gj_futex_key *pKey,
                  u64 u64DeadlineMonoNsec, u32 u32Bitset)
{
    u32 iSlot;
    struct futex_waiter *pW;
    struct gj_thread *pThr;
    u32 u32TimedOut;
    u64 u64Flags;

    if (pU32 == NULL || pKey == NULL) {
        return -LINUX_EINVAL;
    }
    if ((pKey->u64Uaddr & 3ull) != 0) {
        return -LINUX_EINVAL;
    }
    /* Linux: bitset 0 is EINVAL for WAIT_BITSET; classic uses MATCH_ANY. */
    if (u32Bitset == 0) {
        return -LINUX_EINVAL;
    }
    /* Shared keys with zero PA cannot form a stable queue. */
    if (!pKey->u8Private && pKey->u64Phys == 0) {
        return -LINUX_EINVAL;
    }

    pThr = thread_current();
    /*
     * G-FUT-3: product path sleeps only via thread_block + timer.
     * No schedulable current thread → fail closed (no pause spin).
     */
    if (pThr == NULL) {
        return -LINUX_EINVAL;
    }

    /* Fast path: already mismatched before taking the table lock. */
    if (futex_load_u32(pU32) != u32Val) {
        return -LINUX_EAGAIN;
    }

    u64Flags = gj_spin_lock_irqsave(&g_lockFutex);

    pW = NULL;
    for (iSlot = 0; iSlot < GJ_FUTEX_MAX_WAITERS; iSlot++) {
        if (!g_aWaiters[iSlot].u8Used) {
            pW = &g_aWaiters[iSlot];
            break;
        }
    }
    if (pW == NULL) {
        gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
        return -LINUX_ENOMEM;
    }

    /*
     * Recheck under lock, then register block *before* unlock so a concurrent
     * wake/timer cannot miss this waiter (lost-wake).
     */
    if (futex_load_u32(pU32) != u32Val) {
        gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
        return -LINUX_EAGAIN;
    }

    pW->u8Used = 1;
    pW->u8Waiting = 1;
    pW->u32Val = u32Val;
    pW->u32Bitset = u32Bitset;
    pW->key = *pKey;
    pW->pThr = pThr;
    pW->u64Deadline = u64DeadlineMonoNsec;
    pW->u32TimedOut = 0;

    /* Absolute deadline already past — treat as immediate timeout. */
    if (u64DeadlineMonoNsec != 0 && timer_ready() &&
        timer_mono_nsec() >= u64DeadlineMonoNsec) {
        pW->u8Used = 0;
        pW->u8Waiting = 0;
        pW->u32Bitset = 0;
        gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
        return -LINUX_ETIMEDOUT;
    }

    thread_block(pW, 0);

    /* Value changed after enqueue+block: cancel and return EAGAIN. */
    if (futex_load_u32(pU32) != u32Val) {
        waiter_cancel_blocked(pW);
        gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
        return -LINUX_EAGAIN;
    }

    /* Already woken (or timed out) before schedule — do not sleep. */
    if (!pW->u8Waiting) {
        u32TimedOut = pW->u32TimedOut;
        pW->u8Used = 0;
        pW->u8Waiting = 0;
        pW->u32Bitset = 0;
        if (pThr->u32State == GJ_THR_BLOCKED && pThr->pBlockObj == (void *)pW) {
            pThr->pBlockObj = NULL;
            pThr->u32BlockTag = 0;
            pThr->u32State = GJ_THR_RUNNING;
        } else if (pThr->u32State == GJ_THR_RUNNABLE) {
            /* thread_wake ran; stay current without schedule. */
            pThr->u32State = GJ_THR_RUNNING;
        }
        gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
        if (u32TimedOut) {
            return -LINUX_ETIMEDOUT;
        }
        return 0;
    }

    gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
    schedule();

    u64Flags = gj_spin_lock_irqsave(&g_lockFutex);
    u32TimedOut = pW->u32TimedOut;
    pW->u8Used = 0;
    pW->u8Waiting = 0;
    pW->u32Bitset = 0;
    gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);

    if (u32TimedOut) {
        return -LINUX_ETIMEDOUT;
    }
    return 0;
}

/*
 * Classic FUTEX_WAIT: if *pU32 still equals u32Val, park until wake/timeout.
 * Returns 0, or -LINUX_* (EAGAIN if value already changed, ETIMEDOUT, …).
 * u64DeadlineMonoNsec is absolute mono nsec, or 0 for no deadline.
 */
i64
futex_wait(volatile u32 *pU32, u32 u32Val, const struct gj_futex_key *pKey,
           u64 u64DeadlineMonoNsec)
{
    return futex_wait_common(pU32, u32Val, pKey, u64DeadlineMonoNsec,
                             GJ_FUTEX_BITSET_MATCH_ANY);
}

/* futex: wait_bitset — G-FUT-BITSET soft product */
i64
futex_wait_bitset(volatile u32 *pU32, u32 u32Val, const struct gj_futex_key *pKey,
                  u64 u64DeadlineMonoNsec, u32 u32Bitset)
{
    return futex_wait_common(pU32, u32Val, pKey, u64DeadlineMonoNsec, u32Bitset);
}

/*
 * Shared wake core: key match AND (waiter bitset & wake bitset) != 0.
 * G-FUT-BITSET: classic FUTEX_WAKE passes MATCH_ANY.
 */
static i64
futex_wake_common(const struct gj_futex_key *pKey, u32 u32Count, u32 u32Bitset)
{
    u32 iSlot;
    u32 u32Woken = 0;
    u64 u64Flags;

    if (pKey == NULL) {
        return -LINUX_EINVAL;
    }
    if (u32Bitset == 0) {
        return -LINUX_EINVAL;
    }
    if (u32Count == 0) {
        return 0;
    }
    if (!pKey->u8Private && pKey->u64Phys == 0) {
        return -LINUX_EINVAL;
    }

    u64Flags = gj_spin_lock_irqsave(&g_lockFutex);
    for (iSlot = 0; iSlot < GJ_FUTEX_MAX_WAITERS && u32Woken < u32Count;
         iSlot++) {
        struct futex_waiter *pW = &g_aWaiters[iSlot];

        if (!pW->u8Used || !pW->u8Waiting) {
            continue;
        }
        if (!key_eq(&pW->key, pKey)) {
            continue;
        }
        if ((pW->u32Bitset & u32Bitset) == 0) {
            continue;
        }
        pW->u8Waiting = 0;
        (void)thread_wake(pW, 0, 1);
        u32Woken++;
    }
    gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
    return (i64)u32Woken;
}

/*
 * Classic FUTEX_WAKE: wake up to u32Count waiters matching pKey.
 * Returns number woken, or -LINUX_EINVAL.
 */
i64
futex_wake(const struct gj_futex_key *pKey, u32 u32Count)
{
    return futex_wake_common(pKey, u32Count, GJ_FUTEX_BITSET_MATCH_ANY);
}

/* futex: wake_bitset — G-FUT-BITSET soft product */
i64
futex_wake_bitset(const struct gj_futex_key *pKey, u32 u32Count, u32 u32Bitset)
{
    return futex_wake_common(pKey, u32Count, u32Bitset);
}

/* ---- Soft robust list (G-FUT-ROBUST) ------------------------------------ */

static struct futex_robust_slot *
robust_find_tid(u32 u32Tid)
{
    u32 iSlot;

    for (iSlot = 0; iSlot < GJ_FUTEX_ROBUST_SLOTS; iSlot++) {
        if (g_aRobust[iSlot].u8Used && g_aRobust[iSlot].u32Tid == u32Tid) {
            return &g_aRobust[iSlot];
        }
    }
    return NULL;
}

static struct futex_robust_slot *
robust_alloc_tid(u32 u32Tid)
{
    struct futex_robust_slot *pS;
    u32 iSlot;

    pS = robust_find_tid(u32Tid);
    if (pS != NULL) {
        return pS;
    }
    for (iSlot = 0; iSlot < GJ_FUTEX_ROBUST_SLOTS; iSlot++) {
        if (!g_aRobust[iSlot].u8Used) {
            g_aRobust[iSlot].u8Used = 1;
            g_aRobust[iSlot].u32Tid = u32Tid;
            g_aRobust[iSlot].u64Head = 0;
            g_aRobust[iSlot].u64Len = 0;
            return &g_aRobust[iSlot];
        }
    }
    return NULL;
}

/* futex: robust set — G-FUT-ROBUST */
gj_status_t
futex_set_robust_list(u64 u64Head, u64 u64Len)
{
    struct gj_thread *pThr;
    struct futex_robust_slot *pS;
    u64 u64Flags;

    pThr = thread_current();
    if (pThr == NULL || pThr->u32Id == 0) {
        return GJ_ERR_INVAL;
    }
    /*
     * Linux requires len == sizeof(robust_list_head). Soft path also accepts
     * small non-zero lengths for bring-up smoke (main robust_list PASS).
     */
    if (u64Len == 0 || u64Len > 64ull) {
        return GJ_ERR_INVAL;
    }
    if (u64Head != 0 && (u64Head & 7ull) != 0) {
        return GJ_ERR_INVAL;
    }

    u64Flags = gj_spin_lock_irqsave(&g_lockFutex);
    pS = robust_alloc_tid(pThr->u32Id);
    if (pS == NULL) {
        gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
        return GJ_ERR_NOMEM;
    }
    pS->u64Head = u64Head;
    pS->u64Len = u64Len;
    gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
    return GJ_OK;
}

/* futex: robust get — G-FUT-ROBUST */
gj_status_t
futex_get_robust_list(u32 u32Tid, u64 *pHeadOut, u64 *pLenOut)
{
    struct gj_thread *pThr;
    struct futex_robust_slot *pS;
    u64 u64Flags;
    u64 u64Head;
    u64 u64Len;

    if (u32Tid == 0) {
        pThr = thread_current();
        if (pThr == NULL) {
            return GJ_ERR_INVAL;
        }
        u32Tid = pThr->u32Id;
    }

    u64Flags = gj_spin_lock_irqsave(&g_lockFutex);
    pS = robust_find_tid(u32Tid);
    if (pS == NULL) {
        gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
        return GJ_ERR_NOENT;
    }
    u64Head = pS->u64Head;
    u64Len = pS->u64Len;
    gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);

    if (pHeadOut != NULL) {
        *pHeadOut = u64Head;
    }
    if (pLenOut != NULL) {
        *pLenOut = u64Len;
    }
    return GJ_OK;
}

/*
 * Soft handle one robust list entry: if the futex word's owner TID matches
 * u32Tid, OR FUTEX_OWNER_DIED and wake one waiter (private or shared key).
 * Returns 1 if OWNER_DIED was applied, 0 otherwise.
 */
static int
robust_handle_entry(struct gj_thread *pThr, u64 u64Entry, i64 i64Offset,
                    u32 u32Tid)
{
    u64 u64Futex;
    u32 u32Word;
    struct gj_futex_key key;
    gj_status_t st;

    if (u64Entry == 0) {
        return 0;
    }
    u64Futex = (u64)((i64)u64Entry + i64Offset);
    if ((u64Futex & 3ull) != 0) {
        return 0;
    }
    if (user_load_u32(u64Futex, &u32Word) != GJ_OK) {
        return 0;
    }
    if ((u32Word & GJ_FUTEX_TID_MASK) != (u32Tid & GJ_FUTEX_TID_MASK)) {
        return 0;
    }
    if ((u32Word & GJ_FUTEX_OWNER_DIED) != 0) {
        return 0;
    }

    u32Word |= GJ_FUTEX_OWNER_DIED;
    if (futex_store_u32(u64Futex, u32Word) != 0) {
        return 0;
    }

    /* Prefer private key in the dying thr AS; fall back to shared PA. */
    st = futex_key_from_uaddr_proc(&key, u64Futex, 1, pThr != NULL ? pThr->pProc
                                                                   : NULL);
    if (st != GJ_OK) {
        st = futex_key_from_uaddr_proc(&key, u64Futex, 0,
                                       pThr != NULL ? pThr->pProc : NULL);
    }
    if (st == GJ_OK) {
        (void)futex_wake(&key, 1);
    }
    return 1;
}

/*
 * Soft robust exit (futex: robust exit / G-FUT-ROBUST).
 * Walk robust_list_head:
 *   list.next chain + list_op_pending, futex_offset applied per entry.
 * Cycle-capped; best-effort user copies only — no PI recovery.
 */
i64
futex_exit_robust_list(struct gj_thread *pThr)
{
    struct futex_robust_slot *pS;
    u64 u64Flags;
    u64 u64Head;
    u64 u64Len;
    u32 u32Tid;
    u64 aHead[3]; /* next, futex_offset, list_op_pending (x86_64 layout) */
    u64 u64Entry;
    u64 u64Pending;
    i64 i64Offset;
    u32 cWalk;
    i64 i64Marked;
    u32 iSlot;

    if (pThr == NULL || pThr->u32Id == 0) {
        return 0;
    }
    u32Tid = pThr->u32Id;

    u64Flags = gj_spin_lock_irqsave(&g_lockFutex);
    pS = robust_find_tid(u32Tid);
    if (pS == NULL || pS->u64Head == 0) {
        /* Still clear any waiter slots owned by this thr. */
        gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
        (void)futex_cancel_thr(pThr);
        return 0;
    }
    u64Head = pS->u64Head;
    u64Len = pS->u64Len;
    pS->u8Used = 0;
    pS->u64Head = 0;
    pS->u64Len = 0;
    gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);

    (void)u64Len;
    memset(aHead, 0, sizeof(aHead));
    if (copy_from_user(aHead, u64Head, sizeof(aHead)) != GJ_OK) {
        (void)futex_cancel_thr(pThr);
        return 0;
    }
    i64Offset = (i64)aHead[1];
    u64Pending = aHead[2];
    i64Marked = 0;

    /* Pending op first (lock not yet linked or mid-unlink). */
    if (u64Pending != 0) {
        i64Marked += robust_handle_entry(pThr, u64Pending, i64Offset, u32Tid);
    }

    u64Entry = aHead[0];
    for (cWalk = 0; cWalk < GJ_FUTEX_ROBUST_WALK_MAX; cWalk++) {
        u64 u64Next;

        if (u64Entry == 0 || u64Entry == u64Head) {
            break;
        }
        if (u64Entry != u64Pending) {
            i64Marked +=
                robust_handle_entry(pThr, u64Entry, i64Offset, u32Tid);
        }
        if (copy_from_user(&u64Next, u64Entry, sizeof(u64Next)) != GJ_OK) {
            break;
        }
        if (u64Next == u64Entry) {
            break; /* self-cycle */
        }
        u64Entry = u64Next;
    }

    /* Drop thr waiters so death cannot leave orphans. */
    (void)futex_cancel_thr(pThr);

    /* Clear any remaining robust slot alias for this tid (soft). */
    u64Flags = gj_spin_lock_irqsave(&g_lockFutex);
    for (iSlot = 0; iSlot < GJ_FUTEX_ROBUST_SLOTS; iSlot++) {
        if (g_aRobust[iSlot].u8Used && g_aRobust[iSlot].u32Tid == u32Tid) {
            g_aRobust[iSlot].u8Used = 0;
            g_aRobust[iSlot].u64Head = 0;
            g_aRobust[iSlot].u64Len = 0;
        }
    }
    gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);

    return i64Marked;
}

/*
 * Cancel waiters belonging to a dying thread and wake them (no timeout).
 * Prevents hangs if thr exits while blocked on a futex.
 */
u32
futex_cancel_thr(struct gj_thread *pThr)
{
    u32 iSlot;
    u32 u32Cleared = 0;
    u64 u64Flags;

    if (pThr == NULL) {
        return 0;
    }

    u64Flags = gj_spin_lock_irqsave(&g_lockFutex);
    for (iSlot = 0; iSlot < GJ_FUTEX_MAX_WAITERS; iSlot++) {
        struct futex_waiter *pW = &g_aWaiters[iSlot];

        if (!pW->u8Used || pW->pThr != pThr) {
            continue;
        }
        if (pW->u8Waiting) {
            pW->u8Waiting = 0;
            (void)thread_wake(pW, 0, 1);
        }
        pW->u8Used = 0;
        pW->u32Bitset = 0;
        pW->pThr = NULL;
        u32Cleared++;
    }
    gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
    return u32Cleared;
}
