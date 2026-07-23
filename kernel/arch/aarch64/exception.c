/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * EL1 exception dispatch. Routes SVC64; recovers PSCI probe traps (HVC/SMC
 * without firmware) by advancing ELR so kmain does not UNDEF-loop.
 *
 * -------------------------------------------------------------------------
 * Soft IRQ observability (pairs with gic.c SPI soft + timer tick soft)
 * -------------------------------------------------------------------------
 * VBAR slots (Current EL, SP_ELx):
 *   vec 4 = Sync   vec 5 = IRQ   vec 6 = FIQ   vec 7 = SError
 * IRQ soft path (no product delivery yet — DAIF.I held, timer IMASK set):
 *   on vec%4==1: soft-ACK via aarch64_gic_soft_iar, EOI non-spurious,
 *   count take, return (avoid re-entry loop). Greppable counter line at
 *   install time documents zero baseline; live takes print soft IRQ line.
 *
 * Greppable: aarch64: exceptions PASS
 *
 * Freestanding pure C; no GPL Linux arch paste.
 */
#include "types_arch.h"

extern void aarch64_uart_puts(const char *sz);
extern void aarch64_uart_put_hex(unsigned long v);
extern int aarch64_svc_try_handle(unsigned long u64Vec, unsigned long esr,
                                  unsigned long elr);
extern int aarch64_psci_exception_recover(unsigned long *pElr,
                                          unsigned long *pX0);
extern volatile int g_psci_probe_active;

/* Soft GIC IAR/EOI helpers (gic.c) — IRQ soft path only. */
extern unsigned aarch64_gic_soft_iar(void);
extern void aarch64_gic_soft_eoi(unsigned uIntId);

/* Soft observability counters (IRQ / FIQ takes; baseline 0 at install). */
static unsigned long g_cIrqSoft;
static unsigned long g_cFiqSoft;
static unsigned long g_cSyncSoft;
static unsigned long g_cSerrorSoft;

void
aarch64_exception_dispatch(unsigned long u64Vec)
{
    unsigned long esr;
    unsigned long elr;
    unsigned long x0fix;
    unsigned ec;
    unsigned uClass;
    unsigned uIntId;

    uClass = (unsigned)(u64Vec & 3ul);

    /* Soft IRQ path: vec%4 == 1 (IRQ at any exception level slot). */
    if (uClass == 1u) {
        g_cIrqSoft++;
        uIntId = aarch64_gic_soft_iar();
        if (uIntId != 1023u) {
            aarch64_gic_soft_eoi(uIntId);
        }
        aarch64_uart_puts("aarch64: exc IRQ soft intid=");
        aarch64_uart_put_hex((unsigned long)uIntId);
        aarch64_uart_puts(" vec=");
        aarch64_uart_put_hex(u64Vec);
        aarch64_uart_puts(" n=");
        aarch64_uart_put_hex(g_cIrqSoft);
        aarch64_uart_puts("\n");
        return;
    }

    /* Soft FIQ path: count + return (no GIC group1 take yet). */
    if (uClass == 2u) {
        g_cFiqSoft++;
        aarch64_uart_puts("aarch64: exc FIQ soft vec=");
        aarch64_uart_put_hex(u64Vec);
        aarch64_uart_puts(" n=");
        aarch64_uart_put_hex(g_cFiqSoft);
        aarch64_uart_puts("\n");
        return;
    }

    /* Soft SError path: count + return (avoid tight fault loop). */
    if (uClass == 3u) {
        g_cSerrorSoft++;
        aarch64_uart_puts("aarch64: exc SError soft vec=");
        aarch64_uart_put_hex(u64Vec);
        aarch64_uart_puts(" n=");
        aarch64_uart_put_hex(g_cSerrorSoft);
        aarch64_uart_puts("\n");
        return;
    }

    /* Sync (class 0): ESR/ELR path. */
    g_cSyncSoft++;
    __asm__ volatile("mrs %0, esr_el1" : "=r"(esr));
    __asm__ volatile("mrs %0, elr_el1" : "=r"(elr));
    ec = (unsigned)((esr >> 26) & 0x3fu);

    /* PSCI probe: HVC (0x16) / SMC (0x17) / unknown UNDEF — recover. */
    if (g_psci_probe_active != 0) {
        if (ec == 0x16u || ec == 0x17u || ec == 0x00u || ec == 0x0eu) {
            x0fix = 0;
            if (aarch64_psci_exception_recover(&elr, &x0fix) != 0) {
                __asm__ volatile("msr elr_el1, %0" :: "r"(elr) : "memory");
                return;
            }
        }
    }

    if (aarch64_svc_try_handle(u64Vec, esr, elr) != 0) {
        return;
    }

    aarch64_uart_puts("aarch64: exc vec=");
    aarch64_uart_put_hex(u64Vec);
    aarch64_uart_puts(" esr=");
    aarch64_uart_put_hex(esr);
    aarch64_uart_puts(" elr=");
    aarch64_uart_put_hex(elr);
    aarch64_uart_puts(" sync_n=");
    aarch64_uart_put_hex(g_cSyncSoft);
    aarch64_uart_puts("\n");
}

void
aarch64_exceptions_install(void)
{
    extern char aarch64_vectors[];
    unsigned long vbar;

    __asm__ volatile("msr vbar_el1, %0" : : "r"(aarch64_vectors) : "memory");
    __asm__ volatile("isb");
    __asm__ volatile("mrs %0, vbar_el1" : "=r"(vbar));

    /* Soft baseline: IRQ/FIQ counters zero; VBAR points at vector table. */
    aarch64_uart_puts("aarch64: exceptions vbar=");
    aarch64_uart_put_hex(vbar);
    aarch64_uart_puts(" irq_soft=");
    aarch64_uart_put_hex(g_cIrqSoft);
    aarch64_uart_puts(" fiq_soft=");
    aarch64_uart_put_hex(g_cFiqSoft);
    aarch64_uart_puts("\n");
    aarch64_uart_puts("aarch64: exceptions PASS\n");
}
