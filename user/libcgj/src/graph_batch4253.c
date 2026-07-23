/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4253: 32-bit Hamming distance (XOR popcount).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_hamming_u(uint32_t a, uint32_t b);
 *     - Return popcount(a ^ b): bit-level Hamming distance of two
 *       32-bit words. SWAR pure integer path; no builtins.
 *   uint32_t __gj_u32_hamming_u  (alias)
 *   __libcgj_batch4253_marker = "libcgj-batch4253"
 *
 * Exclusive continuum CREATE-ONLY (4251-4260). Unique gj_u32_hamming_u
 * surface only; no multi-def. Distinct from gj_hamming_distance
 * (batch184) buffer path and gj_hamming_u8 (batch537). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4253_marker[] = "libcgj-batch4253";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR popcount of a 32-bit word (no __builtin dependency). */
static uint32_t
b4253_pop32(uint32_t u32X)
{
	uint32_t u32V;

	u32V = u32X;
	u32V = u32V - ((u32V >> 1) & 0x55555555u);
	u32V = (u32V & 0x33333333u) + ((u32V >> 2) & 0x33333333u);
	u32V = (u32V + (u32V >> 4)) & 0x0f0f0f0fu;
	u32V = u32V + (u32V >> 8);
	u32V = u32V + (u32V >> 16);
	return u32V & 0x3fu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_hamming_u - Hamming distance between two 32-bit words.
 *
 * a, b: words to compare
 *
 * Returns popcount(a ^ b) in [0, 32]. Does not call libc. No parent wires.
 */
uint32_t
gj_u32_hamming_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4253_pop32(u32A ^ u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_hamming_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_hamming_u")));
