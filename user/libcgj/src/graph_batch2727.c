/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2727: clear one bit in a uint64_t word.
 *
 * Surface (unique symbols):
 *   uint64_t gj_bit_clr_u64(uint64_t x, unsigned b);
 *     - Return x with bit b cleared (LSB is bit 0). Bits outside
 *       0..63 leave x unchanged (no shift UB). Pure integer; no
 *       builtins.
 *   uint64_t __gj_bit_clr_u64  (alias)
 *   __libcgj_batch2727_marker = "libcgj-batch2727"
 *
 * Bitops exclusive wave (2721-2730). Distinct from gj_bit_clear_u64
 * (batch652), gj_u64_clear_bit (batch2012) — unique gj_bit_clr_u64
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2727_marker[] = "libcgj-batch2727";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2727_clr(uint64_t u64X, unsigned uB)
{
	if (uB >= 64u) {
		return u64X;
	}
	return u64X & ~((uint64_t)1u << uB);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_clr_u64 - clear bit b of a 64-bit word.
 *
 * x: value
 * b: bit index (0 = LSB). If b >= 64, return x unchanged.
 *
 * Example: gj_bit_clr_u64(0xFFull, 0) -> 0xFEull;
 *          gj_bit_clr_u64(0x8000000000000000ull, 63) -> 0.
 */
uint64_t
gj_bit_clr_u64(uint64_t u64X, unsigned uB)
{
	(void)NULL;
	return b2727_clr(u64X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bit_clr_u64(uint64_t u64X, unsigned uB)
    __attribute__((alias("gj_bit_clr_u64")));
