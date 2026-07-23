/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3254: uint64_t overflow-safe rounded average
 * (exclusive avg_round_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_avg_round_u(uint64_t a, uint64_t b);
 *     - Round-half-up of (a + b) / 2 without overflow of a + b.
 *       Halfway cases (odd sum) round up (away from zero / toward +inf).
 *   uint64_t __gj_u64_avg_round_u  (alias)
 *   __libcgj_batch3254_marker = "libcgj-batch3254"
 *
 * CREATE-ONLY exclusive continuum wave (3251-3260). Unique
 * gj_u64_avg_round_u surface only; no multi-def. Distinct from
 * gj_u64_avg_u (batch2644), gj_u64_avg_floor_u (batch3011),
 * gj_u64_avg_ceil_u. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3254_marker[] = "libcgj-batch3254";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Round-half-up average via bit ops; no intermediate sum overflow.
 * floor = (a & b) + ((a ^ b) >> 1); half-up adds the odd-parity bit.
 * Equivalent to floor((a + b + 1) / 2) for all a, b in [0, UINT64_MAX].
 */
static uint64_t
b3254_avg_round(uint64_t u64A, uint64_t u64B)
{
	uint64_t u64Floor;

	u64Floor = (u64A & u64B) + ((u64A ^ u64B) >> 1);
	return u64Floor + ((u64A ^ u64B) & 1ull);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_avg_round_u - round-half-up((a + b) / 2) without overflow.
 *
 * a, b: operands
 * When a + b is even the result is exact; when odd, the .5 residual
 * rounds up. Self-contained; no parent wires to floor/ceil avg helpers.
 */
uint64_t
gj_u64_avg_round_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3254_avg_round(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_avg_round_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_avg_round_u")));
