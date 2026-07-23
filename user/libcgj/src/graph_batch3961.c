/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3961: previous binary-reflected Gray code (u64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_gray_prev_u(uint64_t g);
 *     - Interpret g as a binary-reflected Gray code, retreat the
 *       underlying binary rank by one (wrapping at 0 -> 2^64-1), and
 *       return the Gray encoding of that predecessor.
 *       encode(decode(g) - 1).
 *   uint64_t __gj_u64_gray_prev_u  (alias)
 *   __libcgj_batch3961_marker = "libcgj-batch3961"
 *
 * Exclusive continuum CREATE-ONLY (3961-3970). Distinct from
 * gj_gray_dec (batch1015, uint32_t), gj_gray_decode_u64 (batch597),
 * and gj_gray_encode_u64 (batch596) — unique gj_u64_gray_prev_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3961_marker[] = "libcgj-batch3961";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary-reflected Gray encode (same map as batch596; private). */
static uint64_t
b3961_gray_encode(uint64_t u64N)
{
	return u64N ^ (u64N >> 1);
}

/* Binary-reflected Gray decode (same map as batch597; private). */
static uint64_t
b3961_gray_decode(uint64_t u64G)
{
	u64G ^= u64G >> 1;
	u64G ^= u64G >> 2;
	u64G ^= u64G >> 4;
	u64G ^= u64G >> 8;
	u64G ^= u64G >> 16;
	u64G ^= u64G >> 32;
	return u64G;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_gray_prev_u - Gray-code predecessor of g (64-bit).
 *
 * g: current Gray code word
 *
 * Decodes g to binary rank n, returns encode(n - 1) with wraparound.
 * Adjacent Gray codes differ by exactly one bit.
 */
uint64_t
gj_u64_gray_prev_u(uint64_t u64G)
{
	uint64_t u64N;

	(void)NULL;
	u64N = b3961_gray_decode(u64G);
	u64N -= 1ull;
	return b3961_gray_encode(u64N);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_gray_prev_u(uint64_t u64G)
    __attribute__((alias("gj_u64_gray_prev_u")));
