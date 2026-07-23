/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1684: unpack 0xAARRGGBB into R,G,B,A channels.
 *
 * Surface (unique symbols):
 *   void gj_rgba_unpack_u32(uint32_t packed, uint8_t *r, uint8_t *g,
 *                           uint8_t *b, uint8_t *a);
 *     — Inverse of gj_rgba_pack_u32. NULL out-pointers are skipped.
 *   void __gj_rgba_unpack_u32  (alias)
 *   __libcgj_batch1684_marker = "libcgj-batch1684"
 *
 * Color / pixel exclusive integer wave. Distinct from gj_rgb_unpack_u32
 * (batch1682). Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1684_marker[] = "libcgj-batch1684";

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b1684_byte(uint32_t u32V, unsigned uShift)
{
	return (uint8_t)((u32V >> uShift) & 0xffu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rgba_unpack_u32 — split 0xAARRGGBB into four 8-bit channels.
 *
 * u32Packed: pixel word
 * pR / pG / pB / pA: optional outs; NULL → skip that channel
 */
void
gj_rgba_unpack_u32(uint32_t u32Packed, uint8_t *pR, uint8_t *pG, uint8_t *pB,
    uint8_t *pA)
{
	if (pR != NULL) {
		*pR = b1684_byte(u32Packed, 16u);
	}
	if (pG != NULL) {
		*pG = b1684_byte(u32Packed, 8u);
	}
	if (pB != NULL) {
		*pB = b1684_byte(u32Packed, 0u);
	}
	if (pA != NULL) {
		*pA = b1684_byte(u32Packed, 24u);
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_rgba_unpack_u32(uint32_t u32Packed, uint8_t *pR, uint8_t *pG,
    uint8_t *pB, uint8_t *pA)
    __attribute__((alias("gj_rgba_unpack_u32")));
