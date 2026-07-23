/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4315: set one indexed bit of a uint32_t to v&1.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_bit_set_to_u(uint32_t x, unsigned i, unsigned v);
 *     - Force bit i of x to (v & 1). Bits outside 0..31 leave x
 *       unchanged (no shift UB). Pure integer; no builtins.
 *   uint32_t __gj_u32_bit_set_to_u  (alias)
 *   __libcgj_batch4315_marker = "libcgj-batch4315"
 *
 * Exclusive continuum CREATE-ONLY (4311-4320). Distinct from
 * gj_u32_set_bit (batch2017), gj_u32_bit_clear_u (batch4311), and
 * gj_u32_set_msb_u (batch4007) — unique gj_u32_bit_set_to_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4315_marker[] = "libcgj-batch4315";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4315_bit_set_to(uint32_t u32X, unsigned uI, unsigned uV)
{
	uint32_t u32Bit;

	if (uI >= 32u) {
		return u32X;
	}
	u32Bit = (uint32_t)1u << uI;
	u32X &= ~u32Bit;
	if ((uV & 1u) != 0u) {
		u32X |= u32Bit;
	}
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_bit_set_to_u - set bit i of a 32-bit word to (v & 1).
 *
 * x: value
 * i: bit index (0 = LSB). If i >= 32, return x unchanged.
 * v: desired bit value; only the least-significant bit is used.
 *
 * Example: gj_u32_bit_set_to_u(0, 0, 1) -> 1;
 *          gj_u32_bit_set_to_u(0xFFu, 0, 0) -> 0xFEu;
 *          gj_u32_bit_set_to_u(0, 31, 3) -> 0x80000000u (v&1 == 1).
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_bit_set_to_u(uint32_t u32X, unsigned uI, unsigned uV)
{
	(void)NULL;
	return b4315_bit_set_to(u32X, uI, uV);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_bit_set_to_u(uint32_t u32X, unsigned uI, unsigned uV)
    __attribute__((alias("gj_u32_bit_set_to_u")));
