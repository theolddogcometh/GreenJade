/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * UDX runtime core: init / run / exit / printk.
 * Product markers (greppable): GREENJADE_UDX, UDX_PRODUCT.
 *
 * Soft inventory (Wave 10 exclusive deepen) — greppable "udx: soft …":
 *   udx: soft inventory …
 *   udx: soft run …
 *   udx: soft stats …
 * Pure observation; never gates host skeleton PASS or freestanding path.
 */
#include "udx_internal.h"

#include <udx/irq.h>
#include <udx/udx.h>

#include <stdarg.h>

#if defined(UDX_HOST_LIBC)
#include <stdio.h>
#endif

static u32 g_u32UdxStop;
static u32 g_u32UdxInited;

/*
 * Soft product inventory (Wave 10 exclusive). Cumulative for this process.
 * greppable: udx: soft …
 */
static u32 g_u32SoftInitOk;     /* successful first-time udx_init */
static u32 g_u32SoftInitRe;     /* udx_init while already inited (idempotent) */
static u32 g_u32SoftExit;       /* udx_exit calls */
static u32 g_u32SoftStop;       /* udx_request_stop calls */
static u32 g_u32SoftRun;        /* udx_run entries */
static u32 g_u32SoftLoop;       /* event-loop iterations */
static u32 g_u32SoftFlush;      /* udx_work_flush invocations from core */
static u32 g_u32SoftIrqBadge;   /* NOTIFY_WAIT badge hits (freestanding) */
static u32 g_u32SoftIrqLine;    /* per-line udx_irq_dispatch calls */
static u32 g_u32SoftIdleBreak;  /* loops ended on idle spin budget */
static u32 g_u32SoftStopBreak;  /* loops ended because stop/inited flag */
static u32 g_u32SoftPrintk;     /* udx_printk calls (excl. soft inventory) */
static u32 g_u32SoftLogN;       /* soft inventory dumps emitted */

static void soft_inc(u32 *pu32);
static void soft_emit(const char *szFmt, ...);
static void soft_inventory_log(void);

static void
soft_inc(u32 *pu32)
{
    if (pu32 != NULL && *pu32 < 0xffffffffu) {
        (*pu32)++;
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

/*
 * Greppable soft inventory (Wave 10). Prefix "udx: soft …".
 * Pure observation — always soft; does not gate skeleton PASS.
 *
 *   udx: soft inventory init=… reinit=… exit=… stop=… run=…
 *   udx: soft run loops=… flushes=… irq_badge=… irq_line=…
 *                idle_break=… stop_break=…
 *   udx: soft stats printk=… inited=… stop_flag=… log_n=…
 */
static void
soft_inventory_log(void)
{
    soft_inc(&g_u32SoftLogN);

    /* Grep: udx: soft inventory */
    soft_emit("udx: soft inventory init=%u reinit=%u exit=%u stop=%u run=%u\n",
              g_u32SoftInitOk, g_u32SoftInitRe, g_u32SoftExit, g_u32SoftStop,
              g_u32SoftRun);

    /* Grep: udx: soft run */
    soft_emit("udx: soft run loops=%u flushes=%u irq_badge=%u irq_line=%u "
              "idle_break=%u stop_break=%u\n",
              g_u32SoftLoop, g_u32SoftFlush, g_u32SoftIrqBadge,
              g_u32SoftIrqLine, g_u32SoftIdleBreak, g_u32SoftStopBreak);

    /* Grep: udx: soft stats (rollup) */
    soft_emit("udx: soft stats printk=%u inited=%u stop_flag=%u log_n=%u\n",
              g_u32SoftPrintk, g_u32UdxInited, g_u32UdxStop, g_u32SoftLogN);
}

u32
udx_core_should_run(void)
{
    return g_u32UdxInited && !g_u32UdxStop;
}

udx_status_t
udx_init(void)
{
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
    /* Wave 10 soft inventory baseline (greppable udx: soft …). */
    soft_inventory_log();
    return UDX_OK;
}

void
udx_exit(void)
{
    soft_inc(&g_u32SoftExit);
    /* Final soft rollup while flags still reflect live state. */
    soft_inventory_log();
    g_u32UdxInited = 0;
    g_u32UdxStop = 1;
}

void
udx_request_stop(void)
{
    soft_inc(&g_u32SoftStop);
    g_u32UdxStop = 1;
}

void
udx_run(void)
{
    u32 u32IdleSpins;
    u32 fIdleBreak;

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
    fIdleBreak = 0;
    while (udx_core_should_run()) {
        soft_inc(&g_u32SoftLoop);
#if !defined(UDX_HOST_LIBC)
        {
            long badge = udx_gj_notify_wait_poll();

            if (badge > 0) {
                int nIrq;

                soft_inc(&g_u32SoftIrqBadge);
                for (nIrq = 0; nIrq < 64; nIrq++) {
                    if (((unsigned long)badge >> (unsigned)nIrq) & 1ul) {
                        soft_inc(&g_u32SoftIrqLine);
                        udx_irq_dispatch(nIrq);
                    }
                }
                u32IdleSpins = 0;
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
        if (u32IdleSpins > 64u) {
            fIdleBreak = 1;
            break;
        }
    }
    if (fIdleBreak != 0u) {
        soft_inc(&g_u32SoftIdleBreak);
    } else {
        soft_inc(&g_u32SoftStopBreak);
    }
    soft_inc(&g_u32SoftFlush);
    udx_work_flush();
    /* Wave 10 soft run snapshot (greppable udx: soft …). */
    soft_inventory_log();
}

void
udx_printk(const char *szFmt, ...)
{
    if (szFmt == NULL) {
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
