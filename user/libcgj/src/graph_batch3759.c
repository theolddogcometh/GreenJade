/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3759: gj_scc_link_min_u - Tarjan lowlink min.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scc_link_min_u(uint32_t *low, uint32_t n, uint32_t v,
 *                              uint32_t cand);
 *     - Tarjan / path-based SCC low-link update: low[v] = min(low[v],
 *       cand). Returns the resulting low[v]. On bad args (NULL low,
 *       v ≥ n, n == 0) returns cand without writing. Used for both
 *       tree-edge (cand = low[w]) and back/cross-edge (cand = disc[w])
 *       link steps.
 *   uint32_t __gj_scc_link_min_u  (alias)
 *   __libcgj_batch3759_marker = "libcgj-batch3759"
 *
 * Graph exclusive continuum CREATE-ONLY (3751-3760). Unique
 * gj_scc_link_min_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3759_marker[] = "libcgj-batch3759";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scc_link_min_u - low[v] = min(low[v], cand); return low[v].
 *
 * low:  per-vertex low-link values (caller-owned; may be NULL → cand)
 * n:    vertex count
 * v:    vertex whose low-link is updated, in [0, n)
 * cand: candidate discovery / low-link value to fold in
 *
 * Returns the updated low-link (or cand on bad args).
 */
uint32_t
gj_scc_link_min_u(uint32_t *pLow, uint32_t u32N, uint32_t u32V, uint32_t u32Cand)
{
	uint32_t u32Cur;

	(void)NULL;
	if (pLow == NULL || u32N == 0u || u32V >= u32N) {
		return u32Cand;
	}
	u32Cur = pLow[u32V];
	if (u32Cand < u32Cur) {
		pLow[u32V] = u32Cand;
		return u32Cand;
	}
	return u32Cur;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scc_link_min_u(uint32_t *pLow, uint32_t u32N, uint32_t u32V,
    uint32_t u32Cand)
    __attribute__((alias("gj_scc_link_min_u")));
