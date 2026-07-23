/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch975: round size up to a 4 KiB page boundary.
 *
 * Surface (unique symbols):
 *   size_t gj_page_round_up(size_t x);
 *     — Round x up to the next multiple of 4096. x already aligned → x.
 *       Overflow of (x + 4095) → 0.
 *   size_t __gj_page_round_up  (alias)
 *   __libcgj_batch975_marker = "libcgj-batch975"
 *
 * Distinct from gj_align_up (batch526, general pow2 a) and
 * gj_u32_align_up (batch779). Unique gj_page_round_up surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch975_marker[] = "libcgj-batch975";

/* Standard x86_64 base page size used by this freestanding helper. */
#define B975_PAGE 4096u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_page_round_up — (x + 4095) & ~4095 with overflow → 0.
 *
 * x: byte count or address-as-size to round up to a page boundary.
 *
 * Already page-aligned values return unchanged.
 */
size_t
gj_page_round_up(size_t x)
{
	size_t uMask = (size_t)B975_PAGE - 1u;
	size_t uAdj;

	if (x > (SIZE_MAX - uMask)) {
		return 0u;
	}
	uAdj = (x + uMask) & ~uMask;
	return uAdj;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_page_round_up(size_t x)
    __attribute__((alias("gj_page_round_up")));
