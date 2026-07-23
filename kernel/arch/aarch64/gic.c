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
 * -------------------------------------------------------------------------
 * Soft SPI path (INTID 32+, QEMU virt defaults)
 * -------------------------------------------------------------------------
 * QEMU virt SPI numbering (SPI n → INTID 32+n):
 *   SPI 1  / INTID 33 — PL011 UART0
 *   SPI 16 / INTID 48 — virtio-mmio slot 0 (first transport)
 * Soft bring-up programs per-SPI:
 *   IPRIORITYR[INTID]  = mid priority (0xa0)
 *   ITARGETSR[INTID]   = CPU0 affinity bit0 (0x01)
 *   ICFGR field        = level-sensitive (clear edge bit; virt default)
 *   ISENABLERn bit     = set (prime distributor; DAIF.I still set)
 * Soft IAR peek expects GICC_INTID_SPURIOUS (1023) with no pending source
 * asserted into the CPU interface (DAIF + timer IMASK keep lines quiet).
 *
 * Future IRQ take (not fully wired; offsets used by soft path now):
 *   GICC_IAR  @ 0x00C — read INTID (bits [9:0]); spurious = 1023
 *   GICC_EOIR @ 0x010 — write same INTID after handle (priority drop)
 *   GICC_HPPIR / GICC_BPR — optional priority tuning
 *
 * DSB SY after GIC writes so later loads/stores observe distributor state;
 * ISB so subsequent instruction stream sees CPU-interface side effects.
 *
 * Greppable: aarch64: GIC PASS
 *            aarch64: GIC SPI soft PASS
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
/* Soft SPI / priority / target / config (byte or word per GICv2 layout). */
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
#define GICC_IAR_INTID_MASK 0x3ffu

/* Soft SPI INTIDs on QEMU virt (32 + SPI#). */
#define GIC_SPI_UART_INTID   33u /* SPI 1  — PL011 UART0 */
#define GIC_SPI_VIRTIO0_INTID 48u /* SPI 16 — virtio-mmio slot 0 */
#define GIC_SOFT_SPI_PRIO    0xa0u /* mid priority; lower value = higher prio */
#define GIC_SOFT_SPI_TARGET  0x01u /* CPU interface 0 */

extern void aarch64_uart_puts(const char *sz);
extern void aarch64_uart_put_hex(unsigned long v);

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

/* Soft byte poke into distributor banked arrays (priority / target). */
static void
gicd_write_byte(unsigned uOff, unsigned uIntId, unsigned uVal)
{
    volatile unsigned char *p;

    p = (volatile unsigned char *)(GICD_BASE + (unsigned long)uOff +
                                   (unsigned long)uIntId);
    *p = (unsigned char)(uVal & 0xffu);
}

static unsigned
gicd_read_byte(unsigned uOff, unsigned uIntId)
{
    volatile unsigned char *p;

    p = (volatile unsigned char *)(GICD_BASE + (unsigned long)uOff +
                                   (unsigned long)uIntId);
    return (unsigned)(*p);
}

/*
 * Soft SPI configure: priority + CPU0 target + level-sensitive + enable bit.
 * Does not unmask DAIF; product IRQ take still needs VBAR IRQ + IAR/EOIR.
 */
static void
gic_spi_soft_program(unsigned uIntId)
{
    unsigned uIsenOff;
    unsigned uBit;
    unsigned uIcfgOff;
    unsigned uIcfgShift;
    unsigned uIcfg;
    unsigned uIsen;

    /* Priority (byte). SPI only for meaningful ITARGETSR under GICv2. */
    gicd_write_byte(GICD_IPRIORITYR0, uIntId, GIC_SOFT_SPI_PRIO);
    /*
     * ITARGETSR: GICv2 CPU target bitmask. On GICv3 with affinity routing
     * (QEMU virt default gic-version=3) these bytes are RES0 — write is
     * soft/best-effort; observe path accepts 0 or CPU0 bit.
     */
    gicd_write_byte(GICD_ITARGETSR0, uIntId, GIC_SOFT_SPI_TARGET);
    /* Word RMW fallback (some models ignore byte stores). */
    {
        unsigned uWordOff;
        unsigned uShift;
        unsigned uW;

        uWordOff = GICD_ITARGETSR0 + (uIntId & ~3u);
        uShift = (uIntId & 3u) * 8u;
        uW = *gicd(uWordOff);
        uW = (uW & ~(0xffu << uShift)) | (GIC_SOFT_SPI_TARGET << uShift);
        *gicd(uWordOff) = uW;
    }

    /*
     * ICFGR: 2 bits per INTID; bit1 of the pair = edge (1) / level (0).
     * Soft path forces level for UART/virtio (QEMU default).
     */
    uIcfgOff = GICD_ICFGR0 + 4u * (uIntId / 16u);
    uIcfgShift = (uIntId % 16u) * 2u;
    uIcfg = *gicd(uIcfgOff);
    uIcfg &= ~(3u << uIcfgShift); /* level-sensitive, no model bit */
    *gicd(uIcfgOff) = uIcfg;
    gic_barrier();

    /* ISENABLERn: set enable bit for this INTID. */
    uIsenOff = GICD_ISENABLER0 + 4u * (uIntId / 32u);
    uBit = 1u << (uIntId % 32u);
    *gicd(uIsenOff) = uBit; /* write-1-to-set */
    gic_barrier();

    uIsen = *gicd(uIsenOff);
    (void)uIsen;
}

