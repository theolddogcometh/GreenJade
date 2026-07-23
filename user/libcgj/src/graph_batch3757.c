/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3757: gj_dfs_visit_mark_u - DFS white→gray mark.
 *
 * Surface (unique symbols):
 *   int gj_dfs_visit_mark_u(uint32_t *color, uint32_t n, uint32_t v);
 *     - DFS discovery step on a per-vertex color array over n vertices:
 *         0 = white (undiscovered)
 *         1 = gray  (on the recursion / explicit stack)
 *         2 = black (finished) — treated as already visited
 *       If color[v] is white, set it to gray and return 1 (caller should
 *       push / recurse). If already non-white or on bad args (NULL color,
 *       v ≥ n, n == 0), return 0.
 *   int __gj_dfs_visit_mark_u  (alias)
 *   __libcgj_batch3757_marker = "libcgj-batch3757"
 *
 * Graph exclusive continuum CREATE-ONLY (3751-3760). Unique
 * gj_dfs_visit_mark_u surface only; no multi-def. Distinct from
 * gj_bfs_visit_mark_u (batch3756 bitset). No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3757_marker[] = "libcgj-batch3757";

/* White / gray color tags (black = 2 is left for the caller on finish). */
#define B3757_WHITE  0u
#define B3757_GRAY   1u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dfs_visit_mark_u - mark vertex v gray on first DFS discovery.
 *
 * color: per-vertex colors (caller zero-inits to white; may be NULL → 0)
 * n:     vertex count
 * v:     vertex to mark in [0, n)
 *
 * Returns 1 if newly marked gray, 0 if already non-white or bad args.
 */
int
gj_dfs_visit_mark_u(uint32_t *pColor, uint32_t u32N, uint32_t u32V)
{
	(void)NULL;
	if (pColor == NULL || u32N == 0u || u32V >= u32N) {
		return 0;
	}
	if (pColor[u32V] != B3757_WHITE) {
		return 0;
	}
	pColor[u32V] = B3757_GRAY;
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dfs_visit_mark_u(uint32_t *pColor, uint32_t u32N, uint32_t u32V)
    __attribute__((alias("gj_dfs_visit_mark_u")));
