/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch182: binary-reflected Gray code encode/decode.
 * Clean-room freestanding pure C — integer only (no SSE required;
 * builds under userspace -msse2). No third-party Gray-code source was copied.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gray_encode(uint32_t n);
 *     — binary-reflected Gray code of n: g = n ^ (n >> 1).
 *   uint32_t gj_gray_decode(uint32_t g);
 *     — inverse: recover binary from Gray via successive XOR of right shifts
 *       (1, 2, 4, 8, 16). Encode then decode is identity for all uint32_t.
 *   __gj_gray_encode / __gj_gray_decode  (aliases)
 *   __libcgj_batch182_marker = "libcgj-batch182"
 */

#include <stdint.h>

const char __libcgj_batch182_marker[] = "libcgj-batch182";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gray_encode — convert binary n to binary-reflected Gray code.
 * Adjacent integers map to codes differing by exactly one bit.
 */
uint32_t
gj_gray_encode(uint32_t n)
{
	return n ^ (n >> 1);
}

uint32_t __gj_gray_encode(uint32_t n)
    __attribute__((alias("gj_gray_encode")));

/*
 * gj_gray_decode — convert binary-reflected Gray code g back to binary.
 * Unfolds the prefix-XOR structure of the encode map over 32 bits.
 */
uint32_t
gj_gray_decode(uint32_t g)
{
	g ^= g >> 1;
	g ^= g >> 2;
	g ^= g >> 4;
	g ^= g >> 8;
	g ^= g >> 16;
	return g;
}

uint32_t __gj_gray_decode(uint32_t g)
    __attribute__((alias("gj_gray_decode")));
