/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch100: freestanding O(n log n) sort suite.
 * Clean-room pure C; builds under userspace -msse2. No third-party sort
 * source was copied.
 *
 * Existing surface NOT redefined (avoid multi-def):
 *   qsort        → stdlib.c (insertion sort bring-up)
 *   qsort_r      → graph_stdlib.c
 *   heapsort     → graph_batch18.c (malloc-based)
 *   mergesort    → graph_batch18.c (malloc-based)
 *   radixsort    → graph_batch36.c
 *
 * This TU adds only unique symbols (no multi-def with qsort*):
 *   void gj_qsort(void *base, size_t nmemb, size_t size,
 *                 int (*compar)(const void *, const void *));
 *   void gj_qsort_r(void *base, size_t nmemb, size_t size,
 *                   int (*compar)(const void *, const void *, void *),
 *                   void *arg);
 *   int  gj_heapsort(void *base, size_t nmemb, size_t size,
 *                    int (*compar)(const void *, const void *));
 *   int  gj_mergesort(void *base, size_t nmemb, size_t size,
 *                     int (*compar)(const void *, const void *));
 *   __gj_qsort / __gj_qsort_r / __gj_heapsort / __gj_mergesort  (aliases)
 *   __libcgj_batch100_marker = "libcgj-batch100"
 *
 * Notes:
 *   - gj_qsort / gj_qsort_r: introsort-style (median-of-3 quicksort +
 *     insertion for small partitions; heapsort if depth limit hit).
 *   - gj_heapsort: fully in-place, no malloc (unlike batch18 heapsort).
 *   - gj_mergesort: stable bottom-up merge when nmemb*size fits in a
 *     fixed stack temp (8 KiB); else returns -1 with errno = ENOMEM.
 *   - SSE2 accelerates element moves for size >= 16 (unaligned 16B).
 */

#include <errno.h>
#include <emmintrin.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch100_marker[] = "libcgj-batch100";

/* Insertion threshold (elements). */
#define B100_INS_MAX 16u
/* Max recursion / partition depth before heapsort fallback. */
#define B100_DEPTH_MAX 64u
/* Stack temp capacity for stable mergesort (bytes). */
#define B100_MERGE_STACK 8192u

/* ---- freestanding byte moves (SSE2 bulk when size allows) -------------- */

static void
b100_copy(void *pDst, const void *pSrc, size_t cb)
{
	unsigned char *pD = (unsigned char *)pDst;
	const unsigned char *pS = (const unsigned char *)pSrc;
	size_t i = 0;

	if (cb >= 16u) {
		size_t nBlk = cb & ~(size_t)15u;

		while (i < nBlk) {
			__m128i v = _mm_loadu_si128(
			    (const __m128i *)(const void *)(pS + i));
			_mm_storeu_si128((__m128i *)(void *)(pD + i), v);
			i += 16u;
		}
	}
	while (i < cb) {
		pD[i] = pS[i];
		i++;
	}
}

static void
b100_swap(unsigned char *pA, unsigned char *pB, size_t cb)
{
	size_t i = 0;

	if (cb >= 16u) {
		size_t nBlk = cb & ~(size_t)15u;

		while (i < nBlk) {
			__m128i vA = _mm_loadu_si128(
			    (const __m128i *)(const void *)(pA + i));
			__m128i vB = _mm_loadu_si128(
			    (const __m128i *)(const void *)(pB + i));
			_mm_storeu_si128((__m128i *)(void *)(pA + i), vB);
			_mm_storeu_si128((__m128i *)(void *)(pB + i), vA);
			i += 16u;
		}
	}
	while (i < cb) {
		unsigned char t = pA[i];

		pA[i] = pB[i];
		pB[i] = t;
		i++;
	}
}

/* ---- insertion sort (2-arg and 3-arg comparators) ---------------------- */

static void
b100_ins(unsigned char *p, size_t n, size_t cb,
         int (*pfnCmp)(const void *, const void *))
{
	size_t i, j;

	for (i = 1; i < n; i++) {
		j = i;
		while (j > 0 &&
		       pfnCmp(p + (j - 1u) * cb, p + j * cb) > 0) {
			b100_swap(p + (j - 1u) * cb, p + j * cb, cb);
			j--;
		}
	}
}

