/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch727: gj_crc8_poly — CRC-8 one-shot with poly/init.
 *
 * Surface (unique symbols — do NOT collide with bare crc8 / gj_crc8):
 *   uint8_t gj_crc8_poly(const void *data, size_t n, uint8_t poly,
 *                        uint8_t init);
 *     — CRC-8 one-shot over n bytes with caller-supplied polynomial and
 *       initial value. refin=false, refout=false, xorout=0. MSB-first bit
 *       loop (same non-reflected style as batch42 crc8 / batch451 gj_crc8).
 *       With poly=0x07 and init=0, Check("123456789") == 0xF4.
 *       NULL data with n != 0 → init. Empty (n == 0) → init.
 *   uint8_t __gj_crc8_poly  (alias)
 *   __libcgj_batch727_marker = "libcgj-batch727"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def crc8):
 *   crc8 / crc8_update / __crc8 / __crc8_update  → graph_batch42.c
 *   gj_crc8 / __gj_crc8                          → graph_batch451.c
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch727_marker[] = "libcgj-batch727";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc8_poly — one-shot CRC-8 with parameterized poly and init.
 *
 * Bit-serial MSB-first: each input byte is XOR'd into the CRC register,
 * then 8 shifts with conditional XOR of poly when the high bit is set
 * (non-reflected / refin=false, refout=false, xorout=0).
 */
uint8_t
gj_crc8_poly(const void *data, size_t n, uint8_t poly, uint8_t init)
{
	const unsigned char *pData = (const unsigned char *)data;
	uint8_t uCrc = init;
	size_t iByte;
	unsigned iBit;

	if (pData == NULL && n != 0u) {
		return init;
	}
	if (n == 0u) {
		return init;
	}

	for (iByte = 0u; iByte < n; iByte++) {
		uCrc ^= pData[iByte];
		for (iBit = 0u; iBit < 8u; iBit++) {
			if (uCrc & 0x80u) {
				uCrc = (uint8_t)((uCrc << 1) ^ poly);
			} else {
				uCrc = (uint8_t)(uCrc << 1);
			}
		}
	}
	return uCrc;
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_crc8_poly(const void *data, size_t n, uint8_t poly, uint8_t init)
    __attribute__((alias("gj_crc8_poly")));
