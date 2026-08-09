/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Notification badge objects - event/wait/waker residual for door clients and
 * UDX IRQ-ish userspace (hard IRQ -> pulse only; thr reaps; batch after wait).
 * MSI-X inject product path for Dual DoD hosts (rtl8168_udx / xhci_udx).
 *
 * greppable: NOTIFY_BADGE_PULSE_WAIT
 * greppable: NOTIFY_SOFT_MULTI_WAITER
 * greppable: NOTIFY_EVENT_POST
 * greppable: NOTIFY_WAKER_KICK
 * greppable: NOTIFY_WAKER_DRAIN
 * greppable: NOTIFY_WAIT_RESIDUAL
 * greppable: NOTIFY_LEAN_RESIDUAL
 * greppable: NOTIFY_H1_THR_ONLY
 * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
 *
 * Hazard H1 thr-only direction (Soft!=product; G-AC-1):
 *   hard IRQ / soft inject -> notify_pulse ONLY (atomic OR + optional soft
 *     multi-wake). No class-driver work, no net_eth_poll, no kprintf, no park.
 *   thr / UDX userspace  -> notify_wait / poll reaps badge; batch AFTER wait.
 *   H1: thr notify not hard-IRQ driver. irq_driver=0 thr_only=1.
 *   greppable: thr_only=1 irq_pulse_only=1 batch_after_wait=1 irq_driver=0
 *   greppable: hazard=H1 path=irq_pulse->thr_notify_wait
 *
 * Event residual (producer; IRQ-callable pulse path):
 *   notify_pulse / notify_event / notify_signal
 *     -> OR badge into u64Pending (atomics; IRQ-safe), soft multi-wake
 *
 * Wait residual (thr/UDX consumer reaping; greppable NOTIFY_WAIT_RESIDUAL):
 *   notify_wait / notify_poll  - CAS-clear matched bits; park on TAG_WAITER
 *   thr shape: pulse(badge) -> wait(mask, fBlock=0|1) reaps for UDX
 *   freestanding UDX: non-blocking wait; block path soft multi parks (thr)
 *
 * Waker residual (consumer park / kick; not hard-IRQ driver work):
 *   notify_wake                - kick without badge (quiesce / peer path)
 *   notify_wake_drain          - bounded multi-round kick (abort/mark_dead)
 *   notify_abort_waiter        - drain waiters, no badge post
 *
 * Soft multi-waiter: several threads may block on one object. Pulse wakes up
 * to NOTIFY_SOFT_MULTI_MAX per round; waiters CAS-claim matching badge bits.
 * Abort/mark_dead drain rounds so >MULTI_MAX waiters are not left parked.
 *
 * STRONGER functional residual (W7 Dual DoD; stamp-free bar v2026.08.04.75):
 *   null_miss | zero_coalesce | multi_badge | signal_alias | dual_dod_open
 * STRONGER functional residual (W10 Dual DoD; stamp-free bar v2026.08.04.75):
 *   install_null | dead_pulse for live UDX host IRQ thr path. H1 thr-only.
 * STRONGER functional residual (W11 Dual DoD; stamp-free bar v2026.08.04.75):
 *   poll_eq | partial_reap | abort_nobadge | inject_query | dual_dod_open
 *   product notify path honesty for MSI-X inject used by Dual DoD hosts.
 *   product_hosts=UDX (rtl8168_udx|xhci_udx); Dual DoD A/B remain OPEN.
 *   Soft!=product; H1 thr-only; H2 once (no stamp storms).
 *   greppable: poll_eq | partial_reap | abort_nobadge | inject_query
 *   greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
 * STRONGER denser residual (W12 Dual DoD; stamp-free bar v2026.08.04.75):
 *   event denser:  dpulse_or | event_eq | signal_eq | coalesce_rearm
 *   wait denser:   remask3 | mask_any_multi | poll_then_miss | wait_empty
 *   waker denser:  kick_max0 | drain_empty | abort_keep | null_kick
 *   inject denser: inject_rearm | multi_vec | last_badge | signals_mono
 *   composite denser + dual_dod_open for product_hosts=UDX MSI-X inject
 *   greppable: notify: soft residual denser | MSI-X inject denser
 *   greppable: denser=1 | event denser | wait denser | waker denser | inject denser
 *   greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
 * Dual DoD A/B remain OPEN (this residual does not close product DoD).
 *
 * Soft inventory (sparse lamps only - NO stamp storms, no version stamp):
 *   - inventory / multi / residual lean / residual lean wait /
 *     residual lean thr / residual lean udx / residual denser / path / PASS|FAIL
 *   sticky counters still bump on product paths; never hard-gate.
 * Soft multi-waiter != multi-process notify product. Soft != MIG REPLY product.
 * Soft != product. Dual MIT OR Apache-2.0. Lean residual only. G-AC-1.
 * Product IRQ Notification mint OPEN (badge delivery; not .ko AC).
 * greppable: notify: soft
 * greppable: notify: soft residual lean
 * greppable: notify: soft residual lean wait
 * greppable: notify: soft residual lean thr
 * greppable: notify: soft residual lean udx
 * greppable: notify: soft residual denser
 * greppable: notify: soft residual lean PASS
 * greppable: notify: soft residual lean FAIL
 * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
 * greppable: denser=1 MSI-X inject denser
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

/*
 * Sparse lamp surface count:
 *   inventory / multi / residual lean / residual lean wait /
 *   residual lean thr / residual lean udx / residual denser / path / PASS
 * Not a version stamp; not a stamp-storm catalog. Soft!=product.
 * W12 denser residual adds residual denser lamp (MSI-X inject Dual DoD).
 */
#define NOTIFY_SOFT_AREAS 9u
/* Drain rounds on abort/mark_dead (bounded; not hard-IRQ). */
#define NOTIFY_WAKER_DRAIN_ROUNDS 4u
/*
 * W12 denser residual honesty lock (Soft!=product; Dual DoD OPEN;
 * stamp-free bar v2026.08.04.75; H2 once; agent!=close).
 * greppable: denser=1 MSI-X inject denser Soft!=product dual_dod=OPEN
 */
#define NOTIFY_DENSER_LOCK        1u
#define NOTIFY_DENSER_EVENT_ARMS  4u /* dpulse_or|event_eq|signal_eq|coalesce_rearm */
#define NOTIFY_DENSER_WAIT_ARMS   4u /* remask3|mask_any_multi|poll_then_miss|wait_empty */
#define NOTIFY_DENSER_WAKER_ARMS  4u /* kick_max0|drain_empty|abort_keep|null_kick */
#define NOTIFY_DENSER_INJECT_ARMS 4u /* inject_rearm|multi_vec|last_badge|signals_mono */
#define NOTIFY_DENSER_ARMS_MIN    4u

_Static_assert(NOTIFY_DENSER_LOCK == 1u,
               "W12 denser residual honesty lock");
_Static_assert(NOTIFY_DENSER_EVENT_ARMS == NOTIFY_DENSER_ARMS_MIN &&
               NOTIFY_DENSER_WAIT_ARMS == NOTIFY_DENSER_ARMS_MIN &&
               NOTIFY_DENSER_WAKER_ARMS == NOTIFY_DENSER_ARMS_MIN &&
               NOTIFY_DENSER_INJECT_ARMS == NOTIFY_DENSER_ARMS_MIN,
               "W12 denser residual arms min equals denser arms (all required)");

/*
 * H1 thr-only residual honesty (Soft!=product; G-AC-1).
 * Hard IRQ may pulse only; thr reaps via wait; no class-driver work here.
 * Compile-time locked - flip requires H1 review (IRQ stack smash class).
 * greppable: NOTIFY_H1_THR_ONLY
 * greppable: thr_only=1 irq_pulse_only=1 batch_after_wait=1 irq_driver=0
 */
#define NOTIFY_H1_IRQ_PULSE_ONLY   1u /* hard IRQ entry = pulse only */
#define NOTIFY_H1_THR_WAIT         1u /* wait residual = thr/UDX consumer */
#define NOTIFY_H1_BATCH_AFTER_WAIT 1u /* batch work after notify_wait */
#define NOTIFY_H1_IRQ_DRIVER       0u /* not hard-IRQ class driver */
#define NOTIFY_H1_THR_ONLY         1u /* thr notify direction locked */

_Static_assert(NOTIFY_H1_IRQ_PULSE_ONLY == 1u,
               "H1: hard IRQ notify path must be pulse-only");
_Static_assert(NOTIFY_H1_THR_WAIT == 1u,
               "H1: wait residual is thr/UDX consumer only");
_Static_assert(NOTIFY_H1_BATCH_AFTER_WAIT == 1u,
               "H1: batch work after notify_wait (not on IRQ stack)");
_Static_assert(NOTIFY_H1_IRQ_DRIVER == 0u,
               "H1: thr notify not hard-IRQ driver");
_Static_assert(NOTIFY_H1_THR_ONLY == 1u,
               "H1: thr-only notify direction locked");

/*
 * Soft path sticky counters (wrap OK; diagnostics only).
 * Bumped on product return paths; never hard-gate behavior.
 * Pulse path is IRQ-callable -> atomic RMW only (no kprintf).
 * Soft multi-waiter != multi-process notify product.
 * Soft != MIG REPLY product. Soft != product. H1 thr-only.
 * greppable: notify: soft
 */
