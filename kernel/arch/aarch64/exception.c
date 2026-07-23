/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * EL1 exception dispatch. Routes SVC64; recovers PSCI probe traps (HVC/SMC
 * without firmware) by advancing ELR so kmain does not UNDEF-loop.
 */
#include "types_arch.h"

extern void aarch64_uart_puts(const char *sz);
extern void aarch64_uart_put_hex(unsigned long v);
extern int aarch64_svc_try_handle(unsigned long u64Vec, unsigned long esr,
                                  unsigned long elr);
extern int aarch64_psci_exception_recover(unsigned long *pElr,
                                          unsigned long *pX0);
extern volatile int g_psci_probe_active;

void
aarch64_exception_dispatch(unsigned long u64Vec)
{
    unsigned long esr;
    unsigned long elr;
    unsigned long x0fix;
    unsigned ec;

    __asm__ volatile("mrs %0, esr_el1" : "=r"(esr));
    __asm__ volatile("mrs %0, elr_el1" : "=r"(elr));
    ec = (unsigned)((esr >> 26) & 0x3fu);

    /* PSCI probe: HVC (0x16) / SMC (0x17) / unknown UNDEF — recover. */
    if (g_psci_probe_active && (u64Vec & 3ul) == 0ul) {
        if (ec == 0x16u || ec == 0x17u || ec == 0x00u || ec == 0x0eu) {
            x0fix = 0;
            if (aarch64_psci_exception_recover(&elr, &x0fix) != 0) {
                __asm__ volatile("msr elr_el1, %0" :: "r"(elr) : "memory");
                return;
            }
        }
    }

    if ((u64Vec & 3ul) == 0ul) {
        if (aarch64_svc_try_handle(u64Vec, esr, elr) != 0) {
            return;
        }
    }

    aarch64_uart_puts("aarch64: exc vec=");
    aarch64_uart_put_hex(u64Vec);
    aarch64_uart_puts(" esr=");
    aarch64_uart_put_hex(esr);
    aarch64_uart_puts(" elr=");
    aarch64_uart_put_hex(elr);
    aarch64_uart_puts("\n");
}

void
aarch64_exceptions_install(void)
{
    extern char aarch64_vectors[];

    __asm__ volatile("msr vbar_el1, %0" : : "r"(aarch64_vectors) : "memory");
    __asm__ volatile("isb");
    aarch64_uart_puts("aarch64: exceptions PASS\n");
}
