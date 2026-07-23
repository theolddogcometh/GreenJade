/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal xAPIC memory-mapped local APIC (clean-room SDM).
 * Calibrated against PIT; product mono clock prefers APIC timer (vector 48).
 */
#include <gj/apic.h>
#include <gj/config.h>
#include <gj/cpu.h>
#include <gj/idt.h>
#include <gj/klog.h>
#include <gj/smp.h>
#include <gj/timer.h>
#include <gj/types.h>
#include <gj/x2apic.h>

#define IA32_APIC_BASE_MSR 0x1Bu
#define APIC_BASE_ENABLE   (1ull << 11)
#define APIC_BASE_BSP      (1ull << 8)

#define LAPIC_ID           0x20u
#define LAPIC_EOI          0xB0u
#define LAPIC_SVR          0xF0u
#define LAPIC_TIMER_LVT    0x320u
#define LAPIC_TIMER_INIT   0x380u
#define LAPIC_TIMER_CUR    0x390u
#define LAPIC_TIMER_DIV    0x3E0u
#define LAPIC_ICR_LOW      0x300u
#define LAPIC_ICR_HIGH     0x310u
#define LAPIC_SVR_ENABLE   (1u << 8)
#define LAPIC_TIMER_PERIODIC (1u << 17)
#define LAPIC_TIMER_MASKED   (1u << 16)
#define LAPIC_TIMER_ONESHOT  0u
#define LAPIC_TIMER_VEC    48u
#define LAPIC_IPI_RESCHED_VEC GJ_APIC_IPI_RESCHED_VEC
#define LAPIC_IPI_TLB_VEC     GJ_APIC_IPI_TLB_VEC
#define LAPIC_ICR_PENDING  (1u << 12)
/* TLB shootdown handshake (single outstanding request). */
static volatile gj_vaddr_t g_vaTlbShoot;
static volatile u32        g_u32TlbAck;
static volatile u32        g_u32TlbExpect;
#define LAPIC_ICR_INIT     (5u << 8)
#define LAPIC_ICR_STARTUP  (6u << 8)
#define LAPIC_ICR_LEVEL    (1u << 15)
#define LAPIC_ICR_ASSERT   (1u << 14)
#define LAPIC_ICR_EDGE     0u

/* Divide config: bits 0,1,3 — 0x3 = divide by 16 */
#define LAPIC_DIV_16       0x3u
#define LAPIC_DIV_FACTOR   16u

static volatile u32 *g_pLapic;
static int            g_fReady;
static int            g_fCalibrated;
static volatile u64   g_u64Ticks; /* BSP mono-driving ticks */
static volatile u64   g_aCpuTicks[GJ_CPU_STATIC_MAX];
static u64            g_u64BusHz;      /* timer counts per second (after /16) */
static u64            g_u64NsecPerTick;
static u32            g_u32PeriodInit;

extern void irq_stub_apic_timer(void);
extern void irq_stub_ipi_resched(void);
extern void irq_stub_ipi_tlb(void);

static u64
rdmsr(u32 u32Msr)
{
    u32 u32Lo;
    u32 u32Hi;

    __asm__ volatile ("rdmsr" : "=a"(u32Lo), "=d"(u32Hi) : "c"(u32Msr));
    return ((u64)u32Hi << 32) | u32Lo;
}

static void
wrmsr(u32 u32Msr, u64 u64Val)
{
    u32 u32Lo = (u32)u64Val;
    u32 u32Hi = (u32)(u64Val >> 32);

    __asm__ volatile ("wrmsr" : : "c"(u32Msr), "a"(u32Lo), "d"(u32Hi));
}

/* x2APIC MSR = 0x800 + (mmio_offset >> 4) */
static u32
lapic_msr(u32 u32Off)
{
    return 0x800u + (u32Off >> 4);
}

static void
lapic_w(u32 u32Off, u32 u32Val)
{
    if (x2apic_enabled()) {
        u32 u32Msr = lapic_msr(u32Off);
        u32 u32Lo = u32Val;
        u32 u32Hi = 0;

        /* ICR is 64-bit; other regs are 32-bit in low half */
        __asm__ volatile ("wrmsr" : : "c"(u32Msr), "a"(u32Lo), "d"(u32Hi));
        return;
    }
    if (g_pLapic) {
        g_pLapic[u32Off / 4] = u32Val;
    }
}

