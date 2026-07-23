/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1549: binary min-heap extract (pop min) for
 * parallel uint32_t key/value arrays.
 *
 * Surface (unique symbols):
 *   int gj_kv32_heap_pop(uint32_t *keys, uint32_t *vals, size_t *n,
 *                        uint32_t *out_key, uint32_t *out_val);
 *     — remove the minimum-key root into *out_key / *out_val; shrink *n;
 *       re-heapify. Returns 0 (OK) on success, -1 (FAIL) if empty or any
 *       pointer is NULL.
 *   int __gj_kv32_heap_pop  (alias)
 *   __libcgj_batch1549_marker = "libcgj-batch1549"
 *
 * Layout: caller-owned parallel arrays; *n is live element count. 0-based
 * indices: parent (i-1)/2, children 2i+1 and 2i+2. Assumes keys[0..*n) is
 * already a min-heap and vals is aligned with keys.
 *
 * Distinct from gj_u32_heap_pop (batch674), gj_kv32_heap_push (batch1548),
 * and gj_kv_lookup (batch544). Unique gj_ names only; no multi-def.
 * Self-contained static sift-down.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1549_marker[] = "libcgj-batch1549";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b1549_swap_u32(uint32_t *pA, uint32_t *pB)
{
	uint32_t u32Tmp;

	u32Tmp = *pA;
	*pA = *pB;
	*pB = u32Tmp;
}

/*
 * Sift pair at index i down toward leaves while its key is larger than a
 * child. Swaps both keys and vals together to keep pairs aligned.
 */
static void
b1549_sift_down(uint32_t *pKeys, uint32_t *pVals, size_t cN, size_t i)
{
	size_t iLeft;
	size_t iRight;
	size_t iSmall;

	for (;;) {
		iLeft = 2u * i + 1u;
		iRight = 2u * i + 2u;
		iSmall = i;

		if (iLeft < cN && pKeys[iLeft] < pKeys[iSmall]) {
			iSmall = iLeft;
		}
		if (iRight < cN && pKeys[iRight] < pKeys[iSmall]) {
			iSmall = iRight;
		}
		if (iSmall == i) {
			break;
		}
		b1549_swap_u32(&pKeys[i], &pKeys[iSmall]);
		b1549_swap_u32(&pVals[i], &pVals[iSmall]);
		i = iSmall;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kv32_heap_pop — extract the minimum-key (root) pair into out_key/out_val.
 *
 * Assumes keys[0..*n) is already a min-heap with vals parallel. On success:
 * *out_key = keys[0], *out_val = vals[0], *n decreased by one, last leaf
 * pair moved to root and sifted down by key.
 *
 * Returns:
 *   0  (OK)   — popped into *out_key / *out_val
 *   -1 (FAIL) — empty heap or any of keys / vals / n / out_key / out_val is NULL
 */
int
gj_kv32_heap_pop(uint32_t *pKeys, uint32_t *pVals, size_t *pN,
    uint32_t *pOutKey, uint32_t *pOutVal)
{
	size_t cCur;

	if (pKeys == NULL || pVals == NULL || pN == NULL ||
	    pOutKey == NULL || pOutVal == NULL) {
		return -1;
	}
	cCur = *pN;
	if (cCur == 0u) {
		return -1;
	}

	*pOutKey = pKeys[0];
	*pOutVal = pVals[0];
	cCur -= 1u;
	*pN = cCur;
	if (cCur > 0u) {
		pKeys[0] = pKeys[cCur];
		pVals[0] = pVals[cCur];
		b1549_sift_down(pKeys, pVals, cCur, 0u);
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_kv32_heap_pop(uint32_t *pKeys, uint32_t *pVals, size_t *pN,
    uint32_t *pOutKey, uint32_t *pOutVal)
    __attribute__((alias("gj_kv32_heap_pop")));
