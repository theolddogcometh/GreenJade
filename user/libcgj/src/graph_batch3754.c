/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3754: gj_graph_degree_u - out-degree of a vertex.
 *
 * Surface (unique symbols):
 *   uint32_t gj_graph_degree_u(const uint32_t *mat, uint32_t n, uint32_t u);
 *     - Count directed out-edges from vertex u in a dense n×n adjacency
 *       bit-matrix packed over uint32_t words (row-major, LSB-first).
 *       Partial final word of each row is masked to n bits. Returns 0
 *       on bad args (NULL mat, u ≥ n, or n == 0).
 *   uint32_t __gj_graph_degree_u  (alias)
 *   __libcgj_batch3754_marker = "libcgj-batch3754"
 *
 * Graph exclusive continuum CREATE-ONLY (3751-3760). Unique
 * gj_graph_degree_u surface only; no multi-def. Companion full-graph
 * edge total: gj_graph_edge_count_u (batch3755). No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3754_marker[] = "libcgj-batch3754";

/* ---- freestanding helpers ---------------------------------------------- */

/* Words per adjacency row for n vertices (ceil(n / 32)). */
static uint32_t
b3754_row_words(uint32_t u32N)
{
	return (u32N + 31u) >> 5;
}

/* Mask for the low cRem bits of a word (cRem in 1..31). */
static uint32_t
b3754_lo_mask(uint32_t u32Rem)
{
	return ((uint32_t)1u << u32Rem) - 1u;
}

static uint32_t
b3754_pop32(uint32_t u32W)
{
	return (uint32_t)__builtin_popcount((unsigned)u32W);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_graph_degree_u - out-degree of vertex u in a dense adj bit matrix.
 *
 * mat: packed n×n adjacency (caller-owned; may be NULL → 0)
 * n:   vertex count
 * u:   source vertex in [0, n)
 *
 * Returns the number of set bits in row u (directed out-degree).
 */
uint32_t
gj_graph_degree_u(const uint32_t *pMat, uint32_t u32N, uint32_t u32U)
{
	uint32_t u32RowW;
	uint32_t u32Rem;
	uint32_t u32Full;
	uint32_t u32Deg;
	uint32_t iW;
	size_t iBase;

	(void)NULL;
	if (pMat == NULL || u32N == 0u || u32U >= u32N) {
		return 0u;
	}

	u32RowW = b3754_row_words(u32N);
	u32Rem = u32N & 31u;
	u32Full = (u32Rem == 0u) ? u32RowW : (u32RowW - 1u);
	iBase = (size_t)u32U * (size_t)u32RowW;
	u32Deg = 0u;

	for (iW = 0u; iW < u32Full; iW++) {
		u32Deg += b3754_pop32(pMat[iBase + (size_t)iW]);
	}
	if (u32Rem != 0u) {
		u32Deg += b3754_pop32(pMat[iBase + (size_t)u32Full] &
		    b3754_lo_mask(u32Rem));
	}
	return u32Deg;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_graph_degree_u(const uint32_t *pMat, uint32_t u32N, uint32_t u32U)
    __attribute__((alias("gj_graph_degree_u")));
