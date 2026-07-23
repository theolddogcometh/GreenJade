/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1612: weighted pick by prefix-sum ticket.
 *
 * Surface (unique symbols):
 *   size_t gj_weighted_pick_u32(const uint32_t *w, size_t n, uint32_t ticket);
 *     — Walk weights[0..n) as a prefix sum; return the first index i
 *       where running_sum > ticket (i.e. ticket falls in weight i's
 *       bucket). ticket is expected in [0, total_weight). NULL w with
 *       n > 0, n == 0, or ticket past total → n (invalid sentinel).
 *       Zero-weight slots are skipped (empty buckets).
 *   size_t __gj_weighted_pick_u32  (alias)
 *   __libcgj_batch1612_marker = "libcgj-batch1612"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1612_marker[] = "libcgj-batch1612";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_weighted_pick_u32 — select an index by cumulative weight ticket.
 *
 * Classic lottery / weighted round-robin style pick without allocation:
 * prefix sums are computed on the fly. Overflow of the running sum is
 * saturated at UINT32_MAX so a full-range ticket still resolves.
 */
size_t
gj_weighted_pick_u32(const uint32_t *pW, size_t cN, uint32_t u32Ticket)
{
	size_t iIdx;
	uint32_t u32Sum;

	if (cN == 0u) {
		return 0u;
	}
	if (pW == NULL) {
		return cN;
	}

	u32Sum = 0u;
	for (iIdx = 0u; iIdx < cN; iIdx++) {
		uint32_t u32W = pW[iIdx];
		uint32_t u32Next;

		if (u32W == 0u) {
			continue;
		}
		/* Saturating add so pathological weight totals stay usable. */
		if (u32Sum > UINT32_MAX - u32W) {
			u32Next = UINT32_MAX;
		} else {
			u32Next = u32Sum + u32W;
		}
		if (u32Ticket < u32Next) {
			return iIdx;
		}
		u32Sum = u32Next;
	}
	return cN;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_weighted_pick_u32(const uint32_t *pW, size_t cN, uint32_t u32Ticket)
    __attribute__((alias("gj_weighted_pick_u32")));
