/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2203: bounded C-string lexicographic compare.
 *
 * Surface (unique symbols):
 *   int gj_str_ncmp_u(const char *a, const char *b, size_t n);
 *     - strncmp-shaped: 0 if equal over min(n, shared prefix through NUL);
 *       negative if a < b; positive if a > b (unsigned-char difference).
 *       n == 0 → 0. both NULL → 0; a NULL → -1; b NULL → 1 when n > 0.
 *   int __gj_str_ncmp_u  (alias)
 *   __libcgj_batch2203_marker = "libcgj-batch2203"
 *
 * Post-2200 str exclusive wave (2201-2210). Distinct from gj_str_cmp_n
 * (batch862) and gj_str_eq_n (batch414 boolean) - unique surface; no
 * multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2203_marker[] = "libcgj-batch2203";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2203_ncmp(const char *szA, const char *szB, size_t cN)
{
	const unsigned char *pA;
	const unsigned char *pB;
	size_t i;

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
		unsigned char u8A = pA[i];
		unsigned char u8B = pB[i];

		if (u8A != u8B) {
			return (int)u8A - (int)u8B;
		}
		if (u8A == 0u) {
			return 0;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_ncmp_u - lexicographic compare of at most n octets of C strings.
 *
 * Compares as unsigned char until mismatch, shared NUL, or n octets.
 * Does not call libc strncmp.
 */
int
gj_str_ncmp_u(const char *a, const char *b, size_t n)
{
	(void)NULL;
	return b2203_ncmp(a, b, n);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_ncmp_u(const char *a, const char *b, size_t n)
    __attribute__((alias("gj_str_ncmp_u")));
