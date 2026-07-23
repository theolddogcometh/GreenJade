/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch968: 64-bit XXH64-style avalanche finalizer.
 *
 * Surface (unique symbols):
 *   uint64_t gj_hash_finalizer64(uint64_t x);
 *     — XXH64 avalanche finalizer of a single 64-bit value: xor-shift /
 *       multiply / xor-shift / multiply / xor-shift. Pure; no buffer walk.
 *   uint64_t __gj_hash_finalizer64  (alias)
 *   __libcgj_batch968_marker = "libcgj-batch968"
 *
 * Distinct from gj_hash_mix64 (batch967 Murmur fmix64), gj_splitmix64
 * (batch424), and file-static XXH avalanches. Unique
 * gj_hash_finalizer64 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Constants match the public XXH64
 * avalanche shape (PRIME64_2 / PRIME64_3).
 */

#include <stdint.h>

const char __libcgj_batch968_marker[] = "libcgj-batch968";

/* XXH64 primes used by the public avalanche finalizer. */
#define B968_PRIME64_2 0xC2B2AE3D27D4EB4FULL
#define B968_PRIME64_3 0x165667B19E3779F9ULL

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_finalizer64 — XXH64-style avalanche finalizer of x.
 *
 * x: arbitrary 64-bit input (all values accepted, including 0)
 *
 * Returns the avalanched 64-bit result. Stateless pure function.
 *
 * Mix (public XXH64 avalanche shape):
 *   x ^= x >> 33; x *= PRIME64_2;
 *   x ^= x >> 29; x *= PRIME64_3;
 *   x ^= x >> 32;
 */
uint64_t
gj_hash_finalizer64(uint64_t x)
{
	uint64_t u64H = x;

	u64H ^= u64H >> 33;
	u64H *= B968_PRIME64_2;
	u64H ^= u64H >> 29;
	u64H *= B968_PRIME64_3;
	u64H ^= u64H >> 32;
	return u64H;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hash_finalizer64(uint64_t x)
    __attribute__((alias("gj_hash_finalizer64")));
