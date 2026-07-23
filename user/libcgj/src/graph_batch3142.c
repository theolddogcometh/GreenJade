/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3142: align address/size up to a 4 KiB page.
 *
 * Surface (unique symbols):
 *   uint64_t gj_align_page_4k_u(uint64_t x);
 *     - Round x up to the next multiple of 4096.
 *       x already aligned → x.
 *       Overflow of (x + 4095) → 0.
 *   uint64_t __gj_align_page_4k_u  (alias)
 *   __libcgj_batch3142_marker = "libcgj-batch3142"
 *
 * Milestone 3150 exclusive continuum CREATE-ONLY (3141-3150). Distinct
 * from gj_page_align_up_4k (batch1771 out-pointer) and gj_page_round_up
 * (batch975 size_t) — unique gj_align_page_4k_u surface only; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3142_marker[] = "libcgj-batch3142";

/* Standard x86_64 base page size (4 KiB). */
#define B3142_PAGE_4K  4096ull

/* ---- freestanding helpers ---------------------------------------------- */

/* (x + 4095) & ~4095; overflow of intermediate → 0. */
static uint64_t
b3142_align_4k(uint64_t u64X)
{
	uint64_t u64Mask;
	uint64_t u64Adj;

	u64Mask = B3142_PAGE_4K - 1ull;
	if (u64X > (UINT64_MAX - u64Mask)) {
		return 0ull;
	}
	u64Adj = (u64X + u64Mask) & ~u64Mask;
	return u64Adj;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_align_page_4k_u - round x up to a 4 KiB page boundary.
 *
 * x: byte address or size to round up.
 *
 * Already 4 KiB-aligned values return unchanged. Intermediate overflow
 * yields 0. No parent wires.
 */
uint64_t
gj_align_page_4k_u(uint64_t u64X)
{
	(void)NULL;
	return b3142_align_4k(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_align_page_4k_u(uint64_t u64X)
    __attribute__((alias("gj_align_page_4k_u")));
