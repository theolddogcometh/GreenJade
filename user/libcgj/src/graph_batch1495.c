/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1495: unaligned little-endian uint16 load.
 *
 * Surface (unique symbols):
 *   uint16_t gj_le16_load_unaligned(const unsigned char *p);
 *     — Load a 16-bit little-endian value from p[0..1] without requiring
 *       natural alignment. NULL → 0.
 *   uint16_t __gj_le16_load_unaligned  (alias)
 *   __libcgj_batch1495_marker = "libcgj-batch1495"
 *
 * Distinct from gj_le16_load (batch749) — explicit unaligned surface;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1495_marker[] = "libcgj-batch1495";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_le16_load_unaligned — little-endian uint16 load (unaligned-safe).
 *
 * p: pointer to at least 2 readable octets, or NULL (returns 0).
 * Layout: p[0] is the least-significant byte.
 */
uint16_t
gj_le16_load_unaligned(const unsigned char *p)
{
	if (p == NULL) {
		return 0u;
	}
	return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_le16_load_unaligned(const unsigned char *p)
    __attribute__((alias("gj_le16_load_unaligned")));
