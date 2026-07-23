/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Threaded IRQ table. Kernel Notification → udx_irq_dispatch.
 *
 * Registration (udx_request_irq) only fills the host table. Delivery is
 * pumped from udx_run via non-blocking GJ_SYS_NOTIFY_WAIT (freestanding)
 * or udx_host_fire_irq (UDX_HOST_LIBC). Handlers run in host context.
 *
 * Soft mask (disable_irq) skips dispatch — use in quiesce before free_irq.
 */
#include <udx/irq.h>
#include <udx/udx.h>

#define UDX_IRQ_MAX 256
#define UDX_IRQ_SHARE_MAX 4

struct udx_irq_action {
    udx_irq_handler_t pfnHandler;
    void             *pDevId;
    const char       *szName;
    u32               u32Flags;
    u8                u8Used;
};

static struct udx_irq_action g_aIrq[UDX_IRQ_MAX][UDX_IRQ_SHARE_MAX];
static u8 g_aIrqMasked[UDX_IRQ_MAX];

udx_status_t
udx_request_irq(int nIrq, udx_irq_handler_t pfnHandler, u32 u32Flags,
                const char *szName, void *pDevId)
{
    int iSlot;
    int fAny;
    int fLineShared;

    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX || pfnHandler == NULL) {
        return UDX_ERR_INVAL;
    }

    fAny = 0;
    fLineShared = 0;
    for (iSlot = 0; iSlot < UDX_IRQ_SHARE_MAX; iSlot++) {
        if (g_aIrq[nIrq][iSlot].u8Used) {
            fAny = 1;
            if ((g_aIrq[nIrq][iSlot].u32Flags & UDX_IRQF_SHARED) != 0) {
                fLineShared = 1;
            }
            break;
        }
    }

    /*
     * Shared line only when every registrant sets UDX_IRQF_SHARED.
     * First occupant sets the line policy (slot 0 is the anchor).
     */
    if (fAny) {
        if ((u32Flags & UDX_IRQF_SHARED) == 0) {
            return UDX_ERR_BUSY;
        }
        if (!fLineShared) {
            return UDX_ERR_BUSY;
        }
    }

    for (iSlot = 0; iSlot < UDX_IRQ_SHARE_MAX; iSlot++) {
        if (!g_aIrq[nIrq][iSlot].u8Used) {
            g_aIrq[nIrq][iSlot].pfnHandler = pfnHandler;
            g_aIrq[nIrq][iSlot].pDevId = pDevId;
            g_aIrq[nIrq][iSlot].szName = szName;
            g_aIrq[nIrq][iSlot].u32Flags = u32Flags;
            g_aIrq[nIrq][iSlot].u8Used = 1;
            g_aIrqMasked[nIrq] = 0; /* enabled on bind */
            /*
             * Table-only bind. NOTIFY_WAIT / host fire is pumped from
             * udx_run (core.c) or udx_host_fire_irq — not here.
             */
            return UDX_OK;
        }
    }
    return UDX_ERR_BUSY;
}

void
udx_free_irq(int nIrq, void *pDevId)
{
    int iSlot;

    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return;
    }
    for (iSlot = 0; iSlot < UDX_IRQ_SHARE_MAX; iSlot++) {
        if (g_aIrq[nIrq][iSlot].u8Used &&
            g_aIrq[nIrq][iSlot].pDevId == pDevId) {
            g_aIrq[nIrq][iSlot].u8Used = 0;
            g_aIrq[nIrq][iSlot].pfnHandler = NULL;
            g_aIrq[nIrq][iSlot].pDevId = NULL;
            g_aIrq[nIrq][iSlot].szName = NULL;
            return;
        }
    }
}

void
udx_disable_irq(int nIrq)
{
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return;
    }
    g_aIrqMasked[nIrq] = 1;
}

void
udx_enable_irq(int nIrq)
{
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return;
    }
    g_aIrqMasked[nIrq] = 0;
}

int
udx_irq_is_disabled(int nIrq)
{
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return 1;
    }
    return g_aIrqMasked[nIrq] != 0;
}

void
udx_irq_dispatch(int nIrq)
{
    int iSlot;

    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return;
    }
    if (g_aIrqMasked[nIrq]) {
        return;
    }
    for (iSlot = 0; iSlot < UDX_IRQ_SHARE_MAX; iSlot++) {
        if (g_aIrq[nIrq][iSlot].u8Used && g_aIrq[nIrq][iSlot].pfnHandler) {
            (void)g_aIrq[nIrq][iSlot].pfnHandler(
                nIrq, g_aIrq[nIrq][iSlot].pDevId);
        }
    }
}
