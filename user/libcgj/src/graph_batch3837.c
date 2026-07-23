/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3837: 32-bit additive checksum (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_checksum_sum32_u(const void *data, size_t n);
 *     - One-shot sum of n octets into a uint32_t accumulator starting
 *       at 0 (natural wrap mod 2^32). data == NULL or n == 0 → 0.
 *   uint32_t __gj_checksum_sum32_u  (alias)
 *   __libcgj_batch3837_marker = "libcgj-batch3837"
 *
 * Exclusive continuum CREATE-ONLY (3831-3840). Distinct from
 * gj_sum32_n (batch784) and gj_sum32_bytes (batch1422) — unique
 * checksum_sum32_u surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3837_marker[] = "libcgj-batch3837";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3837_sum32(const unsigned char *pData, size_t cbN)
{
	uint32_t u32Acc;
	size_t iByte;

	u32Acc = 0u;
	for (iByte = 0u; iByte < cbN; iByte++) {
		u32Acc += (uint32_t)pData[iByte];
	}
	return u32Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_checksum_sum32_u - one-shot 32-bit additive checksum over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0, else defensive 0)
 * n:    byte count
 *
 * Returns the sum of all octets mod 2^32 (empty/NULL → 0).
 * No parent wires.
 */
uint32_t
gj_checksum_sum32_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	if (pData == NULL || cbN == 0u) {
		return 0u;
	}
	p = (const unsigned char *)pData;
	return b3837_sum32(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_checksum_sum32_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_checksum_sum32_u")));
