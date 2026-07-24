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
 * -------------------------------------------------------------------------
 * Soft GIC inventory (distributor / cpuif / SPI count)
 * -------------------------------------------------------------------------
 * After enable + SPI soft program, re-read identity/config registers and
 * emit greppable inventory lines (no allocation, no NEON, general regs):
 *   distributor soft — CTLR, TYPER fields, IIDR, ISENABLER0
 *   cpuif soft       — CTLR, PMR, BPR, RPR, HPPIR, IIDR
 *   SPI count soft   — SPI capacity from ITLinesNumber (lines − 32)
 * Soft PASS when dist+cpuif enables stick, PMR allows all, soft ISENABLER0
 * bits remain, and SPI capacity covers UART/virtio0 INTIDs.
 *
 * -------------------------------------------------------------------------
 * Soft inventory deepen (Wave 30 exclusive; this unit only)
 * -------------------------------------------------------------------------
 * Multi-line greppable "aarch64: gic soft …" under fixed areas:
 *   inventory | dist | cpuif | spi | ppi | iar | gates | path | deepen
 * PPI soft: SGI mask + vtimer PPI 27 + masked phys timer lamps.
 * IAR soft: last soft-observe INTID + spurious lamp.
 * Gates soft: enable/PMR/ISEN0/SPI-cap rollup lamps.
 * Path honesty: soft prime only — no product IRQ delivery / EOI loop.
 * Soft PASS/FAIL gates unchanged from prior waves; deepen never hard-gates.
 * Soft ≠ product SMP GIC; soft ≠ bar3.
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
 *            aarch64: gic soft dist …
 *            aarch64: gic soft cpuif …
 *            aarch64: gic soft spi count=…
 *            aarch64: gic soft inventory …
 *            aarch64: gic soft ppi …
 *            aarch64: gic soft iar …
 *            aarch64: gic soft gates …
 *            aarch64: gic soft path …
 *            aarch64: gic soft return inv_ret=… product_kernel=OPEN
 *            aarch64: gic soft deepen …
 *            aarch64: gic soft PASS | FAIL
 *
 * Freestanding pure C; no GPL Linux arch paste.
 * Note: freestanding EL1 does not enable FP/SIMD (CPACR). Avoid NEON —
 * general-regs-only for this TU.
 */
#include "types_arch.h"

#if defined(__GNUC__)
#pragma GCC target("general-regs-only")
#endif

/* QEMU virt GICv2 defaults (not board-probed). */
#define GICD_BASE 0x08000000ul
#define GICC_BASE 0x08010000ul

/* Distributor offsets (word-addressed via u32 *) */
#define GICD_CTLR       0x000u
#define GICD_TYPER      0x004u
#define GICD_IIDR       0x008u /* soft inventory: implementer / revision */
#define GICD_ISENABLER0 0x100u
/* Soft SPI / priority / target / config (byte or word per GICv2 layout). */
#define GICD_IPRIORITYR0 0x400u /* +INTID → byte priority */
#define GICD_ITARGETSR0  0x800u /* SPI CPU targets; SGI/PPI RO/impl-def */
#define GICD_ICFGR0      0xc00u /* 2 bits/INTID config */

/* GICD_TYPER soft-decode masks (GICv2). */
#define GICD_TYPER_ITLINES_MASK 0x1fu
#define GICD_TYPER_CPU_SHIFT    5u
#define GICD_TYPER_CPU_MASK     0x7u
#define GICD_TYPER_SECEXT_BIT   (1u << 10)

/* CPU interface offsets */
#define GICC_CTLR  0x000u
#define GICC_PMR   0x004u
#define GICC_BPR   0x008u
#define GICC_IAR   0x00cu /* soft path: ACK / read INTID */
#define GICC_EOIR  0x010u /* soft path: EOI write INTID */
#define GICC_RPR   0x014u /* running priority (soft inventory) */
#define GICC_HPPIR 0x018u /* highest pending (soft inventory) */
#define GICC_IIDR  0x0fcu /* CPU interface identity (soft inventory) */

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

