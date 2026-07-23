/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4021: CRC-32C hash of a NUL-terminated string.
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_hash_crc32c_u(const char *s);
 *     - Castagnoli CRC-32C over octets of s up to (not including) NUL.
 *       Reflected poly 0x82F63B78, init/final-xor 0xFFFFFFFF.
 *       Check("123456789") == 0xE3069283. s == NULL -> 0.
 *       Empty string "" -> 0 (init ^ final-xor).
 *   uint32_t __gj_str_hash_crc32c_u  (alias)
 *   __libcgj_batch4021_marker = "libcgj-batch4021"
 *
 * Exclusive continuum CREATE-ONLY (4021-4030: str_hash_crc32c_u,
 * str_hash_adler_u, buf_hash_crc32c_u, buf_hash_adler_u, u32_mix_final_u,
 * u64_mix_final_u, u32_avalanche_u, u64_avalanche_u, u32_scramble_u,
 * batch_id_4030). Unique gj_str_hash_crc32c_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4021_marker[] = "libcgj-batch4021";

/* Castagnoli CRC-32C reflected polynomial. */
#define B4021_CRC32C_POLY 0x82F63B78u
#define B4021_CRC32C_INIT 0xFFFFFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

/* Fold one octet into a running reflected CRC-32C. */
static uint32_t
b4021_crc32c_byte(uint32_t u32Crc, unsigned char u8B)
{
	unsigned uBit;

	u32Crc ^= (uint32_t)u8B;
	for (uBit = 0u; uBit < 8u; uBit++) {
		if ((u32Crc & 1u) != 0u) {
			u32Crc = (u32Crc >> 1) ^ B4021_CRC32C_POLY;
		} else {
			u32Crc >>= 1;
		}
	}
	return u32Crc;
}

/* One-shot CRC-32C over a NUL-terminated string (NUL not hashed). */
static uint32_t
b4021_str_crc32c(const char *sz)
{
	uint32_t u32Crc;
	size_t i;

	u32Crc = B4021_CRC32C_INIT;
	i = 0u;
	while (sz[i] != '\0') {
		u32Crc = b4021_crc32c_byte(u32Crc, (unsigned char)sz[i]);
		i++;
	}
	return u32Crc ^ B4021_CRC32C_INIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_hash_crc32c_u - Castagnoli CRC-32C of a NUL-terminated string.
 *
 * s: NUL-terminated C string, or NULL
 *
 * Returns the 32-bit CRC-32C digest. NULL yields 0. Empty string yields 0.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_str_hash_crc32c_u(const char *s)
{
	(void)NULL;
	if (s == NULL) {
		return 0u;
	}
	return b4021_str_crc32c(s);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_hash_crc32c_u(const char *s)
    __attribute__((alias("gj_str_hash_crc32c_u")));
