/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch471: freestanding CRC-32C (Castagnoli) software.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc32c_sw(const void *data, size_t n);
 *     — One-shot CRC-32C over data[0..n) using the Castagnoli polynomial
 *       (reflected form 0x82F63B78). Init/final XOR 0xFFFFFFFF.
 *       NULL data with n != 0 → 0; empty n==0 → CRC of empty (0).
 *   __gj_crc32c_sw  (alias)
 *   __libcgj_batch471_marker = "libcgj-batch471"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Software table path only.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch471_marker[] = "libcgj-batch471";

/* Castagnoli reflected polynomial (iSCSI / SSE4.2 crc32). */
#define B471_POLY 0x82F63B78u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b471_tab_byte — compute one CRC-32C table entry for residual byte uIdx
 * by 8 rounds of reflected division by B471_POLY.
 */
static uint32_t
b471_tab_byte(uint32_t uIdx)
{
	uint32_t uC;
	unsigned uBit;

	uC = uIdx;
	for (uBit = 0u; uBit < 8u; uBit++) {
		if ((uC & 1u) != 0u) {
			uC = (uC >> 1) ^ B471_POLY;
		} else {
			uC >>= 1;
		}
	}
	return uC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc32c_sw — one-shot software CRC-32C (Castagnoli).
 *
 * data: input bytes (NULL only legal when n == 0).
 * n:    byte count.
 *
 * Returns the 32-bit CRC. NULL data with n != 0 yields 0 (defensive).
 * Empty input (n == 0) yields 0 (CRC of empty message after init/final XOR).
 *
 * Algorithm: reflected table-driven CRC with poly 0x82F63B78,
 * crc = ~0 initially, final crc = ~crc. Table entries computed on the
 * fly per residual byte (no static 1 KiB table; freestanding-friendly).
 */
uint32_t
gj_crc32c_sw(const void *data, size_t n)
{
	const unsigned char *p;
	uint32_t uCrc;
	size_t i;

	p = (const unsigned char *)data;
	if (p == NULL && n != 0u) {
		return 0u;
	}

	uCrc = 0xffffffffu;
	for (i = 0u; i < n; i++) {
		uint32_t uIdx;

		uIdx = (uCrc ^ (uint32_t)p[i]) & 0xffu;
		uCrc = b471_tab_byte(uIdx) ^ (uCrc >> 8);
	}
	return ~uCrc;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc32c_sw(const void *data, size_t n)
    __attribute__((alias("gj_crc32c_sw")));
