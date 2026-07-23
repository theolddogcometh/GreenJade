/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4501: reverse all 32 bits of a uint32_t word.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_rev_bits_u(uint32_t x);
 *     - Reverse bit order of x (bit 0 <-> bit 31, bit 1 <-> bit 30, ...).
 *       Pure integer SWAR; no builtins, no __int128.
 *   uint32_t __gj_u32_rev_bits_u  (alias)
 *   __libcgj_batch4501_marker = "libcgj-batch4501"
 *
 * Exclusive continuum CREATE-ONLY (4501-4510: u32_rev_bits_u, u64_rev_bits_u,
 * u32_rev_bytes_u, u64_rev_bytes_u, u32_rev_nibbles_u, u32_swizzle_1357_u,
 * u32_pack_bool4_u, u32_unpack_bool_u, u32_mask_merge_u, batch_id_4510).
 * Distinct from gj_bitrev32 (batch181) and gj_bit_reverse32 (batch418) —
 * unique gj_u32_rev_bits_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4501_marker[] = "libcgj-batch4501";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4501_rev_bits(uint32_t u32X)
{
	u32X = ((u32X >> 1) & 0x55555555u) | ((u32X & 0x55555555u) << 1);
	u32X = ((u32X >> 2) & 0x33333333u) | ((u32X & 0x33333333u) << 2);
	u32X = ((u32X >> 4) & 0x0F0F0F0Fu) | ((u32X & 0x0F0F0F0Fu) << 4);
	u32X = ((u32X >> 8) & 0x00FF00FFu) | ((u32X & 0x00FF00FFu) << 8);
	u32X = (u32X >> 16) | (u32X << 16);
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_rev_bits_u - reverse the bit order of a 32-bit word.
 *
 * x: value whose bits are to be reversed
 *
 * Returns x with bits mirrored end-to-end. Example:
 * gj_u32_rev_bits_u(0x80000000u) -> 1u; gj_u32_rev_bits_u(1u) -> 0x80000000u.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_rev_bits_u(uint32_t u32X)
{
	(void)NULL;
	return b4501_rev_bits(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_rev_bits_u(uint32_t u32X)
    __attribute__((alias("gj_u32_rev_bits_u")));
