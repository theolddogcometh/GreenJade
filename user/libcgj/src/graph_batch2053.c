/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2053: 32-bit Murmur-style hash combine (_u wave).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_combine32_u(uint32_t seed, uint32_t v);
 *     - Fold v into seed with a MurmurHash3 x86_32 body-step shape
 *       (c1/c2 multiply, rotl15, rotl13, mul5+k). Distinct from Boost
 *       hash_combine. Pure; no mutable state.
 *   uint32_t __gj_hash_combine32_u  (alias)
 *   __libcgj_batch2053_marker = "libcgj-batch2053"
 *
 * Post-2050 hash-mix exclusive wave (2051-2060). Distinct from
 * gj_hash_combine32 (batch969 Boost) and gj_hash_combine_u64 (batch713).
 * Unique gj_hash_combine32_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2053_marker[] = "libcgj-batch2053";

/* MurmurHash3 x86_32 body-step constants (public algorithm). */
#define B2053_C1 0xcc9e2d51u
#define B2053_C2 0x1b873593u
#define B2053_N  0xe6546b64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2053_rotl(uint32_t u32X, unsigned uN)
{
	return (u32X << uN) | (u32X >> (32u - uN));
}

static uint32_t
b2053_combine(uint32_t u32Seed, uint32_t u32V)
{
	uint32_t u32K = u32V;

	u32K *= B2053_C1;
	u32K = b2053_rotl(u32K, 15u);
	u32K *= B2053_C2;
	u32Seed ^= u32K;
	u32Seed = b2053_rotl(u32Seed, 13u);
	u32Seed = u32Seed * 5u + B2053_N;
	return u32Seed;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_combine32_u - Murmur-style mix of seed with value v.
 *
 * seed: running combined hash
 * v:    32-bit value (or pre-hashed field) to fold in
 *
 * Returns the updated combined hash. Stateless pure function; all
 * inputs including 0 are accepted. Does not allocate or call libc.
 */
uint32_t
gj_hash_combine32_u(uint32_t u32Seed, uint32_t u32V)
{
	(void)NULL;
	return b2053_combine(u32Seed, u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_combine32_u(uint32_t u32Seed, uint32_t u32V)
    __attribute__((alias("gj_hash_combine32_u")));
