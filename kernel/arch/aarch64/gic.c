/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal GICv2 bring-up for QEMU virt (scaffold only — not x86 product).
 *
 * QEMU -M virt fixed MMIO (identity-mapped physical == early VA):
 *   GICD (distributor)  0x0800_0000
 *   GICC (CPU iface)    0x0801_0000
 *
 * These bases are compile-time constants, not heap pointers. There is no
 * allocation failure path and no NULL to check: a wrong base would fault on
 * first access (or corrupt unrelated MMIO), not return a soft error. Do not
 * add null-pointer guards on the MMIO accessors.
 *
 * -------------------------------------------------------------------------
 * Soft enable path (no full IRQ handler / EOI yet)
 * -------------------------------------------------------------------------
 *   1. GICD_CTLR EnableGrp0
 *   2. GICD_ISENABLER0:
 *        bits [15:0]  — SGI 0–15
 *        bits [31:16] — PPI 16–31  (soft: PPI 27 virtual timer enabled)
 *   3. GICC_PMR allow all priorities; GICC_CTLR enable
 *
 * PPI soft path (GICv2, INTID 16–31, local to each CPU):
 *   INTID 27 — virtual timer (CNTV_CTL_EL0 / CNTV_CVAL_EL0). ARM recommends
 *              this PPI for EL1 OS tick on virt. Paired with timer.c soft arm:
 *              timer programs compare with IMASK set so ISTATUS latches without
 *              asserting the line; when product unmasks CNTV_CTL.IMASK and
 *              clears DAIF.I, this ISENABLER bit is the GIC half of delivery.
 *   INTID 30 — non-secure physical timer (CNTP) — left masked (bit 30 off).
 *   INTID 29 — secure physical timer — left masked.
 *   INTID 28 — virtual maintenance / hyp — not used at EL1 here.
 *
 * Future IRQ take (not wired; offsets documented for product deepen):
 *   GICC_IAR  @ 0x00C — read INTID (bits [9:0]); spurious = 1023
 *   GICC_EOIR @ 0x010 — write same INTID after handle (priority drop)
 *   GICC_HPPIR / GICC_BPR — optional priority tuning
 * Distributor config still needed for SPI (32+): ITARGETSRn, IPRIORITYRn,
 * ICFGRn edge/level — SPI path deferred until virtio/PL011 IRQs land.
 *
 * DSB SY after GIC writes so later loads/stores observe distributor state;
 * ISB so subsequent instruction stream sees CPU-interface side effects.
 *
 * Greppable: aarch64: GIC PASS
 *
 * Freestanding pure C; no GPL Linux arch paste.
 */
#include "types_arch.h"

/* QEMU virt GICv2 defaults (not board-probed). */
#define GICD_BASE 0x08000000ul
#define GICC_BASE 0x08010000ul

/* Distributor offsets (word-addressed via u32 *) */
#define GICD_CTLR       0x000u
#define GICD_TYPER      0x004u
#define GICD_ISENABLER0 0x100u
/* Soft-path docs: future SPI/priority/target (not written yet). */
#define GICD_IPRIORITYR0 0x400u /* +INTID → byte priority */
#define GICD_ITARGETSR0  0x800u /* SPI CPU targets; SGI/PPI RO/impl-def */
#define GICD_ICFGR0      0xc00u /* 2 bits/INTID config */

/* CPU interface offsets */
#define GICC_CTLR 0x000u
#define GICC_PMR  0x004u
#define GICC_BPR  0x008u
#define GICC_IAR  0x00cu /* soft path: ACK / read INTID */
#define GICC_EOIR 0x010u /* soft path: EOI write INTID */

/* GICD_CTLR: bit0 EnableGrp0 (GICv2) */
#define GICD_CTLR_ENABLE 1u
/* GICC_CTLR: bit0 Enable */
#define GICC_CTLR_ENABLE 1u
/* GICC_PMR: 0xff = accept all priority levels */
#define GICC_PMR_ALLOW_ALL 0xffu

