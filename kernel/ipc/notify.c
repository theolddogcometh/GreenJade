/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Notification badge objects (IRQ → userspace UDX path).
 *
 * Hard IRQ / soft inject only ORs badges and may wake one waiter — no alloc,
 * no copy. Userspace (or UDX) batches work after notify_wait returns.
 *
 * Wait key is the notify object; tag 1 = waiter blocked in notify_wait.
 * u64Pending is updated with atomics so IRQ and wait paths do not drop bits.
 */
#include <gj/cap.h>
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/notify.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/thread.h>

#define NOTIFY_TAG_WAITER 1u

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
notify_signal(struct gj_notify *pN, u64 u64Badge)
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
    (void)__atomic_fetch_or(&pN->u64Pending, u64Badge, __ATOMIC_ACQ_REL);
    __atomic_store_n(&pN->u64LastBadge, u64Badge, __ATOMIC_RELEASE);
    (void)__atomic_fetch_add(&pN->u32Signals, 1u, __ATOMIC_ACQ_REL);
    if (pN->pWaiter != NULL) {
        (void)thread_wake(pN, NOTIFY_TAG_WAITER, 1);
    }
}

u64
notify_wait(struct gj_notify *pN, u64 u64Mask, int fBlock)
{
    u64               u64Got;
    u64               u64Pend;
    u64               u64New;
    struct gj_thread *pCur;

    if (!notify_live(pN)) {
        return 0;
    }
    /* mask==0 means "any badge" */
    if (u64Mask == 0) {
        u64Mask = ~0ull;
    }
    pCur = thread_current();
    for (;;) {
        if (!notify_live(pN)) {
            pN->pWaiter = NULL;
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
                return u64Got;
            }
            /* Lost race with signal/clear — retry without sleeping. */
            continue;
        }
        /* Non-blocking, or no runnable thread context (e.g. early IRQ). */
        if (!fBlock || pCur == NULL) {
            return 0;
        }
        pN->pWaiter = pCur;
        thread_block(pN, NOTIFY_TAG_WAITER);
        /*
         * Signal may OR bits after the pending check and before BLOCKED.
         * Re-sample and self-wake so the badge is not lost.
         */
        u64Pend = __atomic_load_n(&pN->u64Pending, __ATOMIC_ACQUIRE);
        if ((u64Pend & u64Mask) != 0 || !notify_live(pN)) {
            (void)thread_wake(pN, NOTIFY_TAG_WAITER, 1);
        }
        schedule();
        pN->pWaiter = NULL;
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
    /* Lifetime signal count (stats); 0 if object missing. */
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

void
notify_abort_waiter(struct gj_notify *pN)
{
    if (pN == NULL) {
        return;
    }
    if (pN->pWaiter != NULL) {
        (void)thread_wake(pN, NOTIFY_TAG_WAITER, 1);
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
     * Stats snapshot at bind time: ready, cumulative signals, pending mask.
     * irq_msix later pulses badges; u32Signals/pending stay queryable via
     * notify_signals / notify_pending.
     */
    kprintf("notify: MSI-X global ready=%u signals=%u pending=0x%lx\n",
            g_msixNotify.u32Ready, g_msixNotify.u32Signals,
            (unsigned long)g_msixNotify.u64Pending);
}

struct gj_notify *
notify_msix_global(void)
{
    return &g_msixNotify;
}
