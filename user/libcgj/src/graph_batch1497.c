/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1497: unaligned big-endian uint64 load.
 *
 * Surface (unique symbols):
 *   uint64_t gj_be64_load_unaligned(const unsigned char *p);
 *     — Load a 64-bit big-endian value from p[0..7] without requiring
 *       natural alignment. NULL → 0.
 *   uint64_t __gj_be64_load_unaligned  (alias)
 *   __libcgj_batch1497_marker = "libcgj-batch1497"
 *
 * Distinct from gj_be64_load (batch752) and gj_load_be64 (batch394) —
 * explicit unaligned surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1497_marker[] = "libcgj-batch1497";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_be64_load_unaligned — big-endian uint64 load (unaligned-safe).
 *
 * p: pointer to at least 8 readable octets, or NULL (returns 0).
 * Layout: p[0] is the most-significant byte, p[7] the least.
 */
uint64_t
gj_be64_load_unaligned(const unsigned char *p)
{
	if (p == NULL) {
		return 0ULL;
	}
	return ((uint64_t)p[0] << 56) | ((uint64_t)p[1] << 48) |
	    ((uint64_t)p[2] << 40) | ((uint64_t)p[3] << 32) |
	    ((uint64_t)p[4] << 24) | ((uint64_t)p[5] << 16) |
	    ((uint64_t)p[6] << 8) | (uint64_t)p[7];
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_be64_load_unaligned(const unsigned char *p)
    __attribute__((alias("gj_be64_load_unaligned")));
