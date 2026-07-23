/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3958: next binary-reflected Gray code u64 (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_gray_next_u(uint64_t g);
 *     - Interpret g as a binary-reflected Gray code, advance the
 *       underlying binary rank by one (wrapping at 2^64), and return the
 *       Gray encoding of that successor. encode(decode(g)+1).
 *   uint64_t __gj_u64_gray_next_u  (alias)
 *   __libcgj_batch3958_marker = "libcgj-batch3958"
 *
 * CREATE-ONLY exclusive continuum wave (3951-3960). Unique
 * gj_u64_gray_next_u surface only; no multi-def. Distinct from
 * gj_u32_gray_next_u (batch3957), gj_gray_inc (batch1014),
 * gj_gray_encode_u64 / gj_gray_decode_u64 (batch596/597).
 * Private helpers only. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3958_marker[] = "libcgj-batch3958";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary-reflected Gray encode u64 (private). */
static uint64_t
b3958_gray_encode(uint64_t uN)
{
	return uN ^ (uN >> 1);
}

/* Binary-reflected Gray decode u64 (private). */
static uint64_t
b3958_gray_decode(uint64_t uG)
{
	uG ^= uG >> 1;
	uG ^= uG >> 2;
	uG ^= uG >> 4;
	uG ^= uG >> 8;
	uG ^= uG >> 16;
	uG ^= uG >> 32;
	return uG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_gray_next_u - Gray-code successor of g (uint64_t).
 *
 * g: current Gray code word
 *
 * Decodes g to binary rank n, returns encode(n + 1) with wraparound.
 * Adjacent Gray codes differ by exactly one bit.
 * Does not call libc. No parent wires.
 */
uint64_t
gj_u64_gray_next_u(uint64_t u64G)
{
	uint64_t uN;

	(void)NULL;
	uN = b3958_gray_decode(u64G);
	uN += 1ull;
	return b3958_gray_encode(uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_gray_next_u(uint64_t u64G)
    __attribute__((alias("gj_u64_gray_next_u")));
