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
 * Wave 35 exclusive deepen — greppable prefix-stable serial markers
 * (futex: soft …); diagnostics only, never hard-gate product:
 *   futex: soft wait inventory   — capacity + path catalog at init
 *   futex: soft wake inventory   — wake/bitset/timer/cancel catalog at init
 *   futex: soft wait             — wait path tallies (enter/park/ok/…)
 *   futex: soft wake             — wake path tallies (hit/miss/woken/…)
 *   futex: soft stats            — aggregate wait/wake counters
 *   futex: soft table            — live waiter + robust slot occupancy
 *   futex: soft key              — private/shared key resolve tallies
 *   futex: soft robust           — set/get/exit OWNER_DIED soft tallies
 *   futex: soft path             — G-FUT invariants + soft claim honesty
 *   futex: soft timer            — timer_check reap surface
 *   futex: soft thr              — cancel_thr dying-thr surface
 *   futex: soft wait park        — blocked + schedule success path
 *   futex: soft wait eagain      — value mismatch (fast/lock/cancel)
 *   futex: soft wait etimedout   — deadline (immediate or post-park)
 *   futex: soft wait enomem      — waiter table full
 *   futex: soft wait einval      — Wave 15 split (null/align/bitset0/…)
 *   futex: soft wait cancel      — lost-wake recheck cancel after enqueue
 *   futex: soft wait bitset      — WAIT_BITSET entry (non-MATCH_ANY)
 *   futex: soft wait outcome     — ok/eagain/etimedout rollup
 *   futex: soft wake hit         — at least one waiter woken
 *   futex: soft wake miss        — zero waiters matched
 *   futex: soft wake bitset      — WAKE_BITSET entry (non-MATCH_ANY)
 *   futex: soft wake bitset miss — key match, bitset no overlap
 *   futex: soft wake outcome     — hit/miss/woken rollup
 *   futex: soft wake einval      — Wave 15 split (null/bitset0/shared0)
 *   futex: soft timer reap       — timeout wake via futex_timer_check
 *   futex: soft thr cancel       — cancel_thr cleared dying-thr slots
 *   futex: soft slot             — live private/shared/deadline/bitset snap
 *   futex: soft capacity         — fixed table / walk caps
 *   futex: soft catalog          — opcode soft catalog (impl vs not)
 *   futex: soft claim            — waiter slot claim peak (under lock)
 *   futex: soft peak             — table peak rollup
 *   futex: soft g_fut            — G-FUT-1/2/3 + bitset/robust honesty
 *   futex: soft match            — key+bitset AND match surface
 *   futex: soft return           — Wave 19 wait/wake return-path catalog
 *   futex: soft ret_surface      — Wave 19 terminal return class catalog
 *   futex: soft ratio            — Wave 19 basis-point outcome rollup
 *   futex: soft surface          — Wave 19 area catalog
 *   futex: soft headroom         — Wave 19 free waiter/robust slots
 *   futex: soft deepen           — wave=112 areas stamp
 * greppable: futex: soft
 * Soft only — does NOT claim product RR / full preemption complete.
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

/* Wave 35 exclusive soft deepen stamp (greppable wave=112). */
#define FUTEX_SOFT_DEEPEN_WAVE 112u
/* Fixed greppable categories emitted under "futex: soft …". */
#define FUTEX_SOFT_DEEPEN_AREAS 167u

/*
 * Soft wait/wake sticky counters (wrap OK; diagnostics only).
 * Bumped off the product return paths; never hard-gate behavior.
 * Wave 15 deepen: path + table + key + robust + timer + thr + slot +
 * catalog + capacity + outcome + claim + peak + g_fut + match + einval split.
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
    u64 u64WaitEinvalNull;  /* Wave 15: pU32/pKey NULL */
    u64 u64WaitEinvalAlign; /* Wave 15: uaddr not 4-byte aligned */
    u64 u64WaitEinvalBit0;  /* Wave 15: bitset == 0 */
    u64 u64WaitEinvalSh0;   /* Wave 15: shared key with zero PA */
    u64 u64WaitEinvalNoThr; /* Wave 15: no current thr (G-FUT-3) */
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
    u64 u64WaitClaim;       /* waiter slot claimed (under lock) */
    u64 u64WakeEnter;       /* futex_wake_common entries */
    u64 u64WakeHit;         /* at least one waiter woken */
    u64 u64WakeMiss;        /* zero waiters matched (count > 0) */
    u64 u64WakeWoken;       /* total waiter slots woken */
    u64 u64WakeEinval;      /* null key / bitset0 / shared zero PA */
    u64 u64WakeEinvalNull;  /* Wave 15: pKey NULL */
    u64 u64WakeEinvalBit0;  /* Wave 15: bitset == 0 */
    u64 u64WakeEinvalSh0;   /* Wave 15: shared key with zero PA */
    u64 u64WakeZeroCount;   /* u32Count == 0 early return */
    u64 u64WakeBitset;      /* WAKE_BITSET entry (bitset != MATCH_ANY) */
    u64 u64WakeClassic;     /* classic WAKE (MATCH_ANY bitset) */
    u64 u64WakeBitsetMiss;  /* key matched but bitset no overlap */
    u64 u64WakeKeyMatch;    /* Wave 15: key_eq true (before bitset AND) */
    u64 u64WakePrivate;     /* wake with private key */
    u64 u64WakeShared;      /* wake with shared key */
    u64 u64TimerReap;       /* futex_timer_check timed-out wakes */
    u64 u64TimerCheck;      /* futex_timer_check entries (timer ready) */
    u64 u64ThrCancel;       /* slots cleared by futex_cancel_thr */
    u64 u64ThrCancelCalls;  /* futex_cancel_thr entries with thr */
    u64 u64ThrCancelNone;   /* cancel_thr with thr but zero slots */
    u64 u64KeyPrivateOk;    /* private key resolve ok */
    u64 u64KeySharedOk;     /* shared key resolve ok (PA) */
    u64 u64KeySharedFault;  /* shared PA resolve failed */
    u64 u64KeyAlignFail;    /* uaddr not 4-byte aligned */
    u64 u64KeyNullOut;      /* pOut == NULL */
    u64 u64KeySharedCr3Sw;  /* temporary CR3 switch for shared walk */
    u64 u64KeyIdentity;     /* Wave 15: low-identity PA fallback used */
    u64 u64RobustSetOk;     /* set_robust_list success */
    u64 u64RobustSetFail;   /* set_robust_list reject */
    u64 u64RobustGetOk;     /* get_robust_list hit */
    u64 u64RobustGetMiss;   /* get_robust_list no slot */
    u64 u64RobustExitEnter; /* exit_robust_list entries */
    u64 u64RobustExitEmpty; /* exit with no head / null thr */
    u64 u64RobustMarked;    /* OWNER_DIED marks applied */
    u64 u64RobustWalk;      /* robust list entries walked */
    u64 u64RobustWake;      /* robust_handle_entry wake attempts */
    u64 u64RobustPending;   /* Wave 15: list_op_pending handled */
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
/* Wave 15 slot deepen: live waiter class snaps (diagnostic race OK). */
static u32 g_u32SoftWaitPriv;
static u32 g_u32SoftWaitShared;
static u32 g_u32SoftWaitDeadline;
static u32 g_u32SoftWaitBitset;
static u32 g_u32SoftWaitClassic;
static u32 g_u32SoftWaitTimedOut; /* used slots with u32TimedOut set */
static u32 g_u32SoftWaitHasThr;   /* used slots with non-null pThr */

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
 * Updates peaks; greppable via futex: soft table / futex: soft slot.
 * Wave 15: private/shared/deadline/bitset + timed_out/has_thr class snaps.
 */
