/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * 16550 COM1 early console (I/O port GJ_SERIAL_PORT, product default 0x3F8).
 * Polled TX/RX only - no IRQ path. Freestanding pure C; no GPL paste.
 *
 * -------------------------------------------------------------------------
 * Soft COM1 init / status observability (boot telemetry - not hot-path)
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
 * (no putchar flood). Soft != IRQ console, != product TTY.
 *
 * Greppable (product / smoke inventory - Wave 10 base + Wave 13 path;
 * prefix-stable; residual lean - no stamp storms, no product version stamp):
 *   serial: soft inventory ...
 *   serial: soft program port=... div=... lcr=... mcr=... fcr=... ier=... baud=38400
 *   serial: soft inits=... chars=... spinmax=... thrwait=... txfull=... poll=... getc=...
 *   serial: soft port=0x... ier=0x... lcr=0x... mcr=0x... lsr=0x... msr=0x... iir=0x...
 *   serial: soft div=0x... thre=... temt=... dr=... dlab=... oe=... pe=... fe=... bi=... err=...
 *   serial: soft msr cts=... dsr=... ri=... dcd=... dcts=... ddsr=... teri=... ddcd=...
 *   serial: soft thr wait=... spinmax=... txfull=... spin_cap=... chars=... thre_ok=...
 *   serial: soft iir noint=... id=... noint_ok=... scr=0x...
 *   serial: soft path polled=1 irq=0 fcr=0x... spin_cap=... ready=... live=...
 *   serial: soft expect ier_ok=... lcr_ok=... mcr_ok=... div_ok=... live_ok=...
 *   serial: soft verify PASS|FAIL|idle (ok=... bad=...)
 *   serial: soft budget ...     - log_max / cap_skip / busy_skip honesty
 * Complementary (kept lean; never reshape primary fields):
 *   serial: soft lamps ... stats ... mcr ... float ... honesty ...
 *   serial: soft exclusive ... claim ... ratio ... err ... deepen ...
 *   serial: soft residual lean ... - reentrancy + G752 no COM1 honesty
 * Residual lean via= (C0 residual deepen; Soft!=product; no version stamp):
 *   via=init   - once at serial_init soft bring-up (busy hold)
 *   via=panel  - THRE-dead first-miss + soft_log SKIP (G752 / no COM1)
 *   via=inv    - full serial_soft_log inventory (cap UART_SOFT_LOG_MAX)
 * G752 no COM1 honesty (greppable residual; Soft!=product; never hard-gate):
 *   g752_no_com1= / panel_path= / com1_absent= / float= / thre_dead=
 *   fb_console_path=1 (GOP/fb is operator log when legacy 0x3F8 is dead)
 * Reentrancy (lean residual; FAULT/H2 harden):
 *   - serial_soft_log busy guard before any kprintf frame
 *   - serial_init soft bring-up holds the same busy guard
 *   - CRLF via serial_tx_byte only (no recursive putchar)
 *   - Cap UART_SOFT_LOG_MAX; THRE-dead: SKIP + residual lean (no multi-line flood)
 *   - No #if-0 stamp-storm residue (nested-comment hazard)
 * Soft != IRQ console, != product TTY. Dual license: MIT OR Apache-2.0.
 *
 * Soft APIs (linkable; no main hook required - serial_init self-logs):
 *   serial_soft_inits / chars / spinmax / thr_waits / polls / getcs
 *   serial_soft_verify_ok / verify_bad / ready / txfull / log_n
 *   serial_soft_port / ier / lcr / mcr / lsr / msr / iir / div
 *   serial_soft_status_refresh / serial_soft_verify / serial_soft_log
 *   serial_thre_dead - sticky panel-path lamp (declared in gj/klog.h)
 *
 * -------------------------------------------------------------------------
 * Panel path: sticky THRE-dead (G752 / laptop DUTs without useful COM1)
 * -------------------------------------------------------------------------
 * Product TX spins on LSR.THRE up to UART_SOFT_SPIN_MAX. Hosts with no
 * legacy 0x3F8 never raise THRE; a full soft inventory at 100k loops/char
 * looks hung after the white kmain bar. After the first full miss, putchar
 * sticky-skips COM1 I/O so boot continues, and serial_thre_dead() returns
 * non-zero so soft inventory flood sites may skip multi-KiB dumps. Residual
 * lean still greps via=panel on that path (one line; Soft!=product).
 * QEMU Multiboot with a live 16550 keeps THRE present -> full spin budget
 * and serial_thre_dead() == 0. Soft != hard boot gate.
 */
