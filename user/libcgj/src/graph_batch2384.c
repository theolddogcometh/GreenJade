/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2384: CRC-16/DNP one-shot unique surface
 * (post-2380 checksum exclusive wave).
 *
 * Surface (unique symbols):
 *   uint16_t gj_crc16_dnp_u(const void *data, size_t n);
 *     - CRC-16/DNP one-shot: poly 0x3D65 (reflected 0xA6BC),
 *       init 0x0000, refin=true, refout=true, xorout 0xFFFF.
 *       LSB-first bit loop. Check("123456789") == 0xEA82.
 *       NULL data with n != 0 -> 0. Empty (n == 0) -> 0xFFFF (init^xor).
 *   uint16_t __gj_crc16_dnp_u  (alias)
 *   __libcgj_batch2384_marker = "libcgj-batch2384"
 *
 * Post-2380 checksum exclusive wave (2381-2390). Unique DNP surface;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Bit-serial
 * only (no table). Compiles with -ffreestanding -msse2 -Wall -Wextra
 * -Werror. No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2384_marker[] = "libcgj-batch2384";

/* CRC-16/DNP reflected poly, init, and final xor. */
#define B2384_POLY    0xA6BCu
#define B2384_INIT    0x0000u
#define B2384_XOROUT  0xFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

/* One-shot CRC-16/DNP reflected bit loop over cb bytes at p. */
static uint16_t
b2384_crc16_dnp(const unsigned char *p, size_t cb)
{
	uint16_t uCrc;
	size_t iByte;
	unsigned iBit;

	uCrc = (uint16_t)B2384_INIT;
	for (iByte = 0u; iByte < cb; iByte++) {
		uCrc ^= (uint16_t)p[iByte];
		for (iBit = 0u; iBit < 8u; iBit++) {
			if ((uCrc & 1u) != 0u) {
				uCrc = (uint16_t)((uCrc >> 1) ^ B2384_POLY);
			} else {
				uCrc = (uint16_t)(uCrc >> 1);
			}
		}
	}
	return (uint16_t)(uCrc ^ (uint16_t)B2384_XOROUT);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc16_dnp_u - one-shot CRC-16/DNP over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0)
 * n:    byte count
 *
 * Returns the 16-bit CRC. NULL data with n != 0 yields 0.
 * Empty input yields init xor xorout (0xFFFF). Does not call libc.
 */
uint16_t
gj_crc16_dnp_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return (uint16_t)(B2384_INIT ^ B2384_XOROUT);
	}
	return b2384_crc16_dnp(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_crc16_dnp_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_crc16_dnp_u")));
