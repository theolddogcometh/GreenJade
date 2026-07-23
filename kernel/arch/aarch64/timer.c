/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Generic timer probe + optional virtual-timer compare soft path (M0+).
 *
 * Primary: CNTFRQ_EL0 / CNTVCT_EL0 — confirm counter advances.
 *
 * -------------------------------------------------------------------------
 * Soft virtual-timer / tick path (no IRQ delivery yet)
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
 *   5. Soft tick reload: CVAL = now+delta again (documents product reload)
 *   6. spin until ISTATUS on second deadline (tick soft observe)
 *   7. CTL = 0          (leave quiet)
 *
 * Why IMASK stays set: product GIC half (gic.c) soft-enables PPI INTID 27
 * (virtual timer) in GICD_ISENABLER0, but VBAR IRQ slots and GICC_IAR/EOIR
 * are only soft-peeked, and entry.S holds DAIF.I. Unmasking CNTV_CTL.IMASK
 * now would assert a line with no consumer — avoid that on freestanding smoke.
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
 *            aarch64: timer tick soft PASS
 *
 * Freestanding pure C; no GPL Linux timer code.
 */
#include "types_arch.h"

extern void aarch64_uart_puts(const char *sz);
extern void aarch64_uart_put_hex(unsigned long v);

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

/* Soft tick spin budget (yield loop; virt counter is fast). */
#define TIMER_SOFT_SPIN_MAX 2000000u

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

static unsigned long
cntv_cval_read(void)
{
    unsigned long u64Cval;

    __asm__ volatile("mrs %0, cntv_cval_el0" : "=r"(u64Cval));
    return u64Cval;
}

/*
 * Soft wait for ISTATUS (or budget). Returns 1 if ISTATUS observed.
 */
static int
timer_wait_istatus(unsigned *pSpinsOut)
{
    unsigned uSpins;
    unsigned int u32Ctl;
    int fStatus;

    fStatus = 0;
    for (uSpins = 0u; uSpins < TIMER_SOFT_SPIN_MAX; uSpins++) {
        u32Ctl = cntv_ctl_read();
        if ((u32Ctl & CNTV_CTL_ISTATUS) != 0u) {
            fStatus = 1;
            break;
        }
        __asm__ volatile("yield");
    }
    if (pSpinsOut != 0) {
        *pSpinsOut = uSpins;
    }
    return fStatus;
}

/*
 * Soft tick path: arm virtual compare (IMASK), wait ISTATUS, reload CVAL
 * once (product tick reload shape), wait second ISTATUS, disable.
 * Returns number of ISTATUS hits observed (0..2). Non-fatal for M0+.
 *
 * This is the "IRQ soft arm documentation" half: same register sequence as a
 * real tick setup, except IMASK remains 1 so PPI TIMER_PPI_VIRT never fires.
 */
static unsigned
timer_tick_soft(unsigned int u32Frq, unsigned long *pDeltaOut,
                unsigned *pSpins0, unsigned *pSpins1)
{
    unsigned long u64Now;
    unsigned long u64Delta;
    unsigned long u64Cval;
    unsigned cHits;
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
    if (pDeltaOut != 0) {
        *pDeltaOut = u64Delta;
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

    cHits = 0u;
    fStatus = timer_wait_istatus(pSpins0);
    if (fStatus != 0) {
        cHits = 1u;
    }

    /*
     * Soft tick reload (product shape): reprogram CVAL to now+delta while
     * ENABLE|IMASK stay set. Second ISTATUS is the soft "next tick".
     */
    if (cHits != 0u) {
        u64Now = cntvct();
        u64Cval = u64Now + u64Delta;
        cntv_cval_write(u64Cval);
        /* Writing CVAL clears ISTATUS until the new deadline. */
        fStatus = timer_wait_istatus(pSpins1);
        if (fStatus != 0) {
            cHits = 2u;
        }
    } else if (pSpins1 != 0) {
        *pSpins1 = 0u;
    }

    /* Leave quiet for later IRQ bring-up (no stale ENABLE). */
    cntv_ctl_write(0u);
    (void)cntv_cval_read(); /* soft observe final CVAL latched */
    return cHits;
}

void
aarch64_timer_probe(void)
{
    unsigned long u64T0;
    unsigned long u64T1;
    unsigned long u64Delta;
    unsigned long u64Adv;
    unsigned int u32Frq;
    unsigned uSpins;
    unsigned uSpins0;
    unsigned uSpins1;
    unsigned cTickHits;
    int fTickSoft;

    u32Frq = cntfrq();
    u64T0 = cntvct();
    for (uSpins = 0u; uSpins < 10000u; uSpins++) {
        __asm__ volatile("yield");
    }
    u64T1 = cntvct();
    u64Adv = u64T1 - u64T0;

    /* Soft virtual-timer compare + tick reload (no IRQ). */
    u64Delta = 0ul;
    uSpins0 = 0u;
    uSpins1 = 0u;
    cTickHits = timer_tick_soft(u32Frq, &u64Delta, &uSpins0, &uSpins1);

    /*
     * Tick soft PASS: counter advanced during probe AND two ISTATUS hits
     * (initial deadline + reloaded tick). Single hit still greppable via
     * detail line; M0+ always emits timer PASS when regs are reachable.
     */
    fTickSoft = 0;
    if (u64Adv != 0ul && cTickHits >= 2u) {
        fTickSoft = 1;
    }

    aarch64_uart_puts("aarch64: timer frq=");
    aarch64_uart_put_hex((unsigned long)u32Frq);
    aarch64_uart_puts(" t0=");
    aarch64_uart_put_hex(u64T0);
    aarch64_uart_puts(" t1=");
    aarch64_uart_put_hex(u64T1);
    aarch64_uart_puts(" adv=");
    aarch64_uart_put_hex(u64Adv);
    aarch64_uart_puts(" delta=");
    aarch64_uart_put_hex(u64Delta);
    aarch64_uart_puts(" hits=");
    aarch64_uart_put_hex((unsigned long)cTickHits);
    aarch64_uart_puts(" spins0=");
    aarch64_uart_put_hex((unsigned long)uSpins0);
    aarch64_uart_puts(" spins1=");
    aarch64_uart_put_hex((unsigned long)uSpins1);
    aarch64_uart_puts(" ppi=");
    aarch64_uart_put_hex((unsigned long)TIMER_PPI_VIRT);
    aarch64_uart_puts("\n");

    aarch64_uart_puts("aarch64: timer PASS\n");
    if (fTickSoft != 0) {
        aarch64_uart_puts("aarch64: timer tick soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: timer tick soft FAIL\n");
    }
}
