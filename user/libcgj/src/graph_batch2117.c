/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2117: gj_crc32_jamcrc_u - CRC-32/JAMCRC.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc32_jamcrc_u(const void *data, size_t n);
 *     - CRC-32/JAMCRC one-shot over n bytes:
 *       poly 0x04C11DB7 (reflected 0xEDB88320), init 0xFFFFFFFF,
 *       refin=true, refout=true, xorout 0x00000000.
 *       Check("123456789") == 0x340BC6D9.
 *       NULL data with n != 0 -> 0. Empty (n == 0) -> 0xFFFFFFFF.
 *   uint32_t __gj_crc32_jamcrc_u  (alias)
 *   __libcgj_batch2117_marker = "libcgj-batch2117"
 *
 * Post-2100 crc exclusive wave. Unique _u surface; same reflected poly
 * path as IEEE/zlib CRC-32 but without final complement (xorout 0).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2117_marker[] = "libcgj-batch2117";

/* CRC-32/JAMCRC reflected poly (IEEE poly bit-reversed). */
#define B2117_POLY 0xEDB88320u
#define B2117_INIT 0xFFFFFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2117_step_byte(uint32_t u32Crc, unsigned char u8Byte)
{
	unsigned uBit;

	u32Crc ^= (uint32_t)u8Byte;
	for (uBit = 0u; uBit < 8u; uBit++) {
		if ((u32Crc & 1u) != 0u) {
			u32Crc = (u32Crc >> 1) ^ B2117_POLY;
		} else {
			u32Crc >>= 1;
		}
	}
	return u32Crc;
}

static uint32_t
b2117_crc32_jamcrc(const unsigned char *pData, size_t cbN)
{
	uint32_t u32Crc;
	size_t iByte;

	u32Crc = B2117_INIT;
	for (iByte = 0u; iByte < cbN; iByte++) {
		u32Crc = b2117_step_byte(u32Crc, pData[iByte]);
	}
	return u32Crc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc32_jamcrc_u - one-shot CRC-32/JAMCRC over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0)
 * n:    byte count
 *
 * Returns the 32-bit CRC. NULL data with n != 0 yields 0 (defensive).
 * Empty input yields init (0xFFFFFFFF); no final XOR.
 */
uint32_t
gj_crc32_jamcrc_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return B2117_INIT;
	}
	return b2117_crc32_jamcrc(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc32_jamcrc_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_crc32_jamcrc_u")));
