/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2792: big-endian bytes → uint64_t (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bytes_to_u64_be_u(const unsigned char *p);
 *     - Load one big-endian uint64_t from eight octets at p (unaligned-safe).
 *       NULL → 0.
 *   uint64_t __gj_bytes_to_u64_be_u  (alias)
 *   __libcgj_batch2792_marker = "libcgj-batch2792"
 *
 * Milestone 2800 exclusive byte/hex helpers (2791-2800). Unique
 * gj_bytes_to_u64_be_u surface only; no multi-def. Distinct from
 * gj_load_be64 (batch394), gj_bytes_to_u32_be_u (batch2791), and
 * gj_u64_to_bytes_be_u (batch2794 inverse).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2792_marker[] = "libcgj-batch2792";

/* ---- freestanding helpers ---------------------------------------------- */

/* Load one BE u64 from eight octets (may be unaligned). NULL → 0. */
static uint64_t
b2792_load(const unsigned char *pB)
{
	if (pB == NULL) {
		return 0u;
	}
	return ((uint64_t)pB[0] << 56)
	    | ((uint64_t)pB[1] << 48)
	    | ((uint64_t)pB[2] << 40)
	    | ((uint64_t)pB[3] << 32)
	    | ((uint64_t)pB[4] << 24)
	    | ((uint64_t)pB[5] << 16)
	    | ((uint64_t)pB[6] << 8)
	    | (uint64_t)pB[7];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bytes_to_u64_be_u - big-endian uint64 load from eight octets.
 *
 * p: pointer to at least 8 readable bytes (NULL → 0).
 *
 * Returns the host-order uint64_t value. Unaligned-safe pure byte
 * assembly. Does not call libc.
 */
uint64_t
gj_bytes_to_u64_be_u(const unsigned char *pB)
{
	(void)NULL;
	return b2792_load(pB);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bytes_to_u64_be_u(const unsigned char *pB)
    __attribute__((alias("gj_bytes_to_u64_be_u")));
