/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4314: toggle one indexed bit in a uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_bit_toggle_u(uint64_t x, unsigned i);
 *     - Flip bit i of x (XOR with 1<<i). Bits outside 0..63 leave x
 *       unchanged (no shift UB). Pure integer; no builtins.
 *   uint64_t __gj_u64_bit_toggle_u  (alias)
 *   __libcgj_batch4314_marker = "libcgj-batch4314"
 *
 * Exclusive continuum CREATE-ONLY (4311-4320). Distinct from
 * gj_bit_toggle_u64 (batch654), gj_u64_toggle_bit (batch2013), and
 * gj_u32_bit_toggle_u (batch4313) — unique gj_u64_bit_toggle_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4314_marker[] = "libcgj-batch4314";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4314_bit_toggle(uint64_t u64X, unsigned uI)
{
	if (uI >= 64u) {
		return u64X;
	}
	return u64X ^ ((uint64_t)1u << uI);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_bit_toggle_u - flip bit i of a 64-bit word.
 *
 * x: value
 * i: bit index (0 = LSB). If i >= 64, return x unchanged.
 *
 * Example: gj_u64_bit_toggle_u(0, 0) -> 1;
 *          gj_u64_bit_toggle_u(1, 0) -> 0;
 *          gj_u64_bit_toggle_u(0, 63) -> 0x8000000000000000ull.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_bit_toggle_u(uint64_t u64X, unsigned uI)
{
	(void)NULL;
	return b4314_bit_toggle(u64X, uI);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_bit_toggle_u(uint64_t u64X, unsigned uI)
    __attribute__((alias("gj_u64_bit_toggle_u")));
