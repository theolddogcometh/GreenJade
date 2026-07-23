/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal workqueue (schedule_work analogue).
 *
 * Soft path after IRQ:
 *   ISR (threaded) → udx_schedule_work → udx_run drains via udx_work_flush
 * Cancel/pending helpers for remove/quiesce races.
 */
#pragma once

#include <udx/types.h>

struct udx_work;

typedef void (*udx_work_fn_t)(struct udx_work *pWork);

struct udx_work {
    udx_work_fn_t    pfnFn;
    void            *pPriv;     /* soft state (driver fills) */
    u32              u32Pending;
    struct udx_work *pNext; /* UDX-internal queue link */
};

static inline void
udx_init_work(struct udx_work *pWork, udx_work_fn_t pfnFn)
{
    if (pWork == NULL) {
        return;
    }
    pWork->pfnFn = pfnFn;
    pWork->pPriv = NULL;
    pWork->u32Pending = 0;
    pWork->pNext = NULL;
}

/* Queue work to UDX host work thread / udx_run drain. */
udx_status_t udx_schedule_work(struct udx_work *pWork);

/**
 * Remove from queue if still pending and not running.
 * Returns UDX_OK if not pending or unlinked; UDX_ERR_BUSY if already running.
 */
udx_status_t udx_cancel_work(struct udx_work *pWork);

/** Non-zero if work is queued or currently executing (pending bit). */
int udx_work_pending(const struct udx_work *pWork);

/* Spinlock for host context (not hard-IRQ). */
struct udx_spinlock {
    u32 u32Lock;
};

static inline void
udx_spin_lock_init(struct udx_spinlock *pLock)
{
    if (pLock) {
        pLock->u32Lock = 0;
    }
}

void udx_spin_lock(struct udx_spinlock *pLock);
void udx_spin_unlock(struct udx_spinlock *pLock);
