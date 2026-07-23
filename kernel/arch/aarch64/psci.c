/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * PSCI probe with conduit auto-select (HVC preferred on QEMU virt; SMC when
 * EL3/firmware present). Exception recovery: if SMC/HVC faults, handler
 * advances ELR past the instruction and returns NOT_SUPPORTED.
 *
 * Greppable:
 *   aarch64: psci PASS conduit=hvc|smc version=…
 *   aarch64: psci soft SKIP (no firmware)
 */
#include "types_arch.h"
#include <stddef.h>

extern void aarch64_uart_puts(const char *sz);
extern void aarch64_uart_put_hex(unsigned long v);

#define PSCI_VERSION           0x84000000ul
#define PSCI_CPU_ON_64         0xC4000003ul
#define PSCI_FEATURES          0x8400000Aul
#define PSCI_RET_NOT_SUPPORTED (-1l)

/* Set by exception path when recovering a PSCI probe trap. */
volatile unsigned long g_psci_probe_fault;
volatile unsigned long g_psci_probe_result;
volatile int g_psci_probe_active;

enum psci_conduit {
    PSCI_CONDUIT_NONE = 0,
    PSCI_CONDUIT_HVC = 1,
    PSCI_CONDUIT_SMC = 2
};

static int g_nConduit = PSCI_CONDUIT_NONE;

/*
 * Called from exception.c when g_psci_probe_active and sync exception.
 * Advances ELR by 4 (skip the smc/hvc) and returns 1 if handled.
 */
int
aarch64_psci_exception_recover(unsigned long *pElr, unsigned long *pX0)
{
    if (!g_psci_probe_active) {
        return 0;
    }
    g_psci_probe_fault = 1;
    g_psci_probe_result = (unsigned long)(long)PSCI_RET_NOT_SUPPORTED;
    if (pElr != NULL) {
        *pElr = *pElr + 4ul; /* skip 32-bit HVC/SMC encoding */
    }
    if (pX0 != NULL) {
        *pX0 = g_psci_probe_result;
    }
    return 1;
}

static unsigned long
psci_hvc(unsigned long a0, unsigned long a1, unsigned long a2, unsigned long a3)
{
    register unsigned long x0 __asm__("x0") = a0;
    register unsigned long x1 __asm__("x1") = a1;
    register unsigned long x2 __asm__("x2") = a2;
    register unsigned long x3 __asm__("x3") = a3;

    g_psci_probe_fault = 0;
    g_psci_probe_active = 1;
    __asm__ volatile("hvc #0"
                     : "+r"(x0), "+r"(x1), "+r"(x2), "+r"(x3)
                     :
                     : "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11",
                       "x12", "x13", "x14", "x15", "x16", "x17", "memory");
    g_psci_probe_active = 0;
    if (g_psci_probe_fault) {
        return g_psci_probe_result;
    }
    return x0;
}

static unsigned long
psci_smc(unsigned long a0, unsigned long a1, unsigned long a2, unsigned long a3)
{
    register unsigned long x0 __asm__("x0") = a0;
    register unsigned long x1 __asm__("x1") = a1;
    register unsigned long x2 __asm__("x2") = a2;
    register unsigned long x3 __asm__("x3") = a3;

    g_psci_probe_fault = 0;
    g_psci_probe_active = 1;
    __asm__ volatile("smc #0"
                     : "+r"(x0), "+r"(x1), "+r"(x2), "+r"(x3)
                     :
                     : "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11",
                       "x12", "x13", "x14", "x15", "x16", "x17", "memory");
    g_psci_probe_active = 0;
    if (g_psci_probe_fault) {
        return g_psci_probe_result;
    }
    return x0;
}

static unsigned long
psci_call(unsigned long a0, unsigned long a1, unsigned long a2, unsigned long a3)
{
    if (g_nConduit == PSCI_CONDUIT_HVC) {
        return psci_hvc(a0, a1, a2, a3);
    }
    if (g_nConduit == PSCI_CONDUIT_SMC) {
        return psci_smc(a0, a1, a2, a3);
    }
    return (unsigned long)(long)PSCI_RET_NOT_SUPPORTED;
}

/*
 * Soft detect EL3 presence via ID_AA64PFR0_EL1.EL3 field [15:12].
 * Non-zero means EL3 implemented (firmware may still use HVC conduit).
 */
static int
el3_implemented(void)
{
    unsigned long pfr0;

    __asm__ volatile("mrs %0, id_aa64pfr0_el1" : "=r"(pfr0));
    return (((pfr0 >> 12) & 0xful) != 0ul) ? 1 : 0;
}

void
aarch64_psci_probe(void)
{
    unsigned long u64Ver;
    unsigned long feat;
    unsigned maj;
    unsigned min;
    int fEl3;

    fEl3 = el3_implemented();

    /* Prefer HVC (QEMU virt / KVM common conduit). */
    g_nConduit = PSCI_CONDUIT_HVC;
    u64Ver = psci_call(PSCI_VERSION, 0, 0, 0);
    if ((long)u64Ver == PSCI_RET_NOT_SUPPORTED || g_psci_probe_fault) {
        /* Try SMC when EL3 present or HVC failed. */
        g_nConduit = PSCI_CONDUIT_SMC;
        u64Ver = psci_call(PSCI_VERSION, 0, 0, 0);
    }

    if ((long)u64Ver == PSCI_RET_NOT_SUPPORTED || g_psci_probe_fault) {
        g_nConduit = PSCI_CONDUIT_NONE;
        aarch64_uart_puts("aarch64: psci soft SKIP (no firmware)\n");
        (void)fEl3;
        return;
    }

    maj = (unsigned)((u64Ver >> 16) & 0xfffful);
    min = (unsigned)(u64Ver & 0xfffful);
    aarch64_uart_puts("aarch64: psci PASS conduit=");
    aarch64_uart_puts(g_nConduit == PSCI_CONDUIT_HVC ? "hvc" : "smc");
    aarch64_uart_puts(" version=");
    aarch64_uart_put_hex((unsigned long)maj);
    aarch64_uart_puts(".");
    aarch64_uart_put_hex((unsigned long)min);
    aarch64_uart_puts(" el3=");
    aarch64_uart_put_hex((unsigned long)fEl3);
    aarch64_uart_puts("\n");

    feat = psci_call(PSCI_FEATURES, PSCI_CPU_ON_64, 0, 0);
    aarch64_uart_puts("aarch64: psci cpu_on_64 feat=");
    aarch64_uart_put_hex(feat);
    aarch64_uart_puts("\n");
}
