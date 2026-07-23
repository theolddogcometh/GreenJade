/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2843: unaligned big-endian uint32 store (_u).
 *
 * Surface (unique symbols):
 *   void gj_be32_store_u(void *p, uint32_t v);
 *     - Write v as big-endian uint32_t into 4 octets at p (unaligned-
 *       safe, octet-wise). p == NULL -> no-op.
 *   void __gj_be32_store_u  (alias)
 *   __libcgj_batch2843_marker = "libcgj-batch2843"
 *
 * Endian/ring exclusive wave (2841-2850). Unique _u surface; no multi-def.
 * Distinct from htons/htonl (host order) and LE store siblings.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2843_marker[] = "libcgj-batch2843";

/* ---- freestanding helpers ---------------------------------------------- */

/* Unaligned BE store of 4 octets; p must be non-NULL. */
static void
b2843_be32(unsigned char *p, uint32_t u32V)
{
	p[0] = (unsigned char)(u32V >> 24);
	p[1] = (unsigned char)(u32V >> 16);
	p[2] = (unsigned char)(u32V >> 8);
	p[3] = (unsigned char)u32V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_be32_store_u - store big-endian uint32_t to unaligned memory.
 *
 * p: destination of at least 4 writable octets (may be NULL)
 * v: value to encode
 *
 * NULL p is a no-op. Does not require natural alignment; no libc.
 */
void
gj_be32_store_u(void *p, uint32_t u32V)
{
	unsigned char *pOct;

	(void)NULL;
	if (p == NULL) {
		return;
	}
	pOct = (unsigned char *)p;
	b2843_be32(pOct, u32V);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_be32_store_u(void *p, uint32_t u32V)
    __attribute__((alias("gj_be32_store_u")));
