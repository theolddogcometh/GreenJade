/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3329: freestanding uint32_t min-heap push (_u).
 *
 * Surface (unique symbols):
 *   int gj_heap_u32_push_u(uint32_t *a, size_t *n, size_t cap, uint32_t v);
 *     - Append v to a caller-owned min-heap a[0..*n) with capacity cap,
 *       then sift-up. Returns 0 on success, -1 if a/n is NULL or heap
 *       is full (*n >= cap). Local sift-up only (no parent wires).
 *   int __gj_heap_u32_push_u  (alias)
 *   __libcgj_batch3329_marker = "libcgj-batch3329"
 *
 * Exclusive continuum CREATE-ONLY (3321-3330). Distinct from
 * gj_u32_heap_push (batch673) — unique heap_u32_*_u surface only;
 * no multi-def. No parent wires (does not call batch3327 sift_up).
 *
 * Layout: 0-based indices; parent (i-1)/2, children 2i+1 and 2i+2.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3329_marker[] = "libcgj-batch3329";

/* ---- internal helpers -------------------------------------------------- */

static void
b3329_swap(uint32_t *pA, uint32_t *pB)
{
	uint32_t nTmp;

	nTmp = *pA;
	*pA = *pB;
	*pB = nTmp;
}

/*
 * Sift element at index i up toward root while it is smaller than its parent.
 * Restores min-heap order after an append at the end of the array.
 * Local to this TU — no call to gj_heap_u32_sift_up_u (no parent wires).
 */
static void
b3329_sift_up(uint32_t *pA, size_t i)
{
	size_t iParent;

	while (i > 0u) {
		iParent = (i - 1u) / 2u;
		if (pA[i] >= pA[iParent]) {
			break;
		}
		b3329_swap(&pA[i], &pA[iParent]);
		i = iParent;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_heap_u32_push_u - min-heap insert of v into a[0..*n) with capacity cap.
 *
 * a:   heap storage (NULL -> -1)
 * n:   live element count in/out (NULL -> -1)
 * cap: maximum elements a can hold; full when *n >= cap -> -1
 * v:   value to insert
 *
 * On success: a[*n] = v, *n increased by 1, local sift-up restores heap
 * order; returns 0. Does not replace root when full.
 */
int
gj_heap_u32_push_u(uint32_t *pA, size_t *pN, size_t cCap, uint32_t u32V)
{
	size_t nCur;

	if (pA == NULL || pN == NULL) {
		return -1;
	}

	nCur = *pN;
	if (nCur >= cCap) {
		return -1;
	}

	pA[nCur] = u32V;
	*pN = nCur + 1u;
	b3329_sift_up(pA, nCur);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_heap_u32_push_u(uint32_t *pA, size_t *pN, size_t cCap, uint32_t u32V)
    __attribute__((alias("gj_heap_u32_push_u")));
