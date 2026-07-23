/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4708: wrapping (two's-complement) uint32_t negate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_wrapping_neg_u(uint32_t x);
 *     - Two's-complement negate: (uint32_t)(0 - x). Wraps; 0 -> 0,
 *       1 -> UINT32_MAX, UINT32_MAX -> 1.
 *   uint32_t __gj_u32_wrapping_neg_u  (alias)
 *   __libcgj_batch4708_marker = "libcgj-batch4708"
 *
 * Exclusive continuum CREATE-ONLY (4701-4710). Unsigned negate is
 * wrapping (not saturating-to-zero). Unique gj_u32_wrapping_neg_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4708_marker[] = "libcgj-batch4708";

/* ---- freestanding helpers ---------------------------------------------- */

/* Wrapping two's-complement negate in the uint32_t domain. */
static uint32_t
b4708_wrapping_neg(uint32_t u32X)
{
	return (uint32_t)(0u - u32X);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_wrapping_neg_u - unsigned 32-bit wrapping two's-complement negate.
 *
 * x: value to negate
 *
 * Returns 0 - x modulo 2^32. Equivalent to (~x) + 1. Example:
 * gj_u32_wrapping_neg_u(0) -> 0; gj_u32_wrapping_neg_u(1) -> UINT32_MAX.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_wrapping_neg_u(uint32_t u32X)
{
	(void)NULL;
	return b4708_wrapping_neg(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_wrapping_neg_u(uint32_t u32X)
    __attribute__((alias("gj_u32_wrapping_neg_u")));
