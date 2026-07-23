/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2385: CRC-32/BZIP2 one-shot unique surface
 * (post-2380 checksum exclusive wave).
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc32_bzip2_u(const void *data, size_t n);
 *     - CRC-32/BZIP2 one-shot: poly 0x04C11DB7, init 0xFFFFFFFF,
 *       refin=false, refout=false, xorout 0xFFFFFFFF. MSB-first bit loop.
 *       Check("123456789") == 0xFC891918.
 *       NULL data with n != 0 -> 0. Empty (n == 0) -> 0 (init^xor).
 *   uint32_t __gj_crc32_bzip2_u  (alias)
 *   __libcgj_batch2385_marker = "libcgj-batch2385"
 *
 * Post-2380 checksum exclusive wave (2381-2390). Unique BZIP2 surface;
 * no multi-def with zlib-style reflected CRC-32.
 *
 * Clean-room freestanding pure C (integer/pointer only). Bit-serial
 * only (no table). Compiles with -ffreestanding -msse2 -Wall -Wextra
 * -Werror. No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2385_marker[] = "libcgj-batch2385";

/* CRC-32/BZIP2 non-reflected poly, init, and final xor. */
#define B2385_POLY    0x04C11DB7u
#define B2385_INIT    0xFFFFFFFFu
#define B2385_XOROUT  0xFFFFFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

/* One-shot CRC-32/BZIP2 MSB-first bit loop over cb bytes at p. */
static uint32_t
b2385_crc32_bzip2(const unsigned char *p, size_t cb)
{
	uint32_t uCrc;
	size_t iByte;
	unsigned iBit;

	uCrc = B2385_INIT;
	for (iByte = 0u; iByte < cb; iByte++) {
		uCrc ^= ((uint32_t)p[iByte]) << 24;
		for (iBit = 0u; iBit < 8u; iBit++) {
			if ((uCrc & 0x80000000u) != 0u) {
				uCrc = (uCrc << 1) ^ B2385_POLY;
			} else {
				uCrc <<= 1;
			}
		}
	}
	return uCrc ^ B2385_XOROUT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc32_bzip2_u - one-shot CRC-32/BZIP2 over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0)
 * n:    byte count
 *
 * Returns the 32-bit CRC. NULL data with n != 0 yields 0.
 * Empty input yields init xor xorout (0). Does not call libc.
 */
uint32_t
gj_crc32_bzip2_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return B2385_INIT ^ B2385_XOROUT;
	}
	return b2385_crc32_bzip2(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc32_bzip2_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_crc32_bzip2_u")));
