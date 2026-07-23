/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3463: freestanding NUL-terminated IEEE CRC-32
 * string hash.
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_hash_crc32_u(const char *s);
 *     - IEEE CRC-32 (poly 0xEDB88320 reflected) over the C string at s
 *       until NUL. Init 0xFFFFFFFF, xorout 0xFFFFFFFF (ISO HDLC /
 *       Ethernet / gzip shape). NULL s -> 0. Empty string -> 0
 *       (0xFFFFFFFF ^ 0xFFFFFFFF).
 *   uint32_t __gj_str_hash_crc32_u  (alias)
 *   __libcgj_batch3463_marker = "libcgj-batch3463"
 *
 * CREATE-ONLY exclusive continuum wave (3461-3470). Unique
 * gj_str_hash_crc32_u surface only; no multi-def. Distinct from crc32 /
 * crc32_z (batch39, length-bounded mem CRC). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Bit-by-bit reflected poly (no
 * table) for freestanding purity.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3463_marker[] = "libcgj-batch3463";

/* IEEE CRC-32 reflected polynomial. */
#define B3463_POLY   0xEDB88320u
#define B3463_INIT   0xFFFFFFFFu
#define B3463_XOROUT 0xFFFFFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

/* Bit-by-bit reflected CRC-32 over a NUL-terminated string. NULL -> 0. */
static uint32_t
b3463_crc32(const char *sz)
{
	const unsigned char *p;
	uint32_t u32Crc;
	unsigned uBit;

	if (sz == NULL) {
		return 0u;
	}
	p = (const unsigned char *)sz;
	u32Crc = B3463_INIT;
	while (*p != 0u) {
		u32Crc ^= (uint32_t)(*p);
		for (uBit = 0u; uBit < 8u; uBit++) {
			if ((u32Crc & 1u) != 0u) {
				u32Crc = (u32Crc >> 1) ^ B3463_POLY;
			} else {
				u32Crc >>= 1;
			}
		}
		p++;
	}
	return u32Crc ^ B3463_XOROUT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_hash_crc32_u - IEEE CRC-32 of a NUL-terminated C string.
 *
 * s: C string (NULL -> 0)
 *
 * Returns the 32-bit CRC. Empty string returns 0. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_str_hash_crc32_u(const char *sz)
{
	(void)NULL;
	return b3463_crc32(sz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_hash_crc32_u(const char *sz)
    __attribute__((alias("gj_str_hash_crc32_u")));
