/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3931: freestanding uint64_t deque pop-front (_u).
 *
 * Surface (unique symbols):
 *   int gj_deque_u64_pop_front_u(uint64_t *buf, size_t cap, size_t *head,
 *                                size_t *tail, size_t *count, uint64_t *out);
 *     - Pop one uint64_t from the front (tail) into *out. Returns 0 on
 *       success, -1 if empty or bad args.
 *   int __gj_deque_u64_pop_front_u  (alias)
 *   __libcgj_batch3931_marker = "libcgj-batch3931"
 *
 * Exclusive continuum CREATE-ONLY (3931-3940). Distinct from
 * gj_deque_u32_pop_front_u (batch3324), gj_u32_deque_pop_front (batch705),
 * and gj_queue_u64_pop_u (batch3478) — unique deque_u64_pop_front_u
 * surface only; no multi-def. No parent wires.
 *
 * Ring layout (count-based, same model as batch438 / batch3321):
 *   head  — next back-write index (mod cap)
 *   tail  — front / next-read index (mod cap)
 *   count — elements currently stored (0..cap)
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3931_marker[] = "libcgj-batch3931";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deque_u64_pop_front_u - remove one uint64_t from the front.
 *
 * Reads from tail, advances tail modulo cap, decrements count.
 * Returns 0 on success; -1 if empty, zero capacity, or NULL required args.
 * (head accepted for API symmetry with pop_back; unused here.)
 */
int
gj_deque_u64_pop_front_u(uint64_t *pBuf, size_t cCap, size_t *pHead,
                         size_t *pTail, size_t *pCount, uint64_t *pOut)
{
	size_t iT;

	(void)pHead;

	if (pBuf == NULL || pTail == NULL || pCount == NULL || pOut == NULL ||
	    cCap == 0u) {
		return -1;
	}
	if (*pCount == 0u) {
		return -1;
	}

	iT = *pTail;
	if (iT >= cCap) {
		/* Corrupt cursor — refuse rather than read out of bounds. */
		return -1;
	}
	*pOut = pBuf[iT];
	iT++;
	if (iT >= cCap) {
		iT = 0u;
	}
	*pTail = iT;
	(*pCount)--;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deque_u64_pop_front_u(uint64_t *pBuf, size_t cCap, size_t *pHead,
                               size_t *pTail, size_t *pCount, uint64_t *pOut)
    __attribute__((alias("gj_deque_u64_pop_front_u")));
