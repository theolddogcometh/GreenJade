/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3323: freestanding uint32_t deque push-back (_u).
 *
 * Surface (unique symbols):
 *   int gj_deque_u32_push_back_u(uint32_t *buf, size_t cap, size_t *head,
 *                                size_t *tail, size_t *count, uint32_t v);
 *     - Insert v after the current back (head). Returns 0 on success,
 *       -1 if full or bad args.
 *   int __gj_deque_u32_push_back_u  (alias)
 *   __libcgj_batch3323_marker = "libcgj-batch3323"
 *
 * Exclusive continuum CREATE-ONLY (3321-3330). Distinct from
 * gj_deque_u32_push_back (batch438) and gj_u32_deque_push_back
 * (batch704) — unique _u surface only; no multi-def. No parent wires.
 *
 * Ring layout (count-based, same as batch438 / batch3321 init):
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

const char __libcgj_batch3323_marker[] = "libcgj-batch3323";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deque_u32_push_back_u - insert one uint32_t at the back.
 *
 * Stores v at head, advances head modulo cap, increments count.
 * Returns 0 on success; -1 if full, zero capacity, or NULL required args.
 * (tail accepted for API symmetry with push_front; unused here.)
 */
int
gj_deque_u32_push_back_u(uint32_t *pBuf, size_t cCap, size_t *pHead,
                         size_t *pTail, size_t *pCount, uint32_t u32V)
{
	size_t iH;

	(void)pTail;

	if (pBuf == NULL || pHead == NULL || pCount == NULL || cCap == 0u) {
		return -1;
	}
	if (*pCount >= cCap) {
		return -1;
	}

	iH = *pHead;
	if (iH >= cCap) {
		/* Corrupt cursor — refuse rather than write out of bounds. */
		return -1;
	}
	pBuf[iH] = u32V;
	iH++;
	if (iH >= cCap) {
		iH = 0u;
	}
	*pHead = iH;
	(*pCount)++;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deque_u32_push_back_u(uint32_t *pBuf, size_t cCap, size_t *pHead,
                               size_t *pTail, size_t *pCount, uint32_t u32V)
    __attribute__((alias("gj_deque_u32_push_back_u")));
