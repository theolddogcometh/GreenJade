/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2717: power-of-two align-down uint32 (u-suffix).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_align_down_u(uint32_t x, uint32_t a);
 *     - Round x down to the nearest multiple of a. a must be a power of
 *       two and non-zero. When a == 0 or a is not a power of two,
 *       returns x unchanged.
 *   uint32_t __gj_u32_align_down_u  (alias)
 *   __libcgj_batch2717_marker = "libcgj-batch2717"
 *
 * Post-2710 continuum exclusive wave (2711-2720). Distinct from
 * gj_u32_align_down (batch781) and gj_align_down (batch527) —
 * u-suffix public name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2717_marker[] = "libcgj-batch2717";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * x & ~(a - 1) when a is a non-zero power of two.
 * a == 0 or non-power-of-two → identity (return x).
 */
static uint32_t
b2717_align_down(uint32_t u32X, uint32_t u32A)
{
	uint32_t u32Mask;

	if (u32A == 0u || (u32A & (u32A - 1u)) != 0u) {
		return u32X;
	}

	u32Mask = u32A - 1u;
	return u32X & ~u32Mask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_align_down_u - round x down to power-of-two alignment a.
 *
 * x: value to align
 * a: alignment (must be non-zero power of two; else identity)
 */
uint32_t
gj_u32_align_down_u(uint32_t u32X, uint32_t u32A)
{
	(void)NULL;
	return b2717_align_down(u32X, u32A);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_align_down_u(uint32_t u32X, uint32_t u32A)
    __attribute__((alias("gj_u32_align_down_u")));
