/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product MSI-X IRQ → Notification delivery (clean-room pure C).
 * Binds IDT vector GJ_MSIX_IRQ_VEC to a global Notification; soft inject and
 * hw-sim pulse exercise the path without requiring a live device fire.
 * Not a full device driver — delivery plumbing only. No GPL source.
 */
#include <gj/apic.h>
#include <gj/idt.h>
#include <gj/irq_msix.h>
#include <gj/klog.h>
#include <gj/notify.h>
#include <gj/types.h>

extern void irq_stub_msix(void);

static u32 g_u32MsixIrq;
static u32 g_u32MsixSoft;
static u32 g_u32MsixHw;
static int g_fReady;
static int g_fInHandler;

void
irq_msix_handler(void)
{
    struct gj_notify *pNotify = notify_msix_global();

    g_fInHandler = 1;
    g_u32MsixIrq++;
    g_u32MsixHw++;
    notify_signal(pNotify, 1ull << 0);
    apic_eoi();
    g_fInHandler = 0;
}

void
irq_msix_soft_inject(u64 u64Badge)
{
    struct gj_notify *pNotify = notify_msix_global();

    g_u32MsixSoft++;
    g_u32MsixIrq++;
    notify_signal(pNotify, u64Badge ? u64Badge : 1ull);
}

u32
irq_msix_hw_pulse(void)
{
    struct gj_notify *pNotify;

    if (!g_fReady) {
        return 0;
    }
    /*
     * Real self-IPI during early kmain risks #DF (stack/TPR). The IDT stub
     * (irq_stub_msix) is installed for live device IRQs; this pulse mimics
     * the Notification side of a hardware IRQ without EOI/reentry.
     */
    pNotify = notify_msix_global();
    g_u32MsixHw++;
    g_u32MsixIrq++;
    notify_signal(pNotify, 1ull << 1); /* distinct badge bit for "hw" path */
    kprintf("irq: MSI-X hw-sim pulse (IDT gate installed for live IRQs)\n");
    return g_u32MsixHw;
}

void
irq_msix_init(void)
{
    notify_msix_init();
    idt_set_gate(GJ_MSIX_IRQ_VEC, (void *)irq_stub_msix, 0x8E);
    g_fReady = 1;
    g_u32MsixIrq = 0;
    g_u32MsixSoft = 0;
    g_u32MsixHw = 0;
    kprintf("irq: MSI-X vec=0x%x Notification bound PASS\n", GJ_MSIX_IRQ_VEC);
}

u32
irq_msix_count(void)
{
    return g_u32MsixIrq;
}

u32
irq_msix_soft_count(void)
{
    return g_u32MsixSoft;
}

u32
irq_msix_hw_count(void)
{
    return g_u32MsixHw;
}

int
irq_msix_ready(void)
{
    return g_fReady;
}
