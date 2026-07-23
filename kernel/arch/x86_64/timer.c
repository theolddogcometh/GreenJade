/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Early mono clock: PIT ch0 @ GJ_TIMER_HZ on remapped IRQ0 (vector 32).
 * After LAPIC calibrate, jiffies/mono switch to the local APIC (vector 48).
 */
#include <gj/cap.h>
#include <gj/futex.h>
#include <gj/idt.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/timer.h>
#include <gj/types.h>

#define PIT_CH0     0x40
#define PIT_CMD     0x43
#define PIT_HZ_IN   1193182u
#define PIC1_CMD    0x20
#define PIC1_DATA   0x21
#define PIC2_CMD    0xA0
#define PIC2_DATA   0xA1
#define PIC_EOI     0x20

static volatile u64 g_u64Jiffies;
static int          g_fTimerReady;
static int          g_fApicSource;
static u64          g_u64NsecPerTick;
static u32          g_u32Quantum = 5; /* ticks */
static u32          g_u32SliceLeft = 5;
static u64          g_u64Preempts;

static void
outb(u16 u16Port, u8 u8Val)
{
    __asm__ volatile ("outb %0, %1" : : "a"(u8Val), "Nd"(u16Port));
}

static u8
inb(u16 u16Port)
{
    u8 u8Val;

    __asm__ volatile ("inb %1, %0" : "=a"(u8Val) : "Nd"(u16Port));
    return u8Val;
}

/* isr_stubs.S — IRQ0 after PIC remap (vector 32) */
extern void irq_stub_0(void);

static void
quantum_tick(void)
{
    if (g_u32SliceLeft > 0) {
        g_u32SliceLeft--;
    }
    if (g_u32SliceLeft == 0) {
        g_u64Preempts++;
        g_u32SliceLeft = g_u32Quantum ? g_u32Quantum : 1;
        /* Soft preempt: request yield at the next safe point */
        thread_yield_request();
    }
}

static void
revoke_hygiene_tick(void)
{
    /* Deferred CNode slot hygiene every 16 ticks (R7) */
    if ((g_u64Jiffies & 15ull) == 0) {
        (void)gj_revoke_process_deferred(8);
    }
}

void
timer_tick(void)
{
    if (!g_fApicSource) {
        g_u64Jiffies++;
        futex_timer_check();
        quantum_tick();
        revoke_hygiene_tick();
    }
    outb(PIC1_CMD, PIC_EOI);
}

void
timer_tick_apic(void)
{
    g_u64Jiffies++;
    futex_timer_check();
    quantum_tick();
    revoke_hygiene_tick();
}

void
timer_set_quantum_ticks(u32 u32Ticks)
{
    if (u32Ticks == 0) {
        u32Ticks = 1;
    }
    g_u32Quantum = u32Ticks;
    g_u32SliceLeft = u32Ticks;
}

u32
timer_quantum_ticks(void)
{
    return g_u32Quantum;
}

u64
timer_preempt_count(void)
{
    return g_u64Preempts;
}

void
timer_preempt_check(void)
{
    /* Yield requests are consumed on the scheduler idle path */
    (void)0;
}

/* C entry from irq_stub_0 (PIC timer) */
void
irq_timer_handler(void)
{
    timer_tick();
}

static void
pic_remap(void)
{
    u8 u8A1;
    u8 u8A2;

    u8A1 = inb(PIC1_DATA);
    u8A2 = inb(PIC2_DATA);

    outb(PIC1_CMD, 0x11);
    outb(PIC2_CMD, 0x11);
    outb(PIC1_DATA, 0x20); /* master offset 32 */
    outb(PIC2_DATA, 0x28); /* slave offset 40 */
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    /* Unmask IRQ0 only; leave remaining PIC lines masked */
    (void)u8A1;
    (void)u8A2;
    outb(PIC1_DATA, 0xFE); /* IRQ0 unmasked */
    outb(PIC2_DATA, 0xFF);
}

static void
pit_set_hz(u32 u32Hz)
{
    u32 u32Div;

    if (u32Hz == 0) {
        u32Hz = GJ_TIMER_HZ;
    }
    u32Div = PIT_HZ_IN / u32Hz;
    if (u32Div == 0) {
        u32Div = 1;
    }
    outb(PIT_CMD, 0x36); /* ch0, lobyte/hibyte, mode 3 */
    outb(PIT_CH0, (u8)(u32Div & 0xff));
    outb(PIT_CH0, (u8)((u32Div >> 8) & 0xff));
    g_u64NsecPerTick = 1000000000ull / (u64)u32Hz;
}

/* Public IDT gate install (idt.c); needed for vector 32 / IRQ0 */
void idt_set_gate(u32 u32Vec, void *pHandler, u8 u8Type);

void
timer_init(void)
{
    g_u64Jiffies = 0;
    g_fApicSource = 0;
    g_u64NsecPerTick = 1000000000ull / (u64)GJ_TIMER_HZ;
    pic_remap();
    pit_set_hz(GJ_TIMER_HZ);
    idt_set_gate(32, (void *)irq_stub_0, 0x8E);
    g_fTimerReady = 1;
    __asm__ volatile ("sti");
    kprintf("timer: PIT %u Hz IRQ0 vector 32 nsec/tick=%lu\n",
            (unsigned)GJ_TIMER_HZ, (unsigned long)g_u64NsecPerTick);
}

void
timer_set_apic_source(u64 u64NsecPerTick)
{
    if (u64NsecPerTick == 0) {
        return;
    }
    g_u64NsecPerTick = u64NsecPerTick;
    g_fApicSource = 1;
    /* Mask PIT IRQ0 so mono advances only from the APIC timer */
    outb(PIC1_DATA, 0xFF);
    kprintf("timer: mono source=APIC nsec/tick=%lu (PIT masked)\n",
            (unsigned long)g_u64NsecPerTick);
}

int
timer_apic_source(void)
{
    return g_fApicSource;
}

u64
timer_jiffies(void)
{
    return g_u64Jiffies;
}

u64
timer_mono_nsec(void)
{
    return g_u64Jiffies * g_u64NsecPerTick;
}

int
timer_ready(void)
{
    return g_fTimerReady;
}

void
timer_sleep_until(u64 u64DeadlineMonoNsec)
{
    u32 u32Guard;

    if (!g_fTimerReady) {
        return;
    }
    if (u64DeadlineMonoNsec == 0) {
        __asm__ volatile ("sti; hlt");
        return;
    }
    /*
     * Yield + STI/HLT so IRQs and other threads can progress. Cap spins so a
     * stuck mono clock (e.g. after a user #PF path) cannot hang forever.
     */
    for (u32Guard = 0;
         timer_mono_nsec() < u64DeadlineMonoNsec && u32Guard < 1000000u;
         u32Guard++) {
        thread_yield();
        __asm__ volatile ("sti; hlt");
    }
}
