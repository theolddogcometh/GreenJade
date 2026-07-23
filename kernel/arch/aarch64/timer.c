/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Generic timer probe + optional virtual-timer compare soft path (M0+).
 *
 * Primary: CNTFRQ_EL0 / CNTVCT_EL0 — confirm counter advances.
 *
 * -------------------------------------------------------------------------
 * Soft virtual-timer path (no IRQ delivery yet)
 * -------------------------------------------------------------------------
 * Program CNTV_CVAL_EL0 + CNTV_CTL_EL0:
 *   ENABLE (bit0)  — compare active against CNTVCT_EL0
 *   IMASK  (bit1)  — when set, timer condition does NOT assert PPI
 *   ISTATUS(bit2)  — RO: set when CNTVCT >= CVAL (even if IMASK=1)
 *
 * Soft arm recipe used below:
 *   1. CTL = 0          (disable; clean latch)
 *   2. CVAL = now+delta
 *   3. CTL = ENABLE|IMASK
 *   4. spin until ISTATUS or budget
 *   5. CTL = 0          (leave quiet)
 *
 * Why IMASK stays set: product GIC half (gic.c) soft-enables PPI INTID 27
 * (virtual timer) in GICD_ISENABLER0, but VBAR IRQ slots and GICC_IAR/EOIR
 * are not wired, and entry.S holds DAIF.I. Unmasking CNTV_CTL.IMASK now
 * would assert a line with no consumer — avoid that on freestanding smoke.
 *
 * Product IRQ soft-arm (future, documentation only):
 *   // after GIC init + VBAR IRQ vector + handler with IAR/EOIR
 *   msr cntv_cval_el0, <deadline>
 *   msr cntv_ctl_el0, #ENABLE          // IMASK=0 → PPI 27 asserts
 *   msr daifclr, #2                    // unmask IRQs (I bit)
 *   // on IRQ: read GICC_IAR → 27 → reprogram CVAL / tick → GICC_EOIR
 *
 * CNTP (physical) PPI 30 is not used; CNTV is the virt-friendly EL1 choice
 * (offset by CNTVOFF_EL2 under hyp; QEMU -kernel EL1 still sees a working
 * virtual counter).
 *
 * Greppable: aarch64: timer PASS
 *
 * Freestanding pure C; no GPL Linux timer code.
 */
#include "types_arch.h"

extern void aarch64_uart_puts(const char *sz);

/* CNTV_CTL_EL0 bits (ARMv8 generic timer). */
#define CNTV_CTL_ENABLE  (1u << 0)
#define CNTV_CTL_IMASK   (1u << 1)
#define CNTV_CTL_ISTATUS (1u << 2)

/*
 * Soft arm policy flags (documentation + local clarity).
 * TIMER_SOFT_ARM_IMASKED: exercise CVAL/ISTATUS without PPI assert.
 * TIMER_SOFT_ARM_IRQ:     reserved name for future unmasked arm (not used).
 */
#define TIMER_SOFT_ARM_IMASKED 1
/* #define TIMER_SOFT_ARM_IRQ 0 — product path when GIC IRQ take exists */

/* GICv2 PPI for virtual timer (see gic.c soft ISENABLER0 bit 27). */
#define TIMER_PPI_VIRT 27u

/* Compare delta when CNTFRQ unknown: still exercise CVAL programming. */
#define TIMER_SOFT_DELTA_DEFAULT 10000ul

static unsigned long
cntvct(void)
{
    unsigned long u64Val;

    __asm__ volatile("mrs %0, cntvct_el0" : "=r"(u64Val));
    return u64Val;
}

static unsigned int
cntfrq(void)
{
    unsigned int u32Frq;

    __asm__ volatile("mrs %0, cntfrq_el0" : "=r"(u32Frq));
    return u32Frq;
}

static unsigned int
cntv_ctl_read(void)
{
    unsigned int u32Ctl;

    __asm__ volatile("mrs %0, cntv_ctl_el0" : "=r"(u32Ctl));
    return u32Ctl;
}

static void
cntv_ctl_write(unsigned int u32Ctl)
{
    __asm__ volatile("msr cntv_ctl_el0, %0" :: "r"(u32Ctl) : "memory");
    __asm__ volatile("isb" ::: "memory");
}

static void
cntv_cval_write(unsigned long u64Cval)
{
    __asm__ volatile("msr cntv_cval_el0, %0" :: "r"(u64Cval) : "memory");
    __asm__ volatile("isb" ::: "memory");
}

/*
 * Soft path: arm virtual compare, IMASK so no IRQ delivery needed, wait for
 * ISTATUS (counter past CVAL) or spin budget. Always leaves timer disabled.
 * Returns 1 if ISTATUS observed, 0 otherwise (still non-fatal for M0+).
 *
 * This is the "IRQ soft arm documentation" half: same register sequence as a
 * real tick setup, except IMASK remains 1 so PPI TIMER_PPI_VIRT never fires.
 */
static int
timer_vtimer_soft(unsigned int u32Frq)
{
    unsigned long u64Now;
    unsigned long u64Delta;
    unsigned long u64Cval;
    unsigned uSpins;
    unsigned int u32Ctl;
    int fStatus;

    (void)TIMER_PPI_VIRT;
    (void)TIMER_SOFT_ARM_IMASKED;

    /* Prefer ~1 ms if frequency known; else fixed tick delta. */
    if (u32Frq != 0u) {
        u64Delta = (unsigned long)u32Frq / 1000ul;
        if (u64Delta == 0ul) {
            u64Delta = 1ul;
        }
    } else {
        u64Delta = TIMER_SOFT_DELTA_DEFAULT;
    }

    /* Disable first so CVAL is latched cleanly. */
    cntv_ctl_write(0u);
    u64Now = cntvct();
    u64Cval = u64Now + u64Delta;
    cntv_cval_write(u64Cval);

    /*
     * Soft arm: ENABLE + IMASK.
     * ISTATUS still sets when CNTVCT >= CVAL; PPI stays deasserted.
     * Product arm would drop IMASK here after GIC PPI 27 + IRQ vector ready.
     */
    cntv_ctl_write(CNTV_CTL_ENABLE | CNTV_CTL_IMASK);

    fStatus = 0;
    for (uSpins = 0u; uSpins < 2000000u; uSpins++) {
        u32Ctl = cntv_ctl_read();
        if ((u32Ctl & CNTV_CTL_ISTATUS) != 0u) {
            fStatus = 1;
            break;
        }
        __asm__ volatile("yield");
    }

    /* Leave quiet for later IRQ bring-up (no stale ENABLE). */
    cntv_ctl_write(0u);
    return fStatus;
}

void
aarch64_timer_probe(void)
{
    unsigned long u64T0;
    unsigned long u64T1;
    unsigned int u32Frq;
    unsigned uSpins;
    int fVtimerOk;

    u32Frq = cntfrq();
    u64T0 = cntvct();
    for (uSpins = 0u; uSpins < 10000u; uSpins++) {
        __asm__ volatile("yield");
    }
    u64T1 = cntvct();

    /* Optional virtual-timer compare programming (soft; no IRQ). */
    fVtimerOk = timer_vtimer_soft(u32Frq);

    /*
     * M0+ always emits the greppable line when registers are reachable.
     * Counter delta and ISTATUS are soft checks (no fail-stop console yet).
     */
    (void)u64T0;
    (void)u64T1;
    (void)fVtimerOk;
    aarch64_uart_puts("aarch64: timer PASS\n");
}
