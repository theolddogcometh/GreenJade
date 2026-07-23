/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2139: freestanding uint32 Gray code to binary.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_gray_to_bin(uint32_t g);
 *     - Convert binary-reflected Gray code g to ordinary binary.
 *       Pure integer SWAR (successive XOR of right-shifted halves).
 *   uint32_t __gj_u32_gray_to_bin  (alias)
 *   __libcgj_batch2139_marker = "libcgj-batch2139"
 *
 * Post-2130 bit reverse exclusive wave (2131-2140). Distinct from
 * gj_gray_decode (batch182) - unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2139_marker[] = "libcgj-batch2139";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Binary-reflected Gray -> binary via parallel prefix XOR of right
 * shifts (1, 2, 4, 8, 16). Inverse of bin-to-gray (n ^ (n >> 1)).
 */
static uint32_t
b2139_gray_to_bin(uint32_t u32G)
{
	uint32_t u32B;

	u32B = u32G;
	u32B ^= u32B >> 1;
	u32B ^= u32B >> 2;
	u32B ^= u32B >> 4;
	u32B ^= u32B >> 8;
	u32B ^= u32B >> 16;
	return u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_gray_to_bin - convert 32-bit binary-reflected Gray code to binary.
 *
 * g: Gray-coded input
 *
 * Example: Gray 0b0011 (3) -> binary 0b0010 (2); Gray 0 -> 0.
 * For any n, gj_u32_gray_to_bin(n ^ (n >> 1)) == n.
 */
uint32_t
gj_u32_gray_to_bin(uint32_t u32G)
{
	(void)NULL;
	return b2139_gray_to_bin(u32G);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_gray_to_bin(uint32_t u32G)
    __attribute__((alias("gj_u32_gray_to_bin")));
