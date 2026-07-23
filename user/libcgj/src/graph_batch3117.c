/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3117: unpack one u32 into four u8 values (_u).
 *
 * Surface (unique symbols):
 *   void gj_u8_unpack4_u(uint32_t v, uint8_t *b0, uint8_t *b1,
 *                        uint8_t *b2, uint8_t *b3);
 *     - Inverse of gj_u8_pack4_u: *b0 = bits [7:0], *b1 = [15:8],
 *       *b2 = [23:16], *b3 = [31:24]. NULL pointer for a lane is ignored.
 *   void __gj_u8_unpack4_u  (alias)
 *   __libcgj_batch3117_marker = "libcgj-batch3117"
 *
 * Exclusive continuum CREATE-ONLY (3111-3120). Distinct from
 * gj_u16_unpack_u8x2 — unique gj_u8_unpack4_u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3117_marker[] = "libcgj-batch3117";

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b3117_b0(uint32_t u32V)
{
	return (uint8_t)u32V;
}

static uint8_t
b3117_b1(uint32_t u32V)
{
	return (uint8_t)(u32V >> 8);
}

static uint8_t
b3117_b2(uint32_t u32V)
{
	return (uint8_t)(u32V >> 16);
}

static uint8_t
b3117_b3(uint32_t u32V)
{
	return (uint8_t)(u32V >> 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_unpack4_u - split one uint32 into four uint8 lanes.
 *
 * u32V: packed word
 * pB0:  receives bits [7:0];   NULL → skip
 * pB1:  receives bits [15:8];  NULL → skip
 * pB2:  receives bits [23:16]; NULL → skip
 * pB3:  receives bits [31:24]; NULL → skip
 */
void
gj_u8_unpack4_u(uint32_t u32V, uint8_t *pB0, uint8_t *pB1, uint8_t *pB2,
    uint8_t *pB3)
{
	(void)NULL;
	if (pB0 != NULL) {
		*pB0 = b3117_b0(u32V);
	}
	if (pB1 != NULL) {
		*pB1 = b3117_b1(u32V);
	}
	if (pB2 != NULL) {
		*pB2 = b3117_b2(u32V);
	}
	if (pB3 != NULL) {
		*pB3 = b3117_b3(u32V);
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u8_unpack4_u(uint32_t u32V, uint8_t *pB0, uint8_t *pB1,
    uint8_t *pB2, uint8_t *pB3)
    __attribute__((alias("gj_u8_unpack4_u")));
