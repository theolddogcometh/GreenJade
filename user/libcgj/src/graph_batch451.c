/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch451: gj_crc8 — CRC-8 one-shot (poly 0x07, init 0).
 *
 * Surface (unique symbols — do NOT collide with bare crc8 from batch42):
 *   uint8_t gj_crc8(const void *data, size_t n);
 *     — CRC-8/SMBUS-style one-shot over n bytes (poly 0x07, refin=false,
 *       refout=false, init=0, xorout=0). MSB-first bit loop.
 *       Check("123456789") == 0xF4.
 *       NULL data with n != 0 → 0. Empty (n == 0) → 0.
 *   uint8_t __gj_crc8  (alias)
 *   __libcgj_batch451_marker = "libcgj-batch451"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   crc8 / crc8_update / __crc8 / __crc8_update  → graph_batch42.c
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch451_marker[] = "libcgj-batch451";

/* CRC-8 poly 0x07 (CRC-8/SMBUS), non-reflected. */
#define B451_POLY 0x07u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc8 — one-shot CRC-8 (poly 0x07, init 0).
 *
 * Same catalog parameters as batch42 crc8():
 *   width=8 poly=0x07 init=0 refin=false refout=false xorout=0
 */
uint8_t
gj_crc8(const void *data, size_t n)
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
		uCrc ^= pData[iByte];
		for (iBit = 0u; iBit < 8u; iBit++) {
			if (uCrc & 0x80u) {
				uCrc = (uint8_t)((uCrc << 1) ^ B451_POLY);
			} else {
				uCrc = (uint8_t)(uCrc << 1);
			}
		}
	}
	return uCrc;
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_crc8(const void *data, size_t n)
    __attribute__((alias("gj_crc8")));
