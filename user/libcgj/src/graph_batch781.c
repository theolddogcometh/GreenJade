/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch781: power-of-two alignment round-down (uint32_t).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_align_down(uint32_t x, uint32_t a);
 *     — Round x down to the nearest multiple of a. a must be a power of
 *       two and non-zero. When a == 0 or a is not a power of two,
 *       returns x unchanged.
 *   uint32_t __gj_u32_align_down  (alias)
 *   __libcgj_batch781_marker = "libcgj-batch781"
 *
 * Distinct from gj_align_down (batch527, uint64_t) — no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch781_marker[] = "libcgj-batch781";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_align_down — x & ~(a - 1) when a is a non-zero power of two.
 *
 * a == 0 or non-power-of-two is treated as identity (return x).
 * a == 1 is a power of two and is a no-op (align to 1).
 */
uint32_t
gj_u32_align_down(uint32_t u32X, uint32_t u32A)
{
	uint32_t u32Mask;

	if (u32A == 0u || (u32A & (u32A - 1u)) != 0u) {
		return u32X;
	}

	u32Mask = u32A - 1u;
	return u32X & ~u32Mask;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_align_down(uint32_t u32X, uint32_t u32A)
    __attribute__((alias("gj_u32_align_down")));
