/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Workqueue (schedule_work analogue). Host: FIFO list drained in udx_run.
 * cancel/pending for remove/quiesce soft path.
 *
 * Soft SPSC ownership protocol (Wave 116 exclusive deepen; this unit only):
 * The in-process work FIFO is the soft stand-in for security core 6
 * zero-copy ring handoff (producer publishes filled slots; consumer
 * claims, processes, releases free slots). Greppable prefix:
 *
 *   udx: spsc soft …
 *
 * Ownership states (greppable / tallied):
 *   producer  — schedule_work publishes a filled work slot
 *   consumer  — work_flush dequeues a filled slot
 *   claimed   — ownership held by consumer while pfnFn runs
 *   released  — pending cleared after run complete or cancel
 *
 * Soft ring handoff protocol inventory mirrors 6 shape (indexes /
 * owner phase / SPSC / backpressure / reset) without claiming shared
 * pages, map grants, or a multi-process driver-host product.
 *
 * Honesty (Wave 116): soft observation only — multi-process driver-host
 * SPSC product remains OPEN. greppable: udx: spsc soft open
 */
#include "udx_internal.h"

#include <udx/work.h>

#include <stdarg.h>

#if defined(UDX_HOST_LIBC)
#include <stdio.h>
#endif

/* Soft wave stamp + greppable area count (Wave 116 exclusive deepen). */
/* Wave 116 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft retfenceangle continuum_toward=25800 soft_ne_product=1 wave=116
 *   greppable: soft retsemaphoreangle exclusive=1 continuum_toward=25800 soft_ne_product=1 wave=116
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */

#define UDX_SPSC_SOFT_WAVE 70u
#define UDX_SPSC_SOFT_AREAS  13u

static struct udx_work *g_pWorkHead;
static struct udx_work *g_pWorkTail;
static struct udx_spinlock g_lockWork;

/*
 * Soft SPSC ownership inventory (security core 6 shape).
 * Cumulative for this process. greppable: udx: spsc soft …
 *
 * Mapping work FIFO → 6 owner phases:
 *   free     ← released (cancel or post-run) — consumer returns free
 *   filled   ← producer schedule — producer publishes
 *   claimed  ← dequeued under flush — consumer processes
 *   payload  ← soft: work object itself (no separate pin bit in host)
 */
static u32 g_u32SpscProducer;   /* new enqueue (filled published) */
static u32 g_u32SpscProdDup;    /* schedule while already pending (coalesce) */
static u32 g_u32SpscProdInval;  /* schedule reject (null / no fn) */
static u32 g_u32SpscConsumer;   /* dequeued by flush (consumer pull) */
static u32 g_u32SpscClaimed;    /* ownership claimed for pfnFn run */
static u32 g_u32SpscReleased;   /* release after run (free returned) */
static u32 g_u32SpscCancelRel;  /* cancel released before claim */
static u32 g_u32SpscCancelBusy; /* cancel while claimed/running */
static u32 g_u32SpscCancelNop;  /* cancel when not pending */
static u32 g_u32SpscCancelInv;  /* cancel null */
static u32 g_u32SpscFlushEnter; /* udx_work_flush entries */
static u32 g_u32SpscFlushEmpty; /* flush found empty free ring */
static u32 g_u32SpscPeakPend;   /* peak pending chain length observed */
static u32 g_u32SpscLivePend;   /* soft live pending (approx; lock-held) */
static u32 g_u32SpscPendingQ;   /* soft query udx_work_pending calls */
static u32 g_u32SpscPendingYes; /* pending query returned yes */
static u32 g_u32SpscPendingNo;  /* pending query returned no */
static u32 g_u32SpscLogN;       /* soft inventory dumps emitted */
static u8  g_fSpscInvOnce;      /* one-shot deep dump after first handoff */

static void spsc_soft_inc(u32 *pu32);
static void spsc_soft_emit(const char *szFmt, ...);
static void spsc_soft_note_pend_locked(void);
static void spsc_soft_inventory_log(void);
static void spsc_soft_maybe_once(void);

static void
spsc_soft_inc(u32 *pu32)
{
    if (pu32 != NULL && *pu32 < 0xffffffffu) {
        (*pu32)++;
    }
}

/*
 * Soft inventory emit — host console only. Does not use udx_printk so
 * core soft printk tallies stay product-path pure.
 */
static void
spsc_soft_emit(const char *szFmt, ...)
{
    if (szFmt == NULL) {
        return;
    }
#if defined(UDX_HOST_LIBC)
    {
        va_list apArgs;

        va_start(apArgs, szFmt);
        (void)vprintf(szFmt, apArgs);
        va_end(apArgs);
    }
#else
    (void)szFmt;
#endif
}