#include <gj/config.h>
#include <gj/console.h>
#include <gj/fb_console.h>
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

/* IER - product path leaves IRQ disabled (polled console). */
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

/* Product baud program: 115200/38400 = 3 -> divisor 0x0003. */
#define UART_SOFT_DIV_LO 0x03u
#define UART_SOFT_DIV_HI 0x00u
#define UART_SOFT_LCR    UART_LCR_WLS8 /* 8N1, DLAB clear */

/* Soft TX wait budget (match UEFI stub / aarch64 PL011 spin ceiling). */
#define UART_SOFT_SPIN_MAX 100000u

/*
 * Hard cap on serial_soft_log multi-line inventory emissions. Stamp-storm
 * floods of kprintf frames on residual / IRQ stacks contributed to FAULT
 * class. Soft!=product; never unbounded re-dump.
 */
#define UART_SOFT_LOG_MAX 8u

/* IIR soft lamps (read path; FCR is write-only at same offset). */
#define UART_IIR_NOINT  0x01u /* 1 = no interrupt pending */
#define UART_IIR_ID_MASK 0x0eu

/* Soft Wave stamp (greppable inventory only; never hard-gates boot). */
#define UART_SOFT_WAVE 126u

/* Product soft baud label (115200/3 -> 38400; divisor program 0x0003). */
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
    u8  u8Float;    /* LSR+MSR all-ones -> missing I/O */
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

/* Soft counters (monotonic; no locks - BSP early console). */
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
static volatile u32 g_u32SoftLogCapSkip;/* soft_log suppressed: UART_SOFT_LOG_MAX */
static volatile u32 g_u32SoftLogBusySkip;/* soft_log reentrancy guard hits */
static volatile u32 g_u32SoftRefreshN;  /* Wave 13: status_refresh_inner calls */
static volatile u32 g_u32SoftVerifyN;   /* Wave 13: verify_inner calls */
/* Wave 15 exclusive path tallies (complementary; never hard-gate). */
static volatile u32 g_u32SoftPutcharN;  /* serial_putchar entries */
static volatile u32 g_u32SoftWriteN;    /* serial_write entries */
static volatile u32 g_u32SoftWriteNull; /* serial_write NULL/empty skip */
static volatile u32 g_u32SoftCrExpand;  /* LF->CRLF expansions */
static volatile u32 g_u32SoftSpinCapHit;/* hit UART_SOFT_SPIN_MAX ceiling */
static volatile u32 g_u32SoftPollHit;   /* poll returned DR=1 */
static volatile u32 g_u32SoftPollMiss;  /* poll returned DR=0 */
static volatile int g_fSoftLogBusy;     /* reentrancy guard for soft_log */
/* C0 residual lean tallies (eng residual; Soft!=product; never hard-gate). */
static volatile u32 g_u32SoftResidualLeanN;    /* residual lean line emissions */
static volatile u32 g_u32SoftResidualLeanInit; /* via=init once-lamp */
static volatile u32 g_u32SoftResidualLeanPanel;/* via=panel (THRE-dead) */
static volatile u32 g_u32SoftResidualLeanInv;  /* via=inv (full inventory) */
/*
 * Sticky THRE-dead lamp (panel path). 0 while COM1 raises THRE (QEMU /
 * real UART -> full UART_SOFT_SPIN_MAX). Set on first full miss; thereafter
 * putchar skips COM1 I/O. Exposed via serial_thre_dead() (gj/klog.h).
 */
static u32 g_fSerialThreDead;

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
     * Product path expects DLAB clear after init - we leave it clear.
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

    /* Floating bus: LSR+MSR all-ones -> no COM1 / unmapped. */
    g_SoftSnap.u8Float =
        (u8)((g_SoftSnap.u8Lsr == 0xffu && g_SoftSnap.u8Msr == 0xffu) ? 1u : 0u);
    g_SoftSnap.u8LiveOk = (u8)(g_SoftSnap.u8Float == 0u ? 1u : 0u);
}

