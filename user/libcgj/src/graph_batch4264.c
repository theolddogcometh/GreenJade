/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4264: freestanding 4 KiB page floor (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_page_floor_u(uint64_t x);
 *     - Align x down to a 4096-byte page boundary: x & ~4095.
 *   uint64_t __gj_u64_page_floor_u  (alias)
 *   __libcgj_batch4264_marker = "libcgj-batch4264"
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

const char __libcgj_batch4264_marker[] = "libcgj-batch4264";

#define B4264_PAGE_4K      4096ull
#define B4264_PAGE_MASK    (B4264_PAGE_4K - 1ull)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Floor to 4 KiB page: clear low 12 bits.
 */
static uint64_t
b4264_page_floor(uint64_t u64X)
{
	return u64X & ~B4264_PAGE_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_page_floor_u - align x down to a 4096-byte page.
 *
 * x: address or size-like value
 * Returns x with low 12 bits cleared. No parent wires.
 */
uint64_t
gj_u64_page_floor_u(uint64_t u64X)
{
	(void)NULL;
	return b4264_page_floor(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_page_floor_u(uint64_t u64X)
    __attribute__((alias("gj_u64_page_floor_u")));
