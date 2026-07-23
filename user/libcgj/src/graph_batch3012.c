/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3012: uint32_t overflow-safe ceiling average
 * (exclusive avg_ceil_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_avg_ceil_u(uint32_t a, uint32_t b);
 *     - Ceiling of (a + b) / 2 without overflow of a + b.
 *   uint32_t __gj_u32_avg_ceil_u  (alias)
 *   __libcgj_batch3012_marker = "libcgj-batch3012"
 *
 * Exclusive continuum CREATE-ONLY wave 3011-3020. Distinct from
 * gj_u32_avg_u (batch2643), gj_u32_avg_floor_n (batch2423). Unique
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3012_marker[] = "libcgj-batch3012";

/* ---- freestanding helpers ---------------------------------------------- */

/* Ceiling average via bit ops; no intermediate sum overflow. */
static uint32_t
b3012_avg_ceil(uint32_t u32A, uint32_t u32B)
{
	return (u32A | u32B) - ((u32A ^ u32B) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_avg_ceil_u - ceil((a + b) / 2) without overflow.
 *
 * Identity: (a | b) - ((a ^ b) >> 1)
 *   Equivalent to floor((a + b + 1) / 2) for all a, b in [0, UINT32_MAX].
 *   Bits set in either contribute; half of the unpaired bits is
 *   subtracted (carry-free ceiling of the average).
 */
uint32_t
gj_u32_avg_ceil_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b3012_avg_ceil(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_avg_ceil_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_avg_ceil_u")));
