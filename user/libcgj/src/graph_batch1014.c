/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1014: next binary-reflected Gray code.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gray_inc(uint32_t g);
 *     — Interpret g as a binary-reflected Gray code, advance the
 *       underlying binary rank by one (wrapping at 2^32), and return the
 *       Gray encoding of that successor. encode(decode(g)+1).
 *   uint32_t __gj_gray_inc  (alias)
 *   __libcgj_batch1014_marker = "libcgj-batch1014"
 *
 * Does NOT redefine gj_gray_encode / gj_gray_decode (batch182) —
 * private helpers only; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1014_marker[] = "libcgj-batch1014";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary-reflected Gray encode (same map as batch182; private). */
static uint32_t
b1014_gray_encode(uint32_t uN)
{
	return uN ^ (uN >> 1);
}

/* Binary-reflected Gray decode (same map as batch182; private). */
static uint32_t
b1014_gray_decode(uint32_t uG)
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
 * gj_gray_inc — Gray-code successor of g.
 *
 * g: current Gray code word
 *
 * Decodes g to binary rank n, returns encode(n + 1) with wraparound.
 * Adjacent Gray codes differ by exactly one bit.
 */
uint32_t
gj_gray_inc(uint32_t g)
{
	uint32_t uN;

	uN = b1014_gray_decode(g);
	uN += 1u;
	return b1014_gray_encode(uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gray_inc(uint32_t g)
    __attribute__((alias("gj_gray_inc")));
