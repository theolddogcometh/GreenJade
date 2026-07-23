/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1496: unaligned little-endian uint32 load.
 *
 * Surface (unique symbols):
 *   uint32_t gj_le32_load_unaligned(const unsigned char *p);
 *     — Load a 32-bit little-endian value from p[0..3] without requiring
 *       natural alignment. NULL → 0.
 *   uint32_t __gj_le32_load_unaligned  (alias)
 *   __libcgj_batch1496_marker = "libcgj-batch1496"
 *
 * Distinct from gj_le32_load (batch751) and gj_load_le32 (batch395) —
 * explicit unaligned surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1496_marker[] = "libcgj-batch1496";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_le32_load_unaligned — little-endian uint32 load (unaligned-safe).
 *
 * p: pointer to at least 4 readable octets, or NULL (returns 0).
 * Layout: p[0] is the least-significant byte, p[3] the most.
 */
uint32_t
gj_le32_load_unaligned(const unsigned char *p)
{
	if (p == NULL) {
		return 0u;
	}
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
	    ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_le32_load_unaligned(const unsigned char *p)
    __attribute__((alias("gj_le32_load_unaligned")));
