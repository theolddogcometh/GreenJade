/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2933: freestanding little-endian uint64 pack
 * (_u continuum).
 *
 * Surface (unique symbols):
 *   void gj_pack_u64_le_u(unsigned char *out, uint64_t v);
 *     - Write v as little-endian into out[0..7]. NULL out -> no-op.
 *       Unaligned-safe (byte stores only).
 *   void __gj_pack_u64_le_u  (alias)
 *   __libcgj_batch2933_marker = "libcgj-batch2933"
 *
 * Pack/mod exclusive wave (2931-2940). Distinct from gj_pack_u64le
 * (batch521) — unique gj_pack_u64_le_u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2933_marker[] = "libcgj-batch2933";

/* ---- freestanding helpers ---------------------------------------------- */

/* Store one LE u64 into eight octets (may be unaligned). pB non-NULL. */
static void
b2933_store(unsigned char *pB, uint64_t u64V)
{
	pB[0] = (unsigned char)(u64V & 0xffu);
	pB[1] = (unsigned char)((u64V >> 8) & 0xffu);
	pB[2] = (unsigned char)((u64V >> 16) & 0xffu);
	pB[3] = (unsigned char)((u64V >> 24) & 0xffu);
	pB[4] = (unsigned char)((u64V >> 32) & 0xffu);
	pB[5] = (unsigned char)((u64V >> 40) & 0xffu);
	pB[6] = (unsigned char)((u64V >> 48) & 0xffu);
	pB[7] = (unsigned char)((u64V >> 56) & 0xffu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pack_u64_le_u - store uint64_t in little-endian byte order.
 *
 * out: destination of at least 8 bytes, or NULL (no-op).
 * v:   value; least-significant byte at out[0], most-significant at out[7].
 */
void
gj_pack_u64_le_u(unsigned char *pOut, uint64_t u64V)
{
	(void)NULL;
	if (pOut == NULL) {
		return;
	}
	b2933_store(pOut, u64V);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_pack_u64_le_u(unsigned char *pOut, uint64_t u64V)
    __attribute__((alias("gj_pack_u64_le_u")));
