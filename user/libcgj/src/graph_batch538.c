/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch538: fixed-point Jaccard similarity on two
 * uint64_t bitsets (milli-jaccard).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc calls. Does not rely on __builtin_popcount* (those expand to
 * libgcc/libcalls under -ffreestanding without -mpopcnt). No third-party
 * source was copied.
 *
 * Surface (unique symbols):
 *   uint32_t gj_jaccard_u64(uint64_t a, uint64_t b);
 *     — milli-jaccard similarity in [0, 1000]:
 *         (popcount(a & b) * 1000) / popcount(a | b)
 *       When a | b == 0 (both empty), returns 1000 (empty sets equal).
 *   __gj_jaccard_u64  (alias)
 *   __libcgj_batch538_marker = "libcgj-batch538"
 */

#include <stdint.h>

const char __libcgj_batch538_marker[] = "libcgj-batch538";

/* ---- SWAR scalar popcount ---------------------------------------------- */

/*
 * Hamming weight via parallel bit sums (Hacker's Delight / SWAR).
 * Safe for freestanding -fno-builtin -msse2; no multiplies.
 */
static unsigned
b538_popcount64(uint64_t u64X)
{
	u64X = u64X - ((u64X >> 1) & 0x5555555555555555ull);
	u64X = (u64X & 0x3333333333333333ull) +
	       ((u64X >> 2) & 0x3333333333333333ull);
	u64X = (u64X + (u64X >> 4)) & 0x0f0f0f0f0f0f0f0full;
	u64X = u64X + (u64X >> 8);
	u64X = u64X + (u64X >> 16);
	u64X = u64X + (u64X >> 32);
	return (unsigned)(u64X & 0x7full);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_jaccard_u64 — fixed-point milli-Jaccard of two 64-bit bitsets.
 *
 * Treats a and b as sets of bit indices. Intersection size is
 * popcount(a & b); union size is popcount(a | b). Returns
 * floor(inter * 1000 / union) in [0, 1000]. Empty-vs-empty is 1000.
 */
uint32_t
gj_jaccard_u64(uint64_t a, uint64_t b)
{
	uint64_t u64Union;
	unsigned cUnion;
	unsigned cInter;

	u64Union = a | b;
	if (u64Union == 0ull) {
		return 1000u;
	}
	cUnion = b538_popcount64(u64Union);
	cInter = b538_popcount64(a & b);
	/* cUnion in 1..64; cInter * 1000 fits in 32 bits (max 64000). */
	return (uint32_t)((cInter * 1000u) / cUnion);
}

uint32_t __gj_jaccard_u64(uint64_t a, uint64_t b)
    __attribute__((alias("gj_jaccard_u64")));
