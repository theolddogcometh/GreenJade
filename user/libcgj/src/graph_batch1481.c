/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1481: UTF-8 codepoint byte length at s
 * (strict Unicode scalar; full sequence check).
 *
 * Surface (unique symbols):
 *   int gj_utf8_codepoint_len_n(const unsigned char *s, size_t n);
 *     — Return the number of bytes (1..4) for one well-formed Unicode
 *       scalar starting at s[0..n). Returns 0 if invalid, truncated,
 *       overlong, surrogate, out of range, or s is NULL with n > 0.
 *       Empty n == 0 returns 0.
 *   int __gj_utf8_codepoint_len_n  (alias)
 *   __libcgj_batch1481_marker = "libcgj-batch1481"
 *
 * Unique vs batch1021 gj_utf8_codepoint_len(unsigned char lead) which
 * only classifies a lead byte (returns -1 on bad lead). This TU measures
 * a full sequence at s. Theme name "gj_utf8_codepoint_len" collides;
 * surface is gj_utf8_codepoint_len_n (bytes at s, 0 invalid).
 *
 * Full-sequence length at s[0..n) (scalar-exclusive). Self-contained;
 * does not call lead/cont classifiers from other batches.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1481_marker[] = "libcgj-batch1481";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1481_one_len — strict length of one scalar at p[0..n).
 * Returns 1..4 on success, 0 on failure.
 */
static int
b1481_one_len(const unsigned char *p, size_t n)
{
	unsigned char u8C0;
	uint32_t u32Cp;
	unsigned int uNeed;
	uint32_t u32Min;
	size_t iByte;

	if (p == NULL || n == 0u) {
		return 0;
	}

	u8C0 = p[0];

	if (u8C0 < 0x80u) {
		return 1;
	}

	if ((u8C0 & 0xE0u) == 0xC0u) {
		if (u8C0 < 0xC2u) {
			return 0;
		}
		uNeed = 1u;
		u32Cp = (uint32_t)(u8C0 & 0x1Fu);
		u32Min = 0x80u;
	} else if ((u8C0 & 0xF0u) == 0xE0u) {
		uNeed = 2u;
		u32Cp = (uint32_t)(u8C0 & 0x0Fu);
		u32Min = 0x800u;
	} else if ((u8C0 & 0xF8u) == 0xF0u) {
		if (u8C0 > 0xF4u) {
			return 0;
		}
		uNeed = 3u;
		u32Cp = (uint32_t)(u8C0 & 0x07u);
		u32Min = 0x10000u;
	} else {
		return 0;
	}

	if (n < (size_t)(uNeed + 1u)) {
		return 0;
	}

	for (iByte = 1u; iByte <= (size_t)uNeed; iByte++) {
		unsigned char u8Cont = p[iByte];

		if ((u8Cont & 0xC0u) != 0x80u) {
			return 0;
		}
		u32Cp = (u32Cp << 6) | (uint32_t)(u8Cont & 0x3Fu);
	}

	if (u32Cp < u32Min) {
		return 0;
	}
	if (u32Cp >= 0xD800u && u32Cp <= 0xDFFFu) {
		return 0;
	}
	if (u32Cp > 0x10FFFFu) {
		return 0;
	}

	if (uNeed == 2u) {
		unsigned char u8C1 = p[1];

		if (u8C0 == 0xE0u && u8C1 < 0xA0u) {
			return 0;
		}
		if (u8C0 == 0xEDu && u8C1 >= 0xA0u) {
			return 0;
		}
	} else if (uNeed == 3u) {
		unsigned char u8C1 = p[1];

		if (u8C0 == 0xF0u && u8C1 < 0x90u) {
			return 0;
		}
		if (u8C0 == 0xF4u && u8C1 >= 0x90u) {
			return 0;
		}
	}

	return (int)(uNeed + 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_codepoint_len_n — bytes for one codepoint at s, 0 invalid.
 *
 * s: start of sequence; n: available bytes.
 * Returns 1..4 on well-formed scalar, else 0.
 */
int
gj_utf8_codepoint_len_n(const unsigned char *s, size_t n)
{
	return b1481_one_len(s, n);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_codepoint_len_n(const unsigned char *s, size_t n)
    __attribute__((alias("gj_utf8_codepoint_len_n")));
