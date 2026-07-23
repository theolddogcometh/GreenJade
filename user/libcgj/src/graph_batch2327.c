/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2327: freestanding 32-bit XXH-style final mix
 * (post-2320 hash exclusive surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_mix_final32(uint32_t x);
 *     - XXH32-style avalanche finalizer of a single 32-bit value:
 *       xor-shift / multiply / xor-shift / multiply / xor-shift. Pure;
 *       no buffer walk.
 *   uint32_t __gj_hash_mix_final32  (alias)
 *   __libcgj_batch2327_marker = "libcgj-batch2327"
 *
 * Post-2320 hash exclusive wave (2321-2330). Distinct from
 * gj_hash_mix32_u (batch2051 Murmur fmix32) - unique
 * gj_hash_mix_final32 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Constants match the public XXH32
 * avalanche shape (PRIME32_2 / PRIME32_3).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2327_marker[] = "libcgj-batch2327";

/* XXH32 primes used by the public avalanche finalizer. */
#define B2327_PRIME32_2 2246822519u
#define B2327_PRIME32_3 3266489917u

/* ---- freestanding helpers ---------------------------------------------- */

/* XXH32-style avalanche of a single 32-bit word. */
static uint32_t
b2327_mix_final32(uint32_t u32X)
{
	uint32_t u32H = u32X;

	u32H ^= u32H >> 15;
	u32H *= B2327_PRIME32_2;
	u32H ^= u32H >> 13;
	u32H *= B2327_PRIME32_3;
	u32H ^= u32H >> 16;
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_mix_final32 - XXH32-style 32-bit finalizer mix of x.
 *
 * x: arbitrary 32-bit input (all values accepted, including 0)
 *
 * Returns the mixed 32-bit output. Stateless pure function.
 */
uint32_t
gj_hash_mix_final32(uint32_t u32X)
{
	(void)NULL;
	return b2327_mix_final32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_mix_final32(uint32_t u32X)
    __attribute__((alias("gj_hash_mix_final32")));
