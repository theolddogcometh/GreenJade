/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2787: load one little-endian uint32_t from
 * four octets (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bytes_to_u32_le_u(const unsigned char *src);
 *     - Assemble src[0..3] as a little-endian uint32_t. Returns 0 if
 *       src is NULL. Unaligned src is fine (byte-wise load).
 *   uint32_t __gj_bytes_to_u32_le_u  (alias)
 *   __libcgj_batch2787_marker = "libcgj-batch2787"
 *
 * Integer-root exclusive wave (2781-2790). Distinct from
 * gj_bytes_to_u32_le_arr (batch1519) — single-value _u surface; no
 * multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2787_marker[] = "libcgj-batch2787";

/* ---- freestanding helpers ---------------------------------------------- */

/* Load one LE u32 from four octets (may be unaligned). pB non-NULL. */
static uint32_t
b2787_load(const unsigned char *pB)
{
	return (uint32_t)pB[0]
	    | ((uint32_t)pB[1] << 8)
	    | ((uint32_t)pB[2] << 16)
	    | ((uint32_t)pB[3] << 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bytes_to_u32_le_u - assemble four LE octets into a uint32_t.
 *
 * src: pointer to at least 4 bytes (NULL -> 0)
 *
 * Returns the little-endian interpretation of src[0..3].
 */
uint32_t
gj_bytes_to_u32_le_u(const unsigned char *pSrc)
{
	(void)NULL;
	if (pSrc == NULL) {
		return 0u;
	}
	return b2787_load(pSrc);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bytes_to_u32_le_u(const unsigned char *pSrc)
    __attribute__((alias("gj_bytes_to_u32_le_u")));
