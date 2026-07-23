/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3986: kilobyte-align size_t up (1024).
 *
 * Surface (unique symbols):
 *   size_t gj_kb_align_u(size_t x);
 *     - Round x up to the next multiple of 1024 (1 KiB). x already
 *       aligned → x. Overflow of (x + 1023) → 0.
 *   size_t __gj_kb_align_u  (alias)
 *   __libcgj_batch3986_marker = "libcgj-batch3986"
 *
 * Exclusive continuum CREATE-ONLY (3981-3990). Distinct from
 * gj_page_align_u (batch3982) / gj_mb_align_u (batch3987) — unique
 * gj_kb_align_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3986_marker[] = "libcgj-batch3986";

/* One kibibyte stride. */
#define B3986_KB  1024u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3986_kb_align(size_t uX)
{
	size_t uMask = (size_t)B3986_KB - 1u;
	size_t uAdj;

	if (uX > (SIZE_MAX - uMask)) {
		return 0u;
	}
	uAdj = (uX + uMask) & ~uMask;
	return uAdj;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kb_align_u - (x + 1023) & ~1023 with overflow → 0.
 *
 * x: byte count to round up to a 1 KiB boundary. Already KiB-aligned
 * values return unchanged. No parent wires.
 */
size_t
gj_kb_align_u(size_t uX)
{
	(void)NULL;
	return b3986_kb_align(uX);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_kb_align_u(size_t uX)
    __attribute__((alias("gj_kb_align_u")));
