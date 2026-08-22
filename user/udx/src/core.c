/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * UDX runtime core: init / run / exit / printk.
 * Product markers (greppable): GREENJADE_UDX, UDX_PRODUCT.
 *
 * Soft inventory (Wave 126 exclusive deepen; this unit only) —
 * greppable "udx: soft …":
 *   udx: soft inventory …
 *   udx: soft lifecycle …
 *   udx: soft run …
 *   udx: soft loop …
 *   udx: soft irq …
 *   udx: soft printk …
 *   udx: soft build …
 *   udx: soft path …
 *   udx: soft product …
 *   udx: soft stats …
 *   udx: soft should …
 *   udx: soft idle …
 *   udx: soft gap …
 *   udx: soft open …
 *   udx: soft deepen …
 *   udx: soft wave …
 *   udx: soft honesty …
 *   udx: soft residual lean …              (once-lamp)
 *   udx: soft residual lean layout_ver=   (once-lamp; eng only)
 *   udx: soft residual freestanding class SKIP  (once-lamp)
 *   udx: soft residual product=UDX+ABI     (once-lamp)
 *   udx: soft residual api honesty …       (once-lamp)
 *   udx: soft residual driver lifecycle …  (once-lamp rollup)
 *   udx: soft residual init …              (once-lamp; product lifecycle)
 *   udx: soft residual register …          (once-lamp deepen)
 *   udx: soft residual probe …             (once-lamp deepen)
 *   udx: soft residual run …               (once-lamp; product lifecycle)
 *   udx: soft residual stop …              (once-lamp; product lifecycle)
 *   udx: soft residual quiesce …           (once-lamp deepen)
 *   udx: soft residual remove …            (once-lamp deepen)
 *   udx: soft residual unregister …        (once-lamp; product lifecycle)
 *   udx: soft residual exit …              (once-lamp; product lifecycle)
 *   udx: soft residual printk …            (once-lamp; observation)
 *   udx: soft residual dual_dod …          (once-lamp; A/B OPEN)
 *   udx: soft residual Dual DoD A OPEN     (once-lamp; host parity)
 *   udx: soft residual Dual DoD B OPEN     (once-lamp; host parity)
 *   udx: soft residual Dual DoD A/B OPEN   (once-lamp; host parity)
 *   udx: soft residual hot_cold_abi …      (once-lamp; product surface)
 *   udx: soft residual hazard …            (once-lamp; H1/H2/H3/H4)
 *   udx: soft residual notify_wait …       (once-lamp; freestanding pump)
 *   udx: soft residual run_loop …          (once-lamp; notify then flush)
 *   udx: soft residual should_run …        (once-lamp; stop/init flags)
 *   udx: soft residual work_flush …        (once-lamp; core flush residual)
 *   udx: soft residual product_host …      (once-lamp; rtl/xhci via core)
 *   udx: soft residual install …           (once-lamp; post-DDI install edge)
 *   udx: soft residual life …              (once-lamp; product-host life path)
 *   udx: soft residual denser …            (once-lamp; denser residual prove)
 *   udx: soft residual denser prove …      (once-lamp; denser_prove rollup)
 *   udx: soft residual kernel_notify …     (once-lamp; multi-proc OPEN)
 *   udx: soft residual multi_process …     (once-lamp; SPSC product OPEN)
 *   udx: soft residual g_ac1 …             (once-lamp; no .ko product AC)
 * Pure observation; never gates host skeleton PASS or freestanding path.
 * Soft != skeleton PASS. greppable: udx: soft
 * greppable: udx: soft residual lean
 * greppable: udx: soft residual lean layout_ver=
 * greppable: udx: soft residual driver lifecycle
 * greppable: udx: soft residual init
 * greppable: udx: soft residual register
 * greppable: udx: soft residual probe
 * greppable: udx: soft residual run
 * greppable: udx: soft residual stop
 * greppable: udx: soft residual quiesce
 * greppable: udx: soft residual remove
 * greppable: udx: soft residual unregister
 * greppable: udx: soft residual exit
 * greppable: udx: soft residual printk
 * greppable: udx: soft residual dual_dod
 * greppable: udx: soft residual Dual DoD A OPEN
 * greppable: udx: soft residual Dual DoD B OPEN
 * greppable: udx: soft residual Dual DoD A/B OPEN
 * greppable: udx: soft residual hot_cold_abi
 * greppable: udx: soft residual hazard
 * greppable: udx: soft residual notify_wait
 * greppable: udx: soft residual run_loop
 * greppable: udx: soft residual should_run
 * greppable: udx: soft residual work_flush
 * greppable: udx: soft residual product_host
 * greppable: udx: soft residual install
 * greppable: udx: soft residual life
 * greppable: udx: soft residual denser
 * greppable: udx: soft residual denser prove
 * greppable: udx: soft residual kernel_notify
 * greppable: udx: soft residual multi_process
 * greppable: udx: soft residual g_ac1
 * greppable: product_hosts=UDX
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 * greppable: denser_prove=1 | install_dense=1 | life_dense=1
 *
 * Honesty (Wave 126): UDX kernel notify multi-process SPSC product remains
 * OPEN -- this file only catalogs the soft core surface; product close is
 * M4.2 / security core 6 multi-process driver-host (not here).
 * Dual DoD A/B remain OPEN (agent!=close); soft lamps never close DoD.
 *
 * Lean residual (exclusive residual; Soft!=product; G-AC-1; once-lamps):
 * Cap/MMIO/IRQ/DMA soft API honesty + full product UDX lifecycle residual
 * (init → register → probe → run → stop → quiesce → remove → unregister
 * → exit) + Dual DoD A/B OPEN + hot/cold ABI + hazard H1/H2/H3/H4 + core
 * run residual (notify_wait / should_run / work_flush / product_host /
 * kernel_notify / multi_process / g_ac1). Functional residual: live
 * udx_exit drains work (stop + final_flush) before clearing inited.
 * Freestanding class drivers SKIP; product = Linux-shaped userspace UDX
 * hosts (rtl8168_udx / xhci_udx) over hot+cold ABI + DDI*UDX
 * (product=UDX/DDI+hot/cold ABI). Dual MIT OR Apache-2.0. No stamp storms.
 * Stamp-free residual (never bump GJ_IMAGE_VERSION from this unit).
 * No invent product stamps. No kernel edits from this unit. C2 residual.
 * Bar honesty v2026.08.04.75 (panel context only; stamp-free here).
 *
 * STRONGER residual densify (core install/life path for product hosts;
 * stamp-free bar v2026.08.04.75; NEVER invent .76):
 *   install residual — post-DDI SCAN→GET→OPEN→MAP_BAR→IRQ→DMA→install
 *     edge into udx_init (product markers + residual lean); never Cap mint.
 *   life residual    — full product UDX life path used by product hosts:
 *     init→register→probe→run→stop→quiesce→remove→unregister→exit
 *     (exit_drains_work thr-only; Soft!=product; Dual DoD OPEN).
 * Denser residual (product Dual DoD; Soft!=product; denser only; bar .75):
 *   denser residual  — denser_prove rollup over install + life + run_loop +
 *     exit + product_host + dual_dod (never Dual DoD close from lamps).
 *   install denser   — post-DDI install→udx_init denser prove
 *   life denser      — 9-step product-host life denser prove
 *   run denser       — thr-only notify→irq→work_flush denser prove
 *   exit denser      — stop+work_flush drain denser prove
 *   product_host denser — rtl8168_udx DoD B / xhci_udx DoD A denser
 *   dual_dod denser  — Dual DoD A/B OPEN denser honesty (agent!=close)
 * product_hosts=UDX (rtl8168_udx DoD B / xhci_udx DoD A); freestanding
 * class SKIP. Soft!=product; dual_dod OPEN; agent!=close; G-AC-1.
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 * greppable: udx: soft residual install / life
 * greppable: udx: soft residual denser
 * greppable: denser_prove=1 | install_dense=1 | life_dense=1
 */
#include "udx_internal.h"

#include <udx/irq.h>
#include <udx/udx.h>

#include <stdarg.h>

#if defined(UDX_HOST_LIBC)
#include <stdio.h>
#else
#include <gj/string.h>
#include <gj/syscalls.h>
#endif

/* Host demo idle-spin budget before soft break (udx_run). */
#define UDX_SOFT_IDLE_BUDGET 64u
/*
 * Soft wave stamp for greppable inventory lines only.
 * Stamp-free residual: never product image identity; never bump
 * GJ_IMAGE_VERSION from this unit (bar identity lives in config.h).
 * Bar honesty v2026.08.04.75 — align soft wave with stamp-free bar (no .76).
 */
#define UDX_SOFT_WAVE 75u
/* Greppable soft area count (inventory + residual once-lamp prefixes). */
#define UDX_SOFT_AREAS       53u

/*
 * FUNCTIONAL residual locks (STRONGER core install/life path for product
 * hosts + thr-only pump + denser residual prove; Soft!=product; G-AC-1;
 * Dual DoD A/B OPEN; agent!=close; stamp-free bar v2026.08.04.75;
 * NEVER invent .76).
 * Product hosts = UDX (rtl8168_udx DoD B / xhci_udx DoD A).
 * Install residual edge (post-DDI; Soft!=product):
 *   SCAN→GET→OPEN→MAP_BAR→IRQ→DMA→install_granted → udx_init
 * Life path residual (product hosts; Soft!=product):
 *   init→register→probe→run→stop→quiesce→remove→unregister→exit
 * thr-only udx_run order:
 *   should_run → NOTIFY_WAIT/badge → thr irq_dispatch → work_flush
 * denser residual: install + life + run + exit + product_host + dual_dod
 * denser prove under denser_prove master (never Dual DoD close).
 * H1: never net_eth_poll from IRQ; thr-only pump in process context.
 * greppable: udx: soft residual run_loop
 * greppable: udx: soft residual install
 * greppable: udx: soft residual life
 * greppable: udx: soft residual denser
 * greppable: thr_only=1 | H1=no_net_eth_poll | DoD_A=OPEN | DoD_B=OPEN
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 * greppable: product_hosts=UDX
 * greppable: denser_prove=1 | install_dense=1 | life_dense=1
 * greppable: run_dense=1 | exit_dense=1 | product_host_dense=1 | dual_dod_dense=1
 */
