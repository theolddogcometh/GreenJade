/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4713: combine two 64-bit hash words (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_hash_combine_u(uint64_t h1, uint64_t h2);
 *     - Fold h2 into h1 with a cascade combine: golden additive fold
 *       followed by a short post-mix. Pure; no mutable state.
 *   uint64_t __gj_hash_combine_u  (alias)
 *   __libcgj_batch4713_marker = "libcgj-batch4713"
 *
 * Exclusive continuum CREATE-ONLY (4711-4720). Unique gj_hash_combine_u
 * surface only; no multi-def. Distinct from gj_hash_combine_u64
 * (batch713 Boost, no post-mix), gj_hash_combine32 (batch969), and
 * gj_entropy_combine64_u (batch4624). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4713_marker[] = "libcgj-batch4713";

/* Cascade-unique combine constants. */
#define B4713_GOLD  0x9e3779b97f4a7c15ULL
#define B4713_M0    0xd6e8feb86659fd93ULL
#define B4713_M1    0xc2b2ae3d27d4eb4fULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4713_combine(uint64_t u64H1, uint64_t u64H2)
{
	uint64_t u64T;

	u64T = u64H1 ^ (u64H2 + B4713_GOLD + (u64H1 << 6) +
	    (u64H1 >> 2));
	u64T ^= u64T >> 32;
	u64T *= B4713_M0;
	u64T ^= u64T >> 29;
	u64T *= B4713_M1;
	u64T ^= u64T >> 32;
	return u64T;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_combine_u - fold two 64-bit hash words into one mixed result.
 *
 * h1: first / running hash
 * h2: second hash (or pre-mixed field) to fold in
 *
 * Returns the combined 64-bit hash. Stateless pure function.
 * No parent wires.
 */
uint64_t
gj_hash_combine_u(uint64_t u64H1, uint64_t u64H2)
{
	(void)NULL;
	return b4713_combine(u64H1, u64H2);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hash_combine_u(uint64_t u64H1, uint64_t u64H2)
    __attribute__((alias("gj_hash_combine_u")));
