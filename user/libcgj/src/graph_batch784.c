/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch784: simple byte sum modulo 2^32.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sum32_n(const void *data, size_t n);
 *     — Sum of n bytes as uint32_t (natural wrap mod 2^32).
 *       data == NULL → 0. n == 0 → 0.
 *   uint32_t __gj_sum32_n  (alias)
 *   __libcgj_batch784_marker = "libcgj-batch784"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch784_marker[] = "libcgj-batch784";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sum32_n — sum of bytes mod 2^32.
 */
uint32_t
gj_sum32_n(const void *pData, size_t cb)
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

uint32_t __gj_sum32_n(const void *pData, size_t cb)
    __attribute__((alias("gj_sum32_n")));
