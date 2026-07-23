/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1709: 64-bit hash seed mixer.
 *
 * Surface (unique symbols):
 *   uint64_t gj_hash_seed_mix_u64(uint64_t seed, uint64_t x);
 *     — Mix a seed with a 64-bit value using a Boost-style combine:
 *       seed ^= x + GOLDEN + (seed << 6) + (seed >> 2).
 *       GOLDEN is the 64-bit fractional golden-ratio constant
 *       0x9e3779b97f4a7c15ULL.
 *   uint64_t __gj_hash_seed_mix_u64  (alias)
 *   __libcgj_batch1709_marker = "libcgj-batch1709"
 *
 * Distinct from gj_hash_mix64 (batch967 fmix), gj_hash_finalizer64
 * (batch968), gj_hash_combine_u64, and splitmix/wyhash mixers.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1709_marker[] = "libcgj-batch1709";

/* 2^64 / φ fractional part (public odd mixing constant). */
#define B1709_GOLDEN 0x9e3779b97f4a7c15ULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1709_seed_mix(uint64_t u64Seed, uint64_t u64X)
{
	return u64Seed ^ (u64X + B1709_GOLDEN + (u64Seed << 6) +
	    (u64Seed >> 2));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_seed_mix_u64 — combine seed with x into a new seed.
 *
 * u64Seed: current seed / accumulator.
 * u64X:    value to fold in (key fragment, counter, salt, etc.).
 *
 * Returns the mixed seed. All input values accepted, including 0.
 */
uint64_t
gj_hash_seed_mix_u64(uint64_t u64Seed, uint64_t u64X)
{
	return b1709_seed_mix(u64Seed, u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hash_seed_mix_u64(uint64_t u64Seed, uint64_t u64X)
    __attribute__((alias("gj_hash_seed_mix_u64")));