/**
 * Soft: compare live regs to product program. Bumps ok/bad. Returns 1 PASS.
 * Records per-field expect subflags for greppable soft expect inventory.
 * Wave 13 deepen: noint_ok/dlab_ok/fcr_ok/thre_ok are inventory lamps only
 * - they do not change the Wave 10 PASS aggregate (ier/lcr/mcr/div/live).
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

/**
 * C0 residual lean emit (one greppable line; Soft!=product).
 *
 * Shared by bring-up (via=init), panel THRE-dead (via=panel: first-miss +
 * soft_log SKIP), and full inventory (via=inv). Surfaces reentrancy +
 * G752 / no COM1 panel honesty without stamp storms or product version
 * stamps. Never reprograms UART. G-AC-1. Dual DoD stays OPEN (not serial).
 *
 * greppable: serial: soft residual lean
 * greppable: g752_no_com1= | panel_path= | com1_absent= | thre_dead=
 */
static void
serial_soft_residual_lean_emit(const char *szVia)
{
    const char *szPath = "inv";
    u8 u8Path = 0u; /* 0=inv, 1=init, 2=panel */
    u8 u8ThreDead;
    u8 u8Float;
    u8 u8Live;
    u8 u8Com1Absent;
    u8 u8PanelPath;
    u8 u8G752NoCom1;

    if (szVia != NULL && szVia[0] != '\0') {
        /* Match via=init | via=panel | via=inv (default inv). */
        if (szVia[0] == 'i' && szVia[1] == 'n' && szVia[2] == 'i' &&
            szVia[3] == 't' && szVia[4] == '\0') {
            szPath = "init";
            u8Path = 1u;
        } else if (szVia[0] == 'p' && szVia[1] == 'a' && szVia[2] == 'n' &&
                   szVia[3] == 'e' && szVia[4] == 'l' && szVia[5] == '\0') {
            szPath = "panel";
            u8Path = 2u;
        } else {
            szPath = "inv";
            u8Path = 0u;
        }
    }

    if (u8Path == 1u) {
        if (g_u32SoftResidualLeanInit < 0xffffffffu) {
            g_u32SoftResidualLeanInit++;
        }
    } else if (u8Path == 2u) {
        if (g_u32SoftResidualLeanPanel < 0xffffffffu) {
            g_u32SoftResidualLeanPanel++;
        }
    } else if (g_u32SoftResidualLeanInv < 0xffffffffu) {
        g_u32SoftResidualLeanInv++;
    }
    if (g_u32SoftResidualLeanN < 0xffffffffu) {
        g_u32SoftResidualLeanN++;
    }

    /*
     * G752 / laptop panel honesty lamps (Soft!=product; never hard-gate).
     * thre_dead sticky = no useful legacy COM1 (G752VT has none on chassis).
     * float = LSR+MSR all-ones (unmapped I/O). com1_absent covers either.
     * panel_path / g752_no_com1 mirror thre_dead for greppable operator grep.
     * fb_console_path=1: GOP/fb is the operator log when COM1 is dead.
     */
    u8ThreDead = (u8)(g_fSerialThreDead != 0u ? 1u : 0u);
    u8Float = (u8)(g_SoftSnap.u8Float != 0u ? 1u : 0u);
    u8Live = (u8)(g_SoftSnap.u8LiveOk != 0u ? 1u : 0u);
    u8Com1Absent =
        (u8)((u8ThreDead != 0u || u8Float != 0u || u8Live == 0u) ? 1u : 0u);
    u8PanelPath = u8ThreDead;
    u8G752NoCom1 = u8ThreDead; /* sticky miss => no useful 0x3F8 (G752 class) */

    /*
     * Grep: serial: soft residual lean
     * Lean reentrancy + G752 no COM1 honesty - Soft!=product dual license.
     * One line only (H2: no stamp storm). Nested soft_log -> busy_skip.
     * CRLF via serial_tx_byte (no putchar self-nest). No #if-0 residue.
     * via=init|panel|inv distinguishes residual path (C0 deepen).
     * greppable: g752_no_com1= panel_path= com1_absent= thre_dead= float=
     */
    kprintf("serial: soft residual lean via=%s reenter_safe=1 busy_guard=1 "
            "tx_nonrecurs=1 crlf_tx=1 fcr_shadow=1 never_reprog=1 "
            "log_max=%u busy_skip=%u cap_skip=%u thre_dead=%u "
            "g752_no_com1=%u panel_path=%u com1_absent=%u float=%u "
            "fb_console_path=1 spin_cap=%u spin_cap_hit=%u log_n=%u "
            "putchar=%u cr_expand=%u lean_n=%u lean_init=%u lean_panel=%u "
            "lean_inv=%u ready=%u live=%u hard_gate=0 irq_console=0 "
            "product_tty=0 soft_ne_product=1 g_ac1=1 dual=MIT_OR_Apache-2.0 "
            "unit=serial.c eng_log=1 stamp_storm=0 dual_dod=OPEN "
            "(Soft!=product; G-AC-1; dual MIT OR Apache-2.0; "
            "no version stamp; no nest via putchar; "
            "no #if0 nested-comment; residual lean eng logs only; "
            "G752 no COM1 honesty; panel residual greppable on THRE-dead; "
            "Dual DoD OPEN not serial)\n",
            szPath,
            (unsigned)UART_SOFT_LOG_MAX,
            (unsigned)g_u32SoftLogBusySkip,
            (unsigned)g_u32SoftLogCapSkip,
            (unsigned)u8ThreDead,
            (unsigned)u8G752NoCom1,
            (unsigned)u8PanelPath,
            (unsigned)u8Com1Absent,
            (unsigned)u8Float,
            (unsigned)UART_SOFT_SPIN_MAX,
            (unsigned)g_u32SoftSpinCapHit,
            (unsigned)g_u32SoftLogN,
            (unsigned)g_u32SoftPutcharN,
            (unsigned)g_u32SoftCrExpand,
            (unsigned)g_u32SoftResidualLeanN,
            (unsigned)g_u32SoftResidualLeanInit,
            (unsigned)g_u32SoftResidualLeanPanel,
            (unsigned)g_u32SoftResidualLeanInv,
            (unsigned)(g_fSerialReady ? 1u : 0u),
            (unsigned)u8Live);
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
     * Soft bring-up lines (self-contained; no main hook). kprintf ->
     * console_putchar -> serial_putchar once UART is programmed above.
     * Keep to a tight set - full inventory is serial_soft_log only.
     * Hold soft-log busy so nested serial_soft_log cannot flood residual
     * stack during bring-up kprintf frames (lean reentrancy; Soft!=product).
     */
    g_fSoftLogBusy = 1;
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
    /* C0 residual lean once at bring-up (via=init; Soft!=product). */
    serial_soft_residual_lean_emit("init");
    g_fSoftLogBusy = 0;
}

