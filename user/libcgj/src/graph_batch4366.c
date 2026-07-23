/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4366: gj_crc32_buf_u - IEEE CRC-32 buffer update.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc32_buf_u(const void *p, size_t n, uint32_t init);
 *     - Fold n bytes into an IEEE CRC-32 internal state starting from init
 *       (reflected poly 0xEDB88320, soft tableless). p == NULL → return init.
 *       Init 0xFFFFFFFF for a new stream; finalize with gj_crc32_finalize_u.
 *   uint32_t __gj_crc32_buf_u  (alias)
 *   __libcgj_batch4366_marker = "libcgj-batch4366"
 *
 * Exclusive continuum CREATE-ONLY (4361-4370). Companion single-byte
 * update is gj_crc32_update_byte_u (batch4365). Distinct from
 * gj_crc32_update_u (batch2651) — unique gj_crc32_buf_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4366_marker[] = "libcgj-batch4366";

/* IEEE / zlib CRC-32 reflected polynomial. */
#define B4366_POLY 0xEDB88320u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4366_step(uint32_t u32Crc, uint8_t u8Byte)
{
	unsigned uBit;

	u32Crc ^= (uint32_t)u8Byte;
	for (uBit = 0u; uBit < 8u; uBit++) {
		if ((u32Crc & 1u) != 0u) {
			u32Crc = (u32Crc >> 1) ^ B4366_POLY;
		} else {
			u32Crc >>= 1;
		}
	}
	return u32Crc;
}

static uint32_t
b4366_buf(const unsigned char *pData, size_t cbN, uint32_t u32Init)
{
	uint32_t u32Crc;
	size_t iByte;

	u32Crc = u32Init;
	for (iByte = 0u; iByte < cbN; iByte++) {
		u32Crc = b4366_step(u32Crc, pData[iByte]);
	}
	return u32Crc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc32_buf_u - fold n bytes into IEEE CRC-32 internal state from init.
 *
 * p:    input bytes (NULL → return init)
 * n:    byte count (0 → return init)
 * init: starting internal CRC (0xFFFFFFFF for a new stream)
 *
 * Returns the updated internal state. Finalize with gj_crc32_finalize_u.
 * Self-contained; no parent wires.
 */
uint32_t
gj_crc32_buf_u(const void *pData, size_t cbN, uint32_t u32Init)
{
	const unsigned char *p;

	(void)NULL;
	if (pData == NULL) {
		return u32Init;
	}
	p = (const unsigned char *)pData;
	return b4366_buf(p, cbN, u32Init);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc32_buf_u(const void *pData, size_t cbN, uint32_t u32Init)
    __attribute__((alias("gj_crc32_buf_u")));
