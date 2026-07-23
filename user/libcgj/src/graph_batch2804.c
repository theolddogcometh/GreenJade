/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2804: sdbm classic hash (exclusive _u surface).
 *
 * Surface (unique symbols only — do not export sdbm_hash / gj_sdbm):
 *   uint32_t gj_sdbm_u(const void *data, size_t n);
 *     - sdbm hash over n bytes at data. Step is
 *       c + (hash << 6) + (hash << 16) - hash. NULL data with n != 0 -> 0.
 *       Empty (n == 0) -> 0.
 *   uint32_t __gj_sdbm_u  (alias)
 *   __libcgj_batch2804_marker = "libcgj-batch2804"
 *
 * Distinct from batch42 sdbm_hash, batch423 gj_sdbm, batch621 gj_sdbm_n,
 * batch1704 gj_sdbm64_update — no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2804_marker[] = "libcgj-batch2804";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2804_sdbm(const unsigned char *pData, size_t cbN)
{
	uint32_t u32H;
	size_t i;

	u32H = 0u;
	for (i = 0u; i < cbN; i++) {
		u32H = (uint32_t)pData[i] + (u32H << 6) + (u32H << 16) - u32H;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sdbm_u - classic sdbm hash of n bytes at data.
 *
 * data: byte buffer (may be NULL only when n == 0)
 * n:    length in bytes
 *
 * Returns the 32-bit digest. On NULL data with n != 0, returns 0.
 * Empty input (n == 0) returns 0.
 */
uint32_t
gj_sdbm_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return 0u;
	}
	return b2804_sdbm(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sdbm_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_sdbm_u")));
