/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1421: gj_sum16_bytes — 16-bit additive byte sum.
 *
 * Surface (unique symbols — do NOT collide with gj_sum32_n / gj_sum_u32):
 *   uint16_t gj_sum16_bytes(const void *data, size_t n);
 *     — Sum of n bytes as uint16_t (natural wrap mod 2^16).
 *       data == NULL → 0. n == 0 → 0.
 *   uint16_t __gj_sum16_bytes  (alias)
 *   __libcgj_batch1421_marker = "libcgj-batch1421"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1421_marker[] = "libcgj-batch1421";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sum16_bytes — sum of bytes mod 2^16.
 */
uint16_t
gj_sum16_bytes(const void *pData, size_t cb)
{
	const unsigned char *p;
	uint16_t u16Sum;
	size_t i;

	if (pData == NULL || cb == 0u) {
		return 0u;
	}

	p = (const unsigned char *)pData;
	u16Sum = 0u;
	for (i = 0u; i < cb; i++) {
		u16Sum = (uint16_t)(u16Sum + (uint16_t)p[i]);
	}
	return u16Sum;
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_sum16_bytes(const void *pData, size_t cb)
    __attribute__((alias("gj_sum16_bytes")));
