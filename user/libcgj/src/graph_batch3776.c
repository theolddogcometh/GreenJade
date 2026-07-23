/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3776: single LCS DP cell update (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_lcs_cell_u(size_t diag, size_t up, size_t left,
 *                        uint8_t a, uint8_t b);
 *     - One longest-common-subsequence DP cell:
 *         if a == b → diag + 1
 *         else       → max(up, left)
 *       diag/up/left are the three neighbouring cells (NW / N / W).
 *       Overflow of diag+1 wraps in size_t (caller keeps cells small).
 *   size_t __gj_lcs_cell_u  (alias)
 *   __libcgj_batch3776_marker = "libcgj-batch3776"
 *
 * Exclusive continuum CREATE-ONLY (3771-3780). Distinct from
 * gj_lcs_len (batch214) full-string DP. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3776_marker[] = "libcgj-batch3776";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3776_max2(size_t uA, size_t uB)
{
	return (uA > uB) ? uA : uB;
}

static size_t
b3776_cell(size_t uDiag, size_t uUp, size_t uLeft, uint8_t u8A, uint8_t u8B)
{
	if (u8A == u8B) {
		return uDiag + 1u;
	}
	return b3776_max2(uUp, uLeft);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lcs_cell_u — one LCS DP cell from (diag, up, left) and bytes a,b.
 *
 * diag: DP[i-1][j-1]
 * up:   DP[i-1][j]
 * left: DP[i][j-1]
 * a/b:  characters being compared
 *
 * Returns DP[i][j]. No parent wires.
 */
size_t
gj_lcs_cell_u(size_t diag, size_t up, size_t left, uint8_t a, uint8_t b)
{
	(void)NULL;
	return b3776_cell(diag, up, left, a, b);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_lcs_cell_u(size_t diag, size_t up, size_t left, uint8_t a,
                       uint8_t b)
    __attribute__((alias("gj_lcs_cell_u")));