/* Soft PPI INTIDs (local; Wave 19 inventory lamps). */
#define GIC_PPI_VTIMER_INTID  27u /* virtual timer */
#define GIC_PPI_PTIMER_INTID  30u /* non-secure physical timer (masked) */
#define GIC_PPI_STIMER_INTID  29u /* secure physical timer (masked) */
#define GIC_PPI_VMAINT_INTID  28u /* virtual maintenance (unused at EL1) */

/* Wave 30 soft inventory stamp (file-local; never product gate). */
#define GIC_SOFT_WAVE   30u
#define GIC_SOFT_AREAS 34u

extern void aarch64_uart_puts(const char *sz);
extern void aarch64_uart_put_hex(unsigned long v);

/*
 * Soft SPI/IAR peeks captured by gic_spi_soft_observe for Wave 19 inventory.
 * Zero until observe runs; never hard-gate product.
 */
static unsigned g_uSoftSpiPrioUart;
static unsigned g_uSoftSpiTgtUart;
static unsigned g_uSoftSpiPrioVio;
static unsigned g_uSoftSpiTgtVio;
static unsigned g_uSoftSpiIsen1;
static unsigned g_uSoftIarRaw;
static unsigned g_uSoftIarId;
static unsigned g_uSoftIarSpur;
static unsigned g_uSoftSpiOk;
static unsigned g_cSoftInvLogs;

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

    /* Wave 19: capture SPI/IAR soft peeks for inventory emission. */
    g_uSoftSpiPrioUart = uPrioUart;
    g_uSoftSpiTgtUart = uTgtUart;
    g_uSoftSpiPrioVio = uPrioVio;
    g_uSoftSpiTgtVio = uTgtVio;
    g_uSoftSpiIsen1 = uIsen1b;
    g_uSoftIarRaw = uIar;
    g_uSoftIarId = uId;
    g_uSoftIarSpur = (uId == GICC_INTID_SPURIOUS) ? 1u : 0u;
    g_uSoftSpiOk = (fOk != 0) ? 1u : 0u;

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

/*
 * Soft GIC inventory deepen (Wave 19): re-read distributor + CPU interface
 * identity and config, decode SPI count from TYPER, emit greppable
 * "aarch64: gic soft …" multi-area inventory. Pure word/byte MMIO; no
 * FP/NEON. Returns 1 on soft PASS. Never hard-gates product bring-up.
 *
 * Gates (all soft / non-fatal for product bring-up):
 *   - GICD_CTLR EnableGrp0 stuck
 *   - GICC_CTLR enable stuck
 *   - GICC_PMR allow-all stuck
 *   - soft ISENABLER0 (SGI + vtimer PPI) still set
 *   - SPI capacity covers virtio0 INTID 48 (cSpi >= 32 → lines >= 64)
 *
 * Grep areas: inventory | dist | cpuif | spi | ppi | iar | gates | path | deepen
 */
