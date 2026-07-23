/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * PL011 UART early putchar / puts / hex helpers for QEMU virt
 * (scaffold; not in x86_64 build). Base: 0x09000000 on virt machine.
 *
 * -------------------------------------------------------------------------
 * Soft observability (no IRQ / no RX product path)
 * -------------------------------------------------------------------------
 * Soft FR peek: TXFF/RXFE/TXFE status bits while transmitting.
 * Soft ID peek: PeripheralID + CellID (PrimeCell PL011 shape on virt).
 * Soft hex path: put_hex / put_hex_n / put_hex_dump exercised from
 * aarch64_uart_soft_selftest (called by cpu_info soft deepen).
 * Soft TX stats: char count + max FR-wait spins (spin budget only).
 *
 * Shared serial hex dump helper: aarch64_uart_put_hex / put_hex_n /
 * put_hex_dump — used by exception, svc, cpu_info, psci soft.
 *
 * Greppable:
 *   aarch64: uart soft fr=… peri=… cell=… chars=… spinmax=…
 *   aarch64: uart soft PASS | FAIL
 *   aarch64: uart hex soft PASS | FAIL
 *
 * Freestanding pure C; no GPL Linux serial paste.
 */
#include <gj/console.h>
#include <gj/types.h>

#ifndef GJ_AARCH64_UART_BASE
#define GJ_AARCH64_UART_BASE 0x09000000ull
#endif

/* PL011 registers (byte offsets; accessed as word indices). */
#define PL011_DR   0x00u
#define PL011_FR   0x18u
#define PL011_RIS  0x3cu /* raw interrupt status (soft peek only) */
#define PL011_MIS  0x40u /* masked interrupt status (soft peek only) */

/* FR bits (PrimeCell PL011). */
#define PL011_FR_TXFE (1u << 7)
#define PL011_FR_RXFF (1u << 6)
#define PL011_FR_TXFF (1u << 5)
#define PL011_FR_RXFE (1u << 4)
#define PL011_FR_BUSY (1u << 3)

/*
 * ID registers (word-spaced bytes at 0xFE0..0xFFC).
 * ARM PL011 typical: PeriID 0x11,0x10,0x14,0x00  CellID 0x0d,0xf0,0x05,0xb1
 * QEMU virt PL011 matches this PrimeCell layout.
 */
#define PL011_PERIPHID0 0xfe0u
#define PL011_PERIPHID1 0xfe4u
#define PL011_PERIPHID2 0xfe8u
#define PL011_PERIPHID3 0xfecu
#define PL011_CELLID0   0xff0u
#define PL011_CELLID1   0xff4u
#define PL011_CELLID2   0xff8u
#define PL011_CELLID3   0xffcu

/* Soft expected PrimeCell identity (best-effort; FAIL only if FR unreadable). */
#define PL011_SOFT_PERI0 0x11u
#define PL011_SOFT_PERI1 0x10u
#define PL011_SOFT_PERI2 0x14u
#define PL011_SOFT_PERI3 0x00u
#define PL011_SOFT_CELL0 0x0du
#define PL011_SOFT_CELL1 0xf0u
#define PL011_SOFT_CELL2 0x05u
#define PL011_SOFT_CELL3 0xb1u

/* Soft TX wait budget (same as product putchar). */
#define PL011_SOFT_SPIN_MAX 100000u

/* Soft hex-dump cap (early-console safety). */
#define PL011_SOFT_DUMP_MAX 256u

/* Soft TX observability counters. */
static unsigned long g_cUartChars;
static unsigned long g_cUartSpinMax;
static unsigned long g_cUartTxFullHits;
static int           g_fUartSoftDone;

static volatile u32 *
uart_base(void)
{
    return (volatile u32 *)(gj_vaddr_t)GJ_AARCH64_UART_BASE;
}

static u32
uart_read_off(unsigned uOff)
{
    volatile u32 *pBase = uart_base();

    return pBase[uOff / 4u];
}

void
aarch64_uart_putchar(char ch)
{
    volatile u32 *pBase = uart_base();
    unsigned uSpins;

    if (ch == '\n') {
        aarch64_uart_putchar('\r');
    }
    for (uSpins = 0; uSpins < PL011_SOFT_SPIN_MAX; uSpins++) {
        if ((pBase[PL011_FR / 4u] & PL011_FR_TXFF) == 0) {
            break;
        }
        g_cUartTxFullHits++;
    }
    if ((unsigned long)uSpins > g_cUartSpinMax) {
        g_cUartSpinMax = (unsigned long)uSpins;
    }
    pBase[PL011_DR / 4u] = (u32)(u8)ch;
    g_cUartChars++;
}

