/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch892: single-word Bloom query (unique name).
 *
 * Distinct from gj_bloom64_may (batch208) — unique symbol
 * gj_bloom64_query_u64 to avoid multi-def. Pair with batch891 insert.
 *
 * Surface (unique symbols):
 *   int gj_bloom64_query_u64(uint64_t b, uint64_t h);
 *     — 1 if all k bits for h are set in b (possible member), else 0.
 *   int __gj_bloom64_query_u64  (alias)
 *   __libcgj_batch892_marker = "libcgj-batch892"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch892_marker[] = "libcgj-batch892";

#define B892_K 4u

static uint64_t
b892_mask(uint64_t h)
{
	uint64_t h1;
	uint64_t h2;
	uint64_t m;
	unsigned i;

	h1 = h;
	h2 = (h >> 32) | 1ull;
	m = 0ull;
	for (i = 0u; i < B892_K; i++) {
		unsigned bit = (unsigned)((h1 + (uint64_t)i * h2) & 63ull);

		m |= (uint64_t)1ull << bit;
	}
	return m;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bloom64_query_u64 — possible-membership test for hash h.
 *
 * Returns 1 if every probe bit is set, 0 if any is clear (definite miss).
 * b is passed by value (snapshot of the filter word).
 */
int
gj_bloom64_query_u64(uint64_t b, uint64_t h)
{
	uint64_t m;

	m = b892_mask(h);
	return (b & m) == m;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bloom64_query_u64(uint64_t b, uint64_t h)
    __attribute__((alias("gj_bloom64_query_u64")));
