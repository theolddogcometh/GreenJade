/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3143: align address/size up to a 2 MiB page.
 *
 * Surface (unique symbols):
 *   uint64_t gj_align_page_2m_u(uint64_t x);
 *     - Round x up to the next multiple of 2 MiB (0x200000).
 *       x already aligned → x.
 *       Overflow of (x + 2MiB - 1) → 0.
 *   uint64_t __gj_align_page_2m_u  (alias)
 *   __libcgj_batch3143_marker = "libcgj-batch3143"
 *
 * Milestone 3150 exclusive continuum CREATE-ONLY (3141-3150). Distinct
 * from gj_page_align_up_2m (batch1773 out-pointer) — unique
 * gj_align_page_2m_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3143_marker[] = "libcgj-batch3143";

/* Standard x86_64 2 MiB large page size. */
#define B3143_PAGE_2M  0x200000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* (x + 2MiB - 1) & ~(2MiB - 1); overflow of intermediate → 0. */
static uint64_t
b3143_align_2m(uint64_t u64X)
{
	uint64_t u64Mask;
	uint64_t u64Adj;

	u64Mask = B3143_PAGE_2M - 1ull;
	if (u64X > (UINT64_MAX - u64Mask)) {
		return 0ull;
	}
	u64Adj = (u64X + u64Mask) & ~u64Mask;
	return u64Adj;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_align_page_2m_u - round x up to a 2 MiB large-page boundary.
 *
 * x: byte address or size to round up.
 *
 * Already 2 MiB-aligned values return unchanged. Intermediate overflow
 * yields 0. No parent wires.
 */
uint64_t
gj_align_page_2m_u(uint64_t u64X)
{
	(void)NULL;
	return b3143_align_2m(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_align_page_2m_u(uint64_t u64X)
    __attribute__((alias("gj_align_page_2m_u")));
