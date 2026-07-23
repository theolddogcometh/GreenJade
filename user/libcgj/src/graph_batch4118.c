/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4118: low-n-bit mask for a uint32_t width.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mask_low_n_u(unsigned n);
 *     - Return a mask with the low n bits set (bits [0 .. n-1]).
 *       n == 0  -> 0
 *       n >= 32 -> all ones (0xFFFFFFFF)  (covers n > 32 and n == 32)
 *       else    -> (1u << n) - 1  (safe: n in 1..31, no shift-by-width UB)
 *   uint32_t __gj_u32_mask_low_n_u  (alias)
 *   __libcgj_batch4118_marker = "libcgj-batch4118"
 *
 * Exclusive continuum CREATE-ONLY (4111-4120). Distinct from
 * gj_u64_mask_low_n (batch2015) — unique gj_u32_mask_low_n_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4118_marker[] = "libcgj-batch4118";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4118_mask_low(unsigned uN)
{
	if (uN >= 32u) {
		return 0xFFFFFFFFu;
	}
	return ((uint32_t)1u << uN) - 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mask_low_n_u - mask with the lowest n bits set (u32 width).
 *
 * n: number of low bits to set (0 .. UINT_MAX; clamped at 32)
 *
 * Avoids undefined shift behaviour at width 32 by special-casing n >= 32
 * (all ones) and using (1u << n) - 1 only when n < 32 (includes n == 0 -> 0).
 */
uint32_t
gj_u32_mask_low_n_u(unsigned uN)
{
	(void)NULL;
	return b4118_mask_low(uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mask_low_n_u(unsigned uN)
    __attribute__((alias("gj_u32_mask_low_n_u")));