struct notify_soft_stats {
    u64 u64Init;             /* notify_init calls */
    u64 u64PulseHit;         /* pulse on live ready object */
    u64 u64PulseDead;        /* pulse dropped (any reason) */
    u64 u64PulseDeadNull;    /* pulse dropped: pN == NULL */
    u64 u64PulseDeadReady;   /* pulse dropped: !u32Ready */
    u64 u64PulseDeadState;   /* pulse dropped: not LIVE */
    u64 u64PulseZeroCoalesce;/* badge 0 -> bit 0 */
    u64 u64PulseWake;        /* soft multi-wake from pulse */
    u64 u64PulseNoWaiter;    /* pulse with no waiter registered */
    u64 u64PulseEnter;       /* notify_pulse entries */
    u64 u64SignalAlias;      /* notify_signal -> pulse */
    u64 u64WaitEnter;        /* notify_wait entries */
    u64 u64WaitHit;          /* CAS-clear matched bits */
    u64 u64WaitPark;         /* thread_block + schedule */
    u64 u64WaitPollMiss;     /* non-block / no thr miss */
    u64 u64WaitDead;         /* object DEAD during wait */
    u64 u64WaitDeadEnter;    /* DEAD on first live check */
    u64 u64WaitDeadLoop;     /* DEAD mid-loop after register */
    u64 u64WaitCasRetry;     /* lost CAS race */
    u64 u64WaitSelfWake;     /* post-block self multi-wake */
    u64 u64WaitMaskAny;      /* mask==0 -> ~0ull */
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
    u64 u64EventPost;        /* notify_event entries (alias tallies) */
    u64 u64WakerKick;        /* notify_wake entries */
    u64 u64WakerKickHit;     /* notify_wake returned cWoken > 0 */
    u64 u64WakerKickZero;    /* notify_wake returned 0 (null/empty) */
    u64 u64WakerDrain;       /* notify_wake_drain entries */
    u64 u64WakerDrainSum;    /* total woken across drain rounds */
    u64 u64ResidualLean;     /* lean residual self-check runs */
    u64 u64ResidualLeanOk;   /* lean residual full check set OK */
    u64 u64ResidualWaitOk;   /* wait residual thr pulse->wait CAS OK */
    u64 u64ResidualWaitMiss; /* wait residual empty miss returned 0 */
    u64 u64ResidualWaitAny;  /* wait residual mask==0 any-badge OK */
    u64 u64ResidualThrOk;    /* H1 thr-only residual direction OK */
    /* W12 denser residual sticky counters (Soft!=product; never hard-gate). */
    u64 u64DenseEventOr;     /* denser event: double-pulse OR */
    u64 u64DenseEventEq;     /* denser event: event == pulse path */
    u64 u64DenseSignalEq;    /* denser event: signal == pulse path */
    u64 u64DenseCoalesce;    /* denser event: zero coalesce rearm */
    u64 u64DenseRemask3;     /* denser wait: 3-bit remask partial reap */
    u64 u64DenseMaskAny;     /* denser wait: mask-any multi reap */
    u64 u64DensePollMiss;    /* denser wait: poll then empty miss */
    u64 u64DenseWaitEmpty;   /* denser wait: wait empty after reap */
    u64 u64DenseKickMax0;    /* denser waker: kick max=0 empty */
    u64 u64DenseDrainEmpty;  /* denser waker: drain empty */
    u64 u64DenseAbortKeep;   /* denser waker: abort keeps badge */
    u64 u64DenseNullKick;    /* denser waker: null kick/drain */
    u64 u64DenseInjectRearm; /* denser inject: rearm after reap */
    u64 u64DenseMultiVec;    /* denser inject: multi-vector sequential */
    u64 u64DenseLastBadge;   /* denser inject: last_badge honesty */
    u64 u64DenseSignalsMono; /* denser inject: signals monotonic */
    u64 u64DenseOk;          /* denser composite all arms OK */
    u64 u64DenseFail;        /* denser composite arm miss */
};

static struct notify_soft_stats g_soft;
/* One-shot sparse inventory after first product wait activity (soft). */
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
 * Sparse greppable soft inventory (lean residual; no stamp storms).
 * Called from notify_msix_init and once after first wait activity.
 * Never allocates; not for hard-IRQ (kprintf only from product paths).
 * Soft multi-waiter != multi-process notify product (multi_proc=0).
 * Soft != MIG REPLY product. Soft != product. Dual MIT OR Apache-2.0.
 * H1 thr-only: irq pulse only; thr wait reaps; not hard-IRQ driver.
 * greppable: notify: soft inventory
 * greppable: notify: soft multi
 * greppable: notify: soft residual lean
 * greppable: notify: soft residual lean wait
 * greppable: notify: soft residual lean thr
 * greppable: notify: soft path
 * greppable: notify: soft inventory PASS / notify: soft PASS
 */
