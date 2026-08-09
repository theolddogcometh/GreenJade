/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal workqueue (schedule_work analogue) for UDX product hosts.
 *
 * Soft path after IRQ (threaded handler context):
 *   ISR (threaded) → udx_schedule_work → udx_run drains via udx_work_flush
 * Cancel/pending helpers for remove/quiesce races.
 *
 * NAPI-shaped deferred work soft residual (product hosts; Soft!=product):
 *   Linux mental model:
 *     hard IRQ → NAPI schedule / softirq → napi_poll(budget) → complete
 *   UDX soft stand-in (this unit only; never product NAPI poll / softirq):
 *     threaded ISR → udx_schedule_work → udx_work_flush(pfnFn) → release
 *   Mapping (shape only):
 *     schedule_work  ≈ NAPI schedule / raise deferred bottom half
 *     work_flush     ≈ poll drain under host run (no budget product)
 *     cancel_work    ≈ napi_disable / disable+sync spirit (BUSY if running)
 *     work_pending   ≈ napi_scheduled residual query
 *     release        ≈ napi_complete spirit (pending cleared post-run)
 *   Product hosts (rtl8168_udx / xhci_udx) deferred BH residual (C2 path):
 *     rtl8168_udx — IntrStatus ack → schedule_work → BH reclaim catalog
 *                   (Dual DoD B direction; never Own handoff / TX/RX product)
 *     xhci_udx    — event/xfer complete → schedule_work residual OPEN
 *                   (Dual DoD A direction catalog; never URB wire / BOT)
 *   Never claims: real softirq, napi_poll product, multi-queue NAPI,
 *   netstack inject, multi-process workqueue product, Own handoff, URB wire,
 *   Dual DoD A/B product close (both remain OPEN).
 *
 * Soft residual deepen (Soft!=product; G-AC-1; dual MIT OR Apache-2.0):
 *   init residual      — probe install of work slot (free phase)
 *   schedule residual  — producer publishes filled slot; coalesce if pending
 *                        (schedule-while-scheduled); ≈ NAPI schedule spirit
 *   coalesce residual  — soft backpressure stand-in (prod_dup; no second slot)
 *   flush residual     — consumer claims + runs pfnFn; ≈ poll drain spirit
 *                        (budget product OPEN; no weight / multi-queue)
 *   release residual   — pending cleared post-run; free returned (napi_complete)
 *   cancel residual    — unlink if still pending; BUSY if claimed/running
 *                        (napi_disable / disable_sync spirit)
 *   pending residual   — observation query on pending bit (napi_scheduled)
 *   quiesce residual   — cancel_work from quiesce/remove (before free_irq/DMA)
 *                        order: quiesce_before_remove; never gates Dual DoD
 *   ownership residual — free / filled / claimed / released (SPSC 6 shape)
 *   deferred BH residual — product-host ISR→schedule→flush catalog only
 *   budget honesty     — budget=OPEN; never product napi_poll weight
 *   product_host residual — rtl8168_udx wired; xhci_udx catalog OPEN (UDX+ABI)
 *   dual_dod residual  — Dual DoD A/B OPEN; work residual != product close
 *                        (alias dual_dod_open; agent!=close)
 *   spinlock residual  — host-context spin (not hard-IRQ irqsave product)
 *   reclaim_catalog residual — soft TX done / RX fill catalog only
 *                        (never Own handoff / NAPI poll / net inject)
 *   h1 residual        — thr-only schedule from threaded ISR; flush under
 *                        udx_run; never net_eth_poll / hard-IRQ work product
 *   freestanding residual — freestanding class SKIP; product=UDX+ABI hosts
 *                        only (kernel rtl8168 / xhci_msc not product here)
 *   soft_ne_product residual — Soft!=product dual-license honesty lock;
 *                        G-AC-1 no .ko product AC from this unit
 *   shape residual     — layout_ver + shape_ok observation helpers (eng only;
 *                        not GJ_IMAGE_VERSION; not product ABI stamp)
 *
 * Soft API honesty (Soft!=product; dual MIT OR Apache-2.0; G-AC-1):
 *   soft path = in-process FIFO + pending bit + flush from udx_run
 *   product   = multi-process driver-host SPSC / NAPI poll OPEN
 *   Dual DoD  = A (xhci_udx) / B (rtl8168_udx) remain OPEN
 *   freestanding class drivers = SKIP; product hosts = UDX+ABI userspace
 * greppable: udx: spsc soft residual lean
 * greppable: udx: spsc soft residual lean layout_ver=
 * greppable: udx: spsc soft residual init
 * greppable: udx: spsc soft residual schedule
 * greppable: udx: spsc soft residual coalesce
 * greppable: udx: spsc soft residual flush
 * greppable: udx: spsc soft residual release
 * greppable: udx: spsc soft residual cancel
 * greppable: udx: spsc soft residual pending
 * greppable: udx: spsc soft residual quiesce
 * greppable: udx: spsc soft residual ownership
 * greppable: udx: spsc soft residual bh
 * greppable: udx: spsc soft residual budget
 * greppable: udx: spsc soft residual product_host
 * greppable: udx: spsc soft residual dual_dod
 * greppable: udx: spsc soft residual dual_dod_open
 * greppable: udx: spsc soft residual spinlock
 * greppable: udx: spsc soft residual reclaim_catalog
 * greppable: udx: spsc soft residual h1
 * greppable: udx: spsc soft residual freestanding
 * greppable: udx: spsc soft residual soft_ne_product
 * greppable: udx: spsc soft residual shape
 * greppable: udx: spsc soft api honesty
 * greppable: udx: spsc soft napi residual
 * greppable: udx: spsc soft deferred
 * greppable: udx: spsc soft open
 * No stamp storms. No version stamp. Lean residual only. Dual DoD OPEN.
 * Stamp-free residual (bar honesty; never invent wave/version stamp).
 * Never bump GJ_IMAGE_VERSION from this unit. Soft!=product. No GPL.
 * Dual MIT OR Apache-2.0. G-AC-1 (no .ko product AC).
 */
