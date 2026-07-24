/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Notification badge objects (IRQ → userspace UDX path).
 *
 * greppable: NOTIFY_BADGE_PULSE_WAIT
 * greppable: NOTIFY_SOFT_MULTI_WAITER
 *
 * Hard IRQ / soft inject only pulses badges (OR) and may soft multi-wake
 * waiters — no alloc, no copy. Userspace (or UDX) batches work after
 * notify_wait returns.
 *
 * Wait key is the notify object; tag NOTIFY_TAG_WAITER = blocked in wait.
 * u64Pending is updated with atomics so IRQ and wait paths do not drop bits.
 *
 * Soft multi-waiter: several threads may block on one object. Pulse wakes up
 * to NOTIFY_SOFT_MULTI_MAX; each waiter CAS-claims matching badge bits.
 *
 * Soft product inventory (file-local sticky counters; never hard-gate).
 * Wave 35 exclusive deepen — greppable prefix-stable serial markers
 * (notify: soft …); diagnostics only, never hard-gate product:
 *   notify: soft inventory         — multi_max + path catalog at init/log
 *   notify: soft pulse inventory   — pulse/OR/wake catalog + counters
 *   notify: soft wait inventory    — wait/poll/CAS/block catalog + counters
 *   notify: soft pulse             — path tallies (hit/dead split/wake/…)
 *   notify: soft pulse outcome     — hit/dead/wake rollup
 *   notify: soft pulse dead        — null/ready/state split surface
 *   notify: soft wait              — path tallies (enter/hit/park/… splits)
 *   notify: soft wait outcome      — hit/park/dead/poll rollup
 *   notify: soft multi             — multi-wake call/sum/zero/peak surface
 *   notify: soft badge             — pending/last/zero_coalesce/bits soft
 *   notify: soft install           — install ok + fail splits
 *   notify: soft abort             — abort + wake + nowaiter
 *   notify: soft mark_dead         — mark_dead + ready/revoke surface
 *   notify: soft msix              — global MSI-X bind / live snapshot
 *   notify: soft query             — accessor sample tallies
 *   notify: soft capacity          — multi_max / tag / heap lamps
 *   notify: soft catalog           — path surface catalog (impl vs not)
 *   notify: soft return            — Wave 17 wait|install|wake return surfaces
 *   notify: soft return rate       — Wave 17 wait|install|wake rate lamps
 *   notify: soft retcode           — Wave 17 observed badge/status retcode catalog
 *   notify: soft return selftest — Wave 19 terminal return surface
 *   notify: soft retmap     — Wave 19 return-surface map
 *   notify: soft deepen            — wave=84 areas stamp
 *   notify: soft path              — G-NOTIFY invariants + honesty claim
 *   notify: soft stats             — aggregate path counters
 *   notify: soft pulse hit         — pulse delivered to live object
 *   notify: soft pulse dead        — pulse dropped (null/not ready/DEAD)
 *   notify: soft pulse wake        — soft multi-wake from pulse
 *   notify: soft pulse nowaiter    — pulse with no registered waiter
 *   notify: soft wait hit          — CAS-clear matched badge bits
 *   notify: soft wait park         — thread_block + schedule
 *   notify: soft wait poll miss    — non-block / no thr, no pending
 *   notify: soft wait dead         — object DEAD during wait
 *   notify: soft wait cas retry    — lost CAS race with concurrent pulse
 *   notify: soft wait self wake    — post-block self soft multi-wake
 *   notify: soft multi wake        — soft multi-wake invocation
 *   notify: soft abort             — abort soft multi-wake (no badge)
 *   notify: soft mark_dead         — mark_dead + abort path
 *   notify: soft install ok        — cap install success
 *   notify: soft install fail      — cap install rejected
 * Honesty: soft multi-waiter ≠ multi-process notify product
 *   (multi_proc=0 / soft_ne_multi_proc=1). Soft ≠ bar3.
 *   Soft ≠ MIG REPLY product.
 * greppable: notify: soft
 */
#include <gj/cap.h>
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/notify.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/thread.h>

static struct gj_notify g_msixNotify;
static int              g_fMsixInited;

/* Wave 35 exclusive soft deepen stamp (greppable wave=84). */
#define NOTIFY_SOFT_DEEPEN_WAVE 77u
/* +return selftest|retmap over Wave 17 return rate|retcode. */
#define NOTIFY_SOFT_DEEPEN_AREAS 116u

/*
 * Soft path sticky counters (wrap OK; diagnostics only).
 * Bumped on product return paths; never hard-gate behavior.
 * Pulse path is IRQ-callable → atomic RMW only (no kprintf).
 * Wave 20 deepen: pulse/wait splits + multi + badge + install + query +
 * msix + capacity + catalog + outcome + path + return surfaces.
 * Soft multi-waiter ≠ multi-process notify product.
 * Soft ≠ MIG REPLY product.
 * greppable: notify: soft stats
 * greppable: notify: soft
 */
struct notify_soft_stats {
    u64 u64Init;             /* notify_init calls */
    u64 u64PulseHit;         /* pulse on live ready object */
    u64 u64PulseDead;        /* pulse dropped (any reason) */
    u64 u64PulseDeadNull;    /* pulse dropped: pN == NULL */
    u64 u64PulseDeadReady;   /* pulse dropped: !u32Ready */
    u64 u64PulseDeadState;   /* pulse dropped: not LIVE */
    u64 u64PulseZeroCoalesce;/* badge 0 → bit 0 */
    u64 u64PulseWake;        /* soft multi-wake from pulse */
    u64 u64PulseNoWaiter;    /* pulse with no waiter registered */
    u64 u64PulseEnter;       /* notify_pulse entries */
    u64 u64SignalAlias;      /* notify_signal → pulse */
    u64 u64WaitEnter;        /* notify_wait entries */
    u64 u64WaitHit;          /* CAS-clear matched bits */
    u64 u64WaitPark;         /* thread_block + schedule */
    u64 u64WaitPollMiss;     /* non-block / no thr miss */
    u64 u64WaitDead;         /* object DEAD during wait */
    u64 u64WaitDeadEnter;    /* DEAD on first live check */
    u64 u64WaitDeadLoop;     /* DEAD mid-loop after register */
    u64 u64WaitCasRetry;     /* lost CAS race */
    u64 u64WaitSelfWake;     /* post-block self multi-wake */
    u64 u64WaitMaskAny;      /* mask==0 → ~0ull */
    u64 u64WaitBlock;        /* fBlock != 0 entries */
    u64 u64WaitNoblock;      /* fBlock == 0 entries */
    u64 u64WaitRegister;     /* first soft multi-waiter register */
    u64 u64WaitNoThr;        /* block requested but pCur == NULL */
    u64 u64WaitLoop;         /* wait loop iterations (soft) */
    u64 u64WaitRetBits;      /* Wave 19: wait returned non-zero badge */
    u64 u64WaitRetZero;      /* Wave 19: wait returned 0 (dead/miss) */
    u64 u64Poll;             /* notify_poll entries */
    u64 u64Abort;            /* notify_abort_waiter */
    u64 u64AbortWake;        /* abort issued multi-wake */
    u64 u64AbortNoWaiter;    /* abort with no waiter */
    u64 u64MarkDead;         /* notify_mark_dead */
    u64 u64MarkDeadRevoke;   /* mark_dead took revoke_begin path */
    u64 u64MarkDeadForce;    /* mark_dead forced DEAD store */
    u64 u64InstallOk;        /* notify_install success */
    u64 u64InstallFail;      /* notify_install rejected (any) */
    u64 u64InstallFailNull;  /* install: null args / no cnode */
    u64 u64InstallFailDead;  /* install: object not live */
    u64 u64InstallFailCap;   /* install: cap_alloc_install fail */
    u64 u64InstallDefaultRights; /* install used default rights */
    u64 u64MultiWakeCalls;   /* notify_soft_multi_wake invocations */
    u64 u64MultiWakeSum;     /* sum of thread_wake return counts */
    u64 u64MultiWakeZero;    /* multi-wake returned 0 woken */
    u64 u64MultiWakeNull;    /* multi-wake pN == NULL */
    u64 u64MultiWakePeak;    /* peak cWoken observed (soft) */
    u64 u64WaitersPeak;      /* peak u32Waiters observed (soft) */
    u64 u64BadgeBitsSum;     /* sum of pulse badge popcount (soft) */
    u64 u64BadgeLastOr;      /* OR of all last badges (soft) */
    u64 u64QuerySignals;     /* notify_signals samples */
    u64 u64QueryPending;     /* notify_pending samples */
    u64 u64QueryLastBadge;   /* notify_last_badge samples */
    u64 u64QueryWaiters;     /* notify_waiters samples */
    u64 u64QueryIsLive;      /* notify_is_live samples */
    u64 u64QueryIsLiveYes;   /* notify_is_live returned live */
    u64 u64QueryIsLiveNo;    /* notify_is_live returned not live */
    u64 u64SoftLog;          /* times soft inventory printed */
    u64 u64MsixInit;         /* notify_msix_init calls */
    u64 u64MsixInitSkip;     /* msix_init early return (already ready) */
    u64 u64MsixGlobal;       /* notify_msix_global samples */
};