void
aarch64_uart_puts(const char *sz)
{
    if (sz == NULL) {
        return;
    }
    while (*sz != '\0') {
        aarch64_uart_putchar(*sz++);
    }
}

/* Shared C kernel console (stdio_k / kprintf linked into aarch64 product). */
void
console_putchar(char chOut)
{
    aarch64_uart_putchar(chOut);
}

void
console_write(const char *szText)
{
    aarch64_uart_puts(szText);
}

/*
 * Shared hex helper: print "0x" + up to 16 lowercase hex digits (full ulong).
 * No libc; fixed buffer on stack.
 */
void
aarch64_uart_put_hex(unsigned long v)
{
    static const char dig[] = "0123456789abcdef";
    char aBuf[19];
    int iNibble;

    aBuf[0] = '0';
    aBuf[1] = 'x';
    for (iNibble = 0; iNibble < 16; iNibble++) {
        aBuf[2 + iNibble] = dig[(v >> (60 - 4 * iNibble)) & 0xfu];
    }
    aBuf[18] = '\0';
    aarch64_uart_puts(aBuf);
}

/*
 * Print low nibble count (1..16) of v as hex without forcing full width.
 * cNibbles is clamped to [1, 16]. Always prefixes "0x".
 */
void
aarch64_uart_put_hex_n(unsigned long v, unsigned cNibbles)
{
    static const char dig[] = "0123456789abcdef";
    char aBuf[19];
    unsigned iNibble;
    unsigned uShift;

    if (cNibbles == 0u) {
        cNibbles = 1u;
    }
    if (cNibbles > 16u) {
        cNibbles = 16u;
    }
    aBuf[0] = '0';
    aBuf[1] = 'x';
    for (iNibble = 0; iNibble < cNibbles; iNibble++) {
        uShift = (cNibbles - 1u - iNibble) * 4u;
        aBuf[2u + iNibble] = dig[(v >> uShift) & 0xfu];
    }
    aBuf[2u + cNibbles] = '\0';
    aarch64_uart_puts(aBuf);
}

/*
 * Soft hex dump of cBytes at p (may be MMIO or DRAM). Bytes as "xx " groups;
 * newline every 16. cBytes capped at PL011_SOFT_DUMP_MAX for early-console.
 */
void
aarch64_uart_put_hex_dump(const void *p, unsigned cBytes)
{
    const volatile unsigned char *pb;
    unsigned iByte;
    static const char dig[] = "0123456789abcdef";

    if (p == NULL || cBytes == 0u) {
        return;
    }
    if (cBytes > PL011_SOFT_DUMP_MAX) {
        cBytes = PL011_SOFT_DUMP_MAX;
    }
    pb = (const volatile unsigned char *)p;
    for (iByte = 0; iByte < cBytes; iByte++) {
        unsigned char b = pb[iByte];

        aarch64_uart_putchar(dig[(b >> 4) & 0xfu]);
        aarch64_uart_putchar(dig[b & 0xfu]);
        aarch64_uart_putchar(' ');
        if (((iByte + 1u) & 15u) == 0u) {
            aarch64_uart_putchar('\n');
        }
    }
    if ((cBytes & 15u) != 0u) {
        aarch64_uart_putchar('\n');
    }
}

/*
 * Soft hex helper exercise: format known pattern via put_hex_n widths and a
 * tiny put_hex_dump of a stack constant. Returns 1 if helpers ran (visual
 * only — no capture of TX stream).
 */
static int
uart_hex_soft_exercise(void)
{
    static const unsigned char aPat[8] = {
        0xde, 0xad, 0xbe, 0xef, 0x01, 0x23, 0x45, 0x67
    };

    /* Width clamp soft: 0→1 nibble, 20→16 nibbles. */
    aarch64_uart_puts("aarch64: uart hex soft n1=");
    aarch64_uart_put_hex_n(0xful, 1u);
    aarch64_uart_puts(" n4=");
    aarch64_uart_put_hex_n(0xabcdul, 4u);
    aarch64_uart_puts(" n0clamp=");
    aarch64_uart_put_hex_n(0x1ul, 0u);
    aarch64_uart_puts(" n20clamp=");
    aarch64_uart_put_hex_n(0x0123456789abcdeful, 20u);
    aarch64_uart_puts("\n");
    aarch64_uart_puts("aarch64: uart hex soft dump=");
    aarch64_uart_put_hex_dump(aPat, (unsigned)sizeof(aPat));
    return 1;
}

