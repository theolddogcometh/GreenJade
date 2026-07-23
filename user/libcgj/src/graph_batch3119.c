/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3119: unpack one u32 into two u16 values (_u).
 *
 * Surface (unique symbols):
 *   void gj_u16_unpack2_u(uint32_t v, uint16_t *lo, uint16_t *hi);
 *     - Inverse of gj_u16_pack2_u: *lo = bits [15:0], *hi = bits [31:16].
 *       NULL lo or NULL hi is ignored for that half only.
 *   void __gj_u16_unpack2_u  (alias)
 *   __libcgj_batch3119_marker = "libcgj-batch3119"
 *
 * Exclusive continuum CREATE-ONLY (3111-3120). Distinct from
 * gj_u32_unpack_u16x2 — unique gj_u16_unpack2_u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3119_marker[] = "libcgj-batch3119";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b3119_lo(uint32_t u32V)
{
	return (uint16_t)u32V;
}

static uint16_t
b3119_hi(uint32_t u32V)
{
	return (uint16_t)(u32V >> 16);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_unpack2_u - split one uint32 into two uint16 halves.
 *
 * u32V: packed word
 * pLo:  receives bits [15:0];  NULL → skip
 * pHi:  receives bits [31:16]; NULL → skip
 */
void
gj_u16_unpack2_u(uint32_t u32V, uint16_t *pLo, uint16_t *pHi)
{
	(void)NULL;
	if (pLo != NULL) {
		*pLo = b3119_lo(u32V);
	}
	if (pHi != NULL) {
		*pHi = b3119_hi(u32V);
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u16_unpack2_u(uint32_t u32V, uint16_t *pLo, uint16_t *pHi)
    __attribute__((alias("gj_u16_unpack2_u")));
