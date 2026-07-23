/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3124: gj_checksum_add16_u - 16-bit additive sum.
 *
 * Surface (unique symbols):
 *   uint16_t gj_checksum_add16_u(const void *data, size_t n);
 *     - Sum of each byte mod 2^16 over data[0..n). NULL data with
 *       n != 0 -> 0. Empty (n == 0) -> 0.
 *   uint16_t __gj_checksum_add16_u  (alias)
 *   __libcgj_batch3124_marker = "libcgj-batch3124"
 *
 * Exclusive checksum/net CREATE-ONLY wave (3121-3130). Distinct from
 * gj_sum16_bytes (batch1421) / gj_checksum_add8_u (batch3123) — unique
 * gj_checksum_add16_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3124_marker[] = "libcgj-batch3124";

/* ---- freestanding helpers ---------------------------------------------- */

/* Sum of cb bytes at p, reduced mod 2^16. */
static uint16_t
b3124_add16(const unsigned char *p, size_t cb)
{
	uint16_t uSum;
	size_t i;

	uSum = 0u;
	for (i = 0u; i < cb; i++) {
		uSum = (uint16_t)(uSum + (uint16_t)p[i]);
	}
	return uSum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_checksum_add16_u - modular 16-bit additive checksum over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0)
 * n:    byte count
 *
 * Returns sum of bytes mod 2^16. NULL data with n != 0 yields 0.
 * Empty input yields 0. No parent wires.
 */
uint16_t
gj_checksum_add16_u(const void *pData, size_t cbN)
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
	return b3124_add16(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_checksum_add16_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_checksum_add16_u")));
