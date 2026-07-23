/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * UDX runtime core: init / run / exit / printk.
 * Product markers (greppable): GREENJADE_UDX, UDX_PRODUCT.
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

u32
udx_core_should_run(void)
{
    return g_u32UdxInited && !g_u32UdxStop;
}

udx_status_t
udx_init(void)
{
    if (g_u32UdxInited) {
        return UDX_OK;
    }
    g_u32UdxStop = 0;
    g_u32UdxInited = 1;
    /* Greppable product markers — see user/udx/README.md */
    udx_printk("udx: init %s %s v%s (Linux-porter surface)\n",
               UDX_PRODUCT_NAME, UDX_PRODUCT_TAG, UDX_PRODUCT_VERSION);
    return UDX_OK;
}

void
udx_exit(void)
{
    g_u32UdxInited = 0;
    g_u32UdxStop = 1;
}

void
udx_request_stop(void)
{
    g_u32UdxStop = 1;
}

void
udx_run(void)
{
    u32 u32IdleSpins;

    /*
     * Full GJ: non-blocking NOTIFY_WAIT on MSI-X global, then
     * udx_irq_dispatch + work flush.
     * Host: pump work queue; exit when udx_request_stop().
     *
     * IRQ table entries from udx_request_irq are delivered here —
     * request_irq itself does not wait on notifications.
     */
    u32IdleSpins = 0;
    while (udx_core_should_run()) {
#if !defined(UDX_HOST_LIBC)
        {
            long badge = udx_gj_notify_wait_poll();

            if (badge > 0) {
                int nIrq;

                for (nIrq = 0; nIrq < 64; nIrq++) {
                    if (((unsigned long)badge >> (unsigned)nIrq) & 1ul) {
                        udx_irq_dispatch(nIrq);
                    }
                }
                u32IdleSpins = 0;
            }
        }
#endif
        udx_work_flush();
        u32IdleSpins++;
        /*
         * Host demo: stop after a few empty iterations so skeleton
         * mains do not hang forever. Long-running hosts should call
         * udx_request_stop from a handler/thread.
         */
        if (u32IdleSpins > 64u) {
            break;
        }
    }
    udx_work_flush();
}

void
udx_printk(const char *szFmt, ...)
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
    /* Freestanding: callers typically also use platform debug log. */
#endif
}
