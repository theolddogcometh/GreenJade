/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2387: 8-bit modular byte sum unique surface
 * (post-2380 checksum exclusive wave).
 *
 * Surface (unique symbols):
 *   uint8_t gj_sum8_bytes(const void *data, size_t n);
 *     - Sum of each byte mod 256 over data[0..n). NULL data with
 *       n != 0 -> 0. Empty (n == 0) -> 0.
 *   uint8_t __gj_sum8_bytes  (alias)
 *   __libcgj_batch2387_marker = "libcgj-batch2387"
 *
 * Post-2380 checksum exclusive wave (2381-2390). Simple additive sum
 * distinct from CRC / Adler / Fletcher surfaces; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2387_marker[] = "libcgj-batch2387";

/* ---- freestanding helpers ---------------------------------------------- */

/* Sum of cb bytes at p, reduced mod 256. */
static uint8_t
b2387_sum8(const unsigned char *p, size_t cb)
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
 * gj_sum8_bytes - modular 8-bit sum of n bytes.
 *
 * data: input bytes (NULL only legal when n == 0)
 * n:    byte count
 *
 * Returns sum of bytes mod 256. NULL data with n != 0 yields 0.
 * Empty input yields 0. Does not call libc.
 */
uint8_t
gj_sum8_bytes(const void *pData, size_t cbN)
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
	return b2387_sum8(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_sum8_bytes(const void *pData, size_t cbN)
    __attribute__((alias("gj_sum8_bytes")));
