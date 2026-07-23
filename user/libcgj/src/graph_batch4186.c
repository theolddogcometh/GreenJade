/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4186: IP-style 16-bit ones-complement checksum.
 *
 * Surface (unique symbols):
 *   uint16_t gj_ones_complement_sum16_u(const uint8_t *p, size_t n);
 *     - Classic Internet ones-complement 16-bit sum over n bytes.
 *       Words are network (big-endian) order; a trailing odd byte is
 *       padded with a zero low-order octet. Returns ~folded_sum
 *       (value to store in a zeroed checksum field).
 *       NULL or n == 0 → 0.
 *   uint16_t __gj_ones_complement_sum16_u  (alias)
 *   __libcgj_batch4186_marker = "libcgj-batch4186"
 *
 * Exclusive continuum CREATE-ONLY (4181-4190: u8_sum_checksum_u,
 * u16_sum_checksum_u, u32_sum_checksum_u, xor_reduce8_u, xor_reduce32_u,
 * ones_complement_sum16_u, fletcher16_u, fletcher32_u, luhn_u32_u,
 * batch_id_4190). Unique gj_ones_complement_sum16_u surface only; no
 * multi-def. Distinct from gj_ip_checksum (batch124) — unique symbol.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4186_marker[] = "libcgj-batch4186";

/* ---- freestanding helpers ---------------------------------------------- */

/* End-around carry fold of a multi-word sum into 16 bits. */
static uint16_t
b4186_fold(uint32_t u32Sum)
{
	while ((u32Sum >> 16) != 0u) {
		u32Sum = (u32Sum & 0xffffu) + (u32Sum >> 16);
	}
	return (uint16_t)u32Sum;
}

/*
 * Ones-complement sum over network-order 16-bit words, then invert.
 * Odd final byte is high-order octet with low-order zero padding.
 */
static uint16_t
b4186_oc16(const uint8_t *pData, size_t cbN)
{
	uint32_t u32Sum;
	size_t i;
	uint16_t u16Folded;

	u32Sum = 0u;
	i = 0u;
	while (i + 1u < cbN) {
		u32Sum += ((uint32_t)pData[i] << 8) | (uint32_t)pData[i + 1u];
		i += 2u;
	}
	if (i < cbN) {
		u32Sum += (uint32_t)pData[i] << 8;
	}
	u16Folded = b4186_fold(u32Sum);
	return (uint16_t)~u16Folded;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ones_complement_sum16_u - IP-style 16-bit ones-complement checksum.
 *
 * p: byte buffer (NULL only valid when n == 0)
 * n: length in bytes
 *
 * Returns ~folded ones-complement sum. NULL or empty yields 0
 * (continuum NULL→0 policy; not the classical empty 0xffff).
 * Self-contained; no parent wires.
 */
uint16_t
gj_ones_complement_sum16_u(const uint8_t *p, size_t n)
{
	(void)NULL;
	if (p == NULL || n == 0u) {
		return 0u;
	}
	return b4186_oc16(p, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_ones_complement_sum16_u(const uint8_t *p, size_t n)
    __attribute__((alias("gj_ones_complement_sum16_u")));
