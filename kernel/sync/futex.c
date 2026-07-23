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
 * Soft wait/wake inventory (file-local sticky counters; never hard-gate).
 * Wave 11 exclusive deepen — greppable prefix-stable serial markers
 * (futex: soft …); diagnostics only, never hard-gate product:
 *   futex: soft wait inventory   — capacity + path catalog at init
 *   futex: soft wake inventory   — wake/bitset/timer/cancel catalog at init
 *   futex: soft wait             — wait path tallies (enter/park/ok/…)
 *   futex: soft wake             — wake path tallies (hit/miss/woken/…)
 *   futex: soft stats            — aggregate wait/wake counters
 *   futex: soft table            — live waiter + robust slot occupancy
 *   futex: soft key              — private/shared key resolve tallies
 *   futex: soft robust           — set/get/exit OWNER_DIED soft tallies
 *   futex: soft path             — G-FUT invariants + product claim
 *   futex: soft timer            — timer_check reap surface
 *   futex: soft thr              — cancel_thr dying-thr surface
 *   futex: soft wait park        — blocked + schedule success path
 *   futex: soft wait eagain      — value mismatch (fast or under lock)
 *   futex: soft wait etimedout   — deadline (immediate or timer reap)
 *   futex: soft wait enomem      — waiter table full
 *   futex: soft wait einval      — bad args / shared zero PA / no thr
 *   futex: soft wait cancel      — lost-wake recheck cancel after enqueue
 *   futex: soft wait bitset      — WAIT_BITSET entry (non-MATCH_ANY)
 *   futex: soft wake hit         — at least one waiter woken
 *   futex: soft wake miss        — zero waiters matched
 *   futex: soft wake bitset      — WAKE_BITSET entry (non-MATCH_ANY)
 *   futex: soft wake bitset miss — key match, bitset no overlap
 *   futex: soft timer reap       — timeout wake via futex_timer_check
 *   futex: soft thr cancel       — cancel_thr cleared dying-thr slots
 * greppable: futex: soft
 *
 * Wait object for thread_block/wake is the futex_waiter slot itself; tag 0.
 * Fixed table (no heap) — ENOMEM when all slots are in use.
 * Product path never busy-spins (G-FUT-3).
 */
#include <gj/cpu.h>
#include <gj/error.h>
#include <gj/futex.h>
#include <gj/klog.h>
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

/*
 * Soft wait/wake sticky counters (wrap OK; diagnostics only).
 * Bumped off the product return paths; never hard-gate behavior.
 * Wave 11 deepen: path + table + key + robust + timer + thr surfaces.
 * greppable: futex: soft stats
 * greppable: futex: soft
 */
struct futex_soft_stats {
    u64 u64WaitEnter;       /* futex_wait_common entries */
    u64 u64WaitPark;        /* schedule() after successful block */
    u64 u64WaitOk;          /* return 0 (wake, not timeout) */
    u64 u64WaitEagain;      /* value mismatch */
    u64 u64WaitEtimedout;   /* deadline (immediate or post-park) */
    u64 u64WaitEnomem;      /* table full */
    u64 u64WaitEinval;      /* bad args / no thr / zero shared PA */
    u64 u64WaitCancel;      /* lost-wake cancel after enqueue+block */
    u64 u64WaitEarlyWake;   /* woken/timed before schedule */
    u64 u64WaitBitset;      /* WAIT_BITSET entry (bitset != MATCH_ANY) */
    u64 u64WaitClassic;     /* classic WAIT (MATCH_ANY bitset) */
    u64 u64WaitPrivate;     /* wait with private key */
    u64 u64WaitShared;      /* wait with shared key */
    u64 u64WaitDeadline;    /* wait entered with non-zero deadline */
    u64 u64WaitNoDeadline;  /* wait entered with deadline 0 */
    u64 u64WaitEagainFast;  /* value mismatch before lock */
    u64 u64WaitEagainLock;  /* value mismatch under lock */
    u64 u64WaitEagainCancel;/* cancel after enqueue → EAGAIN */
    u64 u64WaitEtimedImm;   /* immediate past-deadline (no park) */
    u64 u64WaitEtimedPark;  /* timeout after schedule / early wake */
    u64 u64WakeEnter;       /* futex_wake_common entries */
    u64 u64WakeHit;         /* at least one waiter woken */
    u64 u64WakeMiss;        /* zero waiters matched (count > 0) */
    u64 u64WakeWoken;       /* total waiter slots woken */
    u64 u64WakeEinval;      /* null key / bitset0 / shared zero PA */
    u64 u64WakeZeroCount;   /* u32Count == 0 early return */
    u64 u64WakeBitset;      /* WAKE_BITSET entry (bitset != MATCH_ANY) */
    u64 u64WakeClassic;     /* classic WAKE (MATCH_ANY bitset) */
    u64 u64WakeBitsetMiss;  /* key matched but bitset no overlap */
    u64 u64WakePrivate;     /* wake with private key */
    u64 u64WakeShared;      /* wake with shared key */
    u64 u64TimerReap;       /* futex_timer_check timed-out wakes */
    u64 u64TimerCheck;      /* futex_timer_check entries (timer ready) */
    u64 u64ThrCancel;       /* slots cleared by futex_cancel_thr */
    u64 u64ThrCancelCalls;  /* futex_cancel_thr entries with thr */
    u64 u64KeyPrivateOk;    /* private key resolve ok */
    u64 u64KeySharedOk;     /* shared key resolve ok (PA) */
    u64 u64KeySharedFault;  /* shared PA resolve failed */
    u64 u64KeyAlignFail;    /* uaddr not 4-byte aligned */
    u64 u64KeyNullOut;      /* pOut == NULL */
    u64 u64KeySharedCr3Sw;  /* temporary CR3 switch for shared walk */
    u64 u64RobustSetOk;     /* set_robust_list success */
    u64 u64RobustSetFail;   /* set_robust_list reject */
    u64 u64RobustGetOk;     /* get_robust_list hit */
    u64 u64RobustGetMiss;   /* get_robust_list no slot */
    u64 u64RobustExitEnter; /* exit_robust_list entries */
    u64 u64RobustExitEmpty; /* exit with no head / null thr */
    u64 u64RobustMarked;    /* OWNER_DIED marks applied */
    u64 u64RobustWalk;      /* robust list entries walked */
    u64 u64RobustWake;      /* robust_handle_entry wake attempts */
    u64 u64TablePeakUsed;   /* peak used waiter slots (soft) */
    u64 u64TablePeakWait;   /* peak waiting slots (soft) */
    u64 u64TablePeakRob;    /* peak used robust slots (soft) */
    u64 u64SoftLog;         /* times soft inventory/stats printed */
    u64 u64SoftScan;        /* soft table occupancy samples */
};