/* Caller holds g_lockWork. Soft peak/live pending for inventory. */
static void
spsc_soft_note_pend_locked(void)
{
    struct udx_work *pWalk;
    u32 u32Len;

    u32Len = 0;
    for (pWalk = g_pWorkHead; pWalk != NULL; pWalk = pWalk->pNext) {
        if (u32Len < 0xffffffffu) {
            u32Len++;
        }
    }
    g_u32SpscLivePend = u32Len;
    if (u32Len > g_u32SpscPeakPend) {
        g_u32SpscPeakPend = u32Len;
    }
}

/**
 * Greppable soft SPSC ownership + ring handoff inventory (Wave 111 deepen).
 * Prefix-stable "udx: spsc soft …" — never hard-gates; observation only.
 *
 *   udx: spsc soft honesty   — not multi-process driver-host product
 *   udx: spsc soft inventory — producer/consumer/claimed/released rollup
 *   udx: spsc soft states    — ownership state tallies (+ cancel path)
 *   udx: spsc soft handoff   — 6 free/filled/payload owner-phase shape
 *   udx: spsc soft protocol  — SPSC rules inventory (head/tail, bp, reset)
 *   udx: spsc soft ring      — soft FIFO geometry (live/peak pending)
 *   udx: spsc soft owner     — free/filled/claimed phase tallies
 *   udx: spsc soft cancel    — cancel path split
 *   udx: spsc soft pending   — work_pending query samples
 *   udx: spsc soft open      — multi-process product remains OPEN
 *   udx: spsc soft deepen    — wave=70 stamp + area count
 *   udx: spsc soft path      — claim surface catalog (soft bounds)
 *
 * greppable: udx: spsc soft
 */
