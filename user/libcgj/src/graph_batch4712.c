/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4712: seeded 64-bit integer hash (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_hash_u64_seed_u(uint64_t x, uint64_t seed);
 *     - Mix value x under seed into a single 64-bit hash word:
 *       fold x into seed, then a short cascade finalizer. Pure; no
 *       buffer walk.
 *   uint64_t __gj_hash_u64_seed_u  (alias)
 *   __libcgj_batch4712_marker = "libcgj-batch4712"
 *
 * Exclusive continuum CREATE-ONLY (4711-4720: hash_u32_seed_u,
 * hash_u64_seed_u, hash_combine_u, hash_mix_final32_u,
 * hash_mix_final64_u, hash_ptr_u, hash_size_u, hash_pair32_u,
 * hash_triple32_u, batch_id_4720). Unique gj_hash_u64_seed_u surface
 * only; no multi-def. Distinct from gj_hash_seed_mix_u64 (batch1709)
 * and gj_hash_combine_u64 (batch713). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4712_marker[] = "libcgj-batch4712";

/* Cascade-unique 64-bit fold / mix constants (odd multipliers). */
#define B4712_GOLD  0x9e3779b97f4a7c15ULL
#define B4712_M0    0xbf58476d1ce4e5b9ULL
#define B4712_M1    0x94d049bb133111ebULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4712_hash_u64_seed(uint64_t u64X, uint64_t u64Seed)
{
	uint64_t u64H;

	u64H = u64Seed ^ (u64X + B4712_GOLD + (u64Seed << 12) +
	    (u64Seed >> 4));
	u64H ^= u64H >> 30;
	u64H *= B4712_M0;
	u64H ^= u64H >> 27;
	u64H *= B4712_M1;
	u64H ^= u64H >> 31;
	return u64H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_u64_seed_u - hash a uint64_t value under an explicit seed.
 *
 * x:    value to hash
 * seed: salt / running seed
 *
 * Returns a well-mixed 64-bit hash of (x, seed). Stateless pure
 * function; all inputs including 0 are accepted. No parent wires.
 */
uint64_t
gj_hash_u64_seed_u(uint64_t u64X, uint64_t u64Seed)
{
	(void)NULL;
	return b4712_hash_u64_seed(u64X, u64Seed);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hash_u64_seed_u(uint64_t u64X, uint64_t u64Seed)
    __attribute__((alias("gj_hash_u64_seed_u")));
