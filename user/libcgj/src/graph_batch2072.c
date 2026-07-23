/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2072: URL percent-encode expanded length.
 *
 * Surface (unique symbols):
 *   size_t gj_str_url_encode_len(const char *s, size_t n);
 *     — Return the number of octets needed to RFC 3986 percent-encode
 *       the n-byte sequence s (no trailing NUL counted). Unreserved
 *       octets (ALPHA / DIGIT / "-" / "." / "_" / "~") contribute 1;
 *       all others contribute 3 ("%HH"). NULL s with n > 0 → 0.
 *       n == 0 → 0 (empty input).
 *   size_t __gj_str_url_encode_len  (alias)
 *   __libcgj_batch2072_marker = "libcgj-batch2072"
 *
 * Str encode exclusive wave (2071–2080). Distinct from gj_url_encode /
 * gj_pct_encode (writers) — length-only; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2072_marker[] = "libcgj-batch2072";

/* ---- freestanding helpers ---------------------------------------------- */

/* RFC 3986 §2.3 unreserved: ALPHA / DIGIT / "-" / "." / "_" / "~" */
static int
b2072_is_unreserved(unsigned char u8C)
{
	if (u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') {
		return 1;
	}
	if (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'z') {
		return 1;
	}
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return 1;
	}
	if (u8C == (unsigned char)'-' || u8C == (unsigned char)'.' ||
	    u8C == (unsigned char)'_' || u8C == (unsigned char)'~') {
		return 1;
	}
	return 0;
}

static size_t
b2072_encode_len(const char *sz, size_t n)
{
	size_t cbOut;
	size_t i;
	unsigned char u8C;

	cbOut = 0u;
	for (i = 0u; i < n; i++) {
		u8C = (unsigned char)sz[i];
		if (b2072_is_unreserved(u8C)) {
			cbOut += 1u;
		} else {
			cbOut += 3u; /* %HH */
		}
	}
	return cbOut;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_url_encode_len — expanded length after RFC 3986 percent-encode.
 *
 * s: input bytes (NULL with n > 0 → 0)
 * n: byte count to measure
 *
 * Binary-safe; does not call libc. Result excludes a trailing NUL.
 */
size_t
gj_str_url_encode_len(const char *s, size_t n)
{
	(void)NULL;
	if (n == 0u) {
		return 0u;
	}
	if (s == NULL) {
		return 0u;
	}
	return b2072_encode_len(s, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_url_encode_len(const char *s, size_t n)
    __attribute__((alias("gj_str_url_encode_len")));
