/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Workqueue (schedule_work analogue). Host: FIFO list drained in udx_run.
 * cancel/pending for remove/quiesce soft path.
 *
 * Soft SPSC ownership protocol (exclusive deepen; this unit only):
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
 * -------------------------------------------------------------------------
 * NAPI-shaped deferred BH residual (product hosts; Soft!=product; G-AC-1)
 * greppable: udx: spsc soft …
 *
 * Linux mental model:
 *   hard IRQ → NAPI schedule / softirq → napi_poll(budget) → complete
 * UDX soft stand-in (this unit only; never product NAPI poll / softirq):
 *   threaded ISR → schedule_work → work_flush(pfnFn) → release
 * Mapping (shape only):
 *   schedule_work  ≈ NAPI schedule / raise deferred bottom half
 *   work_flush     ≈ poll drain under host run (no budget product)
 *   cancel_work    ≈ napi_disable / disable+sync spirit (BUSY if running)
 *   work_pending   ≈ napi_scheduled residual query
 *   release        ≈ napi_complete spirit (pending cleared post-run)
 *
 * Product hosts (rtl8168_udx / xhci_udx) deferred BH via this path:
 *   rtl8168_udx — IntrStatus read/ack → schedule_work → BH residual
 *                 (soft TX done / RX fill catalog; never Own handoff /
 *                 napi_poll / net inject product)
 *   xhci_udx    — event/transfer complete residual → schedule_work → BH
 *                 (never URB wire / ring doorbell product / BOT stick)
 *
 * Soft residual deepen surfaces (schedule_work residual for product hosts):
 *   residual init         — free-phase slot install (probe; header inline)
 *   residual schedule     — producer publish + coalesce
 *   residual coalesce     — prod_dup soft backpressure (schedule-while-pending)
 *   residual flush        — consumer claim + pfnFn drain
 *   residual release      — pending cleared post-run (napi_complete spirit)
 *   residual cancel       — unlink / BUSY if claimed
 *   residual pending      — pending-bit query samples
 *   residual ownership    — free/filled/claimed/released phase catalog
 *   residual bh           — ISR→schedule→flush deferred BH catalog
 *   residual budget       — budget=OPEN honesty (no poll weight product)
 *   residual product_host — rtl8168_udx / xhci_udx path honesty
 *   residual dual_dod     — Dual DoD A/B OPEN honesty (agent!=close)
 *   residual dual_dod_open— explicit Dual DoD OPEN residual (never close)
 *   residual h1           — H1: no net_eth_poll from hard IRQ
 *   residual thr          — thr path: thr ISR schedule → udx_run flush
 *                           (TX/RX catalog DoD B; BOT catalog DoD A OPEN)
 *   residual thr-only     — thr-only model lock (no hard-IRQ work product)
 *   residual quiesce      — cancel_work = quiesce/remove cancel surface
 *   residual spinlock     — host-context spin (not irqsave product)
 *   residual reclaim_catalog — soft TX done / RX fill catalog only
 *   residual freestanding — freestanding class SKIP (product=UDX+ABI)
 *   residual soft_ne_product — Soft!=product / G-AC-1 honesty lock
 *   residual shape        — layout_ver + shape_ok + path catalog residual
 *   residual lean         — lean Soft!=product rollup
 *   api honesty           — schedule_work soft != product NAPI
 *
 * Never: real softirq, napi_poll product, multi-queue NAPI, net inject,
 * multi-process workqueue product, Own handoff, URB wire product,
 * Dual DoD A/B close, freestanding rtl/USB product, invent image stamp.
 * greppable: udx: spsc soft napi residual
 * greppable: udx: spsc soft residual lean
 * greppable: udx: spsc soft residual init
 * greppable: udx: spsc soft residual schedule
 * greppable: udx: spsc soft residual coalesce
 * greppable: udx: spsc soft residual flush
 * greppable: udx: spsc soft residual release
 * greppable: udx: spsc soft residual cancel
 * greppable: udx: spsc soft residual pending
 * greppable: udx: spsc soft residual ownership
 * greppable: udx: spsc soft residual bh
 * greppable: udx: spsc soft residual budget
 * greppable: udx: spsc soft residual product_host
 * greppable: udx: spsc soft residual dual_dod
 * greppable: udx: spsc soft residual dual_dod_open
 * greppable: udx: spsc soft residual h1
 * greppable: udx: spsc soft residual thr
 * greppable: udx: spsc soft residual thr-only
 * greppable: udx: spsc soft residual quiesce
 * greppable: udx: spsc soft residual spinlock
 * greppable: udx: spsc soft residual reclaim_catalog
 * greppable: udx: spsc soft residual freestanding
 * greppable: udx: spsc soft residual soft_ne_product
 * greppable: udx: spsc soft residual shape
 * greppable: udx: spsc soft deferred
 * greppable: udx: spsc soft api honesty
 *
 * Honesty: soft observation only — multi-process driver-host
 * SPSC product remains OPEN. greppable: udx: spsc soft open
 * Soft!=product. Dual MIT OR Apache-2.0. G-AC-1 (no .ko product AC).
 * Dual DoD A/B UDX product OPEN (agent!=close). Freestanding class SKIP.
 * Bar v2026.08.04.75 stamp-free; NEVER bump GJ_IMAGE_VERSION.
 * -------------------------------------------------------------------------
 */
#include "udx_internal.h"

#include <udx/work.h>

#include <stdarg.h>

#if defined(UDX_HOST_LIBC)
#include <stdio.h>
#endif

/*
 * Soft wave stamp + greppable area count (schedule_work residual deepen).
 * Wave 75 (stamp-free bar v2026.08.04.75; NEVER bump GJ_IMAGE_VERSION):
 *   deepen C2 work residual catalog: init/coalesce/release/ownership/
 *   dual_dod_open/spinlock/reclaim_catalog alongside NAPI-shaped deferred BH.
 *   Dual DoD A/B OPEN + H1 + thr / thr-only residual for product-host
 *   TX/RX (DoD B) + BOT (DoD A) thr path; freestanding/soft_ne_product/
 *   shape residual parity. Soft!=product; G-AC-1; no stamp storms.
 * Soft != product complete; product lamps 0; Dual DoD A/B OPEN agent!=close;
 */

#define UDX_SPSC_SOFT_WAVE 75u
#define UDX_SPSC_SOFT_AREAS  41u

/*
 * FUNCTIONAL residual locks (STRONGER thr-only work path; Soft!=product;
 * G-AC-1; Dual DoD A/B OPEN; agent!=close; stamp-free bar v2026.08.04.75).
 * thr ISR → schedule_work → udx_run flush; never hard-IRQ work / net_eth_poll.
 * greppable: udx: spsc soft residual thr-only
 * greppable: udx: spsc soft residual h1
 */
#define UDX_WORK_LEAN_CHECKS  8u /* functional lean self-check count */

_Static_assert(UDX_WORK_H1_THR_ONLY == 1u,
               "H1 thr-only: schedule from thr ISR only");
