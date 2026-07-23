/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1682: unpack 0x00RRGGBB into R,G,B channels.
 *
 * Surface (unique symbols):
 *   void gj_rgb_unpack_u32(uint32_t packed, uint8_t *r, uint8_t *g,
 *                          uint8_t *b);
 *     — Inverse of gj_rgb_pack_u32. NULL out-pointers are skipped.
 *   void __gj_rgb_unpack_u32  (alias)
 *   __libcgj_batch1682_marker = "libcgj-batch1682"
 *
 * Color / pixel exclusive integer wave. Does NOT redefine prior batch
 * symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1682_marker[] = "libcgj-batch1682";

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b1682_byte(uint32_t u32V, unsigned uShift)
{
	return (uint8_t)((u32V >> uShift) & 0xffu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rgb_unpack_u32 — split 0x00RRGGBB into three 8-bit channels.
 *
 * u32Packed: pixel word (high byte ignored)
 * pR / pG / pB: optional outs; NULL → skip that channel
 */
void
gj_rgb_unpack_u32(uint32_t u32Packed, uint8_t *pR, uint8_t *pG, uint8_t *pB)
{
	if (pR != NULL) {
		*pR = b1682_byte(u32Packed, 16u);
	}
	if (pG != NULL) {
		*pG = b1682_byte(u32Packed, 8u);
	}
	if (pB != NULL) {
		*pB = b1682_byte(u32Packed, 0u);
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_rgb_unpack_u32(uint32_t u32Packed, uint8_t *pR, uint8_t *pG,
    uint8_t *pB)
    __attribute__((alias("gj_rgb_unpack_u32")));
