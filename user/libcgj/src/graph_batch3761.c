/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3761: Dijkstra single-edge relax primitive (_u).
 *
 * Surface (unique symbols):
 *   int gj_dijkstra_relax_u(uint64_t *dist_v, uint64_t dist_u, uint64_t w);
 *     - One non-negative Dijkstra edge relax for u→v:
 *         cand = sat_add(dist_u, w);  if cand < *dist_v then *dist_v = cand
 *       UINT64_MAX is the unreachable / infinite-distance sentinel.
 *       If dist_u or w is UINT64_MAX the edge is treated as non-finite and
 *       no update occurs. Saturated add never wraps.
 *     Returns 1 if *dist_v was strictly improved, 0 otherwise (including
 *       NULL dist_v or non-finite inputs).
 *   int __gj_dijkstra_relax_u  (alias)
 *   __libcgj_batch3761_marker = "libcgj-batch3761"
 *
 * Exclusive continuum CREATE-ONLY (3761-3770: dijkstra_relax_u,
 * bellman_relax_u, floyd_cell_u, prim_pick_u, kruskal_find_u,
 * kruskal_union_u, mst_edge_ok_u, flow_cap_rem_u, flow_push_u,
 * batch_id_3770). Distinct from gj_dijkstra_dense (batch224) full SSSP —
 * unique single-edge _u surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3761_marker[] = "libcgj-batch3761";

/* Unreachable / non-finite sentinel. */
#define B3761_INF ((uint64_t)UINT64_MAX)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Saturated add for non-negative distances in [0, INF]:
 * min(a + b, INF) without wrap.
 */
static uint64_t
b3761_add_sat(uint64_t uA, uint64_t uB)
{
	if (uA >= B3761_INF || uB >= B3761_INF) {
		return B3761_INF;
	}
	if (uB > B3761_INF - uA) {
		return B3761_INF;
	}
	return uA + uB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dijkstra_relax_u — relax one non-negative edge into *dist_v.
 *
 * dist_v: tentative distance at v (updated in place when improved)
 * dist_u: tentative distance at u
 * w:      non-negative edge weight u→v (UINT64_MAX = missing edge)
 *
 * Returns 1 if improved, else 0. No parent wires.
 */
int
gj_dijkstra_relax_u(uint64_t *dist_v, uint64_t dist_u, uint64_t w)
{
	uint64_t uCand;

	(void)NULL;

	if (dist_v == NULL) {
		return 0;
	}
	if (dist_u >= B3761_INF || w >= B3761_INF) {
		return 0;
	}

	uCand = b3761_add_sat(dist_u, w);
	if (uCand < *dist_v) {
		*dist_v = uCand;
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dijkstra_relax_u(uint64_t *dist_v, uint64_t dist_u, uint64_t w)
    __attribute__((alias("gj_dijkstra_relax_u")));
