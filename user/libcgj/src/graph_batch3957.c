/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3957: next binary-reflected Gray code u32 (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_gray_next_u(uint32_t g);
 *     - Interpret g as a binary-reflected Gray code, advance the
 *       underlying binary rank by one (wrapping at 2^32), and return the
 *       Gray encoding of that successor. encode(decode(g)+1).
 *   uint32_t __gj_u32_gray_next_u  (alias)
 *   __libcgj_batch3957_marker = "libcgj-batch3957"
 *
 * CREATE-ONLY exclusive continuum wave (3951-3960). Unique
 * gj_u32_gray_next_u surface only; no multi-def. Distinct from
 * gj_gray_inc (batch1014), gj_gray_encode / gj_gray_decode (batch182).
 * Private helpers only. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3957_marker[] = "libcgj-batch3957";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary-reflected Gray encode (private; same map as batch182). */
static uint32_t
b3957_gray_encode(uint32_t uN)
{
	return uN ^ (uN >> 1);
}

/* Binary-reflected Gray decode (private; same map as batch182). */
static uint32_t
b3957_gray_decode(uint32_t uG)
{
	uG ^= uG >> 1;
	uG ^= uG >> 2;
	uG ^= uG >> 4;
	uG ^= uG >> 8;
	uG ^= uG >> 16;
	return uG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_gray_next_u - Gray-code successor of g (uint32_t).
 *
 * g: current Gray code word
 *
 * Decodes g to binary rank n, returns encode(n + 1) with wraparound.
 * Adjacent Gray codes differ by exactly one bit.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_u32_gray_next_u(uint32_t u32G)
{
	uint32_t uN;

	(void)NULL;
	uN = b3957_gray_decode(u32G);
	uN += 1u;
	return b3957_gray_encode(uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_gray_next_u(uint32_t u32G)
    __attribute__((alias("gj_u32_gray_next_u")));