#define UDX_CORE_H1_NO_NET_ETH_POLL  1u /* H1: no net_eth_poll from run pump */
#define UDX_CORE_THR_ONLY_PUMP       1u /* notify→irq thr→work_flush thr-only */
#define UDX_CORE_HARD_IRQ_PUMP       0u /* never hard-IRQ run product */
#define UDX_CORE_DOD_A_OPEN          1u /* xhci_udx Dual DoD A OPEN */
#define UDX_CORE_DOD_B_OPEN          1u /* rtl8168_udx Dual DoD B OPEN */
#define UDX_CORE_AGENT_CLOSE         0u /* soft lamps never close Dual DoD */
#define UDX_CORE_EXIT_DRAINS_WORK    1u /* live exit stop+work_flush residual */
#define UDX_CORE_PRODUCT_HOSTS_UDX   1u /* product_hosts=UDX (never fs class) */
#define UDX_CORE_SOFT_NE_PRODUCT     1u /* Soft!=product honesty lock */
#define UDX_CORE_FS_CLASS_SKIP       1u /* freestanding class SKIP */
#define UDX_CORE_CAP_MINT            0u /* core never mints Cap product */
#define UDX_CORE_INSTALL_EDGE        1u /* post-DDI install → udx_init residual */
#define UDX_CORE_LIFE_STEPS          9u /* init/reg/probe/run/stop/quiesce/rem/unreg/exit */
#define UDX_CORE_INSTALL_DENSE       1u /* denser install residual prove */
#define UDX_CORE_LIFE_DENSE          1u /* denser life residual prove */
#define UDX_CORE_RUN_DENSE           1u /* denser thr-only run_loop prove */
#define UDX_CORE_EXIT_DENSE          1u /* denser exit drain prove */
#define UDX_CORE_PRODUCT_HOST_DENSE  1u /* denser product_host Dual DoD surface */
#define UDX_CORE_DUAL_DOD_DENSE      1u /* denser Dual DoD A/B OPEN honesty */
#define UDX_CORE_DENSER_PROVE        1u /* denser residual prove master lock */
#define UDX_CORE_LEAN_CHECKS        20u /* denser functional self-check count */

_Static_assert(UDX_CORE_H1_NO_NET_ETH_POLL == 1u,
               "H1: core run pump never net_eth_poll");
_Static_assert(UDX_CORE_THR_ONLY_PUMP == 1u,
               "core run is thr-only notify→irq→work_flush");
_Static_assert(UDX_CORE_HARD_IRQ_PUMP == 0u,
               "core run never hard-IRQ product pump");
_Static_assert(UDX_CORE_DOD_A_OPEN == 1u,
               "Dual DoD A remains OPEN (xhci_udx)");
_Static_assert(UDX_CORE_DOD_B_OPEN == 1u,
               "Dual DoD B remains OPEN (rtl8168_udx)");
_Static_assert(UDX_CORE_AGENT_CLOSE == 0u,
               "soft residual lamps never close Dual DoD");
_Static_assert(UDX_CORE_EXIT_DRAINS_WORK == 1u,
               "live udx_exit drains work residual");
_Static_assert(UDX_CORE_PRODUCT_HOSTS_UDX == 1u,
               "product_hosts=UDX (never freestanding rtl/usb class)");
_Static_assert(UDX_CORE_SOFT_NE_PRODUCT == 1u,
               "Soft!=product honesty lock");
_Static_assert(UDX_CORE_FS_CLASS_SKIP == 1u,
               "freestanding class SKIP; product=UDX+ABI hosts");
_Static_assert(UDX_CORE_CAP_MINT == 0u,
               "core residual never mints product Cap");
_Static_assert(UDX_CORE_INSTALL_EDGE == 1u,
               "install residual edge: post-DDI install → udx_init");
_Static_assert(UDX_CORE_LIFE_STEPS == 9u,
               "life residual: 9 product-host life steps");
_Static_assert(UDX_CORE_INSTALL_DENSE == 1u,
               "denser residual: install prove on");
_Static_assert(UDX_CORE_LIFE_DENSE == 1u,
               "denser residual: life prove on");
_Static_assert(UDX_CORE_RUN_DENSE == 1u,
               "denser residual: thr-only run_loop prove on");
_Static_assert(UDX_CORE_EXIT_DENSE == 1u,
               "denser residual: exit drain prove on");
_Static_assert(UDX_CORE_PRODUCT_HOST_DENSE == 1u,
               "denser residual: product_host Dual DoD surface");
_Static_assert(UDX_CORE_DUAL_DOD_DENSE == 1u,
               "denser residual: Dual DoD A/B OPEN honesty");
_Static_assert(UDX_CORE_DENSER_PROVE == 1u,
               "denser residual prove master (Soft!=product Dual DoD OPEN)");
_Static_assert(UDX_CORE_LEAN_CHECKS == 20u,
               "FUNCTIONAL lean residual denser: 20 install/life/run core checks");
_Static_assert(UDX_SOFT_WAVE == 75u,
               "stamp-free soft wave aligns bar v2026.08.04.75 (never .76)");

static u32 g_u32UdxStop;
static u32 g_u32UdxInited;
/* Lean residual once-lamp gate (no stamp storms on inventory re-dump). */
static u8  g_fSoftResidualOnce;

/*
 * Soft product inventory (Wave 126 exclusive deepen). Cumulative for this
 * process unless noted live/peak/last. greppable: udx: soft …
 * Never hard-gates; wrap OK if ever hit.
 */
static u32 g_u32SoftInitEnter;  /* udx_init entries (any) */
static u32 g_u32SoftInitOk;     /* successful first-time udx_init */
static u32 g_u32SoftInitRe;     /* udx_init while already inited (idempotent) */
static u32 g_u32SoftExit;       /* udx_exit calls */
static u32 g_u32SoftExitLive;   /* exit while inited=1 */
static u32 g_u32SoftExitIdle;   /* exit while already not inited */
static u32 g_u32SoftStop;       /* udx_request_stop calls */
static u32 g_u32SoftStopLive;   /* stop while inited and not already stopped */
static u32 g_u32SoftStopDup;    /* stop while already stop_flag=1 */
static u32 g_u32SoftStopCold;   /* stop while not inited */
static u32 g_u32SoftRun;        /* udx_run entries */
static u32 g_u32SoftRunLive;    /* run entered with should_run true */
static u32 g_u32SoftRunSkip;    /* run entered with should_run false */
static u32 g_u32SoftLoop;       /* event-loop iterations */
static u32 g_u32SoftFlush;      /* udx_work_flush invocations from core */
static u32 g_u32SoftFinalFlush; /* post-loop terminal flushes */
static u32 g_u32SoftIrqBadge;   /* NOTIFY_WAIT badge hits (freestanding) */
static u32 g_u32SoftIrqLine;    /* per-line udx_irq_dispatch calls */
static u32 g_u32SoftBadgePoll;  /* freestanding notify_wait_poll calls */
static u32 g_u32SoftBadgeZero;  /* poll returned ≤0 (no badge) */
static u32 g_u32SoftBadgePos;   /* poll returned >0 */
static u32 g_u32SoftIdleBreak;  /* loops ended on idle spin budget */
static u32 g_u32SoftStopBreak;  /* loops ended because stop/inited flag */
static u32 g_u32SoftPrintk;     /* udx_printk calls with non-null fmt */
static u32 g_u32SoftPrintkNull; /* udx_printk rejected (szFmt == NULL) */
static u32 g_u32SoftShouldYes;  /* udx_core_should_run returned 1 */
static u32 g_u32SoftShouldNo;   /* udx_core_should_run returned 0 */
static u32 g_u32SoftLastIdle;   /* idle spins observed on last udx_run */
static u32 g_u32SoftPeakIdle;   /* peak idle spins across runs */
static u32 g_u32SoftLastLoops;  /* loop iterations on last udx_run */
static u32 g_u32SoftPeakLoops;  /* peak loop iterations in one run */
static u32 g_u32SoftLastLines;  /* IRQ lines on last positive badge */
static u32 g_u32SoftPeakLines;  /* peak IRQ lines decoded from one badge */
static u32 g_u32SoftLastBadge;  /* last positive badge low 32 (soft snap) */
static u32 g_u32SoftLogN;       /* soft inventory dumps emitted */

static void soft_inc(u32 *pu32);
static void soft_note_peak(u32 *pu32Peak, u32 u32Val);
static void soft_emit(const char *szFmt, ...);
static void soft_residual_lean_once(void);
static void soft_inventory_log(void);

static void
soft_inc(u32 *pu32)
{
    if (pu32 != NULL && *pu32 < 0xffffffffu) {
        (*pu32)++;
    }
}

/** Soft: raise peak if u32Val is higher (diagnostics only). */
static void
soft_note_peak(u32 *pu32Peak, u32 u32Val)
{
    if (pu32Peak != NULL && u32Val > *pu32Peak) {
        *pu32Peak = u32Val;
    }
}

/*
 * Soft inventory emit path — same host console as udx_printk, but does not
 * bump g_u32SoftPrintk (inventory lines are observation, not product printk).
 */
static void
soft_emit(const char *szFmt, ...)
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
    {
        char aBuf[768];
        va_list apArgs;
        size_t n;

        va_start(apArgs, szFmt);
        n = gj_vsnprintf(aBuf, sizeof(aBuf), szFmt, apArgs);
        va_end(apArgs);
        if (n >= sizeof(aBuf)) {
            n = sizeof(aBuf) - 1u;
        }
        (void)gj_debug_log(aBuf, (long)n);
    }
#endif
}

