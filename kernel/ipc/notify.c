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
 * Greppable prefix-stable serial markers (notify: soft …):
 *   notify: soft inventory         — multi_max + path catalog at init/log
 *   notify: soft pulse inventory   — pulse/OR/wake catalog + counters
 *   notify: soft wait inventory    — wait/poll/CAS/block catalog + counters
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
 *   notify: soft abort             — abort soft multi-wake (no badge)
 *   notify: soft mark_dead         — mark_dead + abort path
 *   notify: soft install ok        — cap install success
 *   notify: soft install fail      — cap install rejected
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
 * Soft path sticky counters (wrap OK; diagnostics only).
 * Bumped on product return paths; never hard-gate behavior.
 * Pulse path is IRQ-callable → atomic RMW only (no kprintf).
 * greppable: notify: soft stats
 */
struct notify_soft_stats {
    u64 u64Init;             /* notify_init calls */
    u64 u64PulseHit;         /* pulse on live ready object */
    u64 u64PulseDead;        /* pulse dropped */
    u64 u64PulseZeroCoalesce;/* badge 0 → bit 0 */
    u64 u64PulseWake;        /* soft multi-wake from pulse */
    u64 u64PulseNoWaiter;    /* pulse with no waiter registered */
    u64 u64SignalAlias;      /* notify_signal → pulse */
    u64 u64WaitEnter;        /* notify_wait entries */
    u64 u64WaitHit;          /* CAS-clear matched bits */
    u64 u64WaitPark;         /* thread_block + schedule */
    u64 u64WaitPollMiss;     /* non-block / no thr miss */
    u64 u64WaitDead;         /* object DEAD during wait */
    u64 u64WaitCasRetry;     /* lost CAS race */
    u64 u64WaitSelfWake;     /* post-block self multi-wake */
    u64 u64WaitMaskAny;      /* mask==0 → ~0ull */
    u64 u64Poll;             /* notify_poll entries */
    u64 u64Abort;            /* notify_abort_waiter */
    u64 u64AbortWake;        /* abort issued multi-wake */
    u64 u64MarkDead;         /* notify_mark_dead */
    u64 u64InstallOk;        /* notify_install success */
    u64 u64InstallFail;      /* notify_install rejected */
    u64 u64MultiWakeCalls;   /* notify_soft_multi_wake invocations */
    u64 u64MultiWakeSum;     /* sum of thread_wake return counts */
    u64 u64SoftLog;          /* times soft inventory printed */
    u64 u64MsixInit;         /* notify_msix_init calls */
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
 * Greppable soft pulse/wait inventory + aggregate stats.
 * Called from notify_msix_init and once after first wait activity.
 * Never allocates; not for hard-IRQ (kprintf only from product paths).
 * greppable: notify: soft inventory
 * greppable: notify: soft pulse inventory
 * greppable: notify: soft wait inventory
 * greppable: notify: soft stats
 */
static void
notify_soft_log(void)
{
    struct notify_soft_stats s;

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
    s.u64PulseZeroCoalesce =
        __atomic_load_n(&g_soft.u64PulseZeroCoalesce, __ATOMIC_RELAXED);
    s.u64PulseWake =
        __atomic_load_n(&g_soft.u64PulseWake, __ATOMIC_RELAXED);
    s.u64PulseNoWaiter =
        __atomic_load_n(&g_soft.u64PulseNoWaiter, __ATOMIC_RELAXED);
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
    s.u64WaitCasRetry =
        __atomic_load_n(&g_soft.u64WaitCasRetry, __ATOMIC_RELAXED);
    s.u64WaitSelfWake =
        __atomic_load_n(&g_soft.u64WaitSelfWake, __ATOMIC_RELAXED);
    s.u64WaitMaskAny =
        __atomic_load_n(&g_soft.u64WaitMaskAny, __ATOMIC_RELAXED);
    s.u64Poll = __atomic_load_n(&g_soft.u64Poll, __ATOMIC_RELAXED);
    s.u64Abort = __atomic_load_n(&g_soft.u64Abort, __ATOMIC_RELAXED);
    s.u64AbortWake =
        __atomic_load_n(&g_soft.u64AbortWake, __ATOMIC_RELAXED);
    s.u64MarkDead =
        __atomic_load_n(&g_soft.u64MarkDead, __ATOMIC_RELAXED);
    s.u64InstallOk =
        __atomic_load_n(&g_soft.u64InstallOk, __ATOMIC_RELAXED);
    s.u64InstallFail =
        __atomic_load_n(&g_soft.u64InstallFail, __ATOMIC_RELAXED);
    s.u64MultiWakeCalls =
        __atomic_load_n(&g_soft.u64MultiWakeCalls, __ATOMIC_RELAXED);
    s.u64MultiWakeSum =
        __atomic_load_n(&g_soft.u64MultiWakeSum, __ATOMIC_RELAXED);
    s.u64SoftLog =
        __atomic_load_n(&g_soft.u64SoftLog, __ATOMIC_RELAXED);
    s.u64MsixInit =
        __atomic_load_n(&g_soft.u64MsixInit, __ATOMIC_RELAXED);

    /*
     * Catalog lines (prefix-stable): declare multi-waiter capacity and the
     * pulse/wait soft path surface so smoke/scripts can grep product depth
     * without parsing C.
     */
    /* Grep: notify: soft inventory */
    kprintf("notify: soft inventory multi_max=%u tag_waiter=%u "
            "paths=pulse,signal,wait,poll,abort,mark_dead,install,msix "
            "cas=pending_and_mask park=thread_block+schedule "
            "wake=thread_wake soft_log=%lu msix_init=%lu inits=%lu\n",
            (unsigned)NOTIFY_SOFT_MULTI_MAX, (unsigned)NOTIFY_TAG_WAITER,
            (unsigned long)s.u64SoftLog, (unsigned long)s.u64MsixInit,
            (unsigned long)s.u64Init);

    /* Grep: notify: soft pulse inventory */
    kprintf("notify: soft pulse inventory or=u64Pending "
            "zero_coalesce=bit0 wake=soft_multi_max irq_safe=atomics "
            "hit=%lu dead=%lu zero_coalesce=%lu wake=%lu nowaiter=%lu "
            "signal_alias=%lu multi_wake_calls=%lu multi_wake_sum=%lu\n",
            (unsigned long)s.u64PulseHit, (unsigned long)s.u64PulseDead,
            (unsigned long)s.u64PulseZeroCoalesce,
            (unsigned long)s.u64PulseWake,
            (unsigned long)s.u64PulseNoWaiter,
            (unsigned long)s.u64SignalAlias,
            (unsigned long)s.u64MultiWakeCalls,
            (unsigned long)s.u64MultiWakeSum);

    /* Grep: notify: soft wait inventory */
    kprintf("notify: soft wait inventory park=thread_block+schedule "
            "claim=cas_clear_matched multi_register=u32Waiters+pWaiter_hint "
            "paths=hit,park,poll_miss,dead,cas_retry,self_wake,mask_any "
            "enter=%lu hit=%lu park=%lu poll_miss=%lu dead=%lu "
            "cas_retry=%lu self_wake=%lu mask_any=%lu poll=%lu\n",
            (unsigned long)s.u64WaitEnter, (unsigned long)s.u64WaitHit,
            (unsigned long)s.u64WaitPark, (unsigned long)s.u64WaitPollMiss,
            (unsigned long)s.u64WaitDead, (unsigned long)s.u64WaitCasRetry,
            (unsigned long)s.u64WaitSelfWake, (unsigned long)s.u64WaitMaskAny,
            (unsigned long)s.u64Poll);

    /* Grep: notify: soft stats */
    kprintf("notify: soft stats init=%lu pulse_hit=%lu pulse_dead=%lu "
            "pulse_zero=%lu pulse_wake=%lu pulse_nowaiter=%lu "
            "signal_alias=%lu wait_enter=%lu wait_hit=%lu wait_park=%lu "
            "wait_poll_miss=%lu wait_dead=%lu wait_cas_retry=%lu "
            "wait_self_wake=%lu wait_mask_any=%lu poll=%lu "
            "abort=%lu abort_wake=%lu mark_dead=%lu "
            "install_ok=%lu install_fail=%lu multi_wake_calls=%lu "
            "multi_wake_sum=%lu soft_log=%lu msix_init=%lu\n",
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
            (unsigned long)s.u64MsixInit);
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
        return 0;
    }
    /* greppable: notify: soft multi wake */
    notify_soft_inc(&g_soft.u64MultiWakeCalls);
    cWoken = thread_wake(pN, NOTIFY_TAG_WAITER, NOTIFY_SOFT_MULTI_MAX);
    notify_soft_add(&g_soft.u64MultiWakeSum, (u64)cWoken);
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
    return notify_live(pN);
}

