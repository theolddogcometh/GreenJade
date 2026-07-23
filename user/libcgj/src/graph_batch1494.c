/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1494: unaligned big-endian uint32 load.
 *
 * Surface (unique symbols):
 *   uint32_t gj_be32_load_unaligned(const unsigned char *p);
 *     — Load a 32-bit big-endian value from p[0..3] without requiring
 *       natural alignment. NULL → 0.
 *   uint32_t __gj_be32_load_unaligned  (alias)
 *   __libcgj_batch1494_marker = "libcgj-batch1494"
 *
 * Distinct from gj_be32_load (batch748) and gj_load_be32 (batch394) —
 * explicit unaligned surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1494_marker[] = "libcgj-batch1494";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_be32_load_unaligned — big-endian uint32 load (unaligned-safe).
 *
 * p: pointer to at least 4 readable octets, or NULL (returns 0).
 * Layout: p[0] is the most-significant byte, p[3] the least.
 */
uint32_t
gj_be32_load_unaligned(const unsigned char *p)
{
	if (p == NULL) {
		return 0u;
	}
	return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
	    ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_be32_load_unaligned(const unsigned char *p)
    __attribute__((alias("gj_be32_load_unaligned")));
