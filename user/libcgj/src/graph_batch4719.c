/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4719: triple of 32-bit words hash cascade (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_triple32_u(uint32_t a, uint32_t b, uint32_t c);
 *     - Hash the ordered triple (a, b, c) into one 32-bit word with a
 *       cascade multi-fold + finalizer. Pure; no mutable state.
 *   uint32_t __gj_hash_triple32_u  (alias)
 *   __libcgj_batch4719_marker = "libcgj-batch4719"
 *
 * Exclusive continuum CREATE-ONLY (4711-4720). Unique
 * gj_hash_triple32_u surface only; no multi-def. Distinct from
 * gj_hash_combine3_u64 (batch1355), gj_hash_pair32_u (batch4718), and
 * gj_hash_combine32 (batch969). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4719_marker[] = "libcgj-batch4719";

/* Cascade-unique triple mix constants. */
#define B4719_GOLD  0x9e3779b9u
#define B4719_K2    0x85ebca6bu
#define B4719_M0    0xcc9e2d51u
#define B4719_M1    0x1b873593u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4719_hash_triple32(uint32_t u32A, uint32_t u32B, uint32_t u32C)
{
	uint32_t u32H;

	u32H = u32A + B4719_GOLD;
	u32H ^= u32B + (u32H << 6) + (u32H >> 2);
	u32H += B4719_K2;
	u32H ^= u32C + (u32H << 7) + (u32H >> 3);
	u32H ^= u32H >> 16;
	u32H *= B4719_M0;
	u32H ^= u32H >> 13;
	u32H *= B4719_M1;
	u32H ^= u32H >> 16;
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_triple32_u - hash an ordered triple of 32-bit words.
 *
 * a: first word
 * b: second word
 * c: third word
 *
 * Returns a well-mixed 32-bit hash of (a, b, c). Order matters.
 * Stateless pure function. No parent wires.
 */
uint32_t
gj_hash_triple32_u(uint32_t u32A, uint32_t u32B, uint32_t u32C)
{
	(void)NULL;
	return b4719_hash_triple32(u32A, u32B, u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_triple32_u(uint32_t u32A, uint32_t u32B, uint32_t u32C)
    __attribute__((alias("gj_hash_triple32_u")));
