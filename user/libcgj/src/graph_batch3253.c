/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3253: uint32_t overflow-safe rounded average
 * (exclusive avg_round_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_avg_round_u(uint32_t a, uint32_t b);
 *     - Round-half-up of (a + b) / 2 without overflow of a + b.
 *       Halfway cases (odd sum) round up (away from zero / toward +inf).
 *   uint32_t __gj_u32_avg_round_u  (alias)
 *   __libcgj_batch3253_marker = "libcgj-batch3253"
 *
 * CREATE-ONLY exclusive continuum wave (3251-3260). Unique
 * gj_u32_avg_round_u surface only; no multi-def. Distinct from
 * gj_u32_avg_u (batch2643), gj_u32_avg_floor_u, gj_u32_avg_ceil_u
 * (batch3012). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3253_marker[] = "libcgj-batch3253";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Round-half-up average via bit ops; no intermediate sum overflow.
 * floor = (a & b) + ((a ^ b) >> 1); half-up adds the odd-parity bit.
 * Equivalent to floor((a + b + 1) / 2) for all a, b in [0, UINT32_MAX].
 */
static uint32_t
b3253_avg_round(uint32_t u32A, uint32_t u32B)
{
	uint32_t u32Floor;

	u32Floor = (u32A & u32B) + ((u32A ^ u32B) >> 1);
	return u32Floor + ((u32A ^ u32B) & 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_avg_round_u - round-half-up((a + b) / 2) without overflow.
 *
 * a, b: operands
 * When a + b is even the result is exact; when odd, the .5 residual
 * rounds up. Self-contained; no parent wires to floor/ceil avg helpers.
 */
uint32_t
gj_u32_avg_round_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b3253_avg_round(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_avg_round_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_avg_round_u")));
