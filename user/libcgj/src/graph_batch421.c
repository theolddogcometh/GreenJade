/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch421: FNV-1a 32-bit hash (gj-prefixed surface).
 *
 * Surface (unique symbols only — do not export fnv1a_32 / __fnv1a_32):
 *   uint32_t gj_fnv1a32(const void *data, size_t n);
 *     — FNV-1a 32-bit over n bytes at data. Offset basis 2166136261,
 *       prime 16777619. NULL data with n != 0 → 0. Empty (n == 0) →
 *       offset basis.
 *   __gj_fnv1a32  (alias)
 *   __libcgj_batch421_marker = "libcgj-batch421"
 *
 * Distinct from batch41 fnv1a_32 / __fnv1a_32 and batch264 gj_fnv1a64.
 * This unit only defines gj_fnv1a32 and its alias.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch421_marker[] = "libcgj-batch421";

/* FNV-1a 32-bit parameters (public domain constants). */
#define B421_FNV32_OFFSET 2166136261u
#define B421_FNV32_PRIME  16777619u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fnv1a32 — FNV-1a 32-bit hash of n bytes at data.
 *
 * data: byte buffer (may be NULL only when n == 0)
 * n:    length in bytes
 *
 * Returns the FNV-1a digest. On NULL data with n != 0, returns 0.
 * Empty input (n == 0) returns the offset basis.
 */
uint32_t
gj_fnv1a32(const void *data, size_t n)
{
	const unsigned char *p = (const unsigned char *)data;
	uint32_t u32H = B421_FNV32_OFFSET;
	size_t i;

	if (p == NULL && n != 0u) {
		return 0u;
	}
	for (i = 0u; i < n; i++) {
		u32H ^= (uint32_t)p[i];
		u32H *= B421_FNV32_PRIME;
	}
	return u32H;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fnv1a32(const void *data, size_t n)
    __attribute__((alias("gj_fnv1a32")));
