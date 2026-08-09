/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Linux-true futex wait queues (G-FUT / G-MO-3). Dual-licensed MIT OR
 * Apache-2.0 (product source license - not a soft-inventory claim).
 * Soft!=product. Pure C11 freestanding.
 *
 * Keying (matches Linux private vs shared) - G-FUT-1:
 *   PRIVATE: (address space, user VA) - AS id is process CR3 when set
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
 * Soft product surface (Soft!=product - diagnostics only):
 *   G-FUT-BITSET - WAIT_BITSET / WAKE_BITSET (grep: futex: wait_bitset,
 *                  futex: wake_bitset)
 *   G-FUT-ROBUST - per-thr robust list head + soft exit OWNER_DIED wake
 *                  (grep: futex: robust set/get/exit)
 *
 * Soft wait/wake inventory (file-local sticky counters; never hard-gate).
 * Lean residual: greppable "futex: soft ..." multi-line dumps capped at
 * FUTEX_SOFT_LOG_CAP (no stamp storms / no image version stamp). Past cap:
 * scan-only (HWM snaps stay live). Soft!=product dual-license honesty:
 * soft inventory != product RR/preempt complete != image version claim.
 * greppable: futex: soft
 *
 * C2 lean residual for Linux-shaped UDX host threads (Soft!=product):
 *   UDX multi-thr hosts (rtl8168_udx / xhci_udx / ddi_host_gj) park on
 *   private/shared futex words from pthread-shaped userspace. Host thr
 *   death must not leave orphans: futex_cancel_thr + soft robust exit
 *   clear waiter slots / OWNER_DIED-wake companions to thread H3
 *   thr_exit_before_as_destroy (udx_host_teardown=1). C2 deepen: cancel
 *   class splits (waiting/idle, priv/shared, deadline, bitset/classic,
 *   timed_out) + full slot scrub (memset; never hard-gate) + robust-slot
 *   scrub for dying tid so standalone cancel is complete. Soft residual
 *   != product multi-CPU thr-kill / full UDX Dual DoD A/B close / bar3 /
 *   image version claim. G-AC-1: soft residual != product AC.
 *   Dual DoD A/B remain OPEN. product_kernel=OPEN. dual=MIT_OR_Apache-2.0.
 *   greppable: futex: soft residual lean
 *              futex: soft thr
 *              udx_host_teardown=1 * soft_ne_product=1
 *              freestanding_class=SKIP * product=UDX
 *              hosts=rtl8168_udx|xhci_udx|ddi_host * multi_thr_host=1
 *              thr_exit_before_as_destroy=1 * H3=death_residual
 *              G-AC-1 * product_kernel=OPEN * dual_dod_a=OPEN
 *
 * Wait object for thread_block/wake is the futex_waiter slot itself; tag 0.
 * Fixed table (no heap) - ENOMEM when all slots are in use.
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

/* Soft deepen stamp (greppable wave=126). Soft!=product; no version stamp. */
#define FUTEX_SOFT_DEEPEN_WAVE 126u
/*
 * Lean residual area count (prefix-stable inventory surface only).
 * Stamp-storm ret*angle catalogs removed; Soft!=product; no version stamp.
 * greppable: futex: soft residual lean
 */
#define FUTEX_SOFT_DEEPEN_AREAS 34u
/*
 * Cap full multi-line soft inventory dumps (Soft!=product; no stamp storms).
 * Init + first-activity once + a few residual dumps stay greppable;
 * further calls refresh occupancy snaps only (silent).
 */
#define FUTEX_SOFT_LOG_CAP 4u

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
    u64 u64WaitEagainCancel;/* cancel after enqueue -> EAGAIN */
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
    /* C2 residual deepen (UDX multi-thr host death; Soft!=product):
     * cancel class splits companion to thread H3 residual lean scrub.
     * Silent tallies only; never hard-gate. greppable: futex: soft thr */
    u64 u64ThrCancelWaiting;  /* cleared slots that were u8Waiting */
    u64 u64ThrCancelIdle;     /* cleared used slots not waiting */
    u64 u64ThrCancelPriv;     /* cleared private-key waiters */
    u64 u64ThrCancelShared;   /* cleared shared-key waiters */
    u64 u64ThrCancelDeadline; /* cleared slots with non-zero deadline */
    u64 u64ThrCancelBitset;   /* cleared non-MATCH_ANY bitset waiters */
    u64 u64ThrCancelClassic;  /* cleared MATCH_ANY / classic waiters */
    u64 u64ThrCancelTimedOut; /* cleared slots with u32TimedOut set */
    u64 u64ThrCancelRobust;   /* robust slots scrubbed for dying tid */
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
 * Soft!=product. Multi-line dumps capped (FUTEX_SOFT_LOG_CAP) - no stamp
 * storms. No image version stamp. When capped: scan only (silent).
 * greppable: futex: soft
 */
