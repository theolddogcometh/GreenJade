/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3755: gj_graph_edge_count_u - total edge count.
 *
 * Surface (unique symbols):
 *   uint32_t gj_graph_edge_count_u(const uint32_t *mat, uint32_t n);
 *     - Count all directed edges in a dense n×n adjacency bit-matrix
 *       packed over uint32_t words (row-major, LSB-first). Partial
 *       final word of each row is masked to n bits so padding bits are
 *       ignored. Returns 0 on bad args (NULL mat or n == 0). Saturates
 *       at UINT32_MAX if the true total would exceed that.
 *   uint32_t __gj_graph_edge_count_u  (alias)
 *   __libcgj_batch3755_marker = "libcgj-batch3755"
 *
 * Graph exclusive continuum CREATE-ONLY (3751-3760). Unique
 * gj_graph_edge_count_u surface only; no multi-def. Companion per-vertex
 * degree: gj_graph_degree_u (batch3754). No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3755_marker[] = "libcgj-batch3755";

/* ---- freestanding helpers ---------------------------------------------- */

/* Words per adjacency row for n vertices (ceil(n / 32)). */
static uint32_t
b3755_row_words(uint32_t u32N)
{
	return (u32N + 31u) >> 5;
}

/* Mask for the low cRem bits of a word (cRem in 1..31). */
static uint32_t
b3755_lo_mask(uint32_t u32Rem)
{
	return ((uint32_t)1u << u32Rem) - 1u;
}

static uint32_t
b3755_pop32(uint32_t u32W)
{
	return (uint32_t)__builtin_popcount((unsigned)u32W);
}

/* Add delta into *pAcc, saturating at UINT32_MAX. */
static void
b3755_add_sat(uint32_t *pAcc, uint32_t u32Delta)
{
	uint32_t u32Cur;

	u32Cur = *pAcc;
	if (u32Delta > (UINT32_MAX - u32Cur)) {
		*pAcc = UINT32_MAX;
	} else {
		*pAcc = u32Cur + u32Delta;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_graph_edge_count_u - total directed edge count in a dense adj matrix.
 *
 * mat: packed n×n adjacency (caller-owned; may be NULL → 0)
 * n:   vertex count
 *
 * Returns the number of set edge bits across all rows (padding masked).
 */
uint32_t
gj_graph_edge_count_u(const uint32_t *pMat, uint32_t u32N)
{
	uint32_t u32RowW;
	uint32_t u32Rem;
	uint32_t u32Full;
	uint32_t u32Total;
	uint32_t u32U;
	uint32_t iW;
	size_t iBase;

	(void)NULL;
	if (pMat == NULL || u32N == 0u) {
		return 0u;
	}

	u32RowW = b3755_row_words(u32N);
	u32Rem = u32N & 31u;
	u32Full = (u32Rem == 0u) ? u32RowW : (u32RowW - 1u);
	u32Total = 0u;

	for (u32U = 0u; u32U < u32N; u32U++) {
		iBase = (size_t)u32U * (size_t)u32RowW;
		for (iW = 0u; iW < u32Full; iW++) {
			b3755_add_sat(&u32Total,
			    b3755_pop32(pMat[iBase + (size_t)iW]));
			if (u32Total == UINT32_MAX) {
				return UINT32_MAX;
			}
		}
		if (u32Rem != 0u) {
			b3755_add_sat(&u32Total,
			    b3755_pop32(pMat[iBase + (size_t)u32Full] &
			        b3755_lo_mask(u32Rem)));
			if (u32Total == UINT32_MAX) {
				return UINT32_MAX;
			}
		}
	}
	return u32Total;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_graph_edge_count_u(const uint32_t *pMat, uint32_t u32N)
    __attribute__((alias("gj_graph_edge_count_u")));
