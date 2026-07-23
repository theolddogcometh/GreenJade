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
 * Soft status: live LSR lamps (DR/THRE/TEMT/errors), MSR modem bits,
 * TX spin budget + char/poll/getc counters.
 * Soft verify: IER=0 (polled), LCR=8N1 DLAB-off, MCR DTR|RTS|OUT2,
 * divisor lo/hi match 38400 program (0x0003). Never reprograms UART.
 *
 * Greppable (product / smoke inventory):
 *   serial: soft inits=… chars=… spinmax=… thrwait=… poll=… getc=…
 *   serial: soft port=0x… ier=0x… lcr=0x… mcr=0x… lsr=0x… msr=0x… iir=0x…
 *   serial: soft div=0x… thre=… temt=… dr=… dlab=… oe=… pe=… fe=… bi=…
 *   serial: soft verify PASS|FAIL|idle (ok=… bad=…)
 *
 * Soft APIs (linkable; no main hook required — serial_init self-logs):
 *   serial_soft_inits / chars / spinmax / thr_waits / polls / getcs
 *   serial_soft_verify_ok / verify_bad / ready
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

/* Soft snapshot of last programmed + last live status peeks. */
struct serial_soft_snap {
    u16 u16Port;
    u16 u16Div;     /* DLL|(DLH<<8) after DLAB readback */
    u8  u8Ier;
    u8  u8Lcr;
    u8  u8Mcr;
    u8  u8Lsr;
    u8  u8Msr;
    u8  u8Iir;
    u8  u8Thre;     /* LSR.THRE lamp */
    u8  u8Temt;     /* LSR.TEMT lamp */
    u8  u8Dr;       /* LSR.DR lamp */
    u8  u8Dlab;     /* LCR.DLAB lamp (expect 0 after init) */
    u8  u8Oe;       /* overrun */
    u8  u8Pe;       /* parity */
    u8  u8Fe;       /* framing */
    u8  u8Bi;       /* break */
    u8  u8VerifyOk; /* last soft verify result */
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
}

/**
 * Soft: read live IER/LCR/MCR/LSR/MSR/IIR + divisor via brief DLAB.
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

    g_SoftSnap.u16Port = uPort;
    g_SoftSnap.u8Ier = inb((u16)(uPort + UART_IER));
    g_SoftSnap.u8Lcr = inb((u16)(uPort + UART_LCR));
    g_SoftSnap.u8Mcr = inb((u16)(uPort + UART_MCR));
    u8Lsr = inb((u16)(uPort + UART_LSR));
    serial_soft_decode_lsr(&g_SoftSnap, u8Lsr);
    g_SoftSnap.u8Msr = inb((u16)(uPort + UART_MSR));
    g_SoftSnap.u8Iir = inb((u16)(uPort + UART_IIR));
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
    /* Re-read LCR after restore so snap matches live DLAB-off state. */
    g_SoftSnap.u8Lcr = inb((u16)(uPort + UART_LCR));
    g_SoftSnap.u8Dlab = (u8)((g_SoftSnap.u8Lcr & UART_LCR_DLAB) != 0u);
    g_SoftSnap.u8Ier = inb((u16)(uPort + UART_IER));
}

/**
 * Soft: compare live regs to product program. Bumps ok/bad. Returns 1 PASS.
 */
