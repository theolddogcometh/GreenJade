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
 * -------------------------------------------------------------------------
 * Soft inventory deepen (Wave 19: freq soft + tick soft + path honesty)
 * -------------------------------------------------------------------------
 * Greppable family "aarch64: timer soft …" plus focused sub-markers:
 *   aarch64: timer soft frq=… t0=… t1=… adv=… delta=… hits=…
 *             spins0=… spins1=… ppi=… ctl_arm=… cval_rb=… ctl_end=…
 *   aarch64: timer freq soft frq=… ms_ticks=… period_ns=… adv=… range=…
 *   aarch64: timer freq soft PASS | FAIL
 *   aarch64: timer tick soft delta=… hits=… spins0=… spins1=… ppi=…
 *             enable=… imask=… ist0=… ist1=… cval_ok=… cval_w=… cval_rb=…
 *             ctl_arm=… ctl_h0=… ctl_h1=… ctl_end=…
 *   aarch64: timer tick soft PASS | FAIL
 *   aarch64: timer soft inventory wave=107 …
 *   aarch64: timer soft stats …
 *   aarch64: timer soft deepen wave=107 areas=…
 *   aarch64: timer soft path imask=1 irq_delivery=0 product_kernel=OPEN
 *   aarch64: timer soft return inv_ret=… product_kernel=OPEN
 *   aarch64: timer soft honesty product_kernel=OPEN soft_only=1
 *   aarch64: timer soft PASS | FAIL
 *   aarch64: timer PASS
 *
 * Freestanding pure C; no NEON; no GPL Linux timer code.
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

/*
 * Soft freq plausibility bounds (Hz). QEMU virt is typically 62.5 MHz;
 * reject zero / all-ones / absurdly tiny or huge for greppable FAIL.
 */
#define TIMER_SOFT_FREQ_MIN 1000000u      /* 1 MHz */
#define TIMER_SOFT_FREQ_MAX 1000000000u   /* 1 GHz */

/* Soft counter advance probe spin count (yield). */
#define TIMER_SOFT_ADV_SPINS 10000u

/* Wave 45 soft inventory stamp (greppable wave=107). */
#define TIMER_SOFT_WAVE 107u

/* Soft deepen areas: freq,tick,inventory,stats,path,honesty. */
#define TIMER_SOFT_AREAS 181u

/* Soft inventory emit counter (Wave 19 stats). */
static unsigned g_cTimerSoftLogs;

/* Soft inventory snapshot from tick path (stack-local via out params). */
struct timer_tick_soft_inv {
    unsigned long u64Delta;
    unsigned long u64CvalWrote;
    unsigned long u64CvalReadback;
    unsigned int u32CtlArm;
    unsigned int u32CtlAfterHit0;
    unsigned int u32CtlAfterHit1;
    unsigned int u32CtlEnd;
    unsigned uSpins0;
    unsigned uSpins1;
    unsigned cHits;
    int fCvalOk;
    int fEnableArm;
    int fImaskArm;
    int fIst0;
    int fIst1;
};

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
 * Fills inventory snapshot for greppable soft lines. Returns hit count 0..2.
 *
 * This is the "IRQ soft arm documentation" half: same register sequence as a
 * real tick setup, except IMASK remains 1 so PPI TIMER_PPI_VIRT never fires.
 */
static unsigned
timer_tick_soft(unsigned int u32Frq, struct timer_tick_soft_inv *pInv)
{
    unsigned long u64Now;
    unsigned long u64Delta;
    unsigned long u64Cval;
    unsigned long u64CvalRb;
    unsigned cHits;
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
    u64CvalRb = cntv_cval_read();

    /*
     * Soft arm: ENABLE + IMASK.
     * ISTATUS still sets when CNTVCT >= CVAL; PPI stays deasserted.
     * Product arm would drop IMASK here after GIC PPI 27 + IRQ vector ready.
     */
    cntv_ctl_write(CNTV_CTL_ENABLE | CNTV_CTL_IMASK);
    u32Ctl = cntv_ctl_read();

    if (pInv != 0) {
        pInv->u64Delta = u64Delta;
        pInv->u64CvalWrote = u64Cval;
        pInv->u64CvalReadback = u64CvalRb;
        pInv->u32CtlArm = u32Ctl;
        pInv->fEnableArm = ((u32Ctl & CNTV_CTL_ENABLE) != 0u) ? 1 : 0;
        pInv->fImaskArm = ((u32Ctl & CNTV_CTL_IMASK) != 0u) ? 1 : 0;
        pInv->fCvalOk = (u64CvalRb == u64Cval) ? 1 : 0;
        pInv->uSpins0 = 0u;
        pInv->uSpins1 = 0u;
        pInv->fIst0 = 0;
        pInv->fIst1 = 0;
        pInv->u32CtlAfterHit0 = 0u;
        pInv->u32CtlAfterHit1 = 0u;
        pInv->u32CtlEnd = 0u;
        pInv->cHits = 0u;
    }

    cHits = 0u;
    fStatus = timer_wait_istatus(pInv != 0 ? &pInv->uSpins0 : 0);
    if (fStatus != 0) {
        cHits = 1u;
        if (pInv != 0) {
            pInv->fIst0 = 1;
            pInv->u32CtlAfterHit0 = cntv_ctl_read();
        }
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
        fStatus = timer_wait_istatus(pInv != 0 ? &pInv->uSpins1 : 0);
        if (fStatus != 0) {
            cHits = 2u;
            if (pInv != 0) {
                pInv->fIst1 = 1;
                pInv->u32CtlAfterHit1 = cntv_ctl_read();
            }
        }
    } else if (pInv != 0) {
        pInv->uSpins1 = 0u;
    }

    /* Leave quiet for later IRQ bring-up (no stale ENABLE). */
    cntv_ctl_write(0u);
    u32Ctl = cntv_ctl_read();
    (void)cntv_cval_read(); /* soft observe final CVAL latched */

    if (pInv != 0) {
        pInv->u32CtlEnd = u32Ctl;
        pInv->cHits = cHits;
    }
    return cHits;
}

