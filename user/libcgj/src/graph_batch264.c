/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch264: FNV-1a 64-bit hash (gj-prefixed surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_fnv1a64(const void *data, size_t n);
 *     — FNV-1a 64-bit over n bytes at data. Offset basis
 *       14695981039346656037, prime 1099511628211.
 *       NULL data with n != 0 → 0. Empty (n == 0) → offset basis.
 *   __gj_fnv1a64  (alias)
 *   __libcgj_batch264_marker = "libcgj-batch264"
 *
 * Distinct from batch41 fnv1a_64 / __fnv1a_64 (no gj_ prefix). This unit
 * only defines gj_fnv1a64 and its alias — does not redefine fnv1a_64.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch264_marker[] = "libcgj-batch264";

/* FNV-1a 64-bit parameters (public domain constants). */
#define B264_FNV64_OFFSET 14695981039346656037ULL
#define B264_FNV64_PRIME  1099511628211ULL

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fnv1a64 — FNV-1a 64-bit hash of n bytes at data.
 *
 * data: byte buffer (may be NULL only when n == 0)
 * n:    length in bytes
 *
 * Returns the FNV-1a digest. On NULL data with n != 0, returns 0.
 * Empty input (n == 0) returns the offset basis.
 */
uint64_t
gj_fnv1a64(const void *data, size_t n)
{
	const unsigned char *p = (const unsigned char *)data;
	uint64_t h = B264_FNV64_OFFSET;
	size_t i;

	if (p == NULL && n != 0u) {
		return 0ULL;
	}
	for (i = 0u; i < n; i++) {
		h ^= (uint64_t)p[i];
		h *= B264_FNV64_PRIME;
	}
	return h;
}

uint64_t __gj_fnv1a64(const void *data, size_t n)
    __attribute__((alias("gj_fnv1a64")));
