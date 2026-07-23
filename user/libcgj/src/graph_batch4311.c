/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4311: clear one indexed bit in a uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_bit_clear_u(uint32_t x, unsigned i);
 *     - Return x with bit i cleared (LSB is bit 0). Bits outside
 *       0..31 leave x unchanged (no shift UB). Pure integer; no
 *       builtins.
 *   uint32_t __gj_u32_bit_clear_u  (alias)
 *   __libcgj_batch4311_marker = "libcgj-batch4311"
 *
 * Exclusive continuum CREATE-ONLY (4311-4320: u32_bit_clear_u,
 * u64_bit_clear_u, u32_bit_toggle_u, u64_bit_toggle_u, u32_bit_set_to_u,
 * u64_bit_set_to_u, u32_lowest_n_bits_u, u32_highest_n_bits_u,
 * u32_count_leading_ones_u, batch_id_4320). Distinct from
 * gj_u32_clear_bit (batch2018), gj_bit_clear_u64 (batch652), and
 * gj_u32_clear_msb_u (batch4005) — unique gj_u32_bit_clear_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4311_marker[] = "libcgj-batch4311";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4311_bit_clear(uint32_t u32X, unsigned uI)
{
	if (uI >= 32u) {
		return u32X;
	}
	return u32X & ~((uint32_t)1u << uI);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_bit_clear_u - clear bit i of a 32-bit word.
 *
 * x: value
 * i: bit index (0 = LSB). If i >= 32, return x unchanged.
 *
 * Example: gj_u32_bit_clear_u(0xFFu, 0) -> 0xFEu;
 *          gj_u32_bit_clear_u(0x80000000u, 31) -> 0.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_bit_clear_u(uint32_t u32X, unsigned uI)
{
	(void)NULL;
	return b4311_bit_clear(u32X, uI);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_bit_clear_u(uint32_t u32X, unsigned uI)
    __attribute__((alias("gj_u32_bit_clear_u")));
