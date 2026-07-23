/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2844: unaligned little-endian uint16 store (_u).
 *
 * Surface (unique symbols):
 *   void gj_le16_store_u(void *p, uint16_t v);
 *     - Write v as little-endian uint16_t into 2 octets at p (unaligned-
 *       safe, octet-wise). p == NULL -> no-op.
 *   void __gj_le16_store_u  (alias)
 *   __libcgj_batch2844_marker = "libcgj-batch2844"
 *
 * Endian/ring exclusive wave (2841-2850). Unique _u surface; no multi-def.
 * Pairs with gj_be16_store_u (batch2842) and LE load/store siblings.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2844_marker[] = "libcgj-batch2844";

/* ---- freestanding helpers ---------------------------------------------- */

/* Unaligned LE store of 2 octets; p must be non-NULL. */
static void
b2844_le16(unsigned char *p, uint16_t u16V)
{
	p[0] = (unsigned char)u16V;
	p[1] = (unsigned char)(u16V >> 8);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_le16_store_u - store little-endian uint16_t to unaligned memory.
 *
 * p: destination of at least 2 writable octets (may be NULL)
 * v: value to encode
 *
 * NULL p is a no-op. Does not require natural alignment; no libc.
 */
void
gj_le16_store_u(void *p, uint16_t u16V)
{
	unsigned char *pOct;

	(void)NULL;
	if (p == NULL) {
		return;
	}
	pOct = (unsigned char *)p;
	b2844_le16(pOct, u16V);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_le16_store_u(void *p, uint16_t u16V)
    __attribute__((alias("gj_le16_store_u")));
