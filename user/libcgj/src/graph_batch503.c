/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch503: freestanding UTF-8 single-scalar encode
 * (RFC 3629 / Unicode scalar values).
 *
 * Surface (unique symbols):
 *   int gj_utf8_encode1(uint32_t cp, unsigned char *out, size_t cap);
 *     — Encode one Unicode scalar into out[0..cap).
 *       Returns bytes written (1..4) on success; -1 on bad scalar,
 *       NULL out, or insufficient cap.
 *       Rejects UTF-16 surrogates U+D800..U+DFFF and scalars > U+10FFFF.
 *       Does not support a length-probe mode (out must be non-NULL and
 *       cap large enough for the encoding).
 *   int __gj_utf8_encode1  (alias)
 *   __libcgj_batch503_marker = "libcgj-batch503"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Distinct from utf8_encode in
 * graph_batch78.c (unique symbol names / signature).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch503_marker[] = "libcgj-batch503";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_encode1 — encode one Unicode scalar into a byte buffer.
 *
 * Encoding (shortest form only):
 *   U+0000..U+007F     → 1 byte  0xxxxxxx
 *   U+0080..U+07FF     → 2 bytes 110xxxxx 10xxxxxx
 *   U+0800..U+FFFF     → 3 bytes 1110xxxx 10xxxxxx 10xxxxxx
 *                        (excluding surrogates)
 *   U+10000..U+10FFFF  → 4 bytes 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 *
 * Returns:
 *   1..4  — bytes written
 *   -1    — surrogate, >U+10FFFF, out == NULL, or cap too small
 */
int
gj_utf8_encode1(uint32_t cp, unsigned char *out, size_t cap)
{
	if (cp >= 0xD800u && cp <= 0xDFFFu) {
		return -1;
	}
	if (cp > 0x10FFFFu) {
		return -1;
	}
	if (out == NULL) {
		return -1;
	}

	if (cp <= 0x7Fu) {
		if (cap < 1u) {
			return -1;
		}
		out[0] = (unsigned char)cp;
		return 1;
	}
	if (cp <= 0x7FFu) {
		if (cap < 2u) {
			return -1;
		}
		out[0] = (unsigned char)(0xC0u | (cp >> 6));
		out[1] = (unsigned char)(0x80u | (cp & 0x3Fu));
		return 2;
	}
	if (cp <= 0xFFFFu) {
		if (cap < 3u) {
			return -1;
		}
		out[0] = (unsigned char)(0xE0u | (cp >> 12));
		out[1] = (unsigned char)(0x80u | ((cp >> 6) & 0x3Fu));
		out[2] = (unsigned char)(0x80u | (cp & 0x3Fu));
		return 3;
	}
	/* cp <= 0x10FFFF */
	if (cap < 4u) {
		return -1;
	}
	out[0] = (unsigned char)(0xF0u | (cp >> 18));
	out[1] = (unsigned char)(0x80u | ((cp >> 12) & 0x3Fu));
	out[2] = (unsigned char)(0x80u | ((cp >> 6) & 0x3Fu));
	out[3] = (unsigned char)(0x80u | (cp & 0x3Fu));
	return 4;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_encode1(uint32_t cp, unsigned char *out, size_t cap)
    __attribute__((alias("gj_utf8_encode1")));
