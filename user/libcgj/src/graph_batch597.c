/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch597: binary-reflected Gray code decode (uint64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_gray_decode_u64(uint64_t g);
 *     — Inverse of binary-reflected Gray encode for 64-bit words.
 *       Encode is g = n ^ (n >> 1); decode unfolds the prefix-XOR via
 *       successive right-shift XORs (1, 2, 4, 8, 16, 32). Encode then
 *       decode is identity for all uint64_t.
 *   __gj_gray_decode_u64  (alias)
 *   __libcgj_batch597_marker = "libcgj-batch597"
 *
 * Does NOT redefine gj_gray_encode / gj_gray_decode (batch182, uint32_t)
 * or __gj_gray_seq (batch244). Clean-room freestanding pure C (integer
 * only). Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror. No
 * malloc, no errno, no libc, no compiler builtins. No third-party source
 * copied.
 */

#include <stdint.h>

const char __libcgj_batch597_marker[] = "libcgj-batch597";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gray_decode_u64 — convert binary-reflected Gray code g back to binary.
 *
 * g: Gray-coded 64-bit value
 *
 * Unfolds the prefix-XOR structure of the encode map over 64 bits.
 * Example (low bits match batch182): Gray 0 → 0, Gray 3 → 2, Gray 4 → 7.
 */
uint64_t
gj_gray_decode_u64(uint64_t g)
{
	uint64_t u64V;

	u64V = g;
	u64V ^= u64V >> 1;
	u64V ^= u64V >> 2;
	u64V ^= u64V >> 4;
	u64V ^= u64V >> 8;
	u64V ^= u64V >> 16;
	u64V ^= u64V >> 32;
	return u64V;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_gray_decode_u64(uint64_t g)
    __attribute__((alias("gj_gray_decode_u64")));
