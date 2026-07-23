/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2839: little-endian uint16 load from two octets (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_le16_load_u(const unsigned char *p);
 *     - Load 16-bit little-endian value from p[0..1]. NULL -> 0.
 *       Unaligned-safe octet walk only.
 *   uint16_t __gj_le16_load_u  (alias)
 *   __libcgj_batch2839_marker = "libcgj-batch2839"
 *
 * IPv4/endian exclusive wave (2831-2840). Distinct from gj_le16_load
 * (batch749), gj_le16_load_unaligned (batch1495), and gj_unpack_u16le
 * (batch518) — unique le16_load_u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2839_marker[] = "libcgj-batch2839";

/* ---- freestanding helpers ---------------------------------------------- */

/* Little-endian u16 load; p non-NULL, may be unaligned. */
static uint16_t
b2839_load(const unsigned char *p)
{
	return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_le16_load_u - little-endian load of a 16-bit unsigned value.
 *
 * p: pointer to at least 2 octets, or NULL (returns 0).
 * Layout: p[0] is the least-significant octet.
 */
uint16_t
gj_le16_load_u(const unsigned char *p)
{
	(void)NULL;
	if (p == NULL) {
		return 0u;
	}
	return b2839_load(p);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_le16_load_u(const unsigned char *p)
    __attribute__((alias("gj_le16_load_u")));
