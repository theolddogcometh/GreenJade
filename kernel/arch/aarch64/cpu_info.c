/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Early CPU identity probe for aarch64 product scaffold (QEMU virt).
 * Greppable: aarch64: cpu PASS
 */
#include "types_arch.h"

extern void aarch64_uart_puts(const char *sz);
extern void aarch64_uart_put_hex(unsigned long v);

void
aarch64_cpu_probe(void)
{
    unsigned long mpidr;
    unsigned long midr;
    unsigned long currentel;
    unsigned long el;

    __asm__ volatile("mrs %0, mpidr_el1" : "=r"(mpidr));
    __asm__ volatile("mrs %0, midr_el1" : "=r"(midr));
    __asm__ volatile("mrs %0, CurrentEL" : "=r"(currentel));
    el = (currentel >> 2) & 0x3ul;

    aarch64_uart_puts("aarch64: cpu EL=");
    aarch64_uart_put_hex(el);
    aarch64_uart_puts(" mpidr=");
    aarch64_uart_put_hex(mpidr);
    aarch64_uart_puts(" midr=");
    aarch64_uart_put_hex(midr);
    aarch64_uart_puts("\n");
    aarch64_uart_puts("aarch64: cpu PASS\n");
}
