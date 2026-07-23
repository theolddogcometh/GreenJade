/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2838: big-endian uint32 load from four octets (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_be32_load_u(const unsigned char *p);
 *     - Load 32-bit big-endian value from p[0..3]. NULL -> 0.
 *       Unaligned-safe octet walk only.
 *   uint32_t __gj_be32_load_u  (alias)
 *   __libcgj_batch2838_marker = "libcgj-batch2838"
 *
 * IPv4/endian exclusive wave (2831-2840). Distinct from gj_be32_load
 * (batch748), gj_be32_load_unaligned (batch1494), and gj_unpack_u32be
 * (batch514) — unique be32_load_u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2838_marker[] = "libcgj-batch2838";

/* ---- freestanding helpers ---------------------------------------------- */

/* Big-endian u32 load; p non-NULL, may be unaligned. */
static uint32_t
b2838_load(const unsigned char *p)
{
	return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
	       ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_be32_load_u - big-endian load of a 32-bit unsigned value.
 *
 * p: pointer to at least 4 octets, or NULL (returns 0).
 * Layout: p[0] is the most-significant octet.
 */
uint32_t
gj_be32_load_u(const unsigned char *p)
{
	(void)NULL;
	if (p == NULL) {
		return 0u;
	}
	return b2838_load(p);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_be32_load_u(const unsigned char *p)
    __attribute__((alias("gj_be32_load_u")));