static u32
lapic_r(u32 u32Off)
{
    if (x2apic_enabled()) {
        u32 u32Msr = lapic_msr(u32Off);
        u32 u32Lo;
        u32 u32Hi;

        __asm__ volatile ("rdmsr" : "=a"(u32Lo), "=d"(u32Hi) : "c"(u32Msr));
        (void)u32Hi;
        return u32Lo;
    }
    return g_pLapic ? g_pLapic[u32Off / 4] : 0;
}

void
apic_timer_irq(void)
{
    u32 u32Cpu = 0;

    /* GS may not be live very early; treat as BSP then */
    if (g_fReady) {
        u32Cpu = cpu_id();
    }
    if (u32Cpu < GJ_CPU_STATIC_MAX) {
        g_aCpuTicks[u32Cpu]++;
    }
    /*
     * Only BSP advances global mono/jiffies. APs only EOI + per-cpu count so
     * HLT wakes without double-ticking the clock.
     */
    if (u32Cpu == 0) {
        g_u64Ticks++;
        if (timer_apic_source()) {
            timer_tick_apic();
        }
    }
    if (x2apic_enabled()) {
        x2apic_eoi();
    } else {
        lapic_w(LAPIC_EOI, 0);
    }
}

void
apic_init(void)
{
    u64 u64Base;
    u32 u32Id;

    g_fReady = 0;
    g_fCalibrated = 0;
    g_u64Ticks = 0;
    g_u64BusHz = 0;
    g_u64NsecPerTick = 0;
    g_u32PeriodInit = 0;
    u64Base = rdmsr(IA32_APIC_BASE_MSR);
    if ((u64Base & APIC_BASE_ENABLE) == 0) {
        u64Base |= APIC_BASE_ENABLE;
        wrmsr(IA32_APIC_BASE_MSR, u64Base);
    }
    g_pLapic = (volatile u32 *)(gj_vaddr_t)(u64Base & 0xfffff000ull);
    /* Enable spurious vector */
    lapic_w(LAPIC_SVR, LAPIC_SVR_ENABLE | 0xFFu);
    u32Id = lapic_r(LAPIC_ID) >> 24;
    idt_set_gate(LAPIC_TIMER_VEC, (void *)irq_stub_apic_timer, 0x8E);
    idt_set_gate(LAPIC_IPI_RESCHED_VEC, (void *)irq_stub_ipi_resched, 0x8E);
    idt_set_gate(LAPIC_IPI_TLB_VEC, (void *)irq_stub_ipi_tlb, 0x8E);
    /* Mask timer until programmed */
    lapic_w(LAPIC_TIMER_LVT, LAPIC_TIMER_MASKED | LAPIC_TIMER_VEC);
    lapic_w(LAPIC_TIMER_DIV, LAPIC_DIV_16);
    g_fReady = 1;
    kprintf("apic: xAPIC @%p id=%u\n", (void *)g_pLapic, u32Id);
}

void
apic_ipi_init(void)
{
    /* Gates installed in apic_init; kept for explicit call sites */
    if (g_fReady) {
        idt_set_gate(LAPIC_IPI_RESCHED_VEC, (void *)irq_stub_ipi_resched, 0x8E);
        idt_set_gate(LAPIC_IPI_TLB_VEC, (void *)irq_stub_ipi_tlb, 0x8E);
    }
}

void
apic_eoi(void)
{
    if (x2apic_enabled()) {
        x2apic_eoi();
    } else if (g_pLapic != NULL) {
        lapic_w(LAPIC_EOI, 0);
    }
}

void
apic_ipi_resched_irq(void)
{
    /* Wake from HLT; AP schedule loop picks work */
    apic_eoi();
}

