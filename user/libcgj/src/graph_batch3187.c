/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3187: modular sum of uint8_t buffer (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_buf_sum_u8_u(const uint8_t *a, size_t n);
 *     - Return the sum of a[0..n) as uint32_t (mod 2^32). NULL a with
 *       n == 0 -> 0; NULL a with n > 0 -> 0 (defensive).
 *   uint32_t __gj_buf_sum_u8_u  (alias)
 *   __libcgj_batch3187_marker = "libcgj-batch3187"
 *
 * CREATE-ONLY exclusive continuum wave (3181-3190). Unique
 * gj_buf_sum_u8_u surface only; no multi-def. Distinct from
 * gj_sum32_n / gj_sum32_bytes. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3187_marker[] = "libcgj-batch3187";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3187_sum(const uint8_t *pA, size_t cN)
{
	uint32_t u32S;
	size_t i;

	if (pA == NULL || cN == 0u) {
		return 0u;
	}
	u32S = 0u;
	for (i = 0u; i < cN; i++) {
		u32S += (uint32_t)pA[i];
	}
	return u32S;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_sum_u8_u - sum of bytes in a[0..n), mod 2^32.
 *
 * a: byte buffer (NULL only legal when n == 0; otherwise -> 0)
 * n: element count
 *
 * Returns the modular sum. Does not call libc. No parent wires.
 */
uint32_t
gj_buf_sum_u8_u(const uint8_t *pA, size_t cN)
{
	(void)NULL;
	return b3187_sum(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_buf_sum_u8_u(const uint8_t *pA, size_t cN)
    __attribute__((alias("gj_buf_sum_u8_u")));
