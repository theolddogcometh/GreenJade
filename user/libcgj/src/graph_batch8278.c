/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8278: uint32_t round up to power-of-two.
 *
 * Surface (unique symbols):
 *   uint32_t gj_round_up_pow2_u32_8278(uint32_t n);
 *     - Smallest power of two >= n.
 *       n == 0 -> 1.
 *       If n is already a power of two, return n.
 *       If result would exceed 2^31 (n > 0x80000000), return 0
 *       (overflow: 2^32 not representable in uint32_t).
 *   uint32_t __gj_round_up_pow2_u32_8278  (alias)
 *   __libcgj_batch8278_marker = "libcgj-batch8278"
 *
 * Exclusive continuum CREATE-ONLY (8271-8280: div/mod safe integer
 * stubs — div_safe, mod_safe, div_ceil, div_floor, is_pow2, align_up,
 * align_down, round_up_pow2, gcd, batch_id_8280). Unique
 * gj_round_up_pow2_u32_8278 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8278_marker[] = "libcgj-batch8278";

/* Highest representable power of two in uint32_t. */
#define B8278_POW2_MAX 0x80000000u

/* ---- freestanding helpers ---------------------------------------------- */

/* Smear bits after (n-1); 0 on overflow past 2^31. */
static uint32_t
b8278_round_up(uint32_t u32N)
{
	uint32_t u32V;

	if (u32N == 0u) {
		return 1u;
	}
	if (u32N > B8278_POW2_MAX) {
		return 0u;
	}
	u32V = u32N - 1u;
	u32V |= u32V >> 1;
	u32V |= u32V >> 2;
	u32V |= u32V >> 4;
	u32V |= u32V >> 8;
	u32V |= u32V >> 16;
	u32V += 1u;
	return u32V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_round_up_pow2_u32_8278 - ceil n to the next power of two.
 *
 * n: value to round up
 *
 * Returns 1 for n==0; n when n is already 2^k; 0 when the next power
 * would be 2^32 (not representable). Does not call libc.
 * No parent wires.
 */
uint32_t
gj_round_up_pow2_u32_8278(uint32_t u32N)
{
	(void)NULL;
	return b8278_round_up(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_round_up_pow2_u32_8278(uint32_t u32N)
    __attribute__((alias("gj_round_up_pow2_u32_8278")));
