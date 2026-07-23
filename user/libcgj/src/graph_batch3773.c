/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3773: Boyer–Moore bad-character skip (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_boyer_skip_u(size_t j, size_t last_occ);
 *     - Bad-character skip distance for a mismatch at pattern index j
 *       (0-based from the left) given last_occ = rightmost prior index
 *       of the mismatched text byte in the pattern, or (size_t)-1 if
 *       the byte never occurs. Returns max(1, j - last_occ) with
 *       unsigned arithmetic (last_occ >= j → 1). Pure data; no tables.
 *   size_t __gj_boyer_skip_u  (alias)
 *   __libcgj_batch3773_marker = "libcgj-batch3773"
 *
 * Exclusive continuum CREATE-ONLY (3771-3780). Distinct from
 * gj_boyer_moore_badchar_build (batch1223) which fills a 256-slot table;
 * this is the skip arithmetic only. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3773_marker[] = "libcgj-batch3773";

/* ---- freestanding helpers ---------------------------------------------- */

/* max(1, j - last_occ); last_occ == (size_t)-1 → j + 1, then max with 1. */
static size_t
b3773_skip(size_t uJ, size_t uLast)
{
	size_t uDelta;

	if (uLast == (size_t)-1) {
		/* Char absent: shift by j+1 (past the mismatch slot). */
		uDelta = uJ + 1u;
	} else if (uLast >= uJ) {
		/* Occurrence at or right of mismatch → no useful shift. */
		uDelta = 1u;
	} else {
		uDelta = uJ - uLast;
	}
	if (uDelta == 0u) {
		return 1u;
	}
	return uDelta;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_boyer_skip_u — Boyer–Moore bad-character shift distance.
 *
 * j:        pattern index of the mismatch (0-based from left)
 * last_occ: rightmost index of the mismatched text byte in the
 *           pattern, or (size_t)-1 if absent
 *
 * Always returns at least 1. No parent wires.
 */
size_t
gj_boyer_skip_u(size_t j, size_t last_occ)
{
	(void)NULL;
	return b3773_skip(j, last_occ);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_boyer_skip_u(size_t j, size_t last_occ)
    __attribute__((alias("gj_boyer_skip_u")));