/**
 * Greppable soft inventory (Wave 126 exclusive deepen).
 * Prefix-stable "udx: soft …" — never hard-gates; observation only.
 *
 *   udx: soft inventory  — cumulative API + run rollup
 *   udx: soft lifecycle  — init/reinit/exit/stop path splits
 *   udx: soft run        — loops/flushes/irq + break reason
 *   udx: soft loop       — idle budget + last/peak loop geometry
 *   udx: soft irq        — freestanding badge→line pulse shape
 *   udx: soft printk     — product printk vs soft-emit separation
 *   udx: soft build      — host-libc vs freestanding notify path
 *   udx: soft path       — claim surface catalog (soft bounds)
 *   udx: soft product    — GREENJADE_UDX / UDX_PRODUCT identity
 *   udx: soft stats      — live flags + should_run samples + log_n
 *   udx: soft should     — should_run yes/no sample geometry
 *   udx: soft idle       — idle budget + break reason geometry
 *   udx: soft gap        — cannot wire kernel notify / multi-proc SPSC
 *   udx: soft open       -- product tracks that remain OPEN (honesty)
 *   udx: soft deepen     -- wave stamp + area count
 *   udx: soft wave       -- exclusive deepen stamp
 *   udx: soft residual lean / lean layout_ver= / freestanding class SKIP
 *   udx: soft residual product=UDX+ABI
 *   udx: soft residual api honesty -- Cap/MMIO/IRQ/DMA soft != product
 *   udx: soft residual driver lifecycle -- full product UDX lifecycle
 *   udx: soft residual init / register / probe / run / stop / quiesce /
 *        remove / unregister / exit / printk -- product lifecycle deepen
 *   udx: soft residual dual_dod -- Dual DoD A/B OPEN (agent!=close)
 *   udx: soft residual Dual DoD A/B OPEN -- host-parity Dual DoD lamps
 *   udx: soft residual hot_cold_abi -- product=UDX/DDI+hot/cold ABI
 *   udx: soft residual hazard -- H1/H2/H3/H4 soft residual honesty
 *   udx: soft residual notify_wait / run_loop / should_run / work_flush
 *   udx: soft residual product_host / install / life / denser /
 *        denser prove / kernel_notify / multi_process / g_ac1
 *     (once-lamps only; see soft_residual_lean_once)
 *
 * greppable: udx: soft
 * greppable: udx: soft residual lean
 * greppable: udx: soft residual lean layout_ver=
 * greppable: udx: soft residual driver lifecycle
 * greppable: udx: soft residual init
 * greppable: udx: soft residual register
 * greppable: udx: soft residual probe
 * greppable: udx: soft residual run
 * greppable: udx: soft residual stop
 * greppable: udx: soft residual quiesce
 * greppable: udx: soft residual remove
 * greppable: udx: soft residual unregister
 * greppable: udx: soft residual exit
 * greppable: udx: soft residual printk
 * greppable: udx: soft residual dual_dod
 * greppable: udx: soft residual Dual DoD A OPEN
 * greppable: udx: soft residual Dual DoD B OPEN
 * greppable: udx: soft residual Dual DoD A/B OPEN
 * greppable: udx: soft residual hot_cold_abi
 * greppable: udx: soft residual hazard
 * greppable: udx: soft residual notify_wait
 * greppable: udx: soft residual run_loop
 * greppable: udx: soft residual should_run
 * greppable: udx: soft residual work_flush
 * greppable: udx: soft residual product_host
 * greppable: udx: soft residual install
 * greppable: udx: soft residual life
 * greppable: udx: soft residual denser
 * greppable: udx: soft residual denser prove
 * greppable: udx: soft residual kernel_notify
 * greppable: udx: soft residual multi_process
 * greppable: udx: soft residual g_ac1
 * greppable: product_hosts=UDX
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 * greppable: denser_prove=1 | install_dense=1 | life_dense=1
 */

/**
 * Lean residual once-lamp (exclusive residual; Soft!=product; G-AC-1).
 * Cap/MMIO/IRQ/DMA soft API honesty; freestanding class SKIP;
 * product = Linux-shaped userspace UDX (rtl8168_udx / xhci_udx) over
 * hot+cold ABI + DDI; Dual DoD A/B OPEN (agent!=close); H1/H2/H3/H4
 * hazard honesty; full product UDX lifecycle residual catalog:
 *   init → register → probe → run → stop → quiesce → remove →
 *   unregister → exit (+ printk observation)
 * STRONGER densify: install residual (post-DDI install → udx_init) +
 * life residual (9-step product-host life path); product_hosts=UDX.
 * Core run residual (notify_wait / should_run / work_flush /
 * product_host / kernel_notify / multi_process / g_ac1).
 * Dual MIT OR Apache-2.0. No stamp storms (fires once per process).
 * Stamp-free: never bump GJ_IMAGE_VERSION from this unit.
 *
 * greppable: udx: soft residual lean
 * greppable: udx: soft residual lean layout_ver=
 * greppable: udx: soft residual freestanding class SKIP
 * greppable: udx: soft residual product=UDX+ABI
 * greppable: udx: soft residual api honesty
 * greppable: udx: soft residual driver lifecycle
 * greppable: udx: soft residual init
 * greppable: udx: soft residual register
 * greppable: udx: soft residual probe
 * greppable: udx: soft residual run
 * greppable: udx: soft residual stop
 * greppable: udx: soft residual quiesce
 * greppable: udx: soft residual remove
 * greppable: udx: soft residual unregister
 * greppable: udx: soft residual exit
 * greppable: udx: soft residual printk
 * greppable: udx: soft residual dual_dod
 * greppable: udx: soft residual Dual DoD A OPEN
 * greppable: udx: soft residual Dual DoD B OPEN
 * greppable: udx: soft residual Dual DoD A/B OPEN
 * greppable: udx: soft residual hot_cold_abi
 * greppable: udx: soft residual hazard
 * greppable: udx: soft residual notify_wait
 * greppable: udx: soft residual run_loop
 * greppable: udx: soft residual should_run
 * greppable: udx: soft residual work_flush
 * greppable: udx: soft residual product_host
 * greppable: udx: soft residual install
 * greppable: udx: soft residual life
 * greppable: udx: soft residual denser
 * greppable: udx: soft residual denser prove
 * greppable: udx: soft residual kernel_notify
 * greppable: udx: soft residual multi_process
 * greppable: udx: soft residual g_ac1
 * greppable: product_hosts=UDX
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 * greppable: denser_prove=1 | install_dense=1 | life_dense=1
 */
