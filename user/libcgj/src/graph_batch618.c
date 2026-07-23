/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch618: gj_crc7 — CRC-7 one-shot (poly 0x09, init 0).
 *
 * Surface (unique symbols):
 *   uint8_t gj_crc7(const void *data, size_t n);
 *     — CRC-7/MMC-style one-shot over n bytes (poly 0x09 = x^7 + x^3 + 1,
 *       refin=false, refout=false, init=0, xorout=0). MSB-first bit loop;
 *       result is a 7-bit value in the low 7 bits (0..0x7F).
 *       Check("123456789") == 0x75.
 *       NULL data with n != 0 → 0. Empty (n == 0) → 0.
 *   uint8_t __gj_crc7  (alias)
 *   __libcgj_batch618_marker = "libcgj-batch618"
 *
 * Catalog parameters (CRC-7/MMC, loosely as used for MultiMediaCard CMD):
 *   width=7 poly=0x09 init=0 refin=false refout=false xorout=0 check=0x75
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch618_marker[] = "libcgj-batch618";

/*
 * CRC-7 polynomial 0x09 (x^7 + x^3 + 1), non-reflected, as used loosely
 * for MMC-style command CRC-7. The poly is applied in the low 7 bits of
 * an 8-bit working register.
 */
#define B618_POLY 0x09u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc7 — one-shot CRC-7 (poly 0x09, init 0).
 *
 * Bit-serial MSB-first over each input byte; CRC held in bits [6:0].
 */
uint8_t
gj_crc7(const void *data, size_t n)
{
	const unsigned char *pData = (const unsigned char *)data;
	uint8_t uCrc = 0u;
	size_t iByte;
	unsigned iBit;

	if (pData == NULL && n != 0u) {
		return 0u;
	}
	if (n == 0u) {
		return 0u;
	}

	for (iByte = 0u; iByte < n; iByte++) {
		for (iBit = 0u; iBit < 8u; iBit++) {
			unsigned uBit = ((unsigned)pData[iByte] >> (7u - iBit)) & 1u;
			unsigned uMsb = ((unsigned)uCrc >> 6) & 1u;

			uCrc = (uint8_t)((uCrc << 1) & 0x7Fu);
			if (uBit ^ uMsb) {
				uCrc = (uint8_t)(uCrc ^ B618_POLY);
			}
		}
	}
	return (uint8_t)(uCrc & 0x7Fu);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_crc7(const void *data, size_t n)
    __attribute__((alias("gj_crc7")));
