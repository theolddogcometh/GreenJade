/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3325: freestanding uint32_t deque pop-back (_u).
 *
 * Surface (unique symbols):
 *   int gj_deque_u32_pop_back_u(uint32_t *buf, size_t cap, size_t *head,
 *                               size_t *tail, size_t *count, uint32_t *out);
 *     - Remove one uint32_t from the back (slot before head) into *out.
 *       Returns 0 on success, -1 if empty or bad args.
 *   int __gj_deque_u32_pop_back_u  (alias)
 *   __libcgj_batch3325_marker = "libcgj-batch3325"
 *
 * Exclusive continuum CREATE-ONLY (3321-3330). Distinct from
 * gj_u32_deque_pop_back (batch706) — unique _u surface only; no multi-def.
 * No parent wires.
 *
 * Ring layout (count-based, same as batch438 / batch3321 init):
 *   head  — next back-write index (mod cap)
 *   tail  — front / next-read index (mod cap)
 *   count — elements currently stored (0..cap)
 * Back element lives at (head - 1) mod cap (last slot written by push_back).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3325_marker[] = "libcgj-batch3325";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deque_u32_pop_back_u - remove one uint32_t from the back.
 *
 * Retreats head to the previous write slot, copies that value to *out,
 * decrements count. Returns 0 on success; -1 if empty, zero capacity, or
 * NULL required args. (tail accepted for API symmetry; unused here.)
 */
int
gj_deque_u32_pop_back_u(uint32_t *pBuf, size_t cCap, size_t *pHead,
                        size_t *pTail, size_t *pCount, uint32_t *pOut)
{
	size_t iH;
	size_t iBack;

	(void)pTail;

	if (pBuf == NULL || pHead == NULL || pCount == NULL || pOut == NULL ||
	    cCap == 0u) {
		return -1;
	}
	if (*pCount == 0u) {
		return -1;
	}

	iH = *pHead;
	if (iH >= cCap) {
		/* Corrupt cursor — refuse rather than read out of bounds. */
		return -1;
	}

	/* Back slot is one before the next-write index (mod cap). */
	if (iH == 0u) {
		iBack = cCap - 1u;
	} else {
		iBack = iH - 1u;
	}

	*pOut = pBuf[iBack];
	*pHead = iBack;
	(*pCount)--;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deque_u32_pop_back_u(uint32_t *pBuf, size_t cCap, size_t *pHead,
                              size_t *pTail, size_t *pCount, uint32_t *pOut)
    __attribute__((alias("gj_deque_u32_pop_back_u")));
