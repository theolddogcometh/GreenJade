/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2801: FNV-1a 32-bit hash (exclusive _u surface).
 *
 * Surface (unique symbols only — do not export fnv1a_32 / gj_fnv1a32):
 *   uint32_t gj_fnv1a32_u(const void *data, size_t n);
 *     - FNV-1a 32-bit over n bytes at data. Offset basis 2166136261,
 *       prime 16777619. NULL data with n != 0 -> 0. Empty (n == 0) ->
 *       offset basis.
 *   uint32_t __gj_fnv1a32_u  (alias)
 *   __libcgj_batch2801_marker = "libcgj-batch2801"
 *
 * Distinct from batch41 fnv1a_32, batch421 gj_fnv1a32, batch619
 * gj_fnv1a32_n, batch735 gj_fnv1a32_seed — no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2801_marker[] = "libcgj-batch2801";

/* FNV-1a 32-bit parameters (public domain constants). */
#define B2801_FNV32_OFFSET 2166136261u
#define B2801_FNV32_PRIME  16777619u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2801_fnv1a32(const unsigned char *pData, size_t cbN)
{
	uint32_t u32H;
	size_t i;

	u32H = B2801_FNV32_OFFSET;
	for (i = 0u; i < cbN; i++) {
		u32H ^= (uint32_t)pData[i];
		u32H *= B2801_FNV32_PRIME;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fnv1a32_u - FNV-1a 32-bit hash of n bytes at data.
 *
 * data: byte buffer (may be NULL only when n == 0)
 * n:    length in bytes
 *
 * Returns the FNV-1a digest. On NULL data with n != 0, returns 0.
 * Empty input (n == 0) returns the offset basis.
 */
uint32_t
gj_fnv1a32_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return B2801_FNV32_OFFSET;
	}
	return b2801_fnv1a32(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fnv1a32_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_fnv1a32_u")));