void
notify_pulse(struct gj_notify *pN, u64 u64Badge)
{
    if (pN == NULL || !pN->u32Ready) {
        /* greppable: notify: soft pulse dead */
        notify_soft_inc(&g_soft.u64PulseDead);
        return;
    }
    if (__atomic_load_n(&pN->hdr.u32State, __ATOMIC_ACQUIRE) !=
        (u32)GJ_OBJ_LIVE) {
        /* greppable: notify: soft pulse dead */
        notify_soft_inc(&g_soft.u64PulseDead);
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

    notify_soft_inc(&g_soft.u64WaitEnter);

    if (!notify_live(pN)) {
        /* greppable: notify: soft wait dead */
        notify_soft_inc(&g_soft.u64WaitDead);
        return 0;
    }
    /* mask==0 means "any badge" — greppable: NOTIFY_BADGE_WAIT */
    if (u64Mask == 0) {
        u64Mask = ~0ull;
        notify_soft_inc(&g_soft.u64WaitMaskAny);
    }
    pCur = thread_current();
    for (;;) {
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
            if (fRegistered) {
                if (pN->pWaiter == pCur) {
                    pN->pWaiter = NULL;
                }
                (void)__atomic_fetch_sub(&pN->u32Waiters, 1u, __ATOMIC_ACQ_REL);
            }
            /* greppable: notify: soft wait poll miss */
            notify_soft_inc(&g_soft.u64WaitPollMiss);
            notify_soft_log_once();
            return 0;
        }
        /*
         * Soft multi-waiter register: count + non-exclusive hint.
         * greppable: NOTIFY_SOFT_MULTI_WAITER
         */
        if (!fRegistered) {
            (void)__atomic_fetch_add(&pN->u32Waiters, 1u, __ATOMIC_ACQ_REL);
            fRegistered = 1;
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
    if (pN == NULL) {
        return 0;
    }
    return __atomic_load_n(&pN->u32Signals, __ATOMIC_ACQUIRE);
}

u64
notify_pending(const struct gj_notify *pN)
{
    /* Current uncleared badge bits (stats); 0 if object missing. */
    if (pN == NULL) {
        return 0;
    }
    return __atomic_load_n(&pN->u64Pending, __ATOMIC_ACQUIRE);
}

u64
notify_last_badge(const struct gj_notify *pN)
{
    if (pN == NULL) {
        return 0;
    }
    return __atomic_load_n(&pN->u64LastBadge, __ATOMIC_ACQUIRE);
}

u32
notify_waiters(const struct gj_notify *pN)
{
    /* Soft multi-waiter count (stats). greppable: NOTIFY_SOFT_MULTI_WAITER */
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
    } else {
        __atomic_store_n(&pN->hdr.u32State, (u32)GJ_OBJ_DEAD, __ATOMIC_RELEASE);
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
        return GJ_ERR_INVAL;
    }
    if (!notify_live(pN)) {
        /* greppable: notify: soft install fail */
        notify_soft_inc(&g_soft.u64InstallFail);
        return GJ_ERR_NODEV;
    }
    if (u16Rights == 0) {
        u16Rights = (u16)(GJ_RIGHT_READ | GJ_RIGHT_WAIT | GJ_RIGHT_IDENTIFY);
    }
    st = gj_cap_alloc_install(pProc->pCnode, (u16)GJ_CAP_NOTIFICATION,
                              u16Rights, &pN->hdr, pOutRef);
    if (st == GJ_OK) {
        /* greppable: notify: soft install ok */
        notify_soft_inc(&g_soft.u64InstallOk);
    } else {
        /* greppable: notify: soft install fail */
        notify_soft_inc(&g_soft.u64InstallFail);
    }
    return st;
}

void
notify_msix_init(void)
{
    if (g_fMsixInited && g_msixNotify.u32Ready &&
        g_msixNotify.hdr.u32State == (u32)GJ_OBJ_LIVE) {
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
    return &g_msixNotify;
}