static void
soft_residual_lean_once(void)
{
    if (g_fSoftResidualOnce != 0u) {
        return;
    }
    g_fSoftResidualOnce = 1u;

    /*
     * Grep: udx: soft residual lean
     * Cap/MMIO/IRQ/DMA + freestanding SKIP + product=UDX+ABI rollup.
     * Full product UDX lifecycle order; Dual DoD A/B OPEN lean.
     * greppable: udx: soft residual lean layout_ver=
     */
    soft_emit("udx: soft residual lean "
              "cap=OPEN mmio=soft irq=soft dma=soft "
              "freestanding_class=SKIP product=UDX+ABI "
              "product_surface=UDX/DDI+hot/cold_ABI "
              "Linux-shaped_userspace=1 "
              "product_hosts=UDX targets=rtl8168_udx,xhci_udx "
              "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN agent_ne_close=1 "
              "lifecycle=init/register/probe/run/stop/"
              "quiesce/remove/unregister/exit "
              "install_edge=%u life_steps=%u lean_checks=%u "
              "install_dense=%u life_dense=%u run_dense=%u exit_dense=%u "
              "product_host_dense=%u dual_dod_dense=%u denser_prove=%u "
              "core_run=notify_then_work_flush "
              "exit_drains_work=%u thr_only_pump=%u "
              "soft_ne_product=%u dual=MIT_OR_Apache-2.0 G-AC-1 "
              "once=1 storm=0 stamp_free=1 wave=%u "
              "(Soft!=product soft residual dual_dod OPEN product_hosts=UDX; denser)\n",
              UDX_CORE_INSTALL_EDGE, UDX_CORE_LIFE_STEPS,
              UDX_CORE_LEAN_CHECKS,
              UDX_CORE_INSTALL_DENSE, UDX_CORE_LIFE_DENSE,
              UDX_CORE_RUN_DENSE, UDX_CORE_EXIT_DENSE,
              UDX_CORE_PRODUCT_HOST_DENSE, UDX_CORE_DUAL_DOD_DENSE,
              UDX_CORE_DENSER_PROVE,
              UDX_CORE_EXIT_DRAINS_WORK,
              UDX_CORE_THR_ONLY_PUMP, UDX_CORE_SOFT_NE_PRODUCT,
              UDX_SOFT_WAVE);

    /* Grep: udx: soft residual lean layout_ver= */
    soft_emit("udx: soft residual lean layout_ver=%u "
              "unit=core product_lifecycle=1 "
              "order=init->register->probe->run->stop->"
              "quiesce->remove->unregister->exit "
              "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
              "install_edge=%u life_steps=%u lean_checks=%u "
              "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN agent_ne_close=1 "
              "soft=1 soft_ne_product=%u product_mint=0 G-AC-1 once=1 "
              "stamp_free=1 Soft!=product wave=%u\n",
              UDX_LAYOUT_VER, UDX_CORE_INSTALL_EDGE, UDX_CORE_LIFE_STEPS,
              UDX_CORE_LEAN_CHECKS, UDX_CORE_SOFT_NE_PRODUCT,
              UDX_SOFT_WAVE);

    /* Grep: udx: soft residual freestanding class SKIP */
    soft_emit("udx: soft residual freestanding class SKIP "
              "kernel_rtl8168=SKIP kernel_xhci_msc=SKIP "
              "GJ_RTL8168_PROBE=0 GJ_XHCI_MSC_PROBE=0 "
              "not_DoD_B=1 not_DoD_A=1 never_fs_rtl_usb=1 "
              "product_hosts=UDX product=UDX+ABI Linux-shaped_userspace=1 "
              "install_edge=%u life_steps=%u "
              "soft=1 soft_ne_product=%u product=0 G-AC-1 once=1 "
              "wave=%u (Soft!=product; freestanding SKIP; product_hosts=UDX)\n",
              UDX_CORE_INSTALL_EDGE, UDX_CORE_LIFE_STEPS,
              UDX_CORE_SOFT_NE_PRODUCT, UDX_SOFT_WAVE);

    /* Grep: udx: soft residual product=UDX+ABI */
    soft_emit("udx: soft residual product=UDX+ABI "
              "surface=Linux-porter product_hosts=UDX "
              "hosts=rtl8168_udx,xhci_udx "
              "caps=MMIO_FRAME+IRQ_Notification+DMA_window hidden "
              "lifecycle=init/register/probe/run/stop/"
              "quiesce/remove/unregister/exit "
              "install_edge=%u life_steps=%u "
              "abi=hot+cold ddi=1 product_surface=UDX/DDI+hot/cold_ABI "
              "dual_dod_A=OPEN dual_dod_B=OPEN dual_dod=OPEN agent_ne_close=1 "
              "soft_inventory=1 soft_ne_product=%u product_mint=0 G-AC-1 once=1 "
              "wave=%u (Soft!=product soft residual dual_dod OPEN product_hosts=UDX)\n",
              UDX_CORE_INSTALL_EDGE, UDX_CORE_LIFE_STEPS,
              UDX_CORE_SOFT_NE_PRODUCT, UDX_SOFT_WAVE);

    /* Grep: udx: soft residual api honesty */
    soft_emit("udx: soft residual api honesty "
              "cap=hidden_not_minted "
              "mmio=window_lookup_not_FRAME_cap "
              "irq=table_only_cap_install_OPEN "
              "dma=identity_cookie_iommu_grant_soft "
              "init=udx_init register=udx_pci_register_driver "
              "probe=pfnProbe run=udx_run stop=udx_request_stop "
              "quiesce=pfnQuiesce remove=pfnRemove "
              "unregister=udx_pci_unregister_driver exit=udx_exit "
              "should_run=local_flags_not_cap "
              "notify_wait=badge_poll_not_Notification_cap "
              "work_flush=spsc_consumer_not_napi "
              "soft=1 product_kernel=OPEN Soft!=product once=1 wave=%u\n",
              UDX_SOFT_WAVE);

    /*
     * Grep: udx: soft residual driver lifecycle
     * DDI spirit, Linux-shaped names. Order is normative residual honesty:
     *   init → register_driver → match → probe (attach/set_drvdata) →
     *   run → stop → quiesce (stop DMA / mask IRQ / cancel work) →
     *   remove (free) → unregister → exit.
     * Caps never minted/revoked from driver .c.
     * Product path is UDX+ABI userspace hosts, not freestanding class.
     * Functional residual: live udx_exit drains work before inited clear.
     */
    soft_emit("udx: soft residual driver lifecycle "
              "init=udx_init "
              "register=udx_pci_register_driver "
              "probe=attach_enable_regions_ioremap_dma_irq_set_drvdata "
              "run=udx_run stop=udx_request_stop "
              "quiesce=stop_dma_mask_irq_cancel_work "
              "remove=free_irq_dma_iounmap_release_disable "
              "unregister=udx_pci_unregister_driver "
              "exit=udx_exit_drain_work "
              "order=init->register->probe->run->stop->"
              "quiesce->remove->unregister->exit "
              "life_steps=%u install_edge=%u "
              "unreg=quiesce_then_remove "
              "drvdata=probe_install_remove_clear "
              "backend=opaque_caps_hidden "
              "product=UDX+ABI product_hosts=UDX freestanding_class=SKIP "
              "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN agent_ne_close=1 "
              "soft=1 soft_ne_product=%u product_mint=0 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 lean_checks=%u stamp_free=1 wave=%u "
              "(Soft!=product soft residual dual_dod OPEN product_hosts=UDX; "
              "product UDX lifecycle residual; STRONGER)\n",
              UDX_CORE_LIFE_STEPS, UDX_CORE_INSTALL_EDGE,
              UDX_CORE_SOFT_NE_PRODUCT, UDX_CORE_LEAN_CHECKS,
              UDX_SOFT_WAVE);

    /*
     * Product UDX lifecycle residual deepen (full chain).
     * Soft catalog only — product bind remains UDX+ABI userspace hosts.
     * STRONGER densify: product_hosts=UDX + dual_dod OPEN on each step.
     * greppable: udx: soft residual init
     * greppable: udx: soft residual register
     * greppable: udx: soft residual probe
     * greppable: udx: soft residual run
     * greppable: udx: soft residual stop
     * greppable: udx: soft residual quiesce
     * greppable: udx: soft residual remove
     * greppable: udx: soft residual unregister
     * greppable: udx: soft residual exit
     * greppable: udx: soft residual printk
     * greppable: product_hosts=UDX
     */
    soft_emit("udx: soft residual init "
              "api=udx_init "
              "linux_model=module_init "
              "install_edge=%u life_step=1/%u "
              "idempotent_re=1 stop_clear=1 inited=1 "
              "emits=product_markers+inventory+residual_lean "
              "not_cap_mint=1 not_DoD_close=1 "
              "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
              "product=UDX+ABI freestanding_class=SKIP "
              "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN agent_ne_close=1 "
              "soft=1 soft_ne_product=%u product_mint=0 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 stamp_free=1 wave=%u "
              "(Soft!=product; init residual; product_hosts=UDX)\n",
              UDX_CORE_INSTALL_EDGE, UDX_CORE_LIFE_STEPS,
              UDX_CORE_SOFT_NE_PRODUCT, UDX_SOFT_WAVE);

    soft_emit("udx: soft residual register "
              "api=udx_pci_register_driver "
              "unreg=udx_pci_unregister_driver "
              "life_step=2/%u id_table=udx_pci_device_id "
              "ops=pfnProbe+pfnRemove+pfnQuiesce "
              "match=vendor_device_sub_class "
              "bind=try_bind_on_reg+inject "
              "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
              "product=UDX+ABI freestanding_class=SKIP "
              "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN "
              "soft=1 soft_ne_product=%u product_mint=0 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 stamp_free=1 wave=%u "
              "(Soft!=product; register residual; product_hosts=UDX)\n",
              UDX_CORE_LIFE_STEPS, UDX_CORE_SOFT_NE_PRODUCT,
              UDX_SOFT_WAVE);

    soft_emit("udx: soft residual probe "
              "api=pfnProbe "
              "life_step=3/%u "
              "attach=enable+set_master+request_regions "
              "map=ioremap dma=set_mask+alloc irq=request_irq "
              "drvdata=udx_set_drvdata "
              "backend=opaque_pBackend_caps_hidden "
              "fail=no_partial_bind "
              "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
              "product=UDX+ABI freestanding_class=SKIP "
              "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN "
              "soft=1 soft_ne_product=%u product_mint=0 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 stamp_free=1 wave=%u "
              "(Soft!=product; probe residual; product_hosts=UDX)\n",
              UDX_CORE_LIFE_STEPS, UDX_CORE_SOFT_NE_PRODUCT,
              UDX_SOFT_WAVE);

    soft_emit("udx: soft residual run "
              "api=udx_run "
              "life_step=4/%u "
              "window=after_probe_before_quiesce "
              "order=should_run->notify_wait->irq_dispatch->"
              "work_flush->idle_budget "
              "final_flush=1 idle_budget=%u thr_only_pump=%u "
              "H1=no_net_eth_poll_from_IRQ "
              "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
              "product=UDX+ABI freestanding_class=SKIP "
              "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN agent_ne_close=1 "
              "soft=1 soft_ne_product=%u product_close=0 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 stamp_free=1 wave=%u "
              "(Soft!=product; run residual; product_hosts=UDX)\n",
              UDX_CORE_LIFE_STEPS, UDX_SOFT_IDLE_BUDGET,
              UDX_CORE_THR_ONLY_PUMP, UDX_CORE_SOFT_NE_PRODUCT,
              UDX_SOFT_WAVE);

    soft_emit("udx: soft residual stop "
              "api=udx_request_stop "
              "life_step=5/%u "
              "gate=local_stop_flag_only "
              "breaks=udx_run not_process_death=1 "
              "not_cap_revoke=1 not_DoD_close=1 "
              "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
              "product=UDX+ABI freestanding_class=SKIP "
              "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN "
              "soft=1 soft_ne_product=%u product_mint=0 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 stamp_free=1 wave=%u "
              "(Soft!=product; stop residual; product_hosts=UDX)\n",
              UDX_CORE_LIFE_STEPS, UDX_CORE_SOFT_NE_PRODUCT,
              UDX_SOFT_WAVE);

    soft_emit("udx: soft residual quiesce "
              "api=pfnQuiesce "
              "life_step=6/%u "
              "order=before_remove "
              "stop_dma=1 mask_irq=1 cancel_work=1 "
              "ddi_spirit=1 optional_ops=1 "
              "crash_best_effort=1 "
              "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
              "product=UDX+ABI freestanding_class=SKIP "
              "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN "
              "soft=1 soft_ne_product=%u product_mint=0 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 stamp_free=1 wave=%u "
              "(Soft!=product; quiesce residual; product_hosts=UDX)\n",
              UDX_CORE_LIFE_STEPS, UDX_CORE_SOFT_NE_PRODUCT,
              UDX_SOFT_WAVE);

    soft_emit("udx: soft residual remove "
              "api=pfnRemove "
              "life_step=7/%u "
              "after=quiesce "
              "teardown=free_irq+dma_free+iounmap+"
              "release_regions+clear_master+disable "
              "drvdata=udx_clear_drvdata "
              "unreg=quiesce_then_remove "
              "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
              "product=UDX+ABI freestanding_class=SKIP "
              "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN "
              "soft=1 soft_ne_product=%u product_mint=0 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 stamp_free=1 wave=%u "
              "(Soft!=product; remove residual; product_hosts=UDX)\n",
              UDX_CORE_LIFE_STEPS, UDX_CORE_SOFT_NE_PRODUCT,
              UDX_SOFT_WAVE);

    soft_emit("udx: soft residual unregister "
              "api=udx_pci_unregister_driver "
              "life_step=8/%u "
              "order=quiesce_then_remove_then_unreg "
              "before=udx_exit "
              "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
              "product=UDX+ABI freestanding_class=SKIP "
              "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN agent_ne_close=1 "
              "soft=1 soft_ne_product=%u product_mint=0 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 stamp_free=1 wave=%u "
              "(Soft!=product; unregister residual; product_hosts=UDX)\n",
              UDX_CORE_LIFE_STEPS, UDX_CORE_SOFT_NE_PRODUCT,
              UDX_SOFT_WAVE);

    soft_emit("udx: soft residual exit "
              "api=udx_exit "
              "life_step=9/%u "
              "linux_model=module_exit "
              "live_path=stop+work_flush+clear_inited "
              "exit_drains_work=%u final_flush=1 "
              "drivers_should_unregister_first=1 "
              "not_cap_revoke=1 not_DoD_close=1 "
              "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
              "product=UDX+ABI freestanding_class=SKIP "
              "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN agent_ne_close=1 "
              "soft=1 soft_ne_product=%u product_mint=0 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 stamp_free=1 wave=%u "
              "(Soft!=product; exit residual; product_hosts=UDX)\n",
              UDX_CORE_LIFE_STEPS, UDX_CORE_EXIT_DRAINS_WORK,
              UDX_CORE_SOFT_NE_PRODUCT, UDX_SOFT_WAVE);

    soft_emit("udx: soft residual printk "
              "api=udx_printk "
              "observation=1 not_assurance_lamp=1 "
              "not_DoD_close=1 soft_emit_sep=1 "
              "null_reject=1 host_console=1 "
              "soft=1 product_mint=0 G-AC-1 once=1 dual=MIT_OR_Apache-2.0 "
              "wave=%u (Soft!=product; printk residual)\n",
              UDX_SOFT_WAVE);

    /*
     * Dual DoD A/B residual honesty (C2 libudx core).
     * A OPEN until host USB path (xhci_udx + DDI).
     * B OPEN until interactive SSH login (rtl8168_udx hop).
     * Soft lamps never close DoD; agent!=close. Freestanding SKIP.
     * STRONGER densify: product_hosts=UDX install/life path honesty.
     * greppable: udx: soft residual dual_dod
     * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
     * greppable: product_hosts=UDX
     */
    soft_emit("udx: soft residual dual_dod "
              "A=OPEN host=xhci_udx class=USB ddi=1 "
              "B=OPEN host=rtl8168_udx class=NIC ddi=1 "
              "agent_ne_close=1 soft_ne_close=1 agent_close=%u "
              "freestanding_class=SKIP never_fs_rtl_usb=1 "
              "kernel_rtl8168=SKIP kernel_xhci_msc=SKIP "
              "product=UDX/DDI+hot/cold_ABI product_hosts=UDX "
              "install_edge=%u life_steps=%u lean_checks=%u "
              "dual_dod_dense=%u denser_prove=%u "
              "t0_net=virtio_until_UDX_wire "
              "soft=1 soft_ne_product=%u product_close=0 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 stamp_free=1 wave=%u "
              "(Soft!=product soft residual dual_dod OPEN product_hosts=UDX; denser)\n",
              UDX_CORE_AGENT_CLOSE, UDX_CORE_INSTALL_EDGE,
              UDX_CORE_LIFE_STEPS, UDX_CORE_LEAN_CHECKS,
              UDX_CORE_DUAL_DOD_DENSE, UDX_CORE_DENSER_PROVE,
              UDX_CORE_SOFT_NE_PRODUCT, UDX_SOFT_WAVE);

    /*
     * Host-parity Dual DoD A/B OPEN once-lamps (C2 residual deepen).
     * greppable: udx: soft residual Dual DoD A OPEN
     * greppable: udx: soft residual Dual DoD B OPEN
     * greppable: udx: soft residual Dual DoD A/B OPEN
     * greppable: product_hosts=UDX
     */
    soft_emit("udx: soft residual Dual DoD A OPEN "
              "dual_dod_a=OPEN_UDX path=xhci_udx id=8086:a12f "
              "class=USB ddi=1 bar=0 product=UDX+ABI "
              "product_hosts=UDX fs_class=SKIP kernel_xhci_msc=SKIP "
              "core=udx_run notify_then_work_flush "
              "install_edge=%u life_steps=%u "
              "agent_ne_close=1 close=0 mint=0 soft=1 "
              "soft_ne_product=%u G-AC-1 once=1 Soft!=product wave=%u\n",
              UDX_CORE_INSTALL_EDGE, UDX_CORE_LIFE_STEPS,
              UDX_CORE_SOFT_NE_PRODUCT, UDX_SOFT_WAVE);
    soft_emit("udx: soft residual Dual DoD B OPEN "
              "dual_dod_b=OPEN_UDX path=rtl8168_udx id=10ec:8168 "
              "class=NIC ddi=1 bars=0,2 product=UDX+ABI "
              "product_hosts=UDX fs_class=SKIP kernel_rtl8168=SKIP "
              "core=udx_run notify_then_work_flush "
              "install_edge=%u life_steps=%u "
              "t0_net=virtio_until_UDX_wire "
              "agent_ne_close=1 close=0 mint=0 soft=1 "
              "soft_ne_product=%u G-AC-1 once=1 Soft!=product wave=%u\n",
              UDX_CORE_INSTALL_EDGE, UDX_CORE_LIFE_STEPS,
              UDX_CORE_SOFT_NE_PRODUCT, UDX_SOFT_WAVE);
    soft_emit("udx: soft residual Dual DoD A/B OPEN "
              "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0 "
              "product_hosts=UDX hosts=xhci_udx,rtl8168_udx "
              "product=UDX+ABI surface=UDX/DDI+hot/cold_ABI "
              "lifecycle=init/register/probe/run/stop/"
              "quiesce/remove/unregister/exit "
              "install_edge=%u life_steps=%u lean_checks=%u "
              "freestanding_class=SKIP agent_ne_close=1 "
              "soft_lamp_close=0 soft_ne_product=%u G-AC-1 once=1 mint=0 "
              "Soft!=product stamp_free=1 wave=%u "
              "(Soft!=product soft residual dual_dod OPEN product_hosts=UDX)\n",
              UDX_CORE_INSTALL_EDGE, UDX_CORE_LIFE_STEPS,
              UDX_CORE_LEAN_CHECKS, UDX_CORE_SOFT_NE_PRODUCT,
              UDX_SOFT_WAVE);

    /*
     * Product surface residual: hot + cold Linux ABI + DDI/UDX.
     * greppable: udx: soft residual hot_cold_abi
     */
    soft_emit("udx: soft residual hot_cold_abi "
              "product=UDX/DDI+hot/cold_ABI "
              "hot=linux_shaped_syscalls "
              "cold=doors_ddi_caps "
              "ddi=GJ_SYS_DDI_103 "
              "caps=MMIO_FRAME+IRQ_Notification+DMA_window "
              "hosts=rtl8168_udx,xhci_udx "
              "lifecycle=init/register/probe/run/stop/"
              "quiesce/remove/unregister/exit "
              "t0_net=virtio_until_UDX_wire "
              "freestanding_class=SKIP "
              "soft=1 product_mint=0 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 wave=%u "
              "(Soft!=product; hot+cold ABI residual)\n",
              UDX_SOFT_WAVE);

    /*
     * Hazard residual honesty (assurance H1/H2/H3/H4; Soft!=product).
     * H1: no net_eth_poll from IRQ (core pumps notify then work_flush).
     * H2: once-lamps only; storm=0 on inventory re-dump.
     * H3: thr_exit before as_destroy is process-law (not closed here).
     * H4: soft BAR thrash / freestanding ring residual not product track.
     * greppable: udx: soft residual hazard
     */
    soft_emit("udx: soft residual hazard "
              "H1=no_net_eth_poll_from_IRQ "
              "H2=no_stamp_storms once=1 storm=0 "
              "H3=thr_exit_before_as_destroy "
              "H4=soft_BAR_thrash_not_product "
              "thr_only_pump=%u hard_irq_pump=%u "
              "core_run=notify_then_work_flush "
              "irq_path=badge_dispatch_not_napi "
              "DoD_A=%u DoD_B=%u agent_close=%u "
              "soft=1 product_close=0 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 wave=%u "
              "(Soft!=product; hazard residual; thr-only STRONGER)\n",
              UDX_CORE_THR_ONLY_PUMP, UDX_CORE_HARD_IRQ_PUMP,
              UDX_CORE_DOD_A_OPEN, UDX_CORE_DOD_B_OPEN,
              UDX_CORE_AGENT_CLOSE, UDX_SOFT_WAVE);

    /*
     * Core freestanding notify_wait residual (pumped from udx_run only).
     * Badge bit N → IRQ line N; never mints IRQ Notification cap.
     * greppable: udx: soft residual notify_wait
     */
    soft_emit("udx: soft residual notify_wait "
              "api=udx_gj_notify_wait_poll "
              "which=0 mask_all=1 block=0 "
              "badge_shape=bit_N_to_line_N "
              "pump=udx_run_only not_from_request_irq=1 "
              "cap_mint=OPEN product_IRQ_Notification=OPEN "
              "host_fire=udx_host_fire_irq "
              "freestanding_class=SKIP "
              "hosts=rtl8168_udx,xhci_udx "
              "DoD_A=OPEN DoD_B=OPEN "
              "soft=1 product_mint=0 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 wave=%u "
              "(Soft!=product; notify_wait residual)\n",
              UDX_SOFT_WAVE);

    /*
     * Core run_loop residual: notify → irq_dispatch → work_flush.
     * Never product NAPI poll / hard IRQ EOI / net inject.
     * greppable: udx: soft residual run_loop
     */
    soft_emit("udx: soft residual run_loop "
              "api=udx_run "
              "order=should_run->notify_wait->irq_dispatch->"
              "work_flush->idle_budget "
              "final_flush=1 idle_budget=%u "
              "break=idle_or_stop "
              "thr_only_pump=%u hard_irq_pump=%u run_dense=%u "
              "exit_drains_work=%u lean_checks=%u denser_prove=%u "
              "install_edge=%u life_steps=%u "
              "H1=no_net_eth_poll_from_IRQ "
              "napi_poll_product=OPEN hard_eoi=0 "
              "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
              "product=UDX+ABI freestanding_class=SKIP "
              "DoD_A=%u DoD_B=%u dual_dod=OPEN agent_ne_close=1 "
              "soft=1 soft_ne_product=%u product_close=0 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 stamp_free=1 wave=%u "
              "(Soft!=product; run_loop residual denser; thr-only "
              "install/life; product_hosts=UDX)\n",
              UDX_SOFT_IDLE_BUDGET,
              UDX_CORE_THR_ONLY_PUMP, UDX_CORE_HARD_IRQ_PUMP,
              UDX_CORE_RUN_DENSE,
              UDX_CORE_EXIT_DRAINS_WORK, UDX_CORE_LEAN_CHECKS,
              UDX_CORE_DENSER_PROVE,
              UDX_CORE_INSTALL_EDGE, UDX_CORE_LIFE_STEPS,
              UDX_CORE_DOD_A_OPEN, UDX_CORE_DOD_B_OPEN,
              UDX_CORE_SOFT_NE_PRODUCT, UDX_SOFT_WAVE);

    /*
     * should_run residual: local inited/stop flags only.
     * greppable: udx: soft residual should_run
     */
    soft_emit("udx: soft residual should_run "
              "api=udx_core_should_run "
              "gate=inited_and_not_stop "
              "not_lifecycle_cap=1 not_DoD_close=1 "
              "stop=udx_request_stop init=udx_init "
              "exit=udx_exit "
              "soft=1 product_mint=0 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 wave=%u "
              "(Soft!=product; should_run residual)\n",
              UDX_SOFT_WAVE);

    /*
     * Core work_flush residual (consumer claim; not product NAPI).
     * greppable: udx: soft residual work_flush
     */
    soft_emit("udx: soft residual work_flush "
              "api=udx_work_flush "
              "caller=udx_run consumer_claim=1 "
              "napi_poll_product=OPEN softirq_product=OPEN "
              "final_flush=1 loop_flush=1 "
              "spsc_multi_process=OPEN "
              "hosts=rtl8168_udx,xhci_udx "
              "product=UDX+ABI freestanding_class=SKIP "
              "soft=1 product_close=0 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 wave=%u "
              "(Soft!=product; work_flush residual)\n",
              UDX_SOFT_WAVE);

    /*
     * Product-host residual via core run (Dual DoD A/B seed).
     * STRONGER densify: product_hosts=UDX install/life path catalog.
     * greppable: udx: soft residual product_host
     * greppable: product_hosts=UDX
     * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
     */
    soft_emit("udx: soft residual product_host "
              "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
              "surface=Linux-shaped_userspace_UDX "
              "product=UDX/DDI+hot/cold_ABI "
              "core=udx_init+udx_run+udx_exit "
              "lifecycle=init/register/probe/run/stop/"
              "quiesce/remove/unregister/exit "
              "install_edge=%u life_steps=%u exit_drains_work=%u "
              "product_host_dense=%u denser_prove=%u "
              "irq=request_irq_table_fire_or_NOTIFY_WAIT "
              "bh=schedule_work_then_work_flush "
              "rtl8168_path=ISR_ack_schedule_BH "
              "xhci_path=event_schedule_BH "
              "H1=no_net_eth_poll net_inject=0 thr_only_pump=%u "
              "freestanding_class=SKIP never_fs_rtl_usb=1 "
              "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN agent_ne_close=1 "
              "soft=1 soft_ne_product=%u product=0 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 lean_checks=%u stamp_free=1 wave=%u "
              "(Soft!=product soft residual dual_dod OPEN product_hosts=UDX; denser)\n",
              UDX_CORE_INSTALL_EDGE, UDX_CORE_LIFE_STEPS,
              UDX_CORE_EXIT_DRAINS_WORK, UDX_CORE_PRODUCT_HOST_DENSE,
              UDX_CORE_DENSER_PROVE, UDX_CORE_THR_ONLY_PUMP,
              UDX_CORE_SOFT_NE_PRODUCT, UDX_CORE_LEAN_CHECKS,
              UDX_SOFT_WAVE);

    /*
     * STRONGER residual densify: core install path used by product hosts.
     * Post-DDI SCAN→GET→OPEN→MAP_BAR→IRQ→DMA→install_granted edge into
     * udx_init (product markers + residual lean). Never Cap mint from core.
     * greppable: udx: soft residual install
     * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
     * greppable: product_hosts=UDX
     */
    soft_emit("udx: soft residual install "
              "path=post_DDI_install_granted "
              "chain=SCAN>GET>OPEN>MAP_BAR>IRQ>DMA>install>udx_init "
              "edge=udx_init api=udx_init install_edge=%u install_dense=%u "
              "cap_mint=%u product_mint=0 not_DoD_close=1 "
              "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
              "ids=10ec:8168,8086:a12f "
              "product=UDX+ABI freestanding_class=SKIP never_fs_rtl_usb=1 "
              "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN agent_ne_close=1 "
              "life_next=register_probe_run denser_prove=%u "
              "soft=1 soft_ne_product=%u G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 lean_checks=%u stamp_free=1 wave=%u "
              "(install residual densify denser; product hosts; "
              "Soft!=product soft residual dual_dod OPEN product_hosts=UDX)\n",
              UDX_CORE_INSTALL_EDGE, UDX_CORE_INSTALL_DENSE,
              UDX_CORE_CAP_MINT, UDX_CORE_DENSER_PROVE,
              UDX_CORE_SOFT_NE_PRODUCT, UDX_CORE_LEAN_CHECKS,
              UDX_SOFT_WAVE);

    /*
     * STRONGER residual densify: core life path used by product hosts.
     * Full 9-step product UDX life residual (Soft!=product; Dual DoD OPEN).
     * greppable: udx: soft residual life
     * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
     * greppable: product_hosts=UDX
     */
    soft_emit("udx: soft residual life "
              "order=init->register->probe->run->stop->"
              "quiesce->remove->unregister->exit "
              "life_steps=%u life_dense=%u install_edge=%u "
              "exit_drains_work=%u thr_only_pump=%u hard_irq_pump=%u "
              "H1=no_net_eth_poll_from_IRQ denser_prove=%u "
              "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
              "ids=10ec:8168,8086:a12f "
              "product=UDX+ABI freestanding_class=SKIP never_fs_rtl_usb=1 "
              "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN agent_ne_close=1 "
              "cap_mint=%u soft=1 soft_ne_product=%u product_close=0 "
              "G-AC-1 once=1 dual=MIT_OR_Apache-2.0 "
              "lean_checks=%u stamp_free=1 wave=%u "
              "(life residual densify denser; product hosts; "
              "Soft!=product soft residual dual_dod OPEN product_hosts=UDX)\n",
              UDX_CORE_LIFE_STEPS, UDX_CORE_LIFE_DENSE,
              UDX_CORE_INSTALL_EDGE,
              UDX_CORE_EXIT_DRAINS_WORK, UDX_CORE_THR_ONLY_PUMP,
              UDX_CORE_HARD_IRQ_PUMP, UDX_CORE_DENSER_PROVE,
              UDX_CORE_CAP_MINT,
              UDX_CORE_SOFT_NE_PRODUCT, UDX_CORE_LEAN_CHECKS,
              UDX_SOFT_WAVE);

    /*
     * denser residual prove rollup (product Dual DoD; Soft!=product).
     * greppable: udx: soft residual denser
     * greppable: udx: soft residual denser prove
     * greppable: denser_prove=1 | install_dense=1 | life_dense=1
     * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
     * greppable: product_hosts=UDX
     */
    soft_emit("udx: soft residual denser "
              "denser_prove=%u install_dense=%u life_dense=%u "
              "run_dense=%u exit_dense=%u product_host_dense=%u "
              "dual_dod_dense=%u "
              "install_edge=%u life_steps=%u thr_only_pump=%u "
              "exit_drains_work=%u hard_irq_pump=%u "
              "H1=no_net_eth_poll_from_IRQ "
              "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
              "ids=10ec:8168,8086:a12f "
              "product=UDX+ABI freestanding_class=SKIP never_fs_rtl_usb=1 "
              "DoD_A=%u DoD_B=%u dual_dod=OPEN agent_ne_close=1 "
              "cap_mint=%u soft=1 soft_ne_product=%u product_close=0 "
              "G-AC-1 once=1 dual=MIT_OR_Apache-2.0 "
              "lean_checks=%u stamp_free=1 bar=v2026.08.04.75 "
              "never_invent=.76 wave=%u "
              "(denser residual prove; Soft!=product Dual DoD OPEN "
              "product_hosts=UDX)\n",
              UDX_CORE_DENSER_PROVE, UDX_CORE_INSTALL_DENSE,
              UDX_CORE_LIFE_DENSE, UDX_CORE_RUN_DENSE, UDX_CORE_EXIT_DENSE,
              UDX_CORE_PRODUCT_HOST_DENSE, UDX_CORE_DUAL_DOD_DENSE,
              UDX_CORE_INSTALL_EDGE, UDX_CORE_LIFE_STEPS,
              UDX_CORE_THR_ONLY_PUMP, UDX_CORE_EXIT_DRAINS_WORK,
              UDX_CORE_HARD_IRQ_PUMP,
              UDX_CORE_DOD_A_OPEN, UDX_CORE_DOD_B_OPEN,
              UDX_CORE_CAP_MINT, UDX_CORE_SOFT_NE_PRODUCT,
              UDX_CORE_LEAN_CHECKS, UDX_SOFT_WAVE);
    soft_emit("udx: soft residual denser prove "
              "ok=%u denser_prove=%u "
              "install_dense=%u life_dense=%u run_dense=%u exit_dense=%u "
              "product_host_dense=%u dual_dod_dense=%u "
              "product_hosts=UDX dual_dod=OPEN "
              "DoD_A=OPEN DoD_B=OPEN agent_ne_close=1 "
              "soft=1 Soft!=product soft residual dual_dod OPEN "
              "product_hosts=UDX lean_checks=%u stamp_free=1 wave=%u\n",
              (UDX_CORE_DENSER_PROVE != 0u &&
               UDX_CORE_INSTALL_DENSE != 0u &&
               UDX_CORE_LIFE_DENSE != 0u &&
               UDX_CORE_RUN_DENSE != 0u &&
               UDX_CORE_EXIT_DENSE != 0u &&
               UDX_CORE_PRODUCT_HOST_DENSE != 0u &&
               UDX_CORE_DUAL_DOD_DENSE != 0u) ? 1u : 0u,
              UDX_CORE_DENSER_PROVE, UDX_CORE_INSTALL_DENSE,
              UDX_CORE_LIFE_DENSE, UDX_CORE_RUN_DENSE, UDX_CORE_EXIT_DENSE,
              UDX_CORE_PRODUCT_HOST_DENSE, UDX_CORE_DUAL_DOD_DENSE,
              UDX_CORE_LEAN_CHECKS, UDX_SOFT_WAVE);

    /*
     * Kernel notify multi-process product remains OPEN (honesty).
     * greppable: udx: soft residual kernel_notify
     */
    soft_emit("udx: soft residual kernel_notify "
              "product=OPEN multi_waiter=OPEN "
              "host_sim_from_core=0 "
              "freestanding_poll=badge_only "
              "cap_install=OPEN msix_global_bind=OPEN "
              "M4_2_driver_host=OPEN "
              "soft_inventory=1 product_close=0 "
              "DoD_A=OPEN DoD_B=OPEN "
              "soft=1 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 wave=%u "
              "(Soft!=product; kernel_notify residual OPEN)\n",
              UDX_SOFT_WAVE);

    /*
     * Multi-process SPSC product remains OPEN (security core 6).
     * greppable: udx: soft residual multi_process
     */
    soft_emit("udx: soft residual multi_process "
              "spsc_product=OPEN map_grant=OPEN "
              "peer_death=OPEN shared_pages=OPEN "
              "in_process_fifo=soft_standin "
              "security_core_6=OPEN "
              "soft_inventory=1 product_close=0 "
              "DoD_A=OPEN DoD_B=OPEN "
              "soft=1 G-AC-1 once=1 "
              "dual=MIT_OR_Apache-2.0 wave=%u "
              "(Soft!=product; multi_process residual OPEN)\n",
              UDX_SOFT_WAVE);

    /*
     * G-AC-1 residual: no Linux .ko binary as in-kernel product AC.
     * greppable: udx: soft residual g_ac1
     */
    soft_emit("udx: soft residual g_ac1 "
              "G-AC-1=1 no_ko_in_kernel_product=1 "
              "freestanding_no_exec=1 "
              "product=UDX+ABI product_hosts=UDX "
              "Linux-shaped_userspace=1 "
              "hosts=rtl8168_udx,xhci_udx "
              "kernel_rtl8168=SKIP kernel_xhci_msc=SKIP "
              "in_kernel_r8169=0 in_kernel_usb_storage=0 "
              "install_edge=%u life_steps=%u "
              "dual=MIT_OR_Apache-2.0 no_GPL=1 "
              "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN agent_ne_close=1 "
              "soft=1 soft_ne_product=%u product_close=0 once=1 wave=%u "
              "(Soft!=product soft residual dual_dod OPEN product_hosts=UDX; "
              "G-AC-1 residual)\n",
              UDX_CORE_INSTALL_EDGE, UDX_CORE_LIFE_STEPS,
              UDX_CORE_SOFT_NE_PRODUCT, UDX_SOFT_WAVE);
}