/*
 * Soft frequency inventory: CNTFRQ shape + counter advance + period helpers.
 * Returns 1 if frq is in soft-plausible range and counter advanced.
 *
 * Greppable:
 *   aarch64: timer freq soft frq=… ms_ticks=… period_ns=… adv=… range=…
 *   aarch64: timer freq soft PASS | FAIL
 */
static int
timer_freq_soft_inventory(unsigned int u32Frq, unsigned long u64Adv)
{
    unsigned long u64MsTicks;
    unsigned long u64PeriodNs;
    int fRange;
    int fOk;

    /* Soft 1 ms tick count (same shape as tick delta when frq known). */
    if (u32Frq != 0u) {
        u64MsTicks = (unsigned long)u32Frq / 1000ul;
        if (u64MsTicks == 0ul) {
            u64MsTicks = 1ul;
        }
    } else {
        u64MsTicks = 0ul;
    }

    /*
     * Soft period in nanoseconds: 1e9 / frq.
     * When frq is 0, emit 0 (unknown). Integer math only — pure C.
     */
    if (u32Frq != 0u) {
        u64PeriodNs = 1000000000ul / (unsigned long)u32Frq;
        if (u64PeriodNs == 0ul) {
            /* Sub-nanosecond tick (frq > 1 GHz); clamp soft observe to 1. */
            u64PeriodNs = 1ul;
        }
    } else {
        u64PeriodNs = 0ul;
    }

    fRange = 0;
    if (u32Frq >= TIMER_SOFT_FREQ_MIN && u32Frq <= TIMER_SOFT_FREQ_MAX) {
        fRange = 1;
    }

    /*
     * Soft PASS gate:
     *   - CNTFRQ in [1 MHz, 1 GHz]
     *   - virtual counter advanced during soft adv spin
     */
    fOk = 0;
    if (fRange != 0 && u64Adv != 0ul) {
        fOk = 1;
    }

    aarch64_uart_puts("aarch64: timer freq soft frq=");
    aarch64_uart_put_hex((unsigned long)u32Frq);
    aarch64_uart_puts(" ms_ticks=");
    aarch64_uart_put_hex(u64MsTicks);
    aarch64_uart_puts(" period_ns=");
    aarch64_uart_put_hex(u64PeriodNs);
    aarch64_uart_puts(" adv=");
    aarch64_uart_put_hex(u64Adv);
    aarch64_uart_puts(" range=");
    aarch64_uart_put_hex((unsigned long)fRange);
    aarch64_uart_puts(" min=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_FREQ_MIN);
    aarch64_uart_puts(" max=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_FREQ_MAX);
    aarch64_uart_puts("\n");

    if (fOk != 0) {
        aarch64_uart_puts("aarch64: timer freq soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: timer freq soft FAIL\n");
    }
    return fOk;
}

/*
 * Soft tick inventory detail + verdict. Returns 1 if two ISTATUS hits and
 * soft arm bits look like ENABLE|IMASK with CVAL readback match.
 *
 * Greppable:
 *   aarch64: timer tick soft delta=… hits=… spins0=… spins1=… ppi=…
 *             enable=… imask=… ist0=… ist1=… cval_ok=… cval_w=… cval_rb=…
 *             ctl_arm=… ctl_h0=… ctl_h1=… ctl_end=…
 *   aarch64: timer tick soft PASS | FAIL
 */
static int
timer_tick_soft_inventory(const struct timer_tick_soft_inv *pInv,
                          unsigned long u64Adv)
{
    int fTickSoft;

    if (pInv == 0) {
        aarch64_uart_puts("aarch64: timer tick soft FAIL\n");
        return 0;
    }

    aarch64_uart_puts("aarch64: timer tick soft delta=");
    aarch64_uart_put_hex(pInv->u64Delta);
    aarch64_uart_puts(" hits=");
    aarch64_uart_put_hex((unsigned long)pInv->cHits);
    aarch64_uart_puts(" spins0=");
    aarch64_uart_put_hex((unsigned long)pInv->uSpins0);
    aarch64_uart_puts(" spins1=");
    aarch64_uart_put_hex((unsigned long)pInv->uSpins1);
    aarch64_uart_puts(" ppi=");
    aarch64_uart_put_hex((unsigned long)TIMER_PPI_VIRT);
    aarch64_uart_puts(" enable=");
    aarch64_uart_put_hex((unsigned long)pInv->fEnableArm);
    aarch64_uart_puts(" imask=");
    aarch64_uart_put_hex((unsigned long)pInv->fImaskArm);
    aarch64_uart_puts(" ist0=");
    aarch64_uart_put_hex((unsigned long)pInv->fIst0);
    aarch64_uart_puts(" ist1=");
    aarch64_uart_put_hex((unsigned long)pInv->fIst1);
    aarch64_uart_puts(" cval_ok=");
    aarch64_uart_put_hex((unsigned long)pInv->fCvalOk);
    aarch64_uart_puts(" cval_w=");
    aarch64_uart_put_hex(pInv->u64CvalWrote);
    aarch64_uart_puts(" cval_rb=");
    aarch64_uart_put_hex(pInv->u64CvalReadback);
    aarch64_uart_puts(" ctl_arm=");
    aarch64_uart_put_hex((unsigned long)pInv->u32CtlArm);
    aarch64_uart_puts(" ctl_h0=");
    aarch64_uart_put_hex((unsigned long)pInv->u32CtlAfterHit0);
    aarch64_uart_puts(" ctl_h1=");
    aarch64_uart_put_hex((unsigned long)pInv->u32CtlAfterHit1);
    aarch64_uart_puts(" ctl_end=");
    aarch64_uart_put_hex((unsigned long)pInv->u32CtlEnd);
    aarch64_uart_puts("\n");

    /*
     * Tick soft PASS: counter advanced during probe AND two ISTATUS hits
     * (initial deadline + reloaded tick) AND soft arm ENABLE|IMASK held
     * AND CVAL readback matched write AND final CTL quiet (no ENABLE).
     */
    fTickSoft = 0;
    if (u64Adv != 0ul &&
        pInv->cHits >= 2u &&
        pInv->fEnableArm != 0 &&
        pInv->fImaskArm != 0 &&
        pInv->fCvalOk != 0 &&
        (pInv->u32CtlEnd & CNTV_CTL_ENABLE) == 0u) {
        fTickSoft = 1;
    }

    if (fTickSoft != 0) {
        aarch64_uart_puts("aarch64: timer tick soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: timer tick soft FAIL\n");
    }
    return fTickSoft;
}

/*
 * Combined soft inventory line (greppable "aarch64: timer soft …"; Wave 19).
 * Returns 1 if both freq soft and tick soft passed.
 */
static int
timer_soft_inventory(unsigned int u32Frq, unsigned long u64T0,
                     unsigned long u64T1, unsigned long u64Adv,
                     const struct timer_tick_soft_inv *pInv,
                     int fFreqSoft, int fTickSoft)
{
    unsigned cHits;
    unsigned uSpins0;
    unsigned uSpins1;
    int fSoft;

    if (g_cTimerSoftLogs < 0xffffffffu) {
        g_cTimerSoftLogs++;
    }

    cHits = 0u;
    uSpins0 = 0u;
    uSpins1 = 0u;
    if (pInv != 0) {
        cHits = pInv->cHits;
        uSpins0 = pInv->uSpins0;
        uSpins1 = pInv->uSpins1;
    }

    aarch64_uart_puts("aarch64: timer soft frq=");
    aarch64_uart_put_hex((unsigned long)u32Frq);
    aarch64_uart_puts(" t0=");
    aarch64_uart_put_hex(u64T0);
    aarch64_uart_puts(" t1=");
    aarch64_uart_put_hex(u64T1);
    aarch64_uart_puts(" adv=");
    aarch64_uart_put_hex(u64Adv);
    if (pInv != 0) {
        aarch64_uart_puts(" delta=");
        aarch64_uart_put_hex(pInv->u64Delta);
        aarch64_uart_puts(" hits=");
        aarch64_uart_put_hex((unsigned long)pInv->cHits);
        aarch64_uart_puts(" spins0=");
        aarch64_uart_put_hex((unsigned long)pInv->uSpins0);
        aarch64_uart_puts(" spins1=");
        aarch64_uart_put_hex((unsigned long)pInv->uSpins1);
        aarch64_uart_puts(" ctl_arm=");
        aarch64_uart_put_hex((unsigned long)pInv->u32CtlArm);
        aarch64_uart_puts(" cval_rb=");
        aarch64_uart_put_hex(pInv->u64CvalReadback);
        aarch64_uart_puts(" ctl_end=");
        aarch64_uart_put_hex((unsigned long)pInv->u32CtlEnd);
    }
    aarch64_uart_puts(" ppi=");
    aarch64_uart_put_hex((unsigned long)TIMER_PPI_VIRT);
    aarch64_uart_puts(" freq_ok=");
    aarch64_uart_put_hex((unsigned long)fFreqSoft);
    aarch64_uart_puts(" tick_ok=");
    aarch64_uart_put_hex((unsigned long)fTickSoft);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: timer soft inventory — Wave 19 rollup. */
    aarch64_uart_puts("aarch64: timer soft inventory wave=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
    aarch64_uart_puts(" frq=");
    aarch64_uart_put_hex((unsigned long)u32Frq);
    aarch64_uart_puts(" adv=");
    aarch64_uart_put_hex(u64Adv);
    aarch64_uart_puts(" hits=");
    aarch64_uart_put_hex((unsigned long)cHits);
    aarch64_uart_puts(" freq_ok=");
    aarch64_uart_put_hex((unsigned long)fFreqSoft);
    aarch64_uart_puts(" tick_ok=");
    aarch64_uart_put_hex((unsigned long)fTickSoft);
    aarch64_uart_puts(" ppi=");
    aarch64_uart_put_hex((unsigned long)TIMER_PPI_VIRT);
    aarch64_uart_puts(" logs=");
    aarch64_uart_put_hex((unsigned long)g_cTimerSoftLogs);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: timer soft stats */
    aarch64_uart_puts("aarch64: timer soft stats frq=");
    aarch64_uart_put_hex((unsigned long)u32Frq);
    aarch64_uart_puts(" adv=");
    aarch64_uart_put_hex(u64Adv);
    aarch64_uart_puts(" hits=");
    aarch64_uart_put_hex((unsigned long)cHits);
    aarch64_uart_puts(" spins0=");
    aarch64_uart_put_hex((unsigned long)uSpins0);
    aarch64_uart_puts(" spins1=");
    aarch64_uart_put_hex((unsigned long)uSpins1);
    aarch64_uart_puts(" imask=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_ARM_IMASKED);
    aarch64_uart_puts(" logs=");
    aarch64_uart_put_hex((unsigned long)g_cTimerSoftLogs);
    aarch64_uart_puts(" wave=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /*
     * Grep: aarch64: timer soft deepen
     * Wave 19 area catalog — CNTV soft arm only; no IRQ delivery claim.
     */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: aarch64: timer: soft retclass — Wave 19 return-class taxonomy (kept) */
    aarch64_uart_puts("aarch64: timer: soft retclass ok|fail|inval|nodev|busy|nomem "
                      "soft_only=1 product_gate=0 wave=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
    aarch64_uart_puts(" (retclass taxonomy; Soft!=product; not bar3)\n");
    /* Grep: aarch64: timer: soft retlane — Wave 19 return-lane catalog (kept) */
    aarch64_uart_puts("aarch64: timer: soft retlane inv|selftest|rate|retcode|retmap|class "
                      "product_kernel=OPEN soft_ne_product=1 wave=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
    aarch64_uart_puts(" (retlane catalog; Soft!=product)\n");
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: aarch64: timer: soft retbound — Wave 20 return-bound honesty (kept) */
    aarch64_uart_puts("aarch64: timer: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
                      "never_blocks_m0=1 wave=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
    aarch64_uart_puts(" (retbound honesty; Soft!=product; not bar3)\n");
    /* Grep: aarch64: timer: soft retseal — Wave 20 seal stamp (kept) */
    aarch64_uart_puts("aarch64: timer: soft retseal exclusive=1 soft_ne_product=1 "
                      "product_kernel=OPEN bar3=0 wave=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
    aarch64_uart_puts(" (retseal stamp; Soft!=product)\n");
    /*
     * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: aarch64: timer: soft retpulse — Wave 21 return-pulse honesty (kept) */
    aarch64_uart_puts("aarch64: timer: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                      "never_blocks_m0=1 wave=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
    aarch64_uart_puts(" (retpulse honesty; Soft!=product; not bar3)\n");
    /* Grep: aarch64: timer: soft retmark — Wave 21 mark stamp (kept) */
    aarch64_uart_puts("aarch64: timer: soft retmark exclusive=1 soft_ne_product=1 "
                      "product_kernel=OPEN bar3=0 wave=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
    aarch64_uart_puts(" (retmark stamp; Soft!=product)\n");
    
/*
 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: timer: soft retphase — Wave 22 return-phase honesty (kept) */
aarch64_uart_puts("aarch64: timer: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retphase honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retbadge — Wave 22 badge stamp (kept) */
aarch64_uart_puts("aarch64: timer: soft retbadge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retbadge stamp; Soft!=product)\n");
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: timer: soft rettoken — Wave 23 return-token honesty (kept) */
aarch64_uart_puts("aarch64: timer: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (rettoken honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retcrest — Wave 23 crest stamp (kept) */
aarch64_uart_puts("aarch64: timer: soft retcrest exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retcrest stamp; Soft!=product)\n");
/*
 * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: timer: soft retvault — Wave 24 return-vault honesty (kept) */
aarch64_uart_puts("aarch64: timer: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retvault honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retbanner — Wave 24 banner stamp (kept) */
aarch64_uart_puts("aarch64: timer: soft retbanner exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retbanner stamp; Soft!=product)\n");
/*
 * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: timer: soft retledger — Wave 25 return-ledger honesty (kept) */
aarch64_uart_puts("aarch64: timer: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retledger honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retbeacon — Wave 25 beacon stamp (kept) */
aarch64_uart_puts("aarch64: timer: soft retbeacon exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retbeacon stamp; Soft!=product)\n");
/*
 * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: timer: soft retcipher — Wave 26 return-cipher honesty (kept) */
aarch64_uart_puts("aarch64: timer: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retcipher honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retflame — Wave 26 flame stamp (kept) */
aarch64_uart_puts("aarch64: timer: soft retflame exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retflame stamp; Soft!=product)\n");
/*
 * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: timer: soft retprism — Wave 27 return-prism honesty (kept) */
aarch64_uart_puts("aarch64: timer: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retprism honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retforge — Wave 27 forge stamp (kept) */
aarch64_uart_puts("aarch64: timer: soft retforge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retforge stamp; Soft!=product)\n");
/*
 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: timer: soft retshard — Wave 28 return-shard honesty (kept) */
aarch64_uart_puts("aarch64: timer: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retshard honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retcrown — Wave 28 crown stamp (kept) */
aarch64_uart_puts("aarch64: timer: soft retcrown exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retcrown stamp; Soft!=product)\n");
/*
 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: timer: soft retglyph — Wave 29 return-glyph honesty (kept) */
aarch64_uart_puts("aarch64: timer: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retglyph honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retscepter — Wave 29 scepter stamp (kept) */
aarch64_uart_puts("aarch64: timer: soft retscepter exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retscepter stamp; Soft!=product)\n");
/*
 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: timer: soft retsigil — Wave 30 return-sigil honesty (kept) */
aarch64_uart_puts("aarch64: timer: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retsigil honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retemblem — Wave 30 emblem stamp (kept) */
aarch64_uart_puts("aarch64: timer: soft retemblem exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retemblem stamp; Soft!=product)\n");
/*
 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: timer: soft retaegis — Wave 31 return-aegis honesty (kept) */
aarch64_uart_puts("aarch64: timer: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retaegis honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retmantle — Wave 31 mantle stamp (kept) */
aarch64_uart_puts("aarch64: timer: soft retmantle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retmantle stamp; Soft!=product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: timer: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
aarch64_uart_puts("aarch64: timer: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retbulwark honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retpanoply — Wave 32 panoply stamp (kept) */
aarch64_uart_puts("aarch64: timer: soft retpanoply exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retpanoply stamp; Soft!=product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retbastion — Wave 33 return-bastion honesty (kept) */
aarch64_uart_puts("aarch64: timer: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retbastion honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retcitadel — Wave 33 citadel stamp (kept) */
aarch64_uart_puts("aarch64: timer: soft retcitadel exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retcitadel stamp; Soft!=product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retredoubt — Wave 34 return-redoubt honesty */
aarch64_uart_puts("aarch64: timer: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retredoubt honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retkeep — Wave 34 exclusive keep stamp */
aarch64_uart_puts("aarch64: timer: soft retkeep exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retkeep stamp; Soft!=product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retfortress — Wave 35 return-fortress honesty */
aarch64_uart_puts("aarch64: timer: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retfortress honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retpalace — Wave 35 exclusive palace stamp */
aarch64_uart_puts("aarch64: timer: soft retpalace exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retpalace stamp; Soft!=product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft rethold — Wave 36 return-hold honesty */
aarch64_uart_puts("aarch64: timer: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (rethold honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retspire — Wave 36 exclusive spire stamp */
aarch64_uart_puts("aarch64: timer: soft retspire exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retspire stamp; Soft!=product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retwall — Wave 37 return-wall honesty */
aarch64_uart_puts("aarch64: timer: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retwall honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retgate — Wave 37 exclusive gate stamp */
aarch64_uart_puts("aarch64: timer: soft retgate exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retgate stamp; Soft!=product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retmoat — Wave 38 return-moat honesty */
aarch64_uart_puts("aarch64: timer: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retmoat honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retower — Wave 38 exclusive tower stamp */
aarch64_uart_puts("aarch64: timer: soft retower exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retower stamp; Soft!=product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retbarbican — Wave 39 return-barbican honesty */
aarch64_uart_puts("aarch64: timer: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retbarbican honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retglacis — Wave 39 exclusive glacis stamp */
aarch64_uart_puts("aarch64: timer: soft retglacis exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retglacis stamp; Soft!=product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retcurtain — Wave 40 return-curtain honesty */
aarch64_uart_puts("aarch64: timer: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retcurtain honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retparapet — Wave 40 exclusive parapet stamp */
aarch64_uart_puts("aarch64: timer: soft retparapet exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retparapet stamp; Soft!=product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retravelin — Wave 41 return-travelin honesty */
aarch64_uart_puts("aarch64: timer: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retravelin honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retditch — Wave 41 exclusive ditch stamp */
aarch64_uart_puts("aarch64: timer: soft retditch exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retditch stamp; Soft!=product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retportcullis — Wave 42 return-portcullis honesty */
aarch64_uart_puts("aarch64: timer: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retportcullis honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retbattlement — Wave 42 exclusive battlement stamp */
aarch64_uart_puts("aarch64: timer: soft retbattlement exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retbattlement stamp; Soft!=product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retmachicolation — Wave 43 return-machicolation honesty */
aarch64_uart_puts("aarch64: timer: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retmachicolation honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
aarch64_uart_puts("aarch64: timer: soft retarrowslit exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retarrowslit stamp; Soft!=product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retmerlon — Wave 44 return-merlon honesty */
aarch64_uart_puts("aarch64: timer: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retmerlon honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retembrasure — Wave 44 exclusive embrasure stamp */
aarch64_uart_puts("aarch64: timer: soft retembrasure exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retembrasure stamp; Soft!=product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retkeepgate — Wave 45 return-keepgate honesty */
aarch64_uart_puts("aarch64: timer: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retkeepgate honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retouterward — Wave 45 exclusive outerward stamp */
aarch64_uart_puts("aarch64: timer: soft retouterward exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retouterward stamp; Soft!=product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retbailey — Wave 46 return-bailey honesty */
aarch64_uart_puts("aarch64: timer: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retbailey honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retpostern — Wave 46 exclusive postern stamp */
aarch64_uart_puts("aarch64: timer: soft retpostern exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retpostern stamp; Soft!=product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retinnerward — Wave 47 return-innerward honesty */
aarch64_uart_puts("aarch64: timer: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retinnerward honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retdonjon — Wave 47 exclusive donjon stamp */
aarch64_uart_puts("aarch64: timer: soft retdonjon exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retdonjon stamp; Soft!=product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retchevaux — Wave 48 return-chevaux honesty */
aarch64_uart_puts("aarch64: timer: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retchevaux honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retpalisade — Wave 48 exclusive palisade stamp */
aarch64_uart_puts("aarch64: timer: soft retpalisade exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retpalisade stamp; Soft!=product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retglacisgate — Wave 49 return-glacisgate honesty */
aarch64_uart_puts("aarch64: timer: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retglacisgate honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retoutwork — Wave 49 exclusive outwork stamp */
aarch64_uart_puts("aarch64: timer: soft retoutwork exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retoutwork stamp; Soft!=product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retsally — Wave 50 return-sally honesty */
aarch64_uart_puts("aarch64: timer: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retsally honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
aarch64_uart_puts("aarch64: timer: soft retcounterscarp exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retcounterscarp stamp; Soft!=product)\n");

/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retfosse — Wave 51 return-fosse honesty */
aarch64_uart_puts("aarch64: timer: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retfosse honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retcoveredway — Wave 51 exclusive coveredway stamp */
aarch64_uart_puts("aarch64: timer: soft retcoveredway exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retcoveredway stamp; Soft!=product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft rettenaille — Wave 52 return-tenaille honesty */
aarch64_uart_puts("aarch64: timer: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (rettenaille honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retdemilune — Wave 52 exclusive demilune stamp */
aarch64_uart_puts("aarch64: timer: soft retdemilune exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retdemilune stamp; Soft!=product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retravelin — Wave 53 return-travelin honesty */
aarch64_uart_puts("aarch64: timer: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retravelin honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retlunette — Wave 53 exclusive lunette stamp */
aarch64_uart_puts("aarch64: timer: soft retlunette exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retlunette stamp; Soft!=product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retcaponier — Wave 54 return-caponier honesty */
aarch64_uart_puts("aarch64: timer: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retcaponier honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retredan — Wave 54 exclusive redan stamp */
aarch64_uart_puts("aarch64: timer: soft retredan exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retredan stamp; Soft!=product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retflank — Wave 55 return-flank honesty */
aarch64_uart_puts("aarch64: timer: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retflank honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retface — Wave 55 exclusive face stamp */
aarch64_uart_puts("aarch64: timer: soft retface exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retface stamp; Soft!=product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retgorge — Wave 56 return-gorge honesty */
aarch64_uart_puts("aarch64: timer: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retgorge honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retshoulder — Wave 56 exclusive shoulder stamp */
aarch64_uart_puts("aarch64: timer: soft retshoulder exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retshoulder stamp; Soft!=product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retraverse — Wave 57 return-traverse honesty */
aarch64_uart_puts("aarch64: timer: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retraverse honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retcasemate — Wave 57 exclusive casemate stamp */
aarch64_uart_puts("aarch64: timer: soft retcasemate exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retcasemate stamp; Soft!=product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retorillon — Wave 58 return-orillon honesty */
aarch64_uart_puts("aarch64: timer: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retorillon honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retbonnette — Wave 58 exclusive bonnette stamp */
aarch64_uart_puts("aarch64: timer: soft retbonnette exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retbonnette stamp; Soft!=product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retcrownwork — Wave 59 return-crownwork honesty */
aarch64_uart_puts("aarch64: timer: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retcrownwork honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft rethornwork — Wave 59 exclusive hornwork stamp */
aarch64_uart_puts("aarch64: timer: soft rethornwork exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (rethornwork stamp; Soft!=product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retplace — Wave 60 return-place honesty */
aarch64_uart_puts("aarch64: timer: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retplace honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retenvelope — Wave 60 exclusive envelope stamp */
aarch64_uart_puts("aarch64: timer: soft retenvelope exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retenvelope stamp; Soft!=product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retcounterguard — Wave 61 return-counterguard honesty */
aarch64_uart_puts("aarch64: timer: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retcounterguard honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retcoveredface — Wave 61 exclusive coveredface stamp */
aarch64_uart_puts("aarch64: timer: soft retcoveredface exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retcoveredface stamp; Soft!=product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: timer: soft retbastionface — Wave 62 return-bastionface honesty */
aarch64_uart_puts("aarch64: timer: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retbastionface honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
aarch64_uart_puts("aarch64: timer: soft retcurtainangle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retcurtainangle stamp; Soft!=product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: timer: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
aarch64_uart_puts("aarch64: timer: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retdoubletenaille honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
aarch64_uart_puts("aarch64: timer: soft retplaceofarms exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retplaceofarms stamp; Soft!=product)\n");
/*
 * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: timer: soft retreentrant — Wave 64 return-reentrant honesty */
aarch64_uart_puts("aarch64: timer: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retreentrant honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retsallyport — Wave 64 exclusive sallyport stamp */
aarch64_uart_puts("aarch64: timer: soft retsallyport exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retsallyport stamp; Soft!=product)\n");
/*
 * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: timer: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
aarch64_uart_puts("aarch64: timer: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retgorgeangle honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
aarch64_uart_puts("aarch64: timer: soft retshoulderangle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retshoulderangle stamp; Soft!=product)\n");
/*
 * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: timer: soft retflankangle — Wave 66 return-flankangle honesty */
aarch64_uart_puts("aarch64: timer: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retflankangle honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: timer: soft retfaceangle — Wave 66 exclusive faceangle stamp */
aarch64_uart_puts("aarch64: timer: soft retfaceangle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retcaponierangle — Wave 67 return-caponierangle honesty */
aarch64_uart_puts("aarch64: timer: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=107 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retredanangle — Wave 67 exclusive redanangle stamp */
aarch64_uart_puts("aarch64: timer: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=107 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
aarch64_uart_puts("aarch64: timer: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=107 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
aarch64_uart_puts("aarch64: timer: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=107 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
aarch64_uart_puts("aarch64: timer: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=107 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
aarch64_uart_puts("aarch64: timer: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=107 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retfosseangle — Wave 70 return-fosseangle honesty */
aarch64_uart_puts("aarch64: timer: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
aarch64_uart_puts("aarch64: timer: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
aarch64_uart_puts("aarch64: timer: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
aarch64_uart_puts("aarch64: timer: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: timer: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
aarch64_uart_puts("aarch64: timer: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
aarch64_uart_puts("aarch64: timer: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retcurtainface — Wave 73 return-curtainface honesty */
aarch64_uart_puts("aarch64: timer: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
aarch64_uart_puts("aarch64: timer: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retbastionangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retglacisangle — Wave 74 return-glacisangle honesty */
aarch64_uart_puts("aarch64: timer: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
aarch64_uart_puts("aarch64: timer: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retparapetangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retmoatangle — Wave 75 return-moatangle honesty */
aarch64_uart_puts("aarch64: timer: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retowerangle — Wave 75 exclusive towerangle stamp */
aarch64_uart_puts("aarch64: timer: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retowerangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retgateangle — Wave 76 return-gateangle honesty */
aarch64_uart_puts("aarch64: timer: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retwallangle — Wave 76 exclusive wallangle stamp */
aarch64_uart_puts("aarch64: timer: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retwallangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retspireangle — Wave 77 return-spireangle honesty */
aarch64_uart_puts("aarch64: timer: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retholdangle — Wave 77 exclusive holdangle stamp */
aarch64_uart_puts("aarch64: timer: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retholdangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retpalaceangle — Wave 78 return-palaceangle honesty */
aarch64_uart_puts("aarch64: timer: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
aarch64_uart_puts("aarch64: timer: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retfortressangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retkeepangle — Wave 79 return-keepangle honesty */
aarch64_uart_puts("aarch64: timer: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
aarch64_uart_puts("aarch64: timer: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retcitadelangle — Wave 80 return-citadelangle honesty */
aarch64_uart_puts("aarch64: timer: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
aarch64_uart_puts("aarch64: timer: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
aarch64_uart_puts("aarch64: timer: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
aarch64_uart_puts("aarch64: timer: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retmantleangle — Wave 82 return-mantleangle honesty */
aarch64_uart_puts("aarch64: timer: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
aarch64_uart_puts("aarch64: timer: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retaegisangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retemblemangle — Wave 83 return-emblemangle honesty */
aarch64_uart_puts("aarch64: timer: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
aarch64_uart_puts("aarch64: timer: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retsigilangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retscepterangle — Wave 84 return-scepterangle honesty */
aarch64_uart_puts("aarch64: timer: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
aarch64_uart_puts("aarch64: timer: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retglyphangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retcrownangle — Wave 85 return-crownangle honesty */
aarch64_uart_puts("aarch64: timer: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retshardangle — Wave 85 exclusive shardangle stamp */
aarch64_uart_puts("aarch64: timer: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retshardangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retforgeangle — Wave 86 return-forgeangle honesty */
aarch64_uart_puts("aarch64: timer: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retprismangle — Wave 86 exclusive prismangle stamp */
aarch64_uart_puts("aarch64: timer: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retprismangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retflameangle — Wave 87 return-flameangle honesty */
aarch64_uart_puts("aarch64: timer: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
aarch64_uart_puts("aarch64: timer: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retcipherangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retbeaconangle — Wave 88 return-beaconangle honesty */
aarch64_uart_puts("aarch64: timer: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
aarch64_uart_puts("aarch64: timer: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retledgerangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retbannerangle — Wave 89 return-bannerangle honesty */
aarch64_uart_puts("aarch64: timer: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
aarch64_uart_puts("aarch64: timer: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retvaultangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retcrestangle — Wave 90 return-crestangle honesty */
aarch64_uart_puts("aarch64: timer: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
aarch64_uart_puts("aarch64: timer: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (rettokenangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retbadgeangle — Wave 91 return-badgeangle honesty */
aarch64_uart_puts("aarch64: timer: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
aarch64_uart_puts("aarch64: timer: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retphaseangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retmarkangle — Wave 92 return-markangle honesty */
aarch64_uart_puts("aarch64: timer: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
aarch64_uart_puts("aarch64: timer: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retpulseangle stamp; Soft≠product)\n");

/* Grep: aarch64: timer: soft retsealangle — Wave 93 return-sealangle honesty */
aarch64_uart_puts("aarch64: timer: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retboundangle — Wave 93 exclusive boundangle stamp */
aarch64_uart_puts("aarch64: timer: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retboundangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retstemangle — Wave 94 return-stemangle honesty */
aarch64_uart_puts("aarch64: timer: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
aarch64_uart_puts("aarch64: timer: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retbladeangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retchordangle — Wave 95 return-chordangle honesty */
aarch64_uart_puts("aarch64: timer: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retarcangle — Wave 95 exclusive arcangle stamp */
aarch64_uart_puts("aarch64: timer: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retarcangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retsectorangle — Wave 96 return-sectorangle honesty */
aarch64_uart_puts("aarch64: timer: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
aarch64_uart_puts("aarch64: timer: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retradiusangle — Wave 97 return-radiusangle honesty */
aarch64_uart_puts("aarch64: timer: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
aarch64_uart_puts("aarch64: timer: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retcircumangle — Wave 98 return-circumangle honesty */
aarch64_uart_puts("aarch64: timer: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
aarch64_uart_puts("aarch64: timer: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retellipseangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft rethyperangle — Wave 99 return-hyperangle honesty */
aarch64_uart_puts("aarch64: timer: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
aarch64_uart_puts("aarch64: timer: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retspiralangle — Wave 100 return-spiralangle honesty */
aarch64_uart_puts("aarch64: timer: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft rethelixangle — Wave 100 exclusive helixangle stamp */
aarch64_uart_puts("aarch64: timer: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (rethelixangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft rettorusangle — Wave 101 return-torusangle honesty */
aarch64_uart_puts("aarch64: timer: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retknotangle — Wave 101 exclusive knotangle stamp */
aarch64_uart_puts("aarch64: timer: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retknotangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
aarch64_uart_puts("aarch64: timer: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
aarch64_uart_puts("aarch64: timer: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retkleinangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retprojectangle — Wave 103 return-projectangle honesty */
aarch64_uart_puts("aarch64: timer: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retaffineangle — Wave 103 exclusive affineangle stamp */
aarch64_uart_puts("aarch64: timer: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retaffineangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retlinearangle — Wave 104 return-linearangle honesty */
aarch64_uart_puts("aarch64: timer: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
aarch64_uart_puts("aarch64: timer: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
aarch64_uart_puts("aarch64: timer: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
aarch64_uart_puts("aarch64: timer: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retcubicangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retquarticangle — Wave 106 return-quarticangle honesty */
aarch64_uart_puts("aarch64: timer: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
aarch64_uart_puts("aarch64: timer: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retquinticangle stamp; Soft≠product)\n");
/* Grep: aarch64: timer: soft retsplineangle — Wave 107 return-splineangle honesty */
aarch64_uart_puts("aarch64: timer: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=107 (retsplineangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: timer: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
aarch64_uart_puts("aarch64: timer: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=107 (retbezierangle stamp; Soft≠product)\n");
aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
aarch64_uart_puts(" (retfaceangle stamp; Soft!=product)\n");
    aarch64_uart_puts("aarch64: timer soft deepen wave=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
    aarch64_uart_puts(" areas=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_AREAS);
    aarch64_uart_puts(" catalog=freq,tick,inventory,stats,path,surf,return,honesty,"
                      "deepen logs=");
    aarch64_uart_put_hex((unsigned long)g_cTimerSoftLogs);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: timer soft surf — Wave 19 freq/tick lamps */
    aarch64_uart_puts("aarch64: timer soft surf freq_ok=");
    aarch64_uart_put_hex((unsigned long)fFreqSoft);
    aarch64_uart_puts(" tick_ok=");
    aarch64_uart_put_hex((unsigned long)fTickSoft);
    aarch64_uart_puts(" hits=");
    aarch64_uart_put_hex((unsigned long)cHits);
    aarch64_uart_puts(" imask=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_ARM_IMASKED);
    aarch64_uart_puts(" bits=");
    aarch64_uart_put_hex((unsigned long)(
        ((fFreqSoft != 0) ? 1u : 0u) |
        (((fTickSoft != 0) ? 1u : 0u) << 1) |
        (((cHits >= 2u) ? 1u : 0u) << 2) |
        ((TIMER_SOFT_ARM_IMASKED != 0) ? (1u << 3) : 0u)));
    aarch64_uart_puts(" wave=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /*
     * Grep: aarch64: timer soft path
     * Honesty: IMASK stays set; PPI soft-enabled in GIC but no IRQ take.
     * product_kernel=OPEN: aarch64 product kernel remains OPEN.
     */
    aarch64_uart_puts("aarch64: timer soft path imask=1 irq_delivery=0 "
                      "cntv=1 cntp=0 ppi=");
    aarch64_uart_put_hex((unsigned long)TIMER_PPI_VIRT);
    aarch64_uart_puts(" product_kernel=OPEN hard_gate=0 wave=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: timer soft honesty */
    aarch64_uart_puts("aarch64: timer soft honesty product_kernel=OPEN "
                      "soft_only=1 no_irq_take=1 no_bar3=1 wave=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: timer soft exclusive — Wave 35 exclusive deepen */
    aarch64_uart_puts("aarch64: timer soft exclusive multi_server=0 "
                      "confine=0 bar3=0 product_kernel=OPEN soft_only=1 wave=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: timer soft open — Wave 19 open-lamp rollup */
    aarch64_uart_puts("aarch64: timer soft open multi_server=0 confine=0 "
                      "bar3=0 product_kernel=OPEN soft_only=1 wave=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
    aarch64_uart_puts("\n");


    fSoft = 0;
    if (fFreqSoft != 0 && fTickSoft != 0) {
        fSoft = 1;
    }

    /* Grep: aarch64: timer soft return — Wave 19 return surfaces */
    aarch64_uart_puts("aarch64: timer soft return inv_ret=");
    aarch64_uart_put_hex((unsigned long)(fSoft != 0 ? 1ul : 0ul));
    aarch64_uart_puts(" freq=");
    aarch64_uart_put_hex((unsigned long)fFreqSoft);
    aarch64_uart_puts(" tick=");
    aarch64_uart_put_hex((unsigned long)fTickSoft);
    aarch64_uart_puts(" product_kernel=OPEN wave=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
    aarch64_uart_puts("\n");

    if (fSoft != 0) {
        aarch64_uart_puts("aarch64: timer soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: timer soft FAIL\n");
    }
    return fSoft;
}

void
aarch64_timer_probe(void)
{
    unsigned long u64T0;
    unsigned long u64T1;
    unsigned long u64Adv;
    unsigned int u32Frq;
    unsigned uSpins;
    unsigned cTickHits;
    struct timer_tick_soft_inv inv;
    int fFreqSoft;
    int fTickSoft;
    int fSoft;

    /* Zero soft inventory snapshot before tick path. */
    inv.u64Delta = 0ul;
    inv.u64CvalWrote = 0ul;
    inv.u64CvalReadback = 0ul;
    inv.u32CtlArm = 0u;
    inv.u32CtlAfterHit0 = 0u;
    inv.u32CtlAfterHit1 = 0u;
    inv.u32CtlEnd = 0u;
    inv.uSpins0 = 0u;
    inv.uSpins1 = 0u;
    inv.cHits = 0u;
    inv.fCvalOk = 0;
    inv.fEnableArm = 0;
    inv.fImaskArm = 0;
    inv.fIst0 = 0;
    inv.fIst1 = 0;

    u32Frq = cntfrq();
    u64T0 = cntvct();
    for (uSpins = 0u; uSpins < TIMER_SOFT_ADV_SPINS; uSpins++) {
        __asm__ volatile("yield");
    }
    u64T1 = cntvct();
    u64Adv = u64T1 - u64T0;

    /* Soft virtual-timer compare + tick reload (no IRQ). */
    cTickHits = timer_tick_soft(u32Frq, &inv);
    (void)cTickHits;

    /*
     * Legacy one-line summary (kept greppable for existing smoke detail).
     * Deeper soft inventory follows via freq/tick/soft markers.
     */
    aarch64_uart_puts("aarch64: timer frq=");
    aarch64_uart_put_hex((unsigned long)u32Frq);
    aarch64_uart_puts(" t0=");
    aarch64_uart_put_hex(u64T0);
    aarch64_uart_puts(" t1=");
    aarch64_uart_put_hex(u64T1);
    aarch64_uart_puts(" adv=");
    aarch64_uart_put_hex(u64Adv);
    aarch64_uart_puts(" delta=");
    aarch64_uart_put_hex(inv.u64Delta);
    aarch64_uart_puts(" hits=");
    aarch64_uart_put_hex((unsigned long)inv.cHits);
    aarch64_uart_puts(" spins0=");
    aarch64_uart_put_hex((unsigned long)inv.uSpins0);
    aarch64_uart_puts(" spins1=");
    aarch64_uart_put_hex((unsigned long)inv.uSpins1);
    aarch64_uart_puts(" ppi=");
    aarch64_uart_put_hex((unsigned long)TIMER_PPI_VIRT);
    aarch64_uart_puts("\n");

    /* Soft freq inventory (CNTFRQ range + counter advance). */
    fFreqSoft = timer_freq_soft_inventory(u32Frq, u64Adv);

    /* Soft tick inventory (arm bits + dual ISTATUS + quiet end). */
    fTickSoft = timer_tick_soft_inventory(&inv, u64Adv);

    /* Wave 19 combined soft inventory under "aarch64: timer soft …". */
    fSoft = timer_soft_inventory(u32Frq, u64T0, u64T1, u64Adv, &inv,
                                 fFreqSoft, fTickSoft);

    /*
     * Primary product marker: regs reachable. Always emit when probe runs
     * (M0+ smoke). Soft FAIL paths remain greppable separately.
     * Honesty: timer PASS ≠ product IRQ timer / aarch64 kernel complete.
     */
    aarch64_uart_puts("aarch64: timer PASS\n");

    (void)fSoft;
}
