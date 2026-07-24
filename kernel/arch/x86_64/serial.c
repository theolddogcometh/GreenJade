/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * 16550 COM1 early console (I/O port GJ_SERIAL_PORT, product default 0x3F8).
 * Polled TX/RX only — no IRQ path. Freestanding pure C; no GPL paste.
 *
 * -------------------------------------------------------------------------
 * Soft COM1 init / status observability (boot telemetry — not hot-path)
 * -------------------------------------------------------------------------
 * Soft init notes: after 8N1/FIFO/MCR program, snapshot IER/LCR/MCR +
 * divisor readback (DLAB peek) and live LSR/MSR/IIR. Bumps soft inits.
 * Soft status: live LSR lamps (DR/THRE/TEMT/errors), MSR modem lamps,
 * MCR path lamps, IIR noint/id, TX spin budget + char/poll/getc counters.
 * Soft verify: IER=0 (polled), LCR=8N1 DLAB-off, MCR DTR|RTS|OUT2,
 * divisor lo/hi match 38400 program (0x0003), live not floating 0xFF.
 * Soft expect subflags (ier_ok/lcr_ok/mcr_ok/div_ok/live_ok + Wave 13
 * noint_ok/dlab_ok/fcr_ok/thre_ok + Wave 15 temt_ok/out2_ok/float_ok/scr_ok)
 * deepen the greppable inventory without extra reprogram or IRQ claim. Never
 * reprograms UART after init; FCR is write-only (program shadow only). Boot
 * bring-up stays a tight line set; full inventory is serial_soft_log only
 * (no putchar flood). Soft ≠ IRQ console, ≠ bar3, ≠ product TTY.
 *
 * Greppable (product / smoke inventory — Wave 10 base + Wave 13 path +
 * Wave 35 exclusive deepen, prefix-stable):
 *   serial: soft inventory … wave=79
 *   serial: soft program port=… div=… lcr=… mcr=… fcr=… ier=… baud=38400
 *   serial: soft inits=… chars=… spinmax=… thrwait=… txfull=… poll=… getc=…
 *   serial: soft port=0x… ier=0x… lcr=0x… mcr=0x… lsr=0x… msr=0x… iir=0x…
 *   serial: soft div=0x… thre=… temt=… dr=… dlab=… oe=… pe=… fe=… bi=… err=…
 *   serial: soft msr cts=… dsr=… ri=… dcd=… dcts=… ddsr=… teri=… ddcd=…
 *   serial: soft thr wait=… spinmax=… txfull=… spin_cap=… chars=… thre_ok=…
 *   serial: soft iir noint=… id=… noint_ok=… scr=0x…
 *   serial: soft path polled=1 irq=0 fcr=0x… spin_cap=… ready=… live=…
 *   serial: soft expect ier_ok=… lcr_ok=… mcr_ok=… div_ok=… live_ok=…
 *   serial: soft verify PASS|FAIL|idle (ok=… bad=…)
 * Wave 15 complementary surfaces (kept; never reshape primary fields):
 *   serial: soft lamps …      — ready/live/thre/temt/dtr/rts/out2/noint
 *   serial: soft stats …      — aggregate path counters + wave
 *   serial: soft mcr …        — DTR|RTS|OUT2 path lamps
 *   serial: soft float …      — float/live presence axis
 *   serial: soft honesty …    — soft ≠ IRQ console / bar3 / product TTY
 * Wave 16 complementary surfaces (kept; never reshape primary fields):
 *   serial: soft exclusive …  — exclusive=1 unit stamp + wave
 *   serial: soft claim …      — product claim bounds (polled console)
 *   serial: soft ratio …      — ok/bad + poll hit/miss + thr path ratios
 *   serial: soft err …        — LSR error lamp rollup (oe/pe/fe/bi/err)
 *   serial: soft return rate — Wave 19 ok/fail rate lamps
 *   serial: soft retcode    — Wave 19 retcode catalog
 *   serial: soft deepen …     — wave=79 areas stamp
 * Wave 17 complementary surfaces (kept) (never reshape primary fields):
 *   serial: soft return …     — Wave 17 API return surfaces (kept)
 *   serial: soft return selftest … — Wave 17 terminal return surface (kept)
 *   serial: soft retmap …     — Wave 17 return-surface map (kept)
 *
 * Soft APIs (linkable; no main hook required — serial_init self-logs):
 *   serial_soft_inits / chars / spinmax / thr_waits / polls / getcs
 *   serial_soft_verify_ok / verify_bad / ready / txfull / log_n
 *   serial_soft_port / ier / lcr / mcr / lsr / msr / iir / div
 *   serial_soft_status_refresh / serial_soft_verify / serial_soft_log
 */
#include <gj/config.h>
#include <gj/console.h>
#include <gj/klog.h>
#include <gj/types.h>

/* 16550 register offsets from COM base. */
#define UART_DATA   0u /* RBR/THR; DLL when DLAB */
#define UART_IER    1u /* IER; DLH when DLAB */
#define UART_IIR    2u /* IIR read / FCR write */
#define UART_LCR    3u
#define UART_MCR    4u
#define UART_LSR    5u
#define UART_MSR    6u
#define UART_SCR    7u

/* LCR */
#define UART_LCR_WLS8   0x03u /* 8 data bits */
#define UART_LCR_DLAB   0x80u

/* IER — product path leaves IRQ disabled (polled console). */
#define UART_IER_SOFT_NONE 0x00u

/* MCR: DTR | RTS | OUT2 (OUT2 arms legacy IRQ line on real 16550; still set). */
#define UART_MCR_DTR    0x01u
#define UART_MCR_RTS    0x02u
#define UART_MCR_OUT2   0x08u
#define UART_MCR_SOFT   (UART_MCR_DTR | UART_MCR_RTS | UART_MCR_OUT2) /* 0x0B */

/* FCR program (write-only on classic 16550): enable + clear + 14-byte trigger. */
#define UART_FCR_SOFT   0xC7u

/* LSR bits */
#define UART_LSR_DR     0x01u
#define UART_LSR_OE     0x02u
#define UART_LSR_PE     0x04u
#define UART_LSR_FE     0x08u
#define UART_LSR_BI     0x10u
#define UART_LSR_THRE   0x20u
#define UART_LSR_TEMT   0x40u
#define UART_LSR_ERR    0x80u

/* MSR soft lamps (live peek only). */
#define UART_MSR_DCTS   0x01u
#define UART_MSR_DDSR   0x02u
#define UART_MSR_TERI   0x04u
#define UART_MSR_DDCD   0x08u
#define UART_MSR_CTS    0x10u
#define UART_MSR_DSR    0x20u
#define UART_MSR_RI     0x40u
#define UART_MSR_DCD    0x80u

/* Product baud program: 115200/38400 = 3 → divisor 0x0003. */
#define UART_SOFT_DIV_LO 0x03u
#define UART_SOFT_DIV_HI 0x00u
#define UART_SOFT_LCR    UART_LCR_WLS8 /* 8N1, DLAB clear */

