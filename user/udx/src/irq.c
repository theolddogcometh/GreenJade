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
 * Soft mask: nested disable depth. Fire while masked latches pending;
 * final enable delivers once. Use disable in quiesce before free_irq.
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
static u16 g_aIrqDisableDepth[UDX_IRQ_MAX];
static u8  g_aIrqPending[UDX_IRQ_MAX];

static int
irq_line_has_actions(int nIrq)
{
    int iSlot;

    for (iSlot = 0; iSlot < UDX_IRQ_SHARE_MAX; iSlot++) {
        if (g_aIrq[nIrq][iSlot].u8Used) {
            return 1;
        }
    }
    return 0;
}

static void
irq_dispatch_unlocked(int nIrq)
{
    int iSlot;

    if (g_aIrqDisableDepth[nIrq] != 0) {
        g_aIrqPending[nIrq] = 1;
        return;
    }
    g_aIrqPending[nIrq] = 0;
    for (iSlot = 0; iSlot < UDX_IRQ_SHARE_MAX; iSlot++) {
        if (g_aIrq[nIrq][iSlot].u8Used && g_aIrq[nIrq][iSlot].pfnHandler) {
            (void)g_aIrq[nIrq][iSlot].pfnHandler(
                nIrq, g_aIrq[nIrq][iSlot].pDevId);
        }
    }
}

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

    /* Reject duplicate pDevId on the same line (Linux free_irq key). */
    for (iSlot = 0; iSlot < UDX_IRQ_SHARE_MAX; iSlot++) {
        if (g_aIrq[nIrq][iSlot].u8Used &&
            g_aIrq[nIrq][iSlot].pDevId == pDevId) {
            return UDX_ERR_BUSY;
        }
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
            /*
             * Table-only bind. NOTIFY_WAIT / host fire is pumped from
             * udx_run (core.c) or udx_host_fire_irq — not here.
             * Leave disable depth as-is so a quiesced line stays masked.
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
            g_aIrq[nIrq][iSlot].u32Flags = 0;
            if (!irq_line_has_actions(nIrq)) {
                g_aIrqPending[nIrq] = 0;
                g_aIrqDisableDepth[nIrq] = 0;
            }
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
    if (g_aIrqDisableDepth[nIrq] < 0xffffu) {
        g_aIrqDisableDepth[nIrq]++;
    }
}

void
udx_enable_irq(int nIrq)
{
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return;
    }
    if (g_aIrqDisableDepth[nIrq] > 0) {
        g_aIrqDisableDepth[nIrq]--;
    }
    if (g_aIrqDisableDepth[nIrq] == 0 && g_aIrqPending[nIrq]) {
        /* Deliver latched soft fire after final unmask. */
        irq_dispatch_unlocked(nIrq);
    }
}

int
udx_irq_is_disabled(int nIrq)
{
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return 1;
    }
    return g_aIrqDisableDepth[nIrq] != 0;
}

int
udx_irq_disable_depth(int nIrq)
{
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return 0;
    }
    return (int)g_aIrqDisableDepth[nIrq];
}

int
udx_irq_is_pending(int nIrq)
{
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return 0;
    }
    return g_aIrqPending[nIrq] != 0;
}

void
udx_synchronize_irq(int nIrq)
{
    /*
     * Host / freestanding soft path: handlers run synchronously in
     * dispatch (no separate hard-IRQ thread). Nothing to wait on.
     */
    (void)nIrq;
}

const char *
udx_irq_name(int nIrq)
{
    int iSlot;

    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return NULL;
    }
    for (iSlot = 0; iSlot < UDX_IRQ_SHARE_MAX; iSlot++) {
        if (g_aIrq[nIrq][iSlot].u8Used) {
            return g_aIrq[nIrq][iSlot].szName;
        }
    }
    return NULL;
}

int
udx_irq_action_count(int nIrq)
{
    int iSlot;
    int cActions;

    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return 0;
    }
    cActions = 0;
    for (iSlot = 0; iSlot < UDX_IRQ_SHARE_MAX; iSlot++) {
        if (g_aIrq[nIrq][iSlot].u8Used) {
            cActions++;
        }
    }
    return cActions;
}

void
udx_irq_dispatch(int nIrq)
{
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return;
    }
    irq_dispatch_unlocked(nIrq);
}
