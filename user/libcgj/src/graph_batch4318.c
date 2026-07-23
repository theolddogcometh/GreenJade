/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4318: keep the highest n bits of a uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_highest_n_bits_u(uint32_t x, unsigned n);
 *     - Return x with only the high n bits retained (bits [32-n .. 31]).
 *       n == 0  -> 0
 *       n >= 32 -> x unchanged (full width)
 *       else    -> x & (~0u << (32 - n))  (no shift-by-width UB)
 *   uint32_t __gj_u32_highest_n_bits_u  (alias)
 *   __libcgj_batch4318_marker = "libcgj-batch4318"
 *
 * Exclusive continuum CREATE-ONLY (4311-4320). Distinct from
 * gj_u32_lowest_n_bits_u (batch4317), gj_mask_high_u64 (batch map
 * surface; mask-only), and gj_u32_clear_msb_u (batch4005) — unique
 * gj_u32_highest_n_bits_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4318_marker[] = "libcgj-batch4318";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4318_highest_n(uint32_t u32X, unsigned uN)
{
	if (uN == 0u) {
		return 0u;
	}
	if (uN >= 32u) {
		return u32X;
	}
	return u32X & (~(uint32_t)0u << (32u - uN));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_highest_n_bits_u - keep the highest n bits of a 32-bit word.
 *
 * x: value to mask
 * n: number of high bits to retain (0..UINT_MAX; clamped at 32)
 *
 * Example: gj_u32_highest_n_bits_u(0xFFu, 4) -> 0 (high 4 of 0x000000FF);
 *          gj_u32_highest_n_bits_u(0xF0000000u, 4) -> 0xF0000000u;
 *          gj_u32_highest_n_bits_u(0xABu, 0) -> 0;
 *          gj_u32_highest_n_bits_u(0xABu, 32) -> 0xABu.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_highest_n_bits_u(uint32_t u32X, unsigned uN)
{
	(void)NULL;
	return b4318_highest_n(u32X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_highest_n_bits_u(uint32_t u32X, unsigned uN)
    __attribute__((alias("gj_u32_highest_n_bits_u")));
