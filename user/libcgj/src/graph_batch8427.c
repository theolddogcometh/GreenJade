/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8427: boost-style 32-bit hash combine (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_combine_u_8427(uint32_t seed, uint32_t v);
 *     - Fold v into seed with the classic Boost hash_combine mix:
 *       seed ^ (v + golden + (seed << 6) + (seed >> 2)), using the
 *       32-bit golden-ratio constant 0x9e3779b9. Pure; no mutable state.
 *   uint32_t __gj_hash_combine_u_8427  (alias)
 *   __libcgj_batch8427_marker = "libcgj-batch8427"
 *
 * Hash/fnv-ish integer stubs wave (8421-8430). Distinct from
 * gj_hash_combine32 (batch969) and gj_hash_combine_u64 (batch713) —
 * unique _u_8427 surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8427_marker[] = "libcgj-batch8427";

/*
 * 32-bit golden-ratio constant (2^32 / phi), the classic Boost
 * hash_combine additive constant.
 */
#define B8427_GOLDEN 0x9e3779b9u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8427_combine(uint32_t u32Seed, uint32_t u32V)
{
	return u32Seed ^ (u32V + B8427_GOLDEN + (u32Seed << 6) +
			  (u32Seed >> 2));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_combine_u_8427 - boost-style mix of seed with value v.
 *
 * seed: running combined hash
 * v:    32-bit value (or pre-hashed field) to fold in
 *
 * Returns the updated combined hash. Stateless pure function; all
 * inputs including 0 are accepted.
 *
 * Mix (boost hash_combine shape):
 *   seed ^ (v + 0x9e3779b9 + (seed << 6) + (seed >> 2))
 */
uint32_t
gj_hash_combine_u_8427(uint32_t u32Seed, uint32_t u32V)
{
	(void)NULL;
	return b8427_combine(u32Seed, u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_combine_u_8427(uint32_t u32Seed, uint32_t u32V)
    __attribute__((alias("gj_hash_combine_u_8427")));
