/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1565: Dice coefficient over character bigrams.
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_dice_bigram_hint(const char *a, const char *b);
 *     — Milli-Dice [0, 1000] of ordered character bigram multisets:
 *         2 * |A ∩ B| / (|A| + |B|) × 1000  (floor).
 *       Strings capped at B1565_MAX_LEN (64). Oversize or NULL → 0
 *       (except both empty / both length < 2 → 1000 when both empty of
 *       bigrams). Distinct from char-set Jaccard (batch1564).
 *   uint32_t __gj_str_dice_bigram_hint  (alias)
 *   __libcgj_batch1565_marker = "libcgj-batch1565"
 *
 * Stack only: extract up to 63 bigrams, insertion-sort, multiset merge.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

#define B1565_MAX_LEN 64
#define B1565_MAX_BG  (B1565_MAX_LEN - 1)

const char __libcgj_batch1565_marker[] = "libcgj-batch1565";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1565_strnlen_cap(const char *sz, int nMax)
{
	int n;

	if (sz == NULL) {
		return -1;
	}
	for (n = 0; n <= nMax; n++) {
		if (sz[n] == '\0') {
			return n;
		}
	}
	return -1;
}

/* Extract ordered bigrams as big-endian uint16 (c0<<8 | c1). */
static int
b1565_bigrams(const char *sz, int nLen, uint16_t *aOut)
{
	int i;
	int nBg;

	if (nLen < 2) {
		return 0;
	}
	nBg = nLen - 1;
	for (i = 0; i < nBg; i++) {
		aOut[i] = (uint16_t)(((unsigned)(unsigned char)sz[i] << 8) |
		    (unsigned)(unsigned char)sz[i + 1]);
	}
	return nBg;
}

/* Insertion sort ascending. */
static void
b1565_sort_u16(uint16_t *a, int n)
{
	int i;
	int j;

	for (i = 1; i < n; i++) {
		uint16_t v = a[i];

		j = i;
		while (j > 0 && a[j - 1] > v) {
			a[j] = a[j - 1];
			j--;
		}
		a[j] = v;
	}
}

/*
 * Multiset intersection size of two sorted uint16 arrays.
 * Counts min(freq_a, freq_b) per distinct value via two-pointer merge.
 */
static int
b1565_multiset_inter(const uint16_t *aA, int nA, const uint16_t *aB, int nB)
{
	int i = 0;
	int j = 0;
	int nInter = 0;

	while (i < nA && j < nB) {
		if (aA[i] == aB[j]) {
			nInter++;
			i++;
			j++;
		} else if (aA[i] < aB[j]) {
			i++;
		} else {
			j++;
		}
	}
	return nInter;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_dice_bigram_hint — milli-Dice of bigram multisets.
 *
 * 2*|∩| / (|A|+|B|) × 1000. Both empty of bigrams → 1000. Oversize → 0.
 */
uint32_t
gj_str_dice_bigram_hint(const char *a, const char *b)
{
	int nLenA;
	int nLenB;
	int nBgA;
	int nBgB;
	int nInter;
	int nSum;
	uint16_t aBgA[B1565_MAX_BG];
	uint16_t aBgB[B1565_MAX_BG];

	nLenA = b1565_strnlen_cap(a, B1565_MAX_LEN);
	nLenB = b1565_strnlen_cap(b, B1565_MAX_LEN);
	if (nLenA < 0 || nLenB < 0) {
		return 0u;
	}

	nBgA = b1565_bigrams(a, nLenA, aBgA);
	nBgB = b1565_bigrams(b, nLenB, aBgB);

	nSum = nBgA + nBgB;
	if (nSum == 0) {
		/* Both length < 2: empty bigram sets are equal. */
		return 1000u;
	}

	b1565_sort_u16(aBgA, nBgA);
	b1565_sort_u16(aBgB, nBgB);
	nInter = b1565_multiset_inter(aBgA, nBgA, aBgB, nBgB);

	/* 2 * nInter * 1000 / nSum; nInter <= 63 → product fits uint32. */
	return (uint32_t)(((unsigned)nInter * 2000u) / (unsigned)nSum);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_dice_bigram_hint(const char *a, const char *b)
    __attribute__((alias("gj_str_dice_bigram_hint")));
