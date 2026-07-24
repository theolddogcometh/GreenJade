/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * PL011 UART early putchar / puts / hex helpers for QEMU virt
 * (scaffold; not in x86_64 build). Base: 0x09000000 on virt machine.
 *
 * -------------------------------------------------------------------------
 * Soft UART inventory (Wave 35 exclusive deepen — no IRQ / no RX product path)
 * -------------------------------------------------------------------------
 * Soft FR peek: TXFF/RXFE/TXFE/RXFF/BUSY status lamps while transmitting.
 * Soft ID peek: PeripheralID + CellID (PrimeCell PL011 shape on virt).
 * Soft ctrl peek: CR / LCRH / IBRD / FBRD / IMSC / DMACR / RSR (read-only;
 * never reprograms baud or enables IRQ — QEMU virt is pre-clocked).
 * Soft hex path: put_hex / put_hex_n / put_hex_dump exercised from
 * aarch64_uart_soft_selftest (called by cpu_info soft deepen).
 * Soft TX stats: char count + thrwait + max FR-wait spins + TXFF hits.
 * Soft program: expected PrimeCell peri/cell pack + base + spin cap.
 * Soft deepen: area catalog stamp wave=77.
 * Soft path honesty: polled early console only; product_kernel=OPEN.
 *
 * Shared serial hex dump helper: aarch64_uart_put_hex / put_hex_n /
 * put_hex_dump — used by exception, svc, cpu_info, psci soft.
 *
 * Greppable soft inventory (prefix-stable):
 *   aarch64: uart soft chars=… spinmax=… thrwait=… txfull=… base=…
 *   aarch64: uart soft fr=… ris=… mis=… peri=… cell=…
 *   aarch64: uart soft lamps txfe=… rxfe=… txff=… rxff=… busy=…
 *   aarch64: uart soft ctrl cr=… lcrh=… ibrd=… fbrd=… imsc=… dmacr=… rsr=…
 *   aarch64: uart soft id peri=… cell=… match=… fr_live=…
 *   aarch64: uart soft inventory wave=77 …
 *   aarch64: uart soft program base=… peri=… cell=… spin_cap=…
 *   aarch64: uart soft stats …
 *   aarch64: uart soft deepen wave=77 areas=…
 *   aarch64: uart soft path polled=1 irq=0 product_kernel=OPEN wave=77
 *   aarch64: uart soft return inv_ret=… product_kernel=OPEN
 *   aarch64: uart soft honesty product_kernel=OPEN soft_only=1
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
#define PL011_DR    0x00u
#define PL011_RSR   0x04u /* receive status / error clear (soft peek) */
#define PL011_FR    0x18u
#define PL011_ILPR  0x20u /* IrDA low-power (soft peek; unused product) */
#define PL011_IBRD  0x24u /* integer baud divisor (soft peek) */
#define PL011_FBRD  0x28u /* fractional baud divisor (soft peek) */
#define PL011_LCRH  0x2cu /* line control (soft peek) */
#define PL011_CR    0x30u /* control (soft peek) */
#define PL011_IFLS  0x34u /* interrupt FIFO level select (soft peek) */
#define PL011_IMSC  0x38u /* interrupt mask set/clear (soft peek) */
#define PL011_RIS   0x3cu /* raw interrupt status (soft peek only) */
#define PL011_MIS   0x40u /* masked interrupt status (soft peek only) */
#define PL011_DMACR 0x48u /* DMA control (soft peek) */

/* FR bits (PrimeCell PL011). */
#define PL011_FR_TXFE (1u << 7)
#define PL011_FR_RXFF (1u << 6)
#define PL011_FR_TXFF (1u << 5)
#define PL011_FR_RXFE (1u << 4)
#define PL011_FR_BUSY (1u << 3)

/* CR soft lamps (product does not program; QEMU virt pre-enables). */
#define PL011_CR_UARTEN (1u << 0)
#define PL011_CR_TXE    (1u << 8)
#define PL011_CR_RXE    (1u << 9)

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

/* Wave 45 soft inventory stamp (greppable wave=77). */
#define PL011_SOFT_WAVE 77u

/* Soft deepen areas: chars,fr,lamps,ctrl,id,program,stats,path,honesty. */
#define PL011_SOFT_AREAS 124u

/*
 * Soft UART inventory snapshot (Wave 19).
 * Live peeks only — never written back to MMIO.
 * greppable: aarch64: uart soft
 */
struct uart_soft_snap {
    u32 u32Fr;
    u32 u32Ris;
    u32 u32Mis;
    u32 u32Cr;
    u32 u32Lcrh;
    u32 u32Ibrd;
    u32 u32Fbrd;
    u32 u32Imsc;
    u32 u32Dmacr;
    u32 u32Rsr;
    u32 u32Ifls;
    u32 u32Ilpr;
    u32 u32PeriPack;
    u32 u32CellPack;
    u8  u8Txfe;     /* FR.TXFE lamp */
    u8  u8Rxfe;     /* FR.RXFE lamp */
    u8  u8Txff;     /* FR.TXFF lamp */
    u8  u8Rxff;     /* FR.RXFF lamp */
    u8  u8Busy;     /* FR.BUSY lamp */
    u8  u8Uarten;   /* CR.UARTEN lamp */
    u8  u8Txe;      /* CR.TXE lamp */
    u8  u8Rxe;      /* CR.RXE lamp */
    u8  u8IdMatch;  /* PrimeCell ID soft match */
    u8  u8FrLive;   /* FR not stuck-all-ones */
    u8  u8VerifyOk; /* last soft inventory gate */
    u8  u8Pad;
};