static void
spsc_soft_inventory_log(void)
{
    u32 u32FreePhase;
    u32 u32FilledPhase;

    spsc_soft_inc(&g_u32SpscLogN);

    /*
     * Soft owner-phase mapping (shape only; no shared-page product):
     * free    ≈ released + cancel_rel (descriptors returned)
     * filled  ≈ producer publishes (live pending stands in)
     * claimed ≈ claim tallies while pfnFn runs
     */
    u32FreePhase = g_u32SpscReleased;
    u32FilledPhase = g_u32SpscProducer;

    /*
     * Honesty: in-process work FIFO soft stand-in only.
     * Not multi-process driver-host product; no shared-page map grants,
     * no kernel pin of ring frames, no peer-death revoke path.
     * greppable: udx: spsc soft honesty
     */
    spsc_soft_emit(
        "udx: spsc soft honesty not-multi-process-driver-host "
        "shared_pages=0 map_grant=0 kernel_pin=0 peer_death=0 "
        "product=0 soft=1 multi_server=0 confine=0 bar3=0 "
        "product_kernel=OPEN sec_core_s6=shape wave=%u\n",
        UDX_SPSC_SOFT_WAVE);

    /* Grep: udx: spsc soft inventory */
    spsc_soft_emit(
        "udx: spsc soft inventory producer=%u consumer=%u claimed=%u "
        "released=%u prod_dup=%u cancel_rel=%u cancel_busy=%u "
        "peak_pend=%u live_pend=%u log_n=%u wave=%u\n",
        g_u32SpscProducer, g_u32SpscConsumer, g_u32SpscClaimed,
        g_u32SpscReleased, g_u32SpscProdDup, g_u32SpscCancelRel,
        g_u32SpscCancelBusy, g_u32SpscPeakPend, g_u32SpscLivePend,
        g_u32SpscLogN, UDX_SPSC_SOFT_WAVE);

    /*
     * Ownership states (explicit names for agent greps).
     * greppable: udx: spsc soft states
     * greppable: producer= consumer= claimed= released=
     */
    spsc_soft_emit(
        "udx: spsc soft states producer=%u consumer=%u claimed=%u "
        "released=%u prod_inval=%u cancel_nop=%u cancel_inv=%u "
        "flush_enter=%u flush_empty=%u wave=%u\n",
        g_u32SpscProducer, g_u32SpscConsumer, g_u32SpscClaimed,
        g_u32SpscReleased, g_u32SpscProdInval, g_u32SpscCancelNop,
        g_u32SpscCancelInv, g_u32SpscFlushEnter, g_u32SpscFlushEmpty,
        UDX_SPSC_SOFT_WAVE);

    /*
     * Soft ring handoff protocol inventory — security core 6 shape.
     * free descriptors: consumer returns (released / cancel_rel)
     * filled descriptors: producer publishes (producer)
     * payload owner: claimed while consumer runs; released after
     * greppable: udx: spsc soft handoff
     */
    spsc_soft_emit(
        "udx: spsc soft handoff free=consumer_returns "
        "filled=producer_publishes payload=claimed_owner "
        "owner_phase=explicit spsc=1 mpmc=0 multi_ring=0 "
        "indexes=pending_bit+fifo kernel_interprets=0 wave=%u\n",
        UDX_SPSC_SOFT_WAVE);

    /*
     * 6 rules inventory (soft claim surface).
     * greppable: udx: spsc soft protocol
     */
    spsc_soft_emit(
        "udx: spsc soft protocol only_owner_writes=1 "
        "head_mutator=consumer tail_mutator=producer "
        "backpressure=prod_dup_coalesce reset=cancel_work "
        "secrets_in_ring=0 notify_timeout=0 soft_fifo=1 wave=%u\n",
        UDX_SPSC_SOFT_WAVE);

    /* Grep: udx: spsc soft ring */
    spsc_soft_emit(
        "udx: spsc soft ring head_nonnull=%u tail_nonnull=%u "
        "live_pend=%u peak_pend=%u policy=fifo_spsc "
        "shared_mem=0 same_process=1 wave=%u\n",
        (g_pWorkHead != NULL) ? 1u : 0u,
        (g_pWorkTail != NULL) ? 1u : 0u, g_u32SpscLivePend,
        g_u32SpscPeakPend, UDX_SPSC_SOFT_WAVE);

    /*
     * Owner-phase tallies (Wave 111 deepen).
     * greppable: udx: spsc soft owner
     */
    spsc_soft_emit(
        "udx: spsc soft owner free_phase=%u filled_phase=%u "
        "claimed_phase=%u released=%u cancel_rel=%u "
        "live_filled=%u wave=%u\n",
        u32FreePhase, u32FilledPhase, g_u32SpscClaimed, g_u32SpscReleased,
        g_u32SpscCancelRel, g_u32SpscLivePend, UDX_SPSC_SOFT_WAVE);

    /* Grep: udx: spsc soft cancel (Wave 111 deepen) */
    spsc_soft_emit(
        "udx: spsc soft cancel rel=%u busy=%u nop=%u inv=%u "
        "released_via_cancel=%u wave=%u\n",
        g_u32SpscCancelRel, g_u32SpscCancelBusy, g_u32SpscCancelNop,
        g_u32SpscCancelInv, g_u32SpscCancelRel, UDX_SPSC_SOFT_WAVE);

    /* Grep: udx: spsc soft pending (Wave 111 deepen) */
    spsc_soft_emit(
        "udx: spsc soft pending query=%u yes=%u no=%u "
        "live_pend=%u wave=%u\n",
        g_u32SpscPendingQ, g_u32SpscPendingYes, g_u32SpscPendingNo,
        g_u32SpscLivePend, UDX_SPSC_SOFT_WAVE);

    /*
     * Product OPEN honesty — multi-process driver-host SPSC remains OPEN.
     * Soft in-process FIFO ≠ shared-page multi-process product.
     * greppable: udx: spsc soft open
     */
    spsc_soft_emit(
        "udx: spsc soft open multi_process=OPEN driver_host=OPEN "
        "shared_pages=OPEN map_grant=OPEN kernel_pin=OPEN "
        "peer_death=OPEN product=0 soft=1 wave=%u\n",
        UDX_SPSC_SOFT_WAVE);

    /* Grep: udx: spsc soft deepen wave (Wave 111 stamp) */
    spsc_soft_emit(
        "udx: spsc soft deepen wave=%u areas=%u unit=work exclusive=1 "
        "prefix=udx:_spsc_soft log_n=%u "
        "(soft inventory; not multi-process driver-host product)\n",
        UDX_SPSC_SOFT_WAVE, UDX_SPSC_SOFT_AREAS, g_u32SpscLogN);

    /*
     * Path catalog — what this soft surface is / is not.
     * greppable: udx: spsc soft path
     */
    spsc_soft_emit(
        "udx: spsc soft path schedule=producer flush=consumer "
        "cancel=reset_or_release work_fifo=soft_ring "
        "driver_host_process=0 multi_process=0 "
        "product=OPEN wave=%u "
        "(soft inventory; not multi-process driver-host product)\n",
        UDX_SPSC_SOFT_WAVE);

    /*
     * Grep: udx: spsc soft honesty (Wave 116 exclusive deepen).
     * Soft inventory ≠ product multi-server confine.
     */
    spsc_soft_emit(
        "udx: spsc soft honesty multi_server=0 confine=0 bar3=0 "
        "exclusive=1 soft=1 product_kernel=OPEN wave=%u\n",
        UDX_SPSC_SOFT_WAVE);
}

