/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2011: set one bit in a uint64_t word.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_set_bit(uint64_t x, unsigned b);
 *     - Return x with bit b set (b in 0..63, LSB = bit 0). If b >= 64,
 *       return x unchanged (no shift UB). Pure integer OR; no builtins.
 *   uint64_t __gj_u64_set_bit  (alias)
 *   __libcgj_batch2011_marker = "libcgj-batch2011"
 *
 * Post-2000 bit-ops exclusive wave (2011-2020). Distinct from
 * gj_bit_set_u64 (batch651) - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2011_marker[] = "libcgj-batch2011";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2011_set(uint64_t u64X, unsigned uB)
{
	if (uB >= 64u) {
		return u64X;
	}
	return u64X | ((uint64_t)1 << uB);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_set_bit - set bit b of a 64-bit word.
 *
 * x: value to update
 * b: bit index; 0 is LSB, 63 is MSB. b >= 64 -> no change.
 *
 * Returns x | (1ull << b) for b in 0..63; otherwise x.
 */
uint64_t
gj_u64_set_bit(uint64_t u64X, unsigned uB)
{
	(void)NULL;
	return b2011_set(u64X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_set_bit(uint64_t u64X, unsigned uB)
    __attribute__((alias("gj_u64_set_bit")));