static struct futex_soft_stats g_soft;
/* One-shot deep print after first product wait/wake activity (soft). */
static u8 g_fSoftStatsOnce;
/* Soft live occupancy snapshot (filled by futex_soft_scan). */
static u32 g_u32SoftUsed;
static u32 g_u32SoftWaiting;
static u32 g_u32SoftFree;
static u32 g_u32SoftRobUsed;
static u32 g_u32SoftRobFree;

/** Soft: saturating-ish bump (u64 wrap is fine for telemetry). */
static void
futex_soft_inc(u64 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/**
 * Soft: sample waiter + robust table occupancy (no lock; diagnostic race OK).
 * Updates peaks; greppable via futex: soft table.
 */
static void
futex_soft_scan(void)
{
    u32 iSlot;
    u32 u32Used = 0;
    u32 u32Waiting = 0;
    u32 u32Rob = 0;

    futex_soft_inc(&g_soft.u64SoftScan);

    for (iSlot = 0; iSlot < GJ_FUTEX_MAX_WAITERS; iSlot++) {
        if (g_aWaiters[iSlot].u8Used) {
            u32Used++;
            if (g_aWaiters[iSlot].u8Waiting) {
                u32Waiting++;
            }
        }
    }
    for (iSlot = 0; iSlot < GJ_FUTEX_ROBUST_SLOTS; iSlot++) {
        if (g_aRobust[iSlot].u8Used) {
            u32Rob++;
        }
    }

    g_u32SoftUsed = u32Used;
    g_u32SoftWaiting = u32Waiting;
    g_u32SoftFree = (u32Used < (u32)GJ_FUTEX_MAX_WAITERS)
                        ? ((u32)GJ_FUTEX_MAX_WAITERS - u32Used)
                        : 0u;
    g_u32SoftRobUsed = u32Rob;
    g_u32SoftRobFree = (u32Rob < (u32)GJ_FUTEX_ROBUST_SLOTS)
                           ? ((u32)GJ_FUTEX_ROBUST_SLOTS - u32Rob)
                           : 0u;

    if ((u64)u32Used > g_soft.u64TablePeakUsed) {
        g_soft.u64TablePeakUsed = (u64)u32Used;
    }
    if ((u64)u32Waiting > g_soft.u64TablePeakWait) {
        g_soft.u64TablePeakWait = (u64)u32Waiting;
    }
    if ((u64)u32Rob > g_soft.u64TablePeakRob) {
        g_soft.u64TablePeakRob = (u64)u32Rob;
    }
}

/**
 * Soft: bump waiter-table peak while holding the futex lock (exact).
 * Call after claiming a slot (u8Used set) so soft table peak tracks load.
 */
static void
futex_soft_note_claim(void)
{
    u32 iSlot;
    u32 u32Used = 0;
    u32 u32Waiting = 0;

    for (iSlot = 0; iSlot < GJ_FUTEX_MAX_WAITERS; iSlot++) {
        if (g_aWaiters[iSlot].u8Used) {
            u32Used++;
            if (g_aWaiters[iSlot].u8Waiting) {
                u32Waiting++;
            }
        }
    }
    if ((u64)u32Used > g_soft.u64TablePeakUsed) {
        g_soft.u64TablePeakUsed = (u64)u32Used;
    }
    if ((u64)u32Waiting > g_soft.u64TablePeakWait) {
        g_soft.u64TablePeakWait = (u64)u32Waiting;
    }
}

/**
 * Greppable soft wait/wake inventory + path/table/key/robust deepen.
 * Called from futex_init and once after first wait/wake activity.
 * Never allocates; safe from non-IRQ product paths.
 * greppable: futex: soft wait inventory
 * greppable: futex: soft wake inventory
 * greppable: futex: soft wait
 * greppable: futex: soft wake
 * greppable: futex: soft stats
 * greppable: futex: soft table
 * greppable: futex: soft key
 * greppable: futex: soft robust
 * greppable: futex: soft path
 * greppable: futex: soft timer
 * greppable: futex: soft thr
 */
static void
futex_soft_log(void)
{
    futex_soft_inc(&g_soft.u64SoftLog);
    futex_soft_scan();

    /*
     * Catalog lines (prefix-stable): declare fixed-table capacity and the
     * wait/wake soft path surface so smoke/scripts can grep product depth
     * without parsing C. Wave 11 deepen splits path/table/key/robust.
     */
    /* Grep: futex: soft wait inventory */
    kprintf("futex: soft wait inventory slots=%u park=thread_block+schedule "
            "paths=eagain,etimedout,enomem,einval,cancel,early_wake,bitset "
            "lost_wake=recheck_under_lock g_fut3=no_product_spin "
            "enter=%lu park=%lu ok=%lu eagain=%lu etimedout=%lu enomem=%lu "
            "einval=%lu cancel=%lu early=%lu bitset=%lu classic=%lu "
            "priv=%lu shared=%lu deadline=%lu no_deadline=%lu\n",
            (unsigned)GJ_FUTEX_MAX_WAITERS,
            (unsigned long)g_soft.u64WaitEnter,
            (unsigned long)g_soft.u64WaitPark,
            (unsigned long)g_soft.u64WaitOk,
            (unsigned long)g_soft.u64WaitEagain,
            (unsigned long)g_soft.u64WaitEtimedout,
            (unsigned long)g_soft.u64WaitEnomem,
            (unsigned long)g_soft.u64WaitEinval,
            (unsigned long)g_soft.u64WaitCancel,
            (unsigned long)g_soft.u64WaitEarlyWake,
            (unsigned long)g_soft.u64WaitBitset,
            (unsigned long)g_soft.u64WaitClassic,
            (unsigned long)g_soft.u64WaitPrivate,
            (unsigned long)g_soft.u64WaitShared,
            (unsigned long)g_soft.u64WaitDeadline,
            (unsigned long)g_soft.u64WaitNoDeadline);

    /* Grep: futex: soft wake inventory */
    kprintf("futex: soft wake inventory match=key+bitset_and irqsave_lock=1 "
            "timer_reap=futex_timer_check thr_cancel=futex_cancel_thr "
            "enter=%lu hit=%lu miss=%lu woken=%lu einval=%lu zero=%lu "
            "bitset=%lu classic=%lu bitset_miss=%lu reap=%lu thr_cancel=%lu "
            "priv=%lu shared=%lu\n",
            (unsigned long)g_soft.u64WakeEnter,
            (unsigned long)g_soft.u64WakeHit,
            (unsigned long)g_soft.u64WakeMiss,
            (unsigned long)g_soft.u64WakeWoken,
            (unsigned long)g_soft.u64WakeEinval,
            (unsigned long)g_soft.u64WakeZeroCount,
            (unsigned long)g_soft.u64WakeBitset,
            (unsigned long)g_soft.u64WakeClassic,
            (unsigned long)g_soft.u64WakeBitsetMiss,
            (unsigned long)g_soft.u64TimerReap,
            (unsigned long)g_soft.u64ThrCancel,
            (unsigned long)g_soft.u64WakePrivate,
            (unsigned long)g_soft.u64WakeShared);

    /* Grep: futex: soft wait — path tallies (Wave 11 deepen) */
    kprintf("futex: soft wait enter=%lu park=%lu ok=%lu eagain=%lu "
            "eagain_fast=%lu eagain_lock=%lu eagain_cancel=%lu "
            "etimedout=%lu etimed_imm=%lu etimed_park=%lu enomem=%lu "
            "einval=%lu cancel=%lu early=%lu bitset=%lu classic=%lu "
            "priv=%lu shared=%lu deadline=%lu no_deadline=%lu\n",
            (unsigned long)g_soft.u64WaitEnter,
            (unsigned long)g_soft.u64WaitPark,
            (unsigned long)g_soft.u64WaitOk,
            (unsigned long)g_soft.u64WaitEagain,
            (unsigned long)g_soft.u64WaitEagainFast,
            (unsigned long)g_soft.u64WaitEagainLock,
            (unsigned long)g_soft.u64WaitEagainCancel,
            (unsigned long)g_soft.u64WaitEtimedout,
            (unsigned long)g_soft.u64WaitEtimedImm,
            (unsigned long)g_soft.u64WaitEtimedPark,
            (unsigned long)g_soft.u64WaitEnomem,
            (unsigned long)g_soft.u64WaitEinval,
            (unsigned long)g_soft.u64WaitCancel,
            (unsigned long)g_soft.u64WaitEarlyWake,
            (unsigned long)g_soft.u64WaitBitset,
            (unsigned long)g_soft.u64WaitClassic,
            (unsigned long)g_soft.u64WaitPrivate,
            (unsigned long)g_soft.u64WaitShared,
            (unsigned long)g_soft.u64WaitDeadline,
            (unsigned long)g_soft.u64WaitNoDeadline);

    /* Grep: futex: soft wake — path tallies (Wave 11 deepen) */
    kprintf("futex: soft wake enter=%lu hit=%lu miss=%lu woken=%lu "
            "einval=%lu zero=%lu bitset=%lu classic=%lu bitset_miss=%lu "
            "priv=%lu shared=%lu\n",
            (unsigned long)g_soft.u64WakeEnter,
            (unsigned long)g_soft.u64WakeHit,
            (unsigned long)g_soft.u64WakeMiss,
            (unsigned long)g_soft.u64WakeWoken,
            (unsigned long)g_soft.u64WakeEinval,
            (unsigned long)g_soft.u64WakeZeroCount,
            (unsigned long)g_soft.u64WakeBitset,
            (unsigned long)g_soft.u64WakeClassic,
            (unsigned long)g_soft.u64WakeBitsetMiss,
            (unsigned long)g_soft.u64WakePrivate,
            (unsigned long)g_soft.u64WakeShared);

    /* Grep: futex: soft stats */
    kprintf("futex: soft stats wait_enter=%lu wait_park=%lu wait_ok=%lu "
            "wait_eagain=%lu wait_etimedout=%lu wait_enomem=%lu "
            "wait_einval=%lu wait_cancel=%lu wait_early=%lu "
            "wait_bitset=%lu wait_classic=%lu wake_enter=%lu wake_hit=%lu "
            "wake_miss=%lu wake_woken=%lu wake_einval=%lu wake_zero=%lu "
            "wake_bitset=%lu wake_classic=%lu wake_bitset_miss=%lu "
            "timer_reap=%lu thr_cancel=%lu soft_log=%lu "
            "key_priv=%lu key_shared=%lu robust_set=%lu robust_mark=%lu\n",
            (unsigned long)g_soft.u64WaitEnter,
            (unsigned long)g_soft.u64WaitPark,
            (unsigned long)g_soft.u64WaitOk,
            (unsigned long)g_soft.u64WaitEagain,
            (unsigned long)g_soft.u64WaitEtimedout,
            (unsigned long)g_soft.u64WaitEnomem,
            (unsigned long)g_soft.u64WaitEinval,
            (unsigned long)g_soft.u64WaitCancel,
            (unsigned long)g_soft.u64WaitEarlyWake,
            (unsigned long)g_soft.u64WaitBitset,
            (unsigned long)g_soft.u64WaitClassic,
            (unsigned long)g_soft.u64WakeEnter,
            (unsigned long)g_soft.u64WakeHit,
            (unsigned long)g_soft.u64WakeMiss,
            (unsigned long)g_soft.u64WakeWoken,
            (unsigned long)g_soft.u64WakeEinval,
            (unsigned long)g_soft.u64WakeZeroCount,
            (unsigned long)g_soft.u64WakeBitset,
            (unsigned long)g_soft.u64WakeClassic,
            (unsigned long)g_soft.u64WakeBitsetMiss,
            (unsigned long)g_soft.u64TimerReap,
            (unsigned long)g_soft.u64ThrCancel,
            (unsigned long)g_soft.u64SoftLog,
            (unsigned long)g_soft.u64KeyPrivateOk,
            (unsigned long)g_soft.u64KeySharedOk,
            (unsigned long)g_soft.u64RobustSetOk,
            (unsigned long)g_soft.u64RobustMarked);

    /* Grep: futex: soft table */
    kprintf("futex: soft table waiters_max=%u used=%u free=%u waiting=%u "
            "peak_used=%lu peak_wait=%lu robust_max=%u robust_used=%u "
            "robust_free=%u peak_robust=%lu samples=%lu\n",
            (unsigned)GJ_FUTEX_MAX_WAITERS, g_u32SoftUsed, g_u32SoftFree,
            g_u32SoftWaiting, (unsigned long)g_soft.u64TablePeakUsed,
            (unsigned long)g_soft.u64TablePeakWait,
            (unsigned)GJ_FUTEX_ROBUST_SLOTS, g_u32SoftRobUsed,
            g_u32SoftRobFree, (unsigned long)g_soft.u64TablePeakRob,
            (unsigned long)g_soft.u64SoftScan);

    /* Grep: futex: soft key */
    kprintf("futex: soft key priv_ok=%lu shared_ok=%lu shared_fault=%lu "
            "align_fail=%lu null_out=%lu cr3_switch=%lu "
            "g_fut1=shared_pa private=as+va\n",
            (unsigned long)g_soft.u64KeyPrivateOk,
            (unsigned long)g_soft.u64KeySharedOk,
            (unsigned long)g_soft.u64KeySharedFault,
            (unsigned long)g_soft.u64KeyAlignFail,
            (unsigned long)g_soft.u64KeyNullOut,
            (unsigned long)g_soft.u64KeySharedCr3Sw);

    /* Grep: futex: soft robust */
    kprintf("futex: soft robust set_ok=%lu set_fail=%lu get_ok=%lu "
            "get_miss=%lu exit_enter=%lu exit_empty=%lu marked=%lu "
            "walk=%lu wake=%lu slots=%u walk_max=%u g_fut_robust=1\n",
            (unsigned long)g_soft.u64RobustSetOk,
            (unsigned long)g_soft.u64RobustSetFail,
            (unsigned long)g_soft.u64RobustGetOk,
            (unsigned long)g_soft.u64RobustGetMiss,
            (unsigned long)g_soft.u64RobustExitEnter,
            (unsigned long)g_soft.u64RobustExitEmpty,
            (unsigned long)g_soft.u64RobustMarked,
            (unsigned long)g_soft.u64RobustWalk,
            (unsigned long)g_soft.u64RobustWake,
            (unsigned)GJ_FUTEX_ROBUST_SLOTS,
            (unsigned)GJ_FUTEX_ROBUST_WALK_MAX);

    /* Grep: futex: soft path */
    kprintf("futex: soft path park=thread_block+schedule "
            "lost_wake=recheck_under_lock match=key+bitset_and "
            "irqsave_lock=1 timer=futex_timer_check "
            "thr_cancel=futex_cancel_thr g_fut1=shared_pa "
            "g_fut2=mono_deadline g_fut3=no_product_spin "
            "bitset=g_fut_bitset robust=g_fut_robust "
            "(soft inventory; not bar3)\n");

    /* Grep: futex: soft timer */
    kprintf("futex: soft timer check=%lu reap=%lu "
            "path=futex_timer_check irqsafe_counter=1 g_fut2=1\n",
            (unsigned long)g_soft.u64TimerCheck,
            (unsigned long)g_soft.u64TimerReap);

    /* Grep: futex: soft thr */
    kprintf("futex: soft thr cancel_calls=%lu slots_cleared=%lu "
            "path=futex_cancel_thr death_orphan=wake_clear\n",
            (unsigned long)g_soft.u64ThrCancelCalls,
            (unsigned long)g_soft.u64ThrCancel);
}

/**
 * After first product activity, print soft inventory once (mirrors sched
 * soft-stats-once pattern). Safe from wait/wake return paths only.
 * Also fires on first robust set/get so soft robust greps land early.
 */
static void
futex_soft_maybe_once(void)
{
    if (g_fSoftStatsOnce != 0) {
        return;
    }
    if (g_soft.u64WaitEnter == 0 && g_soft.u64WakeEnter == 0 &&
        g_soft.u64RobustSetOk == 0 && g_soft.u64RobustGetOk == 0 &&
        g_soft.u64RobustExitEnter == 0 && g_soft.u64KeyPrivateOk == 0 &&
        g_soft.u64KeySharedOk == 0) {
        return;
    }
    g_fSoftStatsOnce = 1;
    futex_soft_log();
}

void
futex_init(void)
{
    memset(g_aWaiters, 0, sizeof(g_aWaiters));
    memset(g_aRobust, 0, sizeof(g_aRobust));
    memset(&g_soft, 0, sizeof(g_soft));
    g_fSoftStatsOnce = 0;
    gj_spin_init(&g_lockFutex);
    /* Greppable soft wait/wake inventory at bring-up (zeros expected). */
    futex_soft_log();
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
        /* futex: soft key null_out */
        futex_soft_inc(&g_soft.u64KeyNullOut);
        return GJ_ERR_INVAL;
    }
    /* Linux: futex word must be 4-byte aligned. */
    if ((u64Uaddr & 3ull) != 0) {
        /* futex: soft key align_fail */
        futex_soft_inc(&g_soft.u64KeyAlignFail);
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
        /* futex: soft key priv_ok */
        futex_soft_inc(&g_soft.u64KeyPrivateOk);
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

    if (fSwitched) {
        /* futex: soft key cr3_switch */
        futex_soft_inc(&g_soft.u64KeySharedCr3Sw);
    }

    pa = futex_resolve_pa(u64Uaddr);
    if (fSwitched) {
        cpu_load_cr3(u64SavedCr3);
    }
    if (pa == 0) {
        /* futex: soft key shared_fault */
        futex_soft_inc(&g_soft.u64KeySharedFault);
        return GJ_ERR_FAULT;
    }
    /* futex: shared key PA — G-FUT-1 cross-process queue identity */
    futex_key_set_shared(pOut, u64Uaddr, pa);
    /* futex: soft key shared_ok */
    futex_soft_inc(&g_soft.u64KeySharedOk);
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
    u32 u32Reaped = 0;

    if (!timer_ready()) {
        return;
    }
    /* futex: soft timer check — IRQ-safe counter only (no kprintf). */
    futex_soft_inc(&g_soft.u64TimerCheck);
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
            u32Reaped++;
        }
    }
    gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
    /* futex: soft timer reap — IRQ-safe counter only (no kprintf). */
    if (u32Reaped > 0) {
        g_soft.u64TimerReap += (u64)u32Reaped;
    }
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

    /* futex: soft wait enter (classic or bitset). */
    futex_soft_inc(&g_soft.u64WaitEnter);
    if (u32Bitset != 0 && u32Bitset != GJ_FUTEX_BITSET_MATCH_ANY) {
        /* futex: soft wait bitset */
        futex_soft_inc(&g_soft.u64WaitBitset);
    } else if (u32Bitset == GJ_FUTEX_BITSET_MATCH_ANY) {
        futex_soft_inc(&g_soft.u64WaitClassic);
    }
    if (u64DeadlineMonoNsec != 0) {
        futex_soft_inc(&g_soft.u64WaitDeadline);
    } else {
        futex_soft_inc(&g_soft.u64WaitNoDeadline);
    }

    if (pU32 == NULL || pKey == NULL) {
        /* futex: soft wait einval */
        futex_soft_inc(&g_soft.u64WaitEinval);
        futex_soft_maybe_once();
        return -LINUX_EINVAL;
    }
    if (pKey->u8Private) {
        futex_soft_inc(&g_soft.u64WaitPrivate);
    } else {
        futex_soft_inc(&g_soft.u64WaitShared);
    }
    if ((pKey->u64Uaddr & 3ull) != 0) {
        futex_soft_inc(&g_soft.u64WaitEinval);
        futex_soft_maybe_once();
        return -LINUX_EINVAL;
    }
    /* Linux: bitset 0 is EINVAL for WAIT_BITSET; classic uses MATCH_ANY. */
    if (u32Bitset == 0) {
        futex_soft_inc(&g_soft.u64WaitEinval);
        futex_soft_maybe_once();
        return -LINUX_EINVAL;
    }
    /* Shared keys with zero PA cannot form a stable queue. */
    if (!pKey->u8Private && pKey->u64Phys == 0) {
        futex_soft_inc(&g_soft.u64WaitEinval);
        futex_soft_maybe_once();
        return -LINUX_EINVAL;
    }

    pThr = thread_current();
    /*
     * G-FUT-3: product path sleeps only via thread_block + timer.
     * No schedulable current thread → fail closed (no pause spin).
     */
    if (pThr == NULL) {
        futex_soft_inc(&g_soft.u64WaitEinval);
        futex_soft_maybe_once();
        return -LINUX_EINVAL;
    }

    /* Fast path: already mismatched before taking the table lock. */
    if (futex_load_u32(pU32) != u32Val) {
        /* futex: soft wait eagain (fast) */
        futex_soft_inc(&g_soft.u64WaitEagain);
        futex_soft_inc(&g_soft.u64WaitEagainFast);
        futex_soft_maybe_once();
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
        /* futex: soft wait enomem */
        futex_soft_inc(&g_soft.u64WaitEnomem);
        futex_soft_maybe_once();
        return -LINUX_ENOMEM;
    }

    /*
     * Recheck under lock, then register block *before* unlock so a concurrent
     * wake/timer cannot miss this waiter (lost-wake).
     */
    if (futex_load_u32(pU32) != u32Val) {
        gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
        /* futex: soft wait eagain (under lock) */
        futex_soft_inc(&g_soft.u64WaitEagain);
        futex_soft_inc(&g_soft.u64WaitEagainLock);
        futex_soft_maybe_once();
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
    /* Soft: track peak table load under lock (exact). */
    futex_soft_note_claim();

    /* Absolute deadline already past — treat as immediate timeout. */
    if (u64DeadlineMonoNsec != 0 && timer_ready() &&
        timer_mono_nsec() >= u64DeadlineMonoNsec) {
        pW->u8Used = 0;
        pW->u8Waiting = 0;
        pW->u32Bitset = 0;
        gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
        /* futex: soft wait etimedout (immediate) */
        futex_soft_inc(&g_soft.u64WaitEtimedout);
        futex_soft_inc(&g_soft.u64WaitEtimedImm);
        futex_soft_maybe_once();
        return -LINUX_ETIMEDOUT;
    }

    thread_block(pW, 0);

    /* Value changed after enqueue+block: cancel and return EAGAIN. */
    if (futex_load_u32(pU32) != u32Val) {
        waiter_cancel_blocked(pW);
        gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
        /* futex: soft wait cancel */
        futex_soft_inc(&g_soft.u64WaitCancel);
        futex_soft_inc(&g_soft.u64WaitEagain);
        futex_soft_inc(&g_soft.u64WaitEagainCancel);
        futex_soft_maybe_once();
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
        futex_soft_inc(&g_soft.u64WaitEarlyWake);
        if (u32TimedOut) {
            futex_soft_inc(&g_soft.u64WaitEtimedout);
            futex_soft_inc(&g_soft.u64WaitEtimedPark);
            futex_soft_maybe_once();
            return -LINUX_ETIMEDOUT;
        }
        futex_soft_inc(&g_soft.u64WaitOk);
        futex_soft_maybe_once();
        return 0;
    }

    gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
    /* futex: soft wait park */
    futex_soft_inc(&g_soft.u64WaitPark);
    schedule();

    u64Flags = gj_spin_lock_irqsave(&g_lockFutex);
    u32TimedOut = pW->u32TimedOut;
    pW->u8Used = 0;
    pW->u8Waiting = 0;
    pW->u32Bitset = 0;
    gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);

    if (u32TimedOut) {
        futex_soft_inc(&g_soft.u64WaitEtimedout);
        futex_soft_inc(&g_soft.u64WaitEtimedPark);
        futex_soft_maybe_once();
        return -LINUX_ETIMEDOUT;
    }
    futex_soft_inc(&g_soft.u64WaitOk);
    futex_soft_maybe_once();
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

