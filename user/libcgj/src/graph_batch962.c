/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch962: sdbm classic hash with caller seed.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_sdbm_seed(const void *data, size_t n, uint32_t seed);
 *     — sdbm hash over n bytes at data, starting from seed. Step is
 *       c + (hash << 6) + (hash << 16) - hash. NULL data with n != 0 → 0.
 *       Empty (n == 0) → seed.
 *   uint32_t __gj_hash_sdbm_seed  (alias)
 *   __libcgj_batch962_marker = "libcgj-batch962"
 *
 * Distinct from sdbm_hash (batch42), gj_sdbm (batch423), gj_sdbm_n
 * (batch621) — those start at 0. Unique gj_hash_sdbm_seed surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch962_marker[] = "libcgj-batch962";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_sdbm_seed — seeded sdbm hash of n bytes at data.
 *
 * data: byte buffer (may be NULL only when n == 0)
 * n:    length in bytes
 * seed: initial hash state (classic sdbm starts at 0)
 *
 * Returns the 32-bit digest. On NULL data with n != 0, returns 0.
 * Empty input (n == 0) returns seed unchanged.
 */
uint32_t
gj_hash_sdbm_seed(const void *data, size_t n, uint32_t seed)
{
	const unsigned char *p = (const unsigned char *)data;
	uint32_t u32H = seed;
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

uint32_t __gj_hash_sdbm_seed(const void *data, size_t n, uint32_t seed)
    __attribute__((alias("gj_hash_sdbm_seed")));
