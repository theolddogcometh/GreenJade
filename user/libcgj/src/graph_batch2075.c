/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2075: POSIX shell single-quote escape length.
 *
 * Surface (unique symbols):
 *   size_t gj_str_shell_escape_len(const char *s, size_t n);
 *     — Return the number of octets needed to shell-escape the n-byte
 *       sequence s using POSIX single-quoting:
 *         wrap the whole result in single quotes
 *         each interior '\'' becomes: '\''  (four octets)
 *         every other octet is copied as-is (one octet)
 *       Total always includes the two surrounding quotes (empty → 2).
 *       NULL s with n > 0 → 0. n == 0 → 2 ("''").
 *   size_t __gj_str_shell_escape_len  (alias)
 *   __libcgj_batch2075_marker = "libcgj-batch2075"
 *
 * Str encode exclusive wave (2071–2080). Length-only; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2075_marker[] = "libcgj-batch2075";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2075_escape_len — POSIX single-quote wrap length for sz[0..n).
 * Surrounding quotes always contribute 2. Each '\'' expands to 4.
 */
static size_t
b2075_escape_len(const char *sz, size_t n)
{
	size_t cbOut;
	size_t i;

	cbOut = 2u; /* opening + closing ' */
	for (i = 0u; i < n; i++) {
		if ((unsigned char)sz[i] == (unsigned char)'\'') {
			/* end quote, escaped quote, reopen: '\'' */
			cbOut += 4u;
		} else {
			cbOut += 1u;
		}
	}
	return cbOut;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_shell_escape_len — expanded length after shell single-quote escape.
 *
 * s: input bytes (NULL with n > 0 → 0; n == 0 → 2 for "''")
 * n: byte count to measure
 *
 * Binary-safe; does not call libc. Result excludes a trailing NUL.
 */
size_t
gj_str_shell_escape_len(const char *s, size_t n)
{
	(void)NULL;
	if (n == 0u) {
		return 2u; /* '' */
	}
	if (s == NULL) {
		return 0u;
	}
	return b2075_escape_len(s, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_shell_escape_len(const char *s, size_t n)
    __attribute__((alias("gj_str_shell_escape_len")));
