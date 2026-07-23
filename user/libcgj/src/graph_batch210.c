/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch210: uint32_t radix sort helper.
 *
 * Surface (unique symbols):
 *   void gj_radix_sort_u32(uint32_t *a, size_t n);
 *     — ascending sort of n uint32_t keys in place.
 *   __gj_radix_sort_u32  (alias)
 *   __libcgj_batch210_marker = "libcgj-batch210"
 *
 * Algorithm (freestanding, no malloc):
 *   - n < 2 / NULL: no-op
 *   - tiny n: insertion sort (in-place)
 *   - small value range (max-min <= 255): counting sort with stack
 *     histogram uint32_t[256] only (256*4 bytes)
 *   - n <= 256: stable LSD radix, 8-bit digits × 4 passes; stack aux is
 *     hist[256] + tmp[256] (tmp capped at 256*4)
 *   - n > 256: in-place MSD American-flag radix (8-bit digits), stack
 *     hist/next of 256 bins each; tiny partitions fall back to insertion
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No third-party source copied. Does not redefine
 * radixsort / sradixsort (batch36) or gj_qsort* (batch100).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch210_marker[] = "libcgj-batch210";

/* Insertion cutoff (elements). */
#define B210_INS 32u
/* Max keys for stack LSD temp (256 * sizeof(uint32_t)). */
#define B210_LSD_MAX 256u

/* ---- tiny helpers ------------------------------------------------------ */

static void
b210_insertion(uint32_t *pA, size_t cN)
{
	size_t i;

	for (i = 1u; i < cN; i++) {
		uint32_t uKey = pA[i];
		size_t j = i;

		while (j > 0u && pA[j - 1u] > uKey) {
			pA[j] = pA[j - 1u];
			j--;
		}
		pA[j] = uKey;
	}
}

static void
b210_minmax(const uint32_t *pA, size_t cN, uint32_t *pMin, uint32_t *pMax)
{
	uint32_t uMin = pA[0];
	uint32_t uMax = pA[0];
	size_t i;

	for (i = 1u; i < cN; i++) {
		uint32_t u = pA[i];

		if (u < uMin) {
			uMin = u;
		}
		if (u > uMax) {
			uMax = u;
		}
	}
	*pMin = uMin;
	*pMax = uMax;
}

/*
 * Counting sort when all values lie in [uMin, uMin+255].
 * Stack aux: hist[256] only (256*4). Rewrites pA by walking bins.
 */
static void
b210_counting(uint32_t *pA, size_t cN, uint32_t uMin)
{
	uint32_t aHist[256];
	size_t i;
	size_t iOut;
	unsigned d;

	for (i = 0u; i < 256u; i++) {
		aHist[i] = 0u;
	}
	for (i = 0u; i < cN; i++) {
		aHist[pA[i] - uMin]++;
	}
	iOut = 0u;
	for (d = 0u; d < 256u; d++) {
		uint32_t c = aHist[d];
		uint32_t uVal = uMin + (uint32_t)d;

		while (c > 0u) {
			pA[iOut++] = uVal;
			c--;
		}
	}
}

/*
 * One stable LSD digit pass: scatter pSrc → pDst by byte at uShift.
 * aHist is scratch (cleared/reused internally).
 */
static void
b210_lsd_pass(const uint32_t *pSrc, uint32_t *pDst, size_t cN,
              unsigned uShift, uint32_t *aHist)
{
	size_t i;
	uint32_t uSum;
	uint32_t uTmp;

	for (i = 0u; i < 256u; i++) {
		aHist[i] = 0u;
	}
	for (i = 0u; i < cN; i++) {
		aHist[(pSrc[i] >> uShift) & 0xffu]++;
	}
	/* Exclusive prefix → write heads. */
	uSum = 0u;
	for (i = 0u; i < 256u; i++) {
		uTmp = aHist[i];
		aHist[i] = uSum;
		uSum += uTmp;
	}
	for (i = 0u; i < cN; i++) {
		unsigned d = (pSrc[i] >> uShift) & 0xffu;
		pDst[aHist[d]++] = pSrc[i];
	}
}

