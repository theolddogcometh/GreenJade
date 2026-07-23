/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3836: 16-bit additive checksum (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_checksum_sum16_u(const void *data, size_t n);
 *     - One-shot sum of n octets into a uint16_t accumulator starting
 *       at 0 (natural wrap mod 2^16). data == NULL or n == 0 → 0.
 *   uint16_t __gj_checksum_sum16_u  (alias)
 *   __libcgj_batch3836_marker = "libcgj-batch3836"
 *
 * Exclusive continuum CREATE-ONLY (3831-3840). Distinct from
 * gj_sum16_bytes (batch1421) — unique checksum_sum16_u surface; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3836_marker[] = "libcgj-batch3836";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b3836_sum16(const unsigned char *pData, size_t cbN)
{
	uint16_t u16Acc;
	size_t iByte;

	u16Acc = 0u;
	for (iByte = 0u; iByte < cbN; iByte++) {
		u16Acc = (uint16_t)(u16Acc + (uint16_t)pData[iByte]);
	}
	return u16Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_checksum_sum16_u - one-shot 16-bit additive checksum over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0, else defensive 0)
 * n:    byte count
 *
 * Returns the sum of all octets mod 2^16 (empty/NULL → 0).
 * No parent wires.
 */
uint16_t
gj_checksum_sum16_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	if (pData == NULL || cbN == 0u) {
		return 0u;
	}
	p = (const unsigned char *)pData;
	return b3836_sum16(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_checksum_sum16_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_checksum_sum16_u")));
