/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch592: low-bit mask for a uint64_t width.
 *
 * Surface (unique symbols):
 *   uint64_t gj_mask_low_u64(unsigned n);
 *     — Return a mask with the low n bits set (bits [0 .. n-1]).
 *       n == 0  → 0
 *       n >= 64 → all ones (0xFFFFFFFFFFFFFFFF)
 *       else    → (1 << n) - 1  (safe: n in 1..63, no shift-by-width UB)
 *   __gj_mask_low_u64  (alias)
 *   __libcgj_batch592_marker = "libcgj-batch592"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch592_marker[] = "libcgj-batch592";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mask_low_u64 — mask with the lowest n bits set.
 *
 * n: number of low bits to set (0 .. UINT_MAX; clamped at 64)
 *
 * Avoids undefined shift behaviour at width 64 by special-casing n >= 64
 * (all ones) and using (1u64 << n) - 1 only when n < 64 (includes n == 0 → 0).
 */
uint64_t
gj_mask_low_u64(unsigned n)
{
	if (n >= 64u) {
		return ~(uint64_t)0;
	}
	return (((uint64_t)1) << n) - (uint64_t)1;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_mask_low_u64(unsigned n)
    __attribute__((alias("gj_mask_low_u64")));