/*
 * Stable LSD radix for cN <= B210_LSD_MAX. Stack: hist[256] + tmp[256].
 * Four 8-bit passes; result lands in pA.
 */
static void
b210_lsd(uint32_t *pA, size_t cN)
{
	uint32_t aHist[256];
	uint32_t aTmp[B210_LSD_MAX];

	/* Passes 0,8,16,24: a → tmp → a → tmp → a */
	b210_lsd_pass(pA, aTmp, cN, 0u, aHist);
	b210_lsd_pass(aTmp, pA, cN, 8u, aHist);
	b210_lsd_pass(pA, aTmp, cN, 16u, aHist);
	b210_lsd_pass(aTmp, pA, cN, 24u, aHist);
}

/*
 * In-place MSD American-flag radix (8-bit digits). Stack: count/next
 * of 256 bins. Used when n > B210_LSD_MAX so LSD temp would exceed
 * the 256*4 stack/aux cap. uShift is the bit offset of the active digit
 * (24, 16, 8, then 0).
 */
static void
b210_msd(uint32_t *pA, size_t cN, unsigned uShift)
{
	uint32_t aEnd[256];
	uint32_t aNext[256];
	size_t i;
	uint32_t uSum;
	uint32_t uStart;

	if (cN < 2u) {
		return;
	}
	if (cN <= (size_t)B210_INS) {
		b210_insertion(pA, cN);
		return;
	}

	for (i = 0u; i < 256u; i++) {
		aEnd[i] = 0u;
	}
	for (i = 0u; i < cN; i++) {
		aEnd[(pA[i] >> uShift) & 0xffu]++;
	}

	/* aNext[d] = start of bucket d; aEnd[d] = exclusive end. */
	uSum = 0u;
	for (i = 0u; i < 256u; i++) {
		uint32_t uC = aEnd[i];

		aNext[i] = uSum;
		uSum += uC;
		aEnd[i] = uSum;
	}

	/* Permute so each key sits in its digit bucket. */
	for (i = 0u; i < 256u; i++) {
		while (aNext[i] < aEnd[i]) {
			size_t iCur = (size_t)aNext[i];
			unsigned d = (pA[iCur] >> uShift) & 0xffu;

			if (d == (unsigned)i) {
				aNext[i]++;
			} else {
				size_t iDst = (size_t)aNext[d];
				uint32_t u = pA[iCur];

				pA[iCur] = pA[iDst];
				pA[iDst] = u;
				aNext[d]++;
			}
		}
	}

	/* Recurse on each non-trivial bucket with the next lower digit. */
	if (uShift == 0u) {
		return;
	}
	uStart = 0u;
	for (i = 0u; i < 256u; i++) {
		uint32_t uBucketEnd = aEnd[i];
		uint32_t uLen = uBucketEnd - uStart;

		if (uLen > 1u) {
			b210_msd(pA + (size_t)uStart, (size_t)uLen,
			         uShift - 8u);
		}
		uStart = uBucketEnd;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_radix_sort_u32 — ascending in-place sort of uint32_t keys.
 *
 * a: array of n keys (NULL → no-op). n: element count.
 * See file header for path selection (insertion / counting / LSD / MSD).
 */
void
gj_radix_sort_u32(uint32_t *a, size_t n)
{
	uint32_t uMin;
	uint32_t uMax;

	if (a == NULL || n < 2u) {
		return;
	}

	if (n <= (size_t)B210_INS) {
		b210_insertion(a, n);
		return;
	}

	b210_minmax(a, n, &uMin, &uMax);
	/* Small domain → pure counting (stack hist 256*4 only). */
	if ((uMax - uMin) <= 255u) {
		b210_counting(a, n, uMin);
		return;
	}

	if (n <= (size_t)B210_LSD_MAX) {
		b210_lsd(a, n);
		return;
	}

	/* Large n: in-place MSD radix from the high byte. */
	b210_msd(a, n, 24u);
}

void __gj_radix_sort_u32(uint32_t *a, size_t n)
    __attribute__((alias("gj_radix_sort_u32")));
