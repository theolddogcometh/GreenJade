/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1428: gj_crc6_itu — CRC-6/ITU one-shot.
 *
 * Surface (unique symbols — avoid bare crc8/16/32 names):
 *   uint8_t gj_crc6_itu(const void *data, size_t n);
 *     — CRC-6/ITU over n bytes. Catalog parameters:
 *         width=6 poly=0x03 init=0x00 refin=true refout=true xorout=0x00
 *       Check("123456789") == 0x06. Result in low 6 bits.
 *       NULL data with n != 0 → 0. Empty (n == 0) → 0.
 *   uint8_t __gj_crc6_itu  (alias)
 *   __libcgj_batch1428_marker = "libcgj-batch1428"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1428_marker[] = "libcgj-batch1428";

#define B1428_WIDTH  6u
#define B1428_POLY   0x03u
#define B1428_INIT   0x00u
#define B1428_XOROUT 0x00u
#define B1428_MASK   0x3fu

/* ---- freestanding helpers ---------------------------------------------- */

/* Reverse low n bits of v (n <= 8). */
static uint8_t
b1428_rev_bits(uint8_t u8V, unsigned nBits)
{
	uint8_t u8R = 0u;
	unsigned i;

	for (i = 0u; i < nBits; i++) {
		u8R = (uint8_t)((u8R << 1) | (u8V & 1u));
		u8V = (uint8_t)(u8V >> 1);
	}
	return u8R;
}

static uint8_t
b1428_poly_ref(void)
{
	return b1428_rev_bits((uint8_t)B1428_POLY, B1428_WIDTH);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc6_itu — one-shot CRC-6/ITU (reflected, init/xorout 0).
 */
uint8_t
gj_crc6_itu(const void *pData, size_t cb)
{
	const unsigned char *p;
	uint8_t u8Crc;
	uint8_t u8Poly;
	size_t iByte;
	unsigned iBit;

	if (pData == NULL && cb != 0u) {
		return 0u;
	}

	u8Crc = (uint8_t)B1428_INIT;
	u8Poly = b1428_poly_ref();
	if (cb == 0u) {
		return (uint8_t)((u8Crc ^ (uint8_t)B1428_XOROUT) & B1428_MASK);
	}

	p = (const unsigned char *)pData;
	for (iByte = 0u; iByte < cb; iByte++) {
		uint8_t u8B = p[iByte];

		for (iBit = 0u; iBit < 8u; iBit++) {
			uint8_t u8Mix = (uint8_t)((u8Crc ^ u8B) & 1u);

			u8Crc = (uint8_t)(u8Crc >> 1);
			if (u8Mix != 0u) {
				u8Crc = (uint8_t)(u8Crc ^ u8Poly);
			}
			u8B = (uint8_t)(u8B >> 1);
		}
	}

	u8Crc = (uint8_t)(u8Crc ^ (uint8_t)B1428_XOROUT);
	return (uint8_t)(u8Crc & B1428_MASK);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_crc6_itu(const void *pData, size_t cb)
    __attribute__((alias("gj_crc6_itu")));
