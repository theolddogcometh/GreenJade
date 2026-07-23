/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1504: 3-byte hash for LZ77 sliding window.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lz77_hash3(const uint8_t *p);
 *     — Hash the three bytes at p[0..2] into a 32-bit value suitable for
 *       an LZ77 hash chain / hash table index (caller masks to table size).
 *       p must be non-NULL and address at least three readable bytes.
 *       Returns 0 if p is NULL (safe freestanding guard).
 *   uint32_t __gj_lz77_hash3  (alias)
 *   __libcgj_batch1504_marker = "libcgj-batch1504"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1504_marker[] = "libcgj-batch1504";

/*
 * Multiplicative mixer (odd constant). Distinct from other hash TUs;
 * not a copy of any third-party LZ table.
 */
#define B1504_HASH_MUL 0x9E3779B1u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lz77_hash3 — pack three bytes little-endian and multiply-mix.
 *
 * Result is a full 32-bit hash; typical LZ table use is
 * (hash >> (32 - log2(table_size))) or (hash & (table_size - 1)).
 */
uint32_t
gj_lz77_hash3(const uint8_t *pSrc)
{
	uint32_t u32Trip;
	uint32_t u32H;

	if (pSrc == NULL) {
		return 0u;
	}

	u32Trip = (uint32_t)pSrc[0]
	    | ((uint32_t)pSrc[1] << 8)
	    | ((uint32_t)pSrc[2] << 16);

	u32H = u32Trip * B1504_HASH_MUL;
	/* Fold upper bits into lower for better low-bit diffusion. */
	u32H ^= (u32H >> 16);
	return u32H;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lz77_hash3(const uint8_t *pSrc)
    __attribute__((alias("gj_lz77_hash3")));
