/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch964: FNV-1 (not FNV-1a) 64-bit hash.
 *
 * Surface (unique symbols):
 *   uint64_t gj_hash_fnv1_64(const void *data, size_t n);
 *     — FNV-1 64-bit over n bytes at data. Offset basis
 *       14695981039346656037, prime 1099511628211. Step is
 *       (hash * prime) ^ byte. NULL data with n != 0 → 0. Empty
 *       (n == 0) → offset basis.
 *   uint64_t __gj_hash_fnv1_64  (alias)
 *   __libcgj_batch964_marker = "libcgj-batch964"
 *
 * Distinct from fnv1a_64 (batch41), gj_fnv1a64 (batch264) — those are
 * FNV-1a. Unique gj_hash_fnv1_64 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch964_marker[] = "libcgj-batch964";

/* FNV-1 64-bit parameters (public domain constants). */
#define B964_FNV64_OFFSET 14695981039346656037ULL
#define B964_FNV64_PRIME  1099511628211ULL

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_fnv1_64 — FNV-1 64-bit hash of n bytes at data.
 *
 * data: byte buffer (may be NULL only when n == 0)
 * n:    length in bytes
 *
 * Returns the FNV-1 digest. On NULL data with n != 0, returns 0.
 * Empty input (n == 0) returns the offset basis.
 */
uint64_t
gj_hash_fnv1_64(const void *data, size_t n)
{
	const unsigned char *p = (const unsigned char *)data;
	uint64_t u64H = B964_FNV64_OFFSET;
	size_t i;

	if (p == NULL && n != 0u) {
		return 0ULL;
	}
	for (i = 0u; i < n; i++) {
		/* FNV-1: multiply then xor. */
		u64H *= B964_FNV64_PRIME;
		u64H ^= (uint64_t)p[i];
	}
	return u64H;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hash_fnv1_64(const void *data, size_t n)
    __attribute__((alias("gj_hash_fnv1_64")));
