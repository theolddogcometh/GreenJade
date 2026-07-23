/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch654: toggle a single bit in a uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_bit_toggle_u64(uint64_t x, unsigned b);
 *     — Flip bit b of x (0 = LSB). If b >= 64, return x unchanged
 *       (no shift UB). Pure integer XOR; no builtins.
 *   uint64_t __gj_bit_toggle_u64  (alias)
 *   __libcgj_batch654_marker = "libcgj-batch654"
 *
 * Does NOT define gj_bit_reverse32 (batch418), gj_bitrev8 (batch560),
 * or other bit-reverse surfaces — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch654_marker[] = "libcgj-batch654";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_toggle_u64 — flip bit b of a 64-bit word.
 *
 * x: value whose bit is to be toggled
 * b: bit index (0 = least significant bit, 63 = MSB)
 *
 * Returns x with bit b inverted via XOR. If b >= 64, returns x
 * unchanged so the shift never exceeds the word width.
 * Example: gj_bit_toggle_u64(0, 0) → 1; gj_bit_toggle_u64(1, 0) → 0.
 */
uint64_t
gj_bit_toggle_u64(uint64_t x, unsigned b)
{
	if (b >= 64u) {
		return x;
	}
	return x ^ ((uint64_t)1ull << b);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bit_toggle_u64(uint64_t x, unsigned b)
    __attribute__((alias("gj_bit_toggle_u64")));