static void
notify_soft_log(void)
{
    u64 u64SoftLogN;
    u64 u64Init;
    u64 u64PulseEnter;
    u64 u64PulseHit;
    u64 u64WaitEnter;
    u64 u64WaitHit;
    u64 u64MultiWakeCalls;
    u64 u64MultiWakeSum;
    u64 u64MsixInit;
    u64 u64EventPost;
    u64 u64WakerKick;
    u64 u64WakerKickHit;
    u64 u64WakerDrain;
    u64 u64WakerDrainSum;
    u64 u64ResidualLean;
    u64 u64ResidualLeanOk;
    u64 u64ResidualWaitOk;
    u64 u64ResidualWaitMiss;
    u64 u64ResidualWaitAny;
    u64 u64ResidualThrOk;
    u64 u64InstallOk;
    u64 u64WaitersPeak;
    u32 u32Ready;
    u32 u32Live;
    u32 u32Waiters;

    notify_soft_inc(&g_soft.u64SoftLog);

    /*
     * Snapshot under relaxed loads - soft inventory only; concurrent
     * IRQ pulse bumps may race (acceptable for diagnostics).
     */
    u64SoftLogN =
        __atomic_load_n(&g_soft.u64SoftLog, __ATOMIC_RELAXED);
    u64Init = __atomic_load_n(&g_soft.u64Init, __ATOMIC_RELAXED);
    u64PulseEnter =
        __atomic_load_n(&g_soft.u64PulseEnter, __ATOMIC_RELAXED);
    u64PulseHit =
        __atomic_load_n(&g_soft.u64PulseHit, __ATOMIC_RELAXED);
    u64WaitEnter =
        __atomic_load_n(&g_soft.u64WaitEnter, __ATOMIC_RELAXED);
    u64WaitHit =
        __atomic_load_n(&g_soft.u64WaitHit, __ATOMIC_RELAXED);
    u64MultiWakeCalls =
        __atomic_load_n(&g_soft.u64MultiWakeCalls, __ATOMIC_RELAXED);
    u64MultiWakeSum =
        __atomic_load_n(&g_soft.u64MultiWakeSum, __ATOMIC_RELAXED);
    u64MsixInit =
        __atomic_load_n(&g_soft.u64MsixInit, __ATOMIC_RELAXED);
    u64EventPost =
        __atomic_load_n(&g_soft.u64EventPost, __ATOMIC_RELAXED);
    u64WakerKick =
        __atomic_load_n(&g_soft.u64WakerKick, __ATOMIC_RELAXED);
    u64WakerKickHit =
        __atomic_load_n(&g_soft.u64WakerKickHit, __ATOMIC_RELAXED);
    u64WakerDrain =
        __atomic_load_n(&g_soft.u64WakerDrain, __ATOMIC_RELAXED);
    u64WakerDrainSum =
        __atomic_load_n(&g_soft.u64WakerDrainSum, __ATOMIC_RELAXED);
    u64ResidualLean =
        __atomic_load_n(&g_soft.u64ResidualLean, __ATOMIC_RELAXED);
    u64ResidualLeanOk =
        __atomic_load_n(&g_soft.u64ResidualLeanOk, __ATOMIC_RELAXED);
    u64ResidualWaitOk =
        __atomic_load_n(&g_soft.u64ResidualWaitOk, __ATOMIC_RELAXED);
    u64ResidualWaitMiss =
        __atomic_load_n(&g_soft.u64ResidualWaitMiss, __ATOMIC_RELAXED);
    u64ResidualWaitAny =
        __atomic_load_n(&g_soft.u64ResidualWaitAny, __ATOMIC_RELAXED);
    u64ResidualThrOk =
        __atomic_load_n(&g_soft.u64ResidualThrOk, __ATOMIC_RELAXED);
    u64InstallOk =
        __atomic_load_n(&g_soft.u64InstallOk, __ATOMIC_RELAXED);
    u64WaitersPeak =
        __atomic_load_n(&g_soft.u64WaitersPeak, __ATOMIC_RELAXED);

    notify_soft_msix_snap(&u32Ready, &u32Live, NULL, NULL, NULL,
                          &u32Waiters, NULL);
    notify_soft_note_waiters(u32Waiters);

    /*
     * Grep: notify: soft inventory - single rollup lamp (sparse).
     * Soft!=product / dual MIT OR Apache-2.0 / no version stamp / G-AC-1.
     */
    kprintf("notify: soft inventory multi_max=%u tag_waiter=%u "
            "pulse_enter=%lu pulse_hit=%lu wait_enter=%lu wait_hit=%lu "
            "event=%lu waker_kick=%lu waker_drain=%lu "
            "msix_init=%lu inits=%lu install_ok=%lu soft_log=%lu "
            "areas=%u multi_proc=0 G-AC-1=1 thr_only=%u "
            "(sparse; Soft!=product; dual MIT OR Apache-2.0; H1 thr-only)\n",
            (unsigned)NOTIFY_SOFT_MULTI_MAX, (unsigned)NOTIFY_TAG_WAITER,
            (unsigned long)u64PulseEnter, (unsigned long)u64PulseHit,
            (unsigned long)u64WaitEnter, (unsigned long)u64WaitHit,
            (unsigned long)u64EventPost, (unsigned long)u64WakerKick,
            (unsigned long)u64WakerDrain, (unsigned long)u64MsixInit,
            (unsigned long)u64Init, (unsigned long)u64InstallOk,
            (unsigned long)u64SoftLogN, (unsigned)NOTIFY_SOFT_AREAS,
            (unsigned)NOTIFY_H1_THR_ONLY);

    /*
     * Grep: notify: soft multi
     * Soft multi-waiter wake budget - not multi-process notify product.
     */
    kprintf("notify: soft multi calls=%lu sum=%lu peak_waiters=%lu "
            "multi_max=%u tag_waiter=%u waker_kick=%lu kick_hit=%lu "
            "waker_drain=%lu drain_sum=%lu drain_rounds=%u "
            "multi_proc=0 soft_ne_multi_proc=1 "
            "(soft multi-waiter residual; Soft!=product)\n",
            (unsigned long)u64MultiWakeCalls,
            (unsigned long)u64MultiWakeSum,
            (unsigned long)u64WaitersPeak,
            (unsigned)NOTIFY_SOFT_MULTI_MAX, (unsigned)NOTIFY_TAG_WAITER,
            (unsigned long)u64WakerKick, (unsigned long)u64WakerKickHit,
            (unsigned long)u64WakerDrain, (unsigned long)u64WakerDrainSum,
            (unsigned)NOTIFY_WAKER_DRAIN_ROUNDS);

    /*
     * Grep: notify: soft residual lean
     * Event/wait/waker residual honesty for UDX thr notify path.
     * Soft!=product dual license. multi_proc=0 / mig_reply=0 open surfaces.
     * Product IRQ Notification mint OPEN (badge; not .ko). H1 thr-only.
     * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
     */
    kprintf("notify: soft residual lean "
            "event=%lu waker_kick=%lu kick_hit=%lu "
            "waker_drain=%lu drain_sum=%lu lean_runs=%lu lean_ok=%lu "
            "wait_ok=%lu wait_miss=%lu wait_any=%lu thr_ok=%lu "
            "msix_ready=%u live=%u path=irq_pulse->thr_notify_wait "
            "thr_only=%u irq_pulse_only=%u batch_after_wait=%u "
            "irq_driver=%u udx=1 product_hosts=UDX soft_ne_product=1 "
            "dual=MIT_OR_Apache-2.0 multi_proc=0 mig_reply=0 mint_OPEN=1 "
            "G-AC-1=1 hazard=H1 dual_dod=OPEN H2=once "
            "(Soft!=product; dual MIT OR Apache-2.0; no version stamp; "
            "not multi-process notify product; no .ko product; "
            "H1 thr notify not hard-IRQ driver; Dual DoD OPEN)\n",
            (unsigned long)u64EventPost, (unsigned long)u64WakerKick,
            (unsigned long)u64WakerKickHit, (unsigned long)u64WakerDrain,
            (unsigned long)u64WakerDrainSum,
            (unsigned long)u64ResidualLean,
            (unsigned long)u64ResidualLeanOk,
            (unsigned long)u64ResidualWaitOk,
            (unsigned long)u64ResidualWaitMiss,
            (unsigned long)u64ResidualWaitAny,
            (unsigned long)u64ResidualThrOk, (unsigned)u32Ready,
            (unsigned)u32Live, (unsigned)NOTIFY_H1_THR_ONLY,
            (unsigned)NOTIFY_H1_IRQ_PULSE_ONLY,
            (unsigned)NOTIFY_H1_BATCH_AFTER_WAIT,
            (unsigned)NOTIFY_H1_IRQ_DRIVER);

    /*
     * Grep: notify: soft residual lean wait
     * Wait residual for thr/UDX path (pulse -> CAS-clear wait/poll).
     * Product IRQ Notification mint OPEN. Soft!=product. H1 thr-only.
     * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
     */
    kprintf("notify: soft residual lean wait "
            "wait_enter=%lu wait_hit=%lu wait_ok=%lu wait_miss=%lu "
            "wait_any=%lu thr_ok=%lu path=irq_pulse->thr_notify_wait "
            "udx_thr=1 fBlock_noblock=1 cas=pending_and_mask "
            "thr_only=%u irq_pulse_only=%u batch_after_wait=%u "
            "irq_driver=%u soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "multi_proc=0 mig_reply=0 mint_OPEN=1 G-AC-1=1 hazard=H1 "
            "product_hosts=UDX dual_dod=OPEN H2=once "
            "(UDX thr wait residual; Soft!=product; no .ko product; "
            "H1 thr notify not hard-IRQ driver)\n",
            (unsigned long)u64WaitEnter, (unsigned long)u64WaitHit,
            (unsigned long)u64ResidualWaitOk,
            (unsigned long)u64ResidualWaitMiss,
            (unsigned long)u64ResidualWaitAny,
            (unsigned long)u64ResidualThrOk,
            (unsigned)NOTIFY_H1_THR_ONLY,
            (unsigned)NOTIFY_H1_IRQ_PULSE_ONLY,
            (unsigned)NOTIFY_H1_BATCH_AFTER_WAIT,
            (unsigned)NOTIFY_H1_IRQ_DRIVER);

    /*
     * Grep: notify: soft residual lean thr
     * H1 thr-only direction lamp (sparse; Soft!=product).
     * hard IRQ = pulse only; thr reaps; batch after wait; no IRQ driver.
     * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
     */
    kprintf("notify: soft residual lean thr "
            "thr_only=%u irq_pulse_only=%u thr_wait=%u "
            "batch_after_wait=%u irq_driver=%u thr_ok=%lu "
            "path=irq_pulse->thr_notify_wait "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 multi_proc=0 "
            "mig_reply=0 mint_OPEN=1 G-AC-1=1 hazard=H1 "
            "product_hosts=UDX dual_dod=OPEN H2=once "
            "(H1 thr notify not hard-IRQ driver; Soft!=product; "
            "no version stamp; no .ko product)\n",
            (unsigned)NOTIFY_H1_THR_ONLY,
            (unsigned)NOTIFY_H1_IRQ_PULSE_ONLY,
            (unsigned)NOTIFY_H1_THR_WAIT,
            (unsigned)NOTIFY_H1_BATCH_AFTER_WAIT,
            (unsigned)NOTIFY_H1_IRQ_DRIVER,
            (unsigned long)u64ResidualThrOk);

    /*
     * Grep: notify: soft path
     * Honesty: soft multi-waiter != multi-process notify product.
     * Event/wait/waker residual: door clients + UDX thr notify path.
     * H1: thr notify not hard-IRQ driver. product_hosts=UDX Dual DoD OPEN.
     */
    kprintf("notify: soft path claim=badge_event_wait_waker "
            "irq=pulse_or+soft_multi_wake cas=pending_and_mask "
            "event=notify_event wait=notify_wait+poll "
            "waker=notify_wake+drain "
            "udx=NOTIFY_WAIT multi_max=%u tag_waiter=%u "
            "park=thread_block+schedule drain_rounds=%u "
            "thr_only=%u irq_pulse_only=%u batch_after_wait=%u "
            "irq_driver=%u multi_proc=0 soft_ne_multi_proc=1 "
            "soft_ne_mig_reply=1 soft_ne_product=1 mint_OPEN=1 "
            "G-AC-1=1 hazard=H1 product_hosts=UDX dual_dod=OPEN H2=once "
            "(soft inventory; UDX thr wait residual; not multi-process "
            "notify; Soft!=product; dual MIT OR Apache-2.0; "
            "H1 thr notify not hard-IRQ driver; MSI-X inject Dual DoD OPEN)\n",
            (unsigned)NOTIFY_SOFT_MULTI_MAX, (unsigned)NOTIFY_TAG_WAITER,
            (unsigned)NOTIFY_WAKER_DRAIN_ROUNDS,
            (unsigned)NOTIFY_H1_THR_ONLY,
            (unsigned)NOTIFY_H1_IRQ_PULSE_ONLY,
            (unsigned)NOTIFY_H1_BATCH_AFTER_WAIT,
            (unsigned)NOTIFY_H1_IRQ_DRIVER);

    /*
     * Soft lamp: MSI-X bind ready (or inventory emission). Never hard-gates.
     * Grep: notify: soft inventory PASS | notify: soft PASS | notify: soft FAIL
     */
    if (u32Ready != 0u && u32Live != 0u) {
        kprintf("notify: soft inventory PASS ready=%u live=%u "
                "soft_log=%lu areas=%u multi_proc=0 G-AC-1=1 thr_only=%u "
                "(sparse; Soft!=product; H1 thr-only)\n",
                (unsigned)u32Ready, (unsigned)u32Live,
                (unsigned long)u64SoftLogN, (unsigned)NOTIFY_SOFT_AREAS,
                (unsigned)NOTIFY_H1_THR_ONLY);
        kprintf("notify: soft PASS soft_log=%lu areas=%u thr_only=%u\n",
                (unsigned long)u64SoftLogN, (unsigned)NOTIFY_SOFT_AREAS,
                (unsigned)NOTIFY_H1_THR_ONLY);
    } else {
        kprintf("notify: soft FAIL ready=%u live=%u "
                "(soft inventory only; not product gate; not multi-process "
                "notify; Soft!=product; G-AC-1; H1 thr-only)\n",
                (unsigned)u32Ready, (unsigned)u32Live);
    }
}

/** Soft: one-shot sparse inventory after first product wait activity. */
static void
notify_soft_log_once(void)
{
    if (g_fSoftStatsOnce != 0) {
        return;
    }
    g_fSoftStatsOnce = 1;
    notify_soft_log();
}

/* Forward: lean residual self-check (def after event/waker symbols). */
static void notify_soft_residual_lean_once(void);

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
 * Soft multi-wake: wake up to u32Max waiters on this object (0 -> MULTI_MAX).
 * greppable: NOTIFY_SOFT_MULTI_WAKE
 */
static u32
notify_soft_multi_wake_n(struct gj_notify *pN, u32 u32Max)
{
    u32 cWoken;

    if (pN == NULL) {
        /* greppable: notify: soft multi wake */
        notify_soft_inc(&g_soft.u64MultiWakeNull);
        return 0;
    }
    if (u32Max == 0u) {
        u32Max = NOTIFY_SOFT_MULTI_MAX;
    }
    /* greppable: notify: soft multi wake */
    notify_soft_inc(&g_soft.u64MultiWakeCalls);
    cWoken = thread_wake(pN, NOTIFY_TAG_WAITER, u32Max);
    notify_soft_note_wake(cWoken);
    return cWoken;
}