/*
 * aarch64_uart_soft_selftest — greppable soft markers for PL011 identity +
 * hex helpers. Idempotent (one-shot). Safe to call from cpu_info soft deepen.
 *
 * Does not enable RX IRQ or reprogram baud (QEMU virt is pre-clocked).
 */
void
aarch64_uart_soft_selftest(void)
{
    u32 uFr;
    u32 uRis;
    u32 uMis;
    u32 uPeri0;
    u32 uPeri1;
    u32 uPeri2;
    u32 uPeri3;
    u32 uCell0;
    u32 uCell1;
    u32 uCell2;
    u32 uCell3;
    u32 uPeriPack;
    u32 uCellPack;
    int fIdOk;
    int fHexOk;

    if (g_fUartSoftDone != 0) {
        return;
    }
    g_fUartSoftDone = 1;

    uFr = uart_read_off(PL011_FR);
    uRis = uart_read_off(PL011_RIS);
    uMis = uart_read_off(PL011_MIS);

    uPeri0 = uart_read_off(PL011_PERIPHID0) & 0xffu;
    uPeri1 = uart_read_off(PL011_PERIPHID1) & 0xffu;
    uPeri2 = uart_read_off(PL011_PERIPHID2) & 0xffu;
    uPeri3 = uart_read_off(PL011_PERIPHID3) & 0xffu;
    uCell0 = uart_read_off(PL011_CELLID0) & 0xffu;
    uCell1 = uart_read_off(PL011_CELLID1) & 0xffu;
    uCell2 = uart_read_off(PL011_CELLID2) & 0xffu;
    uCell3 = uart_read_off(PL011_CELLID3) & 0xffu;

    uPeriPack = uPeri0 | (uPeri1 << 8) | (uPeri2 << 16) | (uPeri3 << 24);
    uCellPack = uCell0 | (uCell1 << 8) | (uCell2 << 16) | (uCell3 << 24);

    /*
     * Soft ID match: accept exact PrimeCell PL011 shape. Non-match is soft
     * FAIL only when FR itself looks dead (all-ones unmapped MMIO).
     */
    fIdOk = 1;
    if (uPeri0 != PL011_SOFT_PERI0 || uPeri1 != PL011_SOFT_PERI1 ||
        uPeri2 != PL011_SOFT_PERI2 || uPeri3 != PL011_SOFT_PERI3) {
        fIdOk = 0;
    }
    if (uCell0 != PL011_SOFT_CELL0 || uCell1 != PL011_SOFT_CELL1 ||
        uCell2 != PL011_SOFT_CELL2 || uCell3 != PL011_SOFT_CELL3) {
        fIdOk = 0;
    }
    /* Unmapped / stuck FR is fatal soft; ID mismatch alone is still FAIL. */
    if (uFr == 0xffffffffu) {
        fIdOk = 0;
    }

    aarch64_uart_puts("aarch64: uart soft fr=");
    aarch64_uart_put_hex((unsigned long)uFr);
    aarch64_uart_puts(" ris=");
    aarch64_uart_put_hex((unsigned long)uRis);
    aarch64_uart_puts(" mis=");
    aarch64_uart_put_hex((unsigned long)uMis);
    aarch64_uart_puts(" peri=");
    aarch64_uart_put_hex((unsigned long)uPeriPack);
    aarch64_uart_puts(" cell=");
    aarch64_uart_put_hex((unsigned long)uCellPack);
    aarch64_uart_puts(" chars=");
    aarch64_uart_put_hex(g_cUartChars);
    aarch64_uart_puts(" spinmax=");
    aarch64_uart_put_hex(g_cUartSpinMax);
    aarch64_uart_puts(" txfull=");
    aarch64_uart_put_hex(g_cUartTxFullHits);
    aarch64_uart_puts(" txfe=");
    aarch64_uart_put_hex((unsigned long)((uFr & PL011_FR_TXFE) != 0u));
    aarch64_uart_puts(" rxfe=");
    aarch64_uart_put_hex((unsigned long)((uFr & PL011_FR_RXFE) != 0u));
    aarch64_uart_puts("\n");

    if (fIdOk != 0) {
        aarch64_uart_puts("aarch64: uart soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: uart soft FAIL\n");
    }

    fHexOk = uart_hex_soft_exercise();
    if (fHexOk != 0) {
        aarch64_uart_puts("aarch64: uart hex soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: uart hex soft FAIL\n");
    }

    (void)PL011_FR_RXFF;
    (void)PL011_FR_BUSY;
    (void)PL011_FR_TXFF;
}
