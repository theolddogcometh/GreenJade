/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1483: freestanding UTF-8 encode one scalar.
 *
 * Surface (unique symbols):
 *   int gj_utf8_encode_one(uint32_t cp, unsigned char *out, size_t cap);
 *     — Encode one Unicode scalar into out[0..cap).
 *       Returns bytes written (1..4) on success; 0 on bad scalar,
 *       NULL out, or insufficient cap.
 *       Rejects UTF-16 surrogates U+D800..U+DFFF and scalars > U+10FFFF.
 *   int __gj_utf8_encode_one  (alias)
 *   __libcgj_batch1483_marker = "libcgj-batch1483"
 *
 * Distinct from gj_utf8_encode1 (batch503). Self-contained.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1483_marker[] = "libcgj-batch1483";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_encode_one — encode CP to out (need cap); return bytes or 0.
 *
 * Encoding (shortest form only):
 *   U+0000..U+007F     → 1 byte
 *   U+0080..U+07FF     → 2 bytes
 *   U+0800..U+FFFF     → 3 bytes (excluding surrogates)
 *   U+10000..U+10FFFF  → 4 bytes
 */
int
gj_utf8_encode_one(uint32_t cp, unsigned char *out, size_t cap)
{
	if (cp >= 0xD800u && cp <= 0xDFFFu) {
		return 0;
	}
	if (cp > 0x10FFFFu) {
		return 0;
	}
	if (out == NULL) {
		return 0;
	}

	if (cp <= 0x7Fu) {
		if (cap < 1u) {
			return 0;
		}
		out[0] = (unsigned char)cp;
		return 1;
	}
	if (cp <= 0x7FFu) {
		if (cap < 2u) {
			return 0;
		}
		out[0] = (unsigned char)(0xC0u | (cp >> 6));
		out[1] = (unsigned char)(0x80u | (cp & 0x3Fu));
		return 2;
	}
	if (cp <= 0xFFFFu) {
		if (cap < 3u) {
			return 0;
		}
		out[0] = (unsigned char)(0xE0u | (cp >> 12));
		out[1] = (unsigned char)(0x80u | ((cp >> 6) & 0x3Fu));
		out[2] = (unsigned char)(0x80u | (cp & 0x3Fu));
		return 3;
	}
	/* cp <= 0x10FFFF */
	if (cap < 4u) {
		return 0;
	}
	out[0] = (unsigned char)(0xF0u | (cp >> 18));
	out[1] = (unsigned char)(0x80u | ((cp >> 12) & 0x3Fu));
	out[2] = (unsigned char)(0x80u | ((cp >> 6) & 0x3Fu));
	out[3] = (unsigned char)(0x80u | (cp & 0x3Fu));
	return 4;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_encode_one(uint32_t cp, unsigned char *out, size_t cap)
    __attribute__((alias("gj_utf8_encode_one")));
