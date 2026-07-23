/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4522: freestanding u8 nibble-swap via LUT (_u).
 *
 * Surface (unique symbols):
 *   uint8_t gj_u8_nibble_swap_lut_u(uint8_t x);
 *     - Exchange high and low 4-bit halves of x using a 16-entry LUT
 *       that maps a nibble to its left-shifted (high) placement, then
 *       OR with the high nibble moved down. E.g. 0xab -> 0xba.
 *   uint8_t __gj_u8_nibble_swap_lut_u  (alias)
 *   __libcgj_batch4522_marker = "libcgj-batch4522"
 *
 * Exclusive continuum CREATE-ONLY (4521-4530: table-lookup unique wave).
 * Unique gj_u8_nibble_swap_lut_u surface only; no multi-def. Distinct
 * from gj_nibble_swap (batch938, arithmetic) and gj_u32_nibble_swap_u
 * (batch3436, per-byte on u32). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4522_marker[] = "libcgj-batch4522";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Map nibble n to (n << 4) for high-half placement. Index 0..15.
 * Low half is just the high nibble of the input (no table needed).
 */
static const uint8_t b4522_nibble_to_hi[16] = {
	0x00u, 0x10u, 0x20u, 0x30u, 0x40u, 0x50u, 0x60u, 0x70u,
	0x80u, 0x90u, 0xa0u, 0xb0u, 0xc0u, 0xd0u, 0xe0u, 0xf0u
};

static uint8_t
b4522_nibble_swap(uint8_t u8X)
{
	uint8_t u8Lo;
	uint8_t u8Hi;

	u8Lo = (uint8_t)(u8X & 0x0fu);
	u8Hi = (uint8_t)((u8X >> 4) & 0x0fu);
	return (uint8_t)(b4522_nibble_to_hi[u8Lo] | u8Hi);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_nibble_swap_lut_u - swap high/low nibbles of a byte via LUT.
 *
 * x: byte to transform
 *
 * Returns (lo << 4) | hi. Example: 0xab -> 0xba; 0x0f -> 0xf0.
 * No parent wires.
 */
uint8_t
gj_u8_nibble_swap_lut_u(uint8_t u8X)
{
	(void)NULL;
	return b4522_nibble_swap(u8X);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_u8_nibble_swap_lut_u(uint8_t u8X)
    __attribute__((alias("gj_u8_nibble_swap_lut_u")));
