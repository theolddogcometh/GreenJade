/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch963: FNV-1 (not FNV-1a) 32-bit hash.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_fnv1_32(const void *data, size_t n);
 *     — FNV-1 32-bit over n bytes at data. Offset basis 2166136261,
 *       prime 16777619. Step is (hash * prime) ^ byte (multiply then
 *       xor — opposite of FNV-1a). NULL data with n != 0 → 0. Empty
 *       (n == 0) → offset basis.
 *   uint32_t __gj_hash_fnv1_32  (alias)
 *   __libcgj_batch963_marker = "libcgj-batch963"
 *
 * Distinct from fnv1a_32 (batch41), gj_fnv1a32 (batch421), gj_fnv1a32_n
 * (batch619) — those are FNV-1a. Unique gj_hash_fnv1_32 surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch963_marker[] = "libcgj-batch963";

/* FNV-1 32-bit parameters (public domain constants). */
#define B963_FNV32_OFFSET 2166136261u
#define B963_FNV32_PRIME  16777619u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_fnv1_32 — FNV-1 32-bit hash of n bytes at data.
 *
 * data: byte buffer (may be NULL only when n == 0)
 * n:    length in bytes
 *
 * Returns the FNV-1 digest. On NULL data with n != 0, returns 0.
 * Empty input (n == 0) returns the offset basis.
 */
uint32_t
gj_hash_fnv1_32(const void *data, size_t n)
{
	const unsigned char *p = (const unsigned char *)data;
	uint32_t u32H = B963_FNV32_OFFSET;
	size_t i;

	if (p == NULL && n != 0u) {
		return 0u;
	}
	for (i = 0u; i < n; i++) {
		/* FNV-1: multiply then xor (FNV-1a is xor then multiply). */
		u32H *= B963_FNV32_PRIME;
		u32H ^= (uint32_t)p[i];
	}
	return u32H;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_fnv1_32(const void *data, size_t n)
    __attribute__((alias("gj_hash_fnv1_32")));