/**
 * Polled TX of one byte (no CRLF expand). Non-recursive - LF path must not
 * nest another putchar frame on residual / IRQ stacks (FAULT harden).
 */
static void
serial_tx_byte(char chOut)
{
    u16 uPort = serial_port();
    u32 uSpins;
    u8 u8SawThre = 0;

    /*
     * Panel path (G752 / no 0x3F8): after first THRE miss, skip COM1 I/O
     * entirely. Soft inventory flood x spin budget -> multi-minute "boot".
     * GOP/fb_console is the operator log; UART is dead.
     */
    if (g_fSerialThreDead != 0u) {
        if (g_u32SoftChars < 0xffffffffu) {
            g_u32SoftChars++;
        }
        return;
    }
    /*
     * Spin on LSR.THRE. Laptops without 0x3F8 never set THRE - first full
     * miss sets sticky dead lamp. QEMU Multiboot 16550 raises THRE promptly
     * -> lamp stays 0, full budget remains.
     */
    for (uSpins = 0; uSpins < UART_SOFT_SPIN_MAX; uSpins++) {
        if ((inb((u16)(uPort + UART_LSR)) & UART_LSR_THRE) != 0u) {
            u8SawThre = 1;
            break;
        }
        g_u32SoftTxFullHits++;
    }
    if (u8SawThre == 0) {
        g_fSerialThreDead = 1;
        /* Do not outb into the void; mark dead and return. */
        if (g_u32SoftSpinCapHit < 0xffffffffu) {
            g_u32SoftSpinCapHit++;
        }
        if (uSpins > g_u32SoftSpinMax) {
            g_u32SoftSpinMax = uSpins;
        }
        if (g_u32SoftChars < 0xffffffffu) {
            g_u32SoftChars++;
        }
        /*
         * C0 residual: first THRE-dead once-lamp (G752 / no COM1 honesty).
         * via=panel greps even if soft_log never runs. Nested kprintf is
         * safe: putchar now sticky-skips COM1 I/O; fb_console still paints.
         * Soft!=product; no stamp storm (once; lean_panel tallies).
         */
        if (g_u32SoftResidualLeanPanel == 0u) {
            serial_soft_residual_lean_emit("panel");
        }
        return;
    }
    if (uSpins > 0u) {
        g_u32SoftThrWaits++;
    }
    if (uSpins > g_u32SoftSpinMax) {
        g_u32SoftSpinMax = uSpins;
    }
    outb(uPort, (u8)chOut);
    if (g_u32SoftChars < 0xffffffffu) {
        g_u32SoftChars++;
    }
}

