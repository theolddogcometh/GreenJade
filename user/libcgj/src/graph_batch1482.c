/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1482: freestanding UTF-8 decode one scalar.
 *
 * Surface (unique symbols):
 *   int gj_utf8_decode_one(const unsigned char *s, size_t n, uint32_t *cp);
 *     — Decode one Unicode scalar from s[0..n).
 *       Returns bytes consumed (1..4) on success; writes *cp when
 *       cp is non-NULL. Returns 0 on invalid / truncated / overlong /
 *       surrogate / range / NULL s with n > 0 / n == 0.
 *   int __gj_utf8_decode_one  (alias)
 *   __libcgj_batch1482_marker = "libcgj-batch1482"
 *
 * Distinct from gj_utf8_decode1 (batch502). Self-contained; no
 * cross-batch call. Does not redefine lead/cont classifiers.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1482_marker[] = "libcgj-batch1482";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1482_decode — strict decode of one scalar from p[0..n).
 * On success: returns byte length (1..4); *pU32Cp set when non-NULL.
 * On failure: returns 0.
 */
static int
b1482_decode(const unsigned char *p, size_t n, uint32_t *pU32Cp)
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
		if (pU32Cp != NULL) {
			*pU32Cp = (uint32_t)u8C0;
		}
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

	if (pU32Cp != NULL) {
		*pU32Cp = u32Cp;
	}
	return (int)(uNeed + 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_decode_one — decode one CP; return bytes consumed or 0.
 */
int
gj_utf8_decode_one(const unsigned char *s, size_t n, uint32_t *cp)
{
	return b1482_decode(s, n, cp);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_decode_one(const unsigned char *s, size_t n, uint32_t *cp)
    __attribute__((alias("gj_utf8_decode_one")));
