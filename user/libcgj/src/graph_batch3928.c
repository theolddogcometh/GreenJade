/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3928: freestanding uint64_t deque push-front (_u).
 *
 * Surface (unique symbols):
 *   int gj_deque_u64_push_front_u(uint64_t *buf, size_t cap, size_t *head,
 *                                 size_t *tail, size_t *count, uint64_t v);
 *     - Insert v before the current front (tail). Returns 0 on success,
 *       -1 if full or bad args.
 *   int __gj_deque_u64_push_front_u  (alias)
 *   __libcgj_batch3928_marker = "libcgj-batch3928"
 *
 * Exclusive continuum CREATE-ONLY (3921-3930). Distinct from
 * gj_deque_u32_push_front_u (batch3322), gj_deque_u32_push_front
 * (batch438), and gj_u32_deque_push_front (batch703) — unique
 * deque_u64_*_u surface only; no multi-def. No parent wires.
 *
 * Ring layout (count-based, same as batch438 / batch3322):
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

const char __libcgj_batch3928_marker[] = "libcgj-batch3928";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deque_u64_push_front_u - insert one uint64_t at the front.
 *
 * Retreats tail modulo cap, then stores v at the new front slot.
 * Returns 0 on success; -1 if full, zero capacity, or NULL required args.
 * (head accepted for API symmetry with push_back; unused here.)
 */
int
gj_deque_u64_push_front_u(uint64_t *pBuf, size_t cCap, size_t *pHead,
                          size_t *pTail, size_t *pCount, uint64_t u64V)
{
	size_t iT;

	(void)pHead;

	if (pBuf == NULL || pTail == NULL || pCount == NULL || cCap == 0u) {
		return -1;
	}
	if (*pCount >= cCap) {
		return -1;
	}

	iT = *pTail;
	if (iT >= cCap) {
		/* Corrupt cursor — refuse rather than write out of bounds. */
		return -1;
	}

	/* Retreat tail: front slot is one before the previous front. */
	if (iT == 0u) {
		iT = cCap - 1u;
	} else {
		iT--;
	}
	pBuf[iT] = u64V;
	*pTail = iT;
	(*pCount)++;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deque_u64_push_front_u(uint64_t *pBuf, size_t cCap, size_t *pHead,
                                size_t *pTail, size_t *pCount, uint64_t u64V)
    __attribute__((alias("gj_deque_u64_push_front_u")));
