/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch728: Internet checksum (RFC 1071) ones-complement.
 *
 * Surface (unique symbols for this TU):
 *   uint16_t gj_inet_checksum(const void *data, size_t n);
 *     — Ones-complement 16-bit Internet checksum over n bytes. 16-bit
 *       words are taken in network (big-endian) octet order; a trailing
 *       odd byte is padded with a zero low-order octet. Returns the
 *       value to store in a zeroed checksum field (host integer form of
 *       the 16-bit ones-complement result). Empty / NULL buffer yields
 *       0xffff.
 *   uint16_t __gj_inet_checksum  (alias)
 *   __libcgj_batch728_marker = "libcgj-batch728"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch728_marker[] = "libcgj-batch728";

/* ---- helpers ----------------------------------------------------------- */

/*
 * RFC 1071 §4.1: end-around carry fold of a multi-word sum into 16 bits,
 * then ones-complement. Zero partial sum → 0xffff (checksum of empty).
 */
static uint16_t
b728_fold(uint32_t uSum)
{
	while ((uSum >> 16) != 0u) {
		uSum = (uSum & 0xffffu) + (uSum >> 16);
	}
	return (uint16_t)~uSum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_inet_checksum — RFC 1071 Internet checksum over a byte buffer.
 *
 * data: buffer of n octets (may be NULL only when n == 0)
 * n:    number of bytes
 *
 * Words are summed as network-order 16-bit values. Odd final byte is
 * treated as a high-order octet with low-order zero padding.
 * NULL data with n > 0 is treated as empty (sum 0 → 0xffff).
 */
uint16_t
gj_inet_checksum(const void *data, size_t n)
{
	const unsigned char *pDat = (const unsigned char *)data;
	uint32_t uSum = 0u;
	size_t i;

	if (pDat == NULL || n == 0u) {
		return b728_fold(0u);
	}

	i = 0u;
	while (i + 1u < n) {
		uSum += ((uint32_t)pDat[i] << 8) | (uint32_t)pDat[i + 1u];
		i += 2u;
	}
	if (i < n) {
		/* Odd trailing byte: pad with zero low-order octet. */
		uSum += (uint32_t)pDat[i] << 8;
	}
	return b728_fold(uSum);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_inet_checksum(const void *data, size_t n)
    __attribute__((alias("gj_inet_checksum")));
