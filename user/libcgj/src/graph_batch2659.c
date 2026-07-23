/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2659: gj_checksum_xor8_u - 8-bit XOR checksum.
 *
 * Surface (unique symbols):
 *   uint8_t gj_checksum_xor8_u(const void *data, size_t n);
 *     - One-shot XOR of all n octets into a single uint8_t accumulator
 *       starting at 0. data == NULL or n == 0 → 0.
 *   uint8_t __gj_checksum_xor8_u  (alias)
 *   __libcgj_batch2659_marker = "libcgj-batch2659"
 *
 * Streaming checksum wave (2651-2660). Distinct from gj_xor32_n
 * (batch785) and gj_xor16_fold (batch1423) — unique 8-bit surface.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2659_marker[] = "libcgj-batch2659";

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b2659_xor8(const unsigned char *pData, size_t cbN)
{
	uint8_t u8Acc;
	size_t iByte;

	u8Acc = 0u;
	for (iByte = 0u; iByte < cbN; iByte++) {
		u8Acc = (uint8_t)(u8Acc ^ pData[iByte]);
	}
	return u8Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_checksum_xor8_u - one-shot 8-bit XOR checksum over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0, else defensive 0)
 * n:    byte count
 *
 * Returns the XOR of all octets (empty/NULL → 0).
 */
uint8_t
gj_checksum_xor8_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	if (pData == NULL || cbN == 0u) {
		return 0u;
	}
	p = (const unsigned char *)pData;
	return b2659_xor8(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_checksum_xor8_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_checksum_xor8_u")));
