/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2081: unaligned big-endian uint16 store (_u).
 *
 * Surface (unique symbols):
 *   void gj_store_be16_u(uint8_t *p, uint16_t v);
 *     - Store v as 16-bit big-endian into p[0..1]. NULL p is a no-op.
 *       Unaligned-safe octet walk only.
 *   void __gj_store_be16_u  (alias)
 *   __libcgj_batch2081_marker = "libcgj-batch2081"
 *
 * Endian store exclusive wave (2081-2090). Distinct from gj_be16_store
 * (batch754) and gj_store_be32 (batch394) - unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2081_marker[] = "libcgj-batch2081";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b2081_store_be16(uint8_t *pDst, uint16_t u16V)
{
	pDst[0] = (uint8_t)(u16V >> 8);
	pDst[1] = (uint8_t)u16V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_store_be16_u - big-endian uint16 store (unaligned-safe).
 *
 * p: pointer to at least 2 writable octets, or NULL (no-op)
 * v: value to store; p[0] receives the most-significant octet
 */
void
gj_store_be16_u(uint8_t *p, uint16_t v)
{
	(void)NULL;
	if (p == NULL) {
		return;
	}
	b2081_store_be16(p, v);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_store_be16_u(uint8_t *p, uint16_t v)
    __attribute__((alias("gj_store_be16_u")));
