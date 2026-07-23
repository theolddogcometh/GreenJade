/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1498: unaligned little-endian uint64 load.
 *
 * Surface (unique symbols):
 *   uint64_t gj_le64_load_unaligned(const unsigned char *p);
 *     — Load a 64-bit little-endian value from p[0..7] without requiring
 *       natural alignment. NULL → 0.
 *   uint64_t __gj_le64_load_unaligned  (alias)
 *   __libcgj_batch1498_marker = "libcgj-batch1498"
 *
 * Distinct from gj_le64_load (batch753) and gj_load_le64 (batch395) —
 * explicit unaligned surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1498_marker[] = "libcgj-batch1498";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_le64_load_unaligned — little-endian uint64 load (unaligned-safe).
 *
 * p: pointer to at least 8 readable octets, or NULL (returns 0).
 * Layout: p[0] is the least-significant byte, p[7] the most.
 */
uint64_t
gj_le64_load_unaligned(const unsigned char *p)
{
	if (p == NULL) {
		return 0ULL;
	}
	return (uint64_t)p[0] | ((uint64_t)p[1] << 8) |
	    ((uint64_t)p[2] << 16) | ((uint64_t)p[3] << 24) |
	    ((uint64_t)p[4] << 32) | ((uint64_t)p[5] << 40) |
	    ((uint64_t)p[6] << 48) | ((uint64_t)p[7] << 56);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_le64_load_unaligned(const unsigned char *p)
    __attribute__((alias("gj_le64_load_unaligned")));
