/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1427: gj_crc5_usb — CRC-5/USB one-shot.
 *
 * Surface (unique symbols — avoid bare crc8/16/32 names):
 *   uint8_t gj_crc5_usb(const void *data, size_t n);
 *     — CRC-5/USB over n bytes. Catalog parameters:
 *         width=5 poly=0x05 init=0x1f refin=true refout=true xorout=0x1f
 *       Check("123456789") == 0x19. Result in low 5 bits.
 *       NULL data with n != 0 → 0. Empty (n == 0) → 0x1f ^ 0x1f = 0
 *       after init/xorout (init folded with xorout → 0 for empty).
 *       Empty returns init^xorout = 0.
 *   uint8_t __gj_crc5_usb  (alias)
 *   __libcgj_batch1427_marker = "libcgj-batch1427"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1427_marker[] = "libcgj-batch1427";

/* CRC-5/USB: reflected poly 0x05 → 0x14 in LSB-first form (bit-reversed). */
#define B1427_WIDTH   5u
#define B1427_POLY    0x05u
#define B1427_INIT    0x1fu
#define B1427_XOROUT  0x1fu
#define B1427_MASK    0x1fu

/* ---- freestanding helpers ---------------------------------------------- */

/* Reverse low n bits of v (n <= 8). */
static uint8_t
b1427_rev_bits(uint8_t u8V, unsigned nBits)
{
	uint8_t u8R = 0u;
	unsigned i;

	for (i = 0u; i < nBits; i++) {
		u8R = (uint8_t)((u8R << 1) | (u8V & 1u));
		u8V = (uint8_t)(u8V >> 1);
	}
	return u8R;
}

/* Bit-reversed poly for LSB-first (reflected) processing. */
static uint8_t
b1427_poly_ref(void)
{
	return b1427_rev_bits((uint8_t)B1427_POLY, B1427_WIDTH);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc5_usb — one-shot CRC-5/USB (reflected, init/xorout 0x1f).
 *
 * Bit-serial LSB-first over each input byte (low bit first).
 */
uint8_t
gj_crc5_usb(const void *pData, size_t cb)
{
	const unsigned char *p;
	uint8_t u8Crc;
	uint8_t u8Poly;
	size_t iByte;
	unsigned iBit;

	if (pData == NULL && cb != 0u) {
		return 0u;
	}

	u8Crc = (uint8_t)B1427_INIT;
	u8Poly = b1427_poly_ref();
	if (cb == 0u) {
		return (uint8_t)((u8Crc ^ (uint8_t)B1427_XOROUT) & B1427_MASK);
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

	u8Crc = (uint8_t)(u8Crc ^ (uint8_t)B1427_XOROUT);
	return (uint8_t)(u8Crc & B1427_MASK);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_crc5_usb(const void *pData, size_t cb)
    __attribute__((alias("gj_crc5_usb")));