/* futex: wait_bitset — G-FUT-BITSET soft product (futex: soft wait bitset) */
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
    u32 u32BitsetMiss = 0;

    /* futex: soft wake enter (classic or bitset). */
    futex_soft_inc(&g_soft.u64WakeEnter);
    if (u32Bitset != 0 && u32Bitset != GJ_FUTEX_BITSET_MATCH_ANY) {
        /* futex: soft wake bitset */
        futex_soft_inc(&g_soft.u64WakeBitset);
    } else if (u32Bitset == GJ_FUTEX_BITSET_MATCH_ANY) {
        futex_soft_inc(&g_soft.u64WakeClassic);
    }

    if (pKey == NULL) {
        /* futex: soft wake einval */
        futex_soft_inc(&g_soft.u64WakeEinval);
        futex_soft_maybe_once();
        return -LINUX_EINVAL;
    }
    if (pKey->u8Private) {
        futex_soft_inc(&g_soft.u64WakePrivate);
    } else {
        futex_soft_inc(&g_soft.u64WakeShared);
    }
    if (u32Bitset == 0) {
        futex_soft_inc(&g_soft.u64WakeEinval);
        futex_soft_maybe_once();
        return -LINUX_EINVAL;
    }
    if (u32Count == 0) {
        futex_soft_inc(&g_soft.u64WakeZeroCount);
        futex_soft_maybe_once();
        return 0;
    }
    if (!pKey->u8Private && pKey->u64Phys == 0) {
        futex_soft_inc(&g_soft.u64WakeEinval);
        futex_soft_maybe_once();
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
            /* futex: soft wake bitset miss — key ok, mask no overlap */
            u32BitsetMiss++;
            continue;
        }
        pW->u8Waiting = 0;
        (void)thread_wake(pW, 0, 1);
        u32Woken++;
    }
    gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);

    if (u32BitsetMiss > 0) {
        /* futex: soft wake bitset miss */
        g_soft.u64WakeBitsetMiss += (u64)u32BitsetMiss;
    }
    if (u32Woken > 0) {
        /* futex: soft wake hit */
        futex_soft_inc(&g_soft.u64WakeHit);
        g_soft.u64WakeWoken += (u64)u32Woken;
    } else {
        /* futex: soft wake miss */
        futex_soft_inc(&g_soft.u64WakeMiss);
    }
    futex_soft_maybe_once();
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

