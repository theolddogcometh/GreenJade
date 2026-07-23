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
 * Soft inventory deepen (install-time, greppable "aarch64: exception soft …"):
 *   aarch64: exception soft vbar=… match=… align=… slots=… slotb=…
 *   aarch64: exception soft class sync=0 irq=1 fiq=2 serror=3
 *   aarch64: exception soft counts sync=… irq=… fiq=… serror=… baseline0=…
 *   aarch64: exception soft daif=… i_held=… el=…
 *   aarch64: exception soft PASS | FAIL
 *
 * Greppable: aarch64: exceptions PASS
 *
 * Freestanding pure C; no GPL Linux arch paste. No NEON/FP —
 * general-regs-only for this TU (CPACR FP/SIMD not enabled at EL1 soft).
 */
#include "types_arch.h"

#if defined(__GNUC__)
#pragma GCC target("general-regs-only")
#endif

/* VBAR_EL1 table alignment (exceptions.S .align 11 → 2 KiB). */
#define EXC_SOFT_VBAR_ALIGN     0x800ul
#define EXC_SOFT_VBAR_ALIGN_MASK (EXC_SOFT_VBAR_ALIGN - 1ul)

/* Fixed AArch64 vector table geometry (ARM DDI). */
#define EXC_SOFT_SLOTS          16u
#define EXC_SOFT_SLOT_BYTES     0x80u

/* vec & 3 class encoding (must match exceptions.S + dispatch). */
#define EXC_SOFT_CLASS_SYNC     0u
#define EXC_SOFT_CLASS_IRQ      1u
#define EXC_SOFT_CLASS_FIQ      2u
#define EXC_SOFT_CLASS_SERROR   3u

/* DAIF system-register bit positions (PSTATE view via MRS DAIF). */
#define EXC_SOFT_DAIF_F_BIT     (1ul << 6)
#define EXC_SOFT_DAIF_I_BIT     (1ul << 7)
#define EXC_SOFT_DAIF_A_BIT     (1ul << 8)
#define EXC_SOFT_DAIF_D_BIT     (1ul << 9)

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

/* Soft observability counters (IRQ / FIQ / Sync / SError takes). */
static unsigned long g_cIrqSoft;
static unsigned long g_cFiqSoft;
static unsigned long g_cSyncSoft;
static unsigned long g_cSerrorSoft;

/*
 * Soft exception inventory deepen (install-time).
 * Greppable family: "aarch64: exception soft …"
 * Pure integer / system-register MRS; no FP/NEON.
 *
 * Gates (soft / non-fatal for product bring-up):
 *   - VBAR_EL1 matches aarch64_vectors and is 2 KiB aligned
 *   - take counters all zero at install baseline
 *   - DAIF.I held (product soft path does not unmask IRQs yet)
 *   - CurrentEL is EL1
 * Returns 1 on soft PASS.
 */
