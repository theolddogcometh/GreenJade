/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4916: freestanding UUID copy (u8).
 *
 * Surface (unique symbols):
 *   int gj_uuid_copy_u8(uint8_t dst[16], const uint8_t src[16]);
 *     - Copy 16 octets from src into dst. Either NULL -> no-op return 0.
 *       Same pointer -> return 1 (already equal). Success -> 1.
 *   int __gj_uuid_copy_u8  (alias)
 *   __libcgj_batch4916_marker = "libcgj-batch4916"
 *
 * Exclusive continuum CREATE-ONLY UUID v4 bits wave (4911-4920).
 * Distinct from gj_uuid_copy_u (batch2577, void) and uuid_copy
 * (batch36) — unique gj_uuid_copy_u8 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4916_marker[] = "libcgj-batch4916";

/* ---- freestanding helpers ---------------------------------------------- */

/* Copy 16 octets. pDst and pSrc must be non-NULL and distinct. */
static void
b4916_copy(uint8_t *pDst, const uint8_t *pSrc)
{
	unsigned i;

	for (i = 0u; i < 16u; i++) {
		pDst[i] = pSrc[i];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_copy_u8 - copy a 16-octet UUID from src to dst.
 *
 * dst: destination 16-octet UUID (NULL -> no-op, return 0)
 * src: source 16-octet UUID (NULL -> no-op, return 0)
 *
 * Does not call libc memcpy. Same-pointer is success (return 1).
 * No parent wires.
 */
int
gj_uuid_copy_u8(uint8_t dst[16], const uint8_t src[16])
{
	(void)NULL;
	if (dst == NULL || src == NULL) {
		return 0;
	}
	if (dst == src) {
		return 1;
	}
	b4916_copy(dst, src);
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_uuid_copy_u8(uint8_t dst[16], const uint8_t src[16])
    __attribute__((alias("gj_uuid_copy_u8")));
