/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4718: pair of 32-bit words hash cascade (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_pair32_u(uint32_t a, uint32_t b);
 *     - Hash the ordered pair (a, b) into one 32-bit word with a
 *       cascade fold + finalizer. Pure; no mutable state.
 *   uint32_t __gj_hash_pair32_u  (alias)
 *   __libcgj_batch4718_marker = "libcgj-batch4718"
 *
 * Exclusive continuum CREATE-ONLY (4711-4720). Unique gj_hash_pair32_u
 * surface only; no multi-def. Distinct from gj_hash_combine32
 * (batch969), gj_hash_combine32_u (batch2053), and
 * gj_string_hash_pair_u (batch3787 buffer). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4718_marker[] = "libcgj-batch4718";

/* Cascade-unique pair mix constants. */
#define B4718_GOLD  0x9e3779b9u
#define B4718_M0    0x85ebca77u
#define B4718_M1    0xc2b2ae3du

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4718_hash_pair32(uint32_t u32A, uint32_t u32B)
{
	uint32_t u32H;

	u32H = u32A + B4718_GOLD;
	u32H ^= u32B + (u32H << 6) + (u32H >> 2);
	u32H ^= u32H >> 16;
	u32H *= B4718_M0;
	u32H ^= u32H >> 13;
	u32H *= B4718_M1;
	u32H ^= u32H >> 16;
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_pair32_u - hash an ordered pair of 32-bit words.
 *
 * a: first word
 * b: second word
 *
 * Returns a well-mixed 32-bit hash of (a, b). Order matters:
 * hash(a,b) is not required to equal hash(b,a). Stateless pure
 * function. No parent wires.
 */
uint32_t
gj_hash_pair32_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4718_hash_pair32(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_pair32_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_hash_pair32_u")));
