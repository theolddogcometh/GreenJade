/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4417: Hamming distance of two Gray code words.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_gray_distance_u(uint32_t a, uint32_t b);
 *     - Hamming weight of (a XOR b). When a and b are binary-reflected
 *       Gray codes, this is the bit distance between those codewords.
 *       Adjacent Gray ranks yield distance 1.
 *   uint32_t __gj_u32_gray_distance_u  (alias)
 *   __libcgj_batch4417_marker = "libcgj-batch4417"
 *
 * Exclusive continuum CREATE-ONLY (4411-4420: gray code unique). Unique
 * gj_u32_gray_distance_u surface only; no multi-def. Distinct from
 * gj_hamming_distance (batch184). Private SWAR popcount; no builtins.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4417_marker[] = "libcgj-batch4417";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR Hamming weight of one uint32_t (Hacker's Delight style). */
static uint32_t
b4417_pop32(uint32_t u32X)
{
	u32X = u32X - ((u32X >> 1) & 0x55555555u);
	u32X = (u32X & 0x33333333u) + ((u32X >> 2) & 0x33333333u);
	u32X = (u32X + (u32X >> 4)) & 0x0f0f0f0fu;
	u32X = u32X + (u32X >> 8);
	u32X = u32X + (u32X >> 16);
	return u32X & 0x3fu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_gray_distance_u - Hamming distance between two Gray code words.
 *
 * a, b: Gray code (or arbitrary) 32-bit words
 *
 * Returns popcount(a ^ b) in [0, 32]. Self-contained SWAR; no parent wires.
 */
uint32_t
gj_u32_gray_distance_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4417_pop32(u32A ^ u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_gray_distance_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_gray_distance_u")));
