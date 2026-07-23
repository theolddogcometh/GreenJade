/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3987: megabyte-align size_t up (1 MiB).
 *
 * Surface (unique symbols):
 *   size_t gj_mb_align_u(size_t x);
 *     - Round x up to the next multiple of 1048576 (1 MiB). x already
 *       aligned → x. Overflow of (x + 1048575) → 0.
 *   size_t __gj_mb_align_u  (alias)
 *   __libcgj_batch3987_marker = "libcgj-batch3987"
 *
 * Exclusive continuum CREATE-ONLY (3981-3990). Distinct from
 * gj_kb_align_u (batch3986) / gj_page_align_u (batch3982) — unique
 * gj_mb_align_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3987_marker[] = "libcgj-batch3987";

/* One mebibyte stride (1024 * 1024). */
#define B3987_MB  1048576u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3987_mb_align(size_t uX)
{
	size_t uMask = (size_t)B3987_MB - 1u;
	size_t uAdj;

	if (uX > (SIZE_MAX - uMask)) {
		return 0u;
	}
	uAdj = (uX + uMask) & ~uMask;
	return uAdj;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mb_align_u - (x + 1048575) & ~1048575 with overflow → 0.
 *
 * x: byte count to round up to a 1 MiB boundary. Already MiB-aligned
 * values return unchanged. No parent wires.
 */
size_t
gj_mb_align_u(size_t uX)
{
	(void)NULL;
	return b3987_mb_align(uX);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_mb_align_u(size_t uX)
    __attribute__((alias("gj_mb_align_u")));