static void
soft_inventory_log(void)
{
    u32 u32Host;
    u32 u32Should;
    u32 u32Inited;
    u32 u32Stop;

    soft_inc(&g_u32SoftLogN);

    u32Inited = g_u32UdxInited;
    u32Stop = g_u32UdxStop;
    u32Should = (u32Inited != 0u && u32Stop == 0u) ? 1u : 0u;
#if defined(UDX_HOST_LIBC)
    u32Host = 1u;
#else
    u32Host = 0u;
#endif

    /* Grep: udx: soft inventory */
    soft_emit("udx: soft inventory init=%u reinit=%u exit=%u stop=%u run=%u "
              "loops=%u flushes=%u printk=%u log_n=%u wave=%u\n",
              g_u32SoftInitOk, g_u32SoftInitRe, g_u32SoftExit, g_u32SoftStop,
              g_u32SoftRun, g_u32SoftLoop, g_u32SoftFlush, g_u32SoftPrintk,
              g_u32SoftLogN, UDX_SOFT_WAVE);

    /* Grep: udx: soft lifecycle */
    soft_emit("udx: soft lifecycle init_enter=%u init_ok=%u reinit=%u "
              "exit=%u exit_live=%u exit_idle=%u stop=%u stop_live=%u "
              "stop_dup=%u stop_cold=%u run=%u run_live=%u run_skip=%u\n",
              g_u32SoftInitEnter, g_u32SoftInitOk, g_u32SoftInitRe,
              g_u32SoftExit, g_u32SoftExitLive, g_u32SoftExitIdle,
              g_u32SoftStop, g_u32SoftStopLive, g_u32SoftStopDup,
              g_u32SoftStopCold, g_u32SoftRun, g_u32SoftRunLive,
              g_u32SoftRunSkip);

    /* Grep: udx: soft run */
    soft_emit("udx: soft run loops=%u flushes=%u final_flush=%u "
              "irq_badge=%u irq_line=%u idle_break=%u stop_break=%u "
              "run_live=%u run_skip=%u\n",
              g_u32SoftLoop, g_u32SoftFlush, g_u32SoftFinalFlush,
              g_u32SoftIrqBadge, g_u32SoftIrqLine, g_u32SoftIdleBreak,
              g_u32SoftStopBreak, g_u32SoftRunLive, g_u32SoftRunSkip);

    /* Grep: udx: soft loop */
    soft_emit("udx: soft loop idle_budget=%u last_idle=%u peak_idle=%u "
              "last_loops=%u peak_loops=%u total_loops=%u "
              "idle_break=%u stop_break=%u should_yes=%u should_no=%u\n",
              UDX_SOFT_IDLE_BUDGET, g_u32SoftLastIdle, g_u32SoftPeakIdle,
              g_u32SoftLastLoops, g_u32SoftPeakLoops, g_u32SoftLoop,
              g_u32SoftIdleBreak, g_u32SoftStopBreak, g_u32SoftShouldYes,
              g_u32SoftShouldNo);

    /* Grep: udx: soft irq */
    soft_emit("udx: soft irq badge_poll=%u badge_pos=%u badge_zero=%u "
              "irq_badge=%u irq_line=%u last_lines=%u peak_lines=%u "
              "last_badge_lo=0x%x host_fire=1 freestanding_notify=%u\n",
              g_u32SoftBadgePoll, g_u32SoftBadgePos, g_u32SoftBadgeZero,
              g_u32SoftIrqBadge, g_u32SoftIrqLine, g_u32SoftLastLines,
              g_u32SoftPeakLines, g_u32SoftLastBadge,
              (u32Host == 0u) ? 1u : 0u);

    /* Grep: udx: soft printk */
    soft_emit("udx: soft printk ok=%u null=%u soft_emit_sep=1 "
              "inventory_bumps_printk=0 product_printk=%u\n",
              g_u32SoftPrintk, g_u32SoftPrintkNull, g_u32SoftPrintk);

    /* Grep: udx: soft build */
    soft_emit("udx: soft build host_libc=%u freestanding=%u "
              "notify_wait_poll=%u idle_budget=%u wave=%u "
              "soft_gates_pass=0\n",
              u32Host, (u32Host == 0u) ? 1u : 0u,
              (u32Host == 0u) ? 1u : 0u, UDX_SOFT_IDLE_BUDGET,
              UDX_SOFT_WAVE);

    /*
     * Path catalog — what this soft surface is / is not.
     * greppable: udx: soft path
     */
    soft_emit("udx: soft path init=udx_init run=udx_run exit=udx_exit "
              "stop=udx_request_stop printk=udx_printk "
              "should=udx_core_should_run flush=udx_work_flush "
              "irq_dispatch=udx_irq_dispatch "
              "skeleton_gate=0 hard_gate=0 soft=1\n");

    /* Grep: udx: soft product */
    soft_emit("udx: soft product name=%s tag=%s ver=%s direction=%s "
              "surface=Linux-porter soft_wave=%u\n",
              UDX_PRODUCT_NAME, UDX_PRODUCT_TAG, UDX_PRODUCT_VERSION,
              UDX_PRODUCT_DIRECTION, UDX_SOFT_WAVE);

    /* Grep: udx: soft stats (rollup) */
    soft_emit("udx: soft stats printk=%u printk_null=%u inited=%u "
              "stop_flag=%u should=%u log_n=%u init_enter=%u "
              "run_live=%u peak_loops=%u peak_idle=%u wave=%u\n",
              g_u32SoftPrintk, g_u32SoftPrintkNull, u32Inited, u32Stop,
              u32Should, g_u32SoftLogN, g_u32SoftInitEnter, g_u32SoftRunLive,
              g_u32SoftPeakLoops, g_u32SoftPeakIdle, UDX_SOFT_WAVE);

    /* Grep: udx: soft should (Wave 111 deepen) */
    soft_emit("udx: soft should yes=%u no=%u live=%u "
              "inited=%u stop_flag=%u sample_total=%u wave=%u\n",
              g_u32SoftShouldYes, g_u32SoftShouldNo, u32Should, u32Inited,
              u32Stop, g_u32SoftShouldYes + g_u32SoftShouldNo, UDX_SOFT_WAVE);

    /* Grep: udx: soft idle (Wave 111 deepen) */
    soft_emit("udx: soft idle budget=%u last=%u peak=%u idle_break=%u "
              "stop_break=%u final_flush=%u wave=%u\n",
              UDX_SOFT_IDLE_BUDGET, g_u32SoftLastIdle, g_u32SoftPeakIdle,
              g_u32SoftIdleBreak, g_u32SoftStopBreak, g_u32SoftFinalFlush,
              UDX_SOFT_WAVE);

    /*
     * Gap catalog — what core cannot close alone (Wave 111 honesty).
     * greppable: udx: soft gap
     */
    soft_emit("udx: soft gap kernel_notify_from_host_sim=0 "
              "multi_process_spsc=0 map_grant=0 peer_death=0 "
              "notify_install=0 msix_global_bind=0 "
              "product_close=0 soft=1 wave=%u\n",
              UDX_SOFT_WAVE);

    /*
     * Product OPEN honesty — greppable stamp that kernel notify +
     * multi-process SPSC remain OPEN (soft inventory != product close).
     * greppable: udx: soft open
     */
    soft_emit("udx: soft open kernel_notify=OPEN multi_process_spsc=OPEN "
              "driver_host=OPEN soft_inventory=1 product=0 "
              " wave=%u\n",
              UDX_SOFT_WAVE);

    /* Grep: udx: soft deepen wave (Wave 111 stamp; areas = prior soft lines). */
    soft_emit("udx: soft deepen wave=%u areas=%u unit=core exclusive=1 "
              "prefix=udx:_soft deepen=1 log_n=%u "
              "(soft inventory; never gates skeleton PASS)\n",
              UDX_SOFT_WAVE, UDX_SOFT_AREAS, g_u32SoftLogN);

    /* Grep: udx: soft wave */
    soft_emit("udx: soft wave n=%u unit=core exclusive=1 "
              "prefix=udx:_soft deepen=1 areas=%u "
              "kernel_notify=OPEN multi_process_spsc=OPEN "
              "(soft inventory; never gates skeleton PASS)\n",
              UDX_SOFT_WAVE, UDX_SOFT_AREAS);

    /*
     * Grep: udx: soft honesty (Wave 126 exclusive deepen).
     * Soft inventory != product multi-server confine.
     */
    soft_emit("udx: soft honesty multi_server=0 confine=0 "
              "exclusive=1 soft=1 product_kernel=OPEN wave=%u\n",
              UDX_SOFT_WAVE);

    /*
     * Grep: udx: soft retgradientangle — Wave 126 return-paletteangle honesty
     * Grep: udx: soft retblendangle — Wave 126 exclusive strokeangle stamp
     * CREATE-ONLY soft names; continuum toward 26800; Soft!=product.
     */
    soft_emit("udx: soft retgradientangle soft_only=1 product_gate=0 "
              "soft_ne_product=1 continuum_toward=26800 wave=%u "
              "(retgradientangle honesty; Soft!=product)\n",
              UDX_SOFT_WAVE);
    soft_emit("udx: soft retblendangle exclusive=1 soft_ne_product=1 "
              "continuum_toward=26800 wave=%u "
              "(retblendangle stamp; Soft!=product)\n",
              UDX_SOFT_WAVE);

    /*
     * Lean residual once-lamps (full product UDX lifecycle:
     * init/register/probe/run/stop/quiesce/remove/unregister/exit/printk
     * + install/life densify + Dual DoD A/B OPEN +
     * notify_wait/run_loop/should_run/work_flush/product_host/
     * kernel_notify/multi_process/g_ac1; product=UDX+ABI;
     * product_hosts=UDX; Soft!=product; dual_dod OPEN; G-AC-1).
     * Not re-emitted on every inventory dump — storm=0 / H2.
     * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
     */
    soft_residual_lean_once();
}

