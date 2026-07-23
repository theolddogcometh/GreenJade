/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3667: Count-Min Sketch column index (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_cms_index_u(uint64_t h1, uint64_t h2, size_t row,
 *                         size_t width);
 *     - Column for row `row` under Kirsch–Mitzenmacher:
 *         col = (h1 + row * h2) mod width
 *       Returns the flat cell index row * width + col for a row-major
 *       table. width == 0 → 0. row is not range-checked against depth
 *       (caller owns depth bounds).
 *   size_t __gj_cms_index_u  (alias)
 *   __libcgj_batch3667_marker = "libcgj-batch3667"
 *
 * Exclusive continuum CREATE-ONLY (3661-3670). Distinct from
 * gj_cms_add / gj_cms_est (batch495) — unique index _u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3667_marker[] = "libcgj-batch3667";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Flat CMS cell index: row * width + (h1 + row * h2) % width.
 * nWidth must be non-zero (caller-checked).
 */
static size_t
b3667_index(uint64_t h1, uint64_t h2, size_t iRow, size_t nWidth)
{
	uint64_t uCol;

	uCol = h1 + (uint64_t)iRow * h2;
	uCol %= (uint64_t)nWidth;
	return iRow * nWidth + (size_t)uCol;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cms_index_u - flat CMS cell index for (h1, h2) at row under width.
 *
 * h1/h2: double-hash pair (h2 preferably odd)
 * row:   sketch row in [0, depth)
 * width: columns per row; 0 → 0
 *
 * Returns row * width + col. Does not call libc. No parent wires.
 */
size_t
gj_cms_index_u(uint64_t u64H1, uint64_t u64H2, size_t iRow, size_t nWidth)
{
	(void)NULL;
	if (nWidth == 0u) {
		return 0u;
	}
	return b3667_index(u64H1, u64H2, iRow, nWidth);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_cms_index_u(uint64_t u64H1, uint64_t u64H2, size_t iRow,
                        size_t nWidth)
    __attribute__((alias("gj_cms_index_u")));