static void
b100_ins_r(unsigned char *p, size_t n, size_t cb,
           int (*pfnCmp)(const void *, const void *, void *), void *pArg)
{
	size_t i, j;

	for (i = 1; i < n; i++) {
		j = i;
		while (j > 0 &&
		       pfnCmp(p + (j - 1u) * cb, p + j * cb, pArg) > 0) {
			b100_swap(p + (j - 1u) * cb, p + j * cb, cb);
			j--;
		}
	}
}

/* ---- in-place heapsort core -------------------------------------------- */

static void
b100_sift(unsigned char *p, size_t n, size_t cb, size_t iRoot,
          int (*pfnCmp)(const void *, const void *))
{
	for (;;) {
		size_t iChild = iRoot * 2u + 1u;
		size_t iSwap;

		if (iChild >= n) {
			break;
		}
		iSwap = iRoot;
		if (pfnCmp(p + iSwap * cb, p + iChild * cb) < 0) {
			iSwap = iChild;
		}
		if (iChild + 1u < n &&
		    pfnCmp(p + iSwap * cb, p + (iChild + 1u) * cb) < 0) {
			iSwap = iChild + 1u;
		}
		if (iSwap == iRoot) {
			break;
		}
		b100_swap(p + iRoot * cb, p + iSwap * cb, cb);
		iRoot = iSwap;
	}
}

static void
b100_heap(unsigned char *p, size_t n, size_t cb,
          int (*pfnCmp)(const void *, const void *))
{
	size_t i;

	if (n < 2u) {
		return;
	}
	for (i = n / 2u; i > 0u;) {
		i--;
		b100_sift(p, n, cb, i, pfnCmp);
	}
	while (n > 1u) {
		n--;
		b100_swap(p, p + n * cb, cb);
		b100_sift(p, n, cb, 0, pfnCmp);
	}
}

static void
b100_sift_r(unsigned char *p, size_t n, size_t cb, size_t iRoot,
            int (*pfnCmp)(const void *, const void *, void *), void *pArg)
{
	for (;;) {
		size_t iChild = iRoot * 2u + 1u;
		size_t iSwap;

		if (iChild >= n) {
			break;
		}
		iSwap = iRoot;
		if (pfnCmp(p + iSwap * cb, p + iChild * cb, pArg) < 0) {
			iSwap = iChild;
		}
		if (iChild + 1u < n &&
		    pfnCmp(p + iSwap * cb, p + (iChild + 1u) * cb, pArg) < 0) {
			iSwap = iChild + 1u;
		}
		if (iSwap == iRoot) {
			break;
		}
		b100_swap(p + iRoot * cb, p + iSwap * cb, cb);
		iRoot = iSwap;
	}
}

static void
b100_heap_r(unsigned char *p, size_t n, size_t cb,
            int (*pfnCmp)(const void *, const void *, void *), void *pArg)
{
	size_t i;

	if (n < 2u) {
		return;
	}
	for (i = n / 2u; i > 0u;) {
		i--;
		b100_sift_r(p, n, cb, i, pfnCmp, pArg);
	}
	while (n > 1u) {
		n--;
		b100_swap(p, p + n * cb, cb);
		b100_sift_r(p, n, cb, 0, pfnCmp, pArg);
	}
}

/*
 * Median-of-3, park pivot at last slot, Lomuto partition.
 * Returns final pivot index. Elements left of pivot are < pivot;
 * right are >= pivot. Requires n >= 2.
 */
static size_t
b100_part(unsigned char *p, size_t n, size_t cb,
          int (*pfnCmp)(const void *, const void *))
{
	size_t iLo = 0;
	size_t iHi = n - 1u;
	size_t iMid = n / 2u;
	size_t i, iStore;
	unsigned char *pPivot;

	/* sort three candidates so mid holds the median value */
	if (pfnCmp(p + iLo * cb, p + iMid * cb) > 0) {
		b100_swap(p + iLo * cb, p + iMid * cb, cb);
	}
	if (pfnCmp(p + iMid * cb, p + iHi * cb) > 0) {
		b100_swap(p + iMid * cb, p + iHi * cb, cb);
	}
	if (pfnCmp(p + iLo * cb, p + iMid * cb) > 0) {
		b100_swap(p + iLo * cb, p + iMid * cb, cb);
	}
	/* park median at end */
	b100_swap(p + iMid * cb, p + iHi * cb, cb);
	pPivot = p + iHi * cb;

	iStore = 0;
	for (i = 0; i < iHi; i++) {
		if (pfnCmp(p + i * cb, pPivot) < 0) {
			if (i != iStore) {
				b100_swap(p + i * cb, p + iStore * cb, cb);
			}
			iStore++;
		}
	}
	b100_swap(p + iStore * cb, p + iHi * cb, cb);
	return iStore;
}

