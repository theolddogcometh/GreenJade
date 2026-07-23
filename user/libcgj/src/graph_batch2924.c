/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2924: freestanding little-endian uint16 pack (_u).
 *
 * Surface (unique symbols):
 *   void gj_pack_u16_le_u(unsigned char *out, uint16_t v);
 *     - Write v as little-endian into out[0..1]. NULL out → no-op.
 *   void __gj_pack_u16_le_u  (alias)
 *   __libcgj_batch2924_marker = "libcgj-batch2924"
 *
 * Exclusive continuum CREATE-ONLY (2921-2930). Distinct from
 * gj_pack_u16le (batch517) — unique gj_pack_u16_le_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2924_marker[] = "libcgj-batch2924";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2924_store_le16 - write little-endian u16 into two bytes.
 */
static void
b2924_store_le16(unsigned char *pOut, uint16_t u16V)
{
	pOut[0] = (unsigned char)(u16V & 0xffu);
	pOut[1] = (unsigned char)((u16V >> 8) & 0xffu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pack_u16_le_u - store uint16_t in little-endian byte order.
 *
 * out: destination of at least 2 bytes, or NULL (no-op).
 * v:   value; low byte at out[0], high byte at out[1].
 */
void
gj_pack_u16_le_u(unsigned char *pOut, uint16_t u16V)
{
	(void)NULL;
	if (pOut == NULL) {
		return;
	}
	b2924_store_le16(pOut, u16V);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_pack_u16_le_u(unsigned char *pOut, uint16_t u16V)
    __attribute__((alias("gj_pack_u16_le_u")));
