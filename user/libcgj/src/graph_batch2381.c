/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2381: CRC-8/SMBUS one-shot unique surface
 * (post-2380 checksum exclusive wave).
 *
 * Surface (unique symbols):
 *   uint8_t gj_crc8_smbus_u(const void *data, size_t n);
 *     - CRC-8/SMBUS one-shot over n bytes: poly 0x07, init 0x00,
 *       refin=false, refout=false, xorout 0x00. MSB-first bit loop.
 *       Check("123456789") == 0xF4.
 *       NULL data with n != 0 -> 0. Empty (n == 0) -> 0 (init).
 *   uint8_t __gj_crc8_smbus_u  (alias)
 *   __libcgj_batch2381_marker = "libcgj-batch2381"
 *
 * Post-2380 checksum exclusive wave (2381-2390). Distinct from gj_crc8
 * (batch451) and crc8 (batch42) - unique gj_crc8_smbus_u name only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Bit-serial
 * only (no table). Compiles with -ffreestanding -msse2 -Wall -Wextra
 * -Werror. No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2381_marker[] = "libcgj-batch2381";

/* CRC-8/SMBUS poly (non-reflected, MSB-first). */
#define B2381_POLY  0x07u
#define B2381_INIT  0x00u

/* ---- freestanding helpers ---------------------------------------------- */

/* One-shot CRC-8/SMBUS bit loop over cb bytes at p. */
static uint8_t
b2381_crc8_smbus(const unsigned char *p, size_t cb)
{
	uint8_t uCrc;
	size_t iByte;
	unsigned iBit;

	uCrc = (uint8_t)B2381_INIT;
	for (iByte = 0u; iByte < cb; iByte++) {
		uCrc ^= p[iByte];
		for (iBit = 0u; iBit < 8u; iBit++) {
			if ((uCrc & 0x80u) != 0u) {
				uCrc = (uint8_t)((uCrc << 1) ^ B2381_POLY);
			} else {
				uCrc = (uint8_t)(uCrc << 1);
			}
		}
	}
	return uCrc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc8_smbus_u - one-shot CRC-8/SMBUS over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0)
 * n:    byte count
 *
 * Returns the 8-bit CRC. NULL data with n != 0 yields 0.
 * Empty input yields init (0). Does not call libc.
 */
uint8_t
gj_crc8_smbus_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return (uint8_t)B2381_INIT;
	}
	return b2381_crc8_smbus(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_crc8_smbus_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_crc8_smbus_u")));
