/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2012: clear one bit in a uint64_t word.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_clear_bit(uint64_t x, unsigned b);
 *     - Return x with bit b cleared (LSB is bit 0). Bits outside
 *       0..63 leave x unchanged (no shift UB). Pure integer; no
 *       builtins.
 *   uint64_t __gj_u64_clear_bit  (alias)
 *   __libcgj_batch2012_marker = "libcgj-batch2012"
 *
 * Post-2000 bit-ops exclusive wave (2011-2020). Distinct from
 * gj_bit_clear_u64 (batch652) - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2012_marker[] = "libcgj-batch2012";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2012_clear(uint64_t u64X, unsigned uB)
{
	if (uB >= 64u) {
		return u64X;
	}
	return u64X & ~((uint64_t)1 << uB);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_clear_bit - clear bit b of a 64-bit word.
 *
 * x: value
 * b: bit index (0 = LSB). If b >= 64, return x unchanged.
 *
 * Example: gj_u64_clear_bit(0xFFull, 0) -> 0xFEull;
 *          gj_u64_clear_bit(0x8000000000000000ull, 63) -> 0.
 */
uint64_t
gj_u64_clear_bit(uint64_t u64X, unsigned uB)
{
	(void)NULL;
	return b2012_clear(u64X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_clear_bit(uint64_t u64X, unsigned uB)
    __attribute__((alias("gj_u64_clear_bit")));
