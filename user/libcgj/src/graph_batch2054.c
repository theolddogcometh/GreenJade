/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2054: 64-bit multiply-xor hash combine (_u wave).
 *
 * Surface (unique symbols):
 *   uint64_t gj_hash_combine64_u(uint64_t seed, uint64_t v);
 *     - Fold v into seed with xor, golden multiply, and rotl27-style
 *       diffusion. Distinct from Boost hash_combine_u64. Pure; no
 *       mutable state.
 *   uint64_t __gj_hash_combine64_u  (alias)
 *   __libcgj_batch2054_marker = "libcgj-batch2054"
 *
 * Post-2050 hash-mix exclusive wave (2051-2060). Distinct from
 * gj_hash_combine_u64 (batch713 Boost), gj_hash_combine32 (batch969),
 * gj_hash_seed_mix_u64 (batch1709), and multi-way combine3/4. Unique
 * gj_hash_combine64_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2054_marker[] = "libcgj-batch2054";

/* 64-bit golden-ratio odd constant and combine tail. */
#define B2054_GOLDEN 0x9e3779b97f4a7c15ULL
#define B2054_TAIL   0x85ebca77c2b2ae63ULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2054_rotl(uint64_t u64X, unsigned uN)
{
	return (u64X << uN) | (u64X >> (64u - uN));
}

static uint64_t
b2054_combine(uint64_t u64Seed, uint64_t u64V)
{
	uint64_t u64H = u64Seed;

	u64H ^= u64V * B2054_GOLDEN;
	u64H = b2054_rotl(u64H, 27u);
	u64H = u64H * 5ULL + B2054_TAIL;
	return u64H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_combine64_u - multiply-xor mix of seed with value v.
 *
 * seed: running combined hash
 * v:    64-bit value (or pre-hashed field) to fold in
 *
 * Returns the updated combined hash. Stateless pure function; all
 * inputs including 0 are accepted. Does not allocate or call libc.
 */
uint64_t
gj_hash_combine64_u(uint64_t u64Seed, uint64_t u64V)
{
	(void)NULL;
	return b2054_combine(u64Seed, u64V);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hash_combine64_u(uint64_t u64Seed, uint64_t u64V)
    __attribute__((alias("gj_hash_combine64_u")));
