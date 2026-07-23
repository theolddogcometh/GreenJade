/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch208: fixed 64-bit (single-word) Bloom filter.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party Bloom source was copied.
 *
 * Distinct from the variable-size Bloom surface in batch125:
 *   gj_bloom_init / gj_bloom_add / gj_bloom_may_contain  → graph_batch125.c
 * This TU operates on a single uint64_t word (m = 64 bits) and takes a
 * precomputed 64-bit hash rather than raw key bytes.
 *
 * Surface (unique symbols):
 *   void gj_bloom64_add(uint64_t *b, uint64_t h);
 *     — OR k bit positions derived from h into *b. No-op if b == NULL.
 *   int  gj_bloom64_may(uint64_t b, uint64_t h);
 *     — 1 if all k bits for h are set in b (possible member), else 0.
 *   __gj_bloom64_add / __gj_bloom64_may  (aliases)
 *   __libcgj_batch208_marker = "libcgj-batch208"
 *
 * Hashing: Kirsch–Mitzenmacher double-hashing on the single word:
 *   h1 = h
 *   h2 = ((h >> 32) | 1)          — forced odd for full period mod 64
 *   bit_i = (h1 + i * h2) mod 64  for i in [0, k)
 * k = 4 probes (standard for m=64 at modest load).
 *
 * No errno / malloc (freestanding-safe). Callers zero *b before use.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch208_marker[] = "libcgj-batch208";

/* Number of bit probes per hash. */
#define B208_K 4u

/*
 * Build the k-bit membership mask for hash h over a 64-bit filter.
 * Positions: (h1 + i * h2) & 63 with h2 forced odd.
 */
static uint64_t
b208_mask(uint64_t h)
{
	uint64_t h1;
	uint64_t h2;
	uint64_t m;
	unsigned i;

	h1 = h;
	/* High half as step; force odd so the linear sequence covers all 64. */
	h2 = (h >> 32) | 1ull;
	m = 0ull;
	for (i = 0u; i < B208_K; i++) {
		unsigned bit = (unsigned)((h1 + (uint64_t)i * h2) & 63ull);

		m |= (uint64_t)1ull << bit;
	}
	return m;
}

/* ---- public API -------------------------------------------------------- */

/*
 * Insert hash h into the single-word filter *b by setting its k bits.
 * No-op when b is NULL.
 */
void
gj_bloom64_add(uint64_t *b, uint64_t h)
{
	if (b == NULL) {
		return;
	}
	*b |= b208_mask(h);
}

/*
 * Possible-membership test for hash h against filter word b.
 * Returns 1 if every probe bit is set, 0 if any is clear (definite miss).
 * b is passed by value (snapshot of the filter word).
 */
int
gj_bloom64_may(uint64_t b, uint64_t h)
{
	uint64_t m;

	m = b208_mask(h);
	return (b & m) == m;
}

void __gj_bloom64_add(uint64_t *b, uint64_t h)
    __attribute__((alias("gj_bloom64_add")));
int __gj_bloom64_may(uint64_t b, uint64_t h)
    __attribute__((alias("gj_bloom64_may")));
