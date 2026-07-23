/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2118: gj_crc32_mpeg2_u - CRC-32/MPEG-2.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc32_mpeg2_u(const void *data, size_t n);
 *     - CRC-32/MPEG-2 one-shot over n bytes:
 *       poly 0x04C11DB7, init 0xFFFFFFFF, refin=false, refout=false,
 *       xorout 0x00000000. Non-reflected MSB-first bit loop.
 *       Check("123456789") == 0x0376E6E7.
 *       NULL data with n != 0 -> 0. Empty (n == 0) -> 0xFFFFFFFF.
 *   uint32_t __gj_crc32_mpeg2_u  (alias)
 *   __libcgj_batch2118_marker = "libcgj-batch2118"
 *
 * Post-2100 crc exclusive wave. Unique _u surface; avoid multi-def with
 * reflected IEEE CRC-32 paths.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2118_marker[] = "libcgj-batch2118";

/* CRC-32/MPEG-2 non-reflected MSB-first poly. */
#define B2118_POLY 0x04C11DB7u
#define B2118_INIT 0xFFFFFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2118_step_byte(uint32_t u32Crc, unsigned char u8Byte)
{
	unsigned uBit;

	u32Crc ^= ((uint32_t)u8Byte << 24);
	for (uBit = 0u; uBit < 8u; uBit++) {
		if ((u32Crc & 0x80000000u) != 0u) {
			u32Crc = (u32Crc << 1) ^ B2118_POLY;
		} else {
			u32Crc <<= 1;
		}
	}
	return u32Crc;
}

static uint32_t
b2118_crc32_mpeg2(const unsigned char *pData, size_t cbN)
{
	uint32_t u32Crc;
	size_t iByte;

	u32Crc = B2118_INIT;
	for (iByte = 0u; iByte < cbN; iByte++) {
		u32Crc = b2118_step_byte(u32Crc, pData[iByte]);
	}
	return u32Crc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc32_mpeg2_u - one-shot CRC-32/MPEG-2 over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0)
 * n:    byte count
 *
 * Returns the 32-bit CRC. NULL data with n != 0 yields 0 (defensive).
 */
uint32_t
gj_crc32_mpeg2_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return B2118_INIT;
	}
	return b2118_crc32_mpeg2(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc32_mpeg2_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_crc32_mpeg2_u")));
