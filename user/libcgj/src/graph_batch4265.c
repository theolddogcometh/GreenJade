/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4265: freestanding 4 KiB page ceil (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_page_ceil_u(uint64_t x);
 *     - Align x up to a 4096-byte page boundary:
 *       (x + 4095) & ~4095. Returns 0 on overflow of the intermediate.
 *   uint64_t __gj_u64_page_ceil_u  (alias)
 *   __libcgj_batch4265_marker = "libcgj-batch4265"
 *
 * Exclusive continuum CREATE-ONLY (4261-4270): align/page wave.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4265_marker[] = "libcgj-batch4265";

#define B4265_PAGE_4K      4096ull
#define B4265_PAGE_MASK    (B4265_PAGE_4K - 1ull)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Ceil to 4 KiB page. Overflow of (x + 4095) → 0.
 */
static uint64_t
b4265_page_ceil(uint64_t u64X)
{
	if (u64X > (UINT64_MAX - B4265_PAGE_MASK)) {
		return 0ull;
	}
	return (u64X + B4265_PAGE_MASK) & ~B4265_PAGE_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_page_ceil_u - align x up to a 4096-byte page.
 *
 * x: address or size-like value
 * Returns next page boundary, or 0 on overflow. No parent wires.
 */
uint64_t
gj_u64_page_ceil_u(uint64_t u64X)
{
	(void)NULL;
	return b4265_page_ceil(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_page_ceil_u(uint64_t u64X)
    __attribute__((alias("gj_u64_page_ceil_u")));
