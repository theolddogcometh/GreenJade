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
    if (pN == NULL) {
        return 0;
    }
    return thread_wake(pN, NOTIFY_TAG_WAITER, NOTIFY_SOFT_MULTI_MAX);
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
        return;
    }
    if (__atomic_load_n(&pN->hdr.u32State, __ATOMIC_ACQUIRE) !=
        (u32)GJ_OBJ_LIVE) {
        return;
    }
    /* Badge 0 is not a valid event bit; coalesce to bit 0. */
    if (u64Badge == 0) {
        u64Badge = 1;
    }
    /* greppable: NOTIFY_BADGE_PULSE — OR pending, then soft multi-wake */
    (void)__atomic_fetch_or(&pN->u64Pending, u64Badge, __ATOMIC_ACQ_REL);
    __atomic_store_n(&pN->u64LastBadge, u64Badge, __ATOMIC_RELEASE);
    (void)__atomic_fetch_add(&pN->u32Signals, 1u, __ATOMIC_ACQ_REL);
    if (notify_has_waiter(pN)) {
        (void)notify_soft_multi_wake(pN);
    }
}

void
notify_signal(struct gj_notify *pN, u64 u64Badge)
{
    /* Stable alias: signal == pulse (badge bitmask). */
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

    if (!notify_live(pN)) {
        return 0;
    }
    /* mask==0 means "any badge" — greppable: NOTIFY_BADGE_WAIT */
    if (u64Mask == 0) {
        u64Mask = ~0ull;
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
                return u64Got;
            }
            /* Lost race with signal/clear — retry without sleeping. */
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
            (void)notify_soft_multi_wake(pN);
        }
        schedule();
        /* Drop exclusive-looking hint only if we still own it. */
        if (pN->pWaiter == pCur) {
            pN->pWaiter = NULL;
        }
    }
}

u64
notify_poll(struct gj_notify *pN, u64 u64Mask)
{
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
    if (notify_has_waiter(pN)) {
        (void)notify_soft_multi_wake(pN);
    }
}

void
notify_mark_dead(struct gj_notify *pN)
{
    if (pN == NULL) {
        return;
    }
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
    if (pProc == NULL || pN == NULL || pOutRef == NULL || pProc->pCnode == NULL) {
        return GJ_ERR_INVAL;
    }
    if (!notify_live(pN)) {
        return GJ_ERR_NODEV;
    }
    if (u16Rights == 0) {
        u16Rights = (u16)(GJ_RIGHT_READ | GJ_RIGHT_WAIT | GJ_RIGHT_IDENTIFY);
    }
    return gj_cap_alloc_install(pProc->pCnode, (u16)GJ_CAP_NOTIFICATION,
                                u16Rights, &pN->hdr, pOutRef);
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
}

struct gj_notify *
notify_msix_global(void)
{
    return &g_msixNotify;
}
