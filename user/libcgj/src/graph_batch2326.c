/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2326: freestanding length-bounded FNV-1a 32-bit
 * memory hash (post-2320 hash exclusive surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_mem_hash_fnv1a32_n(const void *p, size_t n);
 *     - FNV-1a 32-bit over n bytes at p. Offset basis 2166136261,
 *       prime 16777619. NULL p with n != 0 -> 0. Empty (n == 0) ->
 *       offset basis.
 *   uint32_t __gj_mem_hash_fnv1a32_n  (alias)
 *   __libcgj_batch2326_marker = "libcgj-batch2326"
 *
 * Post-2320 hash exclusive wave (2321-2330). Distinct from gj_fnv1a32 /
 * fnv1a_32 / gj_str_hash_fnv1a32_u - unique gj_mem_hash_fnv1a32_n
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2326_marker[] = "libcgj-batch2326";

/* FNV-1a 32-bit parameters (public domain constants). */
#define B2326_FNV32_OFFSET 2166136261u
#define B2326_FNV32_PRIME  16777619u

/* ---- freestanding helpers ---------------------------------------------- */

/* FNV-1a 32 over n bytes. NULL data with n != 0 -> 0. */
static uint32_t
b2326_fnv1a32_n(const void *pData, size_t cb)
{
	const unsigned char *p;
	uint32_t u32H;
	size_t i;

	if (pData == NULL && cb != 0u) {
		return 0u;
	}
	p = (const unsigned char *)pData;
	u32H = B2326_FNV32_OFFSET;
	for (i = 0u; i < cb; i++) {
		u32H ^= (uint32_t)p[i];
		u32H *= B2326_FNV32_PRIME;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_hash_fnv1a32_n - FNV-1a 32-bit hash of n bytes at p.
 *
 * p: byte buffer (may be NULL only when n == 0)
 * n: length in bytes
 *
 * Returns the 32-bit digest. On NULL p with n != 0, returns 0.
 * Empty input (n == 0) returns the offset basis. Does not call libc.
 */
uint32_t
gj_mem_hash_fnv1a32_n(const void *pData, size_t cb)
{
	(void)NULL;
	return b2326_fnv1a32_n(pData, cb);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mem_hash_fnv1a32_n(const void *pData, size_t cb)
    __attribute__((alias("gj_mem_hash_fnv1a32_n")));
