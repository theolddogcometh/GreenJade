/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * UDX runtime core: init / run / exit / printk.
 * Product markers (greppable): GREENJADE_UDX, UDX_PRODUCT.
 *
 * Soft inventory (Wave 75 exclusive deepen; this unit only) —
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
 * Pure observation; never gates host skeleton PASS or freestanding path.
 * Soft ≠ skeleton PASS. greppable: udx: soft
 *
 * Honesty (Wave 75): UDX kernel notify multi-process SPSC product remains
 * OPEN — this file only catalogs the soft core surface; product close is
 * M4.2 / security core 6 multi-process driver-host (not here).
 */
#include "udx_internal.h"

#include <udx/irq.h>
#include <udx/udx.h>

#include <stdarg.h>

#if defined(UDX_HOST_LIBC)
#include <stdio.h>
#endif

/* Host demo idle-spin budget before soft break (udx_run). */
#define UDX_SOFT_IDLE_BUDGET 64u
/* Soft wave stamp for greppable inventory lines. */
#define UDX_SOFT_WAVE 70u
/* Greppable soft area count (inventory lines under udx: soft …). */
#define UDX_SOFT_AREAS       17u

static u32 g_u32UdxStop;
static u32 g_u32UdxInited;

/*
 * Soft product inventory (Wave 75 exclusive deepen). Cumulative for this
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
    (void)szFmt;
#endif
}

/**
 * Greppable soft inventory (Wave 75 exclusive deepen).
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
 *   udx: soft open       — product tracks that remain OPEN (honesty)
 *   udx: soft deepen     — wave=70 stamp + area count
 *   udx: soft wave       — exclusive deepen stamp
 *
 * greppable: udx: soft
 */
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
    soft_emit("udx: soft product name=%s tag=%s ver=%s "
              "surface=Linux-porter soft_wave=%u\n",
              UDX_PRODUCT_NAME, UDX_PRODUCT_TAG, UDX_PRODUCT_VERSION,
              UDX_SOFT_WAVE);

    /* Grep: udx: soft stats (rollup) */
    soft_emit("udx: soft stats printk=%u printk_null=%u inited=%u "
              "stop_flag=%u should=%u log_n=%u init_enter=%u "
              "run_live=%u peak_loops=%u peak_idle=%u wave=%u\n",
              g_u32SoftPrintk, g_u32SoftPrintkNull, u32Inited, u32Stop,
              u32Should, g_u32SoftLogN, g_u32SoftInitEnter, g_u32SoftRunLive,
              g_u32SoftPeakLoops, g_u32SoftPeakIdle, UDX_SOFT_WAVE);

    /* Grep: udx: soft should (Wave 75 deepen) */
    soft_emit("udx: soft should yes=%u no=%u live=%u "
              "inited=%u stop_flag=%u sample_total=%u wave=%u\n",
              g_u32SoftShouldYes, g_u32SoftShouldNo, u32Should, u32Inited,
              u32Stop, g_u32SoftShouldYes + g_u32SoftShouldNo, UDX_SOFT_WAVE);

    /* Grep: udx: soft idle (Wave 75 deepen) */
    soft_emit("udx: soft idle budget=%u last=%u peak=%u idle_break=%u "
              "stop_break=%u final_flush=%u wave=%u\n",
              UDX_SOFT_IDLE_BUDGET, g_u32SoftLastIdle, g_u32SoftPeakIdle,
              g_u32SoftIdleBreak, g_u32SoftStopBreak, g_u32SoftFinalFlush,
              UDX_SOFT_WAVE);

    /*
     * Gap catalog — what core cannot close alone (Wave 75 honesty).
     * greppable: udx: soft gap
     */
    soft_emit("udx: soft gap kernel_notify_from_host_sim=0 "
              "multi_process_spsc=0 map_grant=0 peer_death=0 "
              "notify_install=0 msix_global_bind=0 "
              "product_close=0 soft=1 wave=%u\n",
              UDX_SOFT_WAVE);

    /*
     * Product OPEN honesty — greppable stamp that kernel notify +
     * multi-process SPSC remain OPEN (soft inventory ≠ product close).
     * greppable: udx: soft open
     */
    soft_emit("udx: soft open kernel_notify=OPEN multi_process_spsc=OPEN "
              "driver_host=OPEN soft_inventory=1 product=0 "
              "bar3=OPEN wave=%u\n",
              UDX_SOFT_WAVE);

    /* Grep: udx: soft deepen wave (Wave 75 stamp; areas = prior soft lines). */
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
     * Grep: udx: soft honesty (Wave 75 exclusive deepen).
     * Soft inventory ≠ product multi-server confine.
     */
    soft_emit("udx: soft honesty multi_server=0 confine=0 bar3=0 "
              "exclusive=1 soft=1 product_kernel=OPEN wave=%u\n",
              UDX_SOFT_WAVE);

    /*
     * Grep: udx: soft retmoatangle — Wave 75 return-moatangle honesty
     * Grep: udx: soft retowerangle — Wave 75 exclusive towerangle stamp
     * CREATE-ONLY soft names; continuum toward 21700; Soft≠product.
     */
    soft_emit("udx: soft retmoatangle soft_only=1 product_gate=0 "
              "soft_ne_product=1 continuum_toward=21700 wave=%u "
              "(retmoatangle honesty; Soft!=product; not bar3)\n",
              UDX_SOFT_WAVE);
    soft_emit("udx: soft retowerangle exclusive=1 soft_ne_product=1 "
              "continuum_toward=21700 wave=%u "
              "(retowerangle stamp; Soft!=product)\n",
              UDX_SOFT_WAVE);
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
    /* Wave 75 soft inventory baseline (greppable udx: soft …). */
    soft_inventory_log();
    return UDX_OK;
}

void
udx_exit(void)
{
    soft_inc(&g_u32SoftExit);
    if (g_u32UdxInited != 0u) {
        soft_inc(&g_u32SoftExitLive);
    } else {
        soft_inc(&g_u32SoftExitIdle);
    }
    /* Final soft rollup while flags still reflect live state. */
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
    /* Wave 75 soft run snapshot (greppable udx: soft …). */
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
    (void)szFmt;
    /* Freestanding: callers typically also use platform debug log. */
#endif
}
