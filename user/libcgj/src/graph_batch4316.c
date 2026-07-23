/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4316: set one indexed bit of a uint64_t to v&1.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_bit_set_to_u(uint64_t x, unsigned i, unsigned v);
 *     - Force bit i of x to (v & 1). Bits outside 0..63 leave x
 *       unchanged (no shift UB). Pure integer; no builtins.
 *   uint64_t __gj_u64_bit_set_to_u  (alias)
 *   __libcgj_batch4316_marker = "libcgj-batch4316"
 *
 * Exclusive continuum CREATE-ONLY (4311-4320). Distinct from
 * gj_bit_set_u64 (batch651), gj_u64_set_bit (batch2011), and
 * gj_u32_bit_set_to_u (batch4315) — unique gj_u64_bit_set_to_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4316_marker[] = "libcgj-batch4316";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4316_bit_set_to(uint64_t u64X, unsigned uI, unsigned uV)
{
	uint64_t u64Bit;

	if (uI >= 64u) {
		return u64X;
	}
	u64Bit = (uint64_t)1u << uI;
	u64X &= ~u64Bit;
	if ((uV & 1u) != 0u) {
		u64X |= u64Bit;
	}
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_bit_set_to_u - set bit i of a 64-bit word to (v & 1).
 *
 * x: value
 * i: bit index (0 = LSB). If i >= 64, return x unchanged.
 * v: desired bit value; only the least-significant bit is used.
 *
 * Example: gj_u64_bit_set_to_u(0, 0, 1) -> 1;
 *          gj_u64_bit_set_to_u(0xFFull, 0, 0) -> 0xFEull;
 *          gj_u64_bit_set_to_u(0, 63, 3) -> 0x8000000000000000ull.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_bit_set_to_u(uint64_t u64X, unsigned uI, unsigned uV)
{
	(void)NULL;
	return b4316_bit_set_to(u64X, uI, uV);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_bit_set_to_u(uint64_t u64X, unsigned uI, unsigned uV)
    __attribute__((alias("gj_u64_bit_set_to_u")));
