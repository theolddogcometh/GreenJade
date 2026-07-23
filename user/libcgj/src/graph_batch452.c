/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch452: gj_crc16_ccitt — CRC-16/CCITT-FALSE one-shot.
 *
 * Surface (unique symbols — do NOT collide with existing CRC-16 exports):
 *   uint16_t gj_crc16_ccitt(const void *data, size_t n);
 *     — CRC-16/CCITT-FALSE one-shot over n bytes (poly 0x1021, init 0xFFFF,
 *       refin=false, refout=false, xorout=0).
 *       Check("123456789") == 0x29B1.
 *       NULL data with n != 0 → 0. Empty (n == 0) → 0xFFFF (init).
 *   __gj_crc16_ccitt  (alias)
 *   __libcgj_batch452_marker = "libcgj-batch452"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   crc16_ccitt / __crc16_ccitt  (update-style)  → graph_batch41.c
 *   crc16 / __crc16              (one-shot)      → graph_batch41.c
 *   crc16_modbus*                                → graph_batch42.c
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch452_marker[] = "libcgj-batch452";

/* CRC-16/CCITT-FALSE polynomial (non-reflected, MSB-first). */
#define B452_POLY 0x1021u
#define B452_INIT 0xFFFFu

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc16_ccitt — one-shot CRC-16/CCITT-FALSE.
 *
 * Catalog parameters (CRC-16/CCITT-FALSE / CRC-16/AUTOSAR / X.25 false):
 *   width=16 poly=0x1021 init=0xFFFF refin=false refout=false xorout=0
 *
 * Bit-serial MSB-first; equivalent to batch41 crc16()/crc16_ccitt(0xFFFF,...).
 */
uint16_t
gj_crc16_ccitt(const void *data, size_t n)
{
	const unsigned char *p = (const unsigned char *)data;
	uint16_t crc = (uint16_t)B452_INIT;
	size_t i;

	if (p == NULL && n != 0u) {
		return 0u;
	}
	if (n == 0u) {
		return crc;
	}

	for (i = 0u; i < n; i++) {
		unsigned b;

		crc ^= (uint16_t)((uint16_t)p[i] << 8);
		for (b = 0u; b < 8u; b++) {
			if (crc & 0x8000u) {
				crc = (uint16_t)((crc << 1) ^ B452_POLY);
			} else {
				crc = (uint16_t)(crc << 1);
			}
		}
	}
	return crc;
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_crc16_ccitt(const void *data, size_t n)
    __attribute__((alias("gj_crc16_ccitt")));
