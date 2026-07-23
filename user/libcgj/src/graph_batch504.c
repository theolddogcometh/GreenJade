/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch504: freestanding UTF-8 buffer scrub.
 *
 * Surface (unique symbols):
 *   size_t gj_utf8_scrub(const unsigned char *in, size_t n,
 *                        unsigned char *out, size_t out_cap);
 *     — Walk in[0..n) as UTF-8. Copy well-formed sequences (RFC 3629)
 *       into out. Replace each invalid lead / lone continuation / truncated
 *       / overlong / surrogate / out-of-range byte with ASCII '?' (0x3F),
 *       advancing one input byte per replacement. Stops without writing a
 *       partial multi-byte sequence when out_cap is exhausted.
 *       Returns the number of bytes written to out (0..out_cap).
 *       Empty input (n == 0) returns 0. NULL in with n > 0 returns 0.
 *       NULL out with any positive write needed returns 0.
 *   size_t __gj_utf8_scrub  (alias)
 *   __libcgj_batch504_marker = "libcgj-batch504"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied. Self-contained decode
 * (no cross-batch call to gj_utf8_validate).
 */

#include <stddef.h>

const char __libcgj_batch504_marker[] = "libcgj-batch504";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b504_decode1 — decode one Unicode scalar from p[0..n).
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
 *   - Overlong forms are rejected via min_u and lead clamps.
 *   - Bare continuations and 5+/6-byte leads (incl. 0xFE/0xFF) fail.
 */
static int
b504_decode1(const unsigned char *p, size_t n)
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
		/* 2-byte lead: C0/C1 are never legal (always overlong). */
		if (c0 < 0xC2u) {
			return -1;
		}
		need = 1u;
		u = (unsigned int)(c0 & 0x1Fu);
		min_u = 0x80u;
	} else if ((c0 & 0xF0u) == 0xE0u) {
		/* 3-byte lead */
		need = 2u;
		u = (unsigned int)(c0 & 0x0Fu);
		min_u = 0x800u;
	} else if ((c0 & 0xF8u) == 0xF0u) {
		/* 4-byte lead: F5..F7 encode > U+10FFFF */
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

	if (u < min_u) {
		return -1;
	}

	/* UTF-16 surrogates are not Unicode scalar values */
	if (u >= 0xD800u && u <= 0xDFFFu) {
		return -1;
	}

	if (u > 0x10FFFFu) {
		return -1;
	}

	/*
	 * Lead-byte boundary clamps:
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_scrub — sanitize in[0..n) into out[0..out_cap).
 *
 * Valid UTF-8 sequences are copied byte-for-byte. Each invalid input byte
 * is replaced with a single ASCII '?' and one input byte is consumed.
 * Writing stops when the next emission (full valid sequence or one '?')
 * would exceed out_cap; no partial multi-byte sequence is written.
 *
 * Returns:
 *   number of bytes written to out (0 if nothing written / bad args)
 */
size_t
gj_utf8_scrub(const unsigned char *in, size_t n,
              unsigned char *out, size_t out_cap)
{
	size_t iIn;
	size_t iOut;

	if (n == 0u) {
		return 0u;
	}
	if (in == NULL) {
		return 0u;
	}
	if (out == NULL || out_cap == 0u) {
		return 0u;
	}

	iIn = 0u;
	iOut = 0u;

	while (iIn < n) {
		int nb = b504_decode1(in + iIn, n - iIn);

		if (nb > 0) {
			size_t cb = (size_t)nb;
			size_t k;

			if (iOut + cb > out_cap) {
				break;
			}
			for (k = 0u; k < cb; k++) {
				out[iOut + k] = in[iIn + k];
			}
			iOut += cb;
			iIn += cb;
		} else {
			/* Invalid: emit '?' and skip one input byte. */
			if (iOut >= out_cap) {
				break;
			}
			out[iOut] = (unsigned char)'?';
			iOut += 1u;
			iIn += 1u;
		}
	}

	return iOut;
}

size_t __gj_utf8_scrub(const unsigned char *in, size_t n,
                       unsigned char *out, size_t out_cap)
    __attribute__((alias("gj_utf8_scrub")));
