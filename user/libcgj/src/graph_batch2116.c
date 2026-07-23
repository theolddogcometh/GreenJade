/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2116: gj_crc24_openpgp_u - CRC-24/OpenPGP.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc24_openpgp_u(const void *data, size_t n);
 *     - CRC-24/OpenPGP (RFC 4880) one-shot over n bytes:
 *       poly 0x864CFB, init 0xB704CE, refin=false, refout=false,
 *       xorout 0x000000. Result is low 24 bits of the return value.
 *       Check("123456789") == 0x21CF02.
 *       NULL data with n != 0 -> 0. Empty (n == 0) -> 0xB704CE.
 *   uint32_t __gj_crc24_openpgp_u  (alias)
 *   __libcgj_batch2116_marker = "libcgj-batch2116"
 *
 * Post-2100 crc exclusive wave. Unique _u surface; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2116_marker[] = "libcgj-batch2116";

/* CRC-24/OpenPGP non-reflected MSB-first. */
#define B2116_POLY 0x864CFBu
#define B2116_INIT 0xB704CEu
#define B2116_MASK 0xFFFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2116_crc24_openpgp(const unsigned char *pData, size_t cbN)
{
	uint32_t u32Crc;
	size_t iByte;
	unsigned uBit;

	u32Crc = (uint32_t)B2116_INIT;
	for (iByte = 0u; iByte < cbN; iByte++) {
		u32Crc ^= ((uint32_t)pData[iByte] << 16);
		for (uBit = 0u; uBit < 8u; uBit++) {
			if ((u32Crc & 0x800000u) != 0u) {
				u32Crc = ((u32Crc << 1) ^ B2116_POLY) & B2116_MASK;
			} else {
				u32Crc = (u32Crc << 1) & B2116_MASK;
			}
		}
	}
	return u32Crc & B2116_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc24_openpgp_u - one-shot CRC-24/OpenPGP over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0)
 * n:    byte count
 *
 * Returns the 24-bit CRC in the low 24 bits. NULL data with n != 0
 * yields 0 (defensive).
 */
uint32_t
gj_crc24_openpgp_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return (uint32_t)B2116_INIT;
	}
	return b2116_crc24_openpgp(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc24_openpgp_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_crc24_openpgp_u")));
