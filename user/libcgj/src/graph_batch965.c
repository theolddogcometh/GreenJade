/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch965: CRC-32 bit-by-bit with caller polynomial.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_crc32_poly(const void *data, size_t n,
 *                                uint32_t crc, uint32_t poly);
 *     — Non-reflected MSB-first bit-by-bit CRC over data[0..n), starting
 *       from crc and using the caller polynomial (e.g. 0x04C11DB7 for
 *       classic Ethernet CRC-32). No fixed xor-in/xor-out; the register
 *       is returned as-is after processing. poly == 0 → return crc
 *       unchanged (no shift path). NULL data with n != 0 → 0.
 *   uint32_t __gj_hash_crc32_poly  (alias)
 *   __libcgj_batch965_marker = "libcgj-batch965"
 *
 * Distinct from crc32 / crc32_z / crc32c (batch39) and crc32c_hw
 * (batch87) — those use fixed polys. Unique gj_hash_crc32_poly surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch965_marker[] = "libcgj-batch965";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_crc32_poly — CRC-32 with caller poly over n bytes at data.
 *
 * data: byte buffer (may be NULL only when n == 0)
 * n:    length in bytes
 * crc:  initial CRC register
 * poly: non-reflected generator poly (MSB-first bit path)
 *
 * Returns the updated CRC register. On NULL data with n != 0, returns 0.
 * poly == 0 is treated as a no-op (returns crc). Empty input returns crc.
 */
uint32_t
gj_hash_crc32_poly(const void *data, size_t n, uint32_t crc, uint32_t poly)
{
	const unsigned char *p = (const unsigned char *)data;
	uint32_t u32Crc = crc;
	size_t i;
	unsigned uBit;

	if (p == NULL && n != 0u) {
		return 0u;
	}
	if (poly == 0u) {
		return u32Crc;
	}

	for (i = 0u; i < n; i++) {
		u32Crc ^= ((uint32_t)p[i]) << 24;
		for (uBit = 0u; uBit < 8u; uBit++) {
			if ((u32Crc & 0x80000000u) != 0u) {
				u32Crc = (u32Crc << 1) ^ poly;
			} else {
				u32Crc <<= 1;
			}
		}
	}
	return u32Crc;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_crc32_poly(const void *data, size_t n, uint32_t crc,
			      uint32_t poly)
    __attribute__((alias("gj_hash_crc32_poly")));
