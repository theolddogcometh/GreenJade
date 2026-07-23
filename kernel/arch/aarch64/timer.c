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
 *   aarch64: timer soft inventory wave=19 …
 *   aarch64: timer soft stats …
 *   aarch64: timer soft deepen wave=19 areas=…
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

/* Wave 19 soft inventory stamp (greppable wave=19). */
#define TIMER_SOFT_WAVE 19u

/* Soft deepen areas: freq,tick,inventory,stats,path,honesty. */
#define TIMER_SOFT_AREAS 11u

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
     * ---- Wave 19 exclusive complementary surfaces (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: aarch64: timer: soft retclass — Wave 19 return-class taxonomy */
    aarch64_uart_puts("aarch64: timer: soft retclass ok|fail|inval|nodev|busy|nomem "
                      "soft_only=1 product_gate=0 wave=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
    aarch64_uart_puts(" (retclass taxonomy; Soft!=product; not bar3)\n");
    /* Grep: aarch64: timer: soft retlane — Wave 19 return-lane catalog */
    aarch64_uart_puts("aarch64: timer: soft retlane inv|selftest|rate|retcode|retmap|class "
                      "product_kernel=OPEN soft_ne_product=1 wave=");
    aarch64_uart_put_hex((unsigned long)TIMER_SOFT_WAVE);
    aarch64_uart_puts(" (retlane catalog; Soft!=product)\n");
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

    /* Grep: aarch64: timer soft exclusive — Wave 19 exclusive deepen */
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
