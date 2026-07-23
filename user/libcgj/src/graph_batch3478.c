/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3478: freestanding uint64 queue pop on caller
 * storage (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_queue_u64_pop_u(uint64_t *buf, size_t cap, size_t *h, size_t *t,
 *                          size_t *n, uint64_t *out);
 *     - Dequeue one uint64_t from tail into *out. Returns 0 on success,
 *       -1 if empty or bad args.
 *   int __gj_queue_u64_pop_u  (alias)
 *   __libcgj_batch3478_marker = "libcgj-batch3478"
 *
 * Queue design (count-based head/tail):
 *   h / *h — next write index (accepted for API symmetry; unused)
 *   t / *t — next read index (mod cap)
 *   n / *n — elements currently stored (0..cap)
 *   empty when *n == 0
 *
 * Exclusive continuum CREATE-ONLY (3471-3480). Distinct from
 * gj_queue_u32_pop_u (batch2663) / gj_queue_u32_pop (batch437) —
 * unique queue_u64_pop_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3478_marker[] = "libcgj-batch3478";

/* ---- freestanding helpers ---------------------------------------------- */

/* Advance index by one with modular wrap (any cap > 0). */
static size_t
b3478_next(size_t iIdx, size_t cCap)
{
	size_t iNext;

	iNext = iIdx + 1u;
	if (iNext >= cCap) {
		iNext = 0u;
	}
	return iNext;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_queue_u64_pop_u - dequeue one uint64_t into *out.
 *
 * Advances tail modulo cap and decrements count. Does not touch head.
 * Returns 0 on success; -1 if empty, zero capacity, or NULL required args.
 * (h is accepted for API symmetry with push; unused here.)
 * *out and cursors are left untouched on failure.
 */
int
gj_queue_u64_pop_u(uint64_t *pBuf, size_t cCap, size_t *pH, size_t *pT,
                   size_t *pN, uint64_t *pOut)
{
	size_t iT;

	(void)NULL;
	(void)pH;

	if (pBuf == NULL || pT == NULL || pN == NULL || pOut == NULL ||
	    cCap == 0u) {
		return -1;
	}
	if (*pN == 0u) {
		return -1;
	}

	iT = *pT;
	if (iT >= cCap) {
		/* Corrupt cursor — refuse rather than read out of bounds. */
		return -1;
	}
	*pOut = pBuf[iT];
	*pT = b3478_next(iT, cCap);
	(*pN)--;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_queue_u64_pop_u(uint64_t *pBuf, size_t cCap, size_t *pH, size_t *pT,
                         size_t *pN, uint64_t *pOut)
    __attribute__((alias("gj_queue_u64_pop_u")));