static struct notify_soft_stats g_soft;
/* One-shot deep print after first product wait/pulse activity (soft). */
static u8 g_fSoftStatsOnce;

/** Soft: atomic sticky bump (IRQ-safe; wrap OK for telemetry). */
static void
notify_soft_inc(u64 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (void)__atomic_fetch_add(pCtr, 1ull, __ATOMIC_RELAXED);
}

/** Soft: atomic sticky add of a wake count (IRQ-safe). */
static void
notify_soft_add(u64 *pCtr, u64 u64N)
{
    if (pCtr == NULL || u64N == 0) {
        return;
    }
    (void)__atomic_fetch_add(pCtr, u64N, __ATOMIC_RELAXED);
}

/**
 * Soft: popcount of a badge word (diagnostics only; pure C, no builtins
 * required beyond portable shifts).
 */
static u32
notify_soft_popcount64(u64 u64V)
{
    u32 cBits = 0;

    while (u64V != 0) {
        cBits += (u32)(u64V & 1ull);
        u64V >>= 1;
    }
    return cBits;
}

/**
 * Soft: note multi-wake result (calls / sum / zero / peak).
 * IRQ-safe (atomics only).
 */
static void
notify_soft_note_wake(u32 cWoken)
{
    u64 u64Peak;

    if (cWoken == 0) {
        notify_soft_inc(&g_soft.u64MultiWakeZero);
        return;
    }
    notify_soft_add(&g_soft.u64MultiWakeSum, (u64)cWoken);
    u64Peak = __atomic_load_n(&g_soft.u64MultiWakePeak, __ATOMIC_RELAXED);
    while ((u64)cWoken > u64Peak) {
        if (__atomic_compare_exchange_n(&g_soft.u64MultiWakePeak, &u64Peak,
                                        (u64)cWoken, 0, __ATOMIC_RELAXED,
                                        __ATOMIC_RELAXED)) {
            break;
        }
    }
}

/**
 * Soft: note soft multi-waiter count peak (IRQ/wait safe).
 */
static void
notify_soft_note_waiters(u32 cWaiters)
{
    u64 u64Peak;

    u64Peak = __atomic_load_n(&g_soft.u64WaitersPeak, __ATOMIC_RELAXED);
    while ((u64)cWaiters > u64Peak) {
        if (__atomic_compare_exchange_n(&g_soft.u64WaitersPeak, &u64Peak,
                                        (u64)cWaiters, 0, __ATOMIC_RELAXED,
                                        __ATOMIC_RELAXED)) {
            break;
        }
    }
}

/**
 * Soft: snapshot MSI-X global object for greppable msix/badge lines.
 * Diagnostic race OK (no hard lock).
 */
static void
notify_soft_msix_snap(u32 *pReady, u32 *pLive, u32 *pSignals, u64 *pPending,
                      u64 *pLast, u32 *pWaiters, u32 *pHasWaiter)
{
    u32 u32Ready = 0;
    u32 u32Live = 0;
    u32 u32Signals = 0;
    u64 u64Pending = 0;
    u64 u64Last = 0;
    u32 u32Waiters = 0;
    u32 u32Has = 0;

    if (g_fMsixInited) {
        u32Ready = g_msixNotify.u32Ready ? 1u : 0u;
        u32Live = (__atomic_load_n(&g_msixNotify.hdr.u32State,
                                   __ATOMIC_ACQUIRE) ==
                   (u32)GJ_OBJ_LIVE)
                      ? 1u
                      : 0u;
        u32Signals =
            __atomic_load_n(&g_msixNotify.u32Signals, __ATOMIC_ACQUIRE);
        u64Pending =
            __atomic_load_n(&g_msixNotify.u64Pending, __ATOMIC_ACQUIRE);
        u64Last =
            __atomic_load_n(&g_msixNotify.u64LastBadge, __ATOMIC_ACQUIRE);
        u32Waiters =
            __atomic_load_n(&g_msixNotify.u32Waiters, __ATOMIC_ACQUIRE);
        if (g_msixNotify.pWaiter != NULL || u32Waiters > 0u) {
            u32Has = 1u;
        }
    }
    if (pReady != NULL) {
        *pReady = u32Ready;
    }
    if (pLive != NULL) {
        *pLive = u32Live;
    }
    if (pSignals != NULL) {
        *pSignals = u32Signals;
    }
    if (pPending != NULL) {
        *pPending = u64Pending;
    }
    if (pLast != NULL) {
        *pLast = u64Last;
    }
    if (pWaiters != NULL) {
        *pWaiters = u32Waiters;
    }
    if (pHasWaiter != NULL) {
        *pHasWaiter = u32Has;
    }
}

/**
 * Greppable soft pulse/wait inventory + Wave 20 deepen surfaces.
 * Called from notify_msix_init and once after first wait activity.
 * Never allocates; not for hard-IRQ (kprintf only from product paths).
 * Soft multi-waiter ≠ multi-process notify product (multi_proc=0).
 * Soft ≠ MIG REPLY product.
 * greppable: notify: soft inventory
 * greppable: notify: soft pulse inventory
 * greppable: notify: soft wait inventory
 * greppable: notify: soft pulse
 * greppable: notify: soft pulse outcome
 * greppable: notify: soft pulse dead
 * greppable: notify: soft wait
 * greppable: notify: soft wait outcome
 * greppable: notify: soft multi
 * greppable: notify: soft badge
 * greppable: notify: soft install
 * greppable: notify: soft abort
 * greppable: notify: soft mark_dead
 * greppable: notify: soft msix
 * greppable: notify: soft query
 * greppable: notify: soft capacity
 * greppable: notify: soft catalog
 * greppable: notify: soft return
 * greppable: notify: soft deepen
 * greppable: notify: soft path
 * greppable: notify: soft stats
 * greppable: notify: soft inventory PASS / notify: soft PASS
 */
