/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Portable early console (shared C kernel). Arch provides putchar/write:
 *   x86_64  → COM1 serial
 *   aarch64 → PL011 UART
 */
#pragma once

void console_putchar(char chOut);
void console_write(const char *szText);
