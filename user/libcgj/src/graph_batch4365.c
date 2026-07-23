/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4365: gj_crc32_update_byte_u - IEEE CRC-32 byte.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc32_update_byte_u(uint32_t crc, uint8_t byte);
 *     - Fold one byte into a running IEEE CRC-32 internal state using
 *       reflected poly 0xEDB88320, soft tableless bit loop. State is
 *       post-init / pre-final-XOR (start with 0xFFFFFFFF for a new stream).
 *   uint32_t __gj_crc32_update_byte_u  (alias)
 *   __libcgj_batch4365_marker = "libcgj-batch4365"
 *
 * Exclusive continuum CREATE-ONLY (4361-4370). Distinct from
 * gj_crc32_update_u (batch2651 buffer fold) — unique
 * gj_crc32_update_byte_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4365_marker[] = "libcgj-batch4365";

/* IEEE / zlib CRC-32 reflected polynomial. */
#define B4365_POLY 0xEDB88320u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4365_step(uint32_t u32Crc, uint8_t u8Byte)
{
	unsigned uBit;

	u32Crc ^= (uint32_t)u8Byte;
	for (uBit = 0u; uBit < 8u; uBit++) {
		if ((u32Crc & 1u) != 0u) {
			u32Crc = (u32Crc >> 1) ^ B4365_POLY;
		} else {
			u32Crc >>= 1;
		}
	}
	return u32Crc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc32_update_byte_u - fold one byte into IEEE CRC-32 (tableless).
 *
 * crc:  running internal CRC (init 0xFFFFFFFF for a new stream)
 * byte: input byte
 *
 * Returns the updated internal state. Call gj_crc32_finalize_u
 * (batch4367) for the wire/value CRC-32. Self-contained; no parent wires.
 */
uint32_t
gj_crc32_update_byte_u(uint32_t u32Crc, uint8_t u8Byte)
{
	(void)NULL;
	return b4365_step(u32Crc, u8Byte);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc32_update_byte_u(uint32_t u32Crc, uint8_t u8Byte)
    __attribute__((alias("gj_crc32_update_byte_u")));
