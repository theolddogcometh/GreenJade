/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch961: djb2 classic hash with caller seed.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_djb2_seed(const void *data, size_t n, uint32_t seed);
 *     — Dan Bernstein djb2 over n bytes at data, starting from seed.
 *       Step is hash * 33 + c. NULL data with n != 0 → 0. Empty
 *       (n == 0) → seed.
 *   uint32_t __gj_hash_djb2_seed  (alias)
 *   __libcgj_batch961_marker = "libcgj-batch961"
 *
 * Distinct from djb2_hash (batch42), gj_djb2 (batch422), gj_djb2_n
 * (batch620) — those use fixed seed 5381. Unique gj_hash_djb2_seed
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch961_marker[] = "libcgj-batch961";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_djb2_seed — seeded djb2 hash of n bytes at data.
 *
 * data: byte buffer (may be NULL only when n == 0)
 * n:    length in bytes
 * seed: initial hash state (classic djb2 uses 5381)
 *
 * Returns the 32-bit digest. On NULL data with n != 0, returns 0.
 * Empty input (n == 0) returns seed unchanged.
 */
uint32_t
gj_hash_djb2_seed(const void *data, size_t n, uint32_t seed)
{
	const unsigned char *p = (const unsigned char *)data;
	uint32_t u32H = seed;
	size_t i;

	if (p == NULL && n != 0u) {
		return 0u;
	}
	for (i = 0u; i < n; i++) {
		/* hash * 33 + c */
		u32H = ((u32H << 5) + u32H) + (uint32_t)p[i];
	}
	return u32H;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_djb2_seed(const void *data, size_t n, uint32_t seed)
    __attribute__((alias("gj_hash_djb2_seed")));