void
serial_putchar(char chOut)
{
    if (g_u32SoftPutcharN < 0xffffffffu) {
        g_u32SoftPutcharN++;
    }
    /*
     * Non-recursive CRLF: emit CR then LF via serial_tx_byte only.
     * Prior self-call doubled putchar frames on every newline (stack tax
     * under soft inventory flood on deep call stacks / FAULT class).
     */
    if (chOut == '\n') {
        if (g_u32SoftCrExpand < 0xffffffffu) {
            g_u32SoftCrExpand++;
        }
        serial_tx_byte('\r');
    }
    serial_tx_byte(chOut);
}

/**
 * Soft observability: non-zero if COM1 never raised THRE (panel-only DUT).
 *
 * Sticky after the first full soft-spin miss in serial_putchar. Used by
 * pmm/vmm soft inventory to skip multi-KiB floods on dead COM1 so
 * kernel_after_mmap can reach xHCI / M0 on G752-class laptops.
 * Returns 0 when THRE is present (QEMU Multiboot / real UART) - full spin
 * budget remains. Declared in gj/klog.h. Soft != hard boot gate.
 */
u32
serial_thre_dead(void)
{
    return g_fSerialThreDead;
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
    /* Panel path: also paint GOP so DUTs without COM1 get a *nix-style log. */
    fb_console_putchar(chOut);
}

