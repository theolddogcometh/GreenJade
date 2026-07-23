/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4313: toggle one indexed bit in a uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_bit_toggle_u(uint32_t x, unsigned i);
 *     - Flip bit i of x (XOR with 1<<i). Bits outside 0..31 leave x
 *       unchanged (no shift UB). Pure integer; no builtins.
 *   uint32_t __gj_u32_bit_toggle_u  (alias)
 *   __libcgj_batch4313_marker = "libcgj-batch4313"
 *
 * Exclusive continuum CREATE-ONLY (4311-4320). Distinct from
 * gj_bit_toggle_u64 (batch654), gj_u32_toggle_msb_u (batch4009), and
 * gj_u32_bit_clear_u (batch4311) — unique gj_u32_bit_toggle_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4313_marker[] = "libcgj-batch4313";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4313_bit_toggle(uint32_t u32X, unsigned uI)
{
	if (uI >= 32u) {
		return u32X;
	}
	return u32X ^ ((uint32_t)1u << uI);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_bit_toggle_u - flip bit i of a 32-bit word.
 *
 * x: value
 * i: bit index (0 = LSB). If i >= 32, return x unchanged.
 *
 * Example: gj_u32_bit_toggle_u(0, 0) -> 1;
 *          gj_u32_bit_toggle_u(1, 0) -> 0;
 *          gj_u32_bit_toggle_u(0, 31) -> 0x80000000u.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_bit_toggle_u(uint32_t u32X, unsigned uI)
{
	(void)NULL;
	return b4313_bit_toggle(u32X, uI);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_bit_toggle_u(uint32_t u32X, unsigned uI)
    __attribute__((alias("gj_u32_bit_toggle_u")));