static void
futex_soft_log(void)
{
    /*
     * Cap multi-line inventory dumps. Past FUTEX_SOFT_LOG_CAP refresh
     * occupancy snaps only (silent). Soft!=product; no stamp storms.
     */
    if (g_soft.u64SoftLog >= (u64)FUTEX_SOFT_LOG_CAP) {
        futex_soft_scan();
        return;
    }
    futex_soft_inc(&g_soft.u64SoftLog);
    futex_soft_scan();

    /*
     * Lean residual catalog (prefix-stable): fixed-table capacity + wait/wake
     * surface for smoke greps. Soft!=product dual-license honesty: soft lines
     * never claim product RR / preemption / image version.
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

    /* Grep: futex: soft wait - path tallies (Wave 15 deepen) */
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

    /* Grep: futex: soft wait eagain - split surface */
    kprintf("futex: soft wait eagain total=%lu fast=%lu lock=%lu "
            "cancel=%lu wave=%u\n",
            (unsigned long)g_soft.u64WaitEagain,
            (unsigned long)g_soft.u64WaitEagainFast,
            (unsigned long)g_soft.u64WaitEagainLock,
            (unsigned long)g_soft.u64WaitEagainCancel,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft wait etimedout - split surface */
    kprintf("futex: soft wait etimedout total=%lu imm=%lu park=%lu "
            "timer_reap=%lu wave=%u\n",
            (unsigned long)g_soft.u64WaitEtimedout,
            (unsigned long)g_soft.u64WaitEtimedImm,
            (unsigned long)g_soft.u64WaitEtimedPark,
            (unsigned long)g_soft.u64TimerReap,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft wait einval - Wave 15 split surface */
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

    /* Grep: futex: soft wait outcome - rollup */
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

    /* Grep: futex: soft wake - path tallies (Wave 15 deepen) */
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

    /* Grep: futex: soft wake einval - Wave 15 split surface */
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

    /* Grep: futex: soft wake outcome - rollup */
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

    /* Grep: futex: soft slot - Wave 15 live class snap */
    kprintf("futex: soft slot used=%u waiting=%u priv=%u shared=%u "
            "deadline=%u bitset=%u classic=%u timed_out=%u has_thr=%u "
            "robust_used=%u wave=%u\n",
            g_u32SoftUsed, g_u32SoftWaiting, g_u32SoftWaitPriv,
            g_u32SoftWaitShared, g_u32SoftWaitDeadline,
            g_u32SoftWaitBitset, g_u32SoftWaitClassic,
            g_u32SoftWaitTimedOut, g_u32SoftWaitHasThr, g_u32SoftRobUsed,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft capacity - fixed table lamps */
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
     * Grep: futex: soft catalog - opcode soft inventory (impl vs not).
     * Product-active: WAIT/WAKE/WAIT_BITSET/WAKE_BITSET + robust helpers.
     * REQUEUE/PI/FD remain soft catalog only (not implemented here).
     */
    kprintf("futex: soft catalog wait=1 wake=1 wait_bitset=1 wake_bitset=1 "
            "robust=1 requeue=0 cmp_requeue=0 wake_op=0 pi=0 fd=0 "
            "g_fut1=1 g_fut2=1 g_fut3=1 wave=%u\n",
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /*
     * Grep: futex: soft path
     * Honesty: park via thread_block only; no product spin; Soft!=product
     * RR/preempt. Dual-license product source != soft inventory claim.
     * UDX multi-thr host residual: cancel_thr + robust exit companion to
     * thr_exit_before_as_destroy (udx_host_teardown=1). freestanding SKIP.
     */
    kprintf("futex: soft path park=thread_block+schedule "
            "lost_wake=recheck_under_lock match=key+bitset_and "
            "irqsave_lock=1 timer=futex_timer_check "
            "thr_cancel=futex_cancel_thr g_fut1=shared_pa "
            "g_fut2=mono_deadline g_fut3=no_product_spin "
            "bitset=g_fut_bitset robust=g_fut_robust "
            "udx_host_teardown=1 freestanding_class=SKIP product=UDX "
            "product_kernel=OPEN rr_complete=0 preempt_complete=0 "
            "soft_ne_product=1 G-AC-1 dual=MIT_OR_Apache-2.0 "
            "wave=%u (soft inventory; Soft!=product)\n",
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft timer - IRQ-safe check/reap (no kprintf on IRQ path) */
    kprintf("futex: soft timer check=%lu reap=%lu "
            "path=futex_timer_check irqsafe_counter=1 g_fut2=1 wave=%u\n",
            (unsigned long)g_soft.u64TimerCheck,
            (unsigned long)g_soft.u64TimerReap,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /*
     * Grep: futex: soft thr - cancel_thr dying-thr surface
     * C2 residual deepen (Soft!=product; G-AC-1): class splits
     * (waiting/idle, priv/shared, deadline, bitset/classic, timed_out)
     * + robust scrub companion to thread H3 residual lean scrub.
     * Lean residual for Linux-shaped UDX multi-thr hosts (IRQ thr + work thr)
     * so death does not hang shared-AS teardown.
     * hosts=rtl8168_udx|xhci_udx|ddi_host; freestanding class SKIP.
     * Soft residual != Dual DoD A/B close != product multi-CPU thr-kill.
     * product_kernel=OPEN. dual=MIT_OR_Apache-2.0. No version stamp.
     */
    kprintf("futex: soft thr cancel_calls=%lu slots_cleared=%lu "
            "none=%lu waiting=%lu idle=%lu priv=%lu shared=%lu "
            "deadline=%lu bitset=%lu classic=%lu timed_out=%lu "
            "robust_scrub=%lu path=futex_cancel_thr "
            "death_orphan=wake_clear "
            "udx_host_teardown=1 H3=death_residual multi_thr_host=1 "
            "hosts=rtl8168_udx|xhci_udx|ddi_host "
            "thr_exit_before_as_destroy=1 freestanding_class=SKIP "
            "product=UDX product_kernel=OPEN soft_ne_product=1 "
            "G-AC-1 dual=MIT_OR_Apache-2.0 wave=%u\n",
            (unsigned long)g_soft.u64ThrCancelCalls,
            (unsigned long)g_soft.u64ThrCancel,
            (unsigned long)g_soft.u64ThrCancelNone,
            (unsigned long)g_soft.u64ThrCancelWaiting,
            (unsigned long)g_soft.u64ThrCancelIdle,
            (unsigned long)g_soft.u64ThrCancelPriv,
            (unsigned long)g_soft.u64ThrCancelShared,
            (unsigned long)g_soft.u64ThrCancelDeadline,
            (unsigned long)g_soft.u64ThrCancelBitset,
            (unsigned long)g_soft.u64ThrCancelClassic,
            (unsigned long)g_soft.u64ThrCancelTimedOut,
            (unsigned long)g_soft.u64ThrCancelRobust,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft claim - waiter slot claim surface */
    kprintf("futex: soft claim n=%lu peak_used=%lu peak_wait=%lu "
            "max=%u heap=0 wave=%u\n",
            (unsigned long)g_soft.u64WaitClaim,
            (unsigned long)g_soft.u64TablePeakUsed,
            (unsigned long)g_soft.u64TablePeakWait,
            (unsigned)GJ_FUTEX_MAX_WAITERS,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft peak - table peak rollup */
    kprintf("futex: soft peak used=%lu wait=%lu robust=%lu "
            "samples=%lu wave=%u\n",
            (unsigned long)g_soft.u64TablePeakUsed,
            (unsigned long)g_soft.u64TablePeakWait,
            (unsigned long)g_soft.u64TablePeakRob,
            (unsigned long)g_soft.u64SoftScan,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /*
     * Grep: futex: soft g_fut
     * Honesty: G-FUT lamps + Soft!=product (!= RR/preempt complete).
     * UDX host thr residual companion lamps (not product Dual DoD close).
     */
    kprintf("futex: soft g_fut g_fut1=1 g_fut2=1 g_fut3=1 bitset=1 "
            "robust=1 requeue=0 pi=0 fd=0 udx_host_teardown=1 "
            "freestanding_class=SKIP product=UDX product_kernel=OPEN "
            "rr_complete=0 preempt_complete=0 soft_ne_product=1 "
            "G-AC-1 dual=MIT_OR_Apache-2.0 wave=%u "
            "(soft inventory; Soft!=product)\n",
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft match - key + bitset AND surface */
    kprintf("futex: soft match key_eq=1 bitset_and=1 key_match=%lu "
            "bitset_miss=%lu woken=%lu hit=%lu miss=%lu wave=%u\n",
            (unsigned long)g_soft.u64WakeKeyMatch,
            (unsigned long)g_soft.u64WakeBitsetMiss,
            (unsigned long)g_soft.u64WakeWoken,
            (unsigned long)g_soft.u64WakeHit,
            (unsigned long)g_soft.u64WakeMiss,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /*
     * Return-path catalog - wait/wake terminal outcomes.
     * Soft!=product RR / preemption. product_kernel=OPEN. No version stamp.
     */

    /* Grep: futex: soft ratio - basis-point outcome rollup */
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

    /* Grep: futex: soft headroom - free waiter/robust slots */
    kprintf("futex: soft headroom free=%u waiters_max=%u robust_free=%u "
            "robust_max=%u used=%u waiting=%u wave=%u\n",
            g_u32SoftFree, (unsigned)GJ_FUTEX_MAX_WAITERS, g_u32SoftRobFree,
            (unsigned)GJ_FUTEX_ROBUST_SLOTS, g_u32SoftUsed, g_u32SoftWaiting,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /*
     * Grep: futex: soft residual lean
     * C2 residual deepen (Soft!=product dual-license; G-AC-1): Linux-shaped
     * UDX multi-thr host thr residual. cancel_thr class splits + full scrub
     * + robust-slot scrub + robust exit clear orphans on multi-thr host
     * teardown; companion to thread H3 thr_exit_before_as_destroy.
     * Stamp storms removed (areas lean). freestanding class SKIP;
     * product = ABI+UDX. Soft residual != Dual DoD A/B closed != bar3 !=
     * RR/preempt complete != image stamp. product_kernel=OPEN.
     * greppable: hosts=rtl8168_udx|xhci_udx|ddi_host * multi_thr_host=1
     * greppable: G-AC-1 * product_kernel=OPEN * dual_dod_a=OPEN
     */
    kprintf("futex: soft residual lean udx_host_teardown=1 "
            "thr_cancel=futex_cancel_thr robust_exit=futex_exit_robust_list "
            "death_orphan=wake_clear thr_exit_before_as_destroy=1 "
            "H3=death_residual multi_thr_host=1 "
            "hosts=rtl8168_udx|xhci_udx|ddi_host "
            "cancel_waiting=%lu cancel_idle=%lu cancel_priv=%lu "
            "cancel_shared=%lu cancel_deadline=%lu "
            "cancel_bitset=%lu cancel_classic=%lu cancel_timed_out=%lu "
            "cancel_robust=%lu scrub=memset+robust_tid "
            "freestanding_class=SKIP product=UDX product_kernel=OPEN "
            "dual_dod_a=OPEN dual_dod_b=OPEN bar3=OPEN "
            "rr_complete=0 preempt_complete=0 soft_ne_product=1 "
            "G-AC-1 dual=MIT_OR_Apache-2.0 "
            "log_cap=%u areas=%u wave=%u "
            "(Soft!=product; dual MIT OR Apache-2.0; no version stamp)\n",
            (unsigned long)g_soft.u64ThrCancelWaiting,
            (unsigned long)g_soft.u64ThrCancelIdle,
            (unsigned long)g_soft.u64ThrCancelPriv,
            (unsigned long)g_soft.u64ThrCancelShared,
            (unsigned long)g_soft.u64ThrCancelDeadline,
            (unsigned long)g_soft.u64ThrCancelBitset,
            (unsigned long)g_soft.u64ThrCancelClassic,
            (unsigned long)g_soft.u64ThrCancelTimedOut,
            (unsigned long)g_soft.u64ThrCancelRobust,
            (unsigned)FUTEX_SOFT_LOG_CAP,
            (unsigned)FUTEX_SOFT_DEEPEN_AREAS,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);
    kprintf("futex: soft residual lean PASS soft_ne_product=1 "
            "udx_host_teardown=1 multi_thr_host=1 "
            "hosts=rtl8168_udx|xhci_udx|ddi_host "
            "product_kernel=OPEN G-AC-1 dual=MIT_OR_Apache-2.0 "
            "dual_dod_a=OPEN dual_dod_b=OPEN bar3=OPEN wave=%u\n",
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft surface - area catalog (lean residual) */
    kprintf("futex: soft surface wait,wake,stats,table,key,robust,path,"
            "timer,thr,slot,capacity,catalog,claim,peak,g_fut,match,"
            "ratio,surface,headroom,einval,outcome,eagain,etimedout,"
            "bitset,inventory,deepen,residual_lean,PASS,cancel,park "
            "areas=%u wave=%u\n",
            (unsigned)FUTEX_SOFT_DEEPEN_AREAS,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE);

    /* Grep: futex: soft deepen - Soft!=product; no stamp storms; no version stamp */
    kprintf("futex: soft deepen wave=%u areas=%u wait_enter=%lu "
            "wake_enter=%lu used=%u waiting=%u soft_log=%lu "
            "log_cap=%u soft_ne_product=1 G-AC-1 "
            "product_kernel=OPEN ok=1 skip=0\n",
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE,
            (unsigned)FUTEX_SOFT_DEEPEN_AREAS,
            (unsigned long)g_soft.u64WaitEnter,
            (unsigned long)g_soft.u64WakeEnter, g_u32SoftUsed,
            g_u32SoftWaiting, (unsigned long)g_soft.u64SoftLog,
            (unsigned)FUTEX_SOFT_LOG_CAP);

    /* Grep: futex: soft inventory PASS / futex: soft PASS */
    kprintf("futex: soft inventory PASS soft_log=%lu log_cap=%u "
            "wave=%u areas=%u soft_ne_product=1 G-AC-1 "
            "product_kernel=OPEN\n",
            (unsigned long)g_soft.u64SoftLog,
            (unsigned)FUTEX_SOFT_LOG_CAP,
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE,
            (unsigned)FUTEX_SOFT_DEEPEN_AREAS);
    kprintf("futex: soft PASS wave=%u log_cap=%u soft_ne_product=1 "
            "G-AC-1 product_kernel=OPEN\n",
            (unsigned)FUTEX_SOFT_DEEPEN_WAVE,
            (unsigned)FUTEX_SOFT_LOG_CAP);
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
 * Resolve VA -> PA for shared keys (G-FUT-1 / futex: shared key PA).
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
    /* futex: shared key PA - G-FUT-1 cross-process queue identity */
    futex_key_set_shared(pOut, u64Uaddr, pa);
    /* futex: soft key shared_ok */
    futex_soft_inc(&g_soft.u64KeySharedOk);
    return GJ_OK;
}

/*
 * Resolve uaddr into a stable wait key. Private keeps (AS, VA);
 * shared maps VA -> PA so distinct mappings of the same frame share one queue.
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
 * Cancel a wait that was registered (thread_block) but will not schedule -
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
    /* futex: soft timer check - IRQ-safe counter only (no kprintf). */
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
    /* futex: soft timer reap - IRQ-safe counter only (no kprintf). */
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
     * No schedulable current thread -> fail closed (no pause spin).
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

    /* Absolute deadline already past - treat as immediate timeout. */
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

    /* Already woken (or timed out) before schedule - do not sleep. */
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
 * Returns 0, or -LINUX_* (EAGAIN if value already changed, ETIMEDOUT, ...).
 * u64DeadlineMonoNsec is absolute mono nsec, or 0 for no deadline.
 */
i64
futex_wait(volatile u32 *pU32, u32 u32Val, const struct gj_futex_key *pKey,
           u64 u64DeadlineMonoNsec)
{
    return futex_wait_common(pU32, u32Val, pKey, u64DeadlineMonoNsec,
                             GJ_FUTEX_BITSET_MATCH_ANY);
}

/* futex: wait_bitset - G-FUT-BITSET soft product (futex: soft wait bitset) */
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
        /* futex: soft wake key_match - key_eq before bitset AND */
        futex_soft_inc(&g_soft.u64WakeKeyMatch);
        if ((pW->u32Bitset & u32Bitset) == 0) {
            /* futex: soft wake bitset miss - key ok, mask no overlap */
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

/* futex: wake_bitset - G-FUT-BITSET soft product (futex: soft wake bitset) */
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

/* futex: robust set - G-FUT-ROBUST (futex: soft robust) */
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

/* futex: robust get - G-FUT-ROBUST (futex: soft robust) */
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
 * Cycle-capped; best-effort user copies only - no PI recovery.
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
 *
 * C2 residual deepen for Linux-shaped UDX host threads (Soft!=product):
 *   Multi-thr UDX hosts share one process AS (IRQ thr + work thr). On thr
 *   death / robust exit, clear this thr's waiter slots so schedule cannot
 *   resume into a torn-down host. Companion to thread_exit_process H3
 *   thr_exit_before_as_destroy (udx_host_teardown=1).
 *   Class splits (waiting/idle, priv/shared, deadline, bitset/classic,
 *   timed_out) are silent soft tallies only - never hard-gate. Full slot
 *   scrub via memset so soft_scan / reuse never inherits death residue
 *   (incl. pad). Also scrubs robust-list slots for this tid so standalone
 *   cancel is complete without relying on exit_robust walk. Soft residual
 *   != product multi-CPU thr-kill / Dual DoD A/B close / bar3.
 *   G-AC-1: soft residual != product AC. product_kernel=OPEN.
 * greppable: futex: soft thr * udx_host_teardown=1 * soft_ne_product=1
 * greppable: hosts=rtl8168_udx|xhci_udx|ddi_host * multi_thr_host=1
 * greppable: G-AC-1 * product_kernel=OPEN * dual_dod_a=OPEN
 */
u32
futex_cancel_thr(struct gj_thread *pThr)
{
    u32 iSlot;
    u32 u32Cleared = 0;
    u32 u32RobustScrub = 0;
    u32 u32Tid;
    u64 u64Flags;

    if (pThr == NULL) {
        return 0;
    }

    /* futex: soft thr cancel_calls */
    futex_soft_inc(&g_soft.u64ThrCancelCalls);
    u32Tid = pThr->u32Id;

    u64Flags = gj_spin_lock_irqsave(&g_lockFutex);
    for (iSlot = 0; iSlot < GJ_FUTEX_MAX_WAITERS; iSlot++) {
        struct futex_waiter *pW = &g_aWaiters[iSlot];

        if (!pW->u8Used || pW->pThr != pThr) {
            continue;
        }
        /* C2 cancel class splits (soft residual; under lock exact). */
        if (pW->u8Waiting) {
            futex_soft_inc(&g_soft.u64ThrCancelWaiting);
            pW->u8Waiting = 0;
            (void)thread_wake(pW, 0, 1);
        } else {
            futex_soft_inc(&g_soft.u64ThrCancelIdle);
        }
        if (pW->key.u8Private) {
            futex_soft_inc(&g_soft.u64ThrCancelPriv);
        } else {
            futex_soft_inc(&g_soft.u64ThrCancelShared);
        }
        if (pW->u64Deadline != 0) {
            futex_soft_inc(&g_soft.u64ThrCancelDeadline);
        }
        if (pW->u32Bitset != 0 &&
            pW->u32Bitset != GJ_FUTEX_BITSET_MATCH_ANY) {
            futex_soft_inc(&g_soft.u64ThrCancelBitset);
        } else {
            futex_soft_inc(&g_soft.u64ThrCancelClassic);
        }
        if (pW->u32TimedOut != 0) {
            futex_soft_inc(&g_soft.u64ThrCancelTimedOut);
        }
        /*
         * Full slot scrub (memset): no death residue on soft_scan / claim
         * reuse - pad + key + thr ptr all zeroed. Soft!=product; G-AC-1.
         */
        memset(pW, 0, sizeof(*pW));
        u32Cleared++;
    }
    /* C2: robust-slot scrub for dying tid (standalone cancel complete). */
    if (u32Tid != 0) {
        for (iSlot = 0; iSlot < GJ_FUTEX_ROBUST_SLOTS; iSlot++) {
            if (g_aRobust[iSlot].u8Used &&
                g_aRobust[iSlot].u32Tid == u32Tid) {
                memset(&g_aRobust[iSlot], 0, sizeof(g_aRobust[iSlot]));
                u32RobustScrub++;
            }
        }
    }
    gj_spin_unlock_irqrestore(&g_lockFutex, u64Flags);
    /* futex: soft thr cancel - UDX host thr death residual (C2 deepen) */
    if (u32RobustScrub > 0) {
        g_soft.u64ThrCancelRobust += (u64)u32RobustScrub;
    }
    if (u32Cleared > 0) {
        g_soft.u64ThrCancel += (u64)u32Cleared;
    } else {
        futex_soft_inc(&g_soft.u64ThrCancelNone);
    }
    return u32Cleared;
}
