/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2726: set a single bit in a uint64_t word (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bit_set_u64_u(uint64_t x, unsigned b);
 *     - Return x with bit b set (b in 0..63, LSB = bit 0). If b >= 64,
 *       return x unchanged (no shift UB). Pure integer OR; no builtins.
 *   uint64_t __gj_bit_set_u64_u  (alias)
 *   __libcgj_batch2726_marker = "libcgj-batch2726"
 *
 * Bitops exclusive wave (2721-2730). Distinct from gj_bit_set_u64
 * (batch651), gj_u64_set_bit (batch2011) — unique gj_bit_set_u64_u
 * surface only; no multi-def. Suggested bare gj_bit_set_u64 is already
 * owned by batch651.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2726_marker[] = "libcgj-batch2726";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2726_set(uint64_t u64X, unsigned uB)
{
	if (uB >= 64u) {
		return u64X;
	}
	return u64X | ((uint64_t)1 << uB);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_set_u64_u - set bit b of a 64-bit word.
 *
 * x: value to update
 * b: bit index; 0 is LSB, 63 is MSB. b >= 64 -> no change.
 *
 * Returns x | (1ull << b) for b in 0..63; otherwise x.
 * Examples: (0, 0) -> 1; (0, 63) -> 0x8000000000000000; (7, 1) -> 7;
 *           (0, 64) -> 0; (0xff, 100) -> 0xff.
 */
uint64_t
gj_bit_set_u64_u(uint64_t u64X, unsigned uB)
{
	(void)NULL;
	return b2726_set(u64X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bit_set_u64_u(uint64_t u64X, unsigned uB)
    __attribute__((alias("gj_bit_set_u64_u")));
