/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3668: Count-Min Sketch saturating cell
 * increment (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cms_inc_u(uint32_t *cell);
 *     - Saturating +1 on *cell (no wrap past UINT32_MAX). Returns the
 *       new cell value. cell == NULL → 0 (no-op).
 *   uint32_t __gj_cms_inc_u  (alias)
 *   __libcgj_batch3668_marker = "libcgj-batch3668"
 *
 * Exclusive continuum CREATE-ONLY (3661-3670). Companion to
 * gj_cms_index_u (batch3667). Distinct from gj_cms_add (batch495) —
 * unique single-cell inc _u surface only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3668_marker[] = "libcgj-batch3668";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating +1; pCell non-NULL. Returns new value. */
static uint32_t
b3668_inc(uint32_t *pCell)
{
	uint32_t uCur;

	uCur = *pCell;
	if (uCur < 0xffffffffu) {
		uCur += 1u;
		*pCell = uCur;
	}
	return uCur;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cms_inc_u - saturating increment of one CMS counter cell.
 *
 * cell: pointer to a uint32_t counter (may be NULL → 0)
 *
 * Returns the value after the attempt (UINT32_MAX stays UINT32_MAX).
 * Does not call libc. No parent wires.
 */
uint32_t
gj_cms_inc_u(uint32_t *pCell)
{
	(void)NULL;
	if (pCell == NULL) {
		return 0u;
	}
	return b3668_inc(pCell);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cms_inc_u(uint32_t *pCell)
    __attribute__((alias("gj_cms_inc_u")));