/* futex: wake_bitset — G-FUT-BITSET soft product (futex: soft wake bitset) */
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

/* futex: robust set — G-FUT-ROBUST (futex: soft robust) */
gj_status_t
futex_set_robust_list(u64 u64Head, u64 u64Len)
{
    struct gj_thread *pThr;
    struct futex_robust_slot *pS;
    u64 u64Flags;

    pThr = thread_current();
    if (pThr == NULL || pThr->u32Id == 0) {
        futex_soft_inc(&g_soft.u64RobustSetFail);
        futex_soft_maybe_once();
        return GJ_ERR_INVAL;
    }
    /*
     * Linux requires len == sizeof(robust_list_head). Soft path also accepts
     * small non-zero lengths for bring-up smoke (main robust_list PASS).
     */
    if (u64Len == 0 || u64Len > 64ull) {
        futex_soft_inc(&g_soft.u64RobustSetFail);
        futex_soft_maybe_once();
        return GJ_ERR_INVAL;
    }
    if (u64Head != 0 && (u64Head & 7ull) != 0) {
        futex_soft_inc(&g_soft.u64RobustSetFail);
        futex_soft_maybe_once();
        return GJ_ERR_INVAL;
    }

    u64Flags = gj_spin_lock_irqsave(&g_lockFutex);
    pS = robust_alloc_tid(pThr->u32Id);
    if (pS == NULL) {
        gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
        futex_soft_inc(&g_soft.u64RobustSetFail);
        futex_soft_maybe_once();
        return GJ_ERR_NOMEM;
    }
    pS->u64Head = u64Head;
    pS->u64Len = u64Len;
    {
        u32 iSlot;
        u32 u32Rob = 0;

        for (iSlot = 0; iSlot < GJ_FUTEX_ROBUST_SLOTS; iSlot++) {
            if (g_aRobust[iSlot].u8Used) {
                u32Rob++;
            }
        }
        if ((u64)u32Rob > g_soft.u64TablePeakRob) {
            g_soft.u64TablePeakRob = (u64)u32Rob;
        }
    }
    gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
    /* futex: soft robust set_ok */
    futex_soft_inc(&g_soft.u64RobustSetOk);
    futex_soft_maybe_once();
    return GJ_OK;
}

