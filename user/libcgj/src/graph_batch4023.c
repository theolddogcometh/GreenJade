/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4023: CRC-32C hash of a byte buffer.
 *
 * Surface (unique symbols):
 *   uint32_t gj_buf_hash_crc32c_u(const void *data, size_t n);
 *     - Castagnoli CRC-32C over n bytes at data.
 *       Reflected poly 0x82F63B78, init/final-xor 0xFFFFFFFF.
 *       Check("123456789") == 0xE3069283.
 *       NULL data with n != 0 -> 0. Empty (n == 0) -> 0.
 *   uint32_t __gj_buf_hash_crc32c_u  (alias)
 *   __libcgj_batch4023_marker = "libcgj-batch4023"
 *
 * Exclusive continuum CREATE-ONLY (4021-4030: str_hash_crc32c_u,
 * str_hash_adler_u, buf_hash_crc32c_u, buf_hash_adler_u, u32_mix_final_u,
 * u64_mix_final_u, u32_avalanche_u, u64_avalanche_u, u32_scramble_u,
 * batch_id_4030). Unique gj_buf_hash_crc32c_u surface only; no multi-def.
 * Distinct from crc32c (batch39) / crc32c_hw (batch87) — no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4023_marker[] = "libcgj-batch4023";

/* Castagnoli CRC-32C reflected polynomial. */
#define B4023_CRC32C_POLY 0x82F63B78u
#define B4023_CRC32C_INIT 0xFFFFFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

/* Fold one octet into a running reflected CRC-32C. */
static uint32_t
b4023_crc32c_byte(uint32_t u32Crc, unsigned char u8B)
{
	unsigned uBit;

	u32Crc ^= (uint32_t)u8B;
	for (uBit = 0u; uBit < 8u; uBit++) {
		if ((u32Crc & 1u) != 0u) {
			u32Crc = (u32Crc >> 1) ^ B4023_CRC32C_POLY;
		} else {
			u32Crc >>= 1;
		}
	}
	return u32Crc;
}

/* One-shot CRC-32C over n bytes. */
static uint32_t
b4023_buf_crc32c(const unsigned char *pData, size_t cbN)
{
	uint32_t u32Crc;
	size_t i;

	u32Crc = B4023_CRC32C_INIT;
	for (i = 0u; i < cbN; i++) {
		u32Crc = b4023_crc32c_byte(u32Crc, pData[i]);
	}
	return u32Crc ^ B4023_CRC32C_INIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_hash_crc32c_u - Castagnoli CRC-32C of n bytes at data.
 *
 * data: byte buffer (may be NULL only when n == 0)
 * n:    length in bytes
 *
 * Returns the 32-bit CRC-32C digest. NULL data with n != 0 yields 0.
 * Empty input (n == 0) yields 0. Does not call libc. No parent wires.
 */
uint32_t
gj_buf_hash_crc32c_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return 0u;
	}
	return b4023_buf_crc32c(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_buf_hash_crc32c_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_buf_hash_crc32c_u")));
