/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2841: unaligned little-endian uint32 load (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_le32_load_u(const void *p);
 *     - Read 4 octets at p as little-endian uint32_t (unaligned-safe,
 *       octet-wise). p == NULL -> 0.
 *   uint32_t __gj_le32_load_u  (alias)
 *   __libcgj_batch2841_marker = "libcgj-batch2841"
 *
 * Endian/ring exclusive wave (2841-2850). Unique _u surface; no multi-def.
 * Distinct from internal b56/b63 helpers (not exported) and host htonl.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2841_marker[] = "libcgj-batch2841";

/* ---- freestanding helpers ---------------------------------------------- */

/* Unaligned LE load of 4 octets; p must be non-NULL. */
static uint32_t
b2841_le32(const unsigned char *p)
{
	return (uint32_t)p[0]
	    | ((uint32_t)p[1] << 8)
	    | ((uint32_t)p[2] << 16)
	    | ((uint32_t)p[3] << 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_le32_load_u - load little-endian uint32_t from unaligned memory.
 *
 * p: pointer to at least 4 readable octets (may be NULL)
 *
 * Returns the LE-decoded value, or 0 when p is NULL.
 * Does not require natural alignment; does not call libc.
 */
uint32_t
gj_le32_load_u(const void *p)
{
	const unsigned char *pOct;

	(void)NULL;
	if (p == NULL) {
		return 0u;
	}
	pOct = (const unsigned char *)p;
	return b2841_le32(pOct);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_le32_load_u(const void *p)
    __attribute__((alias("gj_le32_load_u")));
