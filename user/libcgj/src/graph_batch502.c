/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch502: freestanding one-codepoint UTF-8 decode.
 *
 * Surface (unique symbols):
 *   int gj_utf8_decode1(const unsigned char *s, size_t n,
 *                       uint32_t *cp, size_t *adv);
 *     — Decode the next Unicode scalar from s[0..n).
 *       Returns 0 on success (writes *cp and *adv when non-NULL),
 *       -1 on failure (invalid / truncated / overlong / surrogate / range /
 *       NULL s with n > 0, or n == 0).
 *       *adv is the number of bytes consumed (1..4).
 *   int __gj_utf8_decode1  (alias)
 *   __libcgj_batch502_marker = "libcgj-batch502"
 *
 * Distinct from batch78 utf8_decode (returns byte length) and batch259
 * gj_utf8_validate (whole-buffer check). Self-contained; no cross-batch
 * call.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch502_marker[] = "libcgj-batch502";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b502_decode1 — strict decode of one scalar from p[0..n).
 *
 * On success: returns byte length consumed (1..4); *pU32Cp receives the
 * Unicode scalar (if pU32Cp non-NULL).
 * On failure: returns -1.
 *
 * Strict rules (RFC 3629 / Unicode scalar values):
 *   - 1-byte:  0xxxxxxx                         U+0000..U+007F
 *   - 2-byte:  110xxxxx 10xxxxxx                U+0080..U+07FF
 *   - 3-byte:  1110xxxx 10xxxxxx 10xxxxxx       U+0800..U+FFFF
 *              excluding U+D800..U+DFFF
 *   - 4-byte:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 *              U+10000..U+10FFFF
 *   - Overlong forms rejected via min_u and lead-byte clamps.
 *   - C0/C1 never legal leads; F5..FF never legal leads.
 */
static int
b502_decode1(const unsigned char *p, size_t n, uint32_t *pU32Cp)
{
	unsigned char u8C0;
	uint32_t u32Cp;
	unsigned int uNeed;
	uint32_t u32Min;
	size_t iByte;

	if (p == NULL || n == 0u) {
		return -1;
	}

	u8C0 = p[0];

	/* ASCII single-byte */
	if (u8C0 < 0x80u) {
		if (pU32Cp != NULL) {
			*pU32Cp = (uint32_t)u8C0;
		}
		return 1;
	}

	if ((u8C0 & 0xE0u) == 0xC0u) {
		/* 2-byte lead: C0/C1 always overlong for < U+0080 */
		if (u8C0 < 0xC2u) {
			return -1;
		}
		uNeed = 1u;
		u32Cp = (uint32_t)(u8C0 & 0x1Fu);
		u32Min = 0x80u;
	} else if ((u8C0 & 0xF0u) == 0xE0u) {
		/* 3-byte lead */
		uNeed = 2u;
		u32Cp = (uint32_t)(u8C0 & 0x0Fu);
		u32Min = 0x800u;
	} else if ((u8C0 & 0xF8u) == 0xF0u) {
		/* 4-byte lead: F5..F7 encode > U+10FFFF */
		if (u8C0 > 0xF4u) {
			return -1;
		}
		uNeed = 3u;
		u32Cp = (uint32_t)(u8C0 & 0x07u);
		u32Min = 0x10000u;
	} else {
		/* Continuation, 5+/6-byte lead, 0xFE, 0xFF */
		return -1;
	}

	if (n < (size_t)(uNeed + 1u)) {
		return -1;
	}

	for (iByte = 1u; iByte <= (size_t)uNeed; iByte++) {
		unsigned char u8Cont = p[iByte];

		if ((u8Cont & 0xC0u) != 0x80u) {
			return -1;
		}
		u32Cp = (u32Cp << 6) | (uint32_t)(u8Cont & 0x3Fu);
	}

	/* Overlong */
	if (u32Cp < u32Min) {
		return -1;
	}

	/* UTF-16 surrogates are not Unicode scalar values */
	if (u32Cp >= 0xD800u && u32Cp <= 0xDFFFu) {
		return -1;
	}

	/* Above Unicode maximum */
	if (u32Cp > 0x10FFFFu) {
		return -1;
	}

	/*
	 * Lead-byte boundary clamps:
	 *   E0: second byte must be A0..BF (else overlong < U+0800)
	 *   ED: second byte must be 80..9F (else surrogate)
	 *   F0: second byte must be 90..BF (else overlong < U+10000)
	 *   F4: second byte must be 80..8F (else > U+10FFFF)
	 */
	if (uNeed == 2u) {
		unsigned char u8C1 = p[1];

		if (u8C0 == 0xE0u && u8C1 < 0xA0u) {
			return -1;
		}
		if (u8C0 == 0xEDu && u8C1 >= 0xA0u) {
			return -1;
		}
	} else if (uNeed == 3u) {
		unsigned char u8C1 = p[1];

		if (u8C0 == 0xF0u && u8C1 < 0x90u) {
			return -1;
		}
		if (u8C0 == 0xF4u && u8C1 >= 0x90u) {
			return -1;
		}
	}

	if (pU32Cp != NULL) {
		*pU32Cp = u32Cp;
	}
	return (int)(uNeed + 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_decode1 — decode next codepoint from s[0..n).
 *
 * Returns:
 *   0  — success; *cp = scalar (if cp non-NULL), *adv = bytes (if adv non-NULL)
 *  -1  — failure (NULL s with n > 0, empty n, or ill-formed sequence)
 *
 * On failure, *cp and *adv are left unchanged.
 */
int
gj_utf8_decode1(const unsigned char *s, size_t n, uint32_t *cp, size_t *adv)
{
	uint32_t u32Cp;
	int nBytes;

	if (n == 0u) {
		return -1;
	}
	if (s == NULL) {
		return -1;
	}

	nBytes = b502_decode1(s, n, &u32Cp);
	if (nBytes < 0) {
		return -1;
	}

	if (cp != NULL) {
		*cp = u32Cp;
	}
	if (adv != NULL) {
		*adv = (size_t)nBytes;
	}
	return 0;
}

int __gj_utf8_decode1(const unsigned char *s, size_t n, uint32_t *cp,
                      size_t *adv)
    __attribute__((alias("gj_utf8_decode1")));
