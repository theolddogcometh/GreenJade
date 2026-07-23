/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2015: low-n-bit mask for a uint64_t width.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mask_low_n(unsigned n);
 *     - Return a mask with the low n bits set (bits [0 .. n-1]).
 *       n == 0  -> 0
 *       n >= 64 -> all ones (0xFFFFFFFFFFFFFFFF)
 *       else    -> (1 << n) - 1  (safe: n in 1..63, no shift-by-width UB)
 *   uint64_t __gj_u64_mask_low_n  (alias)
 *   __libcgj_batch2015_marker = "libcgj-batch2015"
 *
 * Post-2000 bit-ops exclusive wave (2011-2020). Distinct from
 * gj_mask_low_u64 (batch592) - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2015_marker[] = "libcgj-batch2015";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2015_mask_low(unsigned uN)
{
	if (uN >= 64u) {
		return ~(uint64_t)0;
	}
	return (((uint64_t)1) << uN) - (uint64_t)1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mask_low_n - mask with the lowest n bits set.
 *
 * n: number of low bits to set (0 .. UINT_MAX; clamped at 64)
 *
 * Avoids undefined shift behaviour at width 64 by special-casing n >= 64
 * (all ones) and using (1u64 << n) - 1 only when n < 64 (includes n == 0 -> 0).
 */
uint64_t
gj_u64_mask_low_n(unsigned uN)
{
	(void)NULL;
	return b2015_mask_low(uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mask_low_n(unsigned uN)
    __attribute__((alias("gj_u64_mask_low_n")));
