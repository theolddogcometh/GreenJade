/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1548: freestanding key-value u32 min-heap push
 * over parallel key/value arrays.
 *
 * Surface (unique symbols):
 *   int gj_kv32_heap_push(uint32_t *keys, uint32_t *vals, size_t *n,
 *                         size_t cap, uint32_t key, uint32_t val);
 *     — Append (key, val) to parallel arrays keys[0..*n), vals[0..*n)
 *       with capacity cap, then sift-up by key (min-heap on keys).
 *       Returns 0 on success, -1 if keys/vals/n is NULL or heap is full
 *       (*n >= cap).
 *   int __gj_kv32_heap_push  (alias)
 *   __libcgj_batch1548_marker = "libcgj-batch1548"
 *
 * Layout: 0-based indices; parent (i-1)/2. Heap order is on keys only;
 * values ride along when keys are swapped. Distinct from gj_u32_heap_push
 * (batch673), gj_kv_lookup (batch544), and scalar heap surfaces. Unique
 * gj_ names only; no multi-def. Self-contained static sift-up.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1548_marker[] = "libcgj-batch1548";

/* ---- internal helpers -------------------------------------------------- */

static void
b1548_swap_u32(uint32_t *pA, uint32_t *pB)
{
	uint32_t u32Tmp;

	u32Tmp = *pA;
	*pA = *pB;
	*pB = u32Tmp;
}

/*
 * Sift pair at index i up toward root while its key is smaller than parent.
 * Swaps both keys[i] and vals[i] with the parent pair on each step.
 */
static void
b1548_sift_up(uint32_t *pKeys, uint32_t *pVals, size_t i)
{
	size_t iParent;

	while (i > 0u) {
		iParent = (i - 1u) / 2u;
		if (pKeys[i] >= pKeys[iParent]) {
			break;
		}
		b1548_swap_u32(&pKeys[i], &pKeys[iParent]);
		b1548_swap_u32(&pVals[i], &pVals[iParent]);
		i = iParent;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kv32_heap_push — min-heap insert of (key, val) into parallel arrays.
 *
 * keys: key storage ordered as a min-heap (NULL → -1)
 * vals: value storage parallel to keys (NULL → -1)
 * n:    live element count in/out (NULL → -1)
 * cap:  maximum pairs the arrays can hold; full when *n >= cap → -1
 * key:  priority key (heap ordered by this)
 * val:  payload value moved with its key
 *
 * On success: keys[*n] = key, vals[*n] = val, *n increased by 1, sift-up
 * restores min-heap order on keys; returns 0.
 */
int
gj_kv32_heap_push(uint32_t *pKeys, uint32_t *pVals, size_t *pN, size_t cCap,
    uint32_t u32Key, uint32_t u32Val)
{
	size_t cCur;

	if (pKeys == NULL || pVals == NULL || pN == NULL) {
		return -1;
	}

	cCur = *pN;
	if (cCur >= cCap) {
		return -1;
	}

	pKeys[cCur] = u32Key;
	pVals[cCur] = u32Val;
	*pN = cCur + 1u;
	b1548_sift_up(pKeys, pVals, cCur);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_kv32_heap_push(uint32_t *pKeys, uint32_t *pVals, size_t *pN,
    size_t cCap, uint32_t u32Key, uint32_t u32Val)
    __attribute__((alias("gj_kv32_heap_push")));