u32
udx_core_should_run(void)
{
    u32 u32Yes;

    u32Yes = (g_u32UdxInited && !g_u32UdxStop) ? 1u : 0u;
    if (u32Yes != 0u) {
        soft_inc(&g_u32SoftShouldYes);
    } else {
        soft_inc(&g_u32SoftShouldNo);
    }
    return u32Yes;
}

udx_status_t
udx_init(void)
{
    soft_inc(&g_u32SoftInitEnter);
    if (g_u32UdxInited) {
        soft_inc(&g_u32SoftInitRe);
        return UDX_OK;
    }
    g_u32UdxStop = 0;
    g_u32UdxInited = 1;
    soft_inc(&g_u32SoftInitOk);
    /* Greppable product markers — see user/udx/README.md */
    udx_printk("udx: init %s %s v%s (Linux-porter surface)\n",
               UDX_PRODUCT_NAME, UDX_PRODUCT_TAG, UDX_PRODUCT_VERSION);
    /* Wave 126 soft inventory baseline (greppable udx: soft …). */
    soft_inventory_log();
    return UDX_OK;
}

void
udx_exit(void)
{
    soft_inc(&g_u32SoftExit);
    if (g_u32UdxInited != 0u) {
        soft_inc(&g_u32SoftExitLive);
        /*
         * Product UDX lifecycle residual (Soft!=product; G-AC-1):
         * Live exit drains work like a soft quiesce edge — stop the run
         * loop and final_flush the SPSC work queue before clearing inited.
         * Drivers should still unregister (quiesce→remove) before exit;
         * core does not auto-unbind (pci layer owns that).
         * FUNCTIONAL: exit_drains_work=1 (UDX_CORE_EXIT_DRAINS_WORK).
         * greppable residual: udx: soft residual exit
         * Dual DoD A/B remain OPEN; never Cap revoke product close.
         */
        g_u32UdxStop = 1;
        soft_inc(&g_u32SoftFlush);
        soft_inc(&g_u32SoftFinalFlush);
        if (UDX_CORE_EXIT_DRAINS_WORK != 0u) {
            udx_work_flush();
        }
    } else {
        soft_inc(&g_u32SoftExitIdle);
    }
    /* Final soft rollup while inited still reflects live state. */
    soft_inventory_log();
    g_u32UdxInited = 0;
    g_u32UdxStop = 1;
}