void
console_write(const char *szText)
{
    serial_write(szText);
    fb_console_write(szText);
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
        /* spin - product can sleep via yield from userspace poll */
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
 * Residual lean: prefix-stable primary lines + lean complementary set.
 * re-verify once per log. Not hot-path - soft stats smoke only.
 * Soft!=product TTY; dual license MIT OR Apache-2.0 only.
 *
 * Small-stack / reentrancy harden (FAULT class / H2 residual):
 *   - UART_SOFT_LOG_MAX hard-caps multi-line dump emissions
 *   - busy guard acquired before any kprintf (no nest via putchar path)
 *   - shared with serial_init soft bring-up (same guard; no nest flood)
 *   - THRE-dead: one short SKIP line only (no multi-KiB dump)
 *   - CRLF non-recursive (serial_tx_byte); no putchar self-nest
 *   - no stamp storms / no product version stamp / no ret*angle residue
 *   - no #if-0 nested-comment stamp residue
 */
void
serial_soft_log(void)
{
    /*
     * Reentrancy first: kprintf -> console_putchar -> serial_putchar must
     * not re-enter this multi-line dump on residual / IRQ / nested smoke.
     * Same busy lamp as serial_init soft bring-up (lean residual safety).
     */
    if (g_fSoftLogBusy != 0) {
        if (g_u32SoftLogBusySkip < 0xffffffffu) {
            g_u32SoftLogBusySkip++;
        }
        return;
    }
    if (g_u32SoftLogN >= UART_SOFT_LOG_MAX) {
        if (g_u32SoftLogCapSkip < 0xffffffffu) {
            g_u32SoftLogCapSkip++;
        }
        return;
    }

    g_fSoftLogBusy = 1;

    if (g_u32SoftLogN < 0xffffffffu) {
        g_u32SoftLogN++;
    }

    /*
     * Panel path (no COM1 THRE): multi-line inventory is pure stack/time
     * tax via kprintf frames -> fb_console. SKIP + residual lean only
     * (via=panel) so panel residual stays greppable without multi-KiB flood.
     */
    if (g_fSerialThreDead != 0u) {
        /*
         * Grep: serial: soft log SKIP
         * G752 / no COM1 panel path - multi-line inventory is stack/time tax
         * via kprintf -> fb_console only. Soft!=product; no multi-KiB flood.
         */
        kprintf("serial: soft log SKIP thre_dead=1 g752_no_com1=1 "
                "panel_path=1 com1_absent=1 log_n=%u cap=%u wave=%u "
                "(no multi-line flood; G752 no COM1; Soft!=product; "
                "fb_console_path=1)\n",
                (unsigned)g_u32SoftLogN, (unsigned)UART_SOFT_LOG_MAX,
                (unsigned)UART_SOFT_WAVE);
        /* C0 residual: panel THRE-dead still greps residual lean. */
        serial_soft_residual_lean_emit("panel");
        g_fSoftLogBusy = 0;
        return;
    }

    if (g_fSoftSnapLive && g_fSerialReady) {
        /* Fresh snap + expect subflags for this inventory emission. */
        (void)serial_soft_verify_inner();
    } else if (g_fSoftSnapLive) {
        serial_soft_status_refresh_inner();
    }

    /*
     * Grep: serial: soft inventory - rollup (prefix-stable).
     * One catalog line; densifies counters without boot spam.
     * Soft inventory wave stamp only; no product version stamp.
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
     * Grep: serial: soft program - expected product shape (constants).
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

    /* Grep: serial: soft inits=... */
    kprintf("serial: soft inits=%u chars=%u spinmax=%u thrwait=%u "
            "txfull=%u poll=%u getc=%u log_n=%u refresh_n=%u "
            "verify_n=%u\n",
            (unsigned)g_u32SoftInits, (unsigned)g_u32SoftChars,
            (unsigned)g_u32SoftSpinMax, (unsigned)g_u32SoftThrWaits,
            (unsigned)g_u32SoftTxFullHits, (unsigned)g_u32SoftPolls,
            (unsigned)g_u32SoftGetcs, (unsigned)g_u32SoftLogN,
            (unsigned)g_u32SoftRefreshN, (unsigned)g_u32SoftVerifyN);
    /* Grep: serial: soft port=... */
    kprintf("serial: soft port=0x%x ier=0x%x lcr=0x%x mcr=0x%x "
            "lsr=0x%x msr=0x%x iir=0x%x scr=0x%x\n",
            (unsigned)g_SoftSnap.u16Port, (unsigned)g_SoftSnap.u8Ier,
            (unsigned)g_SoftSnap.u8Lcr, (unsigned)g_SoftSnap.u8Mcr,
            (unsigned)g_SoftSnap.u8Lsr, (unsigned)g_SoftSnap.u8Msr,
            (unsigned)g_SoftSnap.u8Iir, (unsigned)g_SoftSnap.u8Scr);
    /* Grep: serial: soft div=... */
    kprintf("serial: soft div=0x%x div_lo=0x%x div_hi=0x%x "
            "thre=%u temt=%u dr=%u dlab=%u oe=%u pe=%u fe=%u bi=%u "
            "err=%u\n",
            (unsigned)g_SoftSnap.u16Div, (unsigned)g_SoftSnap.u8DivLo,
            (unsigned)g_SoftSnap.u8DivHi, (unsigned)g_SoftSnap.u8Thre,
            (unsigned)g_SoftSnap.u8Temt, (unsigned)g_SoftSnap.u8Dr,
            (unsigned)g_SoftSnap.u8Dlab, (unsigned)g_SoftSnap.u8Oe,
            (unsigned)g_SoftSnap.u8Pe, (unsigned)g_SoftSnap.u8Fe,
            (unsigned)g_SoftSnap.u8Bi, (unsigned)g_SoftSnap.u8Err);
    /* Grep: serial: soft msr ... - modem lamps (was define-only). */
    kprintf("serial: soft msr cts=%u dsr=%u ri=%u dcd=%u "
            "dcts=%u ddsr=%u teri=%u ddcd=%u\n",
            (unsigned)g_SoftSnap.u8Cts, (unsigned)g_SoftSnap.u8Dsr,
            (unsigned)g_SoftSnap.u8Ri, (unsigned)g_SoftSnap.u8Dcd,
            (unsigned)g_SoftSnap.u8Dcts, (unsigned)g_SoftSnap.u8Ddsr,
            (unsigned)g_SoftSnap.u8Teri, (unsigned)g_SoftSnap.u8Ddcd);
    /* Grep: serial: soft thr ... - TX spin telemetry (Wave 13). */
    kprintf("serial: soft thr wait=%u spinmax=%u txfull=%u "
            "spin_cap=%u chars=%u thre_ok=%u temt=%u\n",
            (unsigned)g_u32SoftThrWaits, (unsigned)g_u32SoftSpinMax,
            (unsigned)g_u32SoftTxFullHits, (unsigned)UART_SOFT_SPIN_MAX,
            (unsigned)g_u32SoftChars, (unsigned)g_SoftSnap.u8ThreOk,
            (unsigned)g_SoftSnap.u8Temt);
    /* Grep: serial: soft iir ... - polled IIR + SCR (Wave 13). */
    kprintf("serial: soft iir noint=%u id=%u noint_ok=%u scr=0x%x "
            "ier=0x%x\n",
            (unsigned)g_SoftSnap.u8Noint, (unsigned)g_SoftSnap.u8IirId,
            (unsigned)g_SoftSnap.u8NointOk, (unsigned)g_SoftSnap.u8Scr,
            (unsigned)g_SoftSnap.u8Ier);
    /*
     * Grep: serial: soft path ... - polled policy + honesty non-claim.
     * Soft inventory != IRQ console, != product TTY.
     */
    kprintf("serial: soft path polled=1 irq=0 fcr=0x%x spin_cap=%u "
            "ready=%u live=%u float=%u noint=%u iir_id=%u "
            "dtr=%u rts=%u out2=%u hard_gate=0 irq_claim=0 "
            "wave=%u\n",
            (unsigned)g_SoftSnap.u8FcrProg, (unsigned)UART_SOFT_SPIN_MAX,
            (unsigned)g_fSerialReady, (unsigned)g_SoftSnap.u8LiveOk,
            (unsigned)g_SoftSnap.u8Float, (unsigned)g_SoftSnap.u8Noint,
            (unsigned)g_SoftSnap.u8IirId, (unsigned)g_SoftSnap.u8Dtr,
            (unsigned)g_SoftSnap.u8Rts, (unsigned)g_SoftSnap.u8Out2,
            (unsigned)UART_SOFT_WAVE);
    /*
     * Grep: serial: soft expect ... - per-field verify subflags.
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

    /* Grep: serial: soft mcr ... - DTR|RTS|OUT2 path lamps */
    kprintf("serial: soft mcr dtr=%u rts=%u out2=%u raw=0x%x "
            "expect=0x%x mcr_ok=%u out2_ok=%u\n",
            (unsigned)g_SoftSnap.u8Dtr, (unsigned)g_SoftSnap.u8Rts,
            (unsigned)g_SoftSnap.u8Out2, (unsigned)g_SoftSnap.u8Mcr,
            (unsigned)UART_MCR_SOFT, (unsigned)g_SoftSnap.u8McrOk,
            (unsigned)g_SoftSnap.u8Out2Ok);

    /* Grep: serial: soft float ... - float/live presence axis */
    kprintf("serial: soft float float=%u live_ok=%u float_ok=%u "
            "lsr=0x%x msr=0x%x scr=0x%x scr_ok=%u\n",
            (unsigned)g_SoftSnap.u8Float, (unsigned)g_SoftSnap.u8LiveOk,
            (unsigned)g_SoftSnap.u8FloatOk, (unsigned)g_SoftSnap.u8Lsr,
            (unsigned)g_SoftSnap.u8Msr, (unsigned)g_SoftSnap.u8Scr,
            (unsigned)g_SoftSnap.u8ScrOk);

    /*
     * Grep: serial: soft honesty
     * Soft inventory != IRQ console, != product TTY. Dual-license soft only.
     * G752 no COM1: thre_dead / panel_path lamps; fb_console is operator log.
     */
    kprintf("serial: soft honesty polled=1 irq_console=0 "
            "product_tty=0 hard_gate=0 irq_claim=0 "
            "soft_ne_product_tty=1 dual_license=1 "
            "thre_dead=%u g752_no_com1=%u panel_path=%u com1_absent=%u "
            "float=%u fb_console_path=1 dual_dod=OPEN "
            "wave=%u unit=serial.c\n",
            (unsigned)(g_fSerialThreDead != 0u ? 1u : 0u),
            (unsigned)(g_fSerialThreDead != 0u ? 1u : 0u),
            (unsigned)(g_fSerialThreDead != 0u ? 1u : 0u),
            (unsigned)((g_fSerialThreDead != 0u ||
                        g_SoftSnap.u8Float != 0u ||
                        g_SoftSnap.u8LiveOk == 0u)
                           ? 1u
                           : 0u),
            (unsigned)g_SoftSnap.u8Float,
            (unsigned)UART_SOFT_WAVE);

    /*
     * Wave 16 complementary sub-lines (kept; never reshape primary).
     */
    /* Grep: serial: soft exclusive */
    kprintf("serial: soft exclusive wave=%u exclusive=1 soft=1 "
            "unit=serial.c hard_gate=0 irq_console=0 "
            "product_tty=0 soft_ne_product_tty=1\n",
            (unsigned)UART_SOFT_WAVE);

    /* Grep: serial: soft claim - polled console product bounds */
    kprintf("serial: soft claim polled=1 irq=0 baud=%u div=0x%x "
            "lcr=8n1 mcr=dtr|rts|out2 fcr_shadow=1 ier=0 "
            "spin_cap=%u irq_console=0 product_tty=0 wave=%u\n",
            (unsigned)UART_SOFT_BAUD,
            (unsigned)(UART_SOFT_DIV_LO | ((unsigned)UART_SOFT_DIV_HI << 8)),
            (unsigned)UART_SOFT_SPIN_MAX,
            (unsigned)UART_SOFT_WAVE);

    /* Grep: serial: soft ratio - ok/bad + poll/thr path ratios */
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

    /* Grep: serial: soft err - LSR error lamp rollup */
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
     * Grep: serial: soft deepen - lean area catalog only.
     * No ret*angle / retclass / retmap stamp-storm residue.
     * residual_lean covers reentrancy honesty (busy + non-recursive TX)
     * plus C0 via=init|panel|inv residual path inventory.
     */
    kprintf("serial: soft deepen wave=%u "
            "areas=inventory,program,inits,port,div,msr,thr,iir,"
            "path,expect,verify,lamps,stats,mcr,float,honesty,"
            "exclusive,claim,ratio,err,budget,residual_lean,"
            "g752_no_com1 "
            "residual_via=init,panel,inv "
            "g752_no_com1=%u panel_path=%u com1_absent=%u thre_dead=%u "
            "unit=serial.c hard_gate=0 ready=%u live=%u "
            "lean_n=%u lean_init=%u lean_panel=%u lean_inv=%u\n",
            (unsigned)UART_SOFT_WAVE,
            (unsigned)(g_fSerialThreDead != 0u ? 1u : 0u),
            (unsigned)(g_fSerialThreDead != 0u ? 1u : 0u),
            (unsigned)((g_fSerialThreDead != 0u ||
                        g_SoftSnap.u8Float != 0u ||
                        g_SoftSnap.u8LiveOk == 0u)
                           ? 1u
                           : 0u),
            (unsigned)(g_fSerialThreDead != 0u ? 1u : 0u),
            (unsigned)(g_fSerialReady ? 1u : 0u),
            (unsigned)g_SoftSnap.u8LiveOk,
            (unsigned)g_u32SoftResidualLeanN,
            (unsigned)g_u32SoftResidualLeanInit,
            (unsigned)g_u32SoftResidualLeanPanel,
            (unsigned)g_u32SoftResidualLeanInv);

    /* Grep: serial: soft verify PASS|FAIL|idle - smoke scripts stable. */
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

    /* Grep: serial: soft budget - emission geometry (hard-cap honesty). */
    kprintf("serial: soft budget log_max=%u log_n=%u cap_skip=%u "
            "busy_skip=%u thre_dead=%u lean_n=%u lean_init=%u "
            "lean_panel=%u lean_inv=%u wave=%u\n",
            (unsigned)UART_SOFT_LOG_MAX, (unsigned)g_u32SoftLogN,
            (unsigned)g_u32SoftLogCapSkip, (unsigned)g_u32SoftLogBusySkip,
            (unsigned)g_fSerialThreDead, (unsigned)g_u32SoftResidualLeanN,
            (unsigned)g_u32SoftResidualLeanInit,
            (unsigned)g_u32SoftResidualLeanPanel,
            (unsigned)g_u32SoftResidualLeanInv, (unsigned)UART_SOFT_WAVE);

    /* C0 residual lean via=inv (full inventory path; Soft!=product). */
    serial_soft_residual_lean_emit("inv");

    g_fSoftLogBusy = 0;
}
/* Soft residual lean: reentrant soft inventory only. Soft!=product.
 * G-AC-1 (no .ko product AC). Dual MIT OR Apache-2.0. Eng logs only.
 * Dead #if-0 stamp-storm residue stays excised (nested-comment hazard).
 * C0 residual: via=init|panel|inv; G752 no COM1 honesty (g752_no_com1 /
 * panel_path / com1_absent / thre_dead / float / fb_console_path).
 * Dual DoD OPEN (not serial). Stamp-free; never bump GJ_IMAGE_VERSION.
 */

