/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1384: unpack a u32 into two u16 halves.
 *
 * Surface (unique symbols):
 *   void gj_u32_unpack_u16x2(uint32_t v, uint16_t *lo, uint16_t *hi);
 *     — Inverse of gj_u32_pack_u16x2: *lo = bits [15:0], *hi = bits
 *       [31:16]. NULL lo or NULL hi is ignored for that half only.
 *   void __gj_u32_unpack_u16x2  (alias)
 *   __libcgj_batch1384_marker = "libcgj-batch1384"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1384_marker[] = "libcgj-batch1384";

/* ---- freestanding helpers ---------------------------------------------- */

/* Low 16 bits of a u32. */
static uint16_t
b1384_lo(uint32_t u32V)
{
	return (uint16_t)u32V;
}

/* High 16 bits of a u32. */
static uint16_t
b1384_hi(uint32_t u32V)
{
	return (uint16_t)(u32V >> 16);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_unpack_u16x2 — split one uint32 into two uint16 halves.
 *
 * u32V: packed word
 * pLo:  receives bits [15:0];  NULL → skip
 * pHi:  receives bits [31:16]; NULL → skip
 */
void
gj_u32_unpack_u16x2(uint32_t u32V, uint16_t *pLo, uint16_t *pHi)
{
	if (pLo != NULL) {
		*pLo = b1384_lo(u32V);
	}
	if (pHi != NULL) {
		*pHi = b1384_hi(u32V);
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_unpack_u16x2(uint32_t u32V, uint16_t *pLo, uint16_t *pHi)
    __attribute__((alias("gj_u32_unpack_u16x2")));