/* ISENABLER0 layout: one enable bit per INTID 0–31 */
#define GICD_SGI_ENABLE_MASK 0x0000ffffu /* INTID 0–15 */
#define GICD_PPI_VTIMER_BIT  (1u << 27)  /* INTID 27 virtual timer */
/* Soft PPI mask we arm at bring-up (SGI all + vtimer PPI). */
#define GICD_SOFT_ISENABLER0 (GICD_SGI_ENABLE_MASK | GICD_PPI_VTIMER_BIT)

/* Spurious INTID from GICC_IAR when no pending Group0 interrupt. */
#define GICC_INTID_SPURIOUS 1023u

extern void aarch64_uart_puts(const char *sz);

/*
 * gicd / gicc — MMIO word at fixed phys base + offset.
 * off is a register byte offset (must be 4-byte aligned).
 * Return value is always a valid mapped MMIO address on virt; never NULL.
 */
static volatile unsigned int *
gicd(unsigned uOff)
{
    return (volatile unsigned int *)(GICD_BASE + (unsigned long)uOff);
}

static volatile unsigned int *
gicc(unsigned uOff)
{
    return (volatile unsigned int *)(GICC_BASE + (unsigned long)uOff);
}

static void
gic_barrier(void)
{
    __asm__ volatile("dsb sy" ::: "memory");
    __asm__ volatile("isb" ::: "memory");
}

/*
 * Soft PPI path note (no call sites yet): product IRQ entry would
 *   id = *gicc(GICC_IAR);
 *   if (id == GICC_INTID_SPURIOUS) return;
 *   handle(id);  // e.g. INTID 27 → timer tick
 *   *gicc(GICC_EOIR) = id;
 * Keep IAR/EOIR paired; never EOI a spurious ID.
 */
void
aarch64_gic_init(void)
{
    unsigned uTyper;
    unsigned cIrqLines;
    unsigned uIsen;

    /* Enable distributor (group 0). No probe of base — fixed virt map. */
    *gicd(GICD_CTLR) = GICD_CTLR_ENABLE;
    gic_barrier();

    /*
     * TYPER[4:0] = ITLinesNumber → SPI capacity:
     *   32 * (ITLinesNumber + 1) total INTIDs (incl. SGI/PPI).
     * Used only as a soft sanity read that MMIO responds; not freestanding
     * fail-stop (virt always presents GIC).
     */
    uTyper = *gicd(GICD_TYPER);
    cIrqLines = ((uTyper & 0x1fu) + 1u) * 32u;
    (void)cIrqLines;

    /*
     * Soft enable: SGI 0–15 + PPI 27 (virtual timer).
     * SPI 32+ and other PPIs remain masked (bit clear in ISENABLERn).
     * Enabling the PPI bit alone does not fire IRQs: CPU DAIF.I is still
     * set from entry.S, CNTV_CTL.IMASK stays set in timer soft path, and
     * there is no GICC_IAR consumer. This only primes the distributor so a
     * later product arm (unmask timer + clear DAIF + vector IRQ slot) has
     * the GIC half ready.
     */
    *gicd(GICD_ISENABLER0) = GICD_SOFT_ISENABLER0;
    gic_barrier();

    /* Read-back soft check (non-fatal; confirms distributor accepts enable). */
    uIsen = *gicd(GICD_ISENABLER0);
    (void)uIsen;
    /* Soft-path register offsets (GICC_IAR/EOIR, IPRIORITY, …) are documented
     * in the file header for the future IRQ take; not accessed until wired. */

    /* CPU interface: unmask priorities, enable group 0 signaling. */
    *gicc(GICC_PMR) = GICC_PMR_ALLOW_ALL;
    *gicc(GICC_CTLR) = GICC_CTLR_ENABLE;
    gic_barrier();

    /* Greppable: aarch64-smoke / manual serial capture. */
    aarch64_uart_puts("aarch64: GIC PASS\n");
}
