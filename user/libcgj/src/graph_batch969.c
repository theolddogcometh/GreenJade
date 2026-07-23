/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch969: boost-style 32-bit hash combine mix.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_combine32(uint32_t seed, uint32_t v);
 *     — Fold v into seed with the classic Boost hash_combine mix:
 *       seed ^ (v + golden + (seed << 6) + (seed >> 2)), using the
 *       32-bit golden-ratio constant 0x9e3779b9. Pure; no mutable state.
 *   uint32_t __gj_hash_combine32  (alias)
 *   __libcgj_batch969_marker = "libcgj-batch969"
 *
 * Distinct from gj_hash_combine_u64 (batch713, 64-bit). Unique
 * gj_hash_combine32 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Algorithm shape matches the
 * well-known Boost hash_combine mix (32-bit golden-ratio fold).
 */

#include <stdint.h>

const char __libcgj_batch969_marker[] = "libcgj-batch969";

/*
 * 32-bit golden-ratio constant (2^32 / phi), the classic Boost
 * hash_combine additive constant.
 */
#define B969_GOLDEN 0x9e3779b9u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_combine32 — boost-style mix of seed with value v.
 *
 * seed: running combined hash
 * v:    32-bit value (or pre-hashed field) to fold in
 *
 * Returns the updated combined hash. Stateless pure function; all
 * inputs including 0 are accepted. Does not allocate or call libc.
 *
 * Mix (boost hash_combine shape):
 *   seed ^ (v + 0x9e3779b9 + (seed << 6) + (seed >> 2))
 */
uint32_t
gj_hash_combine32(uint32_t seed, uint32_t v)
{
	return seed ^ (v + B969_GOLDEN + (seed << 6) + (seed >> 2));
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_combine32(uint32_t seed, uint32_t v)
    __attribute__((alias("gj_hash_combine32")));