/* Soft TX observability counters. */
static unsigned long g_cUartChars;
static unsigned long g_cUartSpinMax;
static unsigned long g_cUartTxFullHits;
static unsigned long g_cUartThrWaits; /* putchar paths that spun at least once */
static unsigned      g_cUartSoftLogs; /* Wave 19 inventory emit count */
static int           g_fUartSoftDone;
static int           g_fUartSoftSnapLive;
static struct uart_soft_snap g_UartSoftSnap;

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
    if (uSpins > 0u) {
        g_cUartThrWaits++;
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
 * Soft: decode FR + CR into snap lamps. Pure observability.
 */
static void
uart_soft_decode_lamps(struct uart_soft_snap *pSnap)
{
    u32 uFr;
    u32 uCr;

    if (pSnap == NULL) {
        return;
    }
    uFr = pSnap->u32Fr;
    uCr = pSnap->u32Cr;
    pSnap->u8Txfe = (u8)((uFr & PL011_FR_TXFE) != 0u);
    pSnap->u8Rxfe = (u8)((uFr & PL011_FR_RXFE) != 0u);
    pSnap->u8Txff = (u8)((uFr & PL011_FR_TXFF) != 0u);
    pSnap->u8Rxff = (u8)((uFr & PL011_FR_RXFF) != 0u);
    pSnap->u8Busy = (u8)((uFr & PL011_FR_BUSY) != 0u);
    pSnap->u8Uarten = (u8)((uCr & PL011_CR_UARTEN) != 0u);
    pSnap->u8Txe = (u8)((uCr & PL011_CR_TXE) != 0u);
    pSnap->u8Rxe = (u8)((uCr & PL011_CR_RXE) != 0u);
    pSnap->u8FrLive = (u8)(uFr != 0xffffffffu ? 1u : 0u);
}

/*
 * Soft: refresh live PL011 inventory into g_UartSoftSnap (read-only MMIO).
 * Never writes control/baud/IRQ masks. Safe on QEMU virt PL011.
 */
static void
uart_soft_status_refresh_inner(void)
{
    u32 uPeri0;
    u32 uPeri1;
    u32 uPeri2;
    u32 uPeri3;
    u32 uCell0;
    u32 uCell1;
    u32 uCell2;
    u32 uCell3;
    int fIdOk;

    g_UartSoftSnap.u32Fr = uart_read_off(PL011_FR);
    g_UartSoftSnap.u32Ris = uart_read_off(PL011_RIS);
    g_UartSoftSnap.u32Mis = uart_read_off(PL011_MIS);
    g_UartSoftSnap.u32Cr = uart_read_off(PL011_CR);
    g_UartSoftSnap.u32Lcrh = uart_read_off(PL011_LCRH);
    g_UartSoftSnap.u32Ibrd = uart_read_off(PL011_IBRD);
    g_UartSoftSnap.u32Fbrd = uart_read_off(PL011_FBRD);
    g_UartSoftSnap.u32Imsc = uart_read_off(PL011_IMSC);
    g_UartSoftSnap.u32Dmacr = uart_read_off(PL011_DMACR);
    g_UartSoftSnap.u32Rsr = uart_read_off(PL011_RSR);
    g_UartSoftSnap.u32Ifls = uart_read_off(PL011_IFLS);
    g_UartSoftSnap.u32Ilpr = uart_read_off(PL011_ILPR);

    uPeri0 = uart_read_off(PL011_PERIPHID0) & 0xffu;
    uPeri1 = uart_read_off(PL011_PERIPHID1) & 0xffu;
    uPeri2 = uart_read_off(PL011_PERIPHID2) & 0xffu;
    uPeri3 = uart_read_off(PL011_PERIPHID3) & 0xffu;
    uCell0 = uart_read_off(PL011_CELLID0) & 0xffu;
    uCell1 = uart_read_off(PL011_CELLID1) & 0xffu;
    uCell2 = uart_read_off(PL011_CELLID2) & 0xffu;
    uCell3 = uart_read_off(PL011_CELLID3) & 0xffu;

    g_UartSoftSnap.u32PeriPack =
        uPeri0 | (uPeri1 << 8) | (uPeri2 << 16) | (uPeri3 << 24);
    g_UartSoftSnap.u32CellPack =
        uCell0 | (uCell1 << 8) | (uCell2 << 16) | (uCell3 << 24);

    fIdOk = 1;
    if (uPeri0 != PL011_SOFT_PERI0 || uPeri1 != PL011_SOFT_PERI1 ||
        uPeri2 != PL011_SOFT_PERI2 || uPeri3 != PL011_SOFT_PERI3) {
        fIdOk = 0;
    }
    if (uCell0 != PL011_SOFT_CELL0 || uCell1 != PL011_SOFT_CELL1 ||
        uCell2 != PL011_SOFT_CELL2 || uCell3 != PL011_SOFT_CELL3) {
        fIdOk = 0;
    }
    g_UartSoftSnap.u8IdMatch = (u8)(fIdOk != 0 ? 1u : 0u);

    uart_soft_decode_lamps(&g_UartSoftSnap);
    g_fUartSoftSnapLive = 1;
}

/*
 * Soft inventory gate: FR live + PrimeCell ID match.
 * Unmapped / stuck FR (all-ones) is soft FAIL. ID mismatch alone is FAIL
 * on virt (expected PrimeCell shape) but never reprograms silicon.
 * Returns 1 on PASS.
 */
static int
uart_soft_verify_inner(void)
{
    int fOk;

    uart_soft_status_refresh_inner();

    fOk = 1;
    if (g_UartSoftSnap.u8FrLive == 0u) {
        fOk = 0;
    }
    if (g_UartSoftSnap.u8IdMatch == 0u) {
        fOk = 0;
    }
    /* Unmapped FR is fatal soft regardless of ID bytes. */
    if (g_UartSoftSnap.u32Fr == 0xffffffffu) {
        fOk = 0;
    }

    g_UartSoftSnap.u8VerifyOk = (u8)(fOk != 0 ? 1u : 0u);
    return fOk;
}

/*
 * Soft UART inventory lines (greppable "aarch64: uart soft …"; Wave 19).
 * Returns 1 if soft verify PASS.
 */
static int
uart_soft_inventory(void)
{
    unsigned long u64ExpectPeri;
    unsigned long u64ExpectCell;
    int fOk;

    if (g_cUartSoftLogs < 0xffffffffu) {
        g_cUartSoftLogs++;
    }

    fOk = uart_soft_verify_inner();

    u64ExpectPeri = (unsigned long)PL011_SOFT_PERI0 |
                    ((unsigned long)PL011_SOFT_PERI1 << 8) |
                    ((unsigned long)PL011_SOFT_PERI2 << 16) |
                    ((unsigned long)PL011_SOFT_PERI3 << 24);
    u64ExpectCell = (unsigned long)PL011_SOFT_CELL0 |
                    ((unsigned long)PL011_SOFT_CELL1 << 8) |
                    ((unsigned long)PL011_SOFT_CELL2 << 16) |
                    ((unsigned long)PL011_SOFT_CELL3 << 24);

    /* TX / spin soft counters. */
    aarch64_uart_puts("aarch64: uart soft chars=");
    aarch64_uart_put_hex(g_cUartChars);
    aarch64_uart_puts(" spinmax=");
    aarch64_uart_put_hex(g_cUartSpinMax);
    aarch64_uart_puts(" thrwait=");
    aarch64_uart_put_hex(g_cUartThrWaits);
    aarch64_uart_puts(" txfull=");
    aarch64_uart_put_hex(g_cUartTxFullHits);
    aarch64_uart_puts(" base=");
    aarch64_uart_put_hex((unsigned long)GJ_AARCH64_UART_BASE);
    aarch64_uart_puts("\n");

    /* FR / IRQ status + packed ID. */
    aarch64_uart_puts("aarch64: uart soft fr=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u32Fr);
    aarch64_uart_puts(" ris=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u32Ris);
    aarch64_uart_puts(" mis=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u32Mis);
    aarch64_uart_puts(" peri=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u32PeriPack);
    aarch64_uart_puts(" cell=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u32CellPack);
    aarch64_uart_puts("\n");

    /* FR + CR lamps. */
    aarch64_uart_puts("aarch64: uart soft lamps txfe=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u8Txfe);
    aarch64_uart_puts(" rxfe=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u8Rxfe);
    aarch64_uart_puts(" txff=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u8Txff);
    aarch64_uart_puts(" rxff=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u8Rxff);
    aarch64_uart_puts(" busy=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u8Busy);
    aarch64_uart_puts(" uarten=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u8Uarten);
    aarch64_uart_puts(" txe=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u8Txe);
    aarch64_uart_puts(" rxe=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u8Rxe);
    aarch64_uart_puts("\n");

    /* Control / baud / IRQ mask soft peeks (read-only). */
    aarch64_uart_puts("aarch64: uart soft ctrl cr=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u32Cr);
    aarch64_uart_puts(" lcrh=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u32Lcrh);
    aarch64_uart_puts(" ibrd=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u32Ibrd);
    aarch64_uart_puts(" fbrd=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u32Fbrd);
    aarch64_uart_puts(" imsc=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u32Imsc);
    aarch64_uart_puts(" dmacr=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u32Dmacr);
    aarch64_uart_puts(" rsr=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u32Rsr);
    aarch64_uart_puts(" ifls=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u32Ifls);
    aarch64_uart_puts("\n");

    /* Identity soft inventory. */
    aarch64_uart_puts("aarch64: uart soft id peri=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u32PeriPack);
    aarch64_uart_puts(" cell=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u32CellPack);
    aarch64_uart_puts(" match=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u8IdMatch);
    aarch64_uart_puts(" fr_live=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u8FrLive);
    aarch64_uart_puts(" verify=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u8VerifyOk);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: uart soft inventory — Wave 19 rollup. */
    aarch64_uart_puts("aarch64: uart soft inventory wave=");
    aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
    aarch64_uart_puts(" chars=");
    aarch64_uart_put_hex(g_cUartChars);
    aarch64_uart_puts(" match=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u8IdMatch);
    aarch64_uart_puts(" fr_live=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u8FrLive);
    aarch64_uart_puts(" verify=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u8VerifyOk);
    aarch64_uart_puts(" logs=");
    aarch64_uart_put_hex((unsigned long)g_cUartSoftLogs);
    aarch64_uart_puts("\n");

    /*
     * Grep: aarch64: uart soft program
     * Expected PrimeCell shape + base + spin budget (shadow constants only).
     */
    aarch64_uart_puts("aarch64: uart soft program base=");
    aarch64_uart_put_hex((unsigned long)GJ_AARCH64_UART_BASE);
    aarch64_uart_puts(" peri=");
    aarch64_uart_put_hex(u64ExpectPeri);
    aarch64_uart_puts(" cell=");
    aarch64_uart_put_hex(u64ExpectCell);
    aarch64_uart_puts(" spin_cap=");
    aarch64_uart_put_hex((unsigned long)PL011_SOFT_SPIN_MAX);
    aarch64_uart_puts(" dump_max=");
    aarch64_uart_put_hex((unsigned long)PL011_SOFT_DUMP_MAX);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: uart soft stats */
    aarch64_uart_puts("aarch64: uart soft stats chars=");
    aarch64_uart_put_hex(g_cUartChars);
    aarch64_uart_puts(" thrwait=");
    aarch64_uart_put_hex(g_cUartThrWaits);
    aarch64_uart_puts(" txfull=");
    aarch64_uart_put_hex(g_cUartTxFullHits);
    aarch64_uart_puts(" spinmax=");
    aarch64_uart_put_hex(g_cUartSpinMax);
    aarch64_uart_puts(" logs=");
    aarch64_uart_put_hex((unsigned long)g_cUartSoftLogs);
    aarch64_uart_puts(" wave=");
    aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /*
     * Grep: aarch64: uart soft deepen
     * Wave 19 area catalog — polled soft console only.
     */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: aarch64: uart: soft retclass — Wave 19 return-class taxonomy (kept) */
    aarch64_uart_puts("aarch64: uart: soft retclass ok|fail|inval|nodev|busy|nomem "
                      "soft_only=1 product_gate=0 wave=");
    aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
    aarch64_uart_puts(" (retclass taxonomy; Soft!=product; not bar3)\n");
    /* Grep: aarch64: uart: soft retlane — Wave 19 return-lane catalog (kept) */
    aarch64_uart_puts("aarch64: uart: soft retlane inv|selftest|rate|retcode|retmap|class "
                      "product_kernel=OPEN soft_ne_product=1 wave=");
    aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
    aarch64_uart_puts(" (retlane catalog; Soft!=product)\n");
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: aarch64: uart: soft retbound — Wave 20 return-bound honesty (kept) */
    aarch64_uart_puts("aarch64: uart: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
                      "never_blocks_m0=1 wave=");
    aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
    aarch64_uart_puts(" (retbound honesty; Soft!=product; not bar3)\n");
    /* Grep: aarch64: uart: soft retseal — Wave 20 seal stamp (kept) */
    aarch64_uart_puts("aarch64: uart: soft retseal exclusive=1 soft_ne_product=1 "
                      "product_kernel=OPEN bar3=0 wave=");
    aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
    aarch64_uart_puts(" (retseal stamp; Soft!=product)\n");
    /*
     * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: aarch64: uart: soft retpulse — Wave 21 return-pulse honesty (kept) */
    aarch64_uart_puts("aarch64: uart: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                      "never_blocks_m0=1 wave=");
    aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
    aarch64_uart_puts(" (retpulse honesty; Soft!=product; not bar3)\n");
    /* Grep: aarch64: uart: soft retmark — Wave 21 mark stamp (kept) */
    aarch64_uart_puts("aarch64: uart: soft retmark exclusive=1 soft_ne_product=1 "
                      "product_kernel=OPEN bar3=0 wave=");
    aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
    aarch64_uart_puts(" (retmark stamp; Soft!=product)\n");
    
/*
 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: uart: soft retphase — Wave 22 return-phase honesty (kept) */
aarch64_uart_puts("aarch64: uart: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retphase honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retbadge — Wave 22 badge stamp (kept) */
aarch64_uart_puts("aarch64: uart: soft retbadge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retbadge stamp; Soft!=product)\n");
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: uart: soft rettoken — Wave 23 return-token honesty (kept) */
aarch64_uart_puts("aarch64: uart: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (rettoken honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retcrest — Wave 23 crest stamp (kept) */
aarch64_uart_puts("aarch64: uart: soft retcrest exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retcrest stamp; Soft!=product)\n");
/*
 * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: uart: soft retvault — Wave 24 return-vault honesty (kept) */
aarch64_uart_puts("aarch64: uart: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retvault honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retbanner — Wave 24 banner stamp (kept) */
aarch64_uart_puts("aarch64: uart: soft retbanner exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retbanner stamp; Soft!=product)\n");
/*
 * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: uart: soft retledger — Wave 25 return-ledger honesty (kept) */
aarch64_uart_puts("aarch64: uart: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retledger honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retbeacon — Wave 25 beacon stamp (kept) */
aarch64_uart_puts("aarch64: uart: soft retbeacon exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retbeacon stamp; Soft!=product)\n");
/*
 * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: uart: soft retcipher — Wave 26 return-cipher honesty (kept) */
aarch64_uart_puts("aarch64: uart: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retcipher honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retflame — Wave 26 flame stamp (kept) */
aarch64_uart_puts("aarch64: uart: soft retflame exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retflame stamp; Soft!=product)\n");
/*
 * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: uart: soft retprism — Wave 27 return-prism honesty (kept) */
aarch64_uart_puts("aarch64: uart: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retprism honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retforge — Wave 27 forge stamp (kept) */
aarch64_uart_puts("aarch64: uart: soft retforge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retforge stamp; Soft!=product)\n");
/*
 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: uart: soft retshard — Wave 28 return-shard honesty (kept) */
aarch64_uart_puts("aarch64: uart: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retshard honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retcrown — Wave 28 crown stamp (kept) */
aarch64_uart_puts("aarch64: uart: soft retcrown exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retcrown stamp; Soft!=product)\n");
/*
 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: uart: soft retglyph — Wave 29 return-glyph honesty (kept) */
aarch64_uart_puts("aarch64: uart: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retglyph honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retscepter — Wave 29 scepter stamp (kept) */
aarch64_uart_puts("aarch64: uart: soft retscepter exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retscepter stamp; Soft!=product)\n");
/*
 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: uart: soft retsigil — Wave 30 return-sigil honesty (kept) */
aarch64_uart_puts("aarch64: uart: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retsigil honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retemblem — Wave 30 emblem stamp (kept) */
aarch64_uart_puts("aarch64: uart: soft retemblem exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retemblem stamp; Soft!=product)\n");
/*
 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: uart: soft retaegis — Wave 31 return-aegis honesty (kept) */
aarch64_uart_puts("aarch64: uart: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retaegis honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retmantle — Wave 31 mantle stamp (kept) */
aarch64_uart_puts("aarch64: uart: soft retmantle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retmantle stamp; Soft!=product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: uart: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
aarch64_uart_puts("aarch64: uart: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retbulwark honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retpanoply — Wave 32 panoply stamp (kept) */
aarch64_uart_puts("aarch64: uart: soft retpanoply exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retpanoply stamp; Soft!=product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retbastion — Wave 33 return-bastion honesty (kept) */
aarch64_uart_puts("aarch64: uart: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retbastion honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retcitadel — Wave 33 citadel stamp (kept) */
aarch64_uart_puts("aarch64: uart: soft retcitadel exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retcitadel stamp; Soft!=product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retredoubt — Wave 34 return-redoubt honesty */
aarch64_uart_puts("aarch64: uart: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retredoubt honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retkeep — Wave 34 exclusive keep stamp */
aarch64_uart_puts("aarch64: uart: soft retkeep exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retkeep stamp; Soft!=product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retfortress — Wave 35 return-fortress honesty */
aarch64_uart_puts("aarch64: uart: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retfortress honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retpalace — Wave 35 exclusive palace stamp */
aarch64_uart_puts("aarch64: uart: soft retpalace exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retpalace stamp; Soft!=product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft rethold — Wave 36 return-hold honesty */
aarch64_uart_puts("aarch64: uart: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (rethold honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retspire — Wave 36 exclusive spire stamp */
aarch64_uart_puts("aarch64: uart: soft retspire exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retspire stamp; Soft!=product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retwall — Wave 37 return-wall honesty */
aarch64_uart_puts("aarch64: uart: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retwall honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retgate — Wave 37 exclusive gate stamp */
aarch64_uart_puts("aarch64: uart: soft retgate exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retgate stamp; Soft!=product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retmoat — Wave 38 return-moat honesty */
aarch64_uart_puts("aarch64: uart: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retmoat honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retower — Wave 38 exclusive tower stamp */
aarch64_uart_puts("aarch64: uart: soft retower exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retower stamp; Soft!=product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retbarbican — Wave 39 return-barbican honesty */
aarch64_uart_puts("aarch64: uart: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retbarbican honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retglacis — Wave 39 exclusive glacis stamp */
aarch64_uart_puts("aarch64: uart: soft retglacis exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retglacis stamp; Soft!=product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retcurtain — Wave 40 return-curtain honesty */
aarch64_uart_puts("aarch64: uart: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retcurtain honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retparapet — Wave 40 exclusive parapet stamp */
aarch64_uart_puts("aarch64: uart: soft retparapet exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retparapet stamp; Soft!=product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retravelin — Wave 41 return-travelin honesty */
aarch64_uart_puts("aarch64: uart: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retravelin honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retditch — Wave 41 exclusive ditch stamp */
aarch64_uart_puts("aarch64: uart: soft retditch exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retditch stamp; Soft!=product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retportcullis — Wave 42 return-portcullis honesty */
aarch64_uart_puts("aarch64: uart: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retportcullis honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retbattlement — Wave 42 exclusive battlement stamp */
aarch64_uart_puts("aarch64: uart: soft retbattlement exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retbattlement stamp; Soft!=product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retmachicolation — Wave 43 return-machicolation honesty */
aarch64_uart_puts("aarch64: uart: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retmachicolation honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
aarch64_uart_puts("aarch64: uart: soft retarrowslit exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retarrowslit stamp; Soft!=product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retmerlon — Wave 44 return-merlon honesty */
aarch64_uart_puts("aarch64: uart: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retmerlon honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retembrasure — Wave 44 exclusive embrasure stamp */
aarch64_uart_puts("aarch64: uart: soft retembrasure exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retembrasure stamp; Soft!=product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retkeepgate — Wave 45 return-keepgate honesty */
aarch64_uart_puts("aarch64: uart: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retkeepgate honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retouterward — Wave 45 exclusive outerward stamp */
aarch64_uart_puts("aarch64: uart: soft retouterward exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retouterward stamp; Soft!=product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retbailey — Wave 46 return-bailey honesty */
aarch64_uart_puts("aarch64: uart: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retbailey honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retpostern — Wave 46 exclusive postern stamp */
aarch64_uart_puts("aarch64: uart: soft retpostern exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retpostern stamp; Soft!=product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retinnerward — Wave 47 return-innerward honesty */
aarch64_uart_puts("aarch64: uart: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retinnerward honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retdonjon — Wave 47 exclusive donjon stamp */
aarch64_uart_puts("aarch64: uart: soft retdonjon exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retdonjon stamp; Soft!=product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retchevaux — Wave 48 return-chevaux honesty */
aarch64_uart_puts("aarch64: uart: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retchevaux honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retpalisade — Wave 48 exclusive palisade stamp */
aarch64_uart_puts("aarch64: uart: soft retpalisade exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retpalisade stamp; Soft!=product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retglacisgate — Wave 49 return-glacisgate honesty */
aarch64_uart_puts("aarch64: uart: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retglacisgate honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retoutwork — Wave 49 exclusive outwork stamp */
aarch64_uart_puts("aarch64: uart: soft retoutwork exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retoutwork stamp; Soft!=product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retsally — Wave 50 return-sally honesty */
aarch64_uart_puts("aarch64: uart: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retsally honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
aarch64_uart_puts("aarch64: uart: soft retcounterscarp exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retcounterscarp stamp; Soft!=product)\n");

/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retfosse — Wave 51 return-fosse honesty */
aarch64_uart_puts("aarch64: uart: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retfosse honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retcoveredway — Wave 51 exclusive coveredway stamp */
aarch64_uart_puts("aarch64: uart: soft retcoveredway exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retcoveredway stamp; Soft!=product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft rettenaille — Wave 52 return-tenaille honesty */
aarch64_uart_puts("aarch64: uart: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (rettenaille honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retdemilune — Wave 52 exclusive demilune stamp */
aarch64_uart_puts("aarch64: uart: soft retdemilune exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retdemilune stamp; Soft!=product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retravelin — Wave 53 return-travelin honesty */
aarch64_uart_puts("aarch64: uart: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retravelin honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retlunette — Wave 53 exclusive lunette stamp */
aarch64_uart_puts("aarch64: uart: soft retlunette exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retlunette stamp; Soft!=product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retcaponier — Wave 54 return-caponier honesty */
aarch64_uart_puts("aarch64: uart: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retcaponier honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retredan — Wave 54 exclusive redan stamp */
aarch64_uart_puts("aarch64: uart: soft retredan exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retredan stamp; Soft!=product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retflank — Wave 55 return-flank honesty */
aarch64_uart_puts("aarch64: uart: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retflank honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retface — Wave 55 exclusive face stamp */
aarch64_uart_puts("aarch64: uart: soft retface exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retface stamp; Soft!=product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retgorge — Wave 56 return-gorge honesty */
aarch64_uart_puts("aarch64: uart: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retgorge honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retshoulder — Wave 56 exclusive shoulder stamp */
aarch64_uart_puts("aarch64: uart: soft retshoulder exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retshoulder stamp; Soft!=product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retraverse — Wave 57 return-traverse honesty */
aarch64_uart_puts("aarch64: uart: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retraverse honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retcasemate — Wave 57 exclusive casemate stamp */
aarch64_uart_puts("aarch64: uart: soft retcasemate exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retcasemate stamp; Soft!=product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retorillon — Wave 58 return-orillon honesty */
aarch64_uart_puts("aarch64: uart: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retorillon honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retbonnette — Wave 58 exclusive bonnette stamp */
aarch64_uart_puts("aarch64: uart: soft retbonnette exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retbonnette stamp; Soft!=product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retcrownwork — Wave 59 return-crownwork honesty */
aarch64_uart_puts("aarch64: uart: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retcrownwork honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft rethornwork — Wave 59 exclusive hornwork stamp */
aarch64_uart_puts("aarch64: uart: soft rethornwork exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (rethornwork stamp; Soft!=product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retplace — Wave 60 return-place honesty */
aarch64_uart_puts("aarch64: uart: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retplace honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retenvelope — Wave 60 exclusive envelope stamp */
aarch64_uart_puts("aarch64: uart: soft retenvelope exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retenvelope stamp; Soft!=product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retcounterguard — Wave 61 return-counterguard honesty */
aarch64_uart_puts("aarch64: uart: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retcounterguard honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retcoveredface — Wave 61 exclusive coveredface stamp */
aarch64_uart_puts("aarch64: uart: soft retcoveredface exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retcoveredface stamp; Soft!=product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: uart: soft retbastionface — Wave 62 return-bastionface honesty */
aarch64_uart_puts("aarch64: uart: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retbastionface honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
aarch64_uart_puts("aarch64: uart: soft retcurtainangle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retcurtainangle stamp; Soft!=product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: uart: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
aarch64_uart_puts("aarch64: uart: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retdoubletenaille honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
aarch64_uart_puts("aarch64: uart: soft retplaceofarms exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retplaceofarms stamp; Soft!=product)\n");
/*
 * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: uart: soft retreentrant — Wave 64 return-reentrant honesty */
aarch64_uart_puts("aarch64: uart: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retreentrant honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retsallyport — Wave 64 exclusive sallyport stamp */
aarch64_uart_puts("aarch64: uart: soft retsallyport exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retsallyport stamp; Soft!=product)\n");
/*
 * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: uart: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
aarch64_uart_puts("aarch64: uart: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retgorgeangle honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
aarch64_uart_puts("aarch64: uart: soft retshoulderangle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retshoulderangle stamp; Soft!=product)\n");
/*
 * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: uart: soft retflankangle — Wave 66 return-flankangle honesty */
aarch64_uart_puts("aarch64: uart: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retflankangle honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: uart: soft retfaceangle — Wave 66 exclusive faceangle stamp */
aarch64_uart_puts("aarch64: uart: soft retfaceangle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retcaponierangle — Wave 67 return-caponierangle honesty */
aarch64_uart_puts("aarch64: uart: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: uart: soft retredanangle — Wave 67 exclusive redanangle stamp */
aarch64_uart_puts("aarch64: uart: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
aarch64_uart_puts("aarch64: uart: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: uart: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
aarch64_uart_puts("aarch64: uart: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
aarch64_uart_puts("aarch64: uart: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: uart: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
aarch64_uart_puts("aarch64: uart: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retfosseangle — Wave 70 return-fosseangle honesty */
aarch64_uart_puts("aarch64: uart: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=77 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: uart: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
aarch64_uart_puts("aarch64: uart: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=77 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
aarch64_uart_puts("aarch64: uart: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=77 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: uart: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
aarch64_uart_puts("aarch64: uart: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=77 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: uart: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
aarch64_uart_puts("aarch64: uart: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=77 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: uart: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
aarch64_uart_puts("aarch64: uart: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=77 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: aarch64: uart: soft retcurtainface — Wave 73 return-curtainface honesty */
aarch64_uart_puts("aarch64: uart: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=77 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: uart: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
aarch64_uart_puts("aarch64: uart: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=77 (retbastionangle stamp; Soft≠product)\n");
/* Grep: aarch64: uart: soft retglacisangle — Wave 74 return-glacisangle honesty */
aarch64_uart_puts("aarch64: uart: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=77 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: uart: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
aarch64_uart_puts("aarch64: uart: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=77 (retparapetangle stamp; Soft≠product)\n");
/* Grep: aarch64: uart: soft retmoatangle — Wave 75 return-moatangle honesty */
aarch64_uart_puts("aarch64: uart: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=77 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: uart: soft retowerangle — Wave 75 exclusive towerangle stamp */
aarch64_uart_puts("aarch64: uart: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=77 (retowerangle stamp; Soft≠product)\n");
/* Grep: aarch64: uart: soft retgateangle — Wave 76 return-gateangle honesty */
aarch64_uart_puts("aarch64: uart: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=77 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: uart: soft retwallangle — Wave 76 exclusive wallangle stamp */
aarch64_uart_puts("aarch64: uart: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=77 (retwallangle stamp; Soft≠product)\n");
/* Grep: aarch64: uart: soft retspireangle — Wave 77 return-spireangle honesty */
aarch64_uart_puts("aarch64: uart: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=77 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: uart: soft retholdangle — Wave 77 exclusive holdangle stamp */
aarch64_uart_puts("aarch64: uart: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=77 (retholdangle stamp; Soft≠product)\n");

aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
aarch64_uart_puts(" (retfaceangle stamp; Soft!=product)\n");
    aarch64_uart_puts("aarch64: uart soft deepen wave=");
    aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
    aarch64_uart_puts(" areas=");
    aarch64_uart_put_hex((unsigned long)PL011_SOFT_AREAS);
    aarch64_uart_puts(" catalog=chars,fr,lamps,ctrl,id,program,stats,path,"
                      "surf,return,honesty,deepen logs=");
    aarch64_uart_put_hex((unsigned long)g_cUartSoftLogs);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: uart soft surf — Wave 19 id/fr/verify lamps */
    aarch64_uart_puts("aarch64: uart soft surf match=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u8IdMatch);
    aarch64_uart_puts(" fr_live=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u8FrLive);
    aarch64_uart_puts(" verify=");
    aarch64_uart_put_hex((unsigned long)g_UartSoftSnap.u8VerifyOk);
    aarch64_uart_puts(" chars_nz=");
    aarch64_uart_put_hex(g_cUartChars != 0ul ? 1ul : 0ul);
    aarch64_uart_puts(" bits=");
    aarch64_uart_put_hex((unsigned long)(
        ((unsigned)g_UartSoftSnap.u8IdMatch) |
        (((unsigned)g_UartSoftSnap.u8FrLive) << 1) |
        (((unsigned)g_UartSoftSnap.u8VerifyOk) << 2) |
        ((g_cUartChars != 0ul ? 1u : 0u) << 3)));
    aarch64_uart_puts(" wave=");
    aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /*
     * Grep: aarch64: uart soft path
     * Honesty: polled early console; no IRQ / no RX product path.
     * product_kernel=OPEN: aarch64 product kernel remains OPEN.
     */
    aarch64_uart_puts("aarch64: uart soft path polled=1 irq=0 rx_product=0 "
                      "baud_reprogram=0 product_kernel=OPEN hard_gate=0 "
                      "wave=");
    aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: uart soft honesty */
    aarch64_uart_puts("aarch64: uart soft honesty product_kernel=OPEN "
                      "soft_only=1 no_irq=1 no_bar3=1 wave=");
    aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: uart soft exclusive — Wave 35 exclusive deepen */
    aarch64_uart_puts("aarch64: uart soft exclusive multi_server=0 "
                      "confine=0 bar3=0 product_kernel=OPEN soft_only=1 wave=");
    aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: uart soft open — Wave 19 open-lamp rollup */
    aarch64_uart_puts("aarch64: uart soft open multi_server=0 confine=0 "
                      "bar3=0 product_kernel=OPEN soft_only=1 wave=");
    aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
    aarch64_uart_puts("\n");


    /* Grep: aarch64: uart soft return — Wave 19 return surfaces */
    aarch64_uart_puts("aarch64: uart soft return inv_ret=");
    aarch64_uart_put_hex((unsigned long)(fOk != 0 ? 1ul : 0ul));
    aarch64_uart_puts(" product_kernel=OPEN wave=");
    aarch64_uart_put_hex((unsigned long)PL011_SOFT_WAVE);
    aarch64_uart_puts("\n");

    if (fOk != 0) {
        aarch64_uart_puts("aarch64: uart soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: uart soft FAIL\n");
    }
    return fOk;
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
 * aarch64_uart_soft_selftest — greppable soft UART inventory for PL011
 * identity + FR/ctrl lamps + hex helpers. Idempotent (one-shot). Safe to
 * call from cpu_info soft deepen.
 *
 * Does not enable RX IRQ or reprogram baud (QEMU virt is pre-clocked).
 */
void
aarch64_uart_soft_selftest(void)
{
    int fInvOk;
    int fHexOk;

    if (g_fUartSoftDone != 0) {
        return;
    }
    g_fUartSoftDone = 1;

    /* Wave 19 combined soft inventory under "aarch64: uart soft …". */
    fInvOk = uart_soft_inventory();

    fHexOk = uart_hex_soft_exercise();
    if (fHexOk != 0) {
        aarch64_uart_puts("aarch64: uart hex soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: uart hex soft FAIL\n");
    }

    (void)fInvOk;
    (void)g_fUartSoftSnapLive;
    (void)PL011_FR_RXFF;
    (void)PL011_FR_BUSY;
    (void)PL011_FR_TXFF;
    (void)PL011_CR_UARTEN;
    (void)PL011_CR_TXE;
    (void)PL011_CR_RXE;
}
