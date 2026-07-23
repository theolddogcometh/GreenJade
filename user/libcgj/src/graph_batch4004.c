/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4004: extract the MSB / sign bit of a uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_signbit_u(uint64_t x);
 *     - Return 1 if bit 63 (MSB) of x is set, else 0. Treats the word
 *       as a bit pattern (two's-complement / IEEE sign position).
 *   uint64_t __gj_u64_signbit_u  (alias)
 *   __libcgj_batch4004_marker = "libcgj-batch4004"
 *
 * Exclusive continuum CREATE-ONLY (4001-4010). Distinct from
 * gj_u32_signbit_u (batch4003) and gj_f32_bits_sign (batch1834) —
 * unique gj_u64_signbit_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4004_marker[] = "libcgj-batch4004";

/* uint64_t MSB / two's-complement sign position. */
#define B4004_MSB  ((uint64_t)0x8000000000000000ULL)

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4004_signbit(uint64_t u64X)
{
	if ((u64X & B4004_MSB) != 0ULL) {
		return 1ULL;
	}
	return 0ULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_signbit_u - 1 if bit 63 of x is set, else 0.
 *
 * x: 64-bit word (bit pattern)
 *
 * Returns 1 when the MSB is set, else 0. Does not call libc.
 * No parent wires.
 */
uint64_t
gj_u64_signbit_u(uint64_t u64X)
{
	(void)NULL;
	return b4004_signbit(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_signbit_u(uint64_t u64X)
    __attribute__((alias("gj_u64_signbit_u")));
