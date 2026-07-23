/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2016: high-n-bit mask for a uint64_t width.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mask_high_n(unsigned n);
 *     - Return a mask with the high n bits set (bits [64-n .. 63]).
 *       n == 0  -> 0
 *       n >= 64 -> all ones (0xFFFFFFFFFFFFFFFF)
 *       else    -> ~((1 << (64-n)) - 1)  (safe: no shift-by-width UB)
 *   uint64_t __gj_u64_mask_high_n  (alias)
 *   __libcgj_batch2016_marker = "libcgj-batch2016"
 *
 * Post-2000 bit-ops exclusive wave (2011-2020). Complements
 * gj_u64_mask_low_n (batch2015). Unique public name - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2016_marker[] = "libcgj-batch2016";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2016_mask_high(unsigned uN)
{
	unsigned uLow;

	if (uN == 0u) {
		return (uint64_t)0;
	}
	if (uN >= 64u) {
		return ~(uint64_t)0;
	}
	/* Low (64 - n) bits clear; high n bits set. */
	uLow = 64u - uN;
	return ~((((uint64_t)1) << uLow) - (uint64_t)1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mask_high_n - mask with the highest n bits set.
 *
 * n: number of high bits to set (0 .. UINT_MAX; clamped at 64)
 *
 * Examples: n=0 -> 0; n=1 -> 0x8000000000000000; n=8 -> 0xFF00000000000000;
 *           n=64 -> all ones; n=100 -> all ones.
 */
uint64_t
gj_u64_mask_high_n(unsigned uN)
{
	(void)NULL;
	return b2016_mask_high(uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mask_high_n(unsigned uN)
    __attribute__((alias("gj_u64_mask_high_n")));
