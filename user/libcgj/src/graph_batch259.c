/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch259: strict UTF-8 buffer validation (RFC 3629).
 *
 * Surface (unique symbols):
 *   int gj_utf8_validate(const unsigned char *s, size_t n);
 *     — Walk s[0..n) as well-formed UTF-8.
 *       Returns 1 if valid, 0 if invalid, -1 if s is NULL and n > 0.
 *       Empty window (n == 0) is valid (returns 1) even when s is NULL.
 *       Rejects overlong encodings, UTF-16 surrogates U+D800..U+DFFF,
 *       and scalars above U+10FFFF. Bare continuations and 5+/6-byte
 *       leads (incl. 0xFE/0xFF) are invalid.
 *   __gj_utf8_validate  (alias)
 *   __libcgj_batch259_marker = "libcgj-batch259"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch259_marker[] = "libcgj-batch259";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b259_decode1 — decode one Unicode scalar from p[0..n).
 *
 * On success: returns byte length consumed (1..4).
 * On failure: returns -1 (truncated, overlong, surrogate, out of range,
 *             illegal lead, or bad continuation).
 *
 * Strict rules (RFC 3629 / Unicode scalar values):
 *   - 1-byte:  0xxxxxxx                         U+0000..U+007F
 *   - 2-byte:  110xxxxx 10xxxxxx                U+0080..U+07FF
 *   - 3-byte:  1110xxxx 10xxxxxx 10xxxxxx       U+0800..U+FFFF
 *              excluding U+D800..U+DFFF
 *   - 4-byte:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 *              U+10000..U+10FFFF
 *   - Overlong forms (e.g. C0 80 for U+0000) are rejected via min_u.
 *   - Lead-byte clamps for E0/ED/F0/F4 catch boundary overlongs and
 *     out-of-range 4-byte sequences early.
 */
static int
b259_decode1(const unsigned char *p, size_t n)
{
	unsigned char c0;
	unsigned int u;
	unsigned int need;
	unsigned int min_u;
	size_t i;

	if (p == NULL || n == 0u) {
		return -1;
	}

	c0 = p[0];

	/* ASCII single-byte */
	if (c0 < 0x80u) {
		return 1;
	}

	if ((c0 & 0xE0u) == 0xC0u) {
		/* 2-byte lead: 110xxxxx — but 1100000x is overlong for < U+0080.
		 * C0 and C1 are never legal leads (always overlong). */
		if (c0 < 0xC2u) {
			return -1;
		}
		need = 1u;
		u = (unsigned int)(c0 & 0x1Fu);
		min_u = 0x80u;
	} else if ((c0 & 0xF0u) == 0xE0u) {
		/* 3-byte lead: 1110xxxx */
		need = 2u;
		u = (unsigned int)(c0 & 0x0Fu);
		min_u = 0x800u;
	} else if ((c0 & 0xF8u) == 0xF0u) {
		/* 4-byte lead: 11110xxx — F5..F7 encode > U+10FFFF */
		if (c0 > 0xF4u) {
			return -1;
		}
		need = 3u;
		u = (unsigned int)(c0 & 0x07u);
		min_u = 0x10000u;
	} else {
		/* Continuation, 5+/6-byte lead, 0xFE, 0xFF */
		return -1;
	}

	if (n < (size_t)(need + 1u)) {
		return -1;
	}

	for (i = 1u; i <= (size_t)need; i++) {
		unsigned char cont = p[i];

		if ((cont & 0xC0u) != 0x80u) {
			return -1;
		}
		u = (u << 6) | (unsigned int)(cont & 0x3Fu);
	}

	/* Overlong */
	if (u < min_u) {
		return -1;
	}

	/* UTF-16 surrogates are not Unicode scalar values */
	if (u >= 0xD800u && u <= 0xDFFFu) {
		return -1;
	}

	/* Above Unicode maximum */
	if (u > 0x10FFFFu) {
		return -1;
	}

	/*
	 * Lead-byte boundary clamps (redundant with min_u / range for some
	 * cases, but explicit and catches ED surrogates / F4 overflow via
	 * second-byte restrictions as well as decoded scalar checks):
	 *   E0: second byte must be A0..BF (else overlong < U+0800)
	 *   ED: second byte must be 80..9F (else surrogate)
	 *   F0: second byte must be 90..BF (else overlong < U+10000)
	 *   F4: second byte must be 80..8F (else > U+10FFFF)
	 */
	if (need == 2u) {
		unsigned char c1 = p[1];

		if (c0 == 0xE0u && c1 < 0xA0u) {
			return -1;
		}
		if (c0 == 0xEDu && c1 >= 0xA0u) {
			return -1;
		}
	} else if (need == 3u) {
		unsigned char c1 = p[1];

		if (c0 == 0xF0u && c1 < 0x90u) {
			return -1;
		}
		if (c0 == 0xF4u && c1 >= 0x90u) {
			return -1;
		}
	}

	return (int)(need + 1u);
}

/*
 * b259_validate — walk entire window; 1 = all valid, 0 = any invalid.
 * Caller must ensure s is non-NULL when n > 0.
 */
static int
b259_validate(const unsigned char *s, size_t n)
{
	size_t i = 0u;

	while (i < n) {
		int nb = b259_decode1(s + i, n - i);

		if (nb < 0) {
			return 0;
		}
		i += (size_t)nb;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_validate — strict UTF-8 validation of s[0..n).
 *
 * Returns:
 *   1  — well-formed UTF-8 (including empty n == 0)
 *   0  — malformed (overlong, surrogate, >U+10FFFF, truncated, bad lead)
 *  -1  — s is NULL and n > 0
 */
int
gj_utf8_validate(const unsigned char *s, size_t n)
{
	if (n == 0u) {
		return 1;
	}
	if (s == NULL) {
		return -1;
	}
	return b259_validate(s, n);
}

int __gj_utf8_validate(const unsigned char *s, size_t n)
    __attribute__((alias("gj_utf8_validate")));
