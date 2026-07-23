/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1484: count Unicode scalars in a NUL-string.
 *
 * Surface (unique symbols):
 *   size_t gj_utf8_count_cps(const unsigned char *s);
 *     — Count well-formed Unicode scalar values in the NUL-terminated
 *       UTF-8 string s. Stops at the first invalid sequence (does not
 *       count further). Returns 0 if s is NULL or s is empty / invalid
 *       at the first byte. NUL terminator is not counted.
 *   size_t __gj_utf8_count_cps  (alias)
 *   __libcgj_batch1484_marker = "libcgj-batch1484"
 *
 * Distinct from gj_utf8_count_cp (batch692, lead-byte tally over n).
 * Self-contained; no libc strlen.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1484_marker[] = "libcgj-batch1484";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1484_one — strict decode length of one scalar from p with at least
 * nAvail remaining bytes before NUL boundary probe.
 * Returns 1..4 or 0.
 */
static int
b1484_one(const unsigned char *p, size_t nAvail)
{
	unsigned char u8C0;
	uint32_t u32Cp;
	unsigned int uNeed;
	uint32_t u32Min;
	size_t iByte;

	if (p == NULL || nAvail == 0u) {
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

	if (nAvail < (size_t)(uNeed + 1u)) {
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

/*
 * b1484_avail — bytes remaining until NUL from s (not including NUL).
 * Caps at a large walk; freestanding (no libc).
 */
static size_t
b1484_avail(const unsigned char *s)
{
	size_t n;

	n = 0u;
	while (s[n] != 0u) {
		n++;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_count_cps — count codepoints in a NUL-terminated string.
 *
 * Walks well-formed scalars only. On the first ill-formed sequence,
 * stops and returns the count accumulated so far.
 */
size_t
gj_utf8_count_cps(const unsigned char *s)
{
	size_t nLeft;
	size_t nCp;
	size_t iOff;

	if (s == NULL) {
		return 0u;
	}

	nLeft = b1484_avail(s);
	nCp = 0u;
	iOff = 0u;

	while (iOff < nLeft) {
		int nBytes = b1484_one(s + iOff, nLeft - iOff);

		if (nBytes <= 0) {
			break;
		}
		iOff += (size_t)nBytes;
		nCp++;
	}
	return nCp;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_utf8_count_cps(const unsigned char *s)
    __attribute__((alias("gj_utf8_count_cps")));
