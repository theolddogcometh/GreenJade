/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3778: one LCS rolling-row length step (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_lcs_len_step_u(const size_t *prev, size_t *cur, size_t nb,
 *                            uint8_t ch_a, const uint8_t *b);
 *     - Advance one LCS DP row for character ch_a against b[0..nb).
 *       prev[0..nb] is the previous row (length nb+1); cur[0..nb] is
 *       written (cur[0]=0). Returns cur[nb] = LCS length after this
 *       a-prefix. NULL prev/cur/b with nb>0 → 0. nb==0 → 0 (cur[0]=0
 *       if cur non-NULL).
 *   size_t __gj_lcs_len_step_u  (alias)
 *   __libcgj_batch3778_marker = "libcgj-batch3778"
 *
 * Exclusive continuum CREATE-ONLY (3771-3780). Distinct from
 * gj_lcs_len (batch214) and gj_lcs_cell_u (batch3776 single cell).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3778_marker[] = "libcgj-batch3778";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3778_max2(size_t uA, size_t uB)
{
	return (uA > uB) ? uA : uB;
}

/*
 * One LCS row. Caller guarantees prev/cur/b non-NULL and nb may be 0.
 * prev and cur each have nb+1 slots.
 */
static size_t
b3778_step(const size_t *pPrev, size_t *pCur, size_t uNb, uint8_t u8A,
           const uint8_t *pB)
{
	size_t uJ;

	pCur[0] = 0u;
	for (uJ = 1u; uJ <= uNb; uJ++) {
		if (u8A == pB[uJ - 1u]) {
			pCur[uJ] = pPrev[uJ - 1u] + 1u;
		} else {
			pCur[uJ] = b3778_max2(pPrev[uJ], pCur[uJ - 1u]);
		}
	}
	return pCur[uNb];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lcs_len_step_u — one LCS DP row for ch_a vs b[0..nb).
 *
 * prev: previous row, length nb+1
 * cur:  output row, length nb+1 (written)
 * nb:   length of b
 * ch_a: current a character
 * b:    b string bytes
 *
 * Returns LCS length for this a-prefix vs full b (cur[nb]).
 * No parent wires.
 */
size_t
gj_lcs_len_step_u(const size_t *prev, size_t *cur, size_t nb, uint8_t ch_a,
                  const uint8_t *b)
{
	(void)NULL;
	if (cur == NULL) {
		return 0u;
	}
	if (nb == 0u) {
		cur[0] = 0u;
		return 0u;
	}
	if (prev == NULL || b == NULL) {
		return 0u;
	}
	return b3778_step(prev, cur, nb, ch_a, b);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_lcs_len_step_u(const size_t *prev, size_t *cur, size_t nb,
                           uint8_t ch_a, const uint8_t *b)
    __attribute__((alias("gj_lcs_len_step_u")));
