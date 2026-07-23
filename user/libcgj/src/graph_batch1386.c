/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1386: unpack a u16 into two u8 halves.
 *
 * Surface (unique symbols):
 *   void gj_u16_unpack_u8x2(uint16_t v, uint8_t *lo, uint8_t *hi);
 *     — Inverse of gj_u16_pack_u8x2: *lo = bits [7:0], *hi = bits
 *       [15:8]. NULL lo or NULL hi is ignored for that half only.
 *   void __gj_u16_unpack_u8x2  (alias)
 *   __libcgj_batch1386_marker = "libcgj-batch1386"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1386_marker[] = "libcgj-batch1386";

/* ---- freestanding helpers ---------------------------------------------- */

/* Low 8 bits of a u16. */
static uint8_t
b1386_lo(uint16_t u16V)
{
	return (uint8_t)u16V;
}

/* High 8 bits of a u16. */
static uint8_t
b1386_hi(uint16_t u16V)
{
	return (uint8_t)(u16V >> 8);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_unpack_u8x2 — split one uint16 into two uint8 halves.
 *
 * u16V: packed word
 * pLo:  receives bits [7:0];  NULL → skip
 * pHi:  receives bits [15:8]; NULL → skip
 */
void
gj_u16_unpack_u8x2(uint16_t u16V, uint8_t *pLo, uint8_t *pHi)
{
	if (pLo != NULL) {
		*pLo = b1386_lo(u16V);
	}
	if (pHi != NULL) {
		*pHi = b1386_hi(u16V);
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u16_unpack_u8x2(uint16_t u16V, uint8_t *pLo, uint8_t *pHi)
    __attribute__((alias("gj_u16_unpack_u8x2")));
