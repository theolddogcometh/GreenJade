/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch891: single-word Bloom insert (unique name).
 *
 * Distinct from gj_bloom64_add (batch208) and gj_bloom_add (batch125) —
 * unique symbol gj_bloom64_insert_u64 to avoid multi-def.
 *
 * Surface (unique symbols):
 *   void gj_bloom64_insert_u64(uint64_t *b, uint64_t h);
 *     — OR k bit positions derived from h into *b. No-op if b == NULL.
 *   void __gj_bloom64_insert_u64  (alias)
 *   __libcgj_batch891_marker = "libcgj-batch891"
 *
 * Hashing: Kirsch–Mitzenmacher double-hashing on one word, k = 4:
 *   h1 = h; h2 = (h >> 32) | 1; bit_i = (h1 + i * h2) mod 64
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Callers zero *b before use.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch891_marker[] = "libcgj-batch891";

#define B891_K 4u

/*
 * Build the k-bit membership mask for hash h over a 64-bit filter.
 */
static uint64_t
b891_mask(uint64_t h)
{
	uint64_t h1;
	uint64_t h2;
	uint64_t m;
	unsigned i;

	h1 = h;
	h2 = (h >> 32) | 1ull;
	m = 0ull;
	for (i = 0u; i < B891_K; i++) {
		unsigned bit = (unsigned)((h1 + (uint64_t)i * h2) & 63ull);

		m |= (uint64_t)1ull << bit;
	}
	return m;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bloom64_insert_u64 — insert hash h into single-word filter *b.
 *
 * Sets the k probe bits for h. No-op when b is NULL.
 */
void
gj_bloom64_insert_u64(uint64_t *b, uint64_t h)
{
	if (b == NULL) {
		return;
	}
	*b |= b891_mask(h);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_bloom64_insert_u64(uint64_t *b, uint64_t h)
    __attribute__((alias("gj_bloom64_insert_u64")));
