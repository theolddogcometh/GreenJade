/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2513: JSON string-body unescape expanded length.
 *
 * Surface (unique symbols):
 *   size_t gj_json_unescape_len_u(const char *s, size_t n);
 *     - Return octets produced by unescaping JSON string body s[0..n)
 *       (no surrounding quotes). Supported escapes:
 *         \" \\ \/ \b \f \n \r \t → 1 output each
 *         \uXXXX (four hex digits) → 1 output (low 8 bits of code unit)
 *       Bare control 0x00..0x1f, truncated / illegal escape, or NULL s
 *       with n > 0 → 0 (error / empty). n == 0 → 0.
 *   size_t __gj_json_unescape_len_u  (alias)
 *   __libcgj_batch2513_marker = "libcgj-batch2513"
 *
 * JSON exclusive lightweight wave (2511-2520). Distinct from
 * gj_json_unescape (batch272 write form) — length-only; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2513_marker[] = "libcgj-batch2513";

/* ---- freestanding helpers ---------------------------------------------- */

/* Map one ASCII hex digit to 0..15, or -1. */
static int
b2513_hexval(unsigned char u8C)
{
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return (int)(u8C - (unsigned char)'0');
	}
	if (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'f') {
		return (int)(u8C - (unsigned char)'a') + 10;
	}
	if (u8C >= (unsigned char)'A' && u8C <= (unsigned char)'F') {
		return (int)(u8C - (unsigned char)'A') + 10;
	}
	return -1;
}

/*
 * Measure unescaped length of JSON string body s[0..n).
 * Returns 0 on error (illegal sequence) or empty/NULL.
 */
static size_t
b2513_unescape_len(const char *sz, size_t cN)
{
	size_t cbOut;
	size_t i;

	if (sz == NULL || cN == 0u) {
		return 0u;
	}

	cbOut = 0u;
	i = 0u;
	while (i < cN) {
		unsigned char u8C = (unsigned char)sz[i];

		/* Unescaped controls illegal in JSON strings. */
		if (u8C < 0x20u) {
			return 0u;
		}

		if (u8C != (unsigned char)'\\') {
			cbOut++;
			i++;
			continue;
		}

		/* Escape: need at least one more byte. */
		i++;
		if (i >= cN) {
			return 0u;
		}
		u8C = (unsigned char)sz[i];
		i++;

		switch (u8C) {
		case (unsigned char)'"':
		case (unsigned char)'\\':
		case (unsigned char)'/':
		case (unsigned char)'b':
		case (unsigned char)'f':
		case (unsigned char)'n':
		case (unsigned char)'r':
		case (unsigned char)'t':
			cbOut++;
			break;
		case (unsigned char)'u':
			/* Require four hex digits; emit one octet (low 8). */
			if (i + 4u > cN) {
				return 0u;
			}
			if (b2513_hexval((unsigned char)sz[i]) < 0 ||
			    b2513_hexval((unsigned char)sz[i + 1u]) < 0 ||
			    b2513_hexval((unsigned char)sz[i + 2u]) < 0 ||
			    b2513_hexval((unsigned char)sz[i + 3u]) < 0) {
				return 0u;
			}
			i += 4u;
			cbOut++;
			break;
		default:
			return 0u;
		}
	}
	return cbOut;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_unescape_len_u - output length after unescaping JSON string body.
 *
 * s: escaped body bytes (no surrounding quotes)
 * n: byte count
 *
 * Returns produced octet count on success, or 0 on error / empty input.
 * Does not write an output buffer. Does not call libc.
 */
size_t
gj_json_unescape_len_u(const char *sz, size_t cN)
{
	(void)NULL;
	return b2513_unescape_len(sz, cN);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_json_unescape_len_u(const char *sz, size_t cN)
    __attribute__((alias("gj_json_unescape_len_u")));
