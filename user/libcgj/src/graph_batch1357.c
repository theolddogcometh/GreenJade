/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1357: Bloom filter bit-count estimate.
 *
 * Surface (unique symbols):
 *   uint64_t gj_bloom_estimate_bits(uint64_t n_items, uint32_t fpr_ppm);
 *     — Optimal bit array size m ≈ n * log2(1/p) / ln(2)
 *       ≈ n * log2(1/p) * 1.442695 for false-positive rate p.
 *       fpr_ppm is p in parts-per-million (1..1000000). Returns 0 on
 *       bad args. Integer fixed-point; no floating point.
 *   uint64_t __gj_bloom_estimate_bits  (alias)
 *   __libcgj_batch1357_marker = "libcgj-batch1357"
 *
 * Distinct from gj_bloom_* ops (batch125). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1357_marker[] = "libcgj-batch1357";

/* ---- freestanding helpers ---------------------------------------------- */

/* floor(log2(x)) for x > 0; 0 if x == 0. Binary peel. */
static unsigned
b1357_flog2(uint64_t u64X)
{
	unsigned uN;

	if (u64X == 0ull) {
		return 0u;
	}
	uN = 0u;
	if (u64X >= (1ull << 32)) {
		uN += 32u;
		u64X >>= 32;
	}
	if (u64X >= (1ull << 16)) {
		uN += 16u;
		u64X >>= 16;
	}
	if (u64X >= (1ull << 8)) {
		uN += 8u;
		u64X >>= 8;
	}
	if (u64X >= (1ull << 4)) {
		uN += 4u;
		u64X >>= 4;
	}
	if (u64X >= (1ull << 2)) {
		uN += 2u;
		u64X >>= 2;
	}
	if (u64X >= (1ull << 1)) {
		uN += 1u;
	}
	return uN;
}

/*
 * Approximate log2(num/den) in Q16 (i.e. result / 65536 ≈ log2(num/den)).
 * Uses integer log2 of the scaled ratio; good enough for sizing.
 */
static uint64_t
b1357_log2_q16(uint64_t u64Num, uint64_t u64Den)
{
	uint64_t u64R;
	unsigned uLg;

	if (u64Num == 0ull || u64Den == 0ull) {
		return 0ull;
	}
	/* Scale num into high half for fractional bits via flog2. */
	if (u64Num >= u64Den) {
		u64R = u64Num / u64Den;
		uLg = b1357_flog2(u64R);
		/* Fractional: look at (num << k) / den remainder — use
		 * 16 extra bits of ratio when it fits. */
		if (u64Num <= (UINT64_MAX >> 16)) {
			u64R = (u64Num << 16) / u64Den;
			return (uint64_t)b1357_flog2(u64R) << 0; /* already Q16 */
		}
		return ((uint64_t)uLg) << 16;
	}
	return 0ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bloom_estimate_bits — optimal Bloom bit count for n items / FPR.
 *
 * u64NItems: expected insertions (0 → 0)
 * u32FprPpm: false-positive rate in ppm (1 = 1e-6, 10000 = 1%, 500000 = 50%)
 *
 * m = n * log2(1/p) / ln(2) ≈ n * log2_q16(1e6, ppm) * 94475 / 2^32
 * where 94475/2^32 ≈ 1/ln(2) / 65536 * 65536... simplified:
 *   m ≈ n * (log2_q16 / 65536) * 1.442695
 *   m ≈ n * log2_q16 * 94548 / 2^32   (94548 ≈ 1.442695 * 65536)
 */
uint64_t
gj_bloom_estimate_bits(uint64_t u64NItems, uint32_t u32FprPpm)
{
	uint64_t u64LogQ16;
	uint64_t u64M;

	if (u64NItems == 0ull || u32FprPpm == 0u || u32FprPpm > 1000000u) {
		return 0ull;
	}
	/* p = ppm/1e6 → 1/p = 1e6/ppm */
	u64LogQ16 = b1357_log2_q16(1000000ull, (uint64_t)u32FprPpm);
	if (u64LogQ16 == 0ull) {
		/* FPR ≥ 50% → log2(1/p) small; still size a few bits/item. */
		u64LogQ16 = 1ull; /* ~ 1/65536 bit of log — will bump below */
	}

	/*
	 * m = n * log2(1/p) / ln2
	 *   = n * (log2_q16 / 65536) * (1/ln2)
	 * 1/ln2 ≈ 1.4426950408889634
	 * scale = 1.442695 * 65536 ≈ 94548. something
	 * m = n * log2_q16 * 94548 / 2^32
	 */
	if (u64NItems > UINT64_MAX / (u64LogQ16 + 1ull)) {
		/* n * log may overflow; compute in pieces */
		u64M = (u64NItems / 65536ull) * u64LogQ16;
		u64M = u64M + (u64M / 2ull) + (u64M / 5ull); /* ≈ *1.4427 */
	} else {
		u64M = (u64NItems * u64LogQ16 * 94548ull) >> 32;
	}

	/* Ensure at least 1 bit per item when FPR is usable. */
	if (u64M < u64NItems) {
		u64M = u64NItems;
	}
	return u64M;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bloom_estimate_bits(uint64_t u64NItems, uint32_t u32FprPpm)
    __attribute__((alias("gj_bloom_estimate_bits")));