void
apic_ipi_tlb_irq(void)
{
    gj_vaddr_t va = g_vaTlbShoot;

    if (va == 0) {
        /* Full reload of current CR3 */
        u64 u64Cr3;

        __asm__ volatile ("mov %%cr3, %0" : "=r"(u64Cr3));
        __asm__ volatile ("mov %0, %%cr3" : : "r"(u64Cr3) : "memory");
    } else {
        __asm__ volatile ("invlpg (%0)" : : "r"(va) : "memory");
    }
    __asm__ volatile ("" ::: "memory");
    g_u32TlbAck++;
    apic_eoi();
}

void
apic_send_resched(u32 u32CpuSlot)
{
    u32 u32ApicId;

    if (!g_fReady || u32CpuSlot == 0) {
        return;
    }
    u32ApicId = smp_apic_id_for_cpu(u32CpuSlot);
    apic_send_ipi(u32ApicId, (u8)LAPIC_IPI_RESCHED_VEC);
}

void
tlb_shootdown(gj_vaddr_t va)
{
    u32 u32Online;
    u32 u32Self;
    u32 u32Slot;
    u32 u32Spins;

    if (!g_fReady) {
        if (va != 0) {
            __asm__ volatile ("invlpg (%0)" : : "r"(va) : "memory");
        }
        return;
    }

    u32Online = smp_cpu_count_online();
    u32Self = cpu_id();
    if (u32Online <= 1) {
        if (va != 0) {
            __asm__ volatile ("invlpg (%0)" : : "r"(va) : "memory");
        }
        return;
    }

    g_vaTlbShoot = va;
    g_u32TlbAck = 0;
    g_u32TlbExpect = 0;
    __asm__ volatile ("" ::: "memory");

    for (u32Slot = 0; u32Slot < u32Online && u32Slot < GJ_CPU_STATIC_MAX;
         u32Slot++) {
        if (u32Slot == u32Self) {
            continue;
        }
        g_u32TlbExpect++;
        apic_send_ipi(smp_apic_id_for_cpu(u32Slot), (u8)LAPIC_IPI_TLB_VEC);
    }

    /* Local invalidate */
    if (va != 0) {
        __asm__ volatile ("invlpg (%0)" : : "r"(va) : "memory");
    } else {
        u64 u64Cr3;

        __asm__ volatile ("mov %%cr3, %0" : "=r"(u64Cr3));
        __asm__ volatile ("mov %0, %%cr3" : : "r"(u64Cr3) : "memory");
    }

    for (u32Spins = 0;
         u32Spins < 10000000u && g_u32TlbAck < g_u32TlbExpect;
         u32Spins++) {
        __asm__ volatile ("pause");
    }
    g_vaTlbShoot = 0;
}

int
apic_ready(void)
{
    return g_fReady;
}

int
apic_calibrate(void)
{
    u64 u64J0;
    u64 u64J1;
    u32 u32Cur;
    u32 u32Elapsed;
    u32 u32Spins;
    const u32 u32WaitTicks = 10u; /* 100 ms at 100 Hz */
    const u32 u32InitMax = 0xffffffffu;

    if (!g_fReady || !timer_ready()) {
        return -1;
    }

    /* Mask APIC timer; one-shot mode for measurement */
    lapic_w(LAPIC_TIMER_LVT, LAPIC_TIMER_MASKED | LAPIC_TIMER_ONESHOT | LAPIC_TIMER_VEC);
    lapic_w(LAPIC_TIMER_DIV, LAPIC_DIV_16);

    /* Align to a PIT tick boundary */
    u64J0 = timer_jiffies();
    u32Spins = 0;
    while (timer_jiffies() == u64J0 && u32Spins < 100000000u) {
        u32Spins++;
        __asm__ volatile ("pause");
    }
    if (timer_jiffies() == u64J0) {
        kprintf("apic: calibrate failed (PIT not ticking)\n");
        return -1;
    }

    /* Start countdown from max */
    lapic_w(LAPIC_TIMER_INIT, u32InitMax);
    u64J0 = timer_jiffies();
    u32Spins = 0;
    while ((timer_jiffies() - u64J0) < u32WaitTicks && u32Spins < 500000000u) {
        u32Spins++;
        __asm__ volatile ("pause");
    }
    u32Cur = lapic_r(LAPIC_TIMER_CUR);
    u64J1 = timer_jiffies();
    lapic_w(LAPIC_TIMER_INIT, 0); /* stop */

    if (u64J1 <= u64J0 || u32Cur >= u32InitMax) {
        kprintf("apic: calibrate failed j0=%lu j1=%lu cur=0x%x\n",
                (unsigned long)u64J0, (unsigned long)u64J1, u32Cur);
        return -1;
    }

    u32Elapsed = u32InitMax - u32Cur;
    /*
     * u32Elapsed counts over (j1-j0) PIT ticks at GJ_TIMER_HZ.
     * bus_hz = elapsed * GJ_TIMER_HZ / (j1-j0)
     */
    g_u64BusHz = ((u64)u32Elapsed * (u64)GJ_TIMER_HZ) / (u64J1 - u64J0);
    if (g_u64BusHz < 1000ull) {
        kprintf("apic: calibrate bus_hz too low %lu\n",
                (unsigned long)g_u64BusHz);
        g_u64BusHz = 0;
        return -1;
    }
    g_fCalibrated = 1;
    kprintf("apic: calibrated bus_hz=%lu (div16, %lu PIT ticks)\n",
            (unsigned long)g_u64BusHz, (unsigned long)(u64J1 - u64J0));
    return 0;
}

