/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3777: single Levenshtein DP cell update (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_edit_cell_u(size_t diag, size_t up, size_t left,
 *                         uint8_t a, uint8_t b);
 *     - One edit-distance (Levenshtein) DP cell:
 *         if a == b → diag
 *         else       → 1 + min(diag, up, left)
 *       (substitute / delete / insert). Overflow of 1+min wraps in
 *       size_t (caller keeps cells small).
 *   size_t __gj_edit_cell_u  (alias)
 *   __libcgj_batch3777_marker = "libcgj-batch3777"
 *
 * Exclusive continuum CREATE-ONLY (3771-3780). Distinct from
 * gj_edit_distance (full-string) and gj_lcs_cell_u (batch3776). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3777_marker[] = "libcgj-batch3777";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3777_min2(size_t uA, size_t uB)
{
	return (uA < uB) ? uA : uB;
}

static size_t
b3777_min3(size_t uA, size_t uB, size_t uC)
{
	return b3777_min2(uA, b3777_min2(uB, uC));
}

static size_t
b3777_cell(size_t uDiag, size_t uUp, size_t uLeft, uint8_t u8A, uint8_t u8B)
{
	if (u8A == u8B) {
		return uDiag;
	}
	return 1u + b3777_min3(uDiag, uUp, uLeft);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_edit_cell_u — one Levenshtein DP cell from (diag, up, left) and a,b.
 *
 * diag: DP[i-1][j-1]  (substitute / match)
 * up:   DP[i-1][j]    (delete)
 * left: DP[i][j-1]    (insert)
 * a/b:  characters being compared
 *
 * Returns DP[i][j]. No parent wires.
 */
size_t
gj_edit_cell_u(size_t diag, size_t up, size_t left, uint8_t a, uint8_t b)
{
	(void)NULL;
	return b3777_cell(diag, up, left, a, b);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_edit_cell_u(size_t diag, size_t up, size_t left, uint8_t a,
                        uint8_t b)
    __attribute__((alias("gj_edit_cell_u")));
