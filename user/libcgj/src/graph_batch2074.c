/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2074: C string-literal escape expanded length.
 *
 * Surface (unique symbols):
 *   size_t gj_str_c_escape_len(const char *s, size_t n);
 *     — Return the number of octets needed to escape the n-byte sequence
 *       s as a C string-literal body (no surrounding quotes). Escapes:
 *         \ " → 2 (\X)
 *         \a \b \f \n \r \t \v → 2 each
 *         other 0x00..0x1f, 0x7f, and 0x80..0xff → 4 (\xHH)
 *         printable ASCII 0x20..0x7e except \ " → 1
 *       NULL s with n > 0 → 0. n == 0 → 0 (empty input).
 *   size_t __gj_str_c_escape_len  (alias)
 *   __libcgj_batch2074_marker = "libcgj-batch2074"
 *
 * Str encode exclusive wave (2071–2080). Length-only; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2074_marker[] = "libcgj-batch2074";

/* ---- freestanding helpers ---------------------------------------------- */

/* Output byte count for one input octet (C string body, excl. quotes). */
static size_t
b2074_esc_len(unsigned char u8C)
{
	if (u8C == (unsigned char)'\\' || u8C == (unsigned char)'"') {
		return 2u;
	}
	if (u8C == (unsigned char)'\a' || u8C == (unsigned char)'\b' ||
	    u8C == (unsigned char)'\f' || u8C == (unsigned char)'\n' ||
	    u8C == (unsigned char)'\r' || u8C == (unsigned char)'\t' ||
	    u8C == (unsigned char)'\v') {
		return 2u;
	}
	if (u8C < 0x20u || u8C == 0x7fu || u8C >= 0x80u) {
		return 4u; /* \xHH */
	}
	return 1u;
}

static size_t
b2074_escape_len(const char *sz, size_t n)
{
	size_t cbOut;
	size_t i;

	cbOut = 0u;
	for (i = 0u; i < n; i++) {
		cbOut += b2074_esc_len((unsigned char)sz[i]);
	}
	return cbOut;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_c_escape_len — expanded length after C string-literal escaping.
 *
 * s: input bytes (NULL with n > 0 → 0)
 * n: byte count to measure
 *
 * Binary-safe; does not call libc. Result excludes surrounding quotes
 * and a trailing NUL.
 */
size_t
gj_str_c_escape_len(const char *s, size_t n)
{
	(void)NULL;
	if (n == 0u) {
		return 0u;
	}
	if (s == NULL) {
		return 0u;
	}
	return b2074_escape_len(s, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_c_escape_len(const char *s, size_t n)
    __attribute__((alias("gj_str_c_escape_len")));
