/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3753: gj_graph_adj_clr_u - dense adj bit clear.
 *
 * Surface (unique symbols):
 *   void gj_graph_adj_clr_u(uint32_t *mat, uint32_t n, uint32_t u,
 *                           uint32_t v);
 *     - Clear directed edge u → v in a dense n×n adjacency bit-matrix
 *       packed over uint32_t words, row-major, LSB-first within each
 *       word. Row stride is ((n + 31) / 32) words. No-op on bad args
 *       (NULL mat, or any index ≥ n, or n == 0).
 *   void __gj_graph_adj_clr_u  (alias)
 *   __libcgj_batch3753_marker = "libcgj-batch3753"
 *
 * Graph exclusive continuum CREATE-ONLY (3751-3760). Unique
 * gj_graph_adj_clr_u surface only; no multi-def. Companion probe/set:
 * gj_graph_adj_test_u (batch3751), gj_graph_adj_set_u (batch3752).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3753_marker[] = "libcgj-batch3753";

/* ---- freestanding helpers ---------------------------------------------- */

/* Words per adjacency row for n vertices (ceil(n / 32)). */
static uint32_t
b3753_row_words(uint32_t u32N)
{
	return (u32N + 31u) >> 5;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_graph_adj_clr_u - clear directed edge u → v in a dense bit matrix.
 *
 * mat: packed n×n adjacency (caller-owned; may be NULL → no-op)
 * n:   vertex count
 * u:   source vertex in [0, n)
 * v:   target vertex in [0, n)
 *
 * Idempotent clear. Does not call libc.
 */
void
gj_graph_adj_clr_u(uint32_t *pMat, uint32_t u32N, uint32_t u32U, uint32_t u32V)
{
	uint32_t u32RowW;
	size_t iWord;
	uint32_t u32Bit;

	(void)NULL;
	if (pMat == NULL || u32N == 0u || u32U >= u32N || u32V >= u32N) {
		return;
	}
	u32RowW = b3753_row_words(u32N);
	iWord = (size_t)u32U * (size_t)u32RowW + (size_t)(u32V >> 5);
	u32Bit = u32V & 31u;
	pMat[iWord] &= ~((uint32_t)1u << u32Bit);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_graph_adj_clr_u(uint32_t *pMat, uint32_t u32N, uint32_t u32U,
    uint32_t u32V)
    __attribute__((alias("gj_graph_adj_clr_u")));
