/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4085: ceil unsigned Q48.16 → integer (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_ceil_q16_u(uint64_t q);
 *     - Ceiling of unsigned Q48.16 q as a plain integer.
 *       floor(q) if fraction is zero, else floor(q)+1.
 *       Max result 2^48 when q has a nonzero fraction on the top binade.
 *   uint64_t __gj_u64_ceil_q16_u  (alias)
 *   __libcgj_batch4085_marker = "libcgj-batch4085"
 *
 * Exclusive continuum CREATE-ONLY (4081-4090). Distinct from
 * gj_u32_ceil_q16_u (batch4084) / gj_u64_ceil_div_u (batch3852) —
 * unique gj_u64_ceil_q16_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4085_marker[] = "libcgj-batch4085";

/* Low 16 bits = fractional field of unsigned Q48.16. */
#define B4085_FRAC_MASK  0xFFFFull

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * ceil(q) = floor(q) + (frac != 0). Result always fits in uint64_t
 * (max floor is 2^48-1; +1 yields 2^48).
 */
static uint64_t
b4085_ceil(uint64_t u64Q)
{
	uint64_t u64I;

	u64I = u64Q >> 16;
	if ((u64Q & B4085_FRAC_MASK) != 0ull) {
		u64I += 1ull;
	}
	return u64I;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_ceil_q16_u - ceiling of unsigned Q48.16 q to integer.
 *
 * q: unsigned Q48.16 (bits 63..16 integer, 15..0 fraction; 1.0==0x10000)
 * Returns ceil(q) in [0, 2^48]. No parent wires.
 */
uint64_t
gj_u64_ceil_q16_u(uint64_t u64Q)
{
	(void)NULL;
	return b4085_ceil(u64Q);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_ceil_q16_u(uint64_t u64Q)
    __attribute__((alias("gj_u64_ceil_q16_u")));