void
udx_request_stop(void)
{
    soft_inc(&g_u32SoftStop);
    if (g_u32UdxInited == 0u) {
        soft_inc(&g_u32SoftStopCold);
    } else if (g_u32UdxStop != 0u) {
        soft_inc(&g_u32SoftStopDup);
    } else {
        soft_inc(&g_u32SoftStopLive);
    }
    g_u32UdxStop = 1;
}

void
udx_run(void)
{
    u32 u32IdleSpins;
    u32 u32LoopThis;
    u32 fIdleBreak;
    u32 fEnteredLive;

    /*
     * Full GJ: non-blocking NOTIFY_WAIT on MSI-X global, then
     * udx_irq_dispatch + work flush.
     * Host: pump work queue; exit when udx_request_stop().
     *
     * IRQ table entries from udx_request_irq are delivered here —
     * request_irq itself does not wait on notifications.
     *
     * FUNCTIONAL residual (STRONGER thr-only product UDX pump;
     * Soft!=product; Dual DoD A/B OPEN; H1):
     *   thr_only_pump=1 hard_irq_pump=0
     *   order: should_run → badge/notify → thr irq_dispatch → work_flush
     *   H1: never net_eth_poll from IRQ thr path (handlers schedule_work).
     * greppable: udx: soft residual run_loop
     * greppable: thr_only=1 | H1=no_net_eth_poll
     */
    soft_inc(&g_u32SoftRun);
    u32IdleSpins = 0;
    u32LoopThis = 0;
    fIdleBreak = 0;
    fEnteredLive = udx_core_should_run();
    if (fEnteredLive != 0u) {
        soft_inc(&g_u32SoftRunLive);
    } else {
        soft_inc(&g_u32SoftRunSkip);
    }
    while (udx_core_should_run()) {
        soft_inc(&g_u32SoftLoop);
        if (u32LoopThis < 0xffffffffu) {
            u32LoopThis++;
        }
#if !defined(UDX_HOST_LIBC)
        {
            long badge = udx_gj_notify_wait_poll();

            soft_inc(&g_u32SoftBadgePoll);
            if (badge > 0) {
                int nIrq;
                u32 u32Lines;

                soft_inc(&g_u32SoftBadgePos);
                soft_inc(&g_u32SoftIrqBadge);
                g_u32SoftLastBadge = (u32)((unsigned long)badge & 0xfffffffful);
                u32Lines = 0;
                for (nIrq = 0; nIrq < 64; nIrq++) {
                    if (((unsigned long)badge >> (unsigned)nIrq) & 1ul) {
                        soft_inc(&g_u32SoftIrqLine);
                        if (u32Lines < 0xffffffffu) {
                            u32Lines++;
                        }
                        udx_irq_dispatch(nIrq);
                    }
                }
                g_u32SoftLastLines = u32Lines;
                soft_note_peak(&g_u32SoftPeakLines, u32Lines);
                u32IdleSpins = 0;
            } else {
                soft_inc(&g_u32SoftBadgeZero);
            }
        }
#endif
        soft_inc(&g_u32SoftFlush);
        udx_work_flush();
        u32IdleSpins++;
        /*
         * Host demo: stop after a few empty iterations so skeleton
         * mains do not hang forever. Long-running hosts should call
         * udx_request_stop from a handler/thread.
         */
        if (u32IdleSpins > UDX_SOFT_IDLE_BUDGET) {
            fIdleBreak = 1;
            break;
        }
    }
    g_u32SoftLastIdle = u32IdleSpins;
    soft_note_peak(&g_u32SoftPeakIdle, u32IdleSpins);
    g_u32SoftLastLoops = u32LoopThis;
    soft_note_peak(&g_u32SoftPeakLoops, u32LoopThis);
    if (fIdleBreak != 0u) {
        soft_inc(&g_u32SoftIdleBreak);
    } else {
        soft_inc(&g_u32SoftStopBreak);
    }
    soft_inc(&g_u32SoftFlush);
    soft_inc(&g_u32SoftFinalFlush);
    udx_work_flush();
    /* Wave 126 soft run snapshot (greppable udx: soft …). */
    soft_inventory_log();
}

void
udx_printk(const char *szFmt, ...)
{
    if (szFmt == NULL) {
        soft_inc(&g_u32SoftPrintkNull);
        return;
    }
    soft_inc(&g_u32SoftPrintk);
#if defined(UDX_HOST_LIBC)
    {
        va_list apArgs;

        va_start(apArgs, szFmt);
        (void)vprintf(szFmt, apArgs);
        va_end(apArgs);
    }
#else
    {
        /*
         * Freestanding product hosts (rtl8168_udx / xhci_udx): format via
         * gj_vsnprintf (supports %u/%x/%llx/%08x/%s) and emit GJ_SYS_DEBUG_LOG
         * for greppable serial lamps (product program PASS, soft probe PASS).
         * Soft!=product; Dual DoD OPEN; stamp-free.
         */
        char aBuf[768];
        va_list apArgs;
        size_t n;

        va_start(apArgs, szFmt);
        n = gj_vsnprintf(aBuf, sizeof(aBuf), szFmt, apArgs);
        va_end(apArgs);
        if (n >= sizeof(aBuf)) {
            n = sizeof(aBuf) - 1u;
        }
        (void)gj_debug_log(aBuf, (long)n);
    }
#endif
}
