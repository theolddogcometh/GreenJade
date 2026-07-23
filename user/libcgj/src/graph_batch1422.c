/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1422: gj_sum32_bytes — 32-bit additive byte sum.
 *
 * Surface (unique symbols — do NOT collide with gj_sum32_n / gj_sum_u32):
 *   uint32_t gj_sum32_bytes(const void *data, size_t n);
 *     — Sum of n bytes as uint32_t (natural wrap mod 2^32).
 *       data == NULL → 0. n == 0 → 0.
 *   uint32_t __gj_sum32_bytes  (alias)
 *   __libcgj_batch1422_marker = "libcgj-batch1422"
 *
 * Distinct from gj_sum32_n (batch784): same math, different public name
 * for the checksum-family graph surface.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1422_marker[] = "libcgj-batch1422";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sum32_bytes — sum of bytes mod 2^32.
 */
uint32_t
gj_sum32_bytes(const void *pData, size_t cb)
{
	const unsigned char *p;
	uint32_t u32Sum;
	size_t i;

	if (pData == NULL || cb == 0u) {
		return 0u;
	}

	p = (const unsigned char *)pData;
	u32Sum = 0u;
	for (i = 0u; i < cb; i++) {
		u32Sum += (uint32_t)p[i];
	}
	return u32Sum;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sum32_bytes(const void *pData, size_t cb)
    __attribute__((alias("gj_sum32_bytes")));
