/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3838: 16-bit XOR checksum (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_checksum_xor16_u(const void *data, size_t n);
 *     - One-shot XOR of all n octets (zero-extended) into a uint16_t
 *       accumulator starting at 0. data == NULL or n == 0 → 0.
 *   uint16_t __gj_checksum_xor16_u  (alias)
 *   __libcgj_batch3838_marker = "libcgj-batch3838"
 *
 * Exclusive continuum CREATE-ONLY (3831-3840). Distinct from
 * gj_xor16_fold (batch1423) and gj_checksum_xor8_u (batch2659) —
 * unique checksum_xor16_u surface; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3838_marker[] = "libcgj-batch3838";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b3838_xor16(const unsigned char *pData, size_t cbN)
{
	uint16_t u16Acc;
	size_t iByte;

	u16Acc = 0u;
	for (iByte = 0u; iByte < cbN; iByte++) {
		u16Acc = (uint16_t)(u16Acc ^ (uint16_t)pData[iByte]);
	}
	return u16Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_checksum_xor16_u - one-shot 16-bit XOR checksum over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0, else defensive 0)
 * n:    byte count
 *
 * Returns the XOR of all octets into uint16_t (empty/NULL → 0).
 * No parent wires.
 */
uint16_t
gj_checksum_xor16_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	if (pData == NULL || cbN == 0u) {
		return 0u;
	}
	p = (const unsigned char *)pData;
	return b3838_xor16(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_checksum_xor16_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_checksum_xor16_u")));
