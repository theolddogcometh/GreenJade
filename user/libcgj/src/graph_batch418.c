/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch418: reverse all 32 bits of a word.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bit_reverse32(uint32_t x);
 *     — Bit 0 ↔ bit 31, bit 1 ↔ bit 30, … SWAR parallel reverse;
 *       pure integer, no builtins / lookup tables.
 *   uint32_t __gj_bit_reverse32  (alias)
 *   __libcgj_batch418_marker = "libcgj-batch418"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch418_marker[] = "libcgj-batch418";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_reverse32 — reverse bit order of a 32-bit word.
 *
 * x: value whose bits are to be reversed
 *
 * SWAR stages: 1, 2, 4, 8, then 16-bit swap of halves.
 * Example: 0x00000001 → 0x80000000; 0x80000000 → 0x00000001.
 */
uint32_t
gj_bit_reverse32(uint32_t u32X)
{
	uint32_t u32V;

	u32V = u32X;
	u32V = ((u32V >> 1) & 0x55555555u) | ((u32V & 0x55555555u) << 1);
	u32V = ((u32V >> 2) & 0x33333333u) | ((u32V & 0x33333333u) << 2);
	u32V = ((u32V >> 4) & 0x0f0f0f0fu) | ((u32V & 0x0f0f0f0fu) << 4);
	u32V = ((u32V >> 8) & 0x00ff00ffu) | ((u32V & 0x00ff00ffu) << 8);
	u32V = (u32V >> 16) | (u32V << 16);
	return u32V;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bit_reverse32(uint32_t u32X)
    __attribute__((alias("gj_bit_reverse32")));
