/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * x2APIC detect + optional mode enable (clean-room SDM).
 * When enabled, ICR uses MSRs instead of MMIO (large APIC ID friendly).
 */
#include <gj/apic.h>
#include <gj/cpu.h>
#include <gj/klog.h>
#include <gj/x2apic.h>

#define IA32_APIC_BASE_MSR   0x1Bu
#define APIC_BASE_X2APIC     (1ull << 10)
#define APIC_BASE_ENABLE     (1ull << 11)

/* x2APIC MSR base: 0x800 + (MMIO_offset / 0x10) */
#define X2APIC_MSR_EOI       0x80Bu
#define X2APIC_MSR_ICR       0x830u
#define X2APIC_MSR_SELF_IPI  0x83Fu

static int g_fSupported = -1;
/* Per-CPU: x2APIC mode is enabled independently on each logical CPU. */
static int g_aEnabled[16];

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

int
x2apic_supported(void)
{
    u32 u32Eax;
    u32 u32Ebx;
    u32 u32Ecx;
    u32 u32Edx;

    if (g_fSupported >= 0) {
        return g_fSupported;
    }
    u32Eax = 1;
    u32Ecx = 0;
    __asm__ volatile (
        "push %%rbx\n\t"
        "cpuid\n\t"
        "mov %%ebx, %1\n\t"
        "pop %%rbx"
        : "+a"(u32Eax), "=r"(u32Ebx), "+c"(u32Ecx), "=d"(u32Edx)
        :
        : "memory");
    g_fSupported = (u32Ecx & (1u << 21)) ? 1 : 0;
    (void)u32Eax;
    (void)u32Ebx;
    (void)u32Edx;
    return g_fSupported;
}

int
x2apic_enabled(void)
{
    u32 u32Slot = cpu_id();

    if (u32Slot >= 16u) {
        u32Slot = 0;
    }
    return g_aEnabled[u32Slot] != 0;
}

int
x2apic_enable(void)
{
    u64 u64Base;
    u32 u32Slot = cpu_id();

    if (!x2apic_supported()) {
        return -1;
    }
    if (u32Slot >= 16u) {
        u32Slot = 0;
    }
    if (g_aEnabled[u32Slot]) {
        return 0;
    }
    if (!apic_ready()) {
        return -1;
    }

    u64Base = rdmsr(IA32_APIC_BASE_MSR);
    /* Enable x2APIC: set bit 10 while bit 11 (APIC global enable) stays set. */
    u64Base |= APIC_BASE_ENABLE | APIC_BASE_X2APIC;
    wrmsr(IA32_APIC_BASE_MSR, u64Base);

    /* Verify */
    u64Base = rdmsr(IA32_APIC_BASE_MSR);
    if ((u64Base & APIC_BASE_X2APIC) == 0) {
        kprintf("x2apic: enable failed cpu=%u\n", u32Slot);
        return -1;
    }
    g_aEnabled[u32Slot] = 1;
    kprintf("x2apic: mode enabled cpu=%u\n", u32Slot);
    return 0;
}

void
x2apic_send_ipi_raw(u64 u64Icr)
{
    wrmsr(X2APIC_MSR_ICR, u64Icr);
}

void
x2apic_send_ipi(u32 u32ApicId, u8 u8Vector)
{
    /*
     * ICR layout in x2APIC: bits 7:0 vector, 10:8 delivery fixed=0,
     * dest in upper 32 bits of MSR write (EDX:EAX as 64-bit).
     */
    u64 u64Icr = (u64)u8Vector | (((u64)u32ApicId) << 32);

    x2apic_send_ipi_raw(u64Icr);
}

void
x2apic_send_self_ipi(u8 u8Vector)
{
    wrmsr(X2APIC_MSR_SELF_IPI, (u64)u8Vector);
}

void
x2apic_eoi(void)
{
    wrmsr(X2APIC_MSR_EOI, 0);
}

void
x2apic_probe(void)
{
    int f = x2apic_supported();

    /* Detect only — enable after timer calibrate + AP bring-up (safer order). */
    kprintf("x2apic: supported=%d (enable after SMP)\n", f);
}
