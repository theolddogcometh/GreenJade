/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3123: gj_checksum_add8_u - 8-bit additive sum.
 *
 * Surface (unique symbols):
 *   uint8_t gj_checksum_add8_u(const void *data, size_t n);
 *     - Sum of each byte mod 256 over data[0..n). NULL data with
 *       n != 0 -> 0. Empty (n == 0) -> 0.
 *   uint8_t __gj_checksum_add8_u  (alias)
 *   __libcgj_batch3123_marker = "libcgj-batch3123"
 *
 * Exclusive checksum/net CREATE-ONLY wave (3121-3130). Distinct from
 * gj_sum8_bytes (batch2387) / gj_checksum_xor8_u (batch2659) — unique
 * gj_checksum_add8_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3123_marker[] = "libcgj-batch3123";

/* ---- freestanding helpers ---------------------------------------------- */

/* Sum of cb bytes at p, reduced mod 256. */
static uint8_t
b3123_add8(const unsigned char *p, size_t cb)
{
	uint8_t uSum;
	size_t i;

	uSum = 0u;
	for (i = 0u; i < cb; i++) {
		uSum = (uint8_t)(uSum + p[i]);
	}
	return uSum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_checksum_add8_u - modular 8-bit additive checksum over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0)
 * n:    byte count
 *
 * Returns sum of bytes mod 256. NULL data with n != 0 yields 0.
 * Empty input yields 0. No parent wires.
 */
uint8_t
gj_checksum_add8_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return 0u;
	}
	return b3123_add8(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_checksum_add8_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_checksum_add8_u")));
