/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Portable early console soft API (shared C kernel surface).
 * Pure C11 freestanding; dual-licensed MIT OR Apache-2.0.
 *
 * -------------------------------------------------------------------------
 * Role
 * -------------------------------------------------------------------------
 * Thin, arch-independent TX surface used by freestanding kprintf / panic
 * (stdio_k) and early bring-up paths. Shared C never programs a UART; each
 * product arch supplies console_putchar / console_write:
 *
 *   x86_64  → COM1-class 16550 path (serial_putchar / serial_write)
 *   aarch64 → QEMU virt PL011 @ 0x09000000 (aarch64_uart_putchar / puts)
 *
 * This header is the soft contract only. Arch soft observability (port
 * snaps, FR/ID peeks, spin budgets, greppable PASS/FAIL) lives in the
 * arch unit; callers stay backend-agnostic.
 *
 * -------------------------------------------------------------------------
 * Soft contracts (deepened; no redesign)
 * -------------------------------------------------------------------------
 * - TX only: no poll / getchar / RX in this API (arch may expose RX
 *   under a different module name — not through console_*).
 * - Polled, blocking-ish TX: backends spin-wait for THRE / !TXFF with a
 *   finite soft spin budget, then write the data register. No IRQ TX.
 * - Newline soft: backends expand '\n' → '\r' then '\n' so host serial
 *   consoles stay aligned (CRLF). Other control bytes pass through.
 * - console_write(NULL) is a soft no-op (safe for optional banners).
 * - console_write stops at the first NUL; no length parameter (C string).
 * - No locking / no multi-CPU serialization here: concurrent putchar from
 *   several CPUs may interleave bytes. Early console + soft diagnostics
 *   only — not a userspace TTY.
 * - Init order: arch must program (or inherit) the UART before meaningful
 *   output. Pre-init putchar may drop or hang on dead silicon; product
 *   bring-up calls the arch init first (e.g. serial_init on x86_64).
 * - No heap, no format: formatting is kprintf's job; this API is raw.
 *
 * Greppable consumers (implementation elsewhere):
 *   kprintf / panic  → console_putchar / console_write
 *   serial: soft *   → x86 backend observability
 *   aarch64: uart soft * → PL011 backend observability
 */
#pragma once

/**
 * Emit one character on the early console (arch backend).
 *
 * Soft:
 *   - '\n' is CRLF-expanded by the arch putchar (recursive/tail CR).
 *   - May spin briefly for TX ready (soft spin budget; not scheduleable).
 *   - Safe after arch UART init; undefined usefulness before that.
 *   - Not IRQ-reentrant vs concurrent putchar on other CPUs (byte race OK
 *     for soft logs; avoid for atomic multi-char records without external
 *     serialization).
 *
 * @param chOut  character to transmit (8-bit value; signedness ignored).
 */
void console_putchar(char chOut);

/**
 * Emit a NUL-terminated string on the early console (arch backend).
 *
 * Soft:
 *   - NULL → immediate return (no crash, no output).
 *   - Equivalent to putchar for each byte until '\0'.
 *   - CRLF expansion applies per '\n' via putchar.
 *   - No return length / error code (fire-and-forget early console).
 *
 * Prefer kprintf for formatted diagnostics; use console_write for fixed
 * banners and raw strings when format overhead is unwanted.
 *
 * @param szText  C string, or NULL for soft no-op.
 */
void console_write(const char *szText);
