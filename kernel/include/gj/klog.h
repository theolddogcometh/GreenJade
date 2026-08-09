/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Kernel log / serial surface (dual MIT OR Apache-2.0).
 *
 * H2 residual lean (Soft!=product; stamp storm guard):
 *   kprintf is stack-safe: nested depth bounded, soft inventory rate-capped
 *   (pow2 milestones + hard log_max), tiny digit buffers, %s length-capped,
 *   soft dump never re-enters kprintf (console_* + print_u64 only).
 *   panic never nests kprintf. No product version stamp in soft residual.
 * greppable (impl stdio_k.c): kprintf: soft residual lean
 * greppable (impl stdio_k.c): stdio_k: soft residual lean
 */
#pragma once

#include <stdarg.h>
#include <gj/types.h>

void serial_init(void);
void serial_putchar(char chOut);
void serial_write(const char *szText);
/** Non-blocking: 1 if byte available, 0 otherwise */
int  serial_poll(void);
/** Blocking read one byte from COM1 */
int  serial_getchar(void);
/**
 * Soft: non-zero if COM1 never raised LSR.THRE (panel-only DUT, no legacy UART).
 * Sticky after first full soft spin miss in serial_putchar. Callers (pmm/vmm
 * soft inventory) may skip multi-KiB floods on dead COM1. QEMU Multiboot with
 * working THRE leaves this 0 and keeps the full TX spin budget.
 * Soft!=product: dead-COM1 skip is advisory flood guard (H2), not product AC.
 */
u32  serial_thre_dead(void);

/**
 * Freestanding kprintf (%% %c %s %d %i %u %x %p; l/ll; 0-width).
 * Stack-safe residual (H2): depth-capped; soft inventory lean + stamp_storm=0.
 * Soft!=product. Implementation: kernel/lib/stdio_k.c.
 */
void kprintf(const char *szFmt, ...);
/**
 * Halt with message. Never nests kprintf (H2 stack safety). Soft!=product.
 */
void panic(const char *szMsg);