static size_t
b100_part_r(unsigned char *p, size_t n, size_t cb,
            int (*pfnCmp)(const void *, const void *, void *), void *pArg)
{
	size_t iLo = 0;
	size_t iHi = n - 1u;
	size_t iMid = n / 2u;
	size_t i, iStore;
	unsigned char *pPivot;

	if (pfnCmp(p + iLo * cb, p + iMid * cb, pArg) > 0) {
		b100_swap(p + iLo * cb, p + iMid * cb, cb);
	}
	if (pfnCmp(p + iMid * cb, p + iHi * cb, pArg) > 0) {
		b100_swap(p + iMid * cb, p + iHi * cb, cb);
	}
	if (pfnCmp(p + iLo * cb, p + iMid * cb, pArg) > 0) {
		b100_swap(p + iLo * cb, p + iMid * cb, cb);
	}
	b100_swap(p + iMid * cb, p + iHi * cb, cb);
	pPivot = p + iHi * cb;

	iStore = 0;
	for (i = 0; i < iHi; i++) {
		if (pfnCmp(p + i * cb, pPivot, pArg) < 0) {
			if (i != iStore) {
				b100_swap(p + i * cb, p + iStore * cb, cb);
			}
			iStore++;
		}
	}
	b100_swap(p + iStore * cb, p + iHi * cb, cb);
	return iStore;
}

static void
b100_qsort(unsigned char *p, size_t n, size_t cb, unsigned uDepth,
           int (*pfnCmp)(const void *, const void *))
{
	while (n > B100_INS_MAX) {
		size_t iPivot;

		if (uDepth == 0u) {
			b100_heap(p, n, cb, pfnCmp);
			return;
		}
		iPivot = b100_part(p, n, cb, pfnCmp);
		/* recurse smaller side; iterate larger (tail) */
		if (iPivot < n - iPivot - 1u) {
			if (iPivot > 0u) {
				b100_qsort(p, iPivot, cb, uDepth - 1u, pfnCmp);
			}
			p += (iPivot + 1u) * cb;
			n = n - iPivot - 1u;
		} else {
			if (iPivot + 1u < n) {
				b100_qsort(p + (iPivot + 1u) * cb,
				           n - iPivot - 1u, cb, uDepth - 1u,
				           pfnCmp);
			}
			n = iPivot;
		}
		uDepth--;
	}
	if (n > 1u) {
		b100_ins(p, n, cb, pfnCmp);
	}
}

static void
b100_qsort_r(unsigned char *p, size_t n, size_t cb, unsigned uDepth,
             int (*pfnCmp)(const void *, const void *, void *), void *pArg)
{
	while (n > B100_INS_MAX) {
		size_t iPivot;

		if (uDepth == 0u) {
			b100_heap_r(p, n, cb, pfnCmp, pArg);
			return;
		}
		iPivot = b100_part_r(p, n, cb, pfnCmp, pArg);
		if (iPivot < n - iPivot - 1u) {
			if (iPivot > 0u) {
				b100_qsort_r(p, iPivot, cb, uDepth - 1u, pfnCmp,
				             pArg);
			}
			p += (iPivot + 1u) * cb;
			n = n - iPivot - 1u;
		} else {
			if (iPivot + 1u < n) {
				b100_qsort_r(p + (iPivot + 1u) * cb,
				             n - iPivot - 1u, cb, uDepth - 1u,
				             pfnCmp, pArg);
			}
			n = iPivot;
		}
		uDepth--;
	}
	if (n > 1u) {
		b100_ins_r(p, n, cb, pfnCmp, pArg);
	}
}

/* ---- public surface ---------------------------------------------------- */

