/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4166: freestanding length-bounded sdbm buffer
 * hash.
 *
 * Surface (unique symbols):
 *   uint32_t gj_buf_hash_sdbm_u(const void *p, size_t n);
 *     - sdbm hash over n bytes at p. Step is
 *       c + (hash << 6) + (hash << 16) - hash. NULL p with n != 0 -> 0.
 *       Empty (n == 0) -> 0.
 *   uint32_t __gj_buf_hash_sdbm_u  (alias)
 *   __libcgj_batch4166_marker = "libcgj-batch4166"
 *
 * CREATE-ONLY exclusive continuum wave (4161-4170). Unique
 * gj_buf_hash_sdbm_u surface only; no multi-def. Distinct from
 * gj_str_hash_sdbm_u / sdbm_hash / gj_sdbm_n (string or other surfaces).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4166_marker[] = "libcgj-batch4166";

/* ---- freestanding helpers ---------------------------------------------- */

/* sdbm over n bytes. NULL data with n != 0 -> 0. */
static uint32_t
b4166_sdbm(const void *pData, size_t cb)
{
	const unsigned char *p;
	uint32_t u32H;
	size_t i;

	if (pData == NULL && cb != 0u) {
		return 0u;
	}
	p = (const unsigned char *)pData;
	u32H = 0u;
	for (i = 0u; i < cb; i++) {
		u32H = (uint32_t)p[i] + (u32H << 6) + (u32H << 16) - u32H;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_hash_sdbm_u - classic sdbm hash of n bytes at p.
 *
 * p: byte buffer (may be NULL only when n == 0)
 * n: length in bytes
 *
 * Returns the 32-bit digest. On NULL p with n != 0, returns 0.
 * Empty input (n == 0) returns 0. Does not call libc. No parent wires.
 */
uint32_t
gj_buf_hash_sdbm_u(const void *pData, size_t cb)
{
	(void)NULL;
	return b4166_sdbm(pData, cb);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_buf_hash_sdbm_u(const void *pData, size_t cb)
    __attribute__((alias("gj_buf_hash_sdbm_u")));
