/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3465: freestanding length-bounded FNV-1a 64-bit
 * buffer hash.
 *
 * Surface (unique symbols):
 *   uint64_t gj_buf_hash_fnv1a64_u(const void *p, size_t n);
 *     - FNV-1a 64-bit over n bytes at p. Offset basis
 *       14695981039346656037, prime 1099511628211. NULL p with n != 0
 *       -> 0. Empty (n == 0) -> offset basis.
 *   uint64_t __gj_buf_hash_fnv1a64_u  (alias)
 *   __libcgj_batch3465_marker = "libcgj-batch3465"
 *
 * CREATE-ONLY exclusive continuum wave (3461-3470). Unique
 * gj_buf_hash_fnv1a64_u surface only; no multi-def. Distinct from
 * gj_fnv1a64 / gj_fnv1a64_n / gj_str_hash_fnv1a64_u. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3465_marker[] = "libcgj-batch3465";

/* FNV-1a 64-bit parameters (public domain constants). */
#define B3465_FNV64_OFFSET 14695981039346656037ULL
#define B3465_FNV64_PRIME  1099511628211ULL

/* ---- freestanding helpers ---------------------------------------------- */

/* FNV-1a 64 over n bytes. NULL data with n != 0 -> 0. */
static uint64_t
b3465_fnv1a64(const void *pData, size_t cb)
{
	const unsigned char *p;
	uint64_t u64H;
	size_t i;

	if (pData == NULL && cb != 0u) {
		return 0ULL;
	}
	p = (const unsigned char *)pData;
	u64H = B3465_FNV64_OFFSET;
	for (i = 0u; i < cb; i++) {
		u64H ^= (uint64_t)p[i];
		u64H *= B3465_FNV64_PRIME;
	}
	return u64H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_hash_fnv1a64_u - FNV-1a 64-bit hash of n bytes at p.
 *
 * p: byte buffer (may be NULL only when n == 0)
 * n: length in bytes
 *
 * Returns the 64-bit digest. On NULL p with n != 0, returns 0.
 * Empty input (n == 0) returns the offset basis. Does not call libc.
 * No parent wires.
 */
uint64_t
gj_buf_hash_fnv1a64_u(const void *pData, size_t cb)
{
	(void)NULL;
	return b3465_fnv1a64(pData, cb);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_buf_hash_fnv1a64_u(const void *pData, size_t cb)
    __attribute__((alias("gj_buf_hash_fnv1a64_u")));