static int
serial_soft_verify_inner(void)
{
    int fOk = 1;
    u16 u16ExpectDiv;

    if (!g_fSoftSnapLive || !g_fSerialReady) {
        return 0;
    }

    serial_soft_status_refresh_inner();

    u16ExpectDiv = (u16)(UART_SOFT_DIV_LO | ((u16)UART_SOFT_DIV_HI << 8));

    if (g_SoftSnap.u8Ier != UART_IER_SOFT_NONE) {
        fOk = 0;
    }
    if ((g_SoftSnap.u8Lcr & 0x3fu) != UART_SOFT_LCR) {
        /* Ignore spare/break bits; require 8N1 + DLAB clear in low fields. */
        fOk = 0;
    }
    if ((g_SoftSnap.u8Lcr & UART_LCR_DLAB) != 0u) {
        fOk = 0;
    }
    if ((g_SoftSnap.u8Mcr & UART_MCR_SOFT) != UART_MCR_SOFT) {
        fOk = 0;
    }
    if (g_SoftSnap.u16Div != u16ExpectDiv) {
        fOk = 0;
    }
    /* LSR all-ones often means missing I/O port (no COM1). Soft FAIL. */
    if (g_SoftSnap.u8Lsr == 0xffu && g_SoftSnap.u8Msr == 0xffu) {
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
            "oe=%u pe=%u fe=%u bi=%u\n",
            (unsigned)g_SoftSnap.u8Thre, (unsigned)g_SoftSnap.u8Temt,
            (unsigned)g_SoftSnap.u8Dr, (unsigned)g_SoftSnap.u8Dlab,
            (unsigned)g_SoftSnap.u8Oe, (unsigned)g_SoftSnap.u8Pe,
            (unsigned)g_SoftSnap.u8Fe, (unsigned)g_SoftSnap.u8Bi);

    if (serial_soft_verify_inner()) {
        kprintf("serial: soft verify PASS inits=%u\n", g_u32SoftInits);
    } else {
        kprintf("serial: soft verify FAIL inits=%u\n", g_u32SoftInits);
    }
}

void
serial_putchar(char chOut)
{
    u16 uPort = serial_port();
    u32 uSpins;

    if (chOut == '\n') {
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
    outb(uPort, (u8)chOut);
    g_u32SoftChars++;
}

void
serial_write(const char *szText)
{
    while (szText && *szText) {
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

    g_u32SoftPolls++;
    return (inb((u16)(uPort + UART_LSR)) & UART_LSR_DR) != 0u ? 1 : 0;
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
 */
void
serial_soft_log(void)
{
    if (g_fSoftSnapLive) {
        serial_soft_status_refresh_inner();
    }

    kprintf("serial: soft inits=%u chars=%u spinmax=%u thrwait=%u "
            "txfull=%u poll=%u getc=%u\n",
            g_u32SoftInits, g_u32SoftChars, g_u32SoftSpinMax,
            g_u32SoftThrWaits, g_u32SoftTxFullHits, g_u32SoftPolls,
            g_u32SoftGetcs);
    kprintf("serial: soft port=0x%x ier=0x%x lcr=0x%x mcr=0x%x "
            "lsr=0x%x msr=0x%x iir=0x%x\n",
            (unsigned)g_SoftSnap.u16Port, (unsigned)g_SoftSnap.u8Ier,
            (unsigned)g_SoftSnap.u8Lcr, (unsigned)g_SoftSnap.u8Mcr,
            (unsigned)g_SoftSnap.u8Lsr, (unsigned)g_SoftSnap.u8Msr,
            (unsigned)g_SoftSnap.u8Iir);
    kprintf("serial: soft div=0x%x thre=%u temt=%u dr=%u dlab=%u "
            "oe=%u pe=%u fe=%u bi=%u\n",
            (unsigned)g_SoftSnap.u16Div, (unsigned)g_SoftSnap.u8Thre,
            (unsigned)g_SoftSnap.u8Temt, (unsigned)g_SoftSnap.u8Dr,
            (unsigned)g_SoftSnap.u8Dlab, (unsigned)g_SoftSnap.u8Oe,
            (unsigned)g_SoftSnap.u8Pe, (unsigned)g_SoftSnap.u8Fe,
            (unsigned)g_SoftSnap.u8Bi);

    if (!g_fSoftSnapLive) {
        kprintf("serial: soft verify idle (ok=%u bad=%u)\n",
                g_u32SoftVerifyOk, g_u32SoftVerifyBad);
    } else if (g_SoftSnap.u8VerifyOk) {
        kprintf("serial: soft verify PASS (ok=%u bad=%u)\n",
                g_u32SoftVerifyOk, g_u32SoftVerifyBad);
    } else {
        kprintf("serial: soft verify FAIL (ok=%u bad=%u)\n",
                g_u32SoftVerifyOk, g_u32SoftVerifyBad);
    }

    (void)UART_LSR_ERR;
    (void)UART_MSR_DCTS;
    (void)UART_MSR_DDSR;
    (void)UART_MSR_TERI;
    (void)UART_MSR_DDCD;
    (void)UART_MSR_CTS;
    (void)UART_MSR_DSR;
    (void)UART_MSR_RI;
    (void)UART_MSR_DCD;
    (void)UART_SCR;
}
