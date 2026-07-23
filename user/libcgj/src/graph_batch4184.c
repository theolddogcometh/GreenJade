/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4184: 8-bit XOR reduce over a byte buffer.
 *
 * Surface (unique symbols):
 *   uint8_t gj_xor_reduce8_u(const uint8_t *p, size_t n);
 *     - Fold n bytes with bitwise XOR (identity 0).
 *       NULL or n == 0 → 0.
 *   uint8_t __gj_xor_reduce8_u  (alias)
 *   __libcgj_batch4184_marker = "libcgj-batch4184"
 *
 * Exclusive continuum CREATE-ONLY (4181-4190: u8_sum_checksum_u,
 * u16_sum_checksum_u, u32_sum_checksum_u, xor_reduce8_u, xor_reduce32_u,
 * ones_complement_sum16_u, fletcher16_u, fletcher32_u, luhn_u32_u,
 * batch_id_4190). Unique gj_xor_reduce8_u surface only; no multi-def.
 * Distinct from gj_u32_xor_reduce / gj_u64_xor_reduce (word arrays).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4184_marker[] = "libcgj-batch4184";

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b4184_xor8(const uint8_t *pData, size_t cbN)
{
	uint8_t u8Acc;
	size_t i;

	u8Acc = 0u;
	for (i = 0u; i < cbN; i++) {
		u8Acc = (uint8_t)(u8Acc ^ pData[i]);
	}
	return u8Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xor_reduce8_u - XOR-fold n bytes into a single octet.
 *
 * p: byte buffer (NULL only valid when n == 0)
 * n: length in bytes
 *
 * Returns the XOR of all bytes. NULL or empty yields 0.
 * Self-contained; no parent wires.
 */
uint8_t
gj_xor_reduce8_u(const uint8_t *p, size_t n)
{
	(void)NULL;
	if (p == NULL || n == 0u) {
		return 0u;
	}
	return b4184_xor8(p, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_xor_reduce8_u(const uint8_t *p, size_t n)
    __attribute__((alias("gj_xor_reduce8_u")));