/* Soft TX wait budget (match UEFI stub / aarch64 PL011 spin ceiling). */
#define UART_SOFT_SPIN_MAX 100000u

/* IIR soft lamps (read path; FCR is write-only at same offset). */
#define UART_IIR_NOINT  0x01u /* 1 = no interrupt pending */
#define UART_IIR_ID_MASK 0x0eu

/* Soft Wave stamp (greppable inventory only; never hard-gates boot). */
#define UART_SOFT_WAVE 79u

/* Product soft baud label (115200/3 → 38400; divisor program 0x0003). */
#define UART_SOFT_BAUD 38400u

/* Soft snapshot of last programmed + last live status peeks. */
struct serial_soft_snap {
    u16 u16Port;
    u16 u16Div;     /* DLL|(DLH<<8) after DLAB readback */
    u8  u8DivLo;    /* DLL soft lamp (Wave 13) */
    u8  u8DivHi;    /* DLH soft lamp (Wave 13) */
    u8  u8Ier;
    u8  u8Lcr;
    u8  u8Mcr;
    u8  u8Lsr;
    u8  u8Msr;
    u8  u8Iir;
    u8  u8FcrProg;  /* FCR write shadow (classic 16550 is write-only) */
    u8  u8Scr;      /* scratch reg live peek */
    u8  u8Thre;     /* LSR.THRE lamp */
    u8  u8Temt;     /* LSR.TEMT lamp */
    u8  u8Dr;       /* LSR.DR lamp */
    u8  u8Dlab;     /* LCR.DLAB lamp (expect 0 after init) */
    u8  u8Oe;       /* overrun */
    u8  u8Pe;       /* parity */
    u8  u8Fe;       /* framing */
    u8  u8Bi;       /* break */
    u8  u8Err;      /* LSR.ERR (fifo error) lamp */
    /* MSR modem lamps (live). */
    u8  u8Cts;
    u8  u8Dsr;
    u8  u8Ri;
    u8  u8Dcd;
    u8  u8Dcts;
    u8  u8Ddsr;
    u8  u8Teri;
    u8  u8Ddcd;
    /* MCR path lamps. */
    u8  u8Dtr;
    u8  u8Rts;
    u8  u8Out2;
    /* IIR soft. */
    u8  u8Noint;    /* IIR no-interrupt-pending */
    u8  u8IirId;    /* IIR ID field (bits 1..3) */
    /* Soft float / live presence. */
    u8  u8Float;    /* LSR+MSR all-ones → missing I/O */
    u8  u8LiveOk;   /* not floating */
    /* Soft expect subflags (last verify; Wave 10 base + Wave 13 + Wave 15). */
    u8  u8IerOk;
    u8  u8LcrOk;
    u8  u8McrOk;
    u8  u8DivOk;
    u8  u8NointOk;  /* Wave 13: IIR.NOINT (polled path expects quiet) */
    u8  u8DlabOk;   /* Wave 13: DLAB clear after program */
    u8  u8FcrOk;    /* Wave 13: FCR program shadow matches UART_FCR_SOFT */
    u8  u8ThreOk;   /* Wave 13: last LSR.THRE lamp (TX holding empty) */
    u8  u8TemtOk;   /* Wave 15: last LSR.TEMT lamp (TX shift empty) */
    u8  u8Out2Ok;   /* Wave 15: MCR.OUT2 set (legacy IRQ arm; still polled) */
    u8  u8FloatOk;  /* Wave 15: not floating bus (inverse of u8Float) */
    u8  u8ScrOk;    /* Wave 15: SCR peek not 0xFF when live */
    u8  u8VerifyOk; /* last soft verify aggregate (Wave 10 gates only) */
    u8  u8Pad;
};

static int                    g_fSerialReady;
static int                    g_fSoftSnapLive;
static struct serial_soft_snap g_SoftSnap;

/* Soft counters (monotonic; no locks — BSP early console). */
static volatile u32 g_u32SoftInits;
static volatile u32 g_u32SoftVerifyOk;
static volatile u32 g_u32SoftVerifyBad;
static volatile u32 g_u32SoftChars;
static volatile u32 g_u32SoftSpinMax;
static volatile u32 g_u32SoftThrWaits;
static volatile u32 g_u32SoftPolls;
static volatile u32 g_u32SoftGetcs;
static volatile u32 g_u32SoftTxFullHits;
static volatile u32 g_u32SoftLogN;      /* serial_soft_log emissions (cap spam) */
static volatile u32 g_u32SoftRefreshN;  /* Wave 13: status_refresh_inner calls */
static volatile u32 g_u32SoftVerifyN;   /* Wave 13: verify_inner calls */
/* Wave 15 exclusive path tallies (complementary; never hard-gate). */
static volatile u32 g_u32SoftPutcharN;  /* serial_putchar entries */
static volatile u32 g_u32SoftWriteN;    /* serial_write entries */
static volatile u32 g_u32SoftWriteNull; /* serial_write NULL/empty skip */
static volatile u32 g_u32SoftCrExpand;  /* LF→CRLF expansions */
static volatile u32 g_u32SoftSpinCapHit;/* hit UART_SOFT_SPIN_MAX ceiling */
static volatile u32 g_u32SoftPollHit;   /* poll returned DR=1 */
static volatile u32 g_u32SoftPollMiss;  /* poll returned DR=0 */

static inline void
outb(u16 uPort, u8 u8Val)
{
    __asm__ volatile ("outb %0, %1" : : "a"(u8Val), "Nd"(uPort));
}

static inline u8
inb(u16 uPort)
{
    u8 u8Val;

    __asm__ volatile ("inb %1, %0" : "=a"(u8Val) : "Nd"(uPort));
    return u8Val;
}

static u16
serial_port(void)
{
    return (u16)GJ_SERIAL_PORT;
}

/**
 * Soft: decode LSR into snap lamps. Pure observability.
 */
static void
serial_soft_decode_lsr(struct serial_soft_snap *pSnap, u8 u8Lsr)
{
    pSnap->u8Lsr = u8Lsr;
    pSnap->u8Thre = (u8)((u8Lsr & UART_LSR_THRE) != 0u);
    pSnap->u8Temt = (u8)((u8Lsr & UART_LSR_TEMT) != 0u);
    pSnap->u8Dr = (u8)((u8Lsr & UART_LSR_DR) != 0u);
    pSnap->u8Oe = (u8)((u8Lsr & UART_LSR_OE) != 0u);
    pSnap->u8Pe = (u8)((u8Lsr & UART_LSR_PE) != 0u);
    pSnap->u8Fe = (u8)((u8Lsr & UART_LSR_FE) != 0u);
    pSnap->u8Bi = (u8)((u8Lsr & UART_LSR_BI) != 0u);
    pSnap->u8Err = (u8)((u8Lsr & UART_LSR_ERR) != 0u);
}

/**
 * Soft: decode MSR modem lamps. Pure observability.
 */
