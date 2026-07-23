/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2293: graph batch gap predicate.
 *
 * Surface (unique symbols):
 *   int gj_batch_gap_p(uint32_t prev, uint32_t next);
 *     - Return 1 if there is a positive gap between consecutive batch
 *       ids: next > prev + 1 (at least one missing id between them).
 *       Else 0 (including equal, inverted, or contiguous next==prev+1).
 *   int __gj_batch_gap_p  (alias)
 *   __libcgj_batch2293_marker = "libcgj-batch2293"
 *
 * Milestone 2300 exclusive product helpers (2291-2300). Unique
 * gj_batch_gap_p surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2293_marker[] = "libcgj-batch2293";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if next is strictly more than one past prev (gap of missing ids).
 * Avoids overflow on prev == UINT32_MAX by checking next > prev first.
 */
static int
b2293_gap_p(uint32_t uPrev, uint32_t uNext)
{
	if (uNext <= uPrev) {
		return 0;
	}
	if (uNext == (uPrev + 1u)) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_gap_p - 1 iff batch ids prev and next have a gap between them.
 *
 * prev: earlier graph batch number
 * next: later graph batch number
 *
 * Returns 1 when next > prev + 1, else 0. Contiguous (next == prev + 1)
 * and inverted or equal pairs return 0. Does not call libc.
 */
int
gj_batch_gap_p(uint32_t uPrev, uint32_t uNext)
{
	(void)NULL;
	return b2293_gap_p(uPrev, uNext);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_batch_gap_p(uint32_t uPrev, uint32_t uNext)
    __attribute__((alias("gj_batch_gap_p")));
