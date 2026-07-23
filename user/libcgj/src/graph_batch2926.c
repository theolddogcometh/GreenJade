/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2926: freestanding little-endian uint32 pack (_u).
 *
 * Surface (unique symbols):
 *   void gj_pack_u32_le_u(unsigned char *out, uint32_t v);
 *     - Write v as little-endian into out[0..3]. NULL out → no-op.
 *   void __gj_pack_u32_le_u  (alias)
 *   __libcgj_batch2926_marker = "libcgj-batch2926"
 *
 * Exclusive continuum CREATE-ONLY (2921-2930). Distinct from
 * gj_pack_u32le (batch519) — unique gj_pack_u32_le_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2926_marker[] = "libcgj-batch2926";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2926_store_le32 - write little-endian u32 into four bytes.
 */
static void
b2926_store_le32(unsigned char *pOut, uint32_t u32V)
{
	pOut[0] = (unsigned char)(u32V & 0xffu);
	pOut[1] = (unsigned char)((u32V >> 8) & 0xffu);
	pOut[2] = (unsigned char)((u32V >> 16) & 0xffu);
	pOut[3] = (unsigned char)((u32V >> 24) & 0xffu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pack_u32_le_u - store uint32_t in little-endian byte order.
 *
 * out: destination of at least 4 bytes, or NULL (no-op).
 * v:   value; low byte at out[0], high byte at out[3].
 */
void
gj_pack_u32_le_u(unsigned char *pOut, uint32_t u32V)
{
	(void)NULL;
	if (pOut == NULL) {
		return;
	}
	b2926_store_le32(pOut, u32V);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_pack_u32_le_u(unsigned char *pOut, uint32_t u32V)
    __attribute__((alias("gj_pack_u32_le_u")));
