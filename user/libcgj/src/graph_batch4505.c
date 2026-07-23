/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4505: reverse nibble order of a uint32_t word.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_rev_nibbles_u(uint32_t x);
 *     - Reverse the eight 4-bit nibbles of x (nibble 0 <-> nibble 7, ...).
 *       Pure integer; no builtins.
 *   uint32_t __gj_u32_rev_nibbles_u  (alias)
 *   __libcgj_batch4505_marker = "libcgj-batch4505"
 *
 * Exclusive continuum CREATE-ONLY (4501-4510). Distinct from gj_bswap32
 * (batch416), gj_u32_rev_bytes_u (batch4503), and gj_hex_nibble (batch531)
 * — unique gj_u32_rev_nibbles_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4505_marker[] = "libcgj-batch4505";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4505_rev_nibbles(uint32_t u32X)
{
	/* Swap adjacent nibbles, then 8-bit, then 16-bit groups. */
	u32X = ((u32X & 0x0F0F0F0Fu) << 4) | ((u32X & 0xF0F0F0F0u) >> 4);
	u32X = ((u32X & 0x00FF00FFu) << 8) | ((u32X & 0xFF00FF00u) >> 8);
	u32X = (u32X << 16) | (u32X >> 16);
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_rev_nibbles_u - reverse nibble order within a 32-bit word.
 *
 * x: value whose 4-bit digits are to be reversed end-to-end
 *
 * Returns x with nibble order mirrored. Example:
 * gj_u32_rev_nibbles_u(0x12345678u) -> 0x87654321u.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_rev_nibbles_u(uint32_t u32X)
{
	(void)NULL;
	return b4505_rev_nibbles(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_rev_nibbles_u(uint32_t u32X)
    __attribute__((alias("gj_u32_rev_nibbles_u")));
