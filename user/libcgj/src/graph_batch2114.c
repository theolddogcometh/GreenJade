/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2114: gj_crc16_xmodem_u - CRC-16/XMODEM one-shot.
 *
 * Surface (unique symbols):
 *   uint16_t gj_crc16_xmodem_u(const void *data, size_t n);
 *     - CRC-16/XMODEM one-shot over n bytes:
 *       poly 0x1021, init 0x0000, refin=false, refout=false, xorout 0x0000.
 *       Check("123456789") == 0x31C3.
 *       NULL data with n != 0 -> 0. Empty (n == 0) -> 0.
 *   uint16_t __gj_crc16_xmodem_u  (alias)
 *   __libcgj_batch2114_marker = "libcgj-batch2114"
 *
 * Post-2100 crc exclusive wave. Unique _u surface; distinct from
 * gj_crc16_ccitt (init 0xFFFF, batch452).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2114_marker[] = "libcgj-batch2114";

/* CRC-16/XMODEM non-reflected MSB-first poly. */
#define B2114_POLY 0x1021u
#define B2114_INIT 0x0000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b2114_crc16_xmodem(const unsigned char *pData, size_t cbN)
{
	uint16_t u16Crc;
	size_t iByte;
	unsigned uBit;

	u16Crc = (uint16_t)B2114_INIT;
	for (iByte = 0u; iByte < cbN; iByte++) {
		u16Crc ^= (uint16_t)((uint16_t)pData[iByte] << 8);
		for (uBit = 0u; uBit < 8u; uBit++) {
			if ((u16Crc & 0x8000u) != 0u) {
				u16Crc = (uint16_t)((u16Crc << 1) ^ B2114_POLY);
			} else {
				u16Crc = (uint16_t)(u16Crc << 1);
			}
		}
	}
	return u16Crc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc16_xmodem_u - one-shot CRC-16/XMODEM over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0)
 * n:    byte count
 *
 * Returns the 16-bit CRC. NULL data with n != 0 yields 0 (defensive).
 */
uint16_t
gj_crc16_xmodem_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return (uint16_t)B2114_INIT;
	}
	return b2114_crc16_xmodem(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_crc16_xmodem_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_crc16_xmodem_u")));
