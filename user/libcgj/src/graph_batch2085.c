/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2085: unaligned little-endian uint32 store (_u).
 *
 * Surface (unique symbols):
 *   void gj_store_le32_u(uint8_t *p, uint32_t v);
 *     - Store v as 32-bit little-endian into p[0..3]. NULL p is a no-op.
 *       Unaligned-safe octet walk only.
 *   void __gj_store_le32_u  (alias)
 *   __libcgj_batch2085_marker = "libcgj-batch2085"
 *
 * Endian store exclusive wave (2081-2090). Distinct from gj_store_le32
 * (batch395) - unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2085_marker[] = "libcgj-batch2085";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b2085_store_le32(uint8_t *pDst, uint32_t u32V)
{
	pDst[0] = (uint8_t)u32V;
	pDst[1] = (uint8_t)(u32V >> 8);
	pDst[2] = (uint8_t)(u32V >> 16);
	pDst[3] = (uint8_t)(u32V >> 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_store_le32_u - little-endian uint32 store (unaligned-safe).
 *
 * p: pointer to at least 4 writable octets, or NULL (no-op)
 * v: value to store; p[0] receives the least-significant octet
 */
void
gj_store_le32_u(uint8_t *p, uint32_t v)
{
	(void)NULL;
	if (p == NULL) {
		return;
	}
	b2085_store_le32(p, v);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_store_le32_u(uint8_t *p, uint32_t v)
    __attribute__((alias("gj_store_le32_u")));
