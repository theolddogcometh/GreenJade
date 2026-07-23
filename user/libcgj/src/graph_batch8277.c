/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8277: power-of-two align-down uint32.
 *
 * Surface (unique symbols):
 *   uint32_t gj_align_down_u32_8277(uint32_t v, uint32_t a);
 *     - Round v down to the nearest multiple of a. a must be a power
 *       of two and non-zero. When a == 0 or a is not a power of two,
 *       returns v unchanged.
 *   uint32_t __gj_align_down_u32_8277  (alias)
 *   __libcgj_batch8277_marker = "libcgj-batch8277"
 *
 * Exclusive continuum CREATE-ONLY (8271-8280: div/mod safe integer
 * stubs — div_safe, mod_safe, div_ceil, div_floor, is_pow2, align_up,
 * align_down, round_up_pow2, gcd, batch_id_8280). Unique
 * gj_align_down_u32_8277 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8277_marker[] = "libcgj-batch8277";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * v & ~(a - 1) when a is a non-zero power of two.
 * a == 0 or non-power-of-two → identity (return v).
 */
static uint32_t
b8277_align_down(uint32_t u32V, uint32_t u32A)
{
	uint32_t u32Mask;

	if (u32A == 0u || (u32A & (u32A - 1u)) != 0u) {
		return u32V;
	}

	u32Mask = u32A - 1u;
	return u32V & ~u32Mask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_align_down_u32_8277 - round v down to power-of-two alignment a.
 *
 * v: value to align
 * a: alignment (must be non-zero power of two; else identity)
 * No parent wires.
 */
uint32_t
gj_align_down_u32_8277(uint32_t u32V, uint32_t u32A)
{
	(void)NULL;
	return b8277_align_down(u32V, u32A);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_align_down_u32_8277(uint32_t u32V, uint32_t u32A)
    __attribute__((alias("gj_align_down_u32_8277")));
