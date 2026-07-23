/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2651: gj_crc32_update_u - IEEE CRC-32 stream update.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc32_update_u(uint32_t state, const void *data, size_t n);
 *     - Fold n bytes into a running IEEE CRC-32 internal state
 *       (reflected poly 0xEDB88320). State is post-init / pre-final-XOR:
 *       start with 0xFFFFFFFF for a new stream; pass result of prior
 *       update for chaining. data == NULL or n == 0 → state unchanged.
 *   uint32_t __gj_crc32_update_u  (alias)
 *   __libcgj_batch2651_marker = "libcgj-batch2651"
 *
 * Streaming checksum wave (2651-2660). Companion finalizer is
 * gj_crc32_final_u (batch2652). Does NOT redefine crc32 / crc32_z /
 * gj_crc32_jamcrc_u — unique _update_u surface only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2651_marker[] = "libcgj-batch2651";

/* IEEE / zlib CRC-32 reflected polynomial. */
#define B2651_POLY 0xEDB88320u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2651_step_byte(uint32_t u32Crc, unsigned char u8Byte)
{
	unsigned uBit;

	u32Crc ^= (uint32_t)u8Byte;
	for (uBit = 0u; uBit < 8u; uBit++) {
		if ((u32Crc & 1u) != 0u) {
			u32Crc = (u32Crc >> 1) ^ B2651_POLY;
		} else {
			u32Crc >>= 1;
		}
	}
	return u32Crc;
}

static uint32_t
b2651_update(uint32_t u32State, const unsigned char *pData, size_t cbN)
{
	size_t iByte;

	for (iByte = 0u; iByte < cbN; iByte++) {
		u32State = b2651_step_byte(u32State, pData[iByte]);
	}
	return u32State;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc32_update_u - fold n bytes into a running IEEE CRC-32 state.
 *
 * state: internal CRC (init 0xFFFFFFFF for a new stream)
 * data:  input bytes (NULL is a no-op)
 * n:     byte count (0 is a no-op)
 *
 * Returns the updated internal state. Call gj_crc32_final_u to apply
 * the final complement for the wire/value CRC-32.
 */
uint32_t
gj_crc32_update_u(uint32_t u32State, const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	if (pData == NULL || cbN == 0u) {
		return u32State;
	}
	p = (const unsigned char *)pData;
	return b2651_update(u32State, p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc32_update_u(uint32_t u32State, const void *pData, size_t cbN)
    __attribute__((alias("gj_crc32_update_u")));