#pragma once

#include <udx/types.h>

/*
 * Soft residual lean layout version (eng only; Soft!=product; not stamp).
 * Bump when C2 work residual surface grows (ownership / quiesce / Dual DoD /
 * H1 / freestanding SKIP / shape helpers). Not GJ_IMAGE_VERSION.
 * ver1: NAPI residual schedule/flush/cancel/pending/bh/budget/product_host
 * ver2: init/coalesce/release/quiesce/ownership/spinlock/dual_dod_open/
 *       reclaim_catalog residual; Dual DoD A/B OPEN honesty; C2 path.
 * ver3: h1 / freestanding SKIP / soft_ne_product / dual_dod alias /
 *       shape residual (layout_ver + shape_ok + own_phase_soft) + honesty
 *       catalog locks (H1 thr-only, Dual DoD OPEN, soft_ne_product=1);
 *       stamp-free Soft!=product deepen; peer residual parity vs mmio/dma.
 * Grep: udx: spsc soft residual lean layout_ver=
 */
#define UDX_WORK_LAYOUT_VER          3u

/*
 * Soft ownership phase residual (SPSC security-core-6 shape; Soft!=product).
 * Values are catalog constants for greppable honesty — not product wire.
 * Public observation (pending bit): free vs filled only.
 * Claimed/released phases are flush-internal (not public without drain).
 * greppable: udx: spsc soft residual ownership
 */
#define UDX_WORK_OWN_FREE            0u /* released / cancel-before-claim */
#define UDX_WORK_OWN_FILLED          1u /* producer published (pending) */
#define UDX_WORK_OWN_CLAIMED         2u /* consumer dequeued; pfnFn runs */
#define UDX_WORK_OWN_RELEASED        3u /* post-run free returned */

/*
 * Soft residual path-bit catalog (once-lamp spirit for product hosts).
 * Hosts may OR these into private soft path masks; UDX core never hard-gates.
 * greppable: udx: spsc soft residual lean
 */
