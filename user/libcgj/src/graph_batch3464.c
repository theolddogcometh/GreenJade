/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3464: freestanding length-bounded FNV-1a 32-bit
 * buffer hash.
 *
 * Surface (unique symbols):
 *   uint32_t gj_buf_hash_fnv1a32_u(const void *p, size_t n);
 *     - FNV-1a 32-bit over n bytes at p. Offset basis 2166136261,
 *       prime 16777619. NULL p with n != 0 -> 0. Empty (n == 0) ->
 *       offset basis.
 *   uint32_t __gj_buf_hash_fnv1a32_u  (alias)
 *   __libcgj_batch3464_marker = "libcgj-batch3464"
 *
 * CREATE-ONLY exclusive continuum wave (3461-3470). Unique
 * gj_buf_hash_fnv1a32_u surface only; no multi-def. Distinct from
 * gj_mem_hash_fnv1a32_n (batch2326) / fnv1a_32 / gj_str_hash_fnv1a32_u.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3464_marker[] = "libcgj-batch3464";

/* FNV-1a 32-bit parameters (public domain constants). */
#define B3464_FNV32_OFFSET 2166136261u
#define B3464_FNV32_PRIME  16777619u

/* ---- freestanding helpers ---------------------------------------------- */

/* FNV-1a 32 over n bytes. NULL data with n != 0 -> 0. */
static uint32_t
b3464_fnv1a32(const void *pData, size_t cb)
{
	const unsigned char *p;
	uint32_t u32H;
	size_t i;

	if (pData == NULL && cb != 0u) {
		return 0u;
	}
	p = (const unsigned char *)pData;
	u32H = B3464_FNV32_OFFSET;
	for (i = 0u; i < cb; i++) {
		u32H ^= (uint32_t)p[i];
		u32H *= B3464_FNV32_PRIME;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_hash_fnv1a32_u - FNV-1a 32-bit hash of n bytes at p.
 *
 * p: byte buffer (may be NULL only when n == 0)
 * n: length in bytes
 *
 * Returns the 32-bit digest. On NULL p with n != 0, returns 0.
 * Empty input (n == 0) returns the offset basis. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_buf_hash_fnv1a32_u(const void *pData, size_t cb)
{
	(void)NULL;
	return b3464_fnv1a32(pData, cb);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_buf_hash_fnv1a32_u(const void *pData, size_t cb)
    __attribute__((alias("gj_buf_hash_fnv1a32_u")));
