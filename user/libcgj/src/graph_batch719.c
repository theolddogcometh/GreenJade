/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch719: freestanding online k=1 reservoir offer.
 *
 * Surface (unique symbols):
 *   int gj_reservoir_offer(uint32_t *slot, size_t *seen, uint32_t v,
 *                          uint32_t r);
 *     — One step of Algorithm R with reservoir size k=1:
 *         1. (*seen)++  → n  (items seen after this offer; n >= 1)
 *         2. replace *slot with v with probability 1/n, decided by r
 *       r is a single uniform draw on the full 32-bit range
 *       [0, UINT32_MAX]. Index j = ((uint64_t)r * n) >> 32 lands in
 *       [0, n); replace iff j == 0 (probability 1/n). When n == 1 this
 *       always seeds *slot = v. Returns 0 (OK) on success, -1 (FAIL) if
 *       slot or seen is NULL. *slot is unchanged when the coin rejects.
 *   int __gj_reservoir_offer  (alias)
 *   __libcgj_batch719_marker = "libcgj-batch719"
 *
 * Distinct from gj_reservoir_u32 (batch304, offline sample of k indices
 * from [0, n) with caller LCG) and from gj_weighted_sample_one
 * (batch496, one-shot weighted index). Unique gj_reservoir_offer names
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch719_marker[] = "libcgj-batch719";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_reservoir_offer — online k=1 reservoir update for candidate v.
 *
 * slot: single reservoir cell (NULL → -1)
 * seen: items offered so far (in/out); incremented by 1 on every OK call
 * v:    candidate value to optionally store in *slot
 * r:    uniform integer in [0, UINT32_MAX] (full uint32 domain)
 *
 * After advancing *seen to n:
 *   j = ((uint64_t)r * n) >> 32  ∈ [0, n) when 1 <= n <= UINT32_MAX
 *   replace *slot with v iff j == 0
 * When n > UINT32_MAX a 32-bit draw cannot realize 1/n; leave *slot.
 *
 * Returns 0 (OK) or -1 (FAIL). Does not allocate or call libc.
 */
int
gj_reservoir_offer(uint32_t *slot, size_t *seen, uint32_t v, uint32_t r)
{
	size_t cN;
	uint64_t u64Prod;

	if (slot == NULL || seen == NULL) {
		return -1;
	}

	/* Refuse wrap of the seen counter (would make 1/n undefined). */
	if (*seen == (size_t)-1) {
		return -1;
	}

	cN = *seen + 1u;
	*seen = cN;

	/*
	 * Probability 1/cN via multiply-high of full-range r.
	 * cN == 1 → product high half is always 0 → always seed slot.
	 * cN > UINT32_MAX → cannot map 32-bit r into [0, cN); no replace.
	 */
	if (cN > (size_t)UINT32_MAX) {
		return 0;
	}

	u64Prod = (uint64_t)r * (uint64_t)cN;
	if ((u64Prod >> 32) == 0u) {
		*slot = v;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_reservoir_offer(uint32_t *slot, size_t *seen, uint32_t v, uint32_t r)
    __attribute__((alias("gj_reservoir_offer")));
