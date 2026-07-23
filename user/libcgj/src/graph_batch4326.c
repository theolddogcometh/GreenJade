/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4326: bounded C-string lexicographic compare
 * returning ternary -1/0/1 (wave-unique rename).
 *
 * Surface (unique symbols):
 *   int gj_str_ncmp_u2(const char *a, const char *b, size_t n);
 *     - Compare at most n octets. Returns -1 if a < b, 0 if equal over
 *       the compared window, +1 if a > b (unsigned-char order, clamped
 *       to ternary). n == 0 → 0. both NULL → 0; a NULL → -1; b NULL → 1
 *       when n > 0.
 *       Renamed from gj_str_ncmp_u (collision with batch2203; that
 *       surface returns raw byte differences, not ternary).
 *   int __gj_str_ncmp_u2  (alias)
 *   __libcgj_batch4326_marker = "libcgj-batch4326"
 *
 * Exclusive continuum CREATE-ONLY (4321-4330 string utils). Distinct
 * from gj_str_ncmp_u (batch2203) and gj_str_cmp_n (batch862) — unique
 * gj_str_ncmp_u2 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4326_marker[] = "libcgj-batch4326";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4326_ncmp(const char *szA, const char *szB, size_t cN)
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

		if (u8A < u8B) {
			return -1;
		}
		if (u8A > u8B) {
			return 1;
		}
		if (u8A == 0u) {
			return 0;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_ncmp_u2 - ternary lexicographic compare of at most n octets.
 *
 * Returns exactly -1, 0, or +1 (never a raw byte difference).
 * Does not call libc strncmp. Wave-unique rename: gj_str_ncmp_u is
 * owned by batch2203. Self-contained; no parent wires.
 */
int
gj_str_ncmp_u2(const char *a, const char *b, size_t n)
{
	(void)NULL;
	return b4326_ncmp(a, b, n);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_ncmp_u2(const char *a, const char *b, size_t n)
    __attribute__((alias("gj_str_ncmp_u2")));
