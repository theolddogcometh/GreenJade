/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2082: unaligned big-endian uint32 store (_u).
 *
 * Surface (unique symbols):
 *   void gj_store_be32_u(uint8_t *p, uint32_t v);
 *     - Store v as 32-bit big-endian into p[0..3]. NULL p is a no-op.
 *       Unaligned-safe octet walk only.
 *   void __gj_store_be32_u  (alias)
 *   __libcgj_batch2082_marker = "libcgj-batch2082"
 *
 * Endian store exclusive wave (2081-2090). Distinct from gj_store_be32
 * (batch394) - unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2082_marker[] = "libcgj-batch2082";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b2082_store_be32(uint8_t *pDst, uint32_t u32V)
{
	pDst[0] = (uint8_t)(u32V >> 24);
	pDst[1] = (uint8_t)(u32V >> 16);
	pDst[2] = (uint8_t)(u32V >> 8);
	pDst[3] = (uint8_t)u32V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_store_be32_u - big-endian uint32 store (unaligned-safe).
 *
 * p: pointer to at least 4 writable octets, or NULL (no-op)
 * v: value to store; p[0] receives the most-significant octet
 */
void
gj_store_be32_u(uint8_t *p, uint32_t v)
{
	(void)NULL;
	if (p == NULL) {
		return;
	}
	b2082_store_be32(p, v);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_store_be32_u(uint8_t *p, uint32_t v)
    __attribute__((alias("gj_store_be32_u")));