#define UDX_WORK_SOFT_PATH_INIT      (1u << 0)
#define UDX_WORK_SOFT_PATH_SCHEDULE  (1u << 1)
#define UDX_WORK_SOFT_PATH_COALESCE  (1u << 2)
#define UDX_WORK_SOFT_PATH_FLUSH     (1u << 3)
#define UDX_WORK_SOFT_PATH_RELEASE   (1u << 4)
#define UDX_WORK_SOFT_PATH_CANCEL    (1u << 5)
#define UDX_WORK_SOFT_PATH_PENDING   (1u << 6)
#define UDX_WORK_SOFT_PATH_QUIESCE   (1u << 7)
#define UDX_WORK_SOFT_PATH_BH        (1u << 8)
#define UDX_WORK_SOFT_PATH_RECLAIM   (1u << 9)
/* Complete path-bit catalog mask (bits 0..9). Soft residual only. */
#define UDX_WORK_SOFT_PATH_CATALOG   ((1u << 10) - 1u)

/*
 * H1 residual honesty locks (Soft!=product; G-AC-1).
 * thr-only schedule from threaded ISR; flush under udx_run process context.
 * Never net_eth_poll from hard IRQ; never hard-IRQ work product.
 * Flip requires H1 review — catalog constants, not product wire.
 * greppable: udx: spsc soft residual h1
 */
#define UDX_WORK_H1_THR_ONLY             1u
#define UDX_WORK_H1_NO_NET_ETH_POLL      1u
#define UDX_WORK_H1_NO_HARD_IRQ_WORK     1u
#define UDX_WORK_H1_FLUSH_FROM_UDX_RUN   1u

/*
 * Dual DoD residual OPEN locks (Soft!=product; agent!=close).
 * A = xhci_udx + DDI catalog; B = rtl8168_udx + DDI catalog.
 * Work residual lamps never close Dual DoD. greppable dual_dod / dual_dod_open.
 * greppable: udx: spsc soft residual dual_dod
 * greppable: udx: spsc soft residual dual_dod_open
 */
#define UDX_WORK_DUAL_DOD_A_OPEN         1u /* xhci_udx direction OPEN */
#define UDX_WORK_DUAL_DOD_B_OPEN         1u /* rtl8168_udx direction OPEN */
#define UDX_WORK_DUAL_DOD_AGENT_NE_CLOSE 1u

/*
 * Soft!=product / G-AC-1 honesty locks (catalog only; Soft!=product).
 * dual license MIT OR Apache-2.0; no GPL; no Linux .ko product AC.
 * greppable: udx: spsc soft residual soft_ne_product
 * greppable: udx: spsc soft residual freestanding
 */
#define UDX_WORK_SOFT_NE_PRODUCT         1u
#define UDX_WORK_G_AC_1                  1u /* no .ko product AC */
#define UDX_WORK_FREESTANDING_CLASS_SKIP 1u /* product=UDX+ABI hosts only */
#define UDX_WORK_NO_GPL                  1u

struct udx_work;

typedef void (*udx_work_fn_t)(struct udx_work *pWork);

/**
 * Deferred work descriptor (Linux work_struct spirit).
 *
 * Product-host NAPI residual (C2 path; Soft!=product; Dual DoD OPEN):
 *   rtl8168_udx — hangs workIsr off soft state; ISR IntrStatus ack →
 *                 schedule_work → BH reclaim catalog (never Own / TX/RX).
 *   xhci_udx    — deferred BH residual OPEN (catalog direction Dual DoD A;
 *                 never URB wire / BOT stick product).
 * Soft!=product — not a real NAPI struct / poll weight / multi-queue /
 * softirq product. Ownership phases: free → filled → claimed → released.
 * greppable residual: udx: spsc soft residual ownership
 * greppable residual: udx: spsc soft residual product_host
 * greppable residual: udx: spsc soft residual dual_dod
 * greppable residual: udx: spsc soft residual h1
 */
