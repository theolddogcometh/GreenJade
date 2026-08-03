/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
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
 */
u32  serial_thre_dead(void);

void kprintf(const char *szFmt, ...);
void panic(const char *szMsg);
