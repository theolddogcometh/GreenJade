/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3669: top-k candidate predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_topk_candidate_u(uint64_t min_of_topk, size_t n, size_t k,
 *                           uint64_t v);
 *     - Return 1 if value v would enter a top-k-largest min-heap with
 *       current size n and root min_of_topk:
 *         * k == 0 → 0 (no capacity)
 *         * n < k  → 1 (heap not yet full)
 *         * n >= k → 1 iff v > min_of_topk (strictly beats current min)
 *       Pure predicate — does not mutate any heap storage.
 *   int __gj_topk_candidate_u  (alias)
 *   __libcgj_batch3669_marker = "libcgj-batch3669"
 *
 * Exclusive continuum CREATE-ONLY (3661-3670). Distinct from
 * gj_topk_offer (batch497) — unique candidate _u surface only; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3669_marker[] = "libcgj-batch3669";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if v is a top-k candidate given current heap size cN, capacity cK,
 * and current minimum of retained values uMin.
 */
static int
b3669_candidate(uint64_t uMin, size_t cN, size_t cK, uint64_t u64V)
{
	if (cK == 0u) {
		return 0;
	}
	if (cN < cK) {
		return 1;
	}
	return (u64V > uMin) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_topk_candidate_u - 1 if v would enter the top-k min-heap, else 0.
 *
 * min_of_topk: current root (minimum of retained top-k); ignored when
 *              n < k
 * n:           current heap occupancy
 * k:           capacity (top-k bound)
 * v:           offered value
 *
 * Does not call libc. No parent wires.
 */
int
gj_topk_candidate_u(uint64_t u64Min, size_t cN, size_t cK, uint64_t u64V)
{
	(void)NULL;
	return b3669_candidate(u64Min, cN, cK, u64V);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_topk_candidate_u(uint64_t u64Min, size_t cN, size_t cK,
                          uint64_t u64V)
    __attribute__((alias("gj_topk_candidate_u")));