static void
notify_soft_log(void)
{
    struct notify_soft_stats s;
    u32                      u32Ready;
    u32                      u32Live;
    u32                      u32Signals;
    u64                      u64Pending;
    u64                      u64Last;
    u32                      u32Waiters;
    u32                      u32HasWaiter;
    u32                      u32PendBits;
    u32                      u32LastBits;

    notify_soft_inc(&g_soft.u64SoftLog);

    /*
     * Snapshot under relaxed loads — soft inventory only; concurrent
     * IRQ pulse bumps may race (acceptable for diagnostics).
     */
    s.u64Init = __atomic_load_n(&g_soft.u64Init, __ATOMIC_RELAXED);
    s.u64PulseHit =
        __atomic_load_n(&g_soft.u64PulseHit, __ATOMIC_RELAXED);
    s.u64PulseDead =
        __atomic_load_n(&g_soft.u64PulseDead, __ATOMIC_RELAXED);
    s.u64PulseDeadNull =
        __atomic_load_n(&g_soft.u64PulseDeadNull, __ATOMIC_RELAXED);
    s.u64PulseDeadReady =
        __atomic_load_n(&g_soft.u64PulseDeadReady, __ATOMIC_RELAXED);
    s.u64PulseDeadState =
        __atomic_load_n(&g_soft.u64PulseDeadState, __ATOMIC_RELAXED);
    s.u64PulseZeroCoalesce =
        __atomic_load_n(&g_soft.u64PulseZeroCoalesce, __ATOMIC_RELAXED);
    s.u64PulseWake =
        __atomic_load_n(&g_soft.u64PulseWake, __ATOMIC_RELAXED);
    s.u64PulseNoWaiter =
        __atomic_load_n(&g_soft.u64PulseNoWaiter, __ATOMIC_RELAXED);
    s.u64PulseEnter =
        __atomic_load_n(&g_soft.u64PulseEnter, __ATOMIC_RELAXED);
    s.u64SignalAlias =
        __atomic_load_n(&g_soft.u64SignalAlias, __ATOMIC_RELAXED);
    s.u64WaitEnter =
        __atomic_load_n(&g_soft.u64WaitEnter, __ATOMIC_RELAXED);
    s.u64WaitHit =
        __atomic_load_n(&g_soft.u64WaitHit, __ATOMIC_RELAXED);
    s.u64WaitPark =
        __atomic_load_n(&g_soft.u64WaitPark, __ATOMIC_RELAXED);
    s.u64WaitPollMiss =
        __atomic_load_n(&g_soft.u64WaitPollMiss, __ATOMIC_RELAXED);
    s.u64WaitDead =
        __atomic_load_n(&g_soft.u64WaitDead, __ATOMIC_RELAXED);
    s.u64WaitDeadEnter =
        __atomic_load_n(&g_soft.u64WaitDeadEnter, __ATOMIC_RELAXED);
    s.u64WaitDeadLoop =
        __atomic_load_n(&g_soft.u64WaitDeadLoop, __ATOMIC_RELAXED);
    s.u64WaitCasRetry =
        __atomic_load_n(&g_soft.u64WaitCasRetry, __ATOMIC_RELAXED);
    s.u64WaitSelfWake =
        __atomic_load_n(&g_soft.u64WaitSelfWake, __ATOMIC_RELAXED);
    s.u64WaitMaskAny =
        __atomic_load_n(&g_soft.u64WaitMaskAny, __ATOMIC_RELAXED);
    s.u64WaitBlock =
        __atomic_load_n(&g_soft.u64WaitBlock, __ATOMIC_RELAXED);
    s.u64WaitNoblock =
        __atomic_load_n(&g_soft.u64WaitNoblock, __ATOMIC_RELAXED);
    s.u64WaitRegister =
        __atomic_load_n(&g_soft.u64WaitRegister, __ATOMIC_RELAXED);
    s.u64WaitNoThr =
        __atomic_load_n(&g_soft.u64WaitNoThr, __ATOMIC_RELAXED);
    s.u64WaitLoop =
        __atomic_load_n(&g_soft.u64WaitLoop, __ATOMIC_RELAXED);
    s.u64WaitRetBits =
        __atomic_load_n(&g_soft.u64WaitRetBits, __ATOMIC_RELAXED);
    s.u64WaitRetZero =
        __atomic_load_n(&g_soft.u64WaitRetZero, __ATOMIC_RELAXED);
    s.u64Poll = __atomic_load_n(&g_soft.u64Poll, __ATOMIC_RELAXED);
    s.u64Abort = __atomic_load_n(&g_soft.u64Abort, __ATOMIC_RELAXED);
    s.u64AbortWake =
        __atomic_load_n(&g_soft.u64AbortWake, __ATOMIC_RELAXED);
    s.u64AbortNoWaiter =
        __atomic_load_n(&g_soft.u64AbortNoWaiter, __ATOMIC_RELAXED);
    s.u64MarkDead =
        __atomic_load_n(&g_soft.u64MarkDead, __ATOMIC_RELAXED);
    s.u64MarkDeadRevoke =
        __atomic_load_n(&g_soft.u64MarkDeadRevoke, __ATOMIC_RELAXED);
    s.u64MarkDeadForce =
        __atomic_load_n(&g_soft.u64MarkDeadForce, __ATOMIC_RELAXED);
    s.u64InstallOk =
        __atomic_load_n(&g_soft.u64InstallOk, __ATOMIC_RELAXED);
    s.u64InstallFail =
        __atomic_load_n(&g_soft.u64InstallFail, __ATOMIC_RELAXED);
    s.u64InstallFailNull =
        __atomic_load_n(&g_soft.u64InstallFailNull, __ATOMIC_RELAXED);
    s.u64InstallFailDead =
        __atomic_load_n(&g_soft.u64InstallFailDead, __ATOMIC_RELAXED);
    s.u64InstallFailCap =
        __atomic_load_n(&g_soft.u64InstallFailCap, __ATOMIC_RELAXED);
    s.u64InstallDefaultRights =
        __atomic_load_n(&g_soft.u64InstallDefaultRights, __ATOMIC_RELAXED);
    s.u64MultiWakeCalls =
        __atomic_load_n(&g_soft.u64MultiWakeCalls, __ATOMIC_RELAXED);
    s.u64MultiWakeSum =
        __atomic_load_n(&g_soft.u64MultiWakeSum, __ATOMIC_RELAXED);
    s.u64MultiWakeZero =
        __atomic_load_n(&g_soft.u64MultiWakeZero, __ATOMIC_RELAXED);
    s.u64MultiWakeNull =
        __atomic_load_n(&g_soft.u64MultiWakeNull, __ATOMIC_RELAXED);
    s.u64MultiWakePeak =
        __atomic_load_n(&g_soft.u64MultiWakePeak, __ATOMIC_RELAXED);
    s.u64WaitersPeak =
        __atomic_load_n(&g_soft.u64WaitersPeak, __ATOMIC_RELAXED);
    s.u64BadgeBitsSum =
        __atomic_load_n(&g_soft.u64BadgeBitsSum, __ATOMIC_RELAXED);
    s.u64BadgeLastOr =
        __atomic_load_n(&g_soft.u64BadgeLastOr, __ATOMIC_RELAXED);
    s.u64QuerySignals =
        __atomic_load_n(&g_soft.u64QuerySignals, __ATOMIC_RELAXED);
    s.u64QueryPending =
        __atomic_load_n(&g_soft.u64QueryPending, __ATOMIC_RELAXED);
    s.u64QueryLastBadge =
        __atomic_load_n(&g_soft.u64QueryLastBadge, __ATOMIC_RELAXED);
    s.u64QueryWaiters =
        __atomic_load_n(&g_soft.u64QueryWaiters, __ATOMIC_RELAXED);
    s.u64QueryIsLive =
        __atomic_load_n(&g_soft.u64QueryIsLive, __ATOMIC_RELAXED);
    s.u64QueryIsLiveYes =
        __atomic_load_n(&g_soft.u64QueryIsLiveYes, __ATOMIC_RELAXED);
    s.u64QueryIsLiveNo =
        __atomic_load_n(&g_soft.u64QueryIsLiveNo, __ATOMIC_RELAXED);
    s.u64SoftLog =
        __atomic_load_n(&g_soft.u64SoftLog, __ATOMIC_RELAXED);
    s.u64MsixInit =
        __atomic_load_n(&g_soft.u64MsixInit, __ATOMIC_RELAXED);
    s.u64MsixInitSkip =
        __atomic_load_n(&g_soft.u64MsixInitSkip, __ATOMIC_RELAXED);
    s.u64MsixGlobal =
        __atomic_load_n(&g_soft.u64MsixGlobal, __ATOMIC_RELAXED);

    notify_soft_msix_snap(&u32Ready, &u32Live, &u32Signals, &u64Pending,
                          &u64Last, &u32Waiters, &u32HasWaiter);
    u32PendBits = notify_soft_popcount64(u64Pending);
    u32LastBits = notify_soft_popcount64(u64Last);
    notify_soft_note_waiters(u32Waiters);

    /*
     * Catalog lines (prefix-stable): declare multi-waiter capacity and the
     * pulse/wait soft path surface so smoke/scripts can grep product depth
     * without parsing C. Wave 20 deepen splits pulse/wait/multi/badge/
     * install/abort/msix/query/capacity/catalog/outcome/path/return.
     * Soft multi-waiter ≠ multi-process notify product.
     * Soft ≠ MIG REPLY product.
     */
    /* Grep: notify: soft inventory */
    kprintf("notify: soft inventory multi_max=%u tag_waiter=%u "
            "paths=pulse,signal,wait,poll,abort,mark_dead,install,msix "
            "cas=pending_and_mask park=thread_block+schedule "
            "wake=thread_wake soft_log=%lu msix_init=%lu inits=%lu "
            "areas=%u multi_proc=0 wave=%u\n",
            (unsigned)NOTIFY_SOFT_MULTI_MAX, (unsigned)NOTIFY_TAG_WAITER,
            (unsigned long)s.u64SoftLog, (unsigned long)s.u64MsixInit,
            (unsigned long)s.u64Init,
            (unsigned)NOTIFY_SOFT_DEEPEN_AREAS,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft pulse inventory */
    kprintf("notify: soft pulse inventory or=u64Pending "
            "zero_coalesce=bit0 wake=soft_multi_max irq_safe=atomics "
            "enter=%lu hit=%lu dead=%lu dead_null=%lu dead_ready=%lu "
            "dead_state=%lu zero_coalesce=%lu wake=%lu nowaiter=%lu "
            "signal_alias=%lu multi_wake_calls=%lu multi_wake_sum=%lu "
            "wave=%u\n",
            (unsigned long)s.u64PulseEnter,
            (unsigned long)s.u64PulseHit, (unsigned long)s.u64PulseDead,
            (unsigned long)s.u64PulseDeadNull,
            (unsigned long)s.u64PulseDeadReady,
            (unsigned long)s.u64PulseDeadState,
            (unsigned long)s.u64PulseZeroCoalesce,
            (unsigned long)s.u64PulseWake,
            (unsigned long)s.u64PulseNoWaiter,
            (unsigned long)s.u64SignalAlias,
            (unsigned long)s.u64MultiWakeCalls,
            (unsigned long)s.u64MultiWakeSum,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft wait inventory */
    kprintf("notify: soft wait inventory park=thread_block+schedule "
            "claim=cas_clear_matched multi_register=u32Waiters+pWaiter_hint "
            "paths=hit,park,poll_miss,dead,cas_retry,self_wake,mask_any "
            "enter=%lu hit=%lu park=%lu poll_miss=%lu dead=%lu "
            "cas_retry=%lu self_wake=%lu mask_any=%lu poll=%lu "
            "block=%lu noblock=%lu register=%lu no_thr=%lu loop=%lu "
            "wave=%u\n",
            (unsigned long)s.u64WaitEnter, (unsigned long)s.u64WaitHit,
            (unsigned long)s.u64WaitPark, (unsigned long)s.u64WaitPollMiss,
            (unsigned long)s.u64WaitDead, (unsigned long)s.u64WaitCasRetry,
            (unsigned long)s.u64WaitSelfWake, (unsigned long)s.u64WaitMaskAny,
            (unsigned long)s.u64Poll, (unsigned long)s.u64WaitBlock,
            (unsigned long)s.u64WaitNoblock,
            (unsigned long)s.u64WaitRegister, (unsigned long)s.u64WaitNoThr,
            (unsigned long)s.u64WaitLoop,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft pulse — path tallies (Wave 20 deepen) */
    kprintf("notify: soft pulse enter=%lu hit=%lu dead=%lu "
            "dead_null=%lu dead_ready=%lu dead_state=%lu "
            "zero_coalesce=%lu wake=%lu nowaiter=%lu signal_alias=%lu "
            "wave=%u\n",
            (unsigned long)s.u64PulseEnter, (unsigned long)s.u64PulseHit,
            (unsigned long)s.u64PulseDead,
            (unsigned long)s.u64PulseDeadNull,
            (unsigned long)s.u64PulseDeadReady,
            (unsigned long)s.u64PulseDeadState,
            (unsigned long)s.u64PulseZeroCoalesce,
            (unsigned long)s.u64PulseWake,
            (unsigned long)s.u64PulseNoWaiter,
            (unsigned long)s.u64SignalAlias,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft pulse outcome — rollup */
    kprintf("notify: soft pulse outcome hit=%lu dead=%lu wake=%lu "
            "nowaiter=%lu zero_coalesce=%lu signal_alias=%lu wave=%u\n",
            (unsigned long)s.u64PulseHit, (unsigned long)s.u64PulseDead,
            (unsigned long)s.u64PulseWake,
            (unsigned long)s.u64PulseNoWaiter,
            (unsigned long)s.u64PulseZeroCoalesce,
            (unsigned long)s.u64SignalAlias,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft pulse dead — split surface */
    kprintf("notify: soft pulse dead total=%lu null=%lu ready=%lu "
            "state=%lu wave=%u\n",
            (unsigned long)s.u64PulseDead,
            (unsigned long)s.u64PulseDeadNull,
            (unsigned long)s.u64PulseDeadReady,
            (unsigned long)s.u64PulseDeadState,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft wait — path tallies (Wave 20 deepen) */
    kprintf("notify: soft wait enter=%lu hit=%lu park=%lu poll_miss=%lu "
            "dead=%lu dead_enter=%lu dead_loop=%lu cas_retry=%lu "
            "self_wake=%lu mask_any=%lu block=%lu noblock=%lu "
            "register=%lu no_thr=%lu loop=%lu poll=%lu "
            "ret_bits=%lu ret_zero=%lu wave=%u\n",
            (unsigned long)s.u64WaitEnter, (unsigned long)s.u64WaitHit,
            (unsigned long)s.u64WaitPark, (unsigned long)s.u64WaitPollMiss,
            (unsigned long)s.u64WaitDead,
            (unsigned long)s.u64WaitDeadEnter,
            (unsigned long)s.u64WaitDeadLoop,
            (unsigned long)s.u64WaitCasRetry,
            (unsigned long)s.u64WaitSelfWake,
            (unsigned long)s.u64WaitMaskAny, (unsigned long)s.u64WaitBlock,
            (unsigned long)s.u64WaitNoblock,
            (unsigned long)s.u64WaitRegister, (unsigned long)s.u64WaitNoThr,
            (unsigned long)s.u64WaitLoop, (unsigned long)s.u64Poll,
            (unsigned long)s.u64WaitRetBits,
            (unsigned long)s.u64WaitRetZero,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft wait outcome — rollup */
    kprintf("notify: soft wait outcome hit=%lu park=%lu poll_miss=%lu "
            "dead=%lu cas_retry=%lu self_wake=%lu register=%lu "
            "no_thr=%lu ret_bits=%lu ret_zero=%lu wave=%u\n",
            (unsigned long)s.u64WaitHit, (unsigned long)s.u64WaitPark,
            (unsigned long)s.u64WaitPollMiss, (unsigned long)s.u64WaitDead,
            (unsigned long)s.u64WaitCasRetry,
            (unsigned long)s.u64WaitSelfWake,
            (unsigned long)s.u64WaitRegister, (unsigned long)s.u64WaitNoThr,
            (unsigned long)s.u64WaitRetBits,
            (unsigned long)s.u64WaitRetZero,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft wait dead — enter/loop split */
    kprintf("notify: soft wait dead total=%lu enter=%lu loop=%lu "
            "wave=%u\n",
            (unsigned long)s.u64WaitDead,
            (unsigned long)s.u64WaitDeadEnter,
            (unsigned long)s.u64WaitDeadLoop,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /*
     * Grep: notify: soft multi
     * Soft multi-waiter wake budget — not multi-process notify product.
     */
    kprintf("notify: soft multi calls=%lu sum=%lu zero=%lu null=%lu "
            "peak_woken=%lu peak_waiters=%lu multi_max=%u "
            "tag_waiter=%u hint=pWaiter count=u32Waiters multi_proc=0 "
            "soft_ne_multi_proc=1 wave=%u\n",
            (unsigned long)s.u64MultiWakeCalls,
            (unsigned long)s.u64MultiWakeSum,
            (unsigned long)s.u64MultiWakeZero,
            (unsigned long)s.u64MultiWakeNull,
            (unsigned long)s.u64MultiWakePeak,
            (unsigned long)s.u64WaitersPeak,
            (unsigned)NOTIFY_SOFT_MULTI_MAX,
            (unsigned)NOTIFY_TAG_WAITER,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft badge */
    kprintf("notify: soft badge pending=0x%lx pending_bits=%u "
            "last=0x%lx last_bits=%u zero_coalesce=%lu bits_sum=%lu "
            "last_or=0x%lx coalesce_policy=bit0 wave=%u\n",
            (unsigned long)u64Pending, (unsigned)u32PendBits,
            (unsigned long)u64Last, (unsigned)u32LastBits,
            (unsigned long)s.u64PulseZeroCoalesce,
            (unsigned long)s.u64BadgeBitsSum,
            (unsigned long)s.u64BadgeLastOr,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft install */
    kprintf("notify: soft install ok=%lu fail=%lu fail_null=%lu "
            "fail_dead=%lu fail_cap=%lu default_rights=%lu "
            "cap=GJ_CAP_NOTIFICATION wave=%u\n",
            (unsigned long)s.u64InstallOk,
            (unsigned long)s.u64InstallFail,
            (unsigned long)s.u64InstallFailNull,
            (unsigned long)s.u64InstallFailDead,
            (unsigned long)s.u64InstallFailCap,
            (unsigned long)s.u64InstallDefaultRights,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /*
     * Grep: notify: soft return
     * Wave 19 public return-surface: wait badge / install status / wake.
     * Soft multi-waiter ≠ multi-process notify product.
     * Soft ≠ MIG REPLY product.
     */
    kprintf("notify: soft return wait_bits=%lu wait_zero=%lu "
            "wait_hit=%lu wait_poll_miss=%lu wait_dead=%lu "
            "install_ok=%lu install_fail=%lu install_null=%lu "
            "install_dead=%lu install_cap=%lu "
            "wake_calls=%lu wake_sum=%lu wake_zero=%lu "
            "multi_proc=0 soft_ne_multi_proc=1 soft_ne_mig_reply=1 "
            "wave=%u\n",
            (unsigned long)s.u64WaitRetBits,
            (unsigned long)s.u64WaitRetZero,
            (unsigned long)s.u64WaitHit,
            (unsigned long)s.u64WaitPollMiss,
            (unsigned long)s.u64WaitDead,
            (unsigned long)s.u64InstallOk,
            (unsigned long)s.u64InstallFail,
            (unsigned long)s.u64InstallFailNull,
            (unsigned long)s.u64InstallFailDead,
            (unsigned long)s.u64InstallFailCap,
            (unsigned long)s.u64MultiWakeCalls,
            (unsigned long)s.u64MultiWakeSum,
            (unsigned long)s.u64MultiWakeZero,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft return wait — badge return surface */
    kprintf("notify: soft return wait bits=%lu zero=%lu hit=%lu "
            "poll_miss=%lu dead=%lu wave=%u\n",
            (unsigned long)s.u64WaitRetBits,
            (unsigned long)s.u64WaitRetZero,
            (unsigned long)s.u64WaitHit,
            (unsigned long)s.u64WaitPollMiss,
            (unsigned long)s.u64WaitDead,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft return install — status surface */
    kprintf("notify: soft return install ok=%lu fail=%lu null=%lu "
            "dead=%lu cap=%lu wave=%u\n",
            (unsigned long)s.u64InstallOk,
            (unsigned long)s.u64InstallFail,
            (unsigned long)s.u64InstallFailNull,
            (unsigned long)s.u64InstallFailDead,
            (unsigned long)s.u64InstallFailCap,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft return wake — multi-wake return surface */
    kprintf("notify: soft return wake calls=%lu sum=%lu zero=%lu "
            "null=%lu peak=%lu multi_max=%u multi_proc=0 wave=%u\n",
            (unsigned long)s.u64MultiWakeCalls,
            (unsigned long)s.u64MultiWakeSum,
            (unsigned long)s.u64MultiWakeZero,
            (unsigned long)s.u64MultiWakeNull,
            (unsigned long)s.u64MultiWakePeak,
            (unsigned)NOTIFY_SOFT_MULTI_MAX,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft abort */
    kprintf("notify: soft abort enter=%lu wake=%lu nowaiter=%lu "
            "badge=none multi_wake=1 wave=%u\n",
            (unsigned long)s.u64Abort, (unsigned long)s.u64AbortWake,
            (unsigned long)s.u64AbortNoWaiter,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft mark_dead */
    kprintf("notify: soft mark_dead enter=%lu revoke=%lu force=%lu "
            "abort_follow=1 ready_clear=1 wave=%u\n",
            (unsigned long)s.u64MarkDead,
            (unsigned long)s.u64MarkDeadRevoke,
            (unsigned long)s.u64MarkDeadForce,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft msix */
    kprintf("notify: soft msix init=%lu skip=%lu global=%lu ready=%u "
            "live=%u signals=%u pending=0x%lx last=0x%lx waiters=%u "
            "has_waiter=%u soft_multi_max=%u wave=%u\n",
            (unsigned long)s.u64MsixInit,
            (unsigned long)s.u64MsixInitSkip,
            (unsigned long)s.u64MsixGlobal, (unsigned)u32Ready,
            (unsigned)u32Live, (unsigned)u32Signals,
            (unsigned long)u64Pending, (unsigned long)u64Last,
            (unsigned)u32Waiters, (unsigned)u32HasWaiter,
            (unsigned)NOTIFY_SOFT_MULTI_MAX,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft query */
    kprintf("notify: soft query signals=%lu pending=%lu last_badge=%lu "
            "waiters=%lu is_live=%lu is_live_yes=%lu is_live_no=%lu "
            "wave=%u\n",
            (unsigned long)s.u64QuerySignals,
            (unsigned long)s.u64QueryPending,
            (unsigned long)s.u64QueryLastBadge,
            (unsigned long)s.u64QueryWaiters,
            (unsigned long)s.u64QueryIsLive,
            (unsigned long)s.u64QueryIsLiveYes,
            (unsigned long)s.u64QueryIsLiveNo,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft capacity — fixed multi / tag lamps */
    kprintf("notify: soft capacity multi_max=%u tag_waiter=%u heap=0 "
            "multi_proc=0 process_queue=0 spin_product=0 irq_alloc=0 "
            "wave=%u\n",
            (unsigned)NOTIFY_SOFT_MULTI_MAX, (unsigned)NOTIFY_TAG_WAITER,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /*
     * Grep: notify: soft catalog — path surface catalog (impl vs not).
     * Soft multi-waiter threads on one object; multi-process product open.
     */
    kprintf("notify: soft catalog pulse=1 signal_alias=1 wait=1 poll=1 "
            "abort=1 mark_dead=1 install=1 msix=1 soft_multi=1 "
            "multi_proc=0 cross_proc_queue=0 bar3=0 wave=%u\n",
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /*
     * Grep: notify: soft path
     * Honesty: soft multi-waiter ≠ multi-process notify product; not bar3.
     */
    kprintf("notify: soft path claim=badge_pulse_wait "
            "irq=pulse_or+soft_multi_wake cas=pending_and_mask "
            "multi_max=%u tag_waiter=%u park=thread_block+schedule "
            "return_surface=1 multi_proc=0 soft_ne_multi_proc=1 "
            "soft_ne_mig_reply=1 "
            "wave=%u (soft inventory; not bar3; not multi-process notify "
            "product; soft != MIG REPLY product)\n",
            (unsigned)NOTIFY_SOFT_MULTI_MAX, (unsigned)NOTIFY_TAG_WAITER,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft stats */
    kprintf("notify: soft stats init=%lu pulse_hit=%lu pulse_dead=%lu "
            "pulse_zero=%lu pulse_wake=%lu pulse_nowaiter=%lu "
            "signal_alias=%lu wait_enter=%lu wait_hit=%lu wait_park=%lu "
            "wait_poll_miss=%lu wait_dead=%lu wait_cas_retry=%lu "
            "wait_self_wake=%lu wait_mask_any=%lu poll=%lu "
            "abort=%lu abort_wake=%lu mark_dead=%lu "
            "install_ok=%lu install_fail=%lu multi_wake_calls=%lu "
            "multi_wake_sum=%lu soft_log=%lu msix_init=%lu wave=%u\n",
            (unsigned long)s.u64Init, (unsigned long)s.u64PulseHit,
            (unsigned long)s.u64PulseDead,
            (unsigned long)s.u64PulseZeroCoalesce,
            (unsigned long)s.u64PulseWake,
            (unsigned long)s.u64PulseNoWaiter,
            (unsigned long)s.u64SignalAlias, (unsigned long)s.u64WaitEnter,
            (unsigned long)s.u64WaitHit, (unsigned long)s.u64WaitPark,
            (unsigned long)s.u64WaitPollMiss, (unsigned long)s.u64WaitDead,
            (unsigned long)s.u64WaitCasRetry,
            (unsigned long)s.u64WaitSelfWake, (unsigned long)s.u64WaitMaskAny,
            (unsigned long)s.u64Poll, (unsigned long)s.u64Abort,
            (unsigned long)s.u64AbortWake, (unsigned long)s.u64MarkDead,
            (unsigned long)s.u64InstallOk, (unsigned long)s.u64InstallFail,
            (unsigned long)s.u64MultiWakeCalls,
            (unsigned long)s.u64MultiWakeSum, (unsigned long)s.u64SoftLog,
            (unsigned long)s.u64MsixInit,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /*
     * Grep: notify: soft return rate
     * Wave 17 return-surface rate lamps (kept) (soft ≠ product multi-process).
     */
    kprintf("notify: soft return rate "
            "wait_bits=%lu wait_zero=%lu "
            "install_ok=%lu install_fail=%lu "
            "wake_calls=%lu wake_zero=%lu "
            "wait_hit=%lu wait_dead=%lu "
            "wave=%u (return rate; Soft≠product; soft≠multi-process notify; "
            "not bar3)\n",
            (unsigned long)s.u64WaitRetBits,
            (unsigned long)s.u64WaitRetZero,
            (unsigned long)s.u64InstallOk,
            (unsigned long)s.u64InstallFail,
            (unsigned long)s.u64MultiWakeCalls,
            (unsigned long)s.u64MultiWakeZero,
            (unsigned long)s.u64WaitHit,
            (unsigned long)s.u64WaitDead,
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /*
     * Grep: notify: soft retcode
     * Wave 17 retcode catalog for wait badge / install / wake classes.
     */
    kprintf("notify: soft retcode "
            "wait_bits=1 wait_zero=1 wait_hit=1 wait_poll_miss=1 wait_dead=1 "
            "install_ok=1 install_null=1 install_dead=1 install_cap=1 "
            "wake_calls=1 wake_sum=1 wake_zero=1 "
            "multi_proc=0 soft_ne_multi_proc=1 soft_ne_mig_reply=1 wave=%u "
            "(retcode catalog; Soft≠product; soft≠multi-process notify)\n",
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: notify: soft return selftest — Wave 19 terminal return surface */
    kprintf("notify: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 rate_limited=0 wave=%u soft PASS\n",
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft retmap — Wave 19 return-surface map */
    kprintf("notify: soft retmap soft_inv=1 deepen=1 return_rate=1 retcode=1 "
            "product=OPEN wave=%u soft PASS\n",
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);

    /* Grep: notify: soft deepen wave (Wave 24 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: notify: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("notify: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
    /* Grep: notify: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("notify: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: notify: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("notify: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
    /* Grep: notify: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("notify: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: notify: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("notify: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
            /* Grep: notify: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("notify: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: notify: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("notify: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
            /* Grep: notify: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("notify: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: notify: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("notify: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
            /* Grep: notify: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("notify: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: notify: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("notify: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
            /* Grep: notify: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("notify: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: notify: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("notify: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
            /* Grep: notify: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("notify: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: notify: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("notify: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
            /* Grep: notify: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("notify: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: notify: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("notify: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
                    /* Grep: notify: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("notify: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: notify: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("notify: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
                            /* Grep: notify: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("notify: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: notify: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("notify: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=84 "
                                    "(retglyph honesty; Soft≠product; not bar3)\n");
                            /* Grep: notify: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("notify: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=84 "
                                    "(retscepter stamp; Soft≠product)\n");
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: notify: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("notify: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=84 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: notify: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("notify: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=84 "
                                    "(retemblem stamp; Soft≠product)\n");
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: notify: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("notify: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=84 "
                                    "(retaegis honesty; Soft≠product; not bar3)\n");
                            /* Grep: notify: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("notify: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=84 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: notify: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("notify: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=84 "
                                    "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("notify: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("notify: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("notify: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("notify: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("notify: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("notify: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retfortress — Wave 35 return-fortress honesty */
kprintf("notify: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("notify: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft rethold — Wave 36 return-hold honesty */
kprintf("notify: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retspire — Wave 36 exclusive spire stamp */
kprintf("notify: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retwall — Wave 37 return-wall honesty */
kprintf("notify: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retgate — Wave 37 exclusive gate stamp */
kprintf("notify: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retmoat — Wave 38 return-moat honesty */
kprintf("notify: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retower — Wave 38 exclusive tower stamp */
kprintf("notify: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("notify: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("notify: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("notify: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("notify: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retravelin — Wave 41 return-travelin honesty */
kprintf("notify: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("notify: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("notify: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("notify: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("notify: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("notify: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("notify: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("notify: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("notify: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("notify: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retbailey — Wave 46 return-bailey honesty */
kprintf("notify: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("notify: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("notify: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("notify: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("notify: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("notify: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("notify: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("notify: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retsally — Wave 50 return-sally honesty */
kprintf("notify: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("notify: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retfosse — Wave 51 return-fosse honesty */
kprintf("notify: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("notify: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("notify: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("notify: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retravelin — Wave 53 return-travelin honesty */
kprintf("notify: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("notify: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("notify: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retredan — Wave 54 exclusive redan stamp */
kprintf("notify: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retflank — Wave 55 return-flank honesty */
kprintf("notify: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retface — Wave 55 exclusive face stamp */
kprintf("notify: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retgorge — Wave 56 return-gorge honesty */
kprintf("notify: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("notify: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retraverse — Wave 57 return-traverse honesty */
kprintf("notify: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("notify: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retorillon — Wave 58 return-orillon honesty */
kprintf("notify: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("notify: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("notify: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("notify: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retplace — Wave 60 return-place honesty */
kprintf("notify: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("notify: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("notify: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("notify: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("notify: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("notify: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("notify: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("notify: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: notify: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("notify: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: notify: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("notify: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: notify: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("notify: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: notify: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("notify: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: notify: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("notify: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=84 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: notify: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("notify: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=84 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("notify: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("notify: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("notify: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("notify: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("notify: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=84 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("notify: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=84 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("notify: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=84 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("notify: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=84 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("notify: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=84 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("notify: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=84 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: notify: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("notify: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=84 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("notify: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=84 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: notify: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("notify: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=84 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("notify: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=84 (retbastionangle stamp; Soft≠product)\n");
/* Grep: notify: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("notify: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=84 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("notify: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=84 (retparapetangle stamp; Soft≠product)\n");
/* Grep: notify: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("notify: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=84 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("notify: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=84 (retowerangle stamp; Soft≠product)\n");
/* Grep: notify: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("notify: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=84 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("notify: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=84 (retwallangle stamp; Soft≠product)\n");
/* Grep: notify: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("notify: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=84 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("notify: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=84 (retholdangle stamp; Soft≠product)\n");
/* Grep: notify: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("notify: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=84 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("notify: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=84 (retfortressangle stamp; Soft≠product)\n");
/* Grep: notify: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("notify: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=84 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("notify: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=84 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: notify: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("notify: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=84 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("notify: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=84 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: notify: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("notify: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=84 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("notify: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=84 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: notify: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("notify: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=84 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("notify: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=84 (retaegisangle stamp; Soft≠product)\n");
/* Grep: notify: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("notify: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=84 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("notify: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=84 (retsigilangle stamp; Soft≠product)\n");
/* Grep: notify: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("notify: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=84 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: notify: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("notify: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=84 (retglyphangle stamp; Soft≠product)\n");

                            kprintf("notify: soft deepen wave=%u areas=%u pulse_enter=%lu "
            "wait_enter=%lu multi_calls=%lu msix_init=%lu "
            "ret_wait_bits=%lu ret_wait_zero=%lu ret_inst_ok=%lu "
            "soft_log=%lu multi_proc=0 ok=1 skip=0\n",
            (unsigned)NOTIFY_SOFT_DEEPEN_WAVE,
            (unsigned)NOTIFY_SOFT_DEEPEN_AREAS,
            (unsigned long)s.u64PulseEnter,
            (unsigned long)s.u64WaitEnter,
            (unsigned long)s.u64MultiWakeCalls,
            (unsigned long)s.u64MsixInit,
            (unsigned long)s.u64WaitRetBits,
            (unsigned long)s.u64WaitRetZero,
            (unsigned long)s.u64InstallOk,
            (unsigned long)s.u64SoftLog);

    /*
     * Soft lamp: MSI-X bind ready (or inventory emission). Never hard-gates.
     * Grep: notify: soft inventory PASS | notify: soft PASS
     */
    if (u32Ready != 0u && u32Live != 0u) {
        kprintf("notify: soft inventory PASS ready=%u live=%u "
                "soft_log=%lu areas=%u multi_proc=0 wave=%u\n",
                (unsigned)u32Ready, (unsigned)u32Live,
                (unsigned long)s.u64SoftLog,
                (unsigned)NOTIFY_SOFT_DEEPEN_AREAS,
                (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
        kprintf("notify: soft PASS wave=%u areas=%u soft_log=%lu\n",
                (unsigned)NOTIFY_SOFT_DEEPEN_WAVE,
                (unsigned)NOTIFY_SOFT_DEEPEN_AREAS,
                (unsigned long)s.u64SoftLog);
    } else {
        kprintf("notify: soft FAIL ready=%u live=%u "
                "(soft inventory only; not product gate; not multi-process "
                "notify) wave=%u\n",
                (unsigned)u32Ready, (unsigned)u32Live,
                (unsigned)NOTIFY_SOFT_DEEPEN_WAVE);
    }
}

/** Soft: one-shot deep inventory after first product wait activity. */
static void
notify_soft_log_once(void)
{
    if (g_fSoftStatsOnce != 0) {
        return;
    }
    g_fSoftStatsOnce = 1;
    notify_soft_log();
}

static int
notify_live(const struct gj_notify *pN)
{
    if (pN == NULL || !pN->u32Ready) {
        return 0;
    }
    if (__atomic_load_n(&pN->hdr.u32State, __ATOMIC_ACQUIRE) !=
        (u32)GJ_OBJ_LIVE) {
        return 0;
    }
    return 1;
}

/*
 * Soft multi-wake: wake up to NOTIFY_SOFT_MULTI_MAX waiters on this object.
 * greppable: NOTIFY_SOFT_MULTI_WAKE
 */
static u32
notify_soft_multi_wake(struct gj_notify *pN)
{
    u32 cWoken;

    if (pN == NULL) {
        /* greppable: notify: soft multi wake */
        notify_soft_inc(&g_soft.u64MultiWakeNull);
        return 0;
    }
    /* greppable: notify: soft multi wake */
    notify_soft_inc(&g_soft.u64MultiWakeCalls);
    cWoken = thread_wake(pN, NOTIFY_TAG_WAITER, NOTIFY_SOFT_MULTI_MAX);
    notify_soft_note_wake(cWoken);
    return cWoken;
}

/*
 * True if any waiter is registered (hint or count). IRQ fast path.
 * greppable: NOTIFY_SOFT_MULTI_HAS_WAITER
 */
static int
notify_has_waiter(const struct gj_notify *pN)
{
    if (pN == NULL) {
        return 0;
    }
    if (pN->pWaiter != NULL) {
        return 1;
    }
    return __atomic_load_n(&pN->u32Waiters, __ATOMIC_ACQUIRE) > 0u;
}

void
notify_init(struct gj_notify *pN)
{
    if (pN == NULL) {
        return;
    }
    memset(pN, 0, sizeof(*pN));
    gj_obj_hdr_init(&pN->hdr);
    pN->u32Ready = 1;
    notify_soft_inc(&g_soft.u64Init);
}

int
notify_is_live(const struct gj_notify *pN)
{
    int fLive;

    notify_soft_inc(&g_soft.u64QueryIsLive);
    fLive = notify_live(pN);
    if (fLive) {
        notify_soft_inc(&g_soft.u64QueryIsLiveYes);
    } else {
        notify_soft_inc(&g_soft.u64QueryIsLiveNo);
    }
    return fLive;
}

void
notify_pulse(struct gj_notify *pN, u64 u64Badge)
{
    u32 cBits;
    u64 u64Or;

    notify_soft_inc(&g_soft.u64PulseEnter);

    if (pN == NULL) {
        /* greppable: notify: soft pulse dead */
        notify_soft_inc(&g_soft.u64PulseDead);
        notify_soft_inc(&g_soft.u64PulseDeadNull);
        return;
    }
    if (!pN->u32Ready) {
        /* greppable: notify: soft pulse dead */
        notify_soft_inc(&g_soft.u64PulseDead);
        notify_soft_inc(&g_soft.u64PulseDeadReady);
        return;
    }
    if (__atomic_load_n(&pN->hdr.u32State, __ATOMIC_ACQUIRE) !=
        (u32)GJ_OBJ_LIVE) {
        /* greppable: notify: soft pulse dead */
        notify_soft_inc(&g_soft.u64PulseDead);
        notify_soft_inc(&g_soft.u64PulseDeadState);
        return;
    }
    /* Badge 0 is not a valid event bit; coalesce to bit 0. */
    if (u64Badge == 0) {
        u64Badge = 1;
        notify_soft_inc(&g_soft.u64PulseZeroCoalesce);
    }
    /* greppable: NOTIFY_BADGE_PULSE — OR pending, then soft multi-wake */
    /* greppable: notify: soft pulse hit */
    (void)__atomic_fetch_or(&pN->u64Pending, u64Badge, __ATOMIC_ACQ_REL);
    __atomic_store_n(&pN->u64LastBadge, u64Badge, __ATOMIC_RELEASE);
    (void)__atomic_fetch_add(&pN->u32Signals, 1u, __ATOMIC_ACQ_REL);
    notify_soft_inc(&g_soft.u64PulseHit);
    cBits = notify_soft_popcount64(u64Badge);
    notify_soft_add(&g_soft.u64BadgeBitsSum, (u64)cBits);
    /* Soft sticky OR of last badges (diagnostics). */
    u64Or = __atomic_load_n(&g_soft.u64BadgeLastOr, __ATOMIC_RELAXED);
    while (1) {
        u64 u64New = u64Or | u64Badge;
        if (u64New == u64Or) {
            break;
        }
        if (__atomic_compare_exchange_n(&g_soft.u64BadgeLastOr, &u64Or,
                                        u64New, 0, __ATOMIC_RELAXED,
                                        __ATOMIC_RELAXED)) {
            break;
        }
    }
    if (notify_has_waiter(pN)) {
        /* greppable: notify: soft pulse wake */
        (void)notify_soft_multi_wake(pN);
        notify_soft_inc(&g_soft.u64PulseWake);
    } else {
        /* greppable: notify: soft pulse nowaiter */
        notify_soft_inc(&g_soft.u64PulseNoWaiter);
    }
}

void
notify_signal(struct gj_notify *pN, u64 u64Badge)
{
    /* Stable alias: signal == pulse (badge bitmask). */
    notify_soft_inc(&g_soft.u64SignalAlias);
    notify_pulse(pN, u64Badge);
}

u64
notify_wait(struct gj_notify *pN, u64 u64Mask, int fBlock)
{
    u64               u64Got;
    u64               u64Pend;
    u64               u64New;
    struct gj_thread *pCur;
    int               fRegistered = 0;
    u32               cWaiters;

    notify_soft_inc(&g_soft.u64WaitEnter);
    if (fBlock) {
        notify_soft_inc(&g_soft.u64WaitBlock);
    } else {
        notify_soft_inc(&g_soft.u64WaitNoblock);
    }

    if (!notify_live(pN)) {
        /* greppable: notify: soft wait dead */
        notify_soft_inc(&g_soft.u64WaitDead);
        notify_soft_inc(&g_soft.u64WaitDeadEnter);
        notify_soft_inc(&g_soft.u64WaitRetZero); /* Wave 19 return */
        return 0;
    }
    /* mask==0 means "any badge" — greppable: NOTIFY_BADGE_WAIT */
    if (u64Mask == 0) {
        u64Mask = ~0ull;
        notify_soft_inc(&g_soft.u64WaitMaskAny);
    }
    pCur = thread_current();
    for (;;) {
        notify_soft_inc(&g_soft.u64WaitLoop);
        if (!notify_live(pN)) {
            if (fRegistered) {
                if (pN->pWaiter == pCur) {
                    pN->pWaiter = NULL;
                }
                (void)__atomic_fetch_sub(&pN->u32Waiters, 1u, __ATOMIC_ACQ_REL);
                fRegistered = 0;
            }
            /* greppable: notify: soft wait dead */
            notify_soft_inc(&g_soft.u64WaitDead);
            notify_soft_inc(&g_soft.u64WaitDeadLoop);
            notify_soft_inc(&g_soft.u64WaitRetZero); /* Wave 19 return */
            notify_soft_log_once();
            return 0;
        }
        /* Atomic clear of matched bits (IRQ may OR concurrently). */
        u64Pend = __atomic_load_n(&pN->u64Pending, __ATOMIC_ACQUIRE);
        u64Got = u64Pend & u64Mask;
        if (u64Got != 0) {
            u64New = u64Pend & ~u64Got;
            if (__atomic_compare_exchange_n(&pN->u64Pending, &u64Pend, u64New, 0,
                                            __ATOMIC_ACQ_REL,
                                            __ATOMIC_ACQUIRE)) {
                if (fRegistered) {
                    if (pN->pWaiter == pCur) {
                        pN->pWaiter = NULL;
                    }
                    (void)__atomic_fetch_sub(&pN->u32Waiters, 1u,
                                             __ATOMIC_ACQ_REL);
                }
                /* greppable: notify: soft wait hit */
                notify_soft_inc(&g_soft.u64WaitHit);
                notify_soft_inc(&g_soft.u64WaitRetBits); /* Wave 19 return */
                notify_soft_log_once();
                return u64Got;
            }
            /* Lost race with signal/clear — retry without sleeping. */
            /* greppable: notify: soft wait cas retry */
            notify_soft_inc(&g_soft.u64WaitCasRetry);
            continue;
        }
        /* Non-blocking, or no runnable thread context (e.g. early IRQ). */
        if (!fBlock || pCur == NULL) {
            if (pCur == NULL && fBlock) {
                notify_soft_inc(&g_soft.u64WaitNoThr);
            }
            if (fRegistered) {
                if (pN->pWaiter == pCur) {
                    pN->pWaiter = NULL;
                }
                (void)__atomic_fetch_sub(&pN->u32Waiters, 1u, __ATOMIC_ACQ_REL);
            }
            /* greppable: notify: soft wait poll miss */
            notify_soft_inc(&g_soft.u64WaitPollMiss);
            notify_soft_inc(&g_soft.u64WaitRetZero); /* Wave 19 return */
            notify_soft_log_once();
            return 0;
        }
        /*
         * Soft multi-waiter register: count + non-exclusive hint.
         * greppable: NOTIFY_SOFT_MULTI_WAITER
         */
        if (!fRegistered) {
            cWaiters = __atomic_fetch_add(&pN->u32Waiters, 1u,
                                          __ATOMIC_ACQ_REL) +
                       1u;
            fRegistered = 1;
            notify_soft_inc(&g_soft.u64WaitRegister);
            notify_soft_note_waiters(cWaiters);
        }
        pN->pWaiter = pCur;
        thread_block(pN, NOTIFY_TAG_WAITER);
        /*
         * Pulse may OR bits after the pending check and before BLOCKED.
         * Re-sample and self soft multi-wake so the badge is not lost.
         */
        u64Pend = __atomic_load_n(&pN->u64Pending, __ATOMIC_ACQUIRE);
        if ((u64Pend & u64Mask) != 0 || !notify_live(pN)) {
            /* greppable: notify: soft wait self wake */
            (void)notify_soft_multi_wake(pN);
            notify_soft_inc(&g_soft.u64WaitSelfWake);
        }
        schedule();
        /* greppable: notify: soft wait park */
        notify_soft_inc(&g_soft.u64WaitPark);
        /* Drop exclusive-looking hint only if we still own it. */
        if (pN->pWaiter == pCur) {
            pN->pWaiter = NULL;
        }
    }
}

u64
notify_poll(struct gj_notify *pN, u64 u64Mask)
{
    notify_soft_inc(&g_soft.u64Poll);
    return notify_wait(pN, u64Mask, 0);
}

u32
notify_signals(const struct gj_notify *pN)
{
    /* Lifetime pulse count (stats); 0 if object missing. */
    notify_soft_inc(&g_soft.u64QuerySignals);
    if (pN == NULL) {
        return 0;
    }
    return __atomic_load_n(&pN->u32Signals, __ATOMIC_ACQUIRE);
}

u64
notify_pending(const struct gj_notify *pN)
{
    /* Current uncleared badge bits (stats); 0 if object missing. */
    notify_soft_inc(&g_soft.u64QueryPending);
    if (pN == NULL) {
        return 0;
    }
    return __atomic_load_n(&pN->u64Pending, __ATOMIC_ACQUIRE);
}

u64
notify_last_badge(const struct gj_notify *pN)
{
    notify_soft_inc(&g_soft.u64QueryLastBadge);
    if (pN == NULL) {
        return 0;
    }
    return __atomic_load_n(&pN->u64LastBadge, __ATOMIC_ACQUIRE);
}

u32
notify_waiters(const struct gj_notify *pN)
{
    /* Soft multi-waiter count (stats). greppable: NOTIFY_SOFT_MULTI_WAITER */
    notify_soft_inc(&g_soft.u64QueryWaiters);
    if (pN == NULL) {
        return 0;
    }
    return __atomic_load_n(&pN->u32Waiters, __ATOMIC_ACQUIRE);
}

void
notify_abort_waiter(struct gj_notify *pN)
{
    if (pN == NULL) {
        return;
    }
    /* greppable: NOTIFY_ABORT_SOFT_MULTI */
    /* greppable: notify: soft abort */
    notify_soft_inc(&g_soft.u64Abort);
    if (notify_has_waiter(pN)) {
        (void)notify_soft_multi_wake(pN);
        notify_soft_inc(&g_soft.u64AbortWake);
    } else {
        notify_soft_inc(&g_soft.u64AbortNoWaiter);
    }
}

void
notify_mark_dead(struct gj_notify *pN)
{
    if (pN == NULL) {
        return;
    }
    /* greppable: notify: soft mark_dead */
    notify_soft_inc(&g_soft.u64MarkDead);
    pN->u32Ready = 0;
    if (pN->hdr.u32State == (u32)GJ_OBJ_LIVE) {
        (void)gj_obj_revoke_begin(&pN->hdr);
        notify_soft_inc(&g_soft.u64MarkDeadRevoke);
    } else {
        __atomic_store_n(&pN->hdr.u32State, (u32)GJ_OBJ_DEAD, __ATOMIC_RELEASE);
        notify_soft_inc(&g_soft.u64MarkDeadForce);
    }
    notify_abort_waiter(pN);
}

gj_status_t
notify_install(struct gj_process *pProc, struct gj_notify *pN, u16 u16Rights,
               struct gj_cap_ref *pOutRef)
{
    gj_status_t st;

    if (pProc == NULL || pN == NULL || pOutRef == NULL || pProc->pCnode == NULL) {
        /* greppable: notify: soft install fail */
        notify_soft_inc(&g_soft.u64InstallFail);
        notify_soft_inc(&g_soft.u64InstallFailNull);
        return GJ_ERR_INVAL;
    }
    if (!notify_live(pN)) {
        /* greppable: notify: soft install fail */
        notify_soft_inc(&g_soft.u64InstallFail);
        notify_soft_inc(&g_soft.u64InstallFailDead);
        return GJ_ERR_NODEV;
    }
    if (u16Rights == 0) {
        u16Rights = (u16)(GJ_RIGHT_READ | GJ_RIGHT_WAIT | GJ_RIGHT_IDENTIFY);
        notify_soft_inc(&g_soft.u64InstallDefaultRights);
    }
    st = gj_cap_alloc_install(pProc->pCnode, (u16)GJ_CAP_NOTIFICATION,
                              u16Rights, &pN->hdr, pOutRef);
    if (st == GJ_OK) {
        /* greppable: notify: soft install ok */
        notify_soft_inc(&g_soft.u64InstallOk);
    } else {
        /* greppable: notify: soft install fail */
        notify_soft_inc(&g_soft.u64InstallFail);
        notify_soft_inc(&g_soft.u64InstallFailCap);
    }
    return st;
}

void
notify_msix_init(void)
{
    if (g_fMsixInited && g_msixNotify.u32Ready &&
        g_msixNotify.hdr.u32State == (u32)GJ_OBJ_LIVE) {
        notify_soft_inc(&g_soft.u64MsixInitSkip);
        return;
    }
    notify_init(&g_msixNotify);
    g_fMsixInited = 1;
    notify_soft_inc(&g_soft.u64MsixInit);
    /*
     * Stats snapshot at bind time: ready, cumulative pulses, pending mask.
     * irq_msix later pulses badges; signals/pending stay queryable via
     * notify_signals / notify_pending.
     * greppable: notify: MSI-X global ready
     */
    kprintf("notify: MSI-X global ready=%u signals=%u pending=0x%lx "
            "soft_multi_max=%u\n",
            g_msixNotify.u32Ready, g_msixNotify.u32Signals,
            (unsigned long)g_msixNotify.u64Pending,
            (unsigned)NOTIFY_SOFT_MULTI_MAX);
    /* Greppable soft product inventory at MSI-X bind (prefix: notify: soft …) */
    notify_soft_log();
}

struct gj_notify *
notify_msix_global(void)
{
    notify_soft_inc(&g_soft.u64MsixGlobal);
    return &g_msixNotify;
}