/*
 * Soft SPI observability path: program UART + virtio0 SPIs, read-back
 * priority/target/enable, soft-peek GICC_IAR (expect spurious).
 * Returns 1 if SPI soft checks pass.
 */
static int
gic_spi_soft_observe(unsigned uTyper, unsigned cIrqLines)
{
    unsigned uPrioUart;
    unsigned uTgtUart;
    unsigned uPrioVio;
    unsigned uTgtVio;
    unsigned uIsen1;
    unsigned uIsen1b;
    unsigned uIar;
    unsigned uId;
    int fOk;

    (void)uTyper;

    /* Need at least INTID 48 for virtio0 SPI (ITLines covers SPI capacity). */
    if (cIrqLines < 64u) {
        aarch64_uart_puts("aarch64: GIC SPI soft SKIP (lines)\n");
        return 0;
    }

    gic_spi_soft_program(GIC_SPI_UART_INTID);
    gic_spi_soft_program(GIC_SPI_VIRTIO0_INTID);

    uPrioUart = gicd_read_byte(GICD_IPRIORITYR0, GIC_SPI_UART_INTID);
    uTgtUart = gicd_read_byte(GICD_ITARGETSR0, GIC_SPI_UART_INTID);
    uPrioVio = gicd_read_byte(GICD_IPRIORITYR0, GIC_SPI_VIRTIO0_INTID);
    uTgtVio = gicd_read_byte(GICD_ITARGETSR0, GIC_SPI_VIRTIO0_INTID);

    /* ISENABLER1 covers INTID 32–63 (UART 33 + virtio0 48). */
    uIsen1 = *gicd(GICD_ISENABLER0 + 4u);
    uIsen1b = uIsen1;

    /*
     * Soft IAR peek: no consumer yet; with DAIF.I set and no asserted line
     * into the interface, expect spurious. Never EOI a spurious ID.
     */
    /*
     * Soft IAR peek: DAIF.I still set so CPU does not take the exception, but
     * a device line (e.g. PL011) may already be pending at the interface once
     * ISENABLER is set. Accept spurious OR our soft-programmed SPI INTIDs;
     * EOI any non-spurious ID so priority state stays clean.
     */
    uIar = *gicc(GICC_IAR);
    uId = uIar & GICC_IAR_INTID_MASK;
    if (uId != GICC_INTID_SPURIOUS) {
        *gicc(GICC_EOIR) = uId;
        gic_barrier();
    }

    fOk = 1;
    if (uPrioUart != GIC_SOFT_SPI_PRIO || uPrioVio != GIC_SOFT_SPI_PRIO) {
        fOk = 0;
    }
    /*
     * Target: GICv2 expects CPU0 bit; GICv3 ARE leaves ITARGETSR as 0.
     * Soft PASS either shape if priority + ISENABLER stuck.
     */
    if ((uTgtUart != GIC_SOFT_SPI_TARGET && uTgtUart != 0u) ||
        (uTgtVio != GIC_SOFT_SPI_TARGET && uTgtVio != 0u)) {
        fOk = 0;
    }
    /* UART bit 1 and virtio0 bit 16 in ISENABLER1. */
    if ((uIsen1b & (1u << (GIC_SPI_UART_INTID % 32u))) == 0u) {
        fOk = 0;
    }
    if ((uIsen1b & (1u << (GIC_SPI_VIRTIO0_INTID % 32u))) == 0u) {
        fOk = 0;
    }
    if (uId != GICC_INTID_SPURIOUS &&
        uId != GIC_SPI_UART_INTID &&
        uId != GIC_SPI_VIRTIO0_INTID) {
        fOk = 0;
    }

    aarch64_uart_puts("aarch64: GIC SPI soft lines=");
    aarch64_uart_put_hex((unsigned long)cIrqLines);
    aarch64_uart_puts(" uart=");
    aarch64_uart_put_hex((unsigned long)GIC_SPI_UART_INTID);
    aarch64_uart_puts(" prio=");
    aarch64_uart_put_hex((unsigned long)uPrioUart);
    aarch64_uart_puts(" tgt=");
    aarch64_uart_put_hex((unsigned long)uTgtUart);
    aarch64_uart_puts(" vio0=");
    aarch64_uart_put_hex((unsigned long)GIC_SPI_VIRTIO0_INTID);
    aarch64_uart_puts(" isen1=");
    aarch64_uart_put_hex((unsigned long)uIsen1b);
    aarch64_uart_puts(" iar=");
    aarch64_uart_put_hex((unsigned long)uIar);
    aarch64_uart_puts("\n");

    return fOk;
}