int
apic_calibrated(void)
{
    return g_fCalibrated;
}

u64
apic_bus_hz(void)
{
    return g_u64BusHz;
}

void
apic_timer_hz(u32 u32Hz)
{
    u32 u32Init;

    if (!g_fReady || u32Hz == 0) {
        return;
    }

    if (g_fCalibrated && g_u64BusHz != 0) {
        u32Init = (u32)(g_u64BusHz / (u64)u32Hz);
    } else {
        /*
         * Best-effort without calibration: QEMU-ish ~1 GHz / 16.
         */
        u32Init = 1000000000u / LAPIC_DIV_FACTOR / u32Hz;
    }
    if (u32Init < 1000u) {
        u32Init = 1000u;
    }
    g_u32PeriodInit = u32Init;
    g_u64NsecPerTick = 1000000000ull / (u64)u32Hz;

    lapic_w(LAPIC_TIMER_DIV, LAPIC_DIV_16);
    lapic_w(LAPIC_TIMER_INIT, u32Init);
    lapic_w(LAPIC_TIMER_LVT, LAPIC_TIMER_PERIODIC | LAPIC_TIMER_VEC);

    if (g_fCalibrated) {
        timer_set_apic_source(g_u64NsecPerTick);
        kprintf("apic: timer %u Hz init=%u nsec/tick=%lu (mono=APIC)\n",
                u32Hz, u32Init, (unsigned long)g_u64NsecPerTick);
    } else {
        kprintf("apic: timer ~%u Hz init=%u (uncalibrated)\n", u32Hz, u32Init);
    }
}

u64
apic_timer_ticks(void)
{
    return g_u64Ticks;
}

u64
apic_timer_ticks_cpu(u32 u32Cpu)
{
    if (u32Cpu >= GJ_CPU_STATIC_MAX) {
        return 0;
    }
    return g_aCpuTicks[u32Cpu];
}

u64
apic_nsec_per_tick(void)
{
    return g_u64NsecPerTick;
}

u32
apic_timer_init_count(void)
{
    return g_u32PeriodInit;
}

void
apic_timer_start_local(void)
{
    u32 u32Init = g_u32PeriodInit;

    if (!g_fReady) {
        return;
    }
    if (u32Init == 0) {
        u32Init = 1000000000u / LAPIC_DIV_FACTOR / GJ_TIMER_HZ;
        if (u32Init < 1000u) {
            u32Init = 1000u;
        }
    }
    /* Enable this CPU's APIC (APs start with SVR disabled until we set it) */
    {
        u64 u64Base = rdmsr(IA32_APIC_BASE_MSR);

        if ((u64Base & APIC_BASE_ENABLE) == 0) {
            u64Base |= APIC_BASE_ENABLE;
            wrmsr(IA32_APIC_BASE_MSR, u64Base);
        }
        g_pLapic = (volatile u32 *)(gj_vaddr_t)(u64Base & 0xfffff000ull);
    }
    lapic_w(LAPIC_SVR, LAPIC_SVR_ENABLE | 0xFFu);
    lapic_w(LAPIC_TIMER_DIV, LAPIC_DIV_16);
    lapic_w(LAPIC_TIMER_INIT, u32Init);
    lapic_w(LAPIC_TIMER_LVT, LAPIC_TIMER_PERIODIC | LAPIC_TIMER_VEC);
}

