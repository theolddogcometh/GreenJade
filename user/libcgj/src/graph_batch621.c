/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch621: sdbm classic hash over n bytes (gj_sdbm_n).
 *
 * Surface (unique symbols only — do not export sdbm_hash / __sdbm_hash
 * or gj_sdbm / __gj_sdbm; those live in batch42 / batch423):
 *   uint32_t gj_sdbm_n(const void *data, size_t n);
 *     — sdbm hash over n bytes at data. Step is
 *       c + (hash << 6) + (hash << 16) - hash. NULL data with n != 0 → 0.
 *       Empty (n == 0) → 0.
 *   __gj_sdbm_n  (alias)
 *   __libcgj_batch621_marker = "libcgj-batch621"
 *
 * Distinct from batch42 sdbm_hash / __sdbm_hash and batch423 gj_sdbm /
 * __gj_sdbm. This unit only defines gj_sdbm_n and its alias.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch621_marker[] = "libcgj-batch621";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sdbm_n — classic sdbm hash of n bytes at data.
 *
 * data: byte buffer (may be NULL only when n == 0)
 * n:    length in bytes
 *
 * Returns the 32-bit digest. On NULL data with n != 0, returns 0.
 * Empty input (n == 0) returns 0.
 */
uint32_t
gj_sdbm_n(const void *data, size_t n)
{
	const unsigned char *p = (const unsigned char *)data;
	uint32_t u32H = 0u;
	size_t i;

	if (p == NULL && n != 0u) {
		return 0u;
	}
	for (i = 0u; i < n; i++) {
		u32H = (uint32_t)p[i] + (u32H << 6) + (u32H << 16) - u32H;
	}
	return u32H;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sdbm_n(const void *data, size_t n)
    __attribute__((alias("gj_sdbm_n")));
