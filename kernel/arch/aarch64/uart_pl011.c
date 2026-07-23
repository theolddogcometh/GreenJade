/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * PL011 UART early putchar / puts / hex helpers for QEMU virt
 * (scaffold; not in x86_64 build). Base: 0x09000000 on virt machine.
 *
 * Shared serial hex dump helper: aarch64_uart_put_hex / put_hex_n /
 * put_hex_dump — used by exception, svc, cpu_info (and future PSCI soft).
 */
#include <gj/console.h>
#include <gj/types.h>

#ifndef GJ_AARCH64_UART_BASE
#define GJ_AARCH64_UART_BASE 0x09000000ull
#endif

/* PL011 registers (word offsets) */
#define PL011_DR   0x00u
#define PL011_FR   0x18u
#define PL011_FR_TXFF (1u << 5)

void
aarch64_uart_putchar(char ch)
{
    volatile u32 *pBase = (volatile u32 *)(gj_vaddr_t)GJ_AARCH64_UART_BASE;
    u32 spins;

    if (ch == '\n') {
        aarch64_uart_putchar('\r');
    }
    for (spins = 0; spins < 100000u; spins++) {
        if ((pBase[PL011_FR / 4u] & PL011_FR_TXFF) == 0) {
            break;
        }
    }
    pBase[PL011_DR / 4u] = (u32)(u8)ch;
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
    char a[19];
    int i;

    a[0] = '0';
    a[1] = 'x';
    for (i = 0; i < 16; i++) {
        a[2 + i] = dig[(v >> (60 - 4 * i)) & 0xfu];
    }
    a[18] = '\0';
    aarch64_uart_puts(a);
}

/*
 * Print low nibble count (1..16) of v as hex without forcing full width.
 * cNibbles is clamped to [1, 16]. Always prefixes "0x".
 */
void
aarch64_uart_put_hex_n(unsigned long v, unsigned cNibbles)
{
    static const char dig[] = "0123456789abcdef";
    char a[19];
    unsigned i;
    unsigned shift;

    if (cNibbles == 0u) {
        cNibbles = 1u;
    }
    if (cNibbles > 16u) {
        cNibbles = 16u;
    }
    a[0] = '0';
    a[1] = 'x';
    for (i = 0; i < cNibbles; i++) {
        shift = (cNibbles - 1u - i) * 4u;
        a[2u + i] = dig[(v >> shift) & 0xfu];
    }
    a[2u + cNibbles] = '\0';
    aarch64_uart_puts(a);
}

/*
 * Soft hex dump of cBytes at p (may be MMIO or DRAM). Bytes as "xx " groups;
 * newline every 16. cBytes capped at 256 for early-console safety.
 */
void
aarch64_uart_put_hex_dump(const void *p, unsigned cBytes)
{
    const volatile unsigned char *pb;
    unsigned i;
    static const char dig[] = "0123456789abcdef";

    if (p == NULL || cBytes == 0u) {
        return;
    }
    if (cBytes > 256u) {
        cBytes = 256u;
    }
    pb = (const volatile unsigned char *)p;
    for (i = 0; i < cBytes; i++) {
        unsigned char b = pb[i];
        aarch64_uart_putchar(dig[(b >> 4) & 0xfu]);
        aarch64_uart_putchar(dig[b & 0xfu]);
        aarch64_uart_putchar(' ');
        if (((i + 1u) & 15u) == 0u) {
            aarch64_uart_putchar('\n');
        }
    }
    if ((cBytes & 15u) != 0u) {
        aarch64_uart_putchar('\n');
    }
}
