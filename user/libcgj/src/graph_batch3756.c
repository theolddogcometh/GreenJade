/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3756: gj_bfs_visit_mark_u - BFS visit bit mark.
 *
 * Surface (unique symbols):
 *   int gj_bfs_visit_mark_u(uint32_t *seen, uint32_t n, uint32_t v);
 *     - BFS discovery step on a dense visit bitset over n vertices
 *       (uint32_t words, LSB-first, same packing as gj_bitset_*). If v
 *       is unvisited, set its bit and return 1 (caller should enqueue).
 *       If already visited or on bad args (NULL seen, v ≥ n, n == 0),
 *       return 0.
 *   int __gj_bfs_visit_mark_u  (alias)
 *   __libcgj_batch3756_marker = "libcgj-batch3756"
 *
 * Graph exclusive continuum CREATE-ONLY (3751-3760). Unique
 * gj_bfs_visit_mark_u surface only; no multi-def. Distinct from
 * gj_dfs_visit_mark_u (batch3757 color white→gray). No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3756_marker[] = "libcgj-batch3756";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bfs_visit_mark_u - mark vertex v discovered for BFS if not yet seen.
 *
 * seen: visit bitset over n vertices (caller zero-inits; may be NULL → 0)
 * n:    vertex count
 * v:    vertex to mark in [0, n)
 *
 * Returns 1 if newly marked, 0 if already set or bad args.
 */
int
gj_bfs_visit_mark_u(uint32_t *pSeen, uint32_t u32N, uint32_t u32V)
{
	size_t iWord;
	uint32_t u32Bit;
	uint32_t u32Mask;
	uint32_t u32Old;

	(void)NULL;
	if (pSeen == NULL || u32N == 0u || u32V >= u32N) {
		return 0;
	}
	iWord = (size_t)(u32V >> 5);
	u32Bit = u32V & 31u;
	u32Mask = (uint32_t)1u << u32Bit;
	u32Old = pSeen[iWord];
	if ((u32Old & u32Mask) != 0u) {
		return 0;
	}
	pSeen[iWord] = u32Old | u32Mask;
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bfs_visit_mark_u(uint32_t *pSeen, uint32_t u32N, uint32_t u32V)
    __attribute__((alias("gj_bfs_visit_mark_u")));
