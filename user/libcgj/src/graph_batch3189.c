/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3189: maximum of a uint8_t buffer (_u).
 *
 * Surface (unique symbols):
 *   uint8_t gj_buf_max_u8_u(const uint8_t *a, size_t n);
 *     - Return the maximum octet in a[0..n). Empty range or NULL a -> 0
 *       (defined sentinel; there is no element to maximize).
 *   uint8_t __gj_buf_max_u8_u  (alias)
 *   __libcgj_batch3189_marker = "libcgj-batch3189"
 *
 * CREATE-ONLY exclusive continuum wave (3181-3190). Unique
 * gj_buf_max_u8_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3189_marker[] = "libcgj-batch3189";

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b3189_max(const uint8_t *pA, size_t cN)
{
	uint8_t u8M;
	size_t i;

	if (pA == NULL || cN == 0u) {
		return 0u;
	}
	u8M = pA[0];
	for (i = 1u; i < cN; i++) {
		if (pA[i] > u8M) {
			u8M = pA[i];
		}
	}
	return u8M;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_max_u8_u - maximum octet of a[0..n).
 *
 * a: byte buffer (NULL or n == 0 -> 0 sentinel)
 * n: element count
 *
 * Returns the max value, or 0 when the range is empty/invalid.
 * Does not call libc. No parent wires.
 */
uint8_t
gj_buf_max_u8_u(const uint8_t *pA, size_t cN)
{
	(void)NULL;
	return b3189_max(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_buf_max_u8_u(const uint8_t *pA, size_t cN)
    __attribute__((alias("gj_buf_max_u8_u")));
