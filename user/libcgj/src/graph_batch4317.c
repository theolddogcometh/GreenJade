/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4317: keep the lowest n bits of a uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_lowest_n_bits_u(uint32_t x, unsigned n);
 *     - Return x with only the low n bits retained (bits [0 .. n-1]).
 *       n == 0  -> 0
 *       n >= 32 -> x unchanged (full width)
 *       else    -> x & ((1 << n) - 1)  (no shift-by-width UB)
 *   uint32_t __gj_u32_lowest_n_bits_u  (alias)
 *   __libcgj_batch4317_marker = "libcgj-batch4317"
 *
 * Exclusive continuum CREATE-ONLY (4311-4320). Distinct from
 * gj_mask_low_u64 (batch592; mask-only, not applied to x) and
 * gj_u32_highest_n_bits_u (batch4318) — unique gj_u32_lowest_n_bits_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4317_marker[] = "libcgj-batch4317";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4317_lowest_n(uint32_t u32X, unsigned uN)
{
	if (uN == 0u) {
		return 0u;
	}
	if (uN >= 32u) {
		return u32X;
	}
	return u32X & (((uint32_t)1u << uN) - 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_lowest_n_bits_u - keep the lowest n bits of a 32-bit word.
 *
 * x: value to mask
 * n: number of low bits to retain (0..UINT_MAX; clamped at 32)
 *
 * Example: gj_u32_lowest_n_bits_u(0xFFu, 4) -> 0x0Fu;
 *          gj_u32_lowest_n_bits_u(0xABu, 0) -> 0;
 *          gj_u32_lowest_n_bits_u(0xABu, 32) -> 0xABu.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_lowest_n_bits_u(uint32_t u32X, unsigned uN)
{
	(void)NULL;
	return b4317_lowest_n(u32X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_lowest_n_bits_u(uint32_t u32X, unsigned uN)
    __attribute__((alias("gj_u32_lowest_n_bits_u")));