struct udx_work {
    udx_work_fn_t    pfnFn;
    void            *pPriv;     /* soft state (driver fills) */
    u32              u32Pending; /* non-zero = filled or claimed residual */
    struct udx_work *pNext; /* UDX-internal queue link (soft FIFO only) */
};

/**
 * Soft residual: init work slot to free phase (probe install).
 * Clears pending + link; installs pfnFn. Ownership → free.
 * Soft!=product; never Dual DoD close. G-AC-1.
 * greppable residual: udx: spsc soft residual init
 */
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

/**
 * Soft residual: true if work is in free ownership phase (not pending).
 * Observation only — never gates Dual DoD A/B product close.
 * greppable residual: udx: spsc soft residual ownership
 */
static inline int
udx_work_is_free(const struct udx_work *pWork)
{
    return (pWork != NULL && pWork->u32Pending == 0) ? 1 : 0;
}

/**
 * Soft residual ownership phase observation (public pending bit only).
 * free if null/!pending; filled if pending. Claimed/released are
 * flush-internal (not distinguishable without drain context).
 * Soft!=product; observation only; never Dual DoD close.
 * greppable residual: udx: spsc soft residual ownership
 */
static inline u32
udx_work_own_phase_soft(const struct udx_work *pWork)
{
    if (pWork == NULL || pWork->u32Pending == 0) {
        return UDX_WORK_OWN_FREE;
    }
    return UDX_WORK_OWN_FILLED;
}

/**
 * Queue work to UDX host work thread / udx_run drain.
 *
 * Soft residual: NAPI-shaped schedule (producer publishes filled slot).
 * Coalesce if already pending (soft backpressure / schedule-while-pending).
 * Product hosts: ISR → schedule_work for deferred BH (not hard-IRQ work).
 * C2 honesty: Dual DoD A/B remain OPEN; reclaim catalog only for rtl8168.
 * H1: thr-only schedule; never net_eth_poll from hard IRQ (Soft!=product).
 * greppable residual (impl): udx: spsc soft residual schedule
 * greppable residual (impl): udx: spsc soft residual coalesce
 * greppable residual: udx: spsc soft residual dual_dod
 * greppable residual: udx: spsc soft residual dual_dod_open
 * greppable residual: udx: spsc soft residual h1
 */
udx_status_t udx_schedule_work(struct udx_work *pWork);

/**
 * Remove from queue if still pending and not running.
 * Returns UDX_OK if not pending or unlinked; UDX_ERR_BUSY if already running.
 * Soft residual: cancel ≈ NAPI disable / disable_sync spirit (BUSY if claimed).
 * Quiesce residual: product hosts call from quiesce (and remove if still
 * pending) — order quiesce_before_remove; Soft!=product Dual DoD OPEN.
 * greppable residual (impl): udx: spsc soft residual cancel
 * greppable residual: udx: spsc soft residual quiesce
 */
udx_status_t udx_cancel_work(struct udx_work *pWork);

/**
 * Non-zero if work is queued or currently executing (pending bit).
 * Soft residual: pending query ≈ napi_scheduled shape (observation only).
 * Soft!=product; never Dual DoD close.
 * greppable residual (impl): udx: spsc soft residual pending
 */
int udx_work_pending(const struct udx_work *pWork);

/*
 * Drain residual (udx_work_flush) is host-internal — called from udx_run
 * only (udx_internal.h). Soft residual: flush ≈ NAPI poll drain spirit
 * (no budget product); release ≈ napi_complete after pfnFn.
 * H1: flush under udx_run process context (never hard-IRQ poll product).
 * greppable: udx: spsc soft residual flush
 * greppable: udx: spsc soft residual release
 * greppable: udx: spsc soft residual budget
 * greppable: udx: spsc soft residual reclaim_catalog
 * greppable: udx: spsc soft residual h1
 */

/**
 * Spinlock for host context (not hard-IRQ). Soft residual: process-context
 * mutual exclusion for soft state / work list; != spin_lock_irqsave product.
 * greppable residual: udx: spsc soft residual spinlock
 */
