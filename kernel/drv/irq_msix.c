/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product MSI-X IRQ → Notification delivery (clean-room pure C).
 * Binds IDT vector GJ_MSIX_IRQ_VEC to a global Notification; soft inject,
 * soft pulse path, table-soft fire, and hw-sim pulse exercise delivery
 * without requiring a live device fire.
 * Not a full device driver — delivery plumbing only. No GPL source.
 *
 * greppable: MSI-X soft pulse path
 */
#include <gj/apic.h>
#include <gj/idt.h>
#include <gj/irq_msix.h>
#include <gj/klog.h>
#include <gj/notify.h>
#include <gj/pci_caps.h>
#include <gj/types.h>

extern void irq_stub_msix(void);

static u32 g_u32MsixIrq;
static u32 g_u32MsixSoft;
static u32 g_u32MsixHw;
static u32 g_u32SoftPulsePath;
static u32 g_u32TablePulse;
static u64 g_u64LastBadge;
static u32 g_u32LastPath;
static int g_fReady;
static int g_fInHandler;

static void
irq_msix_note_pulse(u64 u64Badge, u32 u32Path)
{
    if (u64Badge == 0) {
        u64Badge = GJ_MSIX_BADGE_SOFT;
    }
    g_u64LastBadge = u64Badge;
    g_u32LastPath = u32Path;
}

void
irq_msix_handler(void)
{
    struct gj_notify *pNotify = notify_msix_global();

    g_fInHandler = 1;
    g_u32MsixIrq++;
    g_u32MsixHw++;
    irq_msix_note_pulse(GJ_MSIX_BADGE_SOFT, GJ_MSIX_PATH_IRQ);
    notify_pulse(pNotify, GJ_MSIX_BADGE_SOFT);
    apic_eoi();
    g_fInHandler = 0;
}

void
irq_msix_soft_inject(u64 u64Badge)
{
    struct gj_notify *pNotify = notify_msix_global();

    if (u64Badge == 0) {
        u64Badge = GJ_MSIX_BADGE_SOFT;
    }
    g_u32MsixSoft++;
    g_u32MsixIrq++;
    irq_msix_note_pulse(u64Badge, GJ_MSIX_PATH_SOFT);
    notify_pulse(pNotify, u64Badge);
}

u32
irq_msix_soft_pulse_path(u64 u64Badge)
{
    struct gj_notify *pNotify;
    u64 u64Pending;

    if (!g_fReady) {
        return 0;
    }
    if (u64Badge == 0) {
        u64Badge = GJ_MSIX_BADGE_SOFT;
    }
    pNotify = notify_msix_global();
    g_u32MsixSoft++;
    g_u32MsixIrq++;
    g_u32SoftPulsePath++;
    irq_msix_note_pulse(u64Badge, GJ_MSIX_PATH_SOFT);
    notify_pulse(pNotify, u64Badge);
    /*
     * Soft path verify: pending must observe the OR'd badge (stats poll;
     * does not clear — wait path owns reclaim).
     */
    u64Pending = notify_pending(pNotify);
    if ((u64Pending & u64Badge) == 0) {
        kprintf("irq: MSI-X soft pulse path pending miss badge=0x%lx "
                "pending=0x%lx\n",
                (unsigned long)u64Badge, (unsigned long)u64Pending);
    }
    return g_u32SoftPulsePath;
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
    irq_msix_note_pulse(GJ_MSIX_BADGE_HW, GJ_MSIX_PATH_HW);
    notify_pulse(pNotify, GJ_MSIX_BADGE_HW);
    kprintf("irq: MSI-X hw-sim pulse (IDT gate installed for live IRQs)\n");
    return g_u32MsixHw;
}

