/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1425: gj_bsd_checksum — historic BSD sum (-r).
 *
 * Surface (unique symbols):
 *   uint16_t gj_bsd_checksum(const void *data, size_t n);
 *     — BSD rotating checksum over n bytes:
 *         checksum = ror1(checksum); checksum += byte; (mod 2^16)
 *       data == NULL or n == 0 → 0.
 *   uint16_t __gj_bsd_checksum  (alias)
 *   __libcgj_batch1425_marker = "libcgj-batch1425"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1425_marker[] = "libcgj-batch1425";

/* ---- freestanding helpers ---------------------------------------------- */

/* 16-bit rotate right by 1. */
static uint16_t
b1425_ror16_1(uint16_t u16X)
{
	return (uint16_t)((u16X >> 1) | ((u16X & 1u) << 15));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bsd_checksum — BSD sum algorithm (16-bit rotate-right-then-add).
 *
 * Matches the classic "sum -r" / BSD cksum path over a raw byte buffer
 * (checksum field only; no block-count packing).
 */
uint16_t
gj_bsd_checksum(const void *pData, size_t cb)
{
	const unsigned char *p;
	uint16_t u16Ck;
	size_t i;

	if (pData == NULL || cb == 0u) {
		return 0u;
	}

	p = (const unsigned char *)pData;
	u16Ck = 0u;
	for (i = 0u; i < cb; i++) {
		u16Ck = b1425_ror16_1(u16Ck);
		u16Ck = (uint16_t)(u16Ck + (uint16_t)p[i]);
	}
	return u16Ck;
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_bsd_checksum(const void *pData, size_t cb)
    __attribute__((alias("gj_bsd_checksum")));