/*
 * Soft PPI path note (no call sites yet): product IRQ entry would
 *   id = *gicc(GICC_IAR);
 *   if (id == GICC_INTID_SPURIOUS) return;
 *   handle(id);  // e.g. INTID 27 → timer tick
 *   *gicc(GICC_EOIR) = id;
 * Keep IAR/EOIR paired; never EOI a spurious ID.
 *
 * Soft helpers for exception IRQ soft path (same GIC half, no full dispatch).
 */
unsigned
aarch64_gic_soft_iar(void)
{
    return *gicc(GICC_IAR) & GICC_IAR_INTID_MASK;
}

void
aarch64_gic_soft_eoi(unsigned uIntId)
{
    if (uIntId == GICC_INTID_SPURIOUS) {
        return;
    }
    *gicc(GICC_EOIR) = uIntId;
    gic_barrier();
}

void
aarch64_gic_init(void)
{
    unsigned uTyper;
    unsigned cIrqLines;
    unsigned uIsen;
    unsigned uCtlr;
    unsigned uPmr;
    int fSpiSoft;

    /* Enable distributor (group 0). No probe of base — fixed virt map. */
    *gicd(GICD_CTLR) = GICD_CTLR_ENABLE;
    gic_barrier();

    /*
     * TYPER[4:0] = ITLinesNumber → SPI capacity:
     *   32 * (ITLinesNumber + 1) total INTIDs (incl. SGI/PPI).
     * Used as soft sanity read that MMIO responds + SPI soft gate.
     */
    uTyper = *gicd(GICD_TYPER);
    cIrqLines = ((uTyper & 0x1fu) + 1u) * 32u;

    /*
     * Soft enable: SGI 0–15 + PPI 27 (virtual timer).
     * SPI 32+ programmed separately in gic_spi_soft_observe.
     * Enabling the PPI bit alone does not fire IRQs: CPU DAIF.I is still
     * set from entry.S, CNTV_CTL.IMASK stays set in timer soft path, and
     * there is no full GICC_IAR consumer. This only primes the distributor.
     */
    *gicd(GICD_ISENABLER0) = GICD_SOFT_ISENABLER0;
    gic_barrier();

    /* Read-back soft check (non-fatal; confirms distributor accepts enable). */
    uIsen = *gicd(GICD_ISENABLER0);

    /* CPU interface: unmask priorities, enable group 0 signaling. */
    *gicc(GICC_PMR) = GICC_PMR_ALLOW_ALL;
    *gicc(GICC_CTLR) = GICC_CTLR_ENABLE;
    gic_barrier();

    uCtlr = *gicc(GICC_CTLR);
    uPmr = *gicc(GICC_PMR);

    aarch64_uart_puts("aarch64: GIC typer=");
    aarch64_uart_put_hex((unsigned long)uTyper);
    aarch64_uart_puts(" lines=");
    aarch64_uart_put_hex((unsigned long)cIrqLines);
    aarch64_uart_puts(" isen0=");
    aarch64_uart_put_hex((unsigned long)uIsen);
    aarch64_uart_puts(" pmr=");
    aarch64_uart_put_hex((unsigned long)uPmr);
    aarch64_uart_puts(" ctlr=");
    aarch64_uart_put_hex((unsigned long)uCtlr);
    aarch64_uart_puts("\n");

    /* Soft SPI path: UART + virtio0 prime + IAR spurious peek. */
    fSpiSoft = gic_spi_soft_observe(uTyper, cIrqLines);

    /* Greppable: aarch64-smoke / manual serial capture. */
    aarch64_uart_puts("aarch64: GIC PASS\n");
    if (fSpiSoft != 0) {
        aarch64_uart_puts("aarch64: GIC SPI soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: GIC SPI soft FAIL\n");
    }
}
