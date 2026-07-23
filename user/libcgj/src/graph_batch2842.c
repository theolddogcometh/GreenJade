/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2842: unaligned big-endian uint16 store (_u).
 *
 * Surface (unique symbols):
 *   void gj_be16_store_u(void *p, uint16_t v);
 *     - Write v as big-endian uint16_t into 2 octets at p (unaligned-
 *       safe, octet-wise). p == NULL -> no-op.
 *   void __gj_be16_store_u  (alias)
 *   __libcgj_batch2842_marker = "libcgj-batch2842"
 *
 * Endian/ring exclusive wave (2841-2850). Unique _u surface; no multi-def.
 * Pairs with gj_le32_load_u (batch2841) and sibling store helpers.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2842_marker[] = "libcgj-batch2842";

/* ---- freestanding helpers ---------------------------------------------- */

/* Unaligned BE store of 2 octets; p must be non-NULL. */
static void
b2842_be16(unsigned char *p, uint16_t u16V)
{
	p[0] = (unsigned char)(u16V >> 8);
	p[1] = (unsigned char)u16V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_be16_store_u - store big-endian uint16_t to unaligned memory.
 *
 * p: destination of at least 2 writable octets (may be NULL)
 * v: value to encode
 *
 * NULL p is a no-op. Does not require natural alignment; no libc.
 */
void
gj_be16_store_u(void *p, uint16_t u16V)
{
	unsigned char *pOct;

	(void)NULL;
	if (p == NULL) {
		return;
	}
	pOct = (unsigned char *)p;
	b2842_be16(pOct, u16V);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_be16_store_u(void *p, uint16_t u16V)
    __attribute__((alias("gj_be16_store_u")));