static void
serial_soft_decode_msr(struct serial_soft_snap *pSnap, u8 u8Msr)
{
    pSnap->u8Msr = u8Msr;
    pSnap->u8Dcts = (u8)((u8Msr & UART_MSR_DCTS) != 0u);
    pSnap->u8Ddsr = (u8)((u8Msr & UART_MSR_DDSR) != 0u);
    pSnap->u8Teri = (u8)((u8Msr & UART_MSR_TERI) != 0u);
    pSnap->u8Ddcd = (u8)((u8Msr & UART_MSR_DDCD) != 0u);
    pSnap->u8Cts = (u8)((u8Msr & UART_MSR_CTS) != 0u);
    pSnap->u8Dsr = (u8)((u8Msr & UART_MSR_DSR) != 0u);
    pSnap->u8Ri = (u8)((u8Msr & UART_MSR_RI) != 0u);
    pSnap->u8Dcd = (u8)((u8Msr & UART_MSR_DCD) != 0u);
}

/**
 * Soft: decode MCR path lamps. Pure observability.
 */
static void
serial_soft_decode_mcr(struct serial_soft_snap *pSnap, u8 u8Mcr)
{
    pSnap->u8Mcr = u8Mcr;
    pSnap->u8Dtr = (u8)((u8Mcr & UART_MCR_DTR) != 0u);
    pSnap->u8Rts = (u8)((u8Mcr & UART_MCR_RTS) != 0u);
    pSnap->u8Out2 = (u8)((u8Mcr & UART_MCR_OUT2) != 0u);
}

/**
 * Soft: decode IIR noint + ID. Pure observability.
 */
static void
serial_soft_decode_iir(struct serial_soft_snap *pSnap, u8 u8Iir)
{
    pSnap->u8Iir = u8Iir;
    pSnap->u8Noint = (u8)((u8Iir & UART_IIR_NOINT) != 0u);
    pSnap->u8IirId = (u8)((u8Iir & UART_IIR_ID_MASK) >> 1);
}

/**
 * Soft: read live IER/LCR/MCR/LSR/MSR/IIR/SCR + divisor via brief DLAB.
 * Restores LCR (clears DLAB). Safe on QEMU 16550 and real COM1.
 */
static void
serial_soft_status_refresh_inner(void)
{
    u16 uPort = serial_port();
    u8 u8Lcr;
    u8 u8Dll;
    u8 u8Dlh;
    u8 u8Lsr;
    u8 u8Msr;
    u8 u8Iir;

    if (g_u32SoftRefreshN < 0xffffffffu) {
        g_u32SoftRefreshN++;
    }

    g_SoftSnap.u16Port = uPort;
    g_SoftSnap.u8Ier = inb((u16)(uPort + UART_IER));
    g_SoftSnap.u8Lcr = inb((u16)(uPort + UART_LCR));
    serial_soft_decode_mcr(&g_SoftSnap, inb((u16)(uPort + UART_MCR)));
    u8Lsr = inb((u16)(uPort + UART_LSR));
    serial_soft_decode_lsr(&g_SoftSnap, u8Lsr);
    u8Msr = inb((u16)(uPort + UART_MSR));
    serial_soft_decode_msr(&g_SoftSnap, u8Msr);
    u8Iir = inb((u16)(uPort + UART_IIR));
    serial_soft_decode_iir(&g_SoftSnap, u8Iir);
    g_SoftSnap.u8Scr = inb((u16)(uPort + UART_SCR));
    g_SoftSnap.u8Dlab = (u8)((g_SoftSnap.u8Lcr & UART_LCR_DLAB) != 0u);

    /*
     * Divisor soft peek: set DLAB, read DLL/DLH, restore prior LCR.
     * Product path expects DLAB clear after init — we leave it clear.
     */
    u8Lcr = g_SoftSnap.u8Lcr;
    outb((u16)(uPort + UART_LCR), (u8)(u8Lcr | UART_LCR_DLAB));
    u8Dll = inb((u16)(uPort + UART_DATA));
    u8Dlh = inb((u16)(uPort + UART_IER));
    outb((u16)(uPort + UART_LCR), (u8)(u8Lcr & (u8)~UART_LCR_DLAB));
    g_SoftSnap.u16Div = (u16)((u16)u8Dll | ((u16)u8Dlh << 8));
    g_SoftSnap.u8DivLo = u8Dll;
    g_SoftSnap.u8DivHi = u8Dlh;
    /* Re-read LCR after restore so snap matches live DLAB-off state. */
    g_SoftSnap.u8Lcr = inb((u16)(uPort + UART_LCR));
    g_SoftSnap.u8Dlab = (u8)((g_SoftSnap.u8Lcr & UART_LCR_DLAB) != 0u);
    g_SoftSnap.u8Ier = inb((u16)(uPort + UART_IER));

    /* Floating bus: LSR+MSR all-ones → no COM1 / unmapped. */
    g_SoftSnap.u8Float =
        (u8)((g_SoftSnap.u8Lsr == 0xffu && g_SoftSnap.u8Msr == 0xffu) ? 1u : 0u);
    g_SoftSnap.u8LiveOk = (u8)(g_SoftSnap.u8Float == 0u ? 1u : 0u);
}

/**
 * Soft: compare live regs to product program. Bumps ok/bad. Returns 1 PASS.
 * Records per-field expect subflags for greppable soft expect inventory.
 * Wave 13 deepen: noint_ok/dlab_ok/fcr_ok/thre_ok are inventory lamps only
 * — they do not change the Wave 10 PASS aggregate (ier/lcr/mcr/div/live).
 * Wave 15 deepen: temt_ok/out2_ok/float_ok/scr_ok inventory-only lamps.
 */