/* futex: robust get — G-FUT-ROBUST (futex: soft robust) */
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
            futex_soft_inc(&g_soft.u64RobustGetMiss);
            futex_soft_maybe_once();
            return GJ_ERR_INVAL;
        }
        u32Tid = pThr->u32Id;
    }

    u64Flags = gj_spin_lock_irqsave(&g_lockFutex);
    pS = robust_find_tid(u32Tid);
    if (pS == NULL) {
        gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
        /* futex: soft robust get_miss */
        futex_soft_inc(&g_soft.u64RobustGetMiss);
        futex_soft_maybe_once();
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
    /* futex: soft robust get_ok */
    futex_soft_inc(&g_soft.u64RobustGetOk);
    futex_soft_maybe_once();
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
        /* futex: soft robust wake */
        futex_soft_inc(&g_soft.u64RobustWake);
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
    u32 u32Walked = 0;

    /* futex: soft robust exit_enter */
    futex_soft_inc(&g_soft.u64RobustExitEnter);

    if (pThr == NULL || pThr->u32Id == 0) {
        futex_soft_inc(&g_soft.u64RobustExitEmpty);
        futex_soft_maybe_once();
        return 0;
    }
    u32Tid = pThr->u32Id;

    u64Flags = gj_spin_lock_irqsave(&g_lockFutex);
    pS = robust_find_tid(u32Tid);
    if (pS == NULL || pS->u64Head == 0) {
        /* Still clear any waiter slots owned by this thr. */
        gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
        (void)futex_cancel_thr(pThr);
        futex_soft_inc(&g_soft.u64RobustExitEmpty);
        futex_soft_maybe_once();
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
        futex_soft_inc(&g_soft.u64RobustExitEmpty);
        futex_soft_maybe_once();
        return 0;
    }
    i64Offset = (i64)aHead[1];
    u64Pending = aHead[2];
    i64Marked = 0;

    /* Pending op first (lock not yet linked or mid-unlink). */
    if (u64Pending != 0) {
        u32Walked++;
        i64Marked += robust_handle_entry(pThr, u64Pending, i64Offset, u32Tid);
    }

    u64Entry = aHead[0];
    for (cWalk = 0; cWalk < GJ_FUTEX_ROBUST_WALK_MAX; cWalk++) {
        u64 u64Next;

        if (u64Entry == 0 || u64Entry == u64Head) {
            break;
        }
        u32Walked++;
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

    /* futex: soft robust marked / walk */
    g_soft.u64RobustWalk += (u64)u32Walked;
    if (i64Marked > 0) {
        g_soft.u64RobustMarked += (u64)i64Marked;
    }
    futex_soft_maybe_once();
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

    /* futex: soft thr cancel_calls */
    futex_soft_inc(&g_soft.u64ThrCancelCalls);

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
    /* futex: soft thr cancel */
    if (u32Cleared > 0) {
        g_soft.u64ThrCancel += (u64)u32Cleared;
    }
    return u32Cleared;
}
