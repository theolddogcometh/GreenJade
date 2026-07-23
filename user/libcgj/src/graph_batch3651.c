/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3651: gj_crc32c_update_u - CRC-32C stream update.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc32c_update_u(uint32_t state, const void *data, size_t n);
 *     - Fold n bytes into a running CRC-32C (Castagnoli) internal state
 *       (reflected poly 0x82F63B78). State is post-init / pre-final-XOR:
 *       start with 0xFFFFFFFF for a new stream; pass result of prior
 *       update for chaining. data == NULL or n == 0 → state unchanged.
 *   uint32_t __gj_crc32c_update_u  (alias)
 *   __libcgj_batch3651_marker = "libcgj-batch3651"
 *
 * Hash/checksum exclusive wave (3651-3660). Companion finalizer is
 * gj_crc32c_final_u (batch3652). Distinct from gj_crc32c_sw (batch471)
 * one-shot and gj_crc32_update_u (batch2651 IEEE) — unique _u Castagnoli
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3651_marker[] = "libcgj-batch3651";

/* Castagnoli reflected polynomial (iSCSI / SSE4.2 crc32). */
#define B3651_POLY 0x82F63B78u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3651_step_byte(uint32_t u32Crc, unsigned char u8Byte)
{
	unsigned uBit;

	u32Crc ^= (uint32_t)u8Byte;
	for (uBit = 0u; uBit < 8u; uBit++) {
		if ((u32Crc & 1u) != 0u) {
			u32Crc = (u32Crc >> 1) ^ B3651_POLY;
		} else {
			u32Crc >>= 1;
		}
	}
	return u32Crc;
}

static uint32_t
b3651_update(uint32_t u32State, const unsigned char *pData, size_t cbN)
{
	size_t iByte;

	for (iByte = 0u; iByte < cbN; iByte++) {
		u32State = b3651_step_byte(u32State, pData[iByte]);
	}
	return u32State;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc32c_update_u - fold n bytes into a running CRC-32C state.
 *
 * state: internal CRC (init 0xFFFFFFFF for a new stream)
 * data:  input bytes (NULL is a no-op)
 * n:     byte count (0 is a no-op)
 *
 * Returns the updated internal state. Call gj_crc32c_final_u to apply
 * the final complement for the wire/value CRC-32C.
 */
uint32_t
gj_crc32c_update_u(uint32_t u32State, const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	if (pData == NULL || cbN == 0u) {
		return u32State;
	}
	p = (const unsigned char *)pData;
	return b3651_update(u32State, p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc32c_update_u(uint32_t u32State, const void *pData, size_t cbN)
    __attribute__((alias("gj_crc32c_update_u")));
