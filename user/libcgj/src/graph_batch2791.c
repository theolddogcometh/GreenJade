/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2791: big-endian bytes → uint32_t (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bytes_to_u32_be_u(const unsigned char *p);
 *     - Load one big-endian uint32_t from four octets at p (unaligned-safe).
 *       NULL → 0.
 *   uint32_t __gj_bytes_to_u32_be_u  (alias)
 *   __libcgj_batch2791_marker = "libcgj-batch2791"
 *
 * Milestone 2800 exclusive byte/hex helpers (2791-2800). Unique
 * gj_bytes_to_u32_be_u surface only; no multi-def. Distinct from
 * gj_load_be32 (batch394), gj_bytes_to_u32_le_arr (batch1519), and
 * gj_bytes_to_u64_be_u (batch2792).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2791_marker[] = "libcgj-batch2791";

/* ---- freestanding helpers ---------------------------------------------- */

/* Load one BE u32 from four octets (may be unaligned). NULL → 0. */
static uint32_t
b2791_load(const unsigned char *pB)
{
	if (pB == NULL) {
		return 0u;
	}
	return ((uint32_t)pB[0] << 24)
	    | ((uint32_t)pB[1] << 16)
	    | ((uint32_t)pB[2] << 8)
	    | (uint32_t)pB[3];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bytes_to_u32_be_u - big-endian uint32 load from four octets.
 *
 * p: pointer to at least 4 readable bytes (NULL → 0).
 *
 * Returns the host-order uint32_t value. Unaligned-safe pure byte
 * assembly. Does not call libc.
 */
uint32_t
gj_bytes_to_u32_be_u(const unsigned char *pB)
{
	(void)NULL;
	return b2791_load(pB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bytes_to_u32_be_u(const unsigned char *pB)
    __attribute__((alias("gj_bytes_to_u32_be_u")));
