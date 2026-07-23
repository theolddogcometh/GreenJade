/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch713: boost-style 64-bit hash combine mix.
 *
 * Surface (unique symbols):
 *   uint64_t gj_hash_combine_u64(uint64_t seed, uint64_t v);
 *     — Fold v into seed with the classic Boost hash_combine mix:
 *       seed ^ (v + golden + (seed << 6) + (seed >> 2)), using the
 *       64-bit golden-ratio constant 0x9e3779b97f4a7c15. Pure; no
 *       mutable state.
 *   uint64_t __gj_hash_combine_u64  (alias)
 *   __libcgj_batch713_marker = "libcgj-batch713"
 *
 * Distinct from gj_splitmix64 (batch424 pure one-shot),
 * gj_splitmix64_next (batch714 stateful), and buffer hashes
 * (fnv/djb2/sdbm/murmur). Unique gj_hash_combine_u64 names only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Algorithm shape matches the
 * well-known Boost hash_combine mix (golden-ratio additive fold).
 */

#include <stdint.h>

const char __libcgj_batch713_marker[] = "libcgj-batch713";

/*
 * 64-bit golden-ratio constant (2^64 / phi), the natural 64-bit
 * extension of Boost's 0x9e3779b9 used in hash_combine.
 */
#define B713_GOLDEN 0x9e3779b97f4a7c15ULL

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_combine_u64 — boost-style mix of seed with value v.
 *
 * seed: running combined hash
 * v:    64-bit value (or pre-hashed field) to fold in
 *
 * Returns the updated combined hash. Stateless pure function; all
 * inputs including 0 are accepted. Does not allocate or call libc.
 *
 * Mix (boost hash_combine shape):
 *   seed ^ (v + 0x9e3779b97f4a7c15 + (seed << 6) + (seed >> 2))
 */
uint64_t
gj_hash_combine_u64(uint64_t u64Seed, uint64_t u64V)
{
	return u64Seed ^ (u64V + B713_GOLDEN + (u64Seed << 6) +
	    (u64Seed >> 2));
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hash_combine_u64(uint64_t u64Seed, uint64_t u64V)
    __attribute__((alias("gj_hash_combine_u64")));
