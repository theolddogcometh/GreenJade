/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch78: clean-room UTF-8 helpers (RFC 3629).
 * Freestanding pure C — integer/pointer only (no SSE, no libc).
 *
 * Existing surface NOT redefined (avoid multi-def):
 *   mbrtowc / wcrtomb / mbsrtowcs / __mbrtowc / __mbrtowc_chk
 *     → wchar.c / graph_batch12.c / graph_batch23.c
 *   iconv_utf8_l1 (static) → iconv.c
 *
 * This TU adds only symbols that were missing:
 *   utf8_validate(const char *s, size_t n) → 0 ok / -1 bad
 *   utf8_strlen(const char *s) → code-point count (NUL-terminated)
 *   utf8_decode — one code point from a byte window
 *   utf8_encode — one code point into a byte buffer
 *   __libcgj_batch78_marker = "libcgj-batch78"
 *
 * Strict UTF-8: reject overlongs, surrogates U+D800..U+DFFF, and
 * scalars above U+10FFFF. Does not call mbrtowc (standalone freestanding).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch78_marker[] = "libcgj-batch78";

/* --------------------------------------------------------------------------
 * Internal: decode one code point from s[0..n).
 * On success: *pCp = scalar, return byte length (1..4).
 * On failure: return -1 (invalid / truncated / overlong / surrogate / range).
 * -------------------------------------------------------------------------- */

static int
b78_decode1(const unsigned char *p, size_t n, unsigned int *pCp)
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
	if (c0 < 0x80u) {
		if (pCp != NULL) {
			*pCp = (unsigned int)c0;
		}
		return 1;
	}

	if ((c0 & 0xE0u) == 0xC0u) {
		need = 1u;
		u = (unsigned int)(c0 & 0x1Fu);
		min_u = 0x80u;
	} else if ((c0 & 0xF0u) == 0xE0u) {
		need = 2u;
		u = (unsigned int)(c0 & 0x0Fu);
		min_u = 0x800u;
	} else if ((c0 & 0xF8u) == 0xF0u) {
		need = 3u;
		u = (unsigned int)(c0 & 0x07u);
		min_u = 0x10000u;
	} else {
		/* 5+/6-byte lead, bare continuation, or 0xFE/0xFF */
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

	/* Overlong, surrogate, or out of Unicode range */
	if (u < min_u) {
		return -1;
	}
	if (u >= 0xD800u && u <= 0xDFFFu) {
		return -1;
	}
	if (u > 0x10FFFFu) {
		return -1;
	}

	/* Lead-byte range clamps that catch overlongs for E0/F0 forms */
	if (need == 2u && c0 == 0xE0u && u < 0x800u) {
		return -1;
	}
	if (need == 3u && c0 == 0xF0u && u < 0x10000u) {
		return -1;
	}
	if (need == 3u && c0 > 0xF4u) {
		return -1;
	}

	if (pCp != NULL) {
		*pCp = u;
	}
	return (int)(need + 1u);
}

/* --------------------------------------------------------------------------
 * Public surface
 * -------------------------------------------------------------------------- */

/*
 * utf8_validate — walk s[0..n) as strict UTF-8.
 * Returns 0 if the whole window is well-formed; -1 otherwise.
 * Empty (n == 0) is valid. NULL with n > 0 is invalid.
 */
int
utf8_validate(const char *s, size_t n)
{
	const unsigned char *p;
	size_t i;

	if (n == 0u) {
		return 0;
	}
	if (s == NULL) {
		return -1;
	}

	p = (const unsigned char *)s;
	i = 0u;
	while (i < n) {
		int nb = b78_decode1(p + i, n - i, NULL);

		if (nb < 0) {
			return -1;
		}
		i += (size_t)nb;
	}
	return 0;
}

/*
 * utf8_strlen — count Unicode scalar values in a NUL-terminated C string.
 * On invalid UTF-8 returns (size_t)-1. NULL returns 0.
 * Does not count the terminating NUL as a code point.
 */
size_t
utf8_strlen(const char *s)
{
	const unsigned char *p;
	size_t nCp = 0u;

	if (s == NULL) {
		return 0u;
	}

	p = (const unsigned char *)s;
	while (*p != 0u) {
		size_t rem = 0u;
		int nb;

		/* At most 4 non-NUL bytes for one scalar; truncated sequences fail. */
		while (p[rem] != 0u && rem < 4u) {
			rem++;
		}
		nb = b78_decode1(p, rem, NULL);
		if (nb < 0) {
			return (size_t)-1;
		}
		p += (size_t)nb;
		nCp++;
	}
	return nCp;
}

/*
 * utf8_decode — decode one code point from s[0..n).
 * On success: *pCp receives the scalar (if pCp non-NULL) and the return
 * value is the number of bytes consumed (1..4). On failure: -1.
 */
int
utf8_decode(const char *s, size_t n, unsigned int *pCp)
{
	if (s == NULL) {
		return -1;
	}
	return b78_decode1((const unsigned char *)s, n, pCp);
}

/*
 * utf8_encode — encode one Unicode scalar into out[0..nOut).
 * Returns byte length written (1..4) on success; -1 on bad scalar or
 * insufficient buffer. out may be NULL only when nOut == 0 (length probe
 * is not supported — nOut must be large enough).
 */
int
utf8_encode(char *out, size_t nOut, unsigned int uCp)
{
	unsigned char *p;

	if (uCp >= 0xD800u && uCp <= 0xDFFFu) {
		return -1;
	}
	if (uCp > 0x10FFFFu) {
		return -1;
	}
	if (out == NULL) {
		return -1;
	}
	p = (unsigned char *)out;

	if (uCp <= 0x7Fu) {
		if (nOut < 1u) {
			return -1;
		}
		p[0] = (unsigned char)uCp;
		return 1;
	}
	if (uCp <= 0x7FFu) {
		if (nOut < 2u) {
			return -1;
		}
		p[0] = (unsigned char)(0xC0u | (uCp >> 6));
		p[1] = (unsigned char)(0x80u | (uCp & 0x3Fu));
		return 2;
	}
	if (uCp <= 0xFFFFu) {
		if (nOut < 3u) {
			return -1;
		}
		p[0] = (unsigned char)(0xE0u | (uCp >> 12));
		p[1] = (unsigned char)(0x80u | ((uCp >> 6) & 0x3Fu));
		p[2] = (unsigned char)(0x80u | (uCp & 0x3Fu));
		return 3;
	}
	/* uCp <= 0x10FFFF */
	if (nOut < 4u) {
		return -1;
	}
	p[0] = (unsigned char)(0xF0u | (uCp >> 18));
	p[1] = (unsigned char)(0x80u | ((uCp >> 12) & 0x3Fu));
	p[2] = (unsigned char)(0x80u | ((uCp >> 6) & 0x3Fu));
	p[3] = (unsigned char)(0x80u | (uCp & 0x3Fu));
	return 4;
}
