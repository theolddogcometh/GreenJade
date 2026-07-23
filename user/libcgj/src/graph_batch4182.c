/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4182: unsigned 16-bit wrapping sum checksum.
 *
 * Surface (unique symbols):
 *   uint16_t gj_u16_sum_checksum_u(const uint8_t *p, size_t n);
 *     - Sum n bytes at p as a uint16_t with wraparound.
 *       NULL or n == 0 → 0.
 *   uint16_t __gj_u16_sum_checksum_u  (alias)
 *   __libcgj_batch4182_marker = "libcgj-batch4182"
 *
 * Exclusive continuum CREATE-ONLY (4181-4190: u8_sum_checksum_u,
 * u16_sum_checksum_u, u32_sum_checksum_u, xor_reduce8_u, xor_reduce32_u,
 * ones_complement_sum16_u, fletcher16_u, fletcher32_u, luhn_u32_u,
 * batch_id_4190). Unique gj_u16_sum_checksum_u surface only; no multi-def.
 * Distinct from gj_u8_sum_checksum_u (batch4181). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4182_marker[] = "libcgj-batch4182";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b4182_sum16(const uint8_t *pData, size_t cbN)
{
	uint16_t u16Sum;
	size_t i;

	u16Sum = 0u;
	for (i = 0u; i < cbN; i++) {
		u16Sum = (uint16_t)(u16Sum + (uint16_t)pData[i]);
	}
	return u16Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_sum_checksum_u - sum of n bytes as a wrapping uint16_t.
 *
 * p: byte buffer (NULL only valid when n == 0)
 * n: length in bytes
 *
 * Returns the 16-bit wrapping sum. NULL or empty yields 0.
 * Self-contained; no parent wires.
 */
uint16_t
gj_u16_sum_checksum_u(const uint8_t *p, size_t n)
{
	(void)NULL;
	if (p == NULL || n == 0u) {
		return 0u;
	}
	return b4182_sum16(p, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_u16_sum_checksum_u(const uint8_t *p, size_t n)
    __attribute__((alias("gj_u16_sum_checksum_u")));
