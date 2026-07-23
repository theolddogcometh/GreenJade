/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch735: seeded FNV-1a 32-bit hash.
 *
 * Surface (unique symbols only — do not export fnv1a_32 / gj_fnv1a32):
 *   uint32_t gj_fnv1a32_seed(const void *d, size_t n, uint32_t seed);
 *     — FNV-1a 32-bit over n bytes at d, starting from seed (instead of
 *       the fixed offset basis). Prime 16777619. NULL d with n != 0 → 0.
 *       Empty (n == 0) → seed unchanged. Pass seed = 2166136261u to
 *       match classic unseeded FNV-1a-32.
 *   __gj_fnv1a32_seed  (alias)
 *   __libcgj_batch735_marker = "libcgj-batch735"
 *
 * Distinct from batch41 fnv1a_32 / __fnv1a_32 and batch421 gj_fnv1a32 /
 * __gj_fnv1a32. This unit only defines the seeded surface — no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch735_marker[] = "libcgj-batch735";

/* FNV-1a 32-bit prime (public domain constant). */
#define B735_FNV32_PRIME 16777619u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fnv1a32_seed — seeded FNV-1a 32-bit hash of n bytes at d.
 *
 * d:    byte buffer (may be NULL only when n == 0)
 * n:    length in bytes
 * seed: initial hash state (offset basis for classic FNV-1a)
 *
 * Returns the FNV-1a digest. On NULL d with n != 0, returns 0.
 * Empty input (n == 0) returns seed.
 */
uint32_t
gj_fnv1a32_seed(const void *d, size_t n, uint32_t seed)
{
	const unsigned char *p = (const unsigned char *)d;
	uint32_t u32H = seed;
	size_t i;

	if (p == NULL && n != 0u) {
		return 0u;
	}
	for (i = 0u; i < n; i++) {
		u32H ^= (uint32_t)p[i];
		u32H *= B735_FNV32_PRIME;
	}
	return u32H;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fnv1a32_seed(const void *d, size_t n, uint32_t seed)
    __attribute__((alias("gj_fnv1a32_seed")));
