/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1219: CRC-32C (Castagnoli) one-shot bytes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc32_c_bytes(const void *data, size_t n);
 *     — One-shot CRC-32C over data[0..n) using the Castagnoli polynomial
 *       (reflected form 0x82F63B78). Init/final XOR 0xFFFFFFFF.
 *       Check("123456789") == 0xE3069283.
 *       NULL data with n != 0 → 0; empty n == 0 → 0.
 *   uint32_t __gj_crc32_c_bytes  (alias)
 *   __libcgj_batch1219_marker = "libcgj-batch1219"
 *
 * Distinct from gj_crc32c_sw (batch471) — unique _bytes name; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Software bit path only (no table).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1219_marker[] = "libcgj-batch1219";

/* Castagnoli reflected polynomial (iSCSI / SSE4.2 crc32). */
#define B1219_POLY 0x82F63B78u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1219_tab_byte(uint32_t uIdx)
{
	uint32_t uC;
	unsigned uBit;

	uC = uIdx;
	for (uBit = 0u; uBit < 8u; uBit++) {
		if ((uC & 1u) != 0u) {
			uC = (uC >> 1) ^ B1219_POLY;
		} else {
			uC >>= 1;
		}
	}
	return uC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc32_c_bytes — one-shot software CRC-32C (Castagnoli) over n bytes.
 */
uint32_t
gj_crc32_c_bytes(const void *pData, size_t cbN)
{
	const unsigned char *p;
	uint32_t u32Crc;
	size_t i;

	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}

	u32Crc = 0xffffffffu;
	for (i = 0u; i < cbN; i++) {
		uint32_t u32Idx;

		u32Idx = (u32Crc ^ (uint32_t)p[i]) & 0xffu;
		u32Crc = b1219_tab_byte(u32Idx) ^ (u32Crc >> 8);
	}
	return ~u32Crc;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc32_c_bytes(const void *pData, size_t cbN)
    __attribute__((alias("gj_crc32_c_bytes")));
