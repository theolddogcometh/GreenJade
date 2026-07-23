/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Workqueue (schedule_work analogue). Host: FIFO list drained in udx_run.
 * cancel/pending for remove/quiesce soft path.
 */
#include "udx_internal.h"

#include <udx/work.h>

static struct udx_work *g_pWorkHead;
static struct udx_work *g_pWorkTail;
static struct udx_spinlock g_lockWork;

udx_status_t
udx_schedule_work(struct udx_work *pWork)
{
    if (pWork == NULL || pWork->pfnFn == NULL) {
        return UDX_ERR_INVAL;
    }

    udx_spin_lock(&g_lockWork);
    if (pWork->u32Pending) {
        udx_spin_unlock(&g_lockWork);
        return UDX_OK;
    }
    pWork->u32Pending = 1;
    pWork->pNext = NULL;
    if (g_pWorkTail == NULL) {
        g_pWorkHead = pWork;
        g_pWorkTail = pWork;
    } else {
        g_pWorkTail->pNext = pWork;
        g_pWorkTail = pWork;
    }
    udx_spin_unlock(&g_lockWork);
    return UDX_OK;
}

udx_status_t
udx_cancel_work(struct udx_work *pWork)
{
    struct udx_work **pp;
    struct udx_work *pWalk;
    int fFound;

    if (pWork == NULL) {
        return UDX_ERR_INVAL;
    }

    udx_spin_lock(&g_lockWork);
    if (!pWork->u32Pending) {
        udx_spin_unlock(&g_lockWork);
        return UDX_OK;
    }

    fFound = 0;
    for (pp = &g_pWorkHead; *pp != NULL; pp = &(*pp)->pNext) {
        if (*pp == pWork) {
            *pp = pWork->pNext;
            if (g_pWorkTail == pWork) {
                /* recompute tail */
                g_pWorkTail = NULL;
                for (pWalk = g_pWorkHead; pWalk != NULL; pWalk = pWalk->pNext) {
                    g_pWorkTail = pWalk;
                }
            }
            pWork->pNext = NULL;
            pWork->u32Pending = 0;
            fFound = 1;
            break;
        }
    }
    udx_spin_unlock(&g_lockWork);

    /*
     * Not on queue but pending: already dequeued and running in flush.
     * Caller cannot stop it; report BUSY (Linux cancel_work_sync spirit).
     */
    return fFound ? UDX_OK : UDX_ERR_BUSY;
}

int
udx_work_pending(const struct udx_work *pWork)
{
    if (pWork == NULL) {
        return 0;
    }
    return pWork->u32Pending != 0;
}

void
udx_work_flush(void)
{
    struct udx_work *pWork;

    for (;;) {
        udx_spin_lock(&g_lockWork);
        pWork = g_pWorkHead;
        if (pWork == NULL) {
            udx_spin_unlock(&g_lockWork);
            return;
        }
        g_pWorkHead = pWork->pNext;
        if (g_pWorkHead == NULL) {
            g_pWorkTail = NULL;
        }
        pWork->pNext = NULL;
        udx_spin_unlock(&g_lockWork);

        if (pWork->pfnFn) {
            pWork->pfnFn(pWork);
        }
        pWork->u32Pending = 0;
    }
}

void
udx_spin_lock(struct udx_spinlock *pLock)
{
    if (pLock == NULL) {
        return;
    }
    while (__atomic_exchange_n(&pLock->u32Lock, 1u, __ATOMIC_ACQUIRE)) {
        /* spin */
    }
}

void
udx_spin_unlock(struct udx_spinlock *pLock)
{
    if (pLock == NULL) {
        return;
    }
    __atomic_store_n(&pLock->u32Lock, 0u, __ATOMIC_RELEASE);
}
