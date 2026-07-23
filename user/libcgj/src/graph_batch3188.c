/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3188: XOR-reduce of a uint8_t buffer (_u).
 *
 * Surface (unique symbols):
 *   uint8_t gj_buf_xor_u8_u(const uint8_t *a, size_t n);
 *     - Return the bitwise XOR of all octets in a[0..n). Empty range or
 *       NULL a -> 0 (identity of XOR). NULL a with n > 0 -> 0.
 *   uint8_t __gj_buf_xor_u8_u  (alias)
 *   __libcgj_batch3188_marker = "libcgj-batch3188"
 *
 * CREATE-ONLY exclusive continuum wave (3181-3190). Unique
 * gj_buf_xor_u8_u surface only; no multi-def. Distinct from pairwise
 * mem_xor helpers. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3188_marker[] = "libcgj-batch3188";

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b3188_xor(const uint8_t *pA, size_t cN)
{
	uint8_t u8X;
	size_t i;

	if (pA == NULL || cN == 0u) {
		return 0u;
	}
	u8X = 0u;
	for (i = 0u; i < cN; i++) {
		u8X = (uint8_t)(u8X ^ pA[i]);
	}
	return u8X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_xor_u8_u - XOR-fold of a[0..n).
 *
 * a: byte buffer (NULL only legal when n == 0; otherwise -> 0)
 * n: element count
 *
 * Returns the XOR reduction (identity 0). Does not call libc.
 * No parent wires.
 */
uint8_t
gj_buf_xor_u8_u(const uint8_t *pA, size_t cN)
{
	(void)NULL;
	return b3188_xor(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_buf_xor_u8_u(const uint8_t *pA, size_t cN)
    __attribute__((alias("gj_buf_xor_u8_u")));
