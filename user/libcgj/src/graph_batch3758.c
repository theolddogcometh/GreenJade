/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3758: gj_topo_ready_u - Kahn indegree ready step.
 *
 * Surface (unique symbols):
 *   int gj_topo_ready_u(uint32_t *indeg, uint32_t n, uint32_t v);
 *     - One Kahn topological-sort predecessor-satisfaction step: if
 *       indeg[v] > 0, decrement it; return 1 when the result is 0 (v is
 *       now ready to enqueue). Returns 0 if already zero, or on bad args
 *       (NULL indeg, v ≥ n, n == 0). Does not go negative.
 *   int __gj_topo_ready_u  (alias)
 *   __libcgj_batch3758_marker = "libcgj-batch3758"
 *
 * Graph exclusive continuum CREATE-ONLY (3751-3760). Unique
 * gj_topo_ready_u surface only; no multi-def. Typical use after
 * finishing a vertex u: for each successor v of u, call this and enqueue
 * when it returns 1. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3758_marker[] = "libcgj-batch3758";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_topo_ready_u - decrement indeg[v]; report when v becomes ready.
 *
 * indeg: per-vertex remaining in-degree (caller-owned; may be NULL → 0)
 * n:     vertex count
 * v:     successor whose predecessor was just processed, in [0, n)
 *
 * Returns 1 if v just became zero-indegree (ready), else 0.
 */
int
gj_topo_ready_u(uint32_t *pIndeg, uint32_t u32N, uint32_t u32V)
{
	uint32_t u32Cur;

	(void)NULL;
	if (pIndeg == NULL || u32N == 0u || u32V >= u32N) {
		return 0;
	}
	u32Cur = pIndeg[u32V];
	if (u32Cur == 0u) {
		return 0;
	}
	u32Cur--;
	pIndeg[u32V] = u32Cur;
	return (u32Cur == 0u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_topo_ready_u(uint32_t *pIndeg, uint32_t u32N, uint32_t u32V)
    __attribute__((alias("gj_topo_ready_u")));
