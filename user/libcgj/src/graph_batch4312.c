/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4312: clear one indexed bit in a uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_bit_clear_u(uint64_t x, unsigned i);
 *     - Return x with bit i cleared (LSB is bit 0). Bits outside
 *       0..63 leave x unchanged (no shift UB). Pure integer; no
 *       builtins.
 *   uint64_t __gj_u64_bit_clear_u  (alias)
 *   __libcgj_batch4312_marker = "libcgj-batch4312"
 *
 * Exclusive continuum CREATE-ONLY (4311-4320). Distinct from
 * gj_bit_clear_u64 (batch652), gj_u64_clear_bit (batch2012), and
 * gj_u32_bit_clear_u (batch4311) — unique gj_u64_bit_clear_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4312_marker[] = "libcgj-batch4312";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4312_bit_clear(uint64_t u64X, unsigned uI)
{
	if (uI >= 64u) {
		return u64X;
	}
	return u64X & ~((uint64_t)1u << uI);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_bit_clear_u - clear bit i of a 64-bit word.
 *
 * x: value
 * i: bit index (0 = LSB). If i >= 64, return x unchanged.
 *
 * Example: gj_u64_bit_clear_u(0xFFull, 0) -> 0xFEull;
 *          gj_u64_bit_clear_u(0x8000000000000000ull, 63) -> 0.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_bit_clear_u(uint64_t u64X, unsigned uI)
{
	(void)NULL;
	return b4312_bit_clear(u64X, uI);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_bit_clear_u(uint64_t u64X, unsigned uI)
    __attribute__((alias("gj_u64_bit_clear_u")));