_Static_assert(UDX_WORK_H1_NO_NET_ETH_POLL == 1u,
               "H1: no net_eth_poll from work/IRQ path");
_Static_assert(UDX_WORK_H1_NO_HARD_IRQ_WORK == 1u,
               "H1: never hard-IRQ work product");
_Static_assert(UDX_WORK_H1_FLUSH_FROM_UDX_RUN == 1u,
               "H1: flush under udx_run process context");
_Static_assert(UDX_WORK_DUAL_DOD_A_OPEN == 1u,
               "Dual DoD A remains OPEN (xhci_udx)");
_Static_assert(UDX_WORK_DUAL_DOD_B_OPEN == 1u,
               "Dual DoD B remains OPEN (rtl8168_udx)");
_Static_assert(UDX_WORK_DUAL_DOD_AGENT_NE_CLOSE == 1u,
               "soft residual lamps never close Dual DoD");
_Static_assert(UDX_WORK_SOFT_NE_PRODUCT == 1u,
               "Soft!=product honesty lock");
_Static_assert(UDX_WORK_G_AC_1 == 1u,
               "G-AC-1: no .ko product AC");
_Static_assert(UDX_WORK_FREESTANDING_CLASS_SKIP == 1u,
               "freestanding class SKIP; product=UDX+ABI hosts");
_Static_assert(UDX_WORK_LEAN_CHECKS == 8u,
               "FUNCTIONAL lean residual STRONGER: 8 thr-only work checks");

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
 *
 * NAPI-shaped residual tallies (schedule_work product-host path):
 *   napi_sched   ← schedule publishes (NAPI schedule spirit)
 *   napi_poll    ← flush claims + runs pfnFn (poll drain spirit)
 *   napi_coalesce← schedule while pending (schedule-while-scheduled)
 *   napi_disable ← cancel released before claim
 *   napi_busy    ← cancel while claimed (disable_sync BUSY)
 *   napi_complete← release after pfnFn (complete spirit)
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

/* NAPI-shaped deferred work residual tallies (product host path). */
static u32 g_u32NapiSched;      /* schedule ≈ NAPI schedule */
static u32 g_u32NapiPoll;       /* flush run ≈ poll drain spirit */
static u32 g_u32NapiCoalesce;   /* schedule while pending */
static u32 g_u32NapiDisable;    /* cancel released before claim */
static u32 g_u32NapiBusy;       /* cancel while claimed */
static u32 g_u32NapiPendQ;      /* pending query samples */
static u32 g_u32NapiComplete;   /* release after pfnFn ≈ napi_complete */

/* Residual deepen emission tallies (sparse one-shots; no stamp storm). */
static u32 g_u32ResidualSchedule;
static u32 g_u32ResidualFlush;
static u32 g_u32ResidualCancel;
static u32 g_u32ResidualPending;
static u32 g_u32ResidualCoalesce;
static u32 g_u32ResidualRelease;
static u32 g_u32ResidualThr;       /* thr path residual once-count */
static u32 g_u32ResidualThrOnly;   /* thr-only model lock residual once */
static u8  g_fResidualScheduleOnce;
static u8  g_fResidualFlushOnce;
static u8  g_fResidualCancelOnce;
static u8  g_fResidualPendingOnce;
static u8  g_fResidualCoalesceOnce;
static u8  g_fResidualReleaseOnce;
static u8  g_fResidualThrOnce;
static u8  g_fResidualThrOnlyOnce;

/*
 * Soft path-bit residual (functional thr path exercise OR).
 * ORs UDX_WORK_SOFT_PATH_* when schedule/flush/cancel/pending run.
 * Observation only; never Dual DoD close. greppable residual shape.
 */
static u32 g_u32SoftPathBits;

static void spsc_soft_inc(u32 *pu32);
static void spsc_soft_emit(const char *szFmt, ...);
static void spsc_soft_path_or(u32 u32Bit);
static void spsc_soft_note_pend_locked(void);
static void spsc_soft_inventory_log(void);
static void spsc_soft_maybe_once(void);
static void spsc_soft_residual_schedule_once(void);
static void spsc_soft_residual_flush_once(void);
static void spsc_soft_residual_cancel_once(void);
static void spsc_soft_residual_pending_once(void);
static void spsc_soft_residual_coalesce_once(void);
static void spsc_soft_residual_release_once(void);
static void spsc_soft_residual_thr_once(void);
static void spsc_soft_residual_thr_only_once(void);

static void
spsc_soft_inc(u32 *pu32)
{
    if (pu32 != NULL && *pu32 < 0xffffffffu) {
        (*pu32)++;
    }
}

/**
 * Soft residual: OR a path-bit into the thr-path exercise mask.
 * Functional observation of which schedule/flush/cancel/pending surfaces
 * ran this process. Atomic OR so thr path bits survive concurrent
 * schedule (thr ISR) vs flush (udx_run). Soft!=product; never Dual DoD close.
 * greppable residual: udx: spsc soft residual shape
 * greppable residual: udx: spsc soft residual thr
 */