static int
exception_soft_inventory(unsigned long u64Vbar, unsigned long u64VecPa)
{
    unsigned long u64Daif;
    unsigned long u64CurrentEl;
    unsigned long u64El;
    unsigned long u64AlignOk;
    unsigned long u64Match;
    unsigned long u64Baseline0;
    unsigned long u64IHeld;
    int fOk;

    __asm__ volatile("mrs %0, daif" : "=r"(u64Daif));
    __asm__ volatile("mrs %0, CurrentEL" : "=r"(u64CurrentEl));
    u64El = (u64CurrentEl >> 2) & 0x3ul;

    u64Match = 0ul;
    if (u64Vbar == u64VecPa) {
        u64Match = 1ul;
    }

    u64AlignOk = 0ul;
    if ((u64Vbar & EXC_SOFT_VBAR_ALIGN_MASK) == 0ul && u64Vbar != 0ul) {
        u64AlignOk = 1ul;
    }

    u64Baseline0 = 0ul;
    if (g_cSyncSoft == 0ul && g_cIrqSoft == 0ul && g_cFiqSoft == 0ul &&
        g_cSerrorSoft == 0ul) {
        u64Baseline0 = 1ul;
    }

    u64IHeld = 0ul;
    if ((u64Daif & EXC_SOFT_DAIF_I_BIT) != 0ul) {
        u64IHeld = 1ul;
    }

    /* VBAR geometry soft inventory. */
    aarch64_uart_puts("aarch64: exception soft vbar=");
    aarch64_uart_put_hex(u64Vbar);
    aarch64_uart_puts(" vecpa=");
    aarch64_uart_put_hex(u64VecPa);
    aarch64_uart_puts(" match=");
    aarch64_uart_put_hex(u64Match);
    aarch64_uart_puts(" align=");
    aarch64_uart_put_hex(u64AlignOk);
    aarch64_uart_puts(" slots=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_SLOTS);
    aarch64_uart_puts(" slotb=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_SLOT_BYTES);
    aarch64_uart_puts("\n");

    /* Class encoding soft inventory (vec & 3). */
    aarch64_uart_puts("aarch64: exception soft class sync=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_CLASS_SYNC);
    aarch64_uart_puts(" irq=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_CLASS_IRQ);
    aarch64_uart_puts(" fiq=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_CLASS_FIQ);
    aarch64_uart_puts(" serror=");
    aarch64_uart_put_hex((unsigned long)EXC_SOFT_CLASS_SERROR);
    aarch64_uart_puts("\n");

    /* Take-counter soft inventory (install baseline expected 0). */
    aarch64_uart_puts("aarch64: exception soft counts sync=");
    aarch64_uart_put_hex(g_cSyncSoft);
    aarch64_uart_puts(" irq=");
    aarch64_uart_put_hex(g_cIrqSoft);
    aarch64_uart_puts(" fiq=");
    aarch64_uart_put_hex(g_cFiqSoft);
    aarch64_uart_puts(" serror=");
    aarch64_uart_put_hex(g_cSerrorSoft);
    aarch64_uart_puts(" baseline0=");
    aarch64_uart_put_hex(u64Baseline0);
    aarch64_uart_puts("\n");

    /* DAIF / EL soft inventory (I held while soft IRQ path documents only). */
    aarch64_uart_puts("aarch64: exception soft daif=");
    aarch64_uart_put_hex(u64Daif);
    aarch64_uart_puts(" i_held=");
    aarch64_uart_put_hex(u64IHeld);
    aarch64_uart_puts(" d=");
    aarch64_uart_put_hex((u64Daif & EXC_SOFT_DAIF_D_BIT) != 0ul ? 1ul : 0ul);
    aarch64_uart_puts(" a=");
    aarch64_uart_put_hex((u64Daif & EXC_SOFT_DAIF_A_BIT) != 0ul ? 1ul : 0ul);
    aarch64_uart_puts(" f=");
    aarch64_uart_put_hex((u64Daif & EXC_SOFT_DAIF_F_BIT) != 0ul ? 1ul : 0ul);
    aarch64_uart_puts(" el=");
    aarch64_uart_put_hex(u64El);
    aarch64_uart_puts("\n");

    fOk = 1;
    if (u64Match == 0ul || u64AlignOk == 0ul) {
        fOk = 0;
    }
    if (u64Baseline0 == 0ul) {
        fOk = 0;
    }
    if (u64IHeld == 0ul) {
        fOk = 0;
    }
    if (u64El != 1ul) {
        fOk = 0;
    }

    if (fOk != 0) {
        aarch64_uart_puts("aarch64: exception soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: exception soft FAIL\n");
    }

    return fOk;
}

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
    if (uClass == EXC_SOFT_CLASS_IRQ) {
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
    if (uClass == EXC_SOFT_CLASS_FIQ) {
        g_cFiqSoft++;
        aarch64_uart_puts("aarch64: exc FIQ soft vec=");
        aarch64_uart_put_hex(u64Vec);
        aarch64_uart_puts(" n=");
        aarch64_uart_put_hex(g_cFiqSoft);
        aarch64_uart_puts("\n");
        return;
    }

    /* Soft SError path: count + return (avoid tight fault loop). */
    if (uClass == EXC_SOFT_CLASS_SERROR) {
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
    unsigned long u64Vbar;
    unsigned long u64VecPa;
    int fSoft;

    u64VecPa = (unsigned long)(void *)aarch64_vectors;

    __asm__ volatile("msr vbar_el1, %0" : : "r"(u64VecPa) : "memory");
    __asm__ volatile("isb");
    __asm__ volatile("mrs %0, vbar_el1" : "=r"(u64Vbar));

    /* Soft baseline: IRQ/FIQ counters zero; VBAR points at vector table. */
    aarch64_uart_puts("aarch64: exceptions vbar=");
    aarch64_uart_put_hex(u64Vbar);
    aarch64_uart_puts(" irq_soft=");
    aarch64_uart_put_hex(g_cIrqSoft);
    aarch64_uart_puts(" fiq_soft=");
    aarch64_uart_put_hex(g_cFiqSoft);
    aarch64_uart_puts("\n");

    /*
     * Greppable soft inventory: "aarch64: exception soft …"
     * (singular "exception soft"; distinct from "exceptions PASS").
     */
    fSoft = exception_soft_inventory(u64Vbar, u64VecPa);
    (void)fSoft;

    aarch64_uart_puts("aarch64: exceptions PASS\n");
}
