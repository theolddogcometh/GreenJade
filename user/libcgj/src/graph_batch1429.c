/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1429: gj_crc10_atm — CRC-10/ATM one-shot.
 *
 * Surface (unique symbols — avoid bare crc8/16/32 names):
 *   uint16_t gj_crc10_atm(const void *data, size_t n);
 *     — CRC-10/ATM over n bytes. Catalog parameters:
 *         width=10 poly=0x233 init=0x000 refin=false refout=false xorout=0
 *       Check("123456789") == 0x199. Result in low 10 bits.
 *       NULL data with n != 0 → 0. Empty (n == 0) → 0.
 *   uint16_t __gj_crc10_atm  (alias)
 *   __libcgj_batch1429_marker = "libcgj-batch1429"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1429_marker[] = "libcgj-batch1429";

#define B1429_WIDTH  10u
#define B1429_POLY   0x233u
#define B1429_INIT   0x000u
#define B1429_XOROUT 0x000u
#define B1429_MASK   0x3ffu
#define B1429_TOP    0x200u /* 1 << (width-1) */

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc10_atm — one-shot CRC-10/ATM (non-reflected, MSB-first).
 *
 * Bit-serial: each input byte is shifted into the top of the register
 * (align to bit width-8 relative to the 10-bit register).
 */
uint16_t
gj_crc10_atm(const void *pData, size_t cb)
{
	const unsigned char *p;
	uint16_t u16Crc;
	size_t iByte;
	unsigned iBit;

	if (pData == NULL && cb != 0u) {
		return 0u;
	}

	u16Crc = (uint16_t)B1429_INIT;
	if (cb == 0u) {
		return (uint16_t)((u16Crc ^ (uint16_t)B1429_XOROUT) & B1429_MASK);
	}

	p = (const unsigned char *)pData;
	for (iByte = 0u; iByte < cb; iByte++) {
		/*
		 * Align data byte with the top of the 10-bit register:
		 * shift left by (width - 8) = 2.
		 */
		u16Crc ^= (uint16_t)((uint16_t)p[iByte] << (B1429_WIDTH - 8u));
		for (iBit = 0u; iBit < 8u; iBit++) {
			if ((u16Crc & (uint16_t)B1429_TOP) != 0u) {
				u16Crc = (uint16_t)(((u16Crc << 1) ^
				    (uint16_t)B1429_POLY) & B1429_MASK);
			} else {
				u16Crc = (uint16_t)((u16Crc << 1) & B1429_MASK);
			}
		}
	}

	u16Crc = (uint16_t)(u16Crc ^ (uint16_t)B1429_XOROUT);
	return (uint16_t)(u16Crc & B1429_MASK);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_crc10_atm(const void *pData, size_t cb)
    __attribute__((alias("gj_crc10_atm")));
