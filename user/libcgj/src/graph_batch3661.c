/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3661: Bloom filter primary hash (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bloom_hash1_u(const void *key, size_t n);
 *     - First independent 64-bit digest for Kirsch–Mitzenmacher
 *       double-hashing. FNV-1a 64 over key with the standard offset
 *       basis. key == NULL or n == 0 → offset basis alone.
 *   uint64_t __gj_bloom_hash1_u  (alias)
 *   __libcgj_batch3661_marker = "libcgj-batch3661"
 *
 * Exclusive continuum CREATE-ONLY (3661-3670): bloom_hash1_u,
 * bloom_hash2_u, bloom_test_bit_u, bloom_set_bit_u, hll_register_u,
 * hll_estimate_u, cms_index_u, cms_inc_u, topk_candidate_u,
 * batch_id_3670. Distinct from gj_bloom_* (batch125) and
 * gj_bloom64_* (batch208) — unique _u hash surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3661_marker[] = "libcgj-batch3661";

/* FNV-1a 64 offset / prime (static to this TU). */
#define B3661_FNV_OFF    0xcbf29ce484222325ULL
#define B3661_FNV_PRIME  0x100000001b3ULL

/* ---- freestanding helpers ---------------------------------------------- */

/* FNV-1a 64 over pKey[0..cb). NULL/empty returns offset basis. */
static uint64_t
b3661_fnv1a64(const void *pKey, size_t cb)
{
	const unsigned char *p;
	uint64_t h;
	size_t i;

	h = B3661_FNV_OFF;
	if (pKey == NULL || cb == 0u) {
		return h;
	}
	p = (const unsigned char *)pKey;
	for (i = 0u; i < cb; i++) {
		h ^= (uint64_t)p[i];
		h *= B3661_FNV_PRIME;
	}
	return h;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bloom_hash1_u - primary Bloom / double-hash digest of key[0..n).
 *
 * Returns FNV-1a 64 with the standard offset basis. Does not call libc.
 * No parent wires.
 */
uint64_t
gj_bloom_hash1_u(const void *pKey, size_t cN)
{
	(void)NULL;
	return b3661_fnv1a64(pKey, cN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bloom_hash1_u(const void *pKey, size_t cN)
    __attribute__((alias("gj_bloom_hash1_u")));
