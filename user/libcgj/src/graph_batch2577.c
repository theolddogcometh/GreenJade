/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2577: freestanding UUID copy (_u).
 *
 * Surface (unique symbols):
 *   void gj_uuid_copy_u(unsigned char dst[16], const unsigned char src[16]);
 *     - Copy 16 octets from src into dst. Either NULL -> no-op.
 *       Same pointer -> no-op (already equal).
 *   void __gj_uuid_copy_u  (alias)
 *   __libcgj_batch2577_marker = "libcgj-batch2577"
 *
 * UUID exclusive wave (2571-2580). Distinct from uuid_copy (batch36)
 * which uses memcpy — this TU is freestanding pure C; unique gj_*_u
 * name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2577_marker[] = "libcgj-batch2577";

/* ---- freestanding helpers ---------------------------------------------- */

/* Copy 16 octets. pDst and pSrc must be non-NULL and distinct. */
static void
b2577_copy(unsigned char *pDst, const unsigned char *pSrc)
{
	unsigned i;

	for (i = 0u; i < 16u; i++) {
		pDst[i] = pSrc[i];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_copy_u - copy a 16-octet UUID from src to dst.
 *
 * dst: destination 16-octet UUID (NULL -> no-op)
 * src: source 16-octet UUID (NULL -> no-op)
 *
 * Does not call libc memcpy. Overlapping same-pointer is a no-op.
 */
void
gj_uuid_copy_u(unsigned char pDst[16], const unsigned char pSrc[16])
{
	(void)NULL;
	if (pDst == NULL || pSrc == NULL) {
		return;
	}
	if (pDst == pSrc) {
		return;
	}
	b2577_copy(pDst, pSrc);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_uuid_copy_u(unsigned char pDst[16], const unsigned char pSrc[16])
    __attribute__((alias("gj_uuid_copy_u")));