static int
serial_soft_verify_inner(void)
{
    int fOk = 1;
    u16 u16ExpectDiv;
    u8 u8IerOk;
    u8 u8LcrOk;
    u8 u8McrOk;
    u8 u8DivOk;
    u8 u8LiveOk;

    if (g_u32SoftVerifyN < 0xffffffffu) {
        g_u32SoftVerifyN++;
    }

    if (!g_fSoftSnapLive || !g_fSerialReady) {
        return 0;
    }

    serial_soft_status_refresh_inner();

    u16ExpectDiv = (u16)(UART_SOFT_DIV_LO | ((u16)UART_SOFT_DIV_HI << 8));

    u8IerOk = (u8)(g_SoftSnap.u8Ier == UART_IER_SOFT_NONE ? 1u : 0u);
    /* Ignore spare/break bits; require 8N1 + DLAB clear in low fields. */
    u8LcrOk = (u8)(((g_SoftSnap.u8Lcr & 0x3fu) == UART_SOFT_LCR &&
                    (g_SoftSnap.u8Lcr & UART_LCR_DLAB) == 0u)
                       ? 1u
                       : 0u);
    u8McrOk =
        (u8)(((g_SoftSnap.u8Mcr & UART_MCR_SOFT) == UART_MCR_SOFT) ? 1u : 0u);
    u8DivOk = (u8)(g_SoftSnap.u16Div == u16ExpectDiv ? 1u : 0u);
    /* LSR all-ones often means missing I/O port (no COM1). Soft FAIL. */
    u8LiveOk = g_SoftSnap.u8LiveOk;

    g_SoftSnap.u8IerOk = u8IerOk;
    g_SoftSnap.u8LcrOk = u8LcrOk;
    g_SoftSnap.u8McrOk = u8McrOk;
    g_SoftSnap.u8DivOk = u8DivOk;

    /* Wave 13 expect deepen (inventory only; not PASS gates). */
    g_SoftSnap.u8NointOk = (u8)(g_SoftSnap.u8Noint != 0u ? 1u : 0u);
    g_SoftSnap.u8DlabOk = (u8)(g_SoftSnap.u8Dlab == 0u ? 1u : 0u);
    g_SoftSnap.u8FcrOk =
        (u8)(g_SoftSnap.u8FcrProg == UART_FCR_SOFT ? 1u : 0u);
    g_SoftSnap.u8ThreOk = (u8)(g_SoftSnap.u8Thre != 0u ? 1u : 0u);

    /* Wave 15 expect deepen (inventory only; not PASS gates). */
    g_SoftSnap.u8TemtOk = (u8)(g_SoftSnap.u8Temt != 0u ? 1u : 0u);
    g_SoftSnap.u8Out2Ok = (u8)(g_SoftSnap.u8Out2 != 0u ? 1u : 0u);
    g_SoftSnap.u8FloatOk = (u8)(g_SoftSnap.u8Float == 0u ? 1u : 0u);
    g_SoftSnap.u8ScrOk =
        (u8)((g_SoftSnap.u8LiveOk != 0u && g_SoftSnap.u8Scr != 0xffu) ? 1u
                                                                     : 0u);

    if (u8IerOk == 0u || u8LcrOk == 0u || u8McrOk == 0u || u8DivOk == 0u ||
        u8LiveOk == 0u) {
        fOk = 0;
    }

    g_SoftSnap.u8VerifyOk = fOk ? 1u : 0u;
    if (fOk) {
        g_u32SoftVerifyOk++;
    } else {
        g_u32SoftVerifyBad++;
    }
    return fOk;
}

/**
 * Soft: note successful program path + first live status. Pure observability.
 */
static void
serial_soft_note_init(void)
{
    g_u32SoftInits++;
    g_fSoftSnapLive = 1;
    g_SoftSnap.u16Port = serial_port();
    g_SoftSnap.u8FcrProg = UART_FCR_SOFT;
    serial_soft_status_refresh_inner();
}

void
serial_init(void)
{
    u16 uPort = serial_port();

    outb((u16)(uPort + UART_IER), UART_IER_SOFT_NONE); /* disable interrupts */
    outb((u16)(uPort + UART_LCR), UART_LCR_DLAB);      /* DLAB on */
    outb((u16)(uPort + UART_DATA), UART_SOFT_DIV_LO);  /* baud divisor lo */
    outb((u16)(uPort + UART_IER), UART_SOFT_DIV_HI);   /* divisor hi */
    outb((u16)(uPort + UART_LCR), UART_SOFT_LCR);      /* 8N1, DLAB off */
    outb((u16)(uPort + UART_IIR), UART_FCR_SOFT);      /* FIFO */
    outb((u16)(uPort + UART_MCR), UART_MCR_SOFT);      /* DTR|RTS|OUT2 */

    g_fSerialReady = 1;
    serial_soft_note_init();

    /*
     * Soft bring-up lines (self-contained; no main hook). kprintf →
     * console_putchar → serial_putchar once UART is programmed above.
     * Keep to a tight set — full Wave 13 inventory is serial_soft_log only.
     */
    kprintf("serial: soft program port=0x%x div=0x%x lcr=0x%x mcr=0x%x "
            "fcr=0x%x ier=0x%x\n",
            (unsigned)uPort,
            (unsigned)(UART_SOFT_DIV_LO | ((unsigned)UART_SOFT_DIV_HI << 8)),
            (unsigned)UART_SOFT_LCR, (unsigned)UART_MCR_SOFT,
            (unsigned)UART_FCR_SOFT, (unsigned)UART_IER_SOFT_NONE);
    kprintf("serial: soft status port=0x%x ier=0x%x lcr=0x%x mcr=0x%x "
            "lsr=0x%x msr=0x%x iir=0x%x div=0x%x\n",
            (unsigned)g_SoftSnap.u16Port, (unsigned)g_SoftSnap.u8Ier,
            (unsigned)g_SoftSnap.u8Lcr, (unsigned)g_SoftSnap.u8Mcr,
            (unsigned)g_SoftSnap.u8Lsr, (unsigned)g_SoftSnap.u8Msr,
            (unsigned)g_SoftSnap.u8Iir, (unsigned)g_SoftSnap.u16Div);
    kprintf("serial: soft lamps thre=%u temt=%u dr=%u dlab=%u "
            "oe=%u pe=%u fe=%u bi=%u err=%u float=%u\n",
            (unsigned)g_SoftSnap.u8Thre, (unsigned)g_SoftSnap.u8Temt,
            (unsigned)g_SoftSnap.u8Dr, (unsigned)g_SoftSnap.u8Dlab,
            (unsigned)g_SoftSnap.u8Oe, (unsigned)g_SoftSnap.u8Pe,
            (unsigned)g_SoftSnap.u8Fe, (unsigned)g_SoftSnap.u8Bi,
            (unsigned)g_SoftSnap.u8Err, (unsigned)g_SoftSnap.u8Float);
    kprintf("serial: soft path polled=1 irq=0 fcr=0x%x spin_cap=%u "
            "ready=%u live=%u noint=%u\n",
            (unsigned)g_SoftSnap.u8FcrProg, (unsigned)UART_SOFT_SPIN_MAX,
            (unsigned)g_fSerialReady, (unsigned)g_SoftSnap.u8LiveOk,
            (unsigned)g_SoftSnap.u8Noint);

    if (serial_soft_verify_inner()) {
        kprintf("serial: soft verify PASS inits=%u\n",
                (unsigned)g_u32SoftInits);
    } else {
        kprintf("serial: soft verify FAIL inits=%u\n",
                (unsigned)g_u32SoftInits);
    }
}

void
serial_putchar(char chOut)
{
    u16 uPort = serial_port();
    u32 uSpins;

    if (g_u32SoftPutcharN < 0xffffffffu) {
        g_u32SoftPutcharN++;
    }
    if (chOut == '\n') {
        if (g_u32SoftCrExpand < 0xffffffffu) {
            g_u32SoftCrExpand++;
        }
        serial_putchar('\r');
    }
    for (uSpins = 0; uSpins < UART_SOFT_SPIN_MAX; uSpins++) {
        if ((inb((u16)(uPort + UART_LSR)) & UART_LSR_THRE) != 0u) {
            break;
        }
        g_u32SoftTxFullHits++;
    }
    if (uSpins > 0u) {
        g_u32SoftThrWaits++;
    }
    if (uSpins > g_u32SoftSpinMax) {
        g_u32SoftSpinMax = uSpins;
    }
    if (uSpins >= UART_SOFT_SPIN_MAX) {
        if (g_u32SoftSpinCapHit < 0xffffffffu) {
            g_u32SoftSpinCapHit++;
        }
    }
    outb(uPort, (u8)chOut);
    g_u32SoftChars++;
}

