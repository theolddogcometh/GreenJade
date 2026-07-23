/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1356: boost-style 4-way 64-bit hash combine.
 *
 * Surface (unique symbols):
 *   uint64_t gj_hash_combine4_u64(uint64_t a, uint64_t b, uint64_t c,
 *                                 uint64_t d);
 *     — Fold four values with the Boost hash_combine mix chain using
 *       the 64-bit golden-ratio constant 0x9e3779b97f4a7c15.
 *   uint64_t __gj_hash_combine4_u64  (alias)
 *   __libcgj_batch1356_marker = "libcgj-batch1356"
 *
 * Distinct from gj_hash_combine_u64 (batch713), gj_hash_combine32
 * (batch969), and gj_hash_combine3_u64 (batch1355). Unique surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1356_marker[] = "libcgj-batch1356";

#define B1356_GOLDEN 0x9e3779b97f4a7c15ULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1356_mix(uint64_t u64Seed, uint64_t u64V)
{
	return u64Seed ^ (u64V + B1356_GOLDEN + (u64Seed << 6) +
	    (u64Seed >> 2));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_combine4_u64 — combine four 64-bit values into one mix.
 *
 * Starts at u64A, folds u64B, u64C, then u64D. Pure; all inputs accepted.
 */
uint64_t
gj_hash_combine4_u64(uint64_t u64A, uint64_t u64B, uint64_t u64C,
    uint64_t u64D)
{
	uint64_t u64H;

	u64H = u64A;
	u64H = b1356_mix(u64H, u64B);
	u64H = b1356_mix(u64H, u64C);
	u64H = b1356_mix(u64H, u64D);
	return u64H;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hash_combine4_u64(uint64_t u64A, uint64_t u64B, uint64_t u64C,
    uint64_t u64D)
    __attribute__((alias("gj_hash_combine4_u64")));
