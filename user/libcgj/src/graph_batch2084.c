/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2084: unaligned little-endian uint16 store (_u).
 *
 * Surface (unique symbols):
 *   void gj_store_le16_u(uint8_t *p, uint16_t v);
 *     - Store v as 16-bit little-endian into p[0..1]. NULL p is a no-op.
 *       Unaligned-safe octet walk only.
 *   void __gj_store_le16_u  (alias)
 *   __libcgj_batch2084_marker = "libcgj-batch2084"
 *
 * Endian store exclusive wave (2081-2090). Distinct from gj_le16_store
 * (batch756) - unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2084_marker[] = "libcgj-batch2084";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b2084_store_le16(uint8_t *pDst, uint16_t u16V)
{
	pDst[0] = (uint8_t)u16V;
	pDst[1] = (uint8_t)(u16V >> 8);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_store_le16_u - little-endian uint16 store (unaligned-safe).
 *
 * p: pointer to at least 2 writable octets, or NULL (no-op)
 * v: value to store; p[0] receives the least-significant octet
 */
void
gj_store_le16_u(uint8_t *p, uint16_t v)
{
	(void)NULL;
	if (p == NULL) {
		return;
	}
	b2084_store_le16(p, v);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_store_le16_u(uint8_t *p, uint16_t v)
    __attribute__((alias("gj_store_le16_u")));
