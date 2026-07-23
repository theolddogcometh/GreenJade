/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2383: CRC-16/IBM (ARC) one-shot unique surface
 * (post-2380 checksum exclusive wave).
 *
 * Surface (unique symbols):
 *   uint16_t gj_crc16_ibm_u(const void *data, size_t n);
 *     - CRC-16/IBM (ARC) one-shot: poly 0x8005 (reflected 0xA001),
 *       init 0x0000, refin=true, refout=true, xorout 0x0000.
 *       LSB-first bit loop. Check("123456789") == 0xBB3D.
 *       NULL data with n != 0 -> 0. Empty (n == 0) -> 0 (init).
 *   uint16_t __gj_crc16_ibm_u  (alias)
 *   __libcgj_batch2383_marker = "libcgj-batch2383"
 *
 * Post-2380 checksum exclusive wave (2381-2390). Distinct from
 * gj_crc16_ibm (batch1218) - unique gj_crc16_ibm_u name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Bit-serial
 * only (no table). Compiles with -ffreestanding -msse2 -Wall -Wextra
 * -Werror. No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2383_marker[] = "libcgj-batch2383";

/* CRC-16/IBM reflected poly and init. */
#define B2383_POLY  0xA001u
#define B2383_INIT  0x0000u

/* ---- freestanding helpers ---------------------------------------------- */

/* One-shot CRC-16/IBM reflected bit loop over cb bytes at p. */
static uint16_t
b2383_crc16_ibm(const unsigned char *p, size_t cb)
{
	uint16_t uCrc;
	size_t iByte;
	unsigned iBit;

	uCrc = (uint16_t)B2383_INIT;
	for (iByte = 0u; iByte < cb; iByte++) {
		uCrc ^= (uint16_t)p[iByte];
		for (iBit = 0u; iBit < 8u; iBit++) {
			if ((uCrc & 1u) != 0u) {
				uCrc = (uint16_t)((uCrc >> 1) ^ B2383_POLY);
			} else {
				uCrc = (uint16_t)(uCrc >> 1);
			}
		}
	}
	return uCrc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc16_ibm_u - one-shot CRC-16/IBM over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0)
 * n:    byte count
 *
 * Returns the 16-bit CRC. NULL data with n != 0 yields 0.
 * Empty input yields init (0). Does not call libc.
 */
uint16_t
gj_crc16_ibm_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return (uint16_t)B2383_INIT;
	}
	return b2383_crc16_ibm(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_crc16_ibm_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_crc16_ibm_u")));