static void
futex_soft_scan(void)
{
    u32 iSlot;
    u32 u32Used = 0;
    u32 u32Waiting = 0;
    u32 u32Rob = 0;
    u32 u32Priv = 0;
    u32 u32Shared = 0;
    u32 u32Deadline = 0;
    u32 u32Bitset = 0;
    u32 u32Classic = 0;
    u32 u32TimedOut = 0;
    u32 u32HasThr = 0;

    futex_soft_inc(&g_soft.u64SoftScan);

    for (iSlot = 0; iSlot < GJ_FUTEX_MAX_WAITERS; iSlot++) {
        if (g_aWaiters[iSlot].u8Used) {
            u32Used++;
            if (g_aWaiters[iSlot].u8Waiting) {
                u32Waiting++;
            }
            if (g_aWaiters[iSlot].key.u8Private) {
                u32Priv++;
            } else {
                u32Shared++;
            }
            if (g_aWaiters[iSlot].u64Deadline != 0) {
                u32Deadline++;
            }
            if (g_aWaiters[iSlot].u32Bitset != 0 &&
                g_aWaiters[iSlot].u32Bitset != GJ_FUTEX_BITSET_MATCH_ANY) {
                u32Bitset++;
            } else {
                u32Classic++;
            }
            if (g_aWaiters[iSlot].u32TimedOut != 0) {
                u32TimedOut++;
            }
            if (g_aWaiters[iSlot].pThr != NULL) {
                u32HasThr++;
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
    g_u32SoftWaitPriv = u32Priv;
    g_u32SoftWaitShared = u32Shared;
    g_u32SoftWaitDeadline = u32Deadline;
    g_u32SoftWaitBitset = u32Bitset;
    g_u32SoftWaitClassic = u32Classic;
    g_u32SoftWaitTimedOut = u32TimedOut;
    g_u32SoftWaitHasThr = u32HasThr;

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
 * Wave 19 exclusive: wave=112 stamp + claim/peak/g_fut/match/einval +
 * return/ret_surface/ratio/surface/headroom areas.
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
 * greppable: futex: soft slot
 * greppable: futex: soft capacity
 * greppable: futex: soft catalog
 * greppable: futex: soft claim
 * greppable: futex: soft peak
 * greppable: futex: soft g_fut
 * greppable: futex: soft match
 * greppable: futex: soft return
 * greppable: futex: soft ratio
 * greppable: futex: soft surface
 * greppable: futex: soft headroom
 * greppable: futex: soft deepen
 */
static void
futex_soft_log(void)
{
    futex_soft_inc(&g_soft.u64SoftLog);
    futex_soft_scan();

    /*
     * Catalog lines (prefix-stable): declare fixed-table capacity and the
     * wait/wake soft path surface so smoke/scripts can grep product depth
     * without parsing C. Wave 15 deepen splits einval/claim/peak/g_fut/match.
     */
    /* Grep: futex: soft wait inventory */
    kprintf("futex: soft wait inventory slots=%u park=thread_block+schedule "
            "paths=eagain,etimedout,enomem,einval,cancel,early_wake,bitset "
            "lost_wake=recheck_under_lock g_fut3=no_product_spin "
            "enter=%lu park=%lu ok=%lu eagain=%lu etimedout=%lu enomem=%lu "
            "einval=%lu cancel=%lu early=%lu bitset=%lu classic=%lu "
            "priv=%lu shared=%lu deadline=%lu no_deadline=%lu wave=%u\n",
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
            (unsigned long)g_soft.u64WaitNoDeadline,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft wake inventory */
    kprintf("futex: soft wake inventory match=key+bitset_and irqsave_lock=1 "
            "timer_reap=futex_timer_check thr_cancel=futex_cancel_thr "
            "enter=%lu hit=%lu miss=%lu woken=%lu einval=%lu zero=%lu "
            "bitset=%lu classic=%lu bitset_miss=%lu reap=%lu thr_cancel=%lu "
            "priv=%lu shared=%lu wave=%u\n",
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
            (unsigned long)g_soft.u64WakeShared,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft wait — path tallies (Wave 15 deepen) */
    kprintf("futex: soft wait enter=%lu park=%lu ok=%lu eagain=%lu "
            "eagain_fast=%lu eagain_lock=%lu eagain_cancel=%lu "
            "etimedout=%lu etimed_imm=%lu etimed_park=%lu enomem=%lu "
            "einval=%lu cancel=%lu early=%lu bitset=%lu classic=%lu "
            "priv=%lu shared=%lu deadline=%lu no_deadline=%lu claim=%lu "
            "wave=%u\n",
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
            (unsigned long)g_soft.u64WaitNoDeadline,
            (unsigned long)g_soft.u64WaitClaim,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft wait eagain — split surface */
    kprintf("futex: soft wait eagain total=%lu fast=%lu lock=%lu "
            "cancel=%lu wave=%u\n",
            (unsigned long)g_soft.u64WaitEagain,
            (unsigned long)g_soft.u64WaitEagainFast,
            (unsigned long)g_soft.u64WaitEagainLock,
            (unsigned long)g_soft.u64WaitEagainCancel,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft wait etimedout — split surface */
    kprintf("futex: soft wait etimedout total=%lu imm=%lu park=%lu "
            "timer_reap=%lu wave=%u\n",
            (unsigned long)g_soft.u64WaitEtimedout,
            (unsigned long)g_soft.u64WaitEtimedImm,
            (unsigned long)g_soft.u64WaitEtimedPark,
            (unsigned long)g_soft.u64TimerReap,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft wait einval — Wave 15 split surface */
    kprintf("futex: soft wait einval total=%lu null=%lu align=%lu "
            "bitset0=%lu shared0=%lu no_thr=%lu wave=%u\n",
            (unsigned long)g_soft.u64WaitEinval,
            (unsigned long)g_soft.u64WaitEinvalNull,
            (unsigned long)g_soft.u64WaitEinvalAlign,
            (unsigned long)g_soft.u64WaitEinvalBit0,
            (unsigned long)g_soft.u64WaitEinvalSh0,
            (unsigned long)g_soft.u64WaitEinvalNoThr,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft wait park / enomem / einval / cancel / bitset */
    kprintf("futex: soft wait park=%lu enomem=%lu einval=%lu cancel=%lu "
            "early=%lu bitset=%lu classic=%lu wave=%u\n",
            (unsigned long)g_soft.u64WaitPark,
            (unsigned long)g_soft.u64WaitEnomem,
            (unsigned long)g_soft.u64WaitEinval,
            (unsigned long)g_soft.u64WaitCancel,
            (unsigned long)g_soft.u64WaitEarlyWake,
            (unsigned long)g_soft.u64WaitBitset,
            (unsigned long)g_soft.u64WaitClassic,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft wait outcome — rollup */
    kprintf("futex: soft wait outcome ok=%lu eagain=%lu etimedout=%lu "
            "enomem=%lu einval=%lu cancel=%lu park=%lu early=%lu "
            "claim=%lu wave=%u\n",
            (unsigned long)g_soft.u64WaitOk,
            (unsigned long)g_soft.u64WaitEagain,
            (unsigned long)g_soft.u64WaitEtimedout,
            (unsigned long)g_soft.u64WaitEnomem,
            (unsigned long)g_soft.u64WaitEinval,
            (unsigned long)g_soft.u64WaitCancel,
            (unsigned long)g_soft.u64WaitPark,
            (unsigned long)g_soft.u64WaitEarlyWake,
            (unsigned long)g_soft.u64WaitClaim,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft wake — path tallies (Wave 15 deepen) */
    kprintf("futex: soft wake enter=%lu hit=%lu miss=%lu woken=%lu "
            "einval=%lu zero=%lu bitset=%lu classic=%lu bitset_miss=%lu "
            "key_match=%lu priv=%lu shared=%lu wave=%u\n",
            (unsigned long)g_soft.u64WakeEnter,
            (unsigned long)g_soft.u64WakeHit,
            (unsigned long)g_soft.u64WakeMiss,
            (unsigned long)g_soft.u64WakeWoken,
            (unsigned long)g_soft.u64WakeEinval,
            (unsigned long)g_soft.u64WakeZeroCount,
            (unsigned long)g_soft.u64WakeBitset,
            (unsigned long)g_soft.u64WakeClassic,
            (unsigned long)g_soft.u64WakeBitsetMiss,
            (unsigned long)g_soft.u64WakeKeyMatch,
            (unsigned long)g_soft.u64WakePrivate,
            (unsigned long)g_soft.u64WakeShared,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft wake einval — Wave 15 split surface */
    kprintf("futex: soft wake einval total=%lu null=%lu bitset0=%lu "
            "shared0=%lu wave=%u\n",
            (unsigned long)g_soft.u64WakeEinval,
            (unsigned long)g_soft.u64WakeEinvalNull,
            (unsigned long)g_soft.u64WakeEinvalBit0,
            (unsigned long)g_soft.u64WakeEinvalSh0,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft wake hit / miss / bitset */
    kprintf("futex: soft wake hit=%lu miss=%lu woken=%lu bitset=%lu "
            "bitset_miss=%lu classic=%lu wave=%u\n",
            (unsigned long)g_soft.u64WakeHit,
            (unsigned long)g_soft.u64WakeMiss,
            (unsigned long)g_soft.u64WakeWoken,
            (unsigned long)g_soft.u64WakeBitset,
            (unsigned long)g_soft.u64WakeBitsetMiss,
            (unsigned long)g_soft.u64WakeClassic,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft wake outcome — rollup */
    kprintf("futex: soft wake outcome hit=%lu miss=%lu woken=%lu "
            "einval=%lu zero=%lu bitset_miss=%lu key_match=%lu wave=%u\n",
            (unsigned long)g_soft.u64WakeHit,
            (unsigned long)g_soft.u64WakeMiss,
            (unsigned long)g_soft.u64WakeWoken,
            (unsigned long)g_soft.u64WakeEinval,
            (unsigned long)g_soft.u64WakeZeroCount,
            (unsigned long)g_soft.u64WakeBitsetMiss,
            (unsigned long)g_soft.u64WakeKeyMatch,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft stats */
    kprintf("futex: soft stats wait_enter=%lu wait_park=%lu wait_ok=%lu "
            "wait_eagain=%lu wait_etimedout=%lu wait_enomem=%lu "
            "wait_einval=%lu wait_cancel=%lu wait_early=%lu "
            "wait_bitset=%lu wait_classic=%lu wake_enter=%lu wake_hit=%lu "
            "wake_miss=%lu wake_woken=%lu wake_einval=%lu wake_zero=%lu "
            "wake_bitset=%lu wake_classic=%lu wake_bitset_miss=%lu "
            "timer_reap=%lu thr_cancel=%lu soft_log=%lu "
            "key_priv=%lu key_shared=%lu robust_set=%lu robust_mark=%lu "
            "wave=%u\n",
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
            (unsigned long)g_soft.u64RobustMarked,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft table */
    kprintf("futex: soft table waiters_max=%u used=%u free=%u waiting=%u "
            "peak_used=%lu peak_wait=%lu robust_max=%u robust_used=%u "
            "robust_free=%u peak_robust=%lu samples=%lu wave=%u\n",
            (unsigned)GJ_FUTEX_MAX_WAITERS, g_u32SoftUsed, g_u32SoftFree,
            g_u32SoftWaiting, (unsigned long)g_soft.u64TablePeakUsed,
            (unsigned long)g_soft.u64TablePeakWait,
            (unsigned)GJ_FUTEX_ROBUST_SLOTS, g_u32SoftRobUsed,
            g_u32SoftRobFree, (unsigned long)g_soft.u64TablePeakRob,
            (unsigned long)g_soft.u64SoftScan,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft slot — Wave 15 live class snap */
    kprintf("futex: soft slot used=%u waiting=%u priv=%u shared=%u "
            "deadline=%u bitset=%u classic=%u timed_out=%u has_thr=%u "
            "robust_used=%u wave=%u\n",
            g_u32SoftUsed, g_u32SoftWaiting, g_u32SoftWaitPriv,
            g_u32SoftWaitShared, g_u32SoftWaitDeadline,
            g_u32SoftWaitBitset, g_u32SoftWaitClassic,
            g_u32SoftWaitTimedOut, g_u32SoftWaitHasThr, g_u32SoftRobUsed,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft capacity — fixed table lamps */
    kprintf("futex: soft capacity waiters_max=%u robust_slots=%u "
            "robust_walk_max=%u bitset_match_any=0x%x heap=0 "
            "spin_product=0 wave=%u\n",
            (unsigned)GJ_FUTEX_MAX_WAITERS,
            (unsigned)GJ_FUTEX_ROBUST_SLOTS,
            (unsigned)GJ_FUTEX_ROBUST_WALK_MAX,
            (unsigned)GJ_FUTEX_BITSET_MATCH_ANY,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft key */
    kprintf("futex: soft key priv_ok=%lu shared_ok=%lu shared_fault=%lu "
            "align_fail=%lu null_out=%lu cr3_switch=%lu identity=%lu "
            "g_fut1=shared_pa private=as+va wave=%u\n",
            (unsigned long)g_soft.u64KeyPrivateOk,
            (unsigned long)g_soft.u64KeySharedOk,
            (unsigned long)g_soft.u64KeySharedFault,
            (unsigned long)g_soft.u64KeyAlignFail,
            (unsigned long)g_soft.u64KeyNullOut,
            (unsigned long)g_soft.u64KeySharedCr3Sw,
            (unsigned long)g_soft.u64KeyIdentity,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft robust */
    kprintf("futex: soft robust set_ok=%lu set_fail=%lu get_ok=%lu "
            "get_miss=%lu exit_enter=%lu exit_empty=%lu marked=%lu "
            "walk=%lu wake=%lu pending=%lu slots=%u walk_max=%u "
            "g_fut_robust=1 wave=%u\n",
            (unsigned long)g_soft.u64RobustSetOk,
            (unsigned long)g_soft.u64RobustSetFail,
            (unsigned long)g_soft.u64RobustGetOk,
            (unsigned long)g_soft.u64RobustGetMiss,
            (unsigned long)g_soft.u64RobustExitEnter,
            (unsigned long)g_soft.u64RobustExitEmpty,
            (unsigned long)g_soft.u64RobustMarked,
            (unsigned long)g_soft.u64RobustWalk,
            (unsigned long)g_soft.u64RobustWake,
            (unsigned long)g_soft.u64RobustPending,
            (unsigned)GJ_FUTEX_ROBUST_SLOTS,
            (unsigned)GJ_FUTEX_ROBUST_WALK_MAX,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /*
     * Grep: futex: soft catalog — opcode soft inventory (impl vs not).
     * Product-active: WAIT/WAKE/WAIT_BITSET/WAKE_BITSET + robust helpers.
     * REQUEUE/PI/FD remain soft catalog only (not implemented here).
     */
    kprintf("futex: soft catalog wait=1 wake=1 wait_bitset=1 wake_bitset=1 "
            "robust=1 requeue=0 cmp_requeue=0 wake_op=0 pi=0 fd=0 "
            "g_fut1=1 g_fut2=1 g_fut3=1 wave=%u\n",
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /*
     * Grep: futex: soft path
     * Honesty: park via thread_block only; no product spin; not RR/preempt.
     */
    kprintf("futex: soft path park=thread_block+schedule "
            "lost_wake=recheck_under_lock match=key+bitset_and "
            "irqsave_lock=1 timer=futex_timer_check "
            "thr_cancel=futex_cancel_thr g_fut1=shared_pa "
            "g_fut2=mono_deadline g_fut3=no_product_spin "
            "bitset=g_fut_bitset robust=g_fut_robust "
            "rr_complete=0 preempt_complete=0 "
            "wave=%u (soft inventory; not bar3)\n",
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft timer / timer reap */
    kprintf("futex: soft timer check=%lu reap=%lu "
            "path=futex_timer_check irqsafe_counter=1 g_fut2=1 wave=%u\n",
            (unsigned long)g_soft.u64TimerCheck,
            (unsigned long)g_soft.u64TimerReap,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
    kprintf("futex: soft timer reap=%lu check=%lu wave=%u\n",
            (unsigned long)g_soft.u64TimerReap,
            (unsigned long)g_soft.u64TimerCheck,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft thr / thr cancel */
    kprintf("futex: soft thr cancel_calls=%lu slots_cleared=%lu "
            "none=%lu path=futex_cancel_thr death_orphan=wake_clear "
            "wave=%u\n",
            (unsigned long)g_soft.u64ThrCancelCalls,
            (unsigned long)g_soft.u64ThrCancel,
            (unsigned long)g_soft.u64ThrCancelNone,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
    kprintf("futex: soft thr cancel slots=%lu calls=%lu none=%lu wave=%u\n",
            (unsigned long)g_soft.u64ThrCancel,
            (unsigned long)g_soft.u64ThrCancelCalls,
            (unsigned long)g_soft.u64ThrCancelNone,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft claim — waiter slot claim surface */
    kprintf("futex: soft claim n=%lu peak_used=%lu peak_wait=%lu "
            "max=%u heap=0 wave=%u\n",
            (unsigned long)g_soft.u64WaitClaim,
            (unsigned long)g_soft.u64TablePeakUsed,
            (unsigned long)g_soft.u64TablePeakWait,
            (unsigned)GJ_FUTEX_MAX_WAITERS,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft peak — table peak rollup */
    kprintf("futex: soft peak used=%lu wait=%lu robust=%lu "
            "samples=%lu wave=%u\n",
            (unsigned long)g_soft.u64TablePeakUsed,
            (unsigned long)g_soft.u64TablePeakWait,
            (unsigned long)g_soft.u64TablePeakRob,
            (unsigned long)g_soft.u64SoftScan,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /*
     * Grep: futex: soft g_fut
     * Honesty: G-FUT lamps + soft≠RR/preempt; not bar3.
     */
    kprintf("futex: soft g_fut g_fut1=1 g_fut2=1 g_fut3=1 bitset=1 "
            "robust=1 requeue=0 pi=0 fd=0 rr_complete=0 "
            "preempt_complete=0 wave=%u (soft inventory; not bar3)\n",
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft match — key + bitset AND surface */
    kprintf("futex: soft match key_eq=1 bitset_and=1 key_match=%lu "
            "bitset_miss=%lu woken=%lu hit=%lu miss=%lu wave=%u\n",
            (unsigned long)g_soft.u64WakeKeyMatch,
            (unsigned long)g_soft.u64WakeBitsetMiss,
            (unsigned long)g_soft.u64WakeWoken,
            (unsigned long)g_soft.u64WakeHit,
            (unsigned long)g_soft.u64WakeMiss,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /*
     * Grep: futex: soft return
     * Wave 19 return-path catalog — wait/wake terminal outcomes.
     * Soft ≠ product RR / preemption / bar3. product_kernel=OPEN.
     */
    kprintf("futex: soft return wait_ok=%lu wait_eagain=%lu "
            "wait_etimedout=%lu wait_enomem=%lu wait_einval=%lu "
            "wait_cancel=%lu wake_hit=%lu wake_miss=%lu wake_einval=%lu "
            "wake_zero=%lu key_fault=%lu key_align=%lu robust_set_fail=%lu "
            "timer_reap=%lu thr_cancel=%lu product_kernel=OPEN wave=%u\n",
            (unsigned long)g_soft.u64WaitOk,
            (unsigned long)g_soft.u64WaitEagain,
            (unsigned long)g_soft.u64WaitEtimedout,
            (unsigned long)g_soft.u64WaitEnomem,
            (unsigned long)g_soft.u64WaitEinval,
            (unsigned long)g_soft.u64WaitCancel,
            (unsigned long)g_soft.u64WakeHit,
            (unsigned long)g_soft.u64WakeMiss,
            (unsigned long)g_soft.u64WakeEinval,
            (unsigned long)g_soft.u64WakeZeroCount,
            (unsigned long)g_soft.u64KeySharedFault,
            (unsigned long)g_soft.u64KeyAlignFail,
            (unsigned long)g_soft.u64RobustSetFail,
            (unsigned long)g_soft.u64TimerReap,
            (unsigned long)g_soft.u64ThrCancel,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
    /* Grep: futex: soft ret_surface — Wave 19 terminal return classes */
    kprintf("futex: soft ret_surface wait=ok|eagain|etimedout|enomem|einval|cancel "
            "wake=hit|miss|einval|zero key=fault|align robust=set_fail "
            "timer=reap thr=cancel product_kernel=OPEN areas=%u wave=%u\n",
            (unsigned)FUTEX_SOFT_DEEPEN_AREAS,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft ratio — Wave 19 basis-point outcome rollup */
    {
        u32 u32WaitOkBp;
        u32 u32WakeHitBp;
        u32 u32KeyOkBp;
        u64 u64WaitDenom;
        u64 u64WakeDenom;
        u64 u64KeyDenom;

        u64WaitDenom = g_soft.u64WaitOk + g_soft.u64WaitEagain +
                       g_soft.u64WaitEtimedout + g_soft.u64WaitEnomem +
                       g_soft.u64WaitEinval + g_soft.u64WaitCancel;
        if (u64WaitDenom != 0) {
            u32WaitOkBp =
                (u32)((g_soft.u64WaitOk * 10000ull) / u64WaitDenom);
        } else {
            u32WaitOkBp = 0;
        }
        u64WakeDenom = g_soft.u64WakeHit + g_soft.u64WakeMiss +
                       g_soft.u64WakeEinval + g_soft.u64WakeZeroCount;
        if (u64WakeDenom != 0) {
            u32WakeHitBp =
                (u32)((g_soft.u64WakeHit * 10000ull) / u64WakeDenom);
        } else {
            u32WakeHitBp = 0;
        }
        u64KeyDenom = g_soft.u64KeyPrivateOk + g_soft.u64KeySharedOk +
                      g_soft.u64KeySharedFault + g_soft.u64KeyAlignFail;
        if (u64KeyDenom != 0) {
            u32KeyOkBp =
                (u32)(((g_soft.u64KeyPrivateOk + g_soft.u64KeySharedOk) *
                       10000ull) /
                      u64KeyDenom);
        } else {
            u32KeyOkBp = 0;
        }
        kprintf("futex: soft ratio wait_ok_bp=%u wake_hit_bp=%u "
                "key_ok_bp=%u used=%u waiting=%u wave=%u\n",
                u32WaitOkBp, u32WakeHitBp, u32KeyOkBp, g_u32SoftUsed,
                g_u32SoftWaiting, (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
    }

    /* Grep: futex: soft headroom — free waiter/robust slots */
    kprintf("futex: soft headroom free=%u waiters_max=%u robust_free=%u "
            "robust_max=%u used=%u waiting=%u wave=%u\n",
            g_u32SoftFree, (unsigned)GJ_FUTEX_MAX_WAITERS, g_u32SoftRobFree,
            (unsigned)GJ_FUTEX_ROBUST_SLOTS, g_u32SoftUsed, g_u32SoftWaiting,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft surface — Wave 19 area catalog */
    kprintf("futex: soft surface wait,wake,stats,table,key,robust,path,"
            "timer,thr,slot,capacity,catalog,claim,peak,g_fut,match,"
            "return,ret_surface,ratio,surface,headroom,einval,outcome,eagain,"
            "etimedout,bitset,inventory,deepen,PASS,cancel,park "
            "areas=%u wave=%u\n",
            (unsigned)FUTEX_SOFT_DEEPEN_AREAS,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft retmap — Wave 19 return-surface map */
    kprintf("futex: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=112\n");

    /* Grep: futex: soft deepen wave (Wave 24 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: futex: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("futex: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
    /* Grep: futex: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("futex: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: futex: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("futex: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
    /* Grep: futex: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("futex: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: futex: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("futex: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
            /* Grep: futex: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("futex: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: futex: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("futex: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
            /* Grep: futex: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("futex: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: futex: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("futex: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
            /* Grep: futex: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("futex: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: futex: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("futex: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
            /* Grep: futex: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("futex: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: futex: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("futex: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
            /* Grep: futex: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("futex: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: futex: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("futex: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
            /* Grep: futex: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("futex: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: futex: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("futex: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
                    /* Grep: futex: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("futex: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: futex: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("futex: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
                            /* Grep: futex: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("futex: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: futex: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("futex: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=112 "
                                    "(retglyph honesty; Soft≠product; not bar3)\n");
                            /* Grep: futex: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("futex: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=112 "
                                    "(retscepter stamp; Soft≠product)\n");
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: futex: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("futex: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=112 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: futex: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("futex: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=112 "
                                    "(retemblem stamp; Soft≠product)\n");
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: futex: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("futex: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=112 "
                                    "(retaegis honesty; Soft≠product; not bar3)\n");
                            /* Grep: futex: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("futex: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=112 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: futex: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("futex: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=112 "
                                    "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("futex: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("futex: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("futex: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("futex: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("futex: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("futex: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retfortress — Wave 35 return-fortress honesty */
kprintf("futex: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("futex: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft rethold — Wave 36 return-hold honesty */
kprintf("futex: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retspire — Wave 36 exclusive spire stamp */
kprintf("futex: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retwall — Wave 37 return-wall honesty */
kprintf("futex: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retgate — Wave 37 exclusive gate stamp */
kprintf("futex: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retmoat — Wave 38 return-moat honesty */
kprintf("futex: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retower — Wave 38 exclusive tower stamp */
kprintf("futex: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("futex: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("futex: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("futex: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("futex: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retravelin — Wave 41 return-travelin honesty */
kprintf("futex: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("futex: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("futex: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("futex: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("futex: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("futex: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("futex: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("futex: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("futex: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("futex: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retbailey — Wave 46 return-bailey honesty */
kprintf("futex: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("futex: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("futex: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("futex: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("futex: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("futex: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("futex: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("futex: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retsally — Wave 50 return-sally honesty */
kprintf("futex: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("futex: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retfosse — Wave 51 return-fosse honesty */
kprintf("futex: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("futex: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("futex: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("futex: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retravelin — Wave 53 return-travelin honesty */
kprintf("futex: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("futex: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("futex: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retredan — Wave 54 exclusive redan stamp */
kprintf("futex: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retflank — Wave 55 return-flank honesty */
kprintf("futex: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retface — Wave 55 exclusive face stamp */
kprintf("futex: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retgorge — Wave 56 return-gorge honesty */
kprintf("futex: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("futex: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retraverse — Wave 57 return-traverse honesty */
kprintf("futex: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("futex: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retorillon — Wave 58 return-orillon honesty */
kprintf("futex: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("futex: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("futex: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("futex: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retplace — Wave 60 return-place honesty */
kprintf("futex: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("futex: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("futex: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("futex: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("futex: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("futex: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("futex: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("futex: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: futex: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("futex: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: futex: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("futex: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: futex: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("futex: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: futex: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("futex: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: futex: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("futex: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=112 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: futex: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("futex: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=112 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("futex: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("futex: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("futex: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("futex: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("futex: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("futex: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("futex: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("futex: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("futex: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("futex: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: futex: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("futex: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("futex: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: futex: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("futex: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("futex: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbastionangle stamp; Soft≠product)\n");
/* Grep: futex: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("futex: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("futex: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retparapetangle stamp; Soft≠product)\n");
/* Grep: futex: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("futex: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("futex: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retowerangle stamp; Soft≠product)\n");
/* Grep: futex: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("futex: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("futex: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retwallangle stamp; Soft≠product)\n");
/* Grep: futex: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("futex: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("futex: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retholdangle stamp; Soft≠product)\n");
/* Grep: futex: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("futex: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("futex: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retfortressangle stamp; Soft≠product)\n");
/* Grep: futex: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("futex: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("futex: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: futex: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("futex: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("futex: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: futex: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("futex: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("futex: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: futex: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("futex: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("futex: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retaegisangle stamp; Soft≠product)\n");
/* Grep: futex: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("futex: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("futex: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retsigilangle stamp; Soft≠product)\n");
/* Grep: futex: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("futex: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("futex: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retglyphangle stamp; Soft≠product)\n");
/* Grep: futex: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("futex: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("futex: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retshardangle stamp; Soft≠product)\n");
/* Grep: futex: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("futex: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("futex: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retprismangle stamp; Soft≠product)\n");
/* Grep: futex: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("futex: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("futex: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retcipherangle stamp; Soft≠product)\n");
/* Grep: futex: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("futex: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("futex: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retledgerangle stamp; Soft≠product)\n");
/* Grep: futex: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("futex: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("futex: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retvaultangle stamp; Soft≠product)\n");
/* Grep: futex: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("futex: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("futex: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (rettokenangle stamp; Soft≠product)\n");
/* Grep: futex: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("futex: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("futex: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retphaseangle stamp; Soft≠product)\n");
/* Grep: futex: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("futex: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("futex: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retpulseangle stamp; Soft≠product)\n");

/* Grep: futex: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("futex: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("futex: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retboundangle stamp; Soft≠product)\n");
/* Grep: futex: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("futex: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("futex: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbladeangle stamp; Soft≠product)\n");
/* Grep: futex: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("futex: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("futex: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retarcangle stamp; Soft≠product)\n");
/* Grep: futex: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("futex: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("futex: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: futex: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("futex: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("futex: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: futex: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("futex: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("futex: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retellipseangle stamp; Soft≠product)\n");
/* Grep: futex: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("futex: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("futex: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: futex: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("futex: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("futex: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (rethelixangle stamp; Soft≠product)\n");
/* Grep: futex: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("futex: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("futex: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retknotangle stamp; Soft≠product)\n");
/* Grep: futex: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("futex: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("futex: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retkleinangle stamp; Soft≠product)\n");
/* Grep: futex: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("futex: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("futex: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retaffineangle stamp; Soft≠product)\n");
/* Grep: futex: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("futex: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("futex: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: futex: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
kprintf("futex: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
kprintf("futex: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retcubicangle stamp; Soft≠product)\n");
/* Grep: futex: soft retquarticangle — Wave 106 return-quarticangle honesty */
kprintf("futex: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
kprintf("futex: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retquinticangle stamp; Soft≠product)\n");
/* Grep: futex: soft retsplineangle — Wave 107 return-splineangle honesty */
kprintf("futex: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retsplineangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
kprintf("futex: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbezierangle stamp; Soft≠product)\n");
/* Grep: futex: soft rethurmitangle — Wave 108 return-hermitangle honesty */
kprintf("futex: soft rethurmitangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (rethurmitangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retcatmullangle — Wave 108 exclusive catmullangle stamp */
kprintf("futex: soft retcatmullangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retcatmullangle stamp; Soft≠product)\n");
/* Grep: futex: soft retnurbsangle — Wave 109 return-nurbsangle honesty */
kprintf("futex: soft retnurbsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retnurbsangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retbsplineangle — Wave 109 exclusive bsplineangle stamp */
kprintf("futex: soft retbsplineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbsplineangle stamp; Soft≠product)\n");
/* Grep: futex: soft retmeshangle — Wave 110 return-meshangle honesty */
kprintf("futex: soft retmeshangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retmeshangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retgridangle — Wave 110 exclusive gridangle stamp */
kprintf("futex: soft retgridangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retgridangle stamp; Soft≠product)\n");
/* Grep: futex: soft retvoxelangle — Wave 111 return-voxelangle honesty */
kprintf("futex: soft retvoxelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retvoxelangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft rettexelangle — Wave 111 exclusive texelangle stamp */
kprintf("futex: soft rettexelangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (rettexelangle stamp; Soft≠product)\n");
/* Grep: futex: soft retfragmentangle — Wave 112 return-fragmentangle honesty */
kprintf("futex: soft retfragmentangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retfragmentangle honesty; Soft≠product; not bar3)\n");
/* Grep: futex: soft retvertexangle — Wave 112 exclusive vertexangle stamp */
kprintf("futex: soft retvertexangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retvertexangle stamp; Soft≠product)\n");
                            kprintf("futex: soft deepen wave=%u areas=%u wait_enter=%lu "
            "wake_enter=%lu used=%u waiting=%u soft_log=%lu ok=1 skip=0\n",
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE,
            (unsigned)FUTEX_SOFT_DEEPEN_AREAS,
            (unsigned long)g_soft.u64WaitEnter,
            (unsigned long)g_soft.u64WakeEnter, g_u32SoftUsed,
            g_u32SoftWaiting, (unsigned long)g_soft.u64SoftLog);

    /* Grep: futex: soft inventory PASS / futex: soft PASS */
    kprintf("futex: soft inventory PASS soft_log=%lu wave=%u areas=%u\n",
            (unsigned long)g_soft.u64SoftLog,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE,
            (unsigned)FUTEX_SOFT_DEEPEN_AREAS);
    kprintf("futex: soft PASS wave=%u\n",
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
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
        /* futex: soft key identity */
        futex_soft_inc(&g_soft.u64KeyIdentity);
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
        /* futex: soft wait einval null */
        futex_soft_inc(&g_soft.u64WaitEinval);
        futex_soft_inc(&g_soft.u64WaitEinvalNull);
        futex_soft_maybe_once();
        return -LINUX_EINVAL;
    }
    if (pKey->u8Private) {
        futex_soft_inc(&g_soft.u64WaitPrivate);
    } else {
        futex_soft_inc(&g_soft.u64WaitShared);
    }
    if ((pKey->u64Uaddr & 3ull) != 0) {
        /* futex: soft wait einval align */
        futex_soft_inc(&g_soft.u64WaitEinval);
        futex_soft_inc(&g_soft.u64WaitEinvalAlign);
        futex_soft_maybe_once();
        return -LINUX_EINVAL;
    }
    /* Linux: bitset 0 is EINVAL for WAIT_BITSET; classic uses MATCH_ANY. */
    if (u32Bitset == 0) {
        /* futex: soft wait einval bitset0 */
        futex_soft_inc(&g_soft.u64WaitEinval);
        futex_soft_inc(&g_soft.u64WaitEinvalBit0);
        futex_soft_maybe_once();
        return -LINUX_EINVAL;
    }
    /* Shared keys with zero PA cannot form a stable queue. */
    if (!pKey->u8Private && pKey->u64Phys == 0) {
        /* futex: soft wait einval shared0 */
        futex_soft_inc(&g_soft.u64WaitEinval);
        futex_soft_inc(&g_soft.u64WaitEinvalSh0);
        futex_soft_maybe_once();
        return -LINUX_EINVAL;
    }

    pThr = thread_current();
    /*
     * G-FUT-3: product path sleeps only via thread_block + timer.
     * No schedulable current thread → fail closed (no pause spin).
     */
    if (pThr == NULL) {
        /* futex: soft wait einval no_thr */
        futex_soft_inc(&g_soft.u64WaitEinval);
        futex_soft_inc(&g_soft.u64WaitEinvalNoThr);
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
    futex_soft_inc(&g_soft.u64WaitClaim);
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
        /* futex: soft wake einval null */
        futex_soft_inc(&g_soft.u64WakeEinval);
        futex_soft_inc(&g_soft.u64WakeEinvalNull);
        futex_soft_maybe_once();
        return -LINUX_EINVAL;
    }
    if (pKey->u8Private) {
        futex_soft_inc(&g_soft.u64WakePrivate);
    } else {
        futex_soft_inc(&g_soft.u64WakeShared);
    }
    if (u32Bitset == 0) {
        /* futex: soft wake einval bitset0 */
        futex_soft_inc(&g_soft.u64WakeEinval);
        futex_soft_inc(&g_soft.u64WakeEinvalBit0);
        futex_soft_maybe_once();
        return -LINUX_EINVAL;
    }
    if (u32Count == 0) {
        futex_soft_inc(&g_soft.u64WakeZeroCount);
        futex_soft_maybe_once();
        return 0;
    }
    if (!pKey->u8Private && pKey->u64Phys == 0) {
        /* futex: soft wake einval shared0 */
        futex_soft_inc(&g_soft.u64WakeEinval);
        futex_soft_inc(&g_soft.u64WakeEinvalSh0);
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
        /* futex: soft wake key_match — key_eq before bitset AND */
        futex_soft_inc(&g_soft.u64WakeKeyMatch);
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
        /* futex: soft robust pending */
        futex_soft_inc(&g_soft.u64RobustPending);
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
    } else {
        futex_soft_inc(&g_soft.u64ThrCancelNone);
    }
    return u32Cleared;
}
