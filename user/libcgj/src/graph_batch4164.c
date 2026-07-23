/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4164: freestanding length-bounded FNV-1a 32-bit
 * buffer hash (wave-unique rename).
 *
 * Surface (unique symbols):
 *   uint32_t gj_buf_hash_fnv1a32_u2(const void *p, size_t n);
 *     - FNV-1a 32-bit over n bytes at p. Offset basis 2166136261,
 *       prime 16777619. NULL p with n != 0 -> 0. Empty (n == 0) ->
 *       offset basis. Renamed from gj_buf_hash_fnv1a32_u (collision
 *       with batch3464).
 *   uint32_t __gj_buf_hash_fnv1a32_u2  (alias)
 *   __libcgj_batch4164_marker = "libcgj-batch4164"
 *
 * CREATE-ONLY exclusive continuum wave (4161-4170). Unique
 * gj_buf_hash_fnv1a32_u2 surface only; no multi-def. Distinct from
 * gj_buf_hash_fnv1a32_u (batch3464) / gj_mem_hash_fnv1a32_n / fnv1a_32.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4164_marker[] = "libcgj-batch4164";

/* FNV-1a 32-bit parameters (public domain constants). */
#define B4164_FNV32_OFFSET 2166136261u
#define B4164_FNV32_PRIME  16777619u

/* ---- freestanding helpers ---------------------------------------------- */

/* FNV-1a 32 over n bytes. NULL data with n != 0 -> 0. */
static uint32_t
b4164_fnv1a32(const void *pData, size_t cb)
{
	const unsigned char *p;
	uint32_t u32H;
	size_t i;

	if (pData == NULL && cb != 0u) {
		return 0u;
	}
	p = (const unsigned char *)pData;
	u32H = B4164_FNV32_OFFSET;
	for (i = 0u; i < cb; i++) {
		u32H ^= (uint32_t)p[i];
		u32H *= B4164_FNV32_PRIME;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_hash_fnv1a32_u2 - FNV-1a 32-bit hash of n bytes at p.
 *
 * p: byte buffer (may be NULL only when n == 0)
 * n: length in bytes
 *
 * Returns the 32-bit digest. On NULL p with n != 0, returns 0.
 * Empty input (n == 0) returns the offset basis. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_buf_hash_fnv1a32_u2(const void *pData, size_t cb)
{
	(void)NULL;
	return b4164_fnv1a32(pData, cb);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_buf_hash_fnv1a32_u2(const void *pData, size_t cb)
    __attribute__((alias("gj_buf_hash_fnv1a32_u2")));