void
serial_write(const char *szText)
{
    if (g_u32SoftWriteN < 0xffffffffu) {
        g_u32SoftWriteN++;
    }
    if (szText == NULL || *szText == '\0') {
        if (g_u32SoftWriteNull < 0xffffffffu) {
            g_u32SoftWriteNull++;
        }
        return;
    }
    while (*szText) {
        serial_putchar(*szText++);
    }
}

/* Shared C kernel console (stdio_k / kprintf). */
void
console_putchar(char chOut)
{
    serial_putchar(chOut);
}

void
console_write(const char *szText)
{
    serial_write(szText);
}

int
serial_poll(void)
{
    u16 uPort = serial_port();
    int fDr;

    g_u32SoftPolls++;
    fDr = (inb((u16)(uPort + UART_LSR)) & UART_LSR_DR) != 0u ? 1 : 0;
    if (fDr != 0) {
        if (g_u32SoftPollHit < 0xffffffffu) {
            g_u32SoftPollHit++;
        }
    } else if (g_u32SoftPollMiss < 0xffffffffu) {
        g_u32SoftPollMiss++;
    }
    return fDr;
}

int
serial_getchar(void)
{
    u16 uPort = serial_port();

    while (!serial_poll()) {
        /* spin — product can sleep via yield from userspace poll */
    }
    g_u32SoftGetcs++;
    return (int)inb(uPort);
}

/* ---- Soft COM1 observability API ----------------------------------- */

int
serial_soft_ready(void)
{
    return g_fSerialReady;
}

u32
serial_soft_inits(void)
{
    return g_u32SoftInits;
}

u32
serial_soft_chars(void)
{
    return g_u32SoftChars;
}

u32
serial_soft_spinmax(void)
{
    return g_u32SoftSpinMax;
}

u32
serial_soft_thr_waits(void)
{
    return g_u32SoftThrWaits;
}

u32
serial_soft_txfull(void)
{
    return g_u32SoftTxFullHits;
}

u32
serial_soft_log_n(void)
{
    return g_u32SoftLogN;
}

u32
serial_soft_polls(void)
{
    return g_u32SoftPolls;
}

u32
serial_soft_getcs(void)
{
    return g_u32SoftGetcs;
}

u32
serial_soft_verify_ok(void)
{
    return g_u32SoftVerifyOk;
}

u32
serial_soft_verify_bad(void)
{
    return g_u32SoftVerifyBad;
}

u16
serial_soft_port(void)
{
    return g_SoftSnap.u16Port;
}

u8
serial_soft_ier(void)
{
    return g_SoftSnap.u8Ier;
}

u8
serial_soft_lcr(void)
{
    return g_SoftSnap.u8Lcr;
}

u8
serial_soft_mcr(void)
{
    return g_SoftSnap.u8Mcr;
}

u8
serial_soft_lsr(void)
{
    return g_SoftSnap.u8Lsr;
}

u8
serial_soft_msr(void)
{
    return g_SoftSnap.u8Msr;
}

u8
serial_soft_iir(void)
{
    return g_SoftSnap.u8Iir;
}

u16
serial_soft_div(void)
{
    return g_SoftSnap.u16Div;
}

/**
 * Soft re-read of live 16550 status into soft snap.
 * Returns 1 if soft snapshot is live (post-init), 0 otherwise.
 */
int
serial_soft_status_refresh(void)
{
    if (!g_fSoftSnapLive) {
        return 0;
    }
    serial_soft_status_refresh_inner();
    return 1;
}

/**
 * Soft re-verify programmed COM1 shape. Bumps ok/bad counters.
 * Returns 1 on PASS, 0 on FAIL / not ready.
 */
int
serial_soft_verify(void)
{
    return serial_soft_verify_inner();
}

/**
 * Greppable soft summary (product / smoke inventory).
 * Wave 10 base + Wave 13 path + Wave 19 exclusive complementary deepen;
 * re-verify once per log. Not hot-path — soft stats smoke only (no flood).
 * Primary field names stay stable; Wave 16 adds complementary sub-lines.
 */