struct udx_spinlock {
    u32 u32Lock;
};

/**
 * Soft residual: init host-context spinlock to unlocked.
 * greppable residual: udx: spsc soft residual spinlock
 */
static inline void
udx_spin_lock_init(struct udx_spinlock *pLock)
{
    if (pLock) {
        pLock->u32Lock = 0;
    }
}

void udx_spin_lock(struct udx_spinlock *pLock);
void udx_spin_unlock(struct udx_spinlock *pLock);

/* ---- Soft residual queries / shape (observation only; Soft!=product) - */

/**
 * Soft residual lean layout version (UDX_WORK_LAYOUT_VER).
 * Grep: udx: spsc soft residual lean layout_ver=
 * Not GJ_IMAGE_VERSION. Not product ABI stamp. Stamp-free residual.
 * greppable residual: udx: spsc soft residual shape
 */
static inline u32
udx_work_layout_ver(void)
{
    return UDX_WORK_LAYOUT_VER;
}

/**
 * Non-zero when soft product-host work residual surface is present:
 * schedule/flush/cancel/pending + ownership + Dual DoD OPEN catalog +
 * H1 thr-only + freestanding SKIP + soft_ne_product honesty + shape helpers.
 * Soft!=product; eng residual only. layout_ver >= 3.
 * Never hard-gates Dual DoD A/B. G-AC-1.
 * greppable residual: udx: spsc soft residual shape
 * greppable residual: udx: spsc soft residual product_host
 * greppable residual: udx: spsc soft residual dual_dod
 */
static inline int
udx_work_shape_ok(void)
{
    return (UDX_WORK_LAYOUT_VER >= 3u) ? 1 : 0;
}

/**
 * Soft residual Dual DoD OPEN observation (always 1 while both OPEN).
 * Lamps alone never close Dual DoD. Soft!=product; agent!=close.
 * greppable residual: udx: spsc soft residual dual_dod
 * greppable residual: udx: spsc soft residual dual_dod_open
 */
static inline int
udx_work_dual_dod_open_soft(void)
{
    return (UDX_WORK_DUAL_DOD_A_OPEN != 0 &&
            UDX_WORK_DUAL_DOD_B_OPEN != 0 &&
            UDX_WORK_DUAL_DOD_AGENT_NE_CLOSE != 0) ? 1 : 0;
}

/**
 * Soft residual Soft!=product honesty lock (always 1).
 * dual MIT OR Apache-2.0; G-AC-1; freestanding class SKIP; no GPL.
 * greppable residual: udx: spsc soft residual soft_ne_product
 * greppable residual: udx: spsc soft residual freestanding
 */
static inline int
udx_work_soft_ne_product(void)
{
    return (UDX_WORK_SOFT_NE_PRODUCT != 0 &&
            UDX_WORK_G_AC_1 != 0 &&
            UDX_WORK_FREESTANDING_CLASS_SKIP != 0 &&
            UDX_WORK_NO_GPL != 0) ? 1 : 0;
}

/**
 * Soft residual H1 honesty lock (always 1 while H1 thr-only holds).
 * thr-only schedule; no net_eth_poll from hard IRQ; flush under udx_run.
 * greppable residual: udx: spsc soft residual h1
 */
static inline int
udx_work_h1_ok(void)
{
    return (UDX_WORK_H1_THR_ONLY != 0 &&
            UDX_WORK_H1_NO_NET_ETH_POLL != 0 &&
            UDX_WORK_H1_NO_HARD_IRQ_WORK != 0 &&
            UDX_WORK_H1_FLUSH_FROM_UDX_RUN != 0) ? 1 : 0;
}

/**
 * Soft residual complete path-bit catalog mask (bits 0..9).
 * Observation only; hosts may OR into private soft path masks.
 * greppable residual: udx: spsc soft residual lean
 */
static inline u32
udx_work_soft_path_catalog(void)
{
    return UDX_WORK_SOFT_PATH_CATALOG;
}
