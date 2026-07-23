/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2925: freestanding big-endian uint32 pack (_u).
 *
 * Surface (unique symbols):
 *   void gj_pack_u32_be_u(unsigned char *out, uint32_t v);
 *     - Write v as big-endian into out[0..3]. NULL out → no-op.
 *   void __gj_pack_u32_be_u  (alias)
 *   __libcgj_batch2925_marker = "libcgj-batch2925"
 *
 * Exclusive continuum CREATE-ONLY (2921-2930). Distinct from
 * gj_pack_u32be (batch513) — unique gj_pack_u32_be_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2925_marker[] = "libcgj-batch2925";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2925_store_be32 - write big-endian u32 into four bytes.
 */
static void
b2925_store_be32(unsigned char *pOut, uint32_t u32V)
{
	pOut[0] = (unsigned char)((u32V >> 24) & 0xffu);
	pOut[1] = (unsigned char)((u32V >> 16) & 0xffu);
	pOut[2] = (unsigned char)((u32V >> 8) & 0xffu);
	pOut[3] = (unsigned char)(u32V & 0xffu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pack_u32_be_u - store uint32_t in network / big-endian byte order.
 *
 * out: destination of at least 4 bytes, or NULL (no-op).
 * v:   value; MSB at out[0], LSB at out[3].
 */
void
gj_pack_u32_be_u(unsigned char *pOut, uint32_t u32V)
{
	(void)NULL;
	if (pOut == NULL) {
		return;
	}
	b2925_store_be32(pOut, u32V);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_pack_u32_be_u(unsigned char *pOut, uint32_t u32V)
    __attribute__((alias("gj_pack_u32_be_u")));