static void
apic_icr_wait(void)
{
    u32 u32Spins = 0;

    while ((lapic_r(LAPIC_ICR_LOW) & LAPIC_ICR_PENDING) != 0 &&
           u32Spins < 1000000u) {
        u32Spins++;
        __asm__ volatile ("pause");
    }
}

void
apic_send_ipi(u32 u32ApicId, u8 u8Vector)
{
    if (!g_fReady) {
        return;
    }
    if (x2apic_enabled()) {
        x2apic_send_ipi(u32ApicId, u8Vector);
        return;
    }
    apic_icr_wait();
    lapic_w(LAPIC_ICR_HIGH, (u32ApicId & 0xffu) << 24);
    lapic_w(LAPIC_ICR_LOW, (u32)u8Vector); /* fixed delivery, edge, dest physical */
    apic_icr_wait();
}

void
apic_send_self_ipi(u8 u8Vector)
{
    if (!g_fReady) {
        return;
    }
    if (x2apic_enabled()) {
        x2apic_send_self_ipi(u8Vector);
        return;
    }
    /* xAPIC ICR dest shorthand: self = bits 19:18 = 01 */
    apic_icr_wait();
    lapic_w(LAPIC_ICR_HIGH, 0);
    lapic_w(LAPIC_ICR_LOW, (u32)u8Vector | (1u << 18));
    apic_icr_wait();
}

void
apic_send_init_sipi(u32 u32ApicId, u8 u8StartupVector)
{
    u32 i;

    if (!g_fReady) {
        return;
    }
    if (x2apic_enabled()) {
        /*
         * x2APIC ICR: delivery mode in bits 10:8, vector in 7:0,
         * destination in bits 63:32. INIT=5, STARTUP=6. Level/assert
         * not used the same way; single INIT + two SIPIs (Intel SDM).
         */
        u64 u64Dest = ((u64)u32ApicId) << 32;

        x2apic_send_ipi_raw(u64Dest | (5ull << 8)); /* INIT */
        for (i = 0; i < 1000000u; i++) {
            __asm__ volatile ("pause");
        }
        for (i = 0; i < 2u; i++) {
            x2apic_send_ipi_raw(u64Dest | (6ull << 8) | (u64)u8StartupVector);
            {
                u32 j;

                for (j = 0; j < 200000u; j++) {
                    __asm__ volatile ("pause");
                }
            }
        }
        return;
    }
    /* INIT assert */
    apic_icr_wait();
    lapic_w(LAPIC_ICR_HIGH, (u32ApicId & 0xffu) << 24);
    lapic_w(LAPIC_ICR_LOW, LAPIC_ICR_INIT | LAPIC_ICR_LEVEL | LAPIC_ICR_ASSERT);
    apic_icr_wait();
    /* INIT deassert */
    lapic_w(LAPIC_ICR_HIGH, (u32ApicId & 0xffu) << 24);
    lapic_w(LAPIC_ICR_LOW, LAPIC_ICR_INIT | LAPIC_ICR_LEVEL);
    apic_icr_wait();
    /* Brief delay via spins (≈10ms class on QEMU) */
    for (i = 0; i < 1000000u; i++) {
        __asm__ volatile ("pause");
    }
    /* Two SIPIs */
    for (i = 0; i < 2u; i++) {
        apic_icr_wait();
        lapic_w(LAPIC_ICR_HIGH, (u32ApicId & 0xffu) << 24);
        lapic_w(LAPIC_ICR_LOW, LAPIC_ICR_STARTUP | (u32)u8StartupVector);
        apic_icr_wait();
        {
            u32 j;

            for (j = 0; j < 200000u; j++) {
                __asm__ volatile ("pause");
            }
        }
    }
}
