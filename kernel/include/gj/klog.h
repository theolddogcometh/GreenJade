/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 */
#pragma once

#include <stdarg.h>

void serial_init(void);
void serial_putchar(char chOut);
void serial_write(const char *szText);
/** Non-blocking: 1 if byte available, 0 otherwise */
int  serial_poll(void);
/** Blocking read one byte from COM1 */
int  serial_getchar(void);

void kprintf(const char *szFmt, ...);
void panic(const char *szMsg);