void
gj_qsort(void *pBase, size_t nMemb, size_t cb,
         int (*pfnCmp)(const void *, const void *))
{
	if (pBase == NULL || pfnCmp == NULL || cb == 0u || nMemb < 2u) {
		return;
	}
	b100_qsort((unsigned char *)pBase, nMemb, cb, B100_DEPTH_MAX, pfnCmp);
}

void
gj_qsort_r(void *pBase, size_t nMemb, size_t cb,
           int (*pfnCmp)(const void *, const void *, void *), void *pArg)
{
	if (pBase == NULL || pfnCmp == NULL || cb == 0u || nMemb < 2u) {
		return;
	}
	b100_qsort_r((unsigned char *)pBase, nMemb, cb, B100_DEPTH_MAX, pfnCmp,
	             pArg);
}

int
gj_heapsort(void *pBase, size_t nMemb, size_t cb,
            int (*pfnCmp)(const void *, const void *))
{
	if (pBase == NULL || pfnCmp == NULL || cb == 0u) {
		errno = EINVAL;
		return -1;
	}
	if (nMemb < 2u) {
		return 0;
	}
	b100_heap((unsigned char *)pBase, nMemb, cb, pfnCmp);
	return 0;
}

/*
 * Stable bottom-up mergesort. Uses stack temp when nMemb*cb <= 8 KiB;
 * otherwise ENOMEM (no malloc — freestanding).
 */
int
gj_mergesort(void *pBase, size_t nMemb, size_t cb,
             int (*pfnCmp)(const void *, const void *))
{
	unsigned char *p;
	unsigned char aTmp[B100_MERGE_STACK];
	size_t cbTotal;
	size_t uWidth;
	size_t i;

	if (pBase == NULL || pfnCmp == NULL || cb == 0u) {
		errno = EINVAL;
		return -1;
	}
	if (nMemb < 2u) {
		return 0;
	}
	if (cb > B100_MERGE_STACK / nMemb) {
		errno = ENOMEM;
		return -1;
	}
	cbTotal = nMemb * cb;
	if (cbTotal > B100_MERGE_STACK) {
		errno = ENOMEM;
		return -1;
	}

	p = (unsigned char *)pBase;
	for (uWidth = 1u; uWidth < nMemb; uWidth *= 2u) {
		for (i = 0u; i < nMemb; i += 2u * uWidth) {
			size_t iLeft = i;
			size_t iMid = i + uWidth;
			size_t iRight = i + 2u * uWidth;
			size_t iA, iB, iD;

			if (iMid > nMemb) {
				iMid = nMemb;
			}
			if (iRight > nMemb) {
				iRight = nMemb;
			}
			iA = iLeft;
			iB = iMid;
			iD = iLeft;
			while (iA < iMid && iB < iRight) {
				if (pfnCmp(p + iA * cb, p + iB * cb) <= 0) {
					b100_copy(aTmp + iD * cb, p + iA * cb,
					          cb);
					iA++;
				} else {
					b100_copy(aTmp + iD * cb, p + iB * cb,
					          cb);
					iB++;
				}
				iD++;
			}
			while (iA < iMid) {
				b100_copy(aTmp + iD * cb, p + iA * cb, cb);
				iA++;
				iD++;
			}
			while (iB < iRight) {
				b100_copy(aTmp + iD * cb, p + iB * cb, cb);
				iB++;
				iD++;
			}
		}
		b100_copy(p, aTmp, cbTotal);
	}
	return 0;
}

void __gj_qsort(void *pBase, size_t nMemb, size_t cb,
                int (*pfnCmp)(const void *, const void *))
    __attribute__((alias("gj_qsort")));

void __gj_qsort_r(void *pBase, size_t nMemb, size_t cb,
                  int (*pfnCmp)(const void *, const void *, void *), void *pArg)
    __attribute__((alias("gj_qsort_r")));

int __gj_heapsort(void *pBase, size_t nMemb, size_t cb,
                  int (*pfnCmp)(const void *, const void *))
    __attribute__((alias("gj_heapsort")));

int __gj_mergesort(void *pBase, size_t nMemb, size_t cb,
                   int (*pfnCmp)(const void *, const void *))
    __attribute__((alias("gj_mergesort")));
