/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch894: Count-Min Sketch clear (zero table).
 *
 * Complements gj_cms_add / gj_cms_est (batch495). Unique symbol.
 *
 * Surface (unique symbols):
 *   void gj_cms_clear(uint32_t *table, size_t depth, size_t width);
 *     — Zero depth * width cells (row-major). No-op on bad args.
 *   void __gj_cms_clear  (alias)
 *   __libcgj_batch894_marker = "libcgj-batch894"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch894_marker[] = "libcgj-batch894";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cms_clear — zero a Count-Min Sketch table.
 *
 * Layout: cell(r, c) = table[r * width + c], r in [0, depth), c in [0, width).
 * NULL table, zero depth, or zero width → no-op.
 */
void
gj_cms_clear(uint32_t *table, size_t depth, size_t width)
{
	size_t n;
	size_t i;

	if (table == NULL || depth == 0u || width == 0u) {
		return;
	}

	/* Guard against size_t overflow of depth * width. */
	if (width > (SIZE_MAX / depth)) {
		return;
	}

	n = depth * width;
	for (i = 0u; i < n; i++) {
		table[i] = 0u;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_cms_clear(uint32_t *table, size_t depth, size_t width)
    __attribute__((alias("gj_cms_clear")));