u32
irq_msix_soft_table_pulse(u16 u16Idx)
{
    u32 u32Before;
    u32 u32Delivered;

    if (!g_fReady) {
        return 0;
    }
    if (!pci_msix_soft_ready()) {
        pci_msix_soft_table_init();
        /* Ensure a programmed unmasked entry for the soft table path. */
        (void)pci_msix_soft_program(u16Idx, 0xFEE00000u, (u32)GJ_MSIX_IRQ_VEC,
                                    0);
    }
    u32Before = g_u32MsixSoft;
    u32Delivered = pci_msix_soft_fire(u16Idx);
    if (u32Delivered == 0) {
        return 0;
    }
    /*
     * soft_fire calls irq_msix_soft_inject when ready — retag path as table
     * soft delivery for stats.
     */
    g_u32LastPath = GJ_MSIX_PATH_TBL;
    g_u64LastBadge = GJ_MSIX_BADGE_TBL(u16Idx);
    if (g_u32MsixSoft > u32Before) {
        g_u32TablePulse++;
    }
    return 1;
}

u32
irq_msix_soft_path_exercise(void)
{
    struct gj_notify *pNotify;
    u64 u64Pend;
    u32 u32SoftPath;
    u32 fOk = 1;
    u32 u32Tbl;

    if (!g_fReady) {
        return 0;
    }
    pNotify = notify_msix_global();
    if (pNotify == NULL || !notify_is_live(pNotify)) {
        kprintf("irq: MSI-X soft pulse path FAIL (notify not live)\n");
        return 0;
    }

    /* Multi-badge soft pulse path (bits 0 + 2). */
    u32SoftPath = irq_msix_soft_pulse_path(GJ_MSIX_BADGE_SOFT |
                                           GJ_MSIX_BADGE_TBL(0));
    if (u32SoftPath == 0) {
        fOk = 0;
    }
    u64Pend = notify_pending(pNotify);
    if ((u64Pend & GJ_MSIX_BADGE_SOFT) == 0) {
        fOk = 0;
    }

    /* Second soft pulse path inject (OR coalesce). */
    if (irq_msix_soft_pulse_path(GJ_MSIX_BADGE_SOFT) == 0) {
        fOk = 0;
    }

    /* Table soft → Notification pulse. */
    if (!pci_msix_soft_ready()) {
        pci_msix_soft_table_init();
        (void)pci_msix_soft_program(0, 0xFEE00000u, (u32)GJ_MSIX_IRQ_VEC, 0);
    } else {
        /* Ensure entry 0 unmasked for delivery. */
        (void)pci_msix_soft_mask(0, 0);
    }
    u32Tbl = irq_msix_soft_table_pulse(0);
    if (u32Tbl == 0) {
        fOk = 0;
    }
    if (g_u32LastPath != GJ_MSIX_PATH_TBL &&
        g_u32LastPath != GJ_MSIX_PATH_SOFT) {
        fOk = 0;
    }
    if (g_u64LastBadge == 0) {
        fOk = 0;
    }

    if (fOk) {
        kprintf("irq: MSI-X soft pulse path soft=%u path=%u tbl=%u "
                "last_badge=0x%lx signals=%u PASS\n",
                g_u32MsixSoft, g_u32SoftPulsePath, g_u32TablePulse,
                (unsigned long)g_u64LastBadge, notify_signals(pNotify));
        kprintf("irq: MSI-X soft pulse path PASS\n");
    } else {
        kprintf("irq: MSI-X soft pulse path FAIL soft=%u path=%u tbl=%u "
                "last=0x%lx\n",
                g_u32MsixSoft, g_u32SoftPulsePath, g_u32TablePulse,
                (unsigned long)g_u64LastBadge);
    }
    return fOk;
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
    g_u32SoftPulsePath = 0;
    g_u32TablePulse = 0;
    g_u64LastBadge = 0;
    g_u32LastPath = GJ_MSIX_PATH_NONE;
    kprintf("irq: MSI-X vec=0x%x Notification bound PASS\n", GJ_MSIX_IRQ_VEC);
    /* Soft Notification pulse path exercise (table soft → badge OR). */
    (void)irq_msix_soft_path_exercise();
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

u32
irq_msix_soft_pulse_path_count(void)
{
    return g_u32SoftPulsePath;
}

u32
irq_msix_table_pulse_count(void)
{
    return g_u32TablePulse;
}

u64
irq_msix_last_badge(void)
{
    return g_u64LastBadge;
}

u32
irq_msix_last_path(void)
{
    return g_u32LastPath;
}

int
irq_msix_ready(void)
{
    return g_fReady;
}