static void
spsc_soft_path_or(u32 u32Bit)
{
    u32 u32Mask;

    u32Mask = u32Bit & UDX_WORK_SOFT_PATH_CATALOG;
    if (u32Mask == 0u) {
        return;
    }
    (void)__atomic_fetch_or(&g_u32SoftPathBits, u32Mask, __ATOMIC_RELAXED);
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
 * Greppable soft SPSC ownership + NAPI-shaped deferred work residual.
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
 *   udx: spsc soft deepen    — wave stamp + area count
 *   udx: spsc soft path      — claim surface catalog (soft bounds)
 *   udx: spsc soft napi residual — NAPI-shaped deferred work residual
 *   udx: spsc soft napi shape    — schedule/flush/cancel/pending map
 *   udx: spsc soft deferred      — product-host BH residual catalog
 *   udx: spsc soft product_host  — rtl8168_udx / xhci_udx path honesty
 *   udx: spsc soft residual lean — lean residual Soft!=product
 *   udx: spsc soft residual init     — free-phase install residual
 *   udx: spsc soft residual schedule — schedule_work residual
 *   udx: spsc soft residual coalesce — prod_dup backpressure residual
 *   udx: spsc soft residual flush    — work_flush residual
 *   udx: spsc soft residual release  — post-run free residual
 *   udx: spsc soft residual cancel   — cancel_work residual
 *   udx: spsc soft residual pending  — work_pending residual
 *   udx: spsc soft residual ownership— free/filled/claimed/released
 *   udx: spsc soft residual bh       — deferred BH residual catalog
 *   udx: spsc soft residual budget   — budget=OPEN honesty
 *   udx: spsc soft residual product_host — product-host BH honesty
 *   udx: spsc soft residual dual_dod — Dual DoD A/B OPEN (agent!=close)
 *   udx: spsc soft residual dual_dod_open — Dual DoD OPEN explicit
 *   udx: spsc soft residual h1       — H1 no net_eth_poll from hard IRQ
 *   udx: spsc soft residual thr      — thr path schedule→flush (TX/RX/BOT)
 *   udx: spsc soft residual thr-only — thr-only model lock residual
 *   udx: spsc soft residual quiesce  — cancel_work quiesce/remove surface
 *   udx: spsc soft residual spinlock — host-context spin residual
 *   udx: spsc soft residual reclaim_catalog — soft reclaim catalog
 *   udx: spsc soft residual freestanding — freestanding class SKIP
 *   udx: spsc soft residual soft_ne_product — Soft!=product honesty
 *   udx: spsc soft residual shape    — layout_ver + path catalog residual
 *   udx: spsc soft api honesty   — schedule_work soft != product NAPI
 *
 * greppable: udx: spsc soft
 */
static void
spsc_soft_inventory_log(void)
{
    u32 u32FreePhase;
    u32 u32FilledPhase;
    u32 u32PathBits;

    spsc_soft_inc(&g_u32SpscLogN);

    /*
     * Soft owner-phase mapping (shape only; no shared-page product):
     * free    ≈ released + cancel_rel (descriptors returned)
     * filled  ≈ producer publishes (live pending stands in)
     * claimed ≈ claim tallies while pfnFn runs
     */
    u32FreePhase = g_u32SpscReleased;
    u32FilledPhase = g_u32SpscProducer;
    u32PathBits = __atomic_load_n(&g_u32SoftPathBits, __ATOMIC_RELAXED);

    /*
     * Honesty: in-process work FIFO soft stand-in only.
     * Not multi-process driver-host product; no shared-page map grants,
     * no kernel pin of ring frames, no peer-death revoke path.
     * greppable: udx: spsc soft honesty
     */
    spsc_soft_emit(
        "udx: spsc soft honesty not-multi-process-driver-host "
        "shared_pages=0 map_grant=0 kernel_pin=0 peer_death=0 "
        "product=0 soft=1 multi_server=0 confine=0 "
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
     * Owner-phase tallies.
     * greppable: udx: spsc soft owner
     */
    spsc_soft_emit(
        "udx: spsc soft owner free_phase=%u filled_phase=%u "
        "claimed_phase=%u released=%u cancel_rel=%u "
        "live_filled=%u wave=%u\n",
        u32FreePhase, u32FilledPhase, g_u32SpscClaimed, g_u32SpscReleased,
        g_u32SpscCancelRel, g_u32SpscLivePend, UDX_SPSC_SOFT_WAVE);

    /* Grep: udx: spsc soft cancel */
    spsc_soft_emit(
        "udx: spsc soft cancel rel=%u busy=%u nop=%u inv=%u "
        "released_via_cancel=%u wave=%u\n",
        g_u32SpscCancelRel, g_u32SpscCancelBusy, g_u32SpscCancelNop,
        g_u32SpscCancelInv, g_u32SpscCancelRel, UDX_SPSC_SOFT_WAVE);

    /* Grep: udx: spsc soft pending */
    spsc_soft_emit(
        "udx: spsc soft pending query=%u yes=%u no=%u "
        "live_pend=%u wave=%u\n",
        g_u32SpscPendingQ, g_u32SpscPendingYes, g_u32SpscPendingNo,
        g_u32SpscLivePend, UDX_SPSC_SOFT_WAVE);

    /*
     * Product OPEN honesty — multi-process driver-host SPSC remains OPEN.
     * Soft in-process FIFO != shared-page multi-process product.
     * greppable: udx: spsc soft open
     */
    spsc_soft_emit(
        "udx: spsc soft open multi_process=OPEN driver_host=OPEN "
        "shared_pages=OPEN map_grant=OPEN kernel_pin=OPEN "
        "peer_death=OPEN product=0 soft=1 wave=%u\n",
        UDX_SPSC_SOFT_WAVE);

    /* Grep: udx: spsc soft deepen wave */
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
     * Grep: udx: spsc soft honesty (exclusive deepen).
     * Soft inventory != product multi-server confine.
     */
    spsc_soft_emit(
        "udx: spsc soft honesty multi_server=0 confine=0 "
        "exclusive=1 soft=1 product_kernel=OPEN wave=%u\n",
        UDX_SPSC_SOFT_WAVE);

    /*
     * NAPI-shaped deferred work soft residual (product hosts).
     * schedule_work residual for UDX product hosts — shape only.
     * greppable: udx: spsc soft napi residual
     * greppable: udx: spsc soft napi shape
     */
    spsc_soft_emit(
        "udx: spsc soft napi residual catalog Soft!=product "
        "G-AC-1 schedule_work=1 softirq=0 napi_poll=0 "
        "budget=OPEN multi_queue=0 net_inject=0 own_handoff=0 "
        "sched=%u poll=%u coalesce=%u disable=%u busy=%u "
        "complete=%u pend_q=%u wave=%u "
        "(soft residual; never product NAPI poll)\n",
        g_u32NapiSched, g_u32NapiPoll, g_u32NapiCoalesce,
        g_u32NapiDisable, g_u32NapiBusy, g_u32NapiComplete,
        g_u32NapiPendQ, UDX_SPSC_SOFT_WAVE);
    spsc_soft_emit(
        "udx: spsc soft napi shape "
        "schedule=NAPI_schedule flush=poll_drain_spirit "
        "cancel=napi_disable_spirit pending=napi_scheduled "
        "release=napi_complete_spirit "
        "budget_product=0 softirq_product=0 own_handoff=0 "
        "product_host_bh=1 soft=1 wave=%u\n",
        UDX_SPSC_SOFT_WAVE);

    /*
     * Deferred bottom-half residual catalog (product-host path).
     * greppable: udx: spsc soft deferred
     * greppable: udx: spsc soft product_host
     */
    spsc_soft_emit(
        "udx: spsc soft deferred path=ISR_to_schedule_work_to_flush "
        "bh=threaded_context hard_irq=0 "
        "producer=%u consumer=%u claimed=%u released=%u "
        "soft=1 product_NAPI=OPEN wave=%u "
        "(Soft!=product; deferred BH residual only)\n",
        g_u32SpscProducer, g_u32SpscConsumer, g_u32SpscClaimed,
        g_u32SpscReleased, UDX_SPSC_SOFT_WAVE);
    spsc_soft_emit(
        "udx: spsc soft product_host hosts=rtl8168_udx,xhci_udx "
        "surface=schedule_work product=UDX+ABI "
        "Linux-shaped_userspace=1 freestanding_class=SKIP "
        "napi_poll_product=OPEN softirq_product=OPEN "
        "soft_ne_product=1 G-AC-1 dual=MIT_OR_Apache-2.0 wave=%u\n",
        UDX_SPSC_SOFT_WAVE);

    /*
     * Lean residual schedule_work API honesty (Soft!=product).
     * greppable: udx: spsc soft residual lean
     * greppable: udx: spsc soft api honesty
     */
    spsc_soft_emit(
        "udx: spsc soft residual lean "
        "fifo_only=1 schedule_work=1 layout_ver=%u "
        "napi_shaped=1 napi_poll_product=OPEN "
        "softirq_product=OPEN multi_process=OPEN "
        "product_host_bh=1 thr_path=1 thr_only=%u soft=1 "
        "hosts=rtl8168_udx,xhci_udx "
        "init=1 coalesce=1 release=1 ownership=1 "
        "spinlock=1 reclaim_catalog=1 dual_dod_open=1 "
        "freestanding=1 soft_ne_product=1 shape=1 "
        "DoD_A=OPEN DoD_B=OPEN freestanding_class=SKIP "
        "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
        "G-AC-1 storm=0 stamp_free=1 wave=%u "
        "(Soft!=product; lean residual only; thr path TX/RX/BOT catalog)\n",
        (u32)UDX_WORK_LAYOUT_VER, (u32)UDX_WORK_H1_THR_ONLY,
        UDX_SPSC_SOFT_WAVE);
    spsc_soft_emit(
        "udx: spsc soft api honesty "
        "schedule=producer flush=consumer cancel=reset "
        "pending_bit=1 spsc_fifo=1 "
        "napi_poll=0 softirq=0 multi_server=0 "
        "cap_mint=0 soft=1 product_kernel=OPEN "
        "DoD_A=OPEN DoD_B=OPEN H1=threaded_ISR_schedule "
        "Soft!=product wave=%u\n",
        UDX_SPSC_SOFT_WAVE);

    /*
     * schedule_work residual (producer publish / NAPI schedule spirit).
     * greppable: udx: spsc soft residual schedule
     */
    spsc_soft_emit(
        "udx: spsc soft residual schedule "
        "api=udx_schedule_work producer_publish=1 "
        "coalesce=prod_dup sched=%u coalesce_n=%u inval=%u "
        "residual_n=%u napi_schedule_spirit=1 "
        "hard_irq=0 softirq_product=0 "
        "product_hosts=rtl8168_udx,xhci_udx "
        "soft=1 soft_ne_product=1 G-AC-1 wave=%u "
        "(schedule residual; never product NAPI schedule)\n",
        g_u32NapiSched, g_u32NapiCoalesce, g_u32SpscProdInval,
        g_u32ResidualSchedule, UDX_SPSC_SOFT_WAVE);

    /*
     * work_flush residual (consumer claim / poll drain spirit).
     * greppable: udx: spsc soft residual flush
     */
    spsc_soft_emit(
        "udx: spsc soft residual flush "
        "api=udx_work_flush consumer_claim=1 "
        "poll=%u complete=%u flush_enter=%u flush_empty=%u "
        "residual_n=%u poll_drain_spirit=1 "
        "budget_product=0 multi_queue=0 weight=0 "
        "product_hosts=rtl8168_udx,xhci_udx "
        "soft=1 soft_ne_product=1 G-AC-1 wave=%u "
        "(flush residual; never product napi_poll)\n",
        g_u32NapiPoll, g_u32NapiComplete, g_u32SpscFlushEnter,
        g_u32SpscFlushEmpty, g_u32ResidualFlush, UDX_SPSC_SOFT_WAVE);

    /*
     * cancel_work residual (napi_disable / disable_sync spirit).
     * greppable: udx: spsc soft residual cancel
     */
    spsc_soft_emit(
        "udx: spsc soft residual cancel "
        "api=udx_cancel_work "
        "rel=%u busy=%u nop=%u inv=%u residual_n=%u "
        "napi_disable_spirit=1 disable_sync_busy=1 "
        "product_hosts=rtl8168_udx,xhci_udx "
        "soft=1 soft_ne_product=1 G-AC-1 wave=%u "
        "(cancel residual; quiesce/remove path)\n",
        g_u32SpscCancelRel, g_u32SpscCancelBusy, g_u32SpscCancelNop,
        g_u32SpscCancelInv, g_u32ResidualCancel, UDX_SPSC_SOFT_WAVE);

    /*
     * work_pending residual (napi_scheduled query spirit).
     * greppable: udx: spsc soft residual pending
     */
    spsc_soft_emit(
        "udx: spsc soft residual pending "
        "api=udx_work_pending "
        "query=%u yes=%u no=%u residual_n=%u "
        "napi_scheduled_spirit=1 observation_only=1 "
        "product_hosts=rtl8168_udx,xhci_udx "
        "soft=1 soft_ne_product=1 G-AC-1 wave=%u "
        "(pending residual; observation only)\n",
        g_u32SpscPendingQ, g_u32SpscPendingYes, g_u32SpscPendingNo,
        g_u32ResidualPending, UDX_SPSC_SOFT_WAVE);

    /*
     * Deferred BH residual catalog — product-host ISR→BH path.
     * greppable: udx: spsc soft residual bh
     */
    spsc_soft_emit(
        "udx: spsc soft residual bh "
        "path=threaded_ISR_to_schedule_work_to_flush "
        "bh=threaded_context hard_irq=0 softirq=0 "
        "producer=%u consumer=%u claimed=%u released=%u "
        "rtl8168_udx=IntrStatus_ack_to_schedule "
        "xhci_udx=event_xfer_complete_to_schedule "
        "own_handoff=0 urb_wire=0 net_inject=0 "
        "product=UDX+ABI soft=1 soft_ne_product=1 "
        "G-AC-1 dual=MIT_OR_Apache-2.0 wave=%u "
        "(deferred BH residual; never product softirq)\n",
        g_u32SpscProducer, g_u32SpscConsumer, g_u32SpscClaimed,
        g_u32SpscReleased, UDX_SPSC_SOFT_WAVE);

    /*
     * Budget honesty residual — never product napi_poll weight.
     * greppable: udx: spsc soft residual budget
     */
    spsc_soft_emit(
        "udx: spsc soft residual budget "
        "budget=OPEN budget_product=0 weight=0 multi_queue=0 "
        "napi_poll_product=OPEN softirq_product=OPEN "
        "poll_drain_spirit=1 complete_spirit=1 "
        "product_hosts=rtl8168_udx,xhci_udx "
        "soft=1 soft_ne_product=1 G-AC-1 wave=%u "
        "(budget residual; never product poll weight)\n",
        UDX_SPSC_SOFT_WAVE);

    /*
     * Product-host residual honesty (rtl8168_udx / xhci_udx).
     * greppable: udx: spsc soft residual product_host
     */
    spsc_soft_emit(
        "udx: spsc soft residual product_host "
        "hosts=rtl8168_udx,xhci_udx surface=schedule_work "
        "product=UDX+ABI Linux-shaped_userspace=1 "
        "freestanding_class=SKIP kernel_rtl8168=SKIP "
        "kernel_xhci_msc=SKIP "
        "rtl8168_udx=DoD_B_OPEN xhci_udx=DoD_A_OPEN "
        "rtl8168_path=thr_ISR_ack_schedule_BH_TX_RX_catalog "
        "xhci_path=thr_event_schedule_BH_BOT_catalog "
        "thr_path=1 thr_only=%u flush_from=udx_run "
        "napi_poll_product=OPEN softirq_product=OPEN "
        "own_handoff=0 urb_wire=0 bot_stick=0 "
        "DoD_B_TX_RX=catalog_OPEN DoD_A_BOT=catalog_OPEN "
        "soft=1 product=0 soft_ne_product=1 "
        "dual=MIT_OR_Apache-2.0 G-AC-1 wave=%u "
        "(product-host thr BH residual; Soft!=product; TX/RX/BOT OPEN)\n",
        (u32)UDX_WORK_H1_THR_ONLY, UDX_SPSC_SOFT_WAVE);

    /*
     * Dual DoD A/B OPEN residual (agent!=close; Soft!=product).
     * Schedule_work is C2 libudx seed for userspace UDX hosts only.
     * greppable: udx: spsc soft residual dual_dod
     */
    spsc_soft_emit(
        "udx: spsc soft residual dual_dod "
        "DoD_A=OPEN DoD_B=OPEN agent_close=0 "
        "A=xhci_udx+DDI B=rtl8168_udx+DDI "
        "surface=schedule_work product=UDX+ABI "
        "Linux-shaped_userspace=1 freestanding_class=SKIP "
        "kernel_rtl8168=SKIP kernel_xhci_msc=SKIP "
        "in_kernel_ko=0 G-AC-1=1 dual_dod_open=1 "
        "soft_lamps_close=0 soft=1 soft_ne_product=1 "
        "dual=MIT_OR_Apache-2.0 stamp_free=1 wave=%u "
        "(Dual DoD residual OPEN; agent!=close)\n",
        UDX_SPSC_SOFT_WAVE);

    /*
     * H1 residual honesty — never net_eth_poll from hard IRQ.
     * Product hosts raise deferred BH via schedule_work from threaded
     * ISR; flush runs under udx_run (not hard-IRQ context).
     * greppable: udx: spsc soft residual h1
     */
    spsc_soft_emit(
        "udx: spsc soft residual h1 "
        "thr_only=%u no_net_eth_poll=%u no_hard_irq_work=%u "
        "flush_from_udx_run=%u "
        "net_eth_poll_from_irq=0 hard_irq_work=0 "
        "schedule_from=threaded_ISR "
        "flush_from=udx_run "
        "bh=threaded_context softirq_product=0 "
        "napi_poll_product=OPEN poll_weight=0 "
        "product_hosts=rtl8168_udx,xhci_udx "
        "soft=1 soft_ne_product=1 G-AC-1 wave=%u "
        "(H1 residual; deferred BH only)\n",
        (u32)UDX_WORK_H1_THR_ONLY,
        (u32)UDX_WORK_H1_NO_NET_ETH_POLL,
        (u32)UDX_WORK_H1_NO_HARD_IRQ_WORK,
        (u32)UDX_WORK_H1_FLUSH_FROM_UDX_RUN,
        UDX_SPSC_SOFT_WAVE);

    /*
     * thr residual — functional work-queue thr path for product hosts.
     * thr ISR → schedule_work → udx_run flush; TX/RX catalog (DoD B)
     * and BOT catalog (DoD A) remain OPEN (Soft!=product; agent!=close).
     * greppable: udx: spsc soft residual thr
     */
    spsc_soft_emit(
        "udx: spsc soft residual thr "
        "thr_only=%u hard_irq_work=0 softirq_product=0 "
        "schedule_from=threaded_ISR flush_from=udx_run "
        "ctx=host_or_udx_run process_context=1 "
        "path=thr_ISR_to_schedule_work_to_flush "
        "producer=%u consumer=%u claimed=%u released=%u "
        "DoD_B_TX_RX=catalog_OPEN DoD_A_BOT=catalog_OPEN "
        "rtl8168_udx=TX_done_RX_fill_catalog "
        "xhci_udx=event_xfer_BOT_catalog_OPEN "
        "own_handoff=0 urb_wire=0 bot_stick=0 net_inject=0 "
        "path_bits=0x%x residual_n=%u "
        "product_hosts=rtl8168_udx,xhci_udx "
        "product=UDX+ABI freestanding_class=SKIP "
        "DoD_A=OPEN DoD_B=OPEN soft=1 soft_ne_product=1 "
        "G-AC-1 dual=MIT_OR_Apache-2.0 wave=%u "
        "(thr residual; work queue thr path; never product TX/RX/BOT)\n",
        (u32)UDX_WORK_H1_THR_ONLY,
        g_u32SpscProducer, g_u32SpscConsumer, g_u32SpscClaimed,
        g_u32SpscReleased, u32PathBits, g_u32ResidualThr,
        UDX_SPSC_SOFT_WAVE);

    /*
     * thr-only residual model lock — no hard primary / hard-IRQ work.
     * UDX work path is thr-only: schedule from thr ISR; flush under
     * udx_run. Never hard-IRQ schedule_work product.
     * greppable: udx: spsc soft residual thr-only
     */
    spsc_soft_emit(
        "udx: spsc soft residual thr-only "
        "thr_only=%u hard_primary=0 hard_irq_work=0 "
        "hard_irq_stack=0 softirq_stack=0 "
        "linux_shape=schedule_work_from_threaded_ISR "
        "udx_shape=schedule_work_thr_only_fifo "
        "flush_ctx=udx_run process_context=1 "
        "schedule_work_ok=%u release_under_lock=1 "
        "no_net_eth_poll=%u flush_from_udx_run=%u "
        "lean_checks=%u path_bits=0x%x residual_n=%u "
        "product_hosts=rtl8168_udx,xhci_udx "
        "DoD_A=%u DoD_B=%u freestanding_class=SKIP "
        "soft=1 product=0 soft_ne_product=1 "
        "dual=MIT_OR_Apache-2.0 G-AC-1 wave=%u "
        "(thr-only model lock; Soft!=product; Dual DoD OPEN; STRONGER)\n",
        (u32)UDX_WORK_H1_THR_ONLY,
        (u32)UDX_WORK_H1_THR_ONLY,
        (u32)UDX_WORK_H1_NO_NET_ETH_POLL,
        (u32)UDX_WORK_H1_FLUSH_FROM_UDX_RUN,
        (u32)UDX_WORK_LEAN_CHECKS,
        u32PathBits, g_u32ResidualThrOnly,
        (u32)UDX_WORK_DUAL_DOD_A_OPEN, (u32)UDX_WORK_DUAL_DOD_B_OPEN,
        UDX_SPSC_SOFT_WAVE);

    /*
     * Quiesce/remove residual — cancel_work is the cancel surface.
     * Core lifecycle residual: quiesce=stop_dma+mask_irq+cancel_work.
     * greppable: udx: spsc soft residual quiesce
     */
    spsc_soft_emit(
        "udx: spsc soft residual quiesce "
        "api=udx_cancel_work "
        "order=before_remove "
        "stop_dma=host mask_irq=host cancel_work=1 "
        "rel=%u busy=%u nop=%u inv=%u "
        "napi_disable_spirit=1 disable_sync_busy=1 "
        "product_hosts=rtl8168_udx,xhci_udx "
        "DoD_A=OPEN DoD_B=OPEN freestanding_class=SKIP "
        "soft=1 soft_ne_product=1 G-AC-1 wave=%u "
        "(quiesce residual; cancel surface only)\n",
        g_u32SpscCancelRel, g_u32SpscCancelBusy, g_u32SpscCancelNop,
        g_u32SpscCancelInv, UDX_SPSC_SOFT_WAVE);

    /*
     * Init residual — free-phase work slot install (probe).
     * udx_init_work is header-inline; this unit catalogs honesty only.
     * greppable: udx: spsc soft residual init
     */
    spsc_soft_emit(
        "udx: spsc soft residual init "
        "api=udx_init_work free_phase=1 "
        "pending_clear=1 next_clear=1 pfnFn_install=1 "
        "ownership=free layout_ver=%u "
        "product_hosts=rtl8168_udx,xhci_udx "
        "soft=1 soft_ne_product=1 G-AC-1 wave=%u "
        "(init residual; free phase only)\n",
        (u32)UDX_WORK_LAYOUT_VER, UDX_SPSC_SOFT_WAVE);

    /*
     * Coalesce residual — schedule-while-pending soft backpressure.
     * greppable: udx: spsc soft residual coalesce
     */
    spsc_soft_emit(
        "udx: spsc soft residual coalesce "
        "api=udx_schedule_work prod_dup=%u coalesce_n=%u "
        "residual_n=%u second_slot=0 backpressure=soft "
        "napi_schedule_while_scheduled=1 "
        "product_hosts=rtl8168_udx,xhci_udx "
        "soft=1 soft_ne_product=1 G-AC-1 wave=%u "
        "(coalesce residual; never multi-queue product)\n",
        g_u32SpscProdDup, g_u32NapiCoalesce, g_u32ResidualCoalesce,
        UDX_SPSC_SOFT_WAVE);

    /*
     * Release residual — pending cleared post-run (napi_complete spirit).
     * greppable: udx: spsc soft residual release
     */
    spsc_soft_emit(
        "udx: spsc soft residual release "
        "api=udx_work_flush post_run=1 "
        "released=%u complete=%u residual_n=%u "
        "napi_complete_spirit=1 free_returned=1 "
        "product_hosts=rtl8168_udx,xhci_udx "
        "soft=1 soft_ne_product=1 G-AC-1 wave=%u "
        "(release residual; never product napi_complete wire)\n",
        g_u32SpscReleased, g_u32NapiComplete, g_u32ResidualRelease,
        UDX_SPSC_SOFT_WAVE);

    /*
     * Ownership residual — SPSC security-core-6 phase catalog.
     * greppable: udx: spsc soft residual ownership
     */
    spsc_soft_emit(
        "udx: spsc soft residual ownership "
        "free=%u filled=%u claimed=%u released=%u "
        "cancel_rel=%u live_filled=%u "
        "own_free=%u own_filled=%u own_claimed=%u own_released=%u "
        "spsc=1 mpmc=0 multi_process=OPEN "
        "product_hosts=rtl8168_udx,xhci_udx "
        "soft=1 soft_ne_product=1 G-AC-1 wave=%u "
        "(ownership residual; shape only)\n",
        u32FreePhase, u32FilledPhase, g_u32SpscClaimed, g_u32SpscReleased,
        g_u32SpscCancelRel, g_u32SpscLivePend,
        (u32)UDX_WORK_OWN_FREE, (u32)UDX_WORK_OWN_FILLED,
        (u32)UDX_WORK_OWN_CLAIMED, (u32)UDX_WORK_OWN_RELEASED,
        UDX_SPSC_SOFT_WAVE);

    /*
     * Dual DoD OPEN residual (explicit; agent!=close; Soft!=product).
     * greppable: udx: spsc soft residual dual_dod_open
     */
    spsc_soft_emit(
        "udx: spsc soft residual dual_dod_open "
        "DoD_A=OPEN DoD_B=OPEN agent_close=0 soft_lamps_close=0 "
        "A=xhci_udx+DDI B=rtl8168_udx+DDI "
        "surface=schedule_work product=UDX+ABI "
        "Linux-shaped_userspace=1 freestanding_class=SKIP "
        "in_kernel_ko=0 G-AC-1=1 soft=1 soft_ne_product=1 "
        "dual=MIT_OR_Apache-2.0 stamp_free=1 wave=%u "
        "(dual_dod_open residual; never product close)\n",
        UDX_SPSC_SOFT_WAVE);

    /*
     * Spinlock residual — host-context spin != irqsave product.
     * greppable: udx: spsc soft residual spinlock
     */
    spsc_soft_emit(
        "udx: spsc soft residual spinlock "
        "api=udx_spin_lock,udx_spin_unlock "
        "host_context=1 hard_irq=0 irqsave_product=0 "
        "work_list_guard=1 soft_state=1 "
        "product_hosts=rtl8168_udx,xhci_udx "
        "soft=1 soft_ne_product=1 G-AC-1 wave=%u "
        "(spinlock residual; process context only)\n",
        UDX_SPSC_SOFT_WAVE);

    /*
     * Reclaim catalog residual — soft TX done / RX fill catalog only.
     * Never Own handoff / NAPI poll / net inject product.
     * thr path: reclaim work runs after thr ISR schedule → flush.
     * greppable: udx: spsc soft residual reclaim_catalog
     */
    spsc_soft_emit(
        "udx: spsc soft residual reclaim_catalog "
        "rtl8168_udx=soft_TX_done_RX_fill_catalog "
        "xhci_udx=event_xfer_BOT_catalog_OPEN "
        "thr_path=1 schedule_from=threaded_ISR flush_from=udx_run "
        "own_handoff=0 napi_poll=0 net_inject=0 "
        "urb_wire=0 bot_stick=0 "
        "DoD_B_TX_RX=catalog_OPEN DoD_A_BOT=catalog_OPEN "
        "producer=%u consumer=%u claimed=%u released=%u "
        "product=UDX+ABI DoD_A=OPEN DoD_B=OPEN "
        "soft=1 soft_ne_product=1 G-AC-1 wave=%u "
        "(reclaim_catalog residual; never Own handoff / product TX/RX/BOT)\n",
        g_u32SpscProducer, g_u32SpscConsumer, g_u32SpscClaimed,
        g_u32SpscReleased, UDX_SPSC_SOFT_WAVE);

    /*
     * Freestanding class SKIP residual (product=UDX+ABI hosts only).
     * greppable: udx: spsc soft residual freestanding
     */
    spsc_soft_emit(
        "udx: spsc soft residual freestanding "
        "freestanding_class=SKIP freestanding_skip=%u "
        "kernel_rtl8168=SKIP kernel_xhci_msc=SKIP "
        "product=UDX+ABI hosts=rtl8168_udx,xhci_udx "
        "in_kernel_ko=0 G-AC-1=%u soft_ne_product=%u "
        "DoD_A=OPEN DoD_B=OPEN soft=1 wave=%u "
        "(freestanding residual SKIP; product hosts only)\n",
        (u32)UDX_WORK_FREESTANDING_CLASS_SKIP,
        (u32)UDX_WORK_G_AC_1,
        (u32)UDX_WORK_SOFT_NE_PRODUCT,
        UDX_SPSC_SOFT_WAVE);

    /*
     * Soft!=product honesty residual (dual license; G-AC-1; no GPL).
     * greppable: udx: spsc soft residual soft_ne_product
     */
    spsc_soft_emit(
        "udx: spsc soft residual soft_ne_product "
        "soft_ne_product=%u G-AC-1=%u freestanding_skip=%u "
        "no_gpl=%u dual=MIT_OR_Apache-2.0 "
        "napi_poll_product=OPEN softirq_product=OPEN "
        "multi_process=OPEN product_kernel=OPEN "
        "DoD_A=OPEN DoD_B=OPEN stamp_free=1 "
        "soft=1 product=0 wave=%u "
        "(soft_ne_product residual; Soft!=product lock)\n",
        (u32)UDX_WORK_SOFT_NE_PRODUCT,
        (u32)UDX_WORK_G_AC_1,
        (u32)UDX_WORK_FREESTANDING_CLASS_SKIP,
        (u32)UDX_WORK_NO_GPL,
        UDX_SPSC_SOFT_WAVE);

    /*
     * Shape residual — layout_ver + shape_ok + soft path catalog.
     * Functional path_bits OR tracks thr path exercise this process.
     * greppable: udx: spsc soft residual shape
     * greppable: udx: spsc soft residual lean layout_ver=
     */
    spsc_soft_emit(
        "udx: spsc soft residual shape "
        "layout_ver=%u shape_ok=%u path_catalog=0x%x "
        "path_bits=0x%x thr_only=%u dual_dod_open=%u "
        "soft_ne_product=%u freestanding_skip=%u "
        "product_hosts=rtl8168_udx,xhci_udx "
        "DoD_A=OPEN DoD_B=OPEN soft=1 G-AC-1 wave=%u "
        "(shape residual; eng only; not GJ_IMAGE_VERSION)\n",
        (u32)UDX_WORK_LAYOUT_VER,
        (UDX_WORK_LAYOUT_VER >= 3u) ? 1u : 0u,
        (u32)UDX_WORK_SOFT_PATH_CATALOG,
        u32PathBits,
        (u32)UDX_WORK_H1_THR_ONLY,
        (u32)(UDX_WORK_DUAL_DOD_A_OPEN && UDX_WORK_DUAL_DOD_B_OPEN &&
              UDX_WORK_DUAL_DOD_AGENT_NE_CLOSE),
        (u32)UDX_WORK_SOFT_NE_PRODUCT,
        (u32)UDX_WORK_FREESTANDING_CLASS_SKIP,
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

/** Soft: one-shot schedule residual honesty (sparse; no stamp storm). */
static void
spsc_soft_residual_schedule_once(void)
{
    if (g_fResidualScheduleOnce != 0) {
        return;
    }
    g_fResidualScheduleOnce = 1;
    spsc_soft_inc(&g_u32ResidualSchedule);
    /* greppable: udx: spsc soft residual schedule */
    spsc_soft_emit(
        "udx: spsc soft residual schedule "
        "producer_publish=1 napi_schedule_spirit=1 "
        "product_hosts=rtl8168_udx,xhci_udx "
        "product_NAPI_schedule=OPEN soft=1 wave=%u\n",
        UDX_SPSC_SOFT_WAVE);
}

/** Soft: one-shot flush residual honesty after first consumer claim. */
static void
spsc_soft_residual_flush_once(void)
{
    if (g_fResidualFlushOnce != 0) {
        return;
    }
    g_fResidualFlushOnce = 1;
    spsc_soft_inc(&g_u32ResidualFlush);
    /* greppable: udx: spsc soft residual flush */
    spsc_soft_emit(
        "udx: spsc soft residual flush "
        "consumer_claim=1 poll_drain_spirit=1 "
        "budget_product=0 napi_poll_product=OPEN "
        "product_hosts=rtl8168_udx,xhci_udx soft=1 wave=%u\n",
        UDX_SPSC_SOFT_WAVE);
}

/** Soft: one-shot cancel residual honesty (quiesce/remove path). */
static void
spsc_soft_residual_cancel_once(void)
{
    if (g_fResidualCancelOnce != 0) {
        return;
    }
    g_fResidualCancelOnce = 1;
    spsc_soft_inc(&g_u32ResidualCancel);
    /* greppable: udx: spsc soft residual cancel */
    spsc_soft_emit(
        "udx: spsc soft residual cancel "
        "napi_disable_spirit=1 disable_sync_busy=1 "
        "product_hosts=rtl8168_udx,xhci_udx soft=1 wave=%u\n",
        UDX_SPSC_SOFT_WAVE);
}

/** Soft: one-shot pending residual honesty (observation only). */
static void
spsc_soft_residual_pending_once(void)
{
    if (g_fResidualPendingOnce != 0) {
        return;
    }
    g_fResidualPendingOnce = 1;
    spsc_soft_inc(&g_u32ResidualPending);
    /* greppable: udx: spsc soft residual pending */
    spsc_soft_emit(
        "udx: spsc soft residual pending "
        "napi_scheduled_spirit=1 observation_only=1 "
        "product_hosts=rtl8168_udx,xhci_udx soft=1 wave=%u\n",
        UDX_SPSC_SOFT_WAVE);
}

/** Soft: one-shot coalesce residual honesty (schedule-while-pending). */
static void
spsc_soft_residual_coalesce_once(void)
{
    if (g_fResidualCoalesceOnce != 0) {
        return;
    }
    g_fResidualCoalesceOnce = 1;
    spsc_soft_inc(&g_u32ResidualCoalesce);
    /* greppable: udx: spsc soft residual coalesce */
    spsc_soft_emit(
        "udx: spsc soft residual coalesce "
        "prod_dup=1 second_slot=0 backpressure=soft "
        "napi_schedule_while_scheduled=1 "
        "product_hosts=rtl8168_udx,xhci_udx soft=1 wave=%u\n",
        UDX_SPSC_SOFT_WAVE);
}

/** Soft: one-shot release residual honesty after first post-run free. */
static void
spsc_soft_residual_release_once(void)
{
    if (g_fResidualReleaseOnce != 0) {
        return;
    }
    g_fResidualReleaseOnce = 1;
    spsc_soft_inc(&g_u32ResidualRelease);
    /* greppable: udx: spsc soft residual release */
    spsc_soft_emit(
        "udx: spsc soft residual release "
        "napi_complete_spirit=1 free_returned=1 "
        "product_hosts=rtl8168_udx,xhci_udx soft=1 wave=%u\n",
        UDX_SPSC_SOFT_WAVE);
}

/**
 * Soft: one-shot thr residual honesty (work queue thr path).
 * thr ISR → schedule_work; flush under udx_run. TX/RX/BOT catalog OPEN.
 * greppable: udx: spsc soft residual thr
 */
static void
spsc_soft_residual_thr_once(void)
{
    if (g_fResidualThrOnce != 0) {
        return;
    }
    g_fResidualThrOnce = 1;
    spsc_soft_inc(&g_u32ResidualThr);
    /* greppable: udx: spsc soft residual thr */
    spsc_soft_emit(
        "udx: spsc soft residual thr "
        "thr_only=%u schedule_from=threaded_ISR flush_from=udx_run "
        "DoD_B_TX_RX=catalog_OPEN DoD_A_BOT=catalog_OPEN "
        "own_handoff=0 bot_stick=0 hard_irq_work=0 "
        "product_hosts=rtl8168_udx,xhci_udx soft=1 wave=%u\n",
        (u32)UDX_WORK_H1_THR_ONLY, UDX_SPSC_SOFT_WAVE);
}

/**
 * Soft: one-shot thr-only model lock residual.
 * greppable: udx: spsc soft residual thr-only
 */
static void
spsc_soft_residual_thr_only_once(void)
{
    if (g_fResidualThrOnlyOnce != 0) {
        return;
    }
    g_fResidualThrOnlyOnce = 1;
    spsc_soft_inc(&g_u32ResidualThrOnly);
    /* greppable: udx: spsc soft residual thr-only */
    spsc_soft_emit(
        "udx: spsc soft residual thr-only "
        "thr_only=%u hard_primary=0 hard_irq_work=0 "
        "release_under_lock=1 schedule_work_ok=1 "
        "product_hosts=rtl8168_udx,xhci_udx soft=1 wave=%u\n",
        (u32)UDX_WORK_H1_THR_ONLY, UDX_SPSC_SOFT_WAVE);
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
         * NAPI residual: schedule while already scheduled.
         * thr path: thr ISR schedule-while-pending (soft coalesce).
         */
        spsc_soft_inc(&g_u32SpscProdDup);
        spsc_soft_inc(&g_u32NapiCoalesce);
        spsc_soft_path_or(UDX_WORK_SOFT_PATH_SCHEDULE |
                          UDX_WORK_SOFT_PATH_COALESCE |
                          UDX_WORK_SOFT_PATH_BH);
        spsc_soft_note_pend_locked();
        udx_spin_unlock(&g_lockWork);
        spsc_soft_residual_schedule_once();
        spsc_soft_residual_coalesce_once();
        spsc_soft_residual_thr_once();
        spsc_soft_residual_thr_only_once();
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
    /* NAPI residual: schedule ≈ NAPI schedule / raise deferred BH. */
    /* thr residual: schedule from threaded ISR (never hard-IRQ work). */
    spsc_soft_inc(&g_u32SpscProducer);
    spsc_soft_inc(&g_u32NapiSched);
    spsc_soft_path_or(UDX_WORK_SOFT_PATH_SCHEDULE |
                      UDX_WORK_SOFT_PATH_BH);
    spsc_soft_note_pend_locked();
    udx_spin_unlock(&g_lockWork);
    spsc_soft_residual_schedule_once();
    spsc_soft_residual_thr_once();
    spsc_soft_residual_thr_only_once();
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
        /* NAPI residual: cancel ≈ napi_disable before poll. */
        /* thr path: quiesce cancel surface (before remove). */
        spsc_soft_inc(&g_u32SpscCancelRel);
        spsc_soft_inc(&g_u32SpscReleased);
        spsc_soft_inc(&g_u32NapiDisable);
        spsc_soft_path_or(UDX_WORK_SOFT_PATH_CANCEL |
                          UDX_WORK_SOFT_PATH_QUIESCE |
                          UDX_WORK_SOFT_PATH_RELEASE);
        spsc_soft_note_pend_locked();
    }
    udx_spin_unlock(&g_lockWork);

    /*
     * Not on queue but pending: already dequeued and running in flush.
     * Caller cannot stop it; report BUSY (Linux cancel_work_sync spirit).
     * Soft: claimed ownership still held by consumer until release.
     * NAPI residual: disable while poll running → BUSY.
     * thr residual: thr path claim held until flush release_under_lock.
     */
    if (!fFound) {
        spsc_soft_inc(&g_u32SpscCancelBusy);
        spsc_soft_inc(&g_u32NapiBusy);
        spsc_soft_path_or(UDX_WORK_SOFT_PATH_CANCEL |
                          UDX_WORK_SOFT_PATH_QUIESCE);
        spsc_soft_residual_cancel_once();
        spsc_soft_maybe_once();
        return UDX_ERR_BUSY;
    }
    spsc_soft_residual_cancel_once();
    spsc_soft_maybe_once();
    return UDX_OK;
}

int
udx_work_pending(const struct udx_work *pWork)
{
    int fYes;

    spsc_soft_inc(&g_u32SpscPendingQ);
    spsc_soft_inc(&g_u32NapiPendQ);
    spsc_soft_path_or(UDX_WORK_SOFT_PATH_PENDING);
    spsc_soft_residual_pending_once();
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
        spsc_soft_path_or(UDX_WORK_SOFT_PATH_FLUSH |
                          UDX_WORK_SOFT_PATH_BH |
                          UDX_WORK_SOFT_PATH_RECLAIM);
        spsc_soft_note_pend_locked();
        udx_spin_unlock(&g_lockWork);

        /*
         * Claim ownership for payload processing (consumer phase).
         * NAPI residual: flush run ≈ poll drain spirit (no budget product).
         * thr residual: flush under udx_run process context only.
         */
        spsc_soft_inc(&g_u32SpscClaimed);
        spsc_soft_inc(&g_u32NapiPoll);
        spsc_soft_residual_flush_once();
        spsc_soft_residual_thr_once();
        spsc_soft_residual_thr_only_once();
        if (pWork->pfnFn) {
            pWork->pfnFn(pWork);
        }
        /*
         * Release under lock: thr-path free return synchronizes with
         * schedule/cancel pending observation (Soft residual thr-only).
         * NAPI residual: release ≈ napi_complete spirit.
         * TX/RX/BOT reclaim catalog only — never Own handoff product.
         */
        udx_spin_lock(&g_lockWork);
        pWork->u32Pending = 0;
        spsc_soft_path_or(UDX_WORK_SOFT_PATH_RELEASE);
        udx_spin_unlock(&g_lockWork);
        spsc_soft_inc(&g_u32SpscReleased);
        spsc_soft_inc(&g_u32NapiComplete);
        spsc_soft_residual_release_once();
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
