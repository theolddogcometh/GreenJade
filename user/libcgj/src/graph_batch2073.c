/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2073: JSON string-body escape expanded length.
 *
 * Surface (unique symbols):
 *   size_t gj_str_json_escape_len(const char *s, size_t n);
 *     — Return the number of octets needed to JSON-escape the n-byte
 *       sequence s as a JSON string body (no surrounding quotes).
 *       Escapes:
 *         " \ and short controls \b \f \n \r \t → 2 each
 *         other controls 0x00..0x1f → 6 (\u00XX)
 *         all other octets → 1
 *       NULL s with n > 0 → 0. n == 0 → 0 (empty input).
 *   size_t __gj_str_json_escape_len  (alias)
 *   __libcgj_batch2073_marker = "libcgj-batch2073"
 *
 * Str encode exclusive wave (2071–2080). Distinct from
 * gj_json_escape_len (batch1135) by the gj_str_ prefix — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2073_marker[] = "libcgj-batch2073";

/* ---- freestanding helpers ---------------------------------------------- */

/* Output byte count for one input octet (JSON string body, excl. quotes). */
static size_t
b2073_esc_len(unsigned char u8C)
{
	if (u8C == (unsigned char)'"' || u8C == (unsigned char)'\\' ||
	    u8C == (unsigned char)'\b' || u8C == (unsigned char)'\f' ||
	    u8C == (unsigned char)'\n' || u8C == (unsigned char)'\r' ||
	    u8C == (unsigned char)'\t') {
		return 2u; /* \X short form */
	}
	if (u8C < 0x20u) {
		return 6u; /* \u00XX */
	}
	return 1u;
}

static size_t
b2073_escape_len(const char *sz, size_t n)
{
	size_t cbOut;
	size_t i;

	cbOut = 0u;
	for (i = 0u; i < n; i++) {
		cbOut += b2073_esc_len((unsigned char)sz[i]);
	}
	return cbOut;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_json_escape_len — expanded length after JSON string escaping.
 *
 * s: input bytes (NULL with n > 0 → 0)
 * n: byte count to measure
 *
 * Binary-safe; does not call libc. Result excludes surrounding quotes
 * and a trailing NUL.
 */
size_t
gj_str_json_escape_len(const char *s, size_t n)
{
	(void)NULL;
	if (n == 0u) {
		return 0u;
	}
	if (s == NULL) {
		return 0u;
	}
	return b2073_escape_len(s, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_json_escape_len(const char *s, size_t n)
    __attribute__((alias("gj_str_json_escape_len")));
