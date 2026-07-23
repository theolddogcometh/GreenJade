/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1358: Bloom filter hash-count estimate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bloom_estimate_hashes(uint64_t m_bits, uint64_t n_items);
 *     — Optimal hash count k ≈ (m/n) * ln(2) ≈ (m/n) * 0.693147.
 *       Returns 0 on bad args (n==0 or m==0). Clamped to [1, 256].
 *   uint32_t __gj_bloom_estimate_hashes  (alias)
 *   __libcgj_batch1358_marker = "libcgj-batch1358"
 *
 * Distinct from gj_bloom_* ops (batch125) and gj_bloom_estimate_bits
 * (batch1357). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1358_marker[] = "libcgj-batch1358";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bloom_estimate_hashes — optimal k for given m and n.
 *
 * u64MBits:  bit array size m
 * u64NItems: expected insertions n
 *
 * k = round((m/n) * ln2). Fixed-point: (m * 45426) / (n * 65536)
 * where 45426/65536 ≈ 0.693147 (ln2).
 */
uint32_t
gj_bloom_estimate_hashes(uint64_t u64MBits, uint64_t u64NItems)
{
	uint64_t u64K;

	if (u64MBits == 0ull || u64NItems == 0ull) {
		return 0u;
	}

	/*
	 * k = m * ln2 / n
	 * Use k = (m * 45426) / (n << 16) with 45426 ≈ ln2 * 65536.
	 * Avoid intermediate overflow: if m large, divide first.
	 */
	if (u64MBits <= UINT64_MAX / 45426ull) {
		u64K = (u64MBits * 45426ull) / (u64NItems << 16);
		/* When n is huge, n<<16 may overflow — fall through fix. */
		if (u64NItems > (UINT64_MAX >> 16)) {
			u64K = (u64MBits * 45426ull / u64NItems) >> 16;
		}
	} else {
		u64K = ((u64MBits / u64NItems) * 45426ull) >> 16;
	}

	if (u64K == 0ull) {
		return 1u;
	}
	if (u64K > 256ull) {
		return 256u;
	}
	return (uint32_t)u64K;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bloom_estimate_hashes(uint64_t u64MBits, uint64_t u64NItems)
    __attribute__((alias("gj_bloom_estimate_hashes")));
