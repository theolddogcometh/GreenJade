/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2788: load one little-endian uint64_t from
 * eight octets (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bytes_to_u64_le_u(const unsigned char *src);
 *     - Assemble src[0..7] as a little-endian uint64_t. Returns 0 if
 *       src is NULL. Unaligned src is fine (byte-wise load).
 *   uint64_t __gj_bytes_to_u64_le_u  (alias)
 *   __libcgj_batch2788_marker = "libcgj-batch2788"
 *
 * Integer-root exclusive wave (2781-2790). Unique single-value _u
 * surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2788_marker[] = "libcgj-batch2788";

/* ---- freestanding helpers ---------------------------------------------- */

/* Load one LE u64 from eight octets (may be unaligned). pB non-NULL. */
static uint64_t
b2788_load(const unsigned char *pB)
{
	return (uint64_t)pB[0]
	    | ((uint64_t)pB[1] << 8)
	    | ((uint64_t)pB[2] << 16)
	    | ((uint64_t)pB[3] << 24)
	    | ((uint64_t)pB[4] << 32)
	    | ((uint64_t)pB[5] << 40)
	    | ((uint64_t)pB[6] << 48)
	    | ((uint64_t)pB[7] << 56);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bytes_to_u64_le_u - assemble eight LE octets into a uint64_t.
 *
 * src: pointer to at least 8 bytes (NULL -> 0)
 *
 * Returns the little-endian interpretation of src[0..7].
 */
uint64_t
gj_bytes_to_u64_le_u(const unsigned char *pSrc)
{
	(void)NULL;
	if (pSrc == NULL) {
		return 0u;
	}
	return b2788_load(pSrc);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bytes_to_u64_le_u(const unsigned char *pSrc)
    __attribute__((alias("gj_bytes_to_u64_le_u")));