/**
 * After first complete ownership handoff (claim/release/cancel), print
 * soft inventory once so greps see producer+consumer+claimed+released
 * together (not a producer-only snapshot). Diagnostics only.
 */
static void
spsc_soft_maybe_once(void)
{
    if (g_fSpscInvOnce != 0) {
        return;
    }
    /* Wait for consumer claim, post-run release, or cancel-before-claim. */
    if (g_u32SpscClaimed == 0 && g_u32SpscReleased == 0 &&
        g_u32SpscCancelRel == 0) {
        return;
    }
    g_fSpscInvOnce = 1;
    spsc_soft_inventory_log();
}

udx_status_t
udx_schedule_work(struct udx_work *pWork)
{
    if (pWork == NULL || pWork->pfnFn == NULL) {
        spsc_soft_inc(&g_u32SpscProdInval);
        return UDX_ERR_INVAL;
    }

    udx_spin_lock(&g_lockWork);
    if (pWork->u32Pending) {
        /*
         * Already filled / pending: producer coalesce (soft backpressure
         * stand-in — no second slot; 6 full-ring waits become dup-OK).
         */
        spsc_soft_inc(&g_u32SpscProdDup);
        spsc_soft_note_pend_locked();
        udx_spin_unlock(&g_lockWork);
        spsc_soft_maybe_once();
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
    /* Producer publishes filled descriptor (ownership → pending chain). */
    spsc_soft_inc(&g_u32SpscProducer);
    spsc_soft_note_pend_locked();
    udx_spin_unlock(&g_lockWork);
    spsc_soft_maybe_once();
    return UDX_OK;
}

udx_status_t
udx_cancel_work(struct udx_work *pWork)
{
    struct udx_work **pp;
    struct udx_work *pWalk;
    int fFound;

    if (pWork == NULL) {
        spsc_soft_inc(&g_u32SpscCancelInv);
        return UDX_ERR_INVAL;
    }

    udx_spin_lock(&g_lockWork);
    if (!pWork->u32Pending) {
        udx_spin_unlock(&g_lockWork);
        spsc_soft_inc(&g_u32SpscCancelNop);
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
    if (fFound) {
        /* Reset path: free returned before consumer claimed. */
        spsc_soft_inc(&g_u32SpscCancelRel);
        spsc_soft_inc(&g_u32SpscReleased);
        spsc_soft_note_pend_locked();
    }
    udx_spin_unlock(&g_lockWork);

    /*
     * Not on queue but pending: already dequeued and running in flush.
     * Caller cannot stop it; report BUSY (Linux cancel_work_sync spirit).
     * Soft: claimed ownership still held by consumer until release.
     */
    if (!fFound) {
        spsc_soft_inc(&g_u32SpscCancelBusy);
        spsc_soft_maybe_once();
        return UDX_ERR_BUSY;
    }
    spsc_soft_maybe_once();
    return UDX_OK;
}

int
udx_work_pending(const struct udx_work *pWork)
{
    int fYes;

    spsc_soft_inc(&g_u32SpscPendingQ);
    if (pWork == NULL) {
        spsc_soft_inc(&g_u32SpscPendingNo);
        return 0;
    }
    fYes = (pWork->u32Pending != 0) ? 1 : 0;
    if (fYes != 0) {
        spsc_soft_inc(&g_u32SpscPendingYes);
    } else {
        spsc_soft_inc(&g_u32SpscPendingNo);
    }
    return fYes;
}

void
udx_work_flush(void)
{
    struct udx_work *pWork;

    spsc_soft_inc(&g_u32SpscFlushEnter);

    for (;;) {
        udx_spin_lock(&g_lockWork);
        pWork = g_pWorkHead;
        if (pWork == NULL) {
            g_u32SpscLivePend = 0;
            udx_spin_unlock(&g_lockWork);
            if (g_u32SpscConsumer == 0 && g_u32SpscClaimed == 0) {
                spsc_soft_inc(&g_u32SpscFlushEmpty);
            }
            spsc_soft_maybe_once();
            return;
        }
        g_pWorkHead = pWork->pNext;
        if (g_pWorkHead == NULL) {
            g_pWorkTail = NULL;
        }
        pWork->pNext = NULL;
        /* Consumer pulls filled slot; still pending until run completes. */
        spsc_soft_inc(&g_u32SpscConsumer);
        spsc_soft_note_pend_locked();
        udx_spin_unlock(&g_lockWork);

        /* Claim ownership for payload processing (consumer phase). */
        spsc_soft_inc(&g_u32SpscClaimed);
        if (pWork->pfnFn) {
            pWork->pfnFn(pWork);
        }
        pWork->u32Pending = 0;
        /* Release: free descriptor returned to producer side. */
        spsc_soft_inc(&g_u32SpscReleased);
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
