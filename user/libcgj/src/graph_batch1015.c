/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1015: previous binary-reflected Gray code.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gray_dec(uint32_t g);
 *     — Interpret g as a binary-reflected Gray code, retreat the
 *       underlying binary rank by one (wrapping at 0 → 2^32-1), and
 *       return the Gray encoding of that predecessor. encode(decode(g)-1).
 *   uint32_t __gj_gray_dec  (alias)
 *   __libcgj_batch1015_marker = "libcgj-batch1015"
 *
 * Does NOT redefine gj_gray_encode / gj_gray_decode (batch182) or
 * gj_gray_inc (batch1014) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1015_marker[] = "libcgj-batch1015";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary-reflected Gray encode (same map as batch182; private). */
static uint32_t
b1015_gray_encode(uint32_t uN)
{
	return uN ^ (uN >> 1);
}

/* Binary-reflected Gray decode (same map as batch182; private). */
static uint32_t
b1015_gray_decode(uint32_t uG)
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
 * gj_gray_dec — Gray-code predecessor of g.
 *
 * g: current Gray code word
 *
 * Decodes g to binary rank n, returns encode(n - 1) with wraparound.
 */
uint32_t
gj_gray_dec(uint32_t g)
{
	uint32_t uN;

	uN = b1015_gray_decode(g);
	uN -= 1u;
	return b1015_gray_encode(uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gray_dec(uint32_t g)
    __attribute__((alias("gj_gray_dec")));
