/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2412: case-insensitive n-octet lexicographic
 * compare.
 *
 * Surface (unique symbols):
 *   int gj_str_cmp_ci_n(const char *a, const char *b, size_t n);
 *     - strncasecmp-shaped over at most n octets of C strings under
 *       ASCII case fold (A-Z -> a-z). Returns 0 if equal over the
 *       window; negative if a < b; positive if a > b (folded unsigned
 *       char difference). n == 0 -> 0. Both NULL -> 0; a NULL -> -1;
 *       b NULL -> 1 when n > 0.
 *   int __gj_str_cmp_ci_n  (alias)
 *   __libcgj_batch2412_marker = "libcgj-batch2412"
 *
 * Distinct from gj_str_cmp_n (case-sensitive, batch862), gj_str_icmp
 * (NUL-terminated, batch762), and libc strncasecmp — unique surface;
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2412_marker[] = "libcgj-batch2412";

/* ---- freestanding helpers ---------------------------------------------- */

/* ASCII A-Z -> a-z; all other octets unchanged. */
static unsigned char
b2412_fold(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return (unsigned char)(u8Ch + (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_cmp_ci_n - lexicographic CI compare of at most n C-string octets.
 *
 * Compares as folded unsigned char until mismatch, shared NUL, or n
 * octets. Does not call libc strncasecmp.
 */
int
gj_str_cmp_ci_n(const char *szA, const char *szB, size_t cN)
{
	const unsigned char *pA;
	const unsigned char *pB;
	size_t i;
	unsigned char u8A;
	unsigned char u8B;

	if (cN == 0u) {
		return 0;
	}
	if (szA == NULL || szB == NULL) {
		if (szA == szB) {
			return 0;
		}
		return (szA == NULL) ? -1 : 1;
	}

	pA = (const unsigned char *)szA;
	pB = (const unsigned char *)szB;
	for (i = 0u; i < cN; i++) {
		u8A = b2412_fold(pA[i]);
		u8B = b2412_fold(pB[i]);
		if (u8A != u8B) {
			return (int)u8A - (int)u8B;
		}
		if (u8A == 0u) {
			return 0;
		}
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_cmp_ci_n(const char *szA, const char *szB, size_t cN)
    __attribute__((alias("gj_str_cmp_ci_n")));