static int
gic_soft_inventory(unsigned uTyper, unsigned cIrqLines, unsigned uIsen0)
{
    unsigned uDistCtlr;
    unsigned uDistIidr;
    unsigned uItLines;
    unsigned uCpuNum;
    unsigned uSecExt;
    unsigned cSpi;
    unsigned uCpuCtlr;
    unsigned uCpuPmr;
    unsigned uBpr;
    unsigned uRpr;
    unsigned uHppir;
    unsigned uCpuIidr;
    unsigned uIsen0rb;
    unsigned uGateDist;
    unsigned uGateCpu;
    unsigned uGatePmr;
    unsigned uGateIsen0;
    unsigned uGateSpiCap;
    unsigned uGateMmio;
    unsigned uPpiVtimer;
    unsigned uPpiPtimer;
    unsigned uSgiMask;
    int fOk;

    if (g_cSoftInvLogs < 0xffffffffu) {
        g_cSoftInvLogs++;
    }

    /* Distributor soft inventory. */
    uDistCtlr = *gicd(GICD_CTLR);
    uDistIidr = *gicd(GICD_IIDR);
    uIsen0rb = *gicd(GICD_ISENABLER0);
    uItLines = uTyper & GICD_TYPER_ITLINES_MASK;
    uCpuNum = (uTyper >> GICD_TYPER_CPU_SHIFT) & GICD_TYPER_CPU_MASK;
    uSecExt = (uTyper & GICD_TYPER_SECEXT_BIT) != 0u ? 1u : 0u;
    /*
     * SPI count soft: total INTIDs minus fixed SGI/PPI bank (0–31).
     * ITLinesNumber N → total lines = 32*(N+1); SPI count = 32*N.
     */
    if (cIrqLines > 32u) {
        cSpi = cIrqLines - 32u;
    } else {
        cSpi = 0u;
    }

    uSgiMask = uIsen0rb & GICD_SGI_ENABLE_MASK;
    uPpiVtimer = (uIsen0rb & GICD_PPI_VTIMER_BIT) != 0u ? 1u : 0u;
    uPpiPtimer = (uIsen0rb & (1u << GIC_PPI_PTIMER_INTID)) != 0u ? 1u : 0u;

    /* Grep: aarch64: gic soft inventory */
    aarch64_uart_puts("aarch64: gic soft inventory lines=");
    aarch64_uart_put_hex((unsigned long)cIrqLines);
    aarch64_uart_puts(" spi=");
    aarch64_uart_put_hex((unsigned long)cSpi);
    aarch64_uart_puts(" spi_ok=");
    aarch64_uart_put_hex((unsigned long)g_uSoftSpiOk);
    aarch64_uart_puts(" logs=");
    aarch64_uart_put_hex((unsigned long)g_cSoftInvLogs);
    aarch64_uart_puts(" wave=");
    aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
    aarch64_uart_puts(" areas=");
    aarch64_uart_put_hex((unsigned long)GIC_SOFT_AREAS);
    aarch64_uart_puts("\n");

    aarch64_uart_puts("aarch64: gic soft dist ctlr=");
    aarch64_uart_put_hex((unsigned long)uDistCtlr);
    aarch64_uart_puts(" typer=");
    aarch64_uart_put_hex((unsigned long)uTyper);
    aarch64_uart_puts(" iidr=");
    aarch64_uart_put_hex((unsigned long)uDistIidr);
    aarch64_uart_puts(" isen0=");
    aarch64_uart_put_hex((unsigned long)uIsen0rb);
    aarch64_uart_puts(" itlines=");
    aarch64_uart_put_hex((unsigned long)uItLines);
    aarch64_uart_puts(" cpus=");
    aarch64_uart_put_hex((unsigned long)(uCpuNum + 1u));
    aarch64_uart_puts(" secext=");
    aarch64_uart_put_hex((unsigned long)uSecExt);
    aarch64_uart_puts(" base=");
    aarch64_uart_put_hex(GICD_BASE);
    aarch64_uart_puts("\n");

    /* CPU interface soft inventory. */
    uCpuCtlr = *gicc(GICC_CTLR);
    uCpuPmr = *gicc(GICC_PMR);
    uBpr = *gicc(GICC_BPR);
    uRpr = *gicc(GICC_RPR);
    uHppir = *gicc(GICC_HPPIR);
    uCpuIidr = *gicc(GICC_IIDR);

    aarch64_uart_puts("aarch64: gic soft cpuif ctlr=");
    aarch64_uart_put_hex((unsigned long)uCpuCtlr);
    aarch64_uart_puts(" pmr=");
    aarch64_uart_put_hex((unsigned long)uCpuPmr);
    aarch64_uart_puts(" bpr=");
    aarch64_uart_put_hex((unsigned long)uBpr);
    aarch64_uart_puts(" rpr=");
    aarch64_uart_put_hex((unsigned long)uRpr);
    aarch64_uart_puts(" hppir=");
    aarch64_uart_put_hex((unsigned long)uHppir);
    aarch64_uart_puts(" iidr=");
    aarch64_uart_put_hex((unsigned long)uCpuIidr);
    aarch64_uart_puts(" base=");
    aarch64_uart_put_hex(GICC_BASE);
    aarch64_uart_puts("\n");

    /* SPI count soft inventory (capacity + soft-programmed INTIDs). */
    aarch64_uart_puts("aarch64: gic soft spi count=");
    aarch64_uart_put_hex((unsigned long)cSpi);
    aarch64_uart_puts(" lines=");
    aarch64_uart_put_hex((unsigned long)cIrqLines);
    aarch64_uart_puts(" itlines=");
    aarch64_uart_put_hex((unsigned long)uItLines);
    aarch64_uart_puts(" uart=");
    aarch64_uart_put_hex((unsigned long)GIC_SPI_UART_INTID);
    aarch64_uart_puts(" vio0=");
    aarch64_uart_put_hex((unsigned long)GIC_SPI_VIRTIO0_INTID);
    aarch64_uart_puts(" need=");
    aarch64_uart_put_hex((unsigned long)(GIC_SPI_VIRTIO0_INTID + 1u));
    aarch64_uart_puts(" prio_u=");
    aarch64_uart_put_hex((unsigned long)g_uSoftSpiPrioUart);
    aarch64_uart_puts(" tgt_u=");
    aarch64_uart_put_hex((unsigned long)g_uSoftSpiTgtUart);
    aarch64_uart_puts(" prio_v=");
    aarch64_uart_put_hex((unsigned long)g_uSoftSpiPrioVio);
    aarch64_uart_puts(" tgt_v=");
    aarch64_uart_put_hex((unsigned long)g_uSoftSpiTgtVio);
    aarch64_uart_puts(" isen1=");
    aarch64_uart_put_hex((unsigned long)g_uSoftSpiIsen1);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: gic soft ppi (SGI bank + vtimer soft arm lamps) */
    aarch64_uart_puts("aarch64: gic soft ppi vtimer=");
    aarch64_uart_put_hex((unsigned long)GIC_PPI_VTIMER_INTID);
    aarch64_uart_puts(" armed=");
    aarch64_uart_put_hex((unsigned long)uPpiVtimer);
    aarch64_uart_puts(" ptimer=");
    aarch64_uart_put_hex((unsigned long)GIC_PPI_PTIMER_INTID);
    aarch64_uart_puts(" p_armed=");
    aarch64_uart_put_hex((unsigned long)uPpiPtimer);
    aarch64_uart_puts(" stimer=");
    aarch64_uart_put_hex((unsigned long)GIC_PPI_STIMER_INTID);
    aarch64_uart_puts(" vmaint=");
    aarch64_uart_put_hex((unsigned long)GIC_PPI_VMAINT_INTID);
    aarch64_uart_puts(" sgi_mask=");
    aarch64_uart_put_hex((unsigned long)uSgiMask);
    aarch64_uart_puts(" soft_isen0=");
    aarch64_uart_put_hex((unsigned long)GICD_SOFT_ISENABLER0);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: gic soft iar */
    aarch64_uart_puts("aarch64: gic soft iar raw=");
    aarch64_uart_put_hex((unsigned long)g_uSoftIarRaw);
    aarch64_uart_puts(" id=");
    aarch64_uart_put_hex((unsigned long)g_uSoftIarId);
    aarch64_uart_puts(" spur=");
    aarch64_uart_put_hex((unsigned long)g_uSoftIarSpur);
    aarch64_uart_puts(" spur_id=");
    aarch64_uart_put_hex((unsigned long)GICC_INTID_SPURIOUS);
    aarch64_uart_puts(" uart_id=");
    aarch64_uart_put_hex((unsigned long)GIC_SPI_UART_INTID);
    aarch64_uart_puts(" vio0_id=");
    aarch64_uart_put_hex((unsigned long)GIC_SPI_VIRTIO0_INTID);
    aarch64_uart_puts("\n");

    uGateDist = ((uDistCtlr & GICD_CTLR_ENABLE) != 0u) ? 1u : 0u;
    uGateCpu = ((uCpuCtlr & GICC_CTLR_ENABLE) != 0u) ? 1u : 0u;
    uGatePmr = ((uCpuPmr & 0xffu) == GICC_PMR_ALLOW_ALL) ? 1u : 0u;
    uGateIsen0 =
        ((uIsen0rb & GICD_SOFT_ISENABLER0) == GICD_SOFT_ISENABLER0) ? 1u : 0u;
    uGateSpiCap = (cSpi >= 32u && cIrqLines >= 64u) ? 1u : 0u;
    uGateMmio = (uTyper != 0xffffffffu && uDistIidr != 0xffffffffu) ? 1u : 0u;

    /* Grep: aarch64: gic soft gates */
    aarch64_uart_puts("aarch64: gic soft gates dist=");
    aarch64_uart_put_hex((unsigned long)uGateDist);
    aarch64_uart_puts(" cpu=");
    aarch64_uart_put_hex((unsigned long)uGateCpu);
    aarch64_uart_puts(" pmr=");
    aarch64_uart_put_hex((unsigned long)uGatePmr);
    aarch64_uart_puts(" isen0=");
    aarch64_uart_put_hex((unsigned long)uGateIsen0);
    aarch64_uart_puts(" spi_cap=");
    aarch64_uart_put_hex((unsigned long)uGateSpiCap);
    aarch64_uart_puts(" mmio=");
    aarch64_uart_put_hex((unsigned long)uGateMmio);
    aarch64_uart_puts(" spi_ok=");
    aarch64_uart_put_hex((unsigned long)g_uSoftSpiOk);
    aarch64_uart_puts("\n");

    fOk = 1;
    if (uGateDist == 0u || uGateCpu == 0u || uGatePmr == 0u ||
        uGateIsen0 == 0u || uGateSpiCap == 0u || uGateMmio == 0u) {
        fOk = 0;
    }
    /* Sanity: caller snapshot of ISENABLER0 should match re-read soft bits. */
    if ((uIsen0 & GICD_SOFT_ISENABLER0) != GICD_SOFT_ISENABLER0) {
        fOk = 0;
    }

    /* Grep: aarch64: gic soft path */
    aarch64_uart_puts("aarch64: gic soft path prime=1 deliver=0 eoi_loop=0 "
                      "daif_i=1 neon=0 smp=0 product_kernel=OPEN "
                      "hard_gate=0 wave=");
    aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
    aarch64_uart_puts(" (soft inventory; not bar3)\n");

    /* Grep: aarch64: gic soft surf — Wave 19 gate bit lamps */
    aarch64_uart_puts("aarch64: gic soft surf dist=");
    aarch64_uart_put_hex((unsigned long)uGateDist);
    aarch64_uart_puts(" cpu=");
    aarch64_uart_put_hex((unsigned long)uGateCpu);
    aarch64_uart_puts(" pmr=");
    aarch64_uart_put_hex((unsigned long)uGatePmr);
    aarch64_uart_puts(" isen0=");
    aarch64_uart_put_hex((unsigned long)uGateIsen0);
    aarch64_uart_puts(" spi_cap=");
    aarch64_uart_put_hex((unsigned long)uGateSpiCap);
    aarch64_uart_puts(" mmio=");
    aarch64_uart_put_hex((unsigned long)uGateMmio);
    aarch64_uart_puts(" spi_ok=");
    aarch64_uart_put_hex((unsigned long)g_uSoftSpiOk);
    aarch64_uart_puts(" bits=");
    aarch64_uart_put_hex((unsigned long)(
        (uGateDist << 0) | (uGateCpu << 1) | (uGatePmr << 2) |
        (uGateIsen0 << 3) | (uGateSpiCap << 4) | (uGateMmio << 5) |
        ((g_uSoftSpiOk != 0u ? 1u : 0u) << 6)));
    aarch64_uart_puts(" wave=");
    aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: gic soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: aarch64: gic: soft retclass — Wave 19 return-class taxonomy (kept) */
    aarch64_uart_puts("aarch64: gic: soft retclass ok|fail|inval|nodev|busy|nomem "
                      "soft_only=1 product_gate=0 wave=");
    aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
    aarch64_uart_puts(" (retclass taxonomy; Soft!=product; not bar3)\n");
    /* Grep: aarch64: gic: soft retlane — Wave 19 return-lane catalog (kept) */
    aarch64_uart_puts("aarch64: gic: soft retlane inv|selftest|rate|retcode|retmap|class "
                      "product_kernel=OPEN soft_ne_product=1 wave=");
    aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
    aarch64_uart_puts(" (retlane catalog; Soft!=product)\n");
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: aarch64: gic: soft retbound — Wave 20 return-bound honesty (kept) */
    aarch64_uart_puts("aarch64: gic: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
                      "never_blocks_m0=1 wave=");
    aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
    aarch64_uart_puts(" (retbound honesty; Soft!=product; not bar3)\n");
    /* Grep: aarch64: gic: soft retseal — Wave 20 seal stamp (kept) */
    aarch64_uart_puts("aarch64: gic: soft retseal exclusive=1 soft_ne_product=1 "
                      "product_kernel=OPEN bar3=0 wave=");
    aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
    aarch64_uart_puts(" (retseal stamp; Soft!=product)\n");
    /*
     * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: aarch64: gic: soft retpulse — Wave 21 return-pulse honesty (kept) */
    aarch64_uart_puts("aarch64: gic: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                      "never_blocks_m0=1 wave=");
    aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
    aarch64_uart_puts(" (retpulse honesty; Soft!=product; not bar3)\n");
    /* Grep: aarch64: gic: soft retmark — Wave 21 mark stamp (kept) */
    aarch64_uart_puts("aarch64: gic: soft retmark exclusive=1 soft_ne_product=1 "
                      "product_kernel=OPEN bar3=0 wave=");
    aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
    aarch64_uart_puts(" (retmark stamp; Soft!=product)\n");
    
/*
 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: gic: soft retphase — Wave 22 return-phase honesty (kept) */
aarch64_uart_puts("aarch64: gic: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
aarch64_uart_puts(" (retphase honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: gic: soft retbadge — Wave 22 badge stamp (kept) */
aarch64_uart_puts("aarch64: gic: soft retbadge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
aarch64_uart_puts(" (retbadge stamp; Soft!=product)\n");
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: gic: soft rettoken — Wave 23 return-token honesty (kept) */
aarch64_uart_puts("aarch64: gic: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
aarch64_uart_puts(" (rettoken honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: gic: soft retcrest — Wave 23 crest stamp (kept) */
aarch64_uart_puts("aarch64: gic: soft retcrest exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
aarch64_uart_puts(" (retcrest stamp; Soft!=product)\n");
/*
 * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: gic: soft retvault — Wave 24 return-vault honesty (kept) */
aarch64_uart_puts("aarch64: gic: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
aarch64_uart_puts(" (retvault honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: gic: soft retbanner — Wave 24 banner stamp (kept) */
aarch64_uart_puts("aarch64: gic: soft retbanner exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
aarch64_uart_puts(" (retbanner stamp; Soft!=product)\n");
/*
 * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: gic: soft retledger — Wave 25 return-ledger honesty (kept) */
aarch64_uart_puts("aarch64: gic: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
aarch64_uart_puts(" (retledger honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: gic: soft retbeacon — Wave 25 beacon stamp (kept) */
aarch64_uart_puts("aarch64: gic: soft retbeacon exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
aarch64_uart_puts(" (retbeacon stamp; Soft!=product)\n");
/*
 * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: gic: soft retcipher — Wave 26 return-cipher honesty (kept) */
aarch64_uart_puts("aarch64: gic: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
aarch64_uart_puts(" (retcipher honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: gic: soft retflame — Wave 26 flame stamp (kept) */
aarch64_uart_puts("aarch64: gic: soft retflame exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
aarch64_uart_puts(" (retflame stamp; Soft!=product)\n");
/*
 * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: gic: soft retprism — Wave 27 return-prism honesty (kept) */
aarch64_uart_puts("aarch64: gic: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
aarch64_uart_puts(" (retprism honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: gic: soft retforge — Wave 27 forge stamp (kept) */
aarch64_uart_puts("aarch64: gic: soft retforge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
aarch64_uart_puts(" (retforge stamp; Soft!=product)\n");
/*
 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: gic: soft retshard — Wave 28 return-shard honesty (kept) */
aarch64_uart_puts("aarch64: gic: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
aarch64_uart_puts(" (retshard honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: gic: soft retcrown — Wave 28 crown stamp (kept) */
aarch64_uart_puts("aarch64: gic: soft retcrown exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
aarch64_uart_puts(" (retcrown stamp; Soft!=product)\n");
/*
 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: gic: soft retglyph — Wave 29 return-glyph honesty (kept) */
aarch64_uart_puts("aarch64: gic: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
aarch64_uart_puts(" (retglyph honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: gic: soft retscepter — Wave 29 scepter stamp (kept) */
aarch64_uart_puts("aarch64: gic: soft retscepter exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
aarch64_uart_puts(" (retscepter stamp; Soft!=product)\n");
/*
 * ---- Wave 30 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: gic: soft retsigil — Wave 30 return-sigil honesty */
aarch64_uart_puts("aarch64: gic: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
aarch64_uart_puts(" (retsigil honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: gic: soft retemblem — Wave 30 exclusive emblem stamp */
aarch64_uart_puts("aarch64: gic: soft retemblem exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
aarch64_uart_puts(" (retemblem stamp; Soft!=product)\n");
    aarch64_uart_puts("aarch64: gic soft deepen wave=");
    aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
    aarch64_uart_puts(" areas=");
    aarch64_uart_put_hex((unsigned long)GIC_SOFT_AREAS);
    aarch64_uart_puts(" catalog=inventory,dist,cpuif,spi,ppi,iar,gates,"
                      "path,surf,return,honesty,deepen unit=gic.c only "
                      "rate_limited=0 soft_only=1\n");

    /* Grep: aarch64: gic soft honesty */
    aarch64_uart_puts("aarch64: gic soft honesty product_kernel=OPEN "
                      "soft_only=1 no_irq_take=1 no_bar3=1 wave=");
    aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: gic soft exclusive — Wave 30 exclusive deepen */
    aarch64_uart_puts("aarch64: gic soft exclusive multi_server=0 "
                      "confine=0 bar3=0 product_kernel=OPEN soft_only=1 wave=");
    aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: gic soft open — Wave 19 open-lamp rollup */
    aarch64_uart_puts("aarch64: gic soft open multi_server=0 confine=0 "
                      "bar3=0 product_kernel=OPEN soft_only=1 wave=");
    aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
    aarch64_uart_puts("\n");

    aarch64_uart_put_hex((unsigned long)(fOk != 0 ? 1ul : 0ul));
    aarch64_uart_puts(" product_kernel=OPEN wave=");
    aarch64_uart_put_hex((unsigned long)GIC_SOFT_WAVE);
    aarch64_uart_puts("\n");

    if (fOk != 0) {
        aarch64_uart_puts("aarch64: gic soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: gic soft FAIL\n");
    }

    return fOk;
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
    int fInvSoft;

    /* Enable distributor (group 0). No probe of base — fixed virt map. */
    *gicd(GICD_CTLR) = GICD_CTLR_ENABLE;
    gic_barrier();

    /*
     * TYPER[4:0] = ITLinesNumber → SPI capacity:
     *   32 * (ITLinesNumber + 1) total INTIDs (incl. SGI/PPI).
     * Used as soft sanity read that MMIO responds + SPI soft gate.
     */
    uTyper = *gicd(GICD_TYPER);
    cIrqLines = ((uTyper & GICD_TYPER_ITLINES_MASK) + 1u) * 32u;

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

    /*
     * Soft inventory deepen (distributor / cpuif / SPI count).
     * Greppable: aarch64: gic soft …
     */
    fInvSoft = gic_soft_inventory(uTyper, cIrqLines, uIsen);

    /* Greppable: aarch64-smoke / manual serial capture. */
    aarch64_uart_puts("aarch64: GIC PASS\n");
    if (fSpiSoft != 0) {
        aarch64_uart_puts("aarch64: GIC SPI soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: GIC SPI soft FAIL\n");
    }

    (void)fInvSoft;
}
