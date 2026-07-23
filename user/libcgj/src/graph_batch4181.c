/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4181: unsigned 8-bit modular sum checksum.
 *
 * Surface (unique symbols):
 *   uint8_t gj_u8_sum_checksum_u(const uint8_t *p, size_t n);
 *     - Sum n bytes at p modulo 256 (wrap in uint8_t).
 *       NULL or n == 0 → 0.
 *   uint8_t __gj_u8_sum_checksum_u  (alias)
 *   __libcgj_batch4181_marker = "libcgj-batch4181"
 *
 * Exclusive continuum CREATE-ONLY (4181-4190: u8_sum_checksum_u,
 * u16_sum_checksum_u, u32_sum_checksum_u, xor_reduce8_u, xor_reduce32_u,
 * ones_complement_sum16_u, fletcher16_u, fletcher32_u, luhn_u32_u,
 * batch_id_4190). Unique gj_u8_sum_checksum_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4181_marker[] = "libcgj-batch4181";

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b4181_sum8(const uint8_t *pData, size_t cbN)
{
	uint8_t u8Sum;
	size_t i;

	u8Sum = 0u;
	for (i = 0u; i < cbN; i++) {
		u8Sum = (uint8_t)(u8Sum + pData[i]);
	}
	return u8Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_sum_checksum_u - sum of n bytes modulo 256.
 *
 * p: byte buffer (NULL only valid when n == 0)
 * n: length in bytes
 *
 * Returns the 8-bit wrapping sum. NULL or empty yields 0.
 * Self-contained; no parent wires.
 */
uint8_t
gj_u8_sum_checksum_u(const uint8_t *p, size_t n)
{
	(void)NULL;
	if (p == NULL || n == 0u) {
		return 0u;
	}
	return b4181_sum8(p, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_u8_sum_checksum_u(const uint8_t *p, size_t n)
    __attribute__((alias("gj_u8_sum_checksum_u")));
