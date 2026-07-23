/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4711: seeded 32-bit integer hash (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_u32_seed_u(uint32_t x, uint32_t seed);
 *     - Mix value x under seed into a single 32-bit hash word:
 *       fold x into seed, then a short cascade finalizer. Pure; no
 *       buffer walk.
 *   uint32_t __gj_hash_u32_seed_u  (alias)
 *   __libcgj_batch4711_marker = "libcgj-batch4711"
 *
 * Exclusive continuum CREATE-ONLY (4711-4720: hash_u32_seed_u,
 * hash_u64_seed_u, hash_combine_u, hash_mix_final32_u,
 * hash_mix_final64_u, hash_ptr_u, hash_size_u, hash_pair32_u,
 * hash_triple32_u, batch_id_4720). Unique gj_hash_u32_seed_u surface
 * only; no multi-def. Distinct from gj_hash_seed_mix_u64 (batch1709)
 * and buffer hashes. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4711_marker[] = "libcgj-batch4711";

/* Cascade-unique 32-bit fold / mix constants (odd multipliers). */
#define B4711_GOLD  0x9e3779b1u
#define B4711_M0    0xa24baed5u
#define B4711_M1    0x3d79b3e7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4711_hash_u32_seed(uint32_t u32X, uint32_t u32Seed)
{
	uint32_t u32H;

	u32H = u32Seed ^ (u32X + B4711_GOLD + (u32Seed << 7) +
	    (u32Seed >> 3));
	u32H ^= u32H >> 16;
	u32H *= B4711_M0;
	u32H ^= u32H >> 13;
	u32H *= B4711_M1;
	u32H ^= u32H >> 16;
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_u32_seed_u - hash a uint32_t value under an explicit seed.
 *
 * x:    value to hash
 * seed: salt / running seed
 *
 * Returns a well-mixed 32-bit hash of (x, seed). Stateless pure
 * function; all inputs including 0 are accepted. No parent wires.
 */
uint32_t
gj_hash_u32_seed_u(uint32_t u32X, uint32_t u32Seed)
{
	(void)NULL;
	return b4711_hash_u32_seed(u32X, u32Seed);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_u32_seed_u(uint32_t u32X, uint32_t u32Seed)
    __attribute__((alias("gj_hash_u32_seed_u")));
