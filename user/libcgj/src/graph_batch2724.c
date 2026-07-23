/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2724: flip a single bit in a uint32_t word.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bit_flip_u32(uint32_t x, unsigned b);
 *     - Flip bit b of x (0 = LSB). If b >= 32, return x unchanged
 *       (no shift UB). Pure integer XOR; no builtins.
 *   uint32_t __gj_bit_flip_u32  (alias)
 *   __libcgj_batch2724_marker = "libcgj-batch2724"
 *
 * Bitops exclusive wave (2721-2730). Distinct from gj_bit_toggle_u64
 * (batch654), gj_u64_toggle_bit (batch2013) — unique gj_bit_flip_u32
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2724_marker[] = "libcgj-batch2724";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2724_flip(uint32_t u32X, unsigned uB)
{
	if (uB >= 32u) {
		return u32X;
	}
	return u32X ^ ((uint32_t)1u << uB);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_flip_u32 - flip bit b of a 32-bit word.
 *
 * x: value whose bit is to be toggled
 * b: bit index (0 = least significant bit, 31 = MSB)
 *
 * Returns x with bit b inverted via XOR. If b >= 32, returns x
 * unchanged so the shift never exceeds the word width.
 * Example: gj_bit_flip_u32(0, 0) -> 1; gj_bit_flip_u32(1, 0) -> 0.
 */
uint32_t
gj_bit_flip_u32(uint32_t u32X, unsigned uB)
{
	(void)NULL;
	return b2724_flip(u32X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bit_flip_u32(uint32_t u32X, unsigned uB)
    __attribute__((alias("gj_bit_flip_u32")));