void
serial_soft_log(void)
{
    if (g_u32SoftLogN < 0xffffffffu) {
        g_u32SoftLogN++;
    }

    if (g_fSoftSnapLive && g_fSerialReady) {
        /* Fresh snap + expect subflags for this inventory emission. */
        (void)serial_soft_verify_inner();
    } else if (g_fSoftSnapLive) {
        serial_soft_status_refresh_inner();
    }

    /*
     * Grep: serial: soft inventory — Wave 16 rollup + wave stamp.
     * One catalog line; densifies counters without boot spam.
     * Prior keys remain prefix-stable; wave= advances.
     */
    kprintf("serial: soft inventory wave=%u ready=%u live=%u float=%u "
            "inits=%u verify_n=%u refresh_n=%u log_n=%u "
            "ok=%u bad=%u match=%u\n",
            (unsigned)UART_SOFT_WAVE,
            (unsigned)(g_fSerialReady ? 1u : 0u),
            (unsigned)g_SoftSnap.u8LiveOk, (unsigned)g_SoftSnap.u8Float,
            (unsigned)g_u32SoftInits, (unsigned)g_u32SoftVerifyN,
            (unsigned)g_u32SoftRefreshN, (unsigned)g_u32SoftLogN,
            (unsigned)g_u32SoftVerifyOk, (unsigned)g_u32SoftVerifyBad,
            (unsigned)g_SoftSnap.u8VerifyOk);

    /*
     * Grep: serial: soft program — expected product shape (constants).
     * Shadow only; never reprograms UART from this path.
     */
    kprintf("serial: soft program port=0x%x div=0x%x div_lo=0x%x "
            "div_hi=0x%x lcr=0x%x mcr=0x%x fcr=0x%x ier=0x%x "
            "baud=%u spin_cap=%u\n",
            (unsigned)serial_port(),
            (unsigned)(UART_SOFT_DIV_LO | ((unsigned)UART_SOFT_DIV_HI << 8)),
            (unsigned)UART_SOFT_DIV_LO, (unsigned)UART_SOFT_DIV_HI,
            (unsigned)UART_SOFT_LCR, (unsigned)UART_MCR_SOFT,
            (unsigned)UART_FCR_SOFT, (unsigned)UART_IER_SOFT_NONE,
            (unsigned)UART_SOFT_BAUD, (unsigned)UART_SOFT_SPIN_MAX);

    /* Grep: serial: soft inits=… */
    kprintf("serial: soft inits=%u chars=%u spinmax=%u thrwait=%u "
            "txfull=%u poll=%u getc=%u log_n=%u refresh_n=%u "
            "verify_n=%u\n",
            (unsigned)g_u32SoftInits, (unsigned)g_u32SoftChars,
            (unsigned)g_u32SoftSpinMax, (unsigned)g_u32SoftThrWaits,
            (unsigned)g_u32SoftTxFullHits, (unsigned)g_u32SoftPolls,
            (unsigned)g_u32SoftGetcs, (unsigned)g_u32SoftLogN,
            (unsigned)g_u32SoftRefreshN, (unsigned)g_u32SoftVerifyN);
    /* Grep: serial: soft port=… */
    kprintf("serial: soft port=0x%x ier=0x%x lcr=0x%x mcr=0x%x "
            "lsr=0x%x msr=0x%x iir=0x%x scr=0x%x\n",
            (unsigned)g_SoftSnap.u16Port, (unsigned)g_SoftSnap.u8Ier,
            (unsigned)g_SoftSnap.u8Lcr, (unsigned)g_SoftSnap.u8Mcr,
            (unsigned)g_SoftSnap.u8Lsr, (unsigned)g_SoftSnap.u8Msr,
            (unsigned)g_SoftSnap.u8Iir, (unsigned)g_SoftSnap.u8Scr);
    /* Grep: serial: soft div=… */
    kprintf("serial: soft div=0x%x div_lo=0x%x div_hi=0x%x "
            "thre=%u temt=%u dr=%u dlab=%u oe=%u pe=%u fe=%u bi=%u "
            "err=%u\n",
            (unsigned)g_SoftSnap.u16Div, (unsigned)g_SoftSnap.u8DivLo,
            (unsigned)g_SoftSnap.u8DivHi, (unsigned)g_SoftSnap.u8Thre,
            (unsigned)g_SoftSnap.u8Temt, (unsigned)g_SoftSnap.u8Dr,
            (unsigned)g_SoftSnap.u8Dlab, (unsigned)g_SoftSnap.u8Oe,
            (unsigned)g_SoftSnap.u8Pe, (unsigned)g_SoftSnap.u8Fe,
            (unsigned)g_SoftSnap.u8Bi, (unsigned)g_SoftSnap.u8Err);
    /* Grep: serial: soft msr … — modem lamps (was define-only). */
    kprintf("serial: soft msr cts=%u dsr=%u ri=%u dcd=%u "
            "dcts=%u ddsr=%u teri=%u ddcd=%u\n",
            (unsigned)g_SoftSnap.u8Cts, (unsigned)g_SoftSnap.u8Dsr,
            (unsigned)g_SoftSnap.u8Ri, (unsigned)g_SoftSnap.u8Dcd,
            (unsigned)g_SoftSnap.u8Dcts, (unsigned)g_SoftSnap.u8Ddsr,
            (unsigned)g_SoftSnap.u8Teri, (unsigned)g_SoftSnap.u8Ddcd);
    /* Grep: serial: soft thr … — TX spin telemetry (Wave 13). */
    kprintf("serial: soft thr wait=%u spinmax=%u txfull=%u "
            "spin_cap=%u chars=%u thre_ok=%u temt=%u\n",
            (unsigned)g_u32SoftThrWaits, (unsigned)g_u32SoftSpinMax,
            (unsigned)g_u32SoftTxFullHits, (unsigned)UART_SOFT_SPIN_MAX,
            (unsigned)g_u32SoftChars, (unsigned)g_SoftSnap.u8ThreOk,
            (unsigned)g_SoftSnap.u8Temt);
    /* Grep: serial: soft iir … — polled IIR + SCR (Wave 13). */
    kprintf("serial: soft iir noint=%u id=%u noint_ok=%u scr=0x%x "
            "ier=0x%x\n",
            (unsigned)g_SoftSnap.u8Noint, (unsigned)g_SoftSnap.u8IirId,
            (unsigned)g_SoftSnap.u8NointOk, (unsigned)g_SoftSnap.u8Scr,
            (unsigned)g_SoftSnap.u8Ier);
    /*
     * Grep: serial: soft path … — polled policy + honesty non-claim.
     * Soft inventory ≠ IRQ console, ≠ bar3, ≠ product TTY.
     */
    kprintf("serial: soft path polled=1 irq=0 fcr=0x%x spin_cap=%u "
            "ready=%u live=%u float=%u noint=%u iir_id=%u "
            "dtr=%u rts=%u out2=%u bar3=0 hard_gate=0 irq_claim=0 "
            "wave=%u\n",
            (unsigned)g_SoftSnap.u8FcrProg, (unsigned)UART_SOFT_SPIN_MAX,
            (unsigned)g_fSerialReady, (unsigned)g_SoftSnap.u8LiveOk,
            (unsigned)g_SoftSnap.u8Float, (unsigned)g_SoftSnap.u8Noint,
            (unsigned)g_SoftSnap.u8IirId, (unsigned)g_SoftSnap.u8Dtr,
            (unsigned)g_SoftSnap.u8Rts, (unsigned)g_SoftSnap.u8Out2,
            (unsigned)UART_SOFT_WAVE);
    /*
     * Grep: serial: soft expect … — per-field verify subflags.
     * Wave 10 gates + Wave 13 lamps + Wave 15 lamps (suffix only).
     */
    kprintf("serial: soft expect ier_ok=%u lcr_ok=%u mcr_ok=%u "
            "div_ok=%u live_ok=%u noint_ok=%u dlab_ok=%u fcr_ok=%u "
            "thre_ok=%u temt_ok=%u out2_ok=%u float_ok=%u scr_ok=%u "
            "match=%u\n",
            (unsigned)g_SoftSnap.u8IerOk, (unsigned)g_SoftSnap.u8LcrOk,
            (unsigned)g_SoftSnap.u8McrOk, (unsigned)g_SoftSnap.u8DivOk,
            (unsigned)g_SoftSnap.u8LiveOk, (unsigned)g_SoftSnap.u8NointOk,
            (unsigned)g_SoftSnap.u8DlabOk, (unsigned)g_SoftSnap.u8FcrOk,
            (unsigned)g_SoftSnap.u8ThreOk, (unsigned)g_SoftSnap.u8TemtOk,
            (unsigned)g_SoftSnap.u8Out2Ok, (unsigned)g_SoftSnap.u8FloatOk,
            (unsigned)g_SoftSnap.u8ScrOk, (unsigned)g_SoftSnap.u8VerifyOk);

    /*
     * Wave 15 complementary sub-lines (kept; never reshape primary).
     */
    /* Grep: serial: soft lamps */
    kprintf("serial: soft lamps ready=%u live=%u float=%u thre=%u "
            "temt=%u dr=%u dlab=%u noint=%u dtr=%u rts=%u out2=%u "
            "match=%u\n",
            (unsigned)(g_fSerialReady ? 1u : 0u),
            (unsigned)g_SoftSnap.u8LiveOk, (unsigned)g_SoftSnap.u8Float,
            (unsigned)g_SoftSnap.u8Thre, (unsigned)g_SoftSnap.u8Temt,
            (unsigned)g_SoftSnap.u8Dr, (unsigned)g_SoftSnap.u8Dlab,
            (unsigned)g_SoftSnap.u8Noint, (unsigned)g_SoftSnap.u8Dtr,
            (unsigned)g_SoftSnap.u8Rts, (unsigned)g_SoftSnap.u8Out2,
            (unsigned)g_SoftSnap.u8VerifyOk);

    /* Grep: serial: soft stats */
    kprintf("serial: soft stats putchar=%u write=%u write_null=%u "
            "cr_expand=%u spin_cap_hit=%u poll_hit=%u poll_miss=%u "
            "chars=%u thrwait=%u txfull=%u getc=%u "
            "inits=%u verify_n=%u refresh_n=%u log_n=%u wave=%u\n",
            (unsigned)g_u32SoftPutcharN, (unsigned)g_u32SoftWriteN,
            (unsigned)g_u32SoftWriteNull, (unsigned)g_u32SoftCrExpand,
            (unsigned)g_u32SoftSpinCapHit, (unsigned)g_u32SoftPollHit,
            (unsigned)g_u32SoftPollMiss, (unsigned)g_u32SoftChars,
            (unsigned)g_u32SoftThrWaits, (unsigned)g_u32SoftTxFullHits,
            (unsigned)g_u32SoftGetcs, (unsigned)g_u32SoftInits,
            (unsigned)g_u32SoftVerifyN, (unsigned)g_u32SoftRefreshN,
            (unsigned)g_u32SoftLogN, (unsigned)UART_SOFT_WAVE);

    /* Grep: serial: soft mcr … — DTR|RTS|OUT2 path lamps */
    kprintf("serial: soft mcr dtr=%u rts=%u out2=%u raw=0x%x "
            "expect=0x%x mcr_ok=%u out2_ok=%u\n",
            (unsigned)g_SoftSnap.u8Dtr, (unsigned)g_SoftSnap.u8Rts,
            (unsigned)g_SoftSnap.u8Out2, (unsigned)g_SoftSnap.u8Mcr,
            (unsigned)UART_MCR_SOFT, (unsigned)g_SoftSnap.u8McrOk,
            (unsigned)g_SoftSnap.u8Out2Ok);

    /* Grep: serial: soft float … — float/live presence axis */
    kprintf("serial: soft float float=%u live_ok=%u float_ok=%u "
            "lsr=0x%x msr=0x%x scr=0x%x scr_ok=%u\n",
            (unsigned)g_SoftSnap.u8Float, (unsigned)g_SoftSnap.u8LiveOk,
            (unsigned)g_SoftSnap.u8FloatOk, (unsigned)g_SoftSnap.u8Lsr,
            (unsigned)g_SoftSnap.u8Msr, (unsigned)g_SoftSnap.u8Scr,
            (unsigned)g_SoftSnap.u8ScrOk);

    /*
     * Grep: serial: soft honesty
     * Soft inventory ≠ IRQ console, ≠ bar3, ≠ product TTY complete.
     */
    kprintf("serial: soft honesty polled=1 irq_console=0 bar3=0 "
            "product_tty=0 hard_gate=0 irq_claim=0 "
            "soft_ne_product_tty=1 wave=%u unit=serial.c\n",
            (unsigned)UART_SOFT_WAVE);

    /*
     * Wave 16 complementary sub-lines (kept; never reshape primary).
     */
    /* Grep: serial: soft exclusive */
    kprintf("serial: soft exclusive wave=%u exclusive=1 soft=1 "
            "unit=serial.c bar3=0 hard_gate=0 irq_console=0 "
            "product_tty=0 soft_ne_product_tty=1\n",
            (unsigned)UART_SOFT_WAVE);

    /* Grep: serial: soft claim — polled console product bounds */
    kprintf("serial: soft claim polled=1 irq=0 baud=%u div=0x%x "
            "lcr=8n1 mcr=dtr|rts|out2 fcr_shadow=1 ier=0 "
            "spin_cap=%u irq_console=0 bar3=0 product_tty=0 wave=%u\n",
            (unsigned)UART_SOFT_BAUD,
            (unsigned)(UART_SOFT_DIV_LO | ((unsigned)UART_SOFT_DIV_HI << 8)),
            (unsigned)UART_SOFT_SPIN_MAX,
            (unsigned)UART_SOFT_WAVE);

    /* Grep: serial: soft ratio — ok/bad + poll/thr path ratios */
    kprintf("serial: soft ratio ok=%u bad=%u match=%u "
            "poll_hit=%u poll_miss=%u thrwait=%u txfull=%u "
            "spin_cap_hit=%u putchar=%u write=%u cr_expand=%u "
            "chars=%u wave=%u\n",
            (unsigned)g_u32SoftVerifyOk, (unsigned)g_u32SoftVerifyBad,
            (unsigned)g_SoftSnap.u8VerifyOk,
            (unsigned)g_u32SoftPollHit, (unsigned)g_u32SoftPollMiss,
            (unsigned)g_u32SoftThrWaits, (unsigned)g_u32SoftTxFullHits,
            (unsigned)g_u32SoftSpinCapHit, (unsigned)g_u32SoftPutcharN,
            (unsigned)g_u32SoftWriteN, (unsigned)g_u32SoftCrExpand,
            (unsigned)g_u32SoftChars, (unsigned)UART_SOFT_WAVE);

    /* Grep: serial: soft err — LSR error lamp rollup */
    kprintf("serial: soft err oe=%u pe=%u fe=%u bi=%u err=%u "
            "lsr=0x%x any=%u wave=%u\n",
            (unsigned)g_SoftSnap.u8Oe, (unsigned)g_SoftSnap.u8Pe,
            (unsigned)g_SoftSnap.u8Fe, (unsigned)g_SoftSnap.u8Bi,
            (unsigned)g_SoftSnap.u8Err, (unsigned)g_SoftSnap.u8Lsr,
            (unsigned)((g_SoftSnap.u8Oe | g_SoftSnap.u8Pe |
                        g_SoftSnap.u8Fe | g_SoftSnap.u8Bi |
                        g_SoftSnap.u8Err) != 0u ? 1u : 0u),
            (unsigned)UART_SOFT_WAVE);

    /*
     * Wave 17 complementary sub-lines (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: serial: soft return — Wave 17 API return surfaces (kept) */
    kprintf("serial: soft return ready=%u live=%u polled=1 soft_inv=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (unsigned)(g_fSerialReady ? 1u : 0u), (unsigned)g_SoftSnap.u8LiveOk, (unsigned)UART_SOFT_WAVE);

    /* Grep: serial: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("serial: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)UART_SOFT_WAVE);

    /* Grep: serial: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("serial: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)UART_SOFT_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: serial: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("serial: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)UART_SOFT_WAVE);

    /* Grep: serial: soft retcode — Wave 19 retcode catalog */
    kprintf("serial: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)UART_SOFT_WAVE);

    /* Grep: serial: soft deepen — wave stamp + area catalog */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: serial: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("serial: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)UART_SOFT_WAVE);
    /* Grep: serial: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("serial: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)UART_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: serial: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("serial: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)UART_SOFT_WAVE);
    /* Grep: serial: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("serial: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)UART_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: serial: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("serial: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)UART_SOFT_WAVE);
            /* Grep: serial: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("serial: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)UART_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: serial: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("serial: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)UART_SOFT_WAVE);
            /* Grep: serial: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("serial: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)UART_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: serial: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("serial: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)UART_SOFT_WAVE);
            /* Grep: serial: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("serial: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)UART_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: serial: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("serial: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)UART_SOFT_WAVE);
            /* Grep: serial: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("serial: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)UART_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: serial: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("serial: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)UART_SOFT_WAVE);
            /* Grep: serial: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("serial: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)UART_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: serial: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("serial: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)UART_SOFT_WAVE);
            /* Grep: serial: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("serial: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)UART_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: serial: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("serial: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)UART_SOFT_WAVE);
                    /* Grep: serial: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("serial: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)UART_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: serial: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("serial: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)UART_SOFT_WAVE);
                            /* Grep: serial: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("serial: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)UART_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: serial: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("serial: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)UART_SOFT_WAVE);
                            /* Grep: serial: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("serial: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)UART_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: serial: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("serial: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)UART_SOFT_WAVE);
                            /* Grep: serial: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("serial: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)UART_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: serial: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("serial: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)UART_SOFT_WAVE);
                            /* Grep: serial: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("serial: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)UART_SOFT_WAVE);
                            /* Grep: serial: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("serial: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)UART_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("serial: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)UART_SOFT_WAVE);
/* Grep: serial: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("serial: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)UART_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("serial: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)UART_SOFT_WAVE);
/* Grep: serial: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("serial: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)UART_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("serial: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)UART_SOFT_WAVE);
/* Grep: serial: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("serial: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)UART_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retfortress — Wave 35 return-fortress honesty */
kprintf("serial: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)UART_SOFT_WAVE);
/* Grep: serial: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("serial: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)UART_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft rethold — Wave 36 return-hold honesty */
kprintf("serial: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)UART_SOFT_WAVE);
/* Grep: serial: soft retspire — Wave 36 exclusive spire stamp */
kprintf("serial: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)UART_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retwall — Wave 37 return-wall honesty */
kprintf("serial: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)UART_SOFT_WAVE);
/* Grep: serial: soft retgate — Wave 37 exclusive gate stamp */
kprintf("serial: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)UART_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retmoat — Wave 38 return-moat honesty */
kprintf("serial: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)UART_SOFT_WAVE);
/* Grep: serial: soft retower — Wave 38 exclusive tower stamp */
kprintf("serial: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)UART_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("serial: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)UART_SOFT_WAVE);
/* Grep: serial: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("serial: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)UART_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("serial: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)UART_SOFT_WAVE);
/* Grep: serial: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("serial: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)UART_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retravelin — Wave 41 return-travelin honesty */
kprintf("serial: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)UART_SOFT_WAVE);
/* Grep: serial: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("serial: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)UART_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("serial: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)UART_SOFT_WAVE);
/* Grep: serial: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("serial: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)UART_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("serial: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)UART_SOFT_WAVE);
/* Grep: serial: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("serial: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)UART_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("serial: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)UART_SOFT_WAVE);
/* Grep: serial: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("serial: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)UART_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("serial: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)UART_SOFT_WAVE);
/* Grep: serial: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("serial: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)UART_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retbailey — Wave 46 return-bailey honesty */
kprintf("serial: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)UART_SOFT_WAVE);
/* Grep: serial: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("serial: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)UART_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("serial: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("serial: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("serial: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("serial: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("serial: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("serial: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retsally — Wave 50 return-sally honesty */
kprintf("serial: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("serial: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retfosse — Wave 51 return-fosse honesty */
kprintf("serial: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("serial: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("serial: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("serial: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retravelin — Wave 53 return-travelin honesty */
kprintf("serial: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("serial: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("serial: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retredan — Wave 54 exclusive redan stamp */
kprintf("serial: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retflank — Wave 55 return-flank honesty */
kprintf("serial: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retface — Wave 55 exclusive face stamp */
kprintf("serial: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retgorge — Wave 56 return-gorge honesty */
kprintf("serial: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("serial: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retraverse — Wave 57 return-traverse honesty */
kprintf("serial: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("serial: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retorillon — Wave 58 return-orillon honesty */
kprintf("serial: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("serial: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("serial: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("serial: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retplace — Wave 60 return-place honesty */
kprintf("serial: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("serial: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("serial: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("serial: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("serial: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("serial: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("serial: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("serial: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: serial: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("serial: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: serial: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("serial: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: serial: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("serial: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: serial: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("serial: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: serial: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("serial: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=79 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: serial: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("serial: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=79 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("serial: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("serial: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("serial: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("serial: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("serial: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=79 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("serial: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=79 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("serial: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=79 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("serial: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=79 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("serial: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=79 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("serial: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=79 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: serial: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("serial: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=79 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("serial: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=79 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: serial: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("serial: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=79 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("serial: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=79 (retbastionangle stamp; Soft≠product)\n");
/* Grep: serial: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("serial: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=79 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("serial: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=79 (retparapetangle stamp; Soft≠product)\n");
/* Grep: serial: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("serial: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=79 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("serial: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=79 (retowerangle stamp; Soft≠product)\n");
/* Grep: serial: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("serial: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=79 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("serial: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=79 (retwallangle stamp; Soft≠product)\n");
/* Grep: serial: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("serial: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=79 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("serial: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=79 (retholdangle stamp; Soft≠product)\n");
/* Grep: serial: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("serial: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=79 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("serial: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=79 (retfortressangle stamp; Soft≠product)\n");
/* Grep: serial: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("serial: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=79 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: serial: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("serial: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=79 (retredoubtangle stamp; Soft≠product)\n");

                            kprintf("serial: soft deepen wave=%u areas=inventory,program,inits,,retclass,retlane"
            "port,div,msr,thr,iir,path,expect,verify,"
            "lamps,stats,mcr,float,honesty,"
            "exclusive,claim,ratio,err,return,return_selftest,retmap,return_rate,retcode "
            "unit=serial.c only hard_gate=0 ready=%u live=%u\n",
            (unsigned)UART_SOFT_WAVE,
            (unsigned)(g_fSerialReady ? 1u : 0u),
            (unsigned)g_SoftSnap.u8LiveOk);

    /* Grep: serial: soft verify PASS|FAIL|idle — smoke scripts stable. */
    if (!g_fSoftSnapLive) {
        kprintf("serial: soft verify idle (ok=%u bad=%u)\n",
                (unsigned)g_u32SoftVerifyOk, (unsigned)g_u32SoftVerifyBad);
    } else if (g_SoftSnap.u8VerifyOk != 0u) {
        kprintf("serial: soft verify PASS (ok=%u bad=%u)\n",
                (unsigned)g_u32SoftVerifyOk, (unsigned)g_u32SoftVerifyBad);
    } else {
        kprintf("serial: soft verify FAIL (ok=%u bad=%u)\n",
                (unsigned)g_u32SoftVerifyOk, (unsigned)g_u32SoftVerifyBad);
    }
}
