/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1485: strict UTF-8 validation of n bytes.
 *
 * Surface (unique symbols):
 *   int gj_utf8_is_valid_n(const unsigned char *s, size_t n);
 *     — Returns 1 if s[0..n) is well-formed UTF-8 of Unicode scalars
 *       only; 0 if invalid. Empty window (n == 0) is valid (returns 1)
 *       even when s is NULL. NULL s with n > 0 returns 0.
 *       Rejects overlongs, surrogates U+D800..U+DFFF, >U+10FFFF,
 *       bare continuations, and truncated sequences.
 *   int __gj_utf8_is_valid_n  (alias)
 *   __libcgj_batch1485_marker = "libcgj-batch1485"
 *
 * Distinct from gj_utf8_validate (batch259). Self-contained.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1485_marker[] = "libcgj-batch1485";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1485_one — length of one well-formed scalar at p[0..n), or 0.
 */
static int
b1485_one(const unsigned char *p, size_t n)
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
 * gj_utf8_is_valid_n — validate n bytes of UTF-8 scalars.
 *
 * Returns 1 if valid, 0 if invalid.
 */
int
gj_utf8_is_valid_n(const unsigned char *s, size_t n)
{
	size_t iOff;

	if (n == 0u) {
		return 1;
	}
	if (s == NULL) {
		return 0;
	}

	iOff = 0u;
	while (iOff < n) {
		int nBytes = b1485_one(s + iOff, n - iOff);

		if (nBytes <= 0) {
			return 0;
		}
		iOff += (size_t)nBytes;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_is_valid_n(const unsigned char *s, size_t n)
    __attribute__((alias("gj_utf8_is_valid_n")));
