/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch862: bounded C-string lexicographic compare.
 *
 * Surface (unique symbols):
 *   int gj_str_cmp_n(const char *a, const char *b, size_t n);
 *     — strncmp-shaped: 0 if equal over min(n, shared prefix+NUL);
 *       negative if a < b; positive if a > b (unsigned-char difference).
 *       n==0 → 0. both NULL → 0; a NULL → -1; b NULL → 1 when n>0.
 *   int __gj_str_cmp_n  (alias)
 *   __libcgj_batch862_marker = "libcgj-batch862"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch862_marker[] = "libcgj-batch862";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_cmp_n — lexicographic compare of at most n octets of C strings.
 *
 * Compares as unsigned char until mismatch, shared NUL, or n octets.
 * Does not call libc strncmp.
 */
int
gj_str_cmp_n(const char *szA, const char *szB, size_t cN)
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

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_cmp_n(const char *szA, const char *szB, size_t cN)
    __attribute__((alias("gj_str_cmp_n")));
