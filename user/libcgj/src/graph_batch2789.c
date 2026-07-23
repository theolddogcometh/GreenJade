/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2789: store one uint32_t as four little-endian
 * octets (_u).
 *
 * Surface (unique symbols):
 *   void gj_u32_to_bytes_le_u(uint32_t v, unsigned char *dst);
 *     - Write v into dst[0..3] little-endian. No-op if dst is NULL.
 *       Unaligned dst is fine (byte-wise store).
 *   void __gj_u32_to_bytes_le_u  (alias)
 *   __libcgj_batch2789_marker = "libcgj-batch2789"
 *
 * Integer-root exclusive wave (2781-2790). Inverse of
 * gj_bytes_to_u32_le_u (batch2787) — unique store _u surface; no
 * multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2789_marker[] = "libcgj-batch2789";

/* ---- freestanding helpers ---------------------------------------------- */

/* Store one LE u32 into four octets (may be unaligned). pB non-NULL. */
static void
b2789_store(uint32_t u32V, unsigned char *pB)
{
	pB[0] = (unsigned char)(u32V & 0xffu);
	pB[1] = (unsigned char)((u32V >> 8) & 0xffu);
	pB[2] = (unsigned char)((u32V >> 16) & 0xffu);
	pB[3] = (unsigned char)((u32V >> 24) & 0xffu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_to_bytes_le_u - write v as four LE octets into dst.
 *
 * v:   value to store
 * dst: destination of at least 4 bytes (NULL -> no-op)
 */
void
gj_u32_to_bytes_le_u(uint32_t u32V, unsigned char *pDst)
{
	(void)NULL;
	if (pDst == NULL) {
		return;
	}
	b2789_store(u32V, pDst);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_to_bytes_le_u(uint32_t u32V, unsigned char *pDst)
    __attribute__((alias("gj_u32_to_bytes_le_u")));
