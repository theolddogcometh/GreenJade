/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4415: next binary-reflected Gray code u32
 * (wave-unique rename).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_gray_next_u2(uint32_t x);
 *     - Interpret x as a binary-reflected Gray code, advance the
 *       underlying binary rank by one (wrapping at 2^32), and return the
 *       Gray encoding of that successor. encode(decode(x)+1).
 *       Renamed from gj_u32_gray_next_u (collision with batch3957).
 *   uint32_t __gj_u32_gray_next_u2  (alias)
 *   __libcgj_batch4415_marker = "libcgj-batch4415"
 *
 * Exclusive continuum CREATE-ONLY (4411-4420: gray code unique). Unique
 * gj_u32_gray_next_u2 surface only; no multi-def. Distinct from
 * gj_u32_gray_next_u (batch3957), gj_gray_inc (batch1014).
 * Private helpers only. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4415_marker[] = "libcgj-batch4415";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary-reflected Gray encode (private). */
static uint32_t
b4415_to_gray(uint32_t u32N)
{
	return u32N ^ (u32N >> 1);
}

/* Binary-reflected Gray decode (private). */
static uint32_t
b4415_from_gray(uint32_t u32G)
{
	u32G ^= u32G >> 1;
	u32G ^= u32G >> 2;
	u32G ^= u32G >> 4;
	u32G ^= u32G >> 8;
	u32G ^= u32G >> 16;
	return u32G;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_gray_next_u2 - Gray-code successor of x (uint32_t).
 *
 * x: current Gray code word
 *
 * Decodes x to binary rank n, returns encode(n + 1) with wraparound.
 * Adjacent Gray codes differ by exactly one bit.
 * Wave-unique rename: gj_u32_gray_next_u is owned by batch3957.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_u32_gray_next_u2(uint32_t u32X)
{
	uint32_t uN;

	(void)NULL;
	uN = b4415_from_gray(u32X);
	uN += 1u;
	return b4415_to_gray(uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_gray_next_u2(uint32_t u32X)
    __attribute__((alias("gj_u32_gray_next_u2")));