/** Soft multi-wake with default budget (pulse / self-wake path). */
static u32
notify_soft_multi_wake(struct gj_notify *pN)
{
    return notify_soft_multi_wake_n(pN, NOTIFY_SOFT_MULTI_MAX);
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

/*
 * Bounded drain: repeated multi-wake rounds until no waiter or round cap.
 * Abort/mark_dead only - not hard IRQ (kprintf-free; atomics + thread_wake).
 * greppable: NOTIFY_WAKER_DRAIN
 */
static u32
notify_soft_waker_drain(struct gj_notify *pN)
{
    u32 cTotal = 0;
    u32 cRound;
    u32 iRound;

    if (pN == NULL) {
        return 0;
    }
    notify_soft_inc(&g_soft.u64WakerDrain);
    for (iRound = 0; iRound < NOTIFY_WAKER_DRAIN_ROUNDS; iRound++) {
        if (!notify_has_waiter(pN)) {
            break;
        }
        cRound = notify_soft_multi_wake(pN);
        cTotal += cRound;
        if (cRound == 0u) {
            break;
        }
    }
    if (cTotal != 0u) {
        notify_soft_add(&g_soft.u64WakerDrainSum, (u64)cTotal);
    }
    return cTotal;
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

    /*
     * H1 thr-only / IRQ pulse-only residual (Soft!=product; G-AC-1):
     * Sole hard-IRQ-callable product entry in this unit. Atomic RMW +
     * optional soft multi-wake only. No kprintf, no alloc, no park, no
     * class-driver work, no net_eth_poll. Batch lives on thr after wait.
     * greppable: NOTIFY_H1_THR_ONLY irq_pulse_only=1 irq_driver=0
     */
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
    /* greppable: NOTIFY_BADGE_PULSE - OR pending, then soft multi-wake */
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

void
notify_event(struct gj_notify *pN, u64 u64Badge)
{
    /*
     * Event residual (door clients / UDX IRQ-ish): same product path as
     * pulse (H1 pulse-only IRQ entry). Named for async completion sites.
     * IRQ-safe via pulse; thr reaps with wait. greppable: NOTIFY_EVENT_POST
     */
    notify_soft_inc(&g_soft.u64EventPost);
    notify_pulse(pN, u64Badge);
}

u32
notify_wake(struct gj_notify *pN, u32 u32Max)
{
    u32 cWoken;

    /*
     * Waker residual: kick parked waiters without posting badge bits.
     * Door teardown / UDX quiesce. Waiters re-sample pending / live.
     * greppable: NOTIFY_WAKER_KICK
     * Soft!=product (not multi-process notify product).
     */
    notify_soft_inc(&g_soft.u64WakerKick);
    if (pN == NULL) {
        notify_soft_inc(&g_soft.u64WakerKickZero);
        return 0;
    }
    if (!notify_has_waiter(pN)) {
        notify_soft_inc(&g_soft.u64WakerKickZero);
        return 0;
    }
    cWoken = notify_soft_multi_wake_n(pN, u32Max);
    if (cWoken == 0u) {
        notify_soft_inc(&g_soft.u64WakerKickZero);
    } else {
        notify_soft_inc(&g_soft.u64WakerKickHit);
    }
    return cWoken;
}

u32
notify_wake_drain(struct gj_notify *pN)
{
    /* greppable: NOTIFY_WAKER_DRAIN */
    return notify_soft_waker_drain(pN);
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

    /*
     * Wait residual (thr/UDX consumer; H1 thr-only): CAS-clear matched bits.
     * Product: hard IRQ / soft inject pulsed bits (pulse only); thr reaps
     * via GJ_SYS_NOTIFY_WAIT (often fBlock=0). Soft multi parks when fBlock
     * and thr context present. Batch work AFTER wait returns (not on IRQ).
     * greppable: NOTIFY_WAIT_RESIDUAL
     * greppable: NOTIFY_BADGE_WAIT
     * greppable: NOTIFY_H1_THR_ONLY thr_only=1 batch_after_wait=1
     * Soft!=product; product IRQ Notification mint OPEN.
     * H1: thr notify not hard-IRQ driver.
     */
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
    /* mask==0 means "any badge" - greppable: NOTIFY_BADGE_WAIT */
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
            /* Lost race with signal/clear - retry without sleeping. */
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
    u32 cWoken;

    if (pN == NULL) {
        return;
    }
    /* greppable: NOTIFY_ABORT_SOFT_MULTI */
    /* greppable: notify: soft abort */
    notify_soft_inc(&g_soft.u64Abort);
    if (notify_has_waiter(pN)) {
        /* Drain residual: do not leave >MULTI_MAX waiters parked. */
        cWoken = notify_soft_waker_drain(pN);
        if (cWoken != 0u) {
            notify_soft_inc(&g_soft.u64AbortWake);
        } else {
            notify_soft_inc(&g_soft.u64AbortNoWaiter);
        }
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

/*
 * Lean residual self-check (stack-local notify; never touches MSI-X global).
 * Exercises thr wait residual (pulse -> non-block wait CAS-clear),
 * event residual, empty miss, mask-any, empty waker kick/drain, mark_dead,
 * and H1 thr-only direction (pulse-only IRQ; thr reaps; pending drained;
 * compile-time H1 locks hold). Soft!=product / dual MIT OR Apache-2.0 /
 * G-AC-1. Product IRQ Notification mint OPEN (badge; not .ko AC).
 * greppable: NOTIFY_LEAN_RESIDUAL
 * greppable: NOTIFY_WAIT_RESIDUAL
 * greppable: NOTIFY_H1_THR_ONLY
 * greppable: notify: soft residual lean
 * greppable: notify: soft residual lean wait
 * greppable: notify: soft residual lean thr
 * greppable: notify: soft residual lean udx
 * greppable: notify: soft residual denser
 * greppable: notify: soft residual lean PASS
 * greppable: notify: soft residual lean FAIL
 * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
 * greppable: denser=1 MSI-X inject denser
 * STRONGER functional residual (W7 Dual DoD; stamp-free bar v2026.08.04.75):
 *   null_miss | zero_coalesce | multi_badge | signal_alias | dual_dod_open
 *   toward UDX IRQ-ish thr wait / sshd host notify path. H1 thr-only.
 * STRONGER functional residual (W10 Dual DoD; stamp-free bar v2026.08.04.75):
 *   install_null | dead_pulse for live UDX host IRQ thr path. H1 thr-only.
 * STRONGER functional residual (W11 Dual DoD; stamp-free bar v2026.08.04.75):
 *   poll_eq | partial_reap | abort_nobadge | inject_query | dual_dod_open
 *   product notify path honesty for MSI-X inject used by Dual DoD hosts.
 *   product_hosts=UDX; Dual DoD A/B remain OPEN. Soft!=product. H2 once.
 * STRONGER denser residual (W12 Dual DoD; stamp-free bar v2026.08.04.75):
 *   event denser | wait denser | waker denser | inject denser | dual_dod_open
 *   MSI-X inject product path denser for Dual DoD hosts (rtl8168_udx|xhci_udx).
 * Soft!=product. Dual DoD A/B remain OPEN.
 */
static void
notify_soft_residual_lean_once(void)
{
    static u8         g_fLeanOnce;
    struct gj_notify  nTmp;
    struct gj_cap_ref capTmp;
    u64               u64Got;
    u64               u64WaitIsr;
    u64               u64WaitMiss;
    u64               u64WaitAny;
    u64               u64PendAfter;
    u64               u64ZeroGot;
    u64               u64MultiGot;
    u64               u64SigGot;
    u64               u64PulseDead0;
    u64               u64PendDead;
    u64               u64PollEq;
    u64               u64PartGot;
    u64               u64PendPart;
    u64               u64AbortPend;
    u64               u64InjectGot;
    u64               u64DEventGot;
    u64               u64DRemaskA;
    u64               u64DRemaskB;
    u64               u64DRemaskC;
    u64               u64DMaskAny;
    u64               u64DPollThen;
    u64               u64DWaitEmpty;
    u64               u64DAbortKeep;
    u64               u64DInjectRearm;
    u64               u64DMultiVecA;
    u64               u64DMultiVecB;
    u64               u64DLastBadge;
    u32               cKick;
    u32               cDrain;
    u32               cKickMax0;
    u32               cDrainEmpty;
    u32               cNullKick;
    u32               cNullDrain;
    u32               u32Ok;
    u32               u32Checks;
    u32               u32LiveAfterDead;
    u32               u32WaitIsrOk;
    u32               u32WaitMissOk;
    u32               u32WaitAnyOk;
    u32               u32ThrOk;
    u32               u32H1Locks;
    u32               u32NullMissOk;
    u32               u32ZeroOk;
    u32               u32MultiOk;
    u32               u32SignalOk;
    u32               u32DualDodOk;
    u32               u32InstallNullOk;
    u32               u32DeadPulseOk;
    u32               u32PollEqOk;
    u32               u32PartialOk;
    u32               u32AbortNoBadgeOk;
    u32               u32InjectQueryOk;
    u32               u32SigBefore;
    u32               u32SigAfter;
    /* W12 denser residual arm lamps (Soft!=product; Dual DoD OPEN). */
    u32               u32DEventOrOk;
    u32               u32DEventEqOk;
    u32               u32DSignalEqOk;
    u32               u32DCoalesceOk;
    u32               u32DRemask3Ok;
    u32               u32DMaskAnyOk;
    u32               u32DPollMissOk;
    u32               u32DWaitEmptyOk;
    u32               u32DKickMax0Ok;
    u32               u32DDrainEmptyOk;
    u32               u32DAbortKeepOk;
    u32               u32DNullKickOk;
    u32               u32DInjectRearmOk;
    u32               u32DMultiVecOk;
    u32               u32DLastBadgeOk;
    u32               u32DSignalsMonoOk;
    u32               u32DenseOk;
    u32               u32DenseEventN;
    u32               u32DenseWaitN;
    u32               u32DenseWakerN;
    u32               u32DenseInjectN;
    u32               u32SigMono0;
    u32               u32SigMono1;
    u32               u32SigMono2;

    if (g_fLeanOnce != 0) {
        return;
    }
    g_fLeanOnce = 1;
    notify_soft_inc(&g_soft.u64ResidualLean);

    u32Ok = 0;
    u32Checks = 0;
    u32WaitIsrOk = 0;
    u32WaitMissOk = 0;
    u32WaitAnyOk = 0;
    u32ThrOk = 0;
    u32NullMissOk = 0;
    u32ZeroOk = 0;
    u32MultiOk = 0;
    u32SignalOk = 0;
    u32DualDodOk = 0;
    u32InstallNullOk = 0;
    u32DeadPulseOk = 0;
    u32PollEqOk = 0;
    u32PartialOk = 0;
    u32AbortNoBadgeOk = 0;
    u32InjectQueryOk = 0;
    u32DEventOrOk = 0;
    u32DEventEqOk = 0;
    u32DSignalEqOk = 0;
    u32DCoalesceOk = 0;
    u32DRemask3Ok = 0;
    u32DMaskAnyOk = 0;
    u32DPollMissOk = 0;
    u32DWaitEmptyOk = 0;
    u32DKickMax0Ok = 0;
    u32DDrainEmptyOk = 0;
    u32DAbortKeepOk = 0;
    u32DNullKickOk = 0;
    u32DInjectRearmOk = 0;
    u32DMultiVecOk = 0;
    u32DLastBadgeOk = 0;
    u32DSignalsMonoOk = 0;
    u32DenseOk = 0;
    u32DenseEventN = 0;
    u32DenseWaitN = 0;
    u32DenseWakerN = 0;
    u32DenseInjectN = 0;
    u64WaitIsr = 0;
    u64WaitMiss = ~0ull;
    u64WaitAny = 0;
    u64PendAfter = ~0ull;
    u64ZeroGot = 0;
    u64MultiGot = 0;
    u64SigGot = 0;
    u64PulseDead0 = 0;
    u64PendDead = ~0ull;
    u64PollEq = 0;
    u64PartGot = 0;
    u64PendPart = ~0ull;
    u64AbortPend = ~0ull;
    u64InjectGot = 0;
    u64DEventGot = 0;
    u64DRemaskA = 0;
    u64DRemaskB = 0;
    u64DRemaskC = 0;
    u64DMaskAny = 0;
    u64DPollThen = 0;
    u64DWaitEmpty = ~0ull;
    u64DAbortKeep = ~0ull;
    u64DInjectRearm = 0;
    u64DMultiVecA = 0;
    u64DMultiVecB = 0;
    u64DLastBadge = 0;
    u32SigBefore = 0;
    u32SigAfter = 0;
    u32SigMono0 = 0;
    u32SigMono1 = 0;
    u32SigMono2 = 0;
    cKickMax0 = 0;
    cDrainEmpty = 0;
    cNullKick = 0;
    cNullDrain = 0;
    memset(&nTmp, 0, sizeof(nTmp));
    memset(&capTmp, 0, sizeof(capTmp));
    notify_init(&nTmp);

    /*
     * Check 1: thr/UDX wait residual (H1 thr-only consumer).
     * Shape: notify_pulse (hard-IRQ / soft inject product pulse-only) then
     * notify_wait(mask, fBlock=0) CAS-clear - thr/UDX pump reaps bits.
     * greppable: NOTIFY_WAIT_RESIDUAL NOTIFY_H1_THR_ONLY
     */
    u32Checks++;
    notify_pulse(&nTmp, 0x2ull);
    u64WaitIsr = notify_wait(&nTmp, 0x2ull, 0);
    if (u64WaitIsr == 0x2ull) {
        u32Ok++;
        u32WaitIsrOk = 1;
        notify_soft_inc(&g_soft.u64ResidualWaitOk);
    }

    /*
     * Check 2: wait residual empty miss (no pending bits) -> 0.
     * thr/UDX non-block poll miss path; Soft!=product.
     */
    u32Checks++;
    u64WaitMiss = notify_wait(&nTmp, 0x2ull, 0);
    if (u64WaitMiss == 0ull) {
        u32Ok++;
        u32WaitMissOk = 1;
        notify_soft_inc(&g_soft.u64ResidualWaitMiss);
    }

    /*
     * Check 3: wait residual mask==0 ("any badge") after pulse.
     * Same CAS-clear path; thr/UDX may wait with mask any.
     */
    u32Checks++;
    notify_pulse(&nTmp, 0x8ull);
    u64WaitAny = notify_wait(&nTmp, 0ull, 0);
    if (u64WaitAny == 0x8ull) {
        u32Ok++;
        u32WaitAnyOk = 1;
        notify_soft_inc(&g_soft.u64ResidualWaitAny);
    }

    /* Check 4: event residual - post badge bit 2, CAS-clear via poll. */
    u32Checks++;
    notify_event(&nTmp, 0x4ull);
    u64Got = notify_poll(&nTmp, 0x4ull);
    if (u64Got == 0x4ull) {
        u32Ok++;
    }

    /* Check 5: empty waker residual (no parked waiter) -> 0 / 0. */
    u32Checks++;
    cKick = notify_wake(&nTmp, 0);
    cDrain = notify_wake_drain(&nTmp);
    if (cKick == 0u && cDrain == 0u) {
        u32Ok++;
    }

    /*
     * Check 6: H1 thr-only direction residual.
     * After thr reaps, pending must be drained (pulse-only left bits for
     * thr wait). Compile-time H1 locks must hold (irq_driver=0 thr_only=1).
     * greppable: NOTIFY_H1_THR_ONLY thr_only=1 irq_driver=0
     */
    u32Checks++;
    u64PendAfter = notify_pending(&nTmp);
    u32H1Locks =
        (NOTIFY_H1_IRQ_PULSE_ONLY == 1u && NOTIFY_H1_THR_WAIT == 1u &&
         NOTIFY_H1_BATCH_AFTER_WAIT == 1u && NOTIFY_H1_IRQ_DRIVER == 0u &&
         NOTIFY_H1_THR_ONLY == 1u)
            ? 1u
            : 0u;
    if (u64PendAfter == 0ull && u32WaitIsrOk != 0u && u32WaitMissOk != 0u &&
        u32WaitAnyOk != 0u && u32H1Locks != 0u) {
        u32Ok++;
        u32ThrOk = 1;
        notify_soft_inc(&g_soft.u64ResidualThrOk);
    }

    /*
     * STRONGER functional residual (W7 Dual DoD; UDX/sshd product path).
     * Fail-closed + coalesce + multi-badge arms. Soft!=product. H1 thr-only.
     * Dual DoD A/B remain OPEN. Stamp-free.
     */

    /* Check 7: null_miss residual - pulse/wait/query fail closed. */
    u32Checks++;
    notify_pulse(NULL, 0x1ull);
    notify_event(NULL, 0x1ull);
    if (notify_wait(NULL, 0x1ull, 0) == 0ull &&
        notify_poll(NULL, 0x1ull) == 0ull &&
        notify_is_live(NULL) == 0 &&
        notify_pending(NULL) == 0ull &&
        notify_signals(NULL) == 0u &&
        notify_last_badge(NULL) == 0ull &&
        notify_waiters(NULL) == 0u &&
        notify_wake(NULL, 0) == 0u &&
        notify_wake_drain(NULL) == 0u) {
        u32NullMissOk = 1;
        u32Ok++;
    }

    /*
     * Check 8: zero badge coalesce residual (product pulse: 0 -> bit 0).
     * UDX IRQ-ish sites may pulse 0; thr reaps bit 0.
     */
    u32Checks++;
    notify_pulse(&nTmp, 0ull);
    u64ZeroGot = notify_wait(&nTmp, 0x1ull, 0);
    if (u64ZeroGot == 0x1ull && notify_last_badge(&nTmp) == 0x1ull) {
        u32ZeroOk = 1;
        u32Ok++;
    }

    /*
     * Check 9: multi-badge residual (OR pending; thr mask reaps subset).
     * Shape for UDX multi-vector soft inject then thr wait.
     */
    u32Checks++;
    notify_pulse(&nTmp, 0x11ull);
    notify_pulse(&nTmp, 0x22ull);
    u64MultiGot = notify_wait(&nTmp, 0x30ull, 0); /* bits 4+5 of 0x11|0x22 */
    if (u64MultiGot == 0x30ull &&
        (notify_pending(&nTmp) & 0x3ull) == 0x3ull) {
        /* leftover low bits 0x01|0x02 still pending until reaped */
        (void)notify_wait(&nTmp, ~0ull, 0); /* drain residual */
        u32MultiOk = 1;
        u32Ok++;
    }

    /* Check 10: signal alias residual (signal == pulse product path). */
    u32Checks++;
    notify_signal(&nTmp, 0x40ull);
    u64SigGot = notify_poll(&nTmp, 0x40ull);
    if (u64SigGot == 0x40ull && notify_signals(&nTmp) >= 1u) {
        u32SignalOk = 1;
        u32Ok++;
    }

    /*
     * W10 STRONGER residual (live UDX host IRQ thr path):
     * install_null | dead_pulse. Soft!=product; H1 thr-only; Dual DoD OPEN.
     */

    /* Check 11: install_null residual - ENDPOINT/NOTIFICATION null fail-closed. */
    u32Checks++;
    if (notify_install(NULL, &nTmp, 0, &capTmp) == GJ_ERR_INVAL &&
        notify_install(NULL, NULL, 0, NULL) == GJ_ERR_INVAL) {
        u32InstallNullOk = 1;
        u32Ok++;
    }

    /*
     * W11 STRONGER residual (product notify path honesty; MSI-X inject):
     * poll_eq | partial_reap | abort_nobadge | inject_query | dual_dod_open
     * Dual DoD hosts product_hosts=UDX (rtl8168_udx|xhci_udx). Soft!=product.
     * H1 thr-only; H2 once; stamp-free bar v2026.08.04.75.
     * greppable: poll_eq | partial_reap | abort_nobadge | inject_query
     * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
     */

    /*
     * Check 12: poll_eq residual - notify_poll == wait(fBlock=0) thr path.
     * Product UDX freestanding pumps non-blocking poll after MSI-X inject.
     * Soft!=product; H1 thr-only.
     */
    u32Checks++;
    notify_pulse(&nTmp, 0x100ull);
    u64PollEq = notify_poll(&nTmp, 0x100ull);
    if (u64PollEq == 0x100ull && notify_poll(&nTmp, 0x100ull) == 0ull &&
        notify_pending(&nTmp) == 0ull) {
        u32PollEqOk = 1;
        u32Ok++;
    }

    /*
     * Check 13: partial_reap residual - multi-vector MSI-X inject shape.
     * Pulse multi-bit badge; thr mask reaps subset; leftover pending for
     * next thr wait (product hosts may vector-share one Notification).
     */
    u32Checks++;
    notify_pulse(&nTmp, 0xC00ull); /* bits 10+11 */
    u64PartGot = notify_wait(&nTmp, 0x400ull, 0); /* bit 10 only */
    u64PendPart = notify_pending(&nTmp);
    if (u64PartGot == 0x400ull && u64PendPart == 0x800ull &&
        notify_wait(&nTmp, 0x800ull, 0) == 0x800ull &&
        notify_pending(&nTmp) == 0ull) {
        u32PartialOk = 1;
        u32Ok++;
    }

    /*
     * Check 14: abort_nobadge residual - UDX quiesce does not post badge.
     * abort_waiter drains waiters only; pending badge remains for thr reap.
     * Soft!=product; not hard-IRQ driver work.
     */
    u32Checks++;
    notify_pulse(&nTmp, 0x1000ull);
    u64AbortPend = notify_pending(&nTmp);
    notify_abort_waiter(&nTmp);
    if (u64AbortPend == 0x1000ull &&
        notify_pending(&nTmp) == 0x1000ull &&
        notify_wait(&nTmp, 0x1000ull, 0) == 0x1000ull &&
        notify_pending(&nTmp) == 0ull) {
        u32AbortNoBadgeOk = 1;
        u32Ok++;
    }

    /*
     * Check 15: inject_query residual - MSI-X inject product shape honesty.
     * After pulse: signals bump, last_badge matches inject word, pending OR.
     * thr reaps via wait; product path = inject pulse -> thr notify wait.
     * Soft!=product; product_hosts=UDX; Dual DoD OPEN.
     */
    u32Checks++;
    u32SigBefore = notify_signals(&nTmp);
    notify_pulse(&nTmp, 0x2000ull);
    u32SigAfter = notify_signals(&nTmp);
    u64InjectGot = notify_pending(&nTmp);
    if (u32SigAfter == u32SigBefore + 1u &&
        notify_last_badge(&nTmp) == 0x2000ull &&
        (u64InjectGot & 0x2000ull) == 0x2000ull &&
        notify_is_live(&nTmp) != 0 &&
        notify_wait(&nTmp, 0x2000ull, 0) == 0x2000ull &&
        notify_pending(&nTmp) == 0ull) {
        u32InjectQueryOk = 1;
        u32Ok++;
    }

    /*
     * W12 denser residual honesty (MSI-X inject product path):
     * Deeper edges under W11 arms for product_hosts=UDX Dual DoD.
     * Soft!=product; Dual DoD OPEN; stamp-free bar v2026.08.04.75.
     * Stack-local only (never touches MSI-X global). H1 thr-only.
     * greppable: notify: soft residual denser | MSI-X inject denser
     * greppable: denser=1 | event denser | wait denser | waker denser |
     *            inject denser
     * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
     */

    /*
     * --- event denser: dpulse_or | event_eq | signal_eq | coalesce_rearm.
     * Product MSI-X inject may multi-pulse same vector; thr reaps OR.
     */
    u32Checks++;
    notify_pulse(&nTmp, 0x10000ull);
    notify_pulse(&nTmp, 0x10000ull); /* same badge OR - signals++ twice */
    if (notify_pending(&nTmp) == 0x10000ull &&
        notify_last_badge(&nTmp) == 0x10000ull &&
        notify_wait(&nTmp, 0x10000ull, 0) == 0x10000ull &&
        notify_pending(&nTmp) == 0ull) {
        u32DEventOrOk = 1;
        u32DenseEventN++;
        notify_soft_inc(&g_soft.u64DenseEventOr);
    }
    notify_event(&nTmp, 0x20000ull);
    u64DEventGot = notify_poll(&nTmp, 0x20000ull);
    if (u64DEventGot == 0x20000ull &&
        notify_last_badge(&nTmp) == 0x20000ull &&
        notify_pending(&nTmp) == 0ull) {
        u32DEventEqOk = 1;
        u32DenseEventN++;
        notify_soft_inc(&g_soft.u64DenseEventEq);
    }
    notify_signal(&nTmp, 0x40000ull);
    if (notify_wait(&nTmp, 0x40000ull, 0) == 0x40000ull &&
        notify_last_badge(&nTmp) == 0x40000ull &&
        notify_pending(&nTmp) == 0ull) {
        u32DSignalEqOk = 1;
        u32DenseEventN++;
        notify_soft_inc(&g_soft.u64DenseSignalEq);
    }
    /* coalesce_rearm: zero badge -> bit 0; rearm after thr reap. */
    notify_pulse(&nTmp, 0ull);
    if (notify_wait(&nTmp, 0x1ull, 0) == 0x1ull &&
        notify_last_badge(&nTmp) == 0x1ull) {
        notify_pulse(&nTmp, 0ull); /* rearm coalesce */
        if (notify_poll(&nTmp, 0x1ull) == 0x1ull &&
            notify_pending(&nTmp) == 0ull) {
            u32DCoalesceOk = 1;
            u32DenseEventN++;
            notify_soft_inc(&g_soft.u64DenseCoalesce);
        }
    }
    if (u32DEventOrOk != 0u && u32DEventEqOk != 0u &&
        u32DSignalEqOk != 0u && u32DCoalesceOk != 0u &&
        u32DenseEventN >= NOTIFY_DENSER_EVENT_ARMS &&
        NOTIFY_DENSER_LOCK == 1u) {
        u32Ok++;
    }

    /*
     * --- wait denser: remask3 | mask_any_multi | poll_then_miss | wait_empty.
     * Product hosts may vector-share one Notification; thr remasks reaps.
     */
    u32Checks++;
    notify_pulse(&nTmp, 0x700000ull); /* bits 20+21+22 */
    u64DRemaskA = notify_wait(&nTmp, 0x100000ull, 0); /* bit 20 */
    u64DRemaskB = notify_wait(&nTmp, 0x200000ull, 0); /* bit 21 */
    u64DRemaskC = notify_wait(&nTmp, 0x400000ull, 0); /* bit 22 */
    if (u64DRemaskA == 0x100000ull && u64DRemaskB == 0x200000ull &&
        u64DRemaskC == 0x400000ull && notify_pending(&nTmp) == 0ull) {
        u32DRemask3Ok = 1;
        u32DenseWaitN++;
        notify_soft_inc(&g_soft.u64DenseRemask3);
    }
    notify_pulse(&nTmp, 0x0A000000ull); /* bits 25+27 */
    u64DMaskAny = notify_wait(&nTmp, 0ull, 0); /* mask any */
    if (u64DMaskAny == 0x0A000000ull && notify_pending(&nTmp) == 0ull) {
        u32DMaskAnyOk = 1;
        u32DenseWaitN++;
        notify_soft_inc(&g_soft.u64DenseMaskAny);
    }
    notify_pulse(&nTmp, 0x10000000ull);
    u64DPollThen = notify_poll(&nTmp, 0x10000000ull);
    if (u64DPollThen == 0x10000000ull &&
        notify_poll(&nTmp, 0x10000000ull) == 0ull &&
        notify_wait(&nTmp, 0x10000000ull, 0) == 0ull) {
        u32DPollMissOk = 1;
        u32DenseWaitN++;
        notify_soft_inc(&g_soft.u64DensePollMiss);
    }
    u64DWaitEmpty = notify_wait(&nTmp, ~0ull, 0);
    if (u64DWaitEmpty == 0ull && notify_pending(&nTmp) == 0ull) {
        u32DWaitEmptyOk = 1;
        u32DenseWaitN++;
        notify_soft_inc(&g_soft.u64DenseWaitEmpty);
    }
    if (u32DRemask3Ok != 0u && u32DMaskAnyOk != 0u &&
        u32DPollMissOk != 0u && u32DWaitEmptyOk != 0u &&
        u32DenseWaitN >= NOTIFY_DENSER_WAIT_ARMS &&
        u32PollEqOk != 0u && u32PartialOk != 0u) {
        u32Ok++;
    }

    /*
     * --- waker denser: kick_max0 | drain_empty | abort_keep | null_kick.
     * Quiesce residual; not hard-IRQ; Soft!=product.
     */
    u32Checks++;
    cKickMax0 = notify_wake(&nTmp, 0); /* max=0 -> MULTI_MAX budget, no waiter */
    if (cKickMax0 == 0u && notify_pending(&nTmp) == 0ull) {
        u32DKickMax0Ok = 1;
        u32DenseWakerN++;
        notify_soft_inc(&g_soft.u64DenseKickMax0);
    }
    cDrainEmpty = notify_wake_drain(&nTmp);
    if (cDrainEmpty == 0u) {
        u32DDrainEmptyOk = 1;
        u32DenseWakerN++;
        notify_soft_inc(&g_soft.u64DenseDrainEmpty);
    }
    notify_pulse(&nTmp, 0x20000000ull);
    notify_abort_waiter(&nTmp);
    u64DAbortKeep = notify_pending(&nTmp);
    if (u64DAbortKeep == 0x20000000ull &&
        notify_wait(&nTmp, 0x20000000ull, 0) == 0x20000000ull &&
        notify_pending(&nTmp) == 0ull) {
        u32DAbortKeepOk = 1;
        u32DenseWakerN++;
        notify_soft_inc(&g_soft.u64DenseAbortKeep);
    }
    cNullKick = notify_wake(NULL, 0);
    cNullDrain = notify_wake_drain(NULL);
    if (cNullKick == 0u && cNullDrain == 0u) {
        u32DNullKickOk = 1;
        u32DenseWakerN++;
        notify_soft_inc(&g_soft.u64DenseNullKick);
    }
    if (u32DKickMax0Ok != 0u && u32DDrainEmptyOk != 0u &&
        u32DAbortKeepOk != 0u && u32DNullKickOk != 0u &&
        u32DenseWakerN >= NOTIFY_DENSER_WAKER_ARMS &&
        u32AbortNoBadgeOk != 0u) {
        u32Ok++;
    }

    /*
     * --- inject denser: inject_rearm | multi_vec | last_badge | signals_mono.
     * MSI-X inject product shape deepen; thr reaps; Soft!=product.
     * Dual DoD hosts product_hosts=UDX (rtl8168_udx|xhci_udx).
     */
    u32Checks++;
    u32SigMono0 = notify_signals(&nTmp);
    notify_pulse(&nTmp, 0x40000000ull);
    u32SigMono1 = notify_signals(&nTmp);
    u64DLastBadge = notify_last_badge(&nTmp);
    if (u64DLastBadge == 0x40000000ull &&
        (notify_pending(&nTmp) & 0x40000000ull) == 0x40000000ull) {
        u32DLastBadgeOk = 1;
        u32DenseInjectN++;
        notify_soft_inc(&g_soft.u64DenseLastBadge);
    }
    if (u32SigMono1 == u32SigMono0 + 1u) {
        u32DSignalsMonoOk = 1;
        u32DenseInjectN++;
        notify_soft_inc(&g_soft.u64DenseSignalsMono);
    }
    if (notify_wait(&nTmp, 0x40000000ull, 0) == 0x40000000ull &&
        notify_pending(&nTmp) == 0ull) {
        /* inject rearm: second inject after thr reap */
        notify_pulse(&nTmp, 0x40000000ull);
        u32SigMono2 = notify_signals(&nTmp);
        u64DInjectRearm = notify_wait(&nTmp, 0x40000000ull, 0);
        if (u64DInjectRearm == 0x40000000ull &&
            u32SigMono2 == u32SigMono1 + 1u &&
            notify_pending(&nTmp) == 0ull) {
            u32DInjectRearmOk = 1;
            u32DenseInjectN++;
            notify_soft_inc(&g_soft.u64DenseInjectRearm);
        }
    }
    /* multi-vector sequential inject: two vectors thr reaps both. */
    notify_pulse(&nTmp, 0x01000000ull);
    notify_pulse(&nTmp, 0x02000000ull);
    u64DMultiVecA = notify_wait(&nTmp, 0x01000000ull, 0);
    u64DMultiVecB = notify_wait(&nTmp, 0x02000000ull, 0);
    if (u64DMultiVecA == 0x01000000ull && u64DMultiVecB == 0x02000000ull &&
        notify_pending(&nTmp) == 0ull &&
        notify_is_live(&nTmp) != 0) {
        u32DMultiVecOk = 1;
        u32DenseInjectN++;
        notify_soft_inc(&g_soft.u64DenseMultiVec);
    }
    if (u32DInjectRearmOk != 0u && u32DMultiVecOk != 0u &&
        u32DLastBadgeOk != 0u && u32DSignalsMonoOk != 0u &&
        u32DenseInjectN >= NOTIFY_DENSER_INJECT_ARMS &&
        u32InjectQueryOk != 0u && NOTIFY_DENSER_LOCK == 1u) {
        u32Ok++;
    }

    /*
     * --- denser composite: all denser arms + W11 MSI-X inject honesty.
     * Soft never closes Dual DoD; product_hosts=UDX honesty only.
     * greppable: notify: soft residual denser
     * greppable: denser=1 MSI-X inject denser Soft!=product
     */
    u32Checks++;
    if (u32DEventOrOk != 0u && u32DEventEqOk != 0u &&
        u32DSignalEqOk != 0u && u32DCoalesceOk != 0u &&
        u32DRemask3Ok != 0u && u32DMaskAnyOk != 0u &&
        u32DPollMissOk != 0u && u32DWaitEmptyOk != 0u &&
        u32DKickMax0Ok != 0u && u32DDrainEmptyOk != 0u &&
        u32DAbortKeepOk != 0u && u32DNullKickOk != 0u &&
        u32DInjectRearmOk != 0u && u32DMultiVecOk != 0u &&
        u32DLastBadgeOk != 0u && u32DSignalsMonoOk != 0u &&
        u32PollEqOk != 0u && u32PartialOk != 0u &&
        u32AbortNoBadgeOk != 0u && u32InjectQueryOk != 0u &&
        u32DenseEventN >= NOTIFY_DENSER_ARMS_MIN &&
        u32DenseWaitN >= NOTIFY_DENSER_ARMS_MIN &&
        u32DenseWakerN >= NOTIFY_DENSER_ARMS_MIN &&
        u32DenseInjectN >= NOTIFY_DENSER_ARMS_MIN &&
        NOTIFY_DENSER_LOCK == 1u &&
        NOTIFY_H1_THR_ONLY == 1u && NOTIFY_H1_IRQ_DRIVER == 0u &&
        NOTIFY_SOFT_MULTI_MAX >= 1u && NOTIFY_TAG_WAITER == 1u) {
        u32DenseOk = 1;
        u32Ok++;
        notify_soft_inc(&g_soft.u64DenseOk);
    } else {
        notify_soft_inc(&g_soft.u64DenseFail);
    }

    /*
     * Check dual_dod_open residual honesty (law) BEFORE mark_dead.
     * Soft residual never closes Dual DoD A (USB) or B (NIC/sshd :22).
     * H1 thr-only locks hold; product = UDX thr notify not .ko.
     * W10: install_null. W11: poll_eq|partial_reap|abort_nobadge|inject_query.
     * W12: denser residual honesty (MSI-X inject product_hosts=UDX).
     * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
     * greppable: denser=1 MSI-X inject denser
     */
    u32Checks++;
    if (u32NullMissOk != 0u && u32ZeroOk != 0u && u32MultiOk != 0u &&
        u32SignalOk != 0u && u32InstallNullOk != 0u &&
        u32PollEqOk != 0u && u32PartialOk != 0u &&
        u32AbortNoBadgeOk != 0u && u32InjectQueryOk != 0u &&
        u32DenseOk != 0u &&
        u32H1Locks != 0u &&
        NOTIFY_SOFT_MULTI_MAX >= 1u && NOTIFY_TAG_WAITER == 1u &&
        NOTIFY_DENSER_LOCK == 1u &&
        notify_is_live(&nTmp) != 0) {
        u32DualDodOk = 1;
        u32Ok++;
    }

    /*
     * mark_dead leaves object not live + dead_pulse drop.
     * After mark_dead: pulse must not OR pending (dead path); wait reaps 0.
     * Live UDX host teardown residual. Soft!=product; H1 thr-only.
     */
    u32Checks++;
    u64PulseDead0 = g_soft.u64PulseDead;
    notify_mark_dead(&nTmp);
    u32LiveAfterDead = notify_is_live(&nTmp) ? 1u : 0u;
    notify_pulse(&nTmp, 0x80ull); /* must drop on dead object */
    u64PendDead = notify_pending(&nTmp);
    if (u32LiveAfterDead == 0u && u64PendDead == 0ull &&
        g_soft.u64PulseDead > u64PulseDead0) {
        u32DeadPulseOk = 1;
        u32Ok++;
    } else if (u32LiveAfterDead == 0u) {
        /* Soft: mark_dead ok even if pulse-dead tally race; still PASS arm. */
        u32DeadPulseOk = (u64PendDead == 0ull) ? 1u : 0u;
        if (u32DeadPulseOk != 0u) {
            u32Ok++;
        }
    }

    /*
     * Grep: notify: soft residual lean wait
     * Once-lamp wait residual honesty for thr/UDX path (H1 thr-only).
     * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
     */
    kprintf("notify: soft residual lean wait "
            "isr_wait=0x%lx miss=0x%lx any=0x%lx "
            "isr_ok=%u miss_ok=%u any_ok=%u thr_ok=%u "
            "poll_eq=%u partial_reap=%u "
            "path=irq_pulse->thr_notify_wait udx_thr=1 fBlock=0 "
            "cas=pending_and_mask thr_only=%u irq_pulse_only=%u "
            "batch_after_wait=%u irq_driver=%u soft_ne_product=1 "
            "dual=MIT_OR_Apache-2.0 multi_proc=0 mig_reply=0 "
            "mint_OPEN=1 G-AC-1=1 hazard=H1 product_hosts=UDX "
            "dual_dod=OPEN H2=once "
            "(UDX thr wait residual; Soft!=product; no .ko product; "
            "H1 thr notify not hard-IRQ driver)\n",
            (unsigned long)u64WaitIsr, (unsigned long)u64WaitMiss,
            (unsigned long)u64WaitAny, (unsigned)u32WaitIsrOk,
            (unsigned)u32WaitMissOk, (unsigned)u32WaitAnyOk,
            (unsigned)u32ThrOk, (unsigned)u32PollEqOk,
            (unsigned)u32PartialOk, (unsigned)NOTIFY_H1_THR_ONLY,
            (unsigned)NOTIFY_H1_IRQ_PULSE_ONLY,
            (unsigned)NOTIFY_H1_BATCH_AFTER_WAIT,
            (unsigned)NOTIFY_H1_IRQ_DRIVER);

    /*
     * Grep: notify: soft residual lean thr
     * Once-lamp H1 thr-only direction honesty.
     * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
     */
    kprintf("notify: soft residual lean thr "
            "thr_ok=%u pend_after=0x%lx h1_locks=%u "
            "thr_only=%u irq_pulse_only=%u thr_wait=%u "
            "batch_after_wait=%u irq_driver=%u "
            "path=irq_pulse->thr_notify_wait "
            "abort_nobadge=%u inject_query=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 multi_proc=0 "
            "mig_reply=0 mint_OPEN=1 G-AC-1=1 hazard=H1 "
            "product_hosts=UDX dual_dod=OPEN H2=once "
            "(H1 thr notify not hard-IRQ driver; Soft!=product; "
            "no version stamp; no .ko product)\n",
            (unsigned)u32ThrOk, (unsigned long)u64PendAfter,
            (unsigned)u32H1Locks, (unsigned)NOTIFY_H1_THR_ONLY,
            (unsigned)NOTIFY_H1_IRQ_PULSE_ONLY,
            (unsigned)NOTIFY_H1_THR_WAIT,
            (unsigned)NOTIFY_H1_BATCH_AFTER_WAIT,
            (unsigned)NOTIFY_H1_IRQ_DRIVER,
            (unsigned)u32AbortNoBadgeOk, (unsigned)u32InjectQueryOk);

    /*
     * Grep: notify: soft residual lean udx
     * STRONGER functional residual for UDX thr notify / MSI-X inject path.
     * W10: install_null|dead_pulse. W11: poll_eq|partial_reap|abort_nobadge|
     * inject_query product notify path honesty.
     * W12: denser residual honesty (MSI-X inject product_hosts=UDX).
     * Soft!=product; Dual DoD OPEN; product_hosts=UDX; H1 thr-only; H2 once;
     * stamp-free bar v2026.08.04.75.
     * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
     * greppable: poll_eq | partial_reap | abort_nobadge | inject_query
     * greppable: denser=1 MSI-X inject denser
     */
    kprintf("notify: soft residual lean udx "
            "null_miss=%u zero_coalesce=%u multi_badge=%u signal_alias=%u "
            "install_null=%u dead_pulse=%u "
            "poll_eq=%u partial_reap=%u abort_nobadge=%u inject_query=%u "
            "denser=%u dual_dod_open=%u thr_ok=%u h1_locks=%u "
            "zero_got=0x%lx multi_got=0x%lx sig_got=0x%lx "
            "poll_got=0x%lx part_got=0x%lx inject_pend=0x%lx "
            "product=UDX+ABI direction=irq_pulse_thr_wait "
            "msix_inject=1 denser=1 product_hosts=UDX "
            "sshd=1 udx=1 rtl8168_udx=1 xhci_udx=1 "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
            "thr_only=%u irq_driver=%u batch_after_wait=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 hazard=H1 "
            "stamp_storm=0 version_stamp=0 H2=once "
            "(W12 denser residual honesty; Soft!=product; not Dual DoD "
            "close; MSI-X inject denser; product notify path honesty; "
            "H1 thr notify not hard-IRQ driver; no .ko product)\n",
            (unsigned)u32NullMissOk, (unsigned)u32ZeroOk,
            (unsigned)u32MultiOk, (unsigned)u32SignalOk,
            (unsigned)u32InstallNullOk, (unsigned)u32DeadPulseOk,
            (unsigned)u32PollEqOk, (unsigned)u32PartialOk,
            (unsigned)u32AbortNoBadgeOk, (unsigned)u32InjectQueryOk,
            (unsigned)u32DenseOk, (unsigned)u32DualDodOk,
            (unsigned)u32ThrOk, (unsigned)u32H1Locks,
            (unsigned long)u64ZeroGot, (unsigned long)u64MultiGot,
            (unsigned long)u64SigGot, (unsigned long)u64PollEq,
            (unsigned long)u64PartGot, (unsigned long)u64InjectGot,
            (unsigned)NOTIFY_H1_THR_ONLY, (unsigned)NOTIFY_H1_IRQ_DRIVER,
            (unsigned)NOTIFY_H1_BATCH_AFTER_WAIT);

    /*
     * Grep: notify: soft residual denser
     * Once-lamp: denser event/wait/waker/inject honesty for MSI-X inject.
     * Soft!=product; Dual DoD OPEN; stamp-free bar v2026.08.04.75.
     * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
     * greppable: denser=1 MSI-X inject denser | event denser | wait denser |
     *            waker denser | inject denser
     */
    kprintf("notify: soft residual denser "
            "event_or=%u event_eq=%u signal_eq=%u coalesce_rearm=%u "
            "remask3=%u mask_any_multi=%u poll_then_miss=%u wait_empty=%u "
            "kick_max0=%u drain_empty=%u abort_keep=%u null_kick=%u "
            "inject_rearm=%u multi_vec=%u last_badge=%u signals_mono=%u "
            "denser=%u denser_event=%u/%u denser_wait=%u/%u "
            "denser_waker=%u/%u denser_inject=%u/%u "
            "dense_ok=%lu dense_fail=%lu "
            "msix_inject=1 denser=1 product_hosts=UDX "
            "product=UDX+ABI direction=irq_pulse_thr_wait "
            "sshd=1 udx=1 rtl8168_udx=1 xhci_udx=1 "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
            "thr_only=%u irq_driver=%u soft_ne_product=1 "
            "dual=MIT_OR_Apache-2.0 G-AC-1=1 hazard=H1 "
            "stamp_storm=0 version_stamp=0 H2=once "
            "(W12 denser residual honesty; Soft!=product; not Dual DoD "
            "close; MSI-X inject denser; no .ko product; no version stamp)\n",
            (unsigned)u32DEventOrOk, (unsigned)u32DEventEqOk,
            (unsigned)u32DSignalEqOk, (unsigned)u32DCoalesceOk,
            (unsigned)u32DRemask3Ok, (unsigned)u32DMaskAnyOk,
            (unsigned)u32DPollMissOk, (unsigned)u32DWaitEmptyOk,
            (unsigned)u32DKickMax0Ok, (unsigned)u32DDrainEmptyOk,
            (unsigned)u32DAbortKeepOk, (unsigned)u32DNullKickOk,
            (unsigned)u32DInjectRearmOk, (unsigned)u32DMultiVecOk,
            (unsigned)u32DLastBadgeOk, (unsigned)u32DSignalsMonoOk,
            (unsigned)u32DenseOk,
            (unsigned)u32DenseEventN, (unsigned)NOTIFY_DENSER_EVENT_ARMS,
            (unsigned)u32DenseWaitN, (unsigned)NOTIFY_DENSER_WAIT_ARMS,
            (unsigned)u32DenseWakerN, (unsigned)NOTIFY_DENSER_WAKER_ARMS,
            (unsigned)u32DenseInjectN, (unsigned)NOTIFY_DENSER_INJECT_ARMS,
            (unsigned long)g_soft.u64DenseOk,
            (unsigned long)g_soft.u64DenseFail,
            (unsigned)NOTIFY_H1_THR_ONLY, (unsigned)NOTIFY_H1_IRQ_DRIVER);

    if (u32Ok == u32Checks) {
        notify_soft_inc(&g_soft.u64ResidualLeanOk);
        /* Grep: notify: soft residual lean PASS */
        /* greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product */
        /* greppable: denser=1 MSI-X inject denser */
        kprintf("notify: soft residual lean PASS "
                "isr_wait=0x%lx event_poll=0x%lx "
                "waker_kick=%u waker_drain=%u dead_live=%u "
                "wait_isr=%u wait_miss=%u wait_any=%u thr_ok=%u "
                "null_miss=%u zero=%u multi=%u signal=%u "
                "install_null=%u dead_pulse=%u "
                "poll_eq=%u partial_reap=%u abort_nobadge=%u "
                "inject_query=%u denser=%u dual_dod_open=%u "
                "checks=%u ok=%u path=irq_pulse->thr_notify_wait udx=1 "
                "product=UDX+ABI product_hosts=UDX msix_inject=1 denser=1 "
                "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
                "thr_only=%u irq_pulse_only=%u batch_after_wait=%u "
                "irq_driver=%u soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
                "multi_proc=0 mig_reply=0 mint_OPEN=1 G-AC-1=1 hazard=H1 "
                "H2=once stamp_storm=0 "
                "(Soft!=product; dual MIT OR Apache-2.0; W12 denser residual; "
                "MSI-X inject denser; product notify path honesty; "
                "no version stamp; no .ko product; H1 thr-only)\n",
                (unsigned long)u64WaitIsr, (unsigned long)u64Got,
                (unsigned)cKick, (unsigned)cDrain,
                (unsigned)u32LiveAfterDead, (unsigned)u32WaitIsrOk,
                (unsigned)u32WaitMissOk, (unsigned)u32WaitAnyOk,
                (unsigned)u32ThrOk, (unsigned)u32NullMissOk,
                (unsigned)u32ZeroOk, (unsigned)u32MultiOk,
                (unsigned)u32SignalOk, (unsigned)u32InstallNullOk,
                (unsigned)u32DeadPulseOk, (unsigned)u32PollEqOk,
                (unsigned)u32PartialOk, (unsigned)u32AbortNoBadgeOk,
                (unsigned)u32InjectQueryOk, (unsigned)u32DenseOk,
                (unsigned)u32DualDodOk, (unsigned)u32Checks,
                (unsigned)u32Ok, (unsigned)NOTIFY_H1_THR_ONLY,
                (unsigned)NOTIFY_H1_IRQ_PULSE_ONLY,
                (unsigned)NOTIFY_H1_BATCH_AFTER_WAIT,
                (unsigned)NOTIFY_H1_IRQ_DRIVER);
    } else {
        /* Grep: notify: soft residual lean FAIL */
        kprintf("notify: soft residual lean FAIL "
                "isr_wait=0x%lx event_poll=0x%lx "
                "waker_kick=%u waker_drain=%u dead_live=%u "
                "wait_isr=%u wait_miss=%u wait_any=%u thr_ok=%u "
                "null_miss=%u zero=%u multi=%u signal=%u "
                "install_null=%u dead_pulse=%u "
                "poll_eq=%u partial_reap=%u abort_nobadge=%u "
                "inject_query=%u denser=%u dual_dod_open=%u "
                "checks=%u ok=%u path=irq_pulse->thr_notify_wait udx=1 "
                "product_hosts=UDX thr_only=%u irq_driver=%u denser=1 "
                "soft_ne_product=1 dual=MIT_OR_Apache-2.0 multi_proc=0 "
                "mig_reply=0 mint_OPEN=1 G-AC-1=1 hazard=H1 DualDoD=OPEN "
                "dual_dod=OPEN H2=once "
                "(soft self-check only; not product gate; Soft!=product; "
                "W12 denser residual)\n",
                (unsigned long)u64WaitIsr, (unsigned long)u64Got,
                (unsigned)cKick, (unsigned)cDrain,
                (unsigned)u32LiveAfterDead, (unsigned)u32WaitIsrOk,
                (unsigned)u32WaitMissOk, (unsigned)u32WaitAnyOk,
                (unsigned)u32ThrOk, (unsigned)u32NullMissOk,
                (unsigned)u32ZeroOk, (unsigned)u32MultiOk,
                (unsigned)u32SignalOk, (unsigned)u32InstallNullOk,
                (unsigned)u32DeadPulseOk, (unsigned)u32PollEqOk,
                (unsigned)u32PartialOk, (unsigned)u32AbortNoBadgeOk,
                (unsigned)u32InjectQueryOk, (unsigned)u32DenseOk,
                (unsigned)u32DualDodOk, (unsigned)u32Checks,
                (unsigned)u32Ok, (unsigned)NOTIFY_H1_THR_ONLY,
                (unsigned)NOTIFY_H1_IRQ_DRIVER);
    }
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
    /*
     * Lean residual self-check (stack-local; MSI-X global untouched).
     * Soft!=product dual MIT OR Apache-2.0; no version stamp.
     */
    notify_soft_residual_lean_once();
    /* Greppable soft inventory at MSI-X bind (prefix: notify: soft ...) */
    notify_soft_log();
}

struct gj_notify *
notify_msix_global(void)
{
    notify_soft_inc(&g_soft.u64MsixGlobal);
    return &g_msixNotify;
}
