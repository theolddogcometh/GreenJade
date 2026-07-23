/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch124: Internet checksum (RFC 1071).
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party checksum source was copied.
 *
 * Surface (unique symbols):
 *   uint16_t gj_ip_checksum(const void *data, size_t len);
 *     Ones-complement 16-bit Internet checksum over len bytes. Words are
 *     taken in network (big-endian) order; a trailing odd byte is padded
 *     with a zero low-order octet. Returns the value to store in a
 *     zeroed checksum field (host integer form of the 16-bit result).
 *   uint16_t gj_ip_checksum_fold(uint32_t sum);
 *     Fold carries of a partial 32-bit sum into 16 bits and ones-complement
 *     (RFC 1071 §4.1 end-around carry).
 *   int gj_ipv4_header_ok(const void *hdr, size_t len);
 *     Validate IPv4 header: version == 4, IHL >= 5, len covers IHL*4
 *     bytes, and header checksum verifies (sum including checksum field
 *     folds to 0xffff → gj_ip_checksum returns 0). Returns 1 if ok, 0 else.
 *   __gj_ip_checksum / __gj_ip_checksum_fold / __gj_ipv4_header_ok
 *   __libcgj_batch124_marker = "libcgj-batch124"
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch124_marker[] = "libcgj-batch124";

/* ---- fold -------------------------------------------------------------- */

/*
 * End-around carry fold of a multi-word sum, then ones-complement.
 * Empty / zero partial sum → 0xffff (checksum of zero-length buffer).
 */
uint16_t
gj_ip_checksum_fold(uint32_t sum)
{
	uint32_t uSum = sum;

	/* RFC 1071: fold until only 16 bits of significance remain. */
	while ((uSum >> 16) != 0u) {
		uSum = (uSum & 0xffffu) + (uSum >> 16);
	}
	return (uint16_t)~uSum;
}

/* ---- ones-complement sum over a buffer --------------------------------- */

/*
 * Sum 16-bit network-order words from data[0..len). Odd final byte is
 * treated as a high-order octet with low-order zero padding.
 * NULL data with len > 0 is treated as empty (sum 0).
 */
uint16_t
gj_ip_checksum(const void *data, size_t len)
{
	const unsigned char *pDat = (const unsigned char *)data;
	uint32_t uSum = 0u;
	size_t i;

	if (pDat == NULL || len == 0u) {
		return gj_ip_checksum_fold(0u);
	}

	i = 0u;
	while (i + 1u < len) {
		uSum += ((uint32_t)pDat[i] << 8) | (uint32_t)pDat[i + 1u];
		i += 2u;
	}
	if (i < len) {
		/* Odd trailing byte: pad with zero low-order octet. */
		uSum += (uint32_t)pDat[i] << 8;
	}
	return gj_ip_checksum_fold(uSum);
}

/* ---- IPv4 header validation -------------------------------------------- */

/*
 * Minimal IPv4 header checks for version, IHL, length, and RFC 1071
 * header checksum. Does not inspect total length vs outer buffer beyond
 * covering the header itself.
 */
int
gj_ipv4_header_ok(const void *hdr, size_t len)
{
	const unsigned char *pHdr = (const unsigned char *)hdr;
	unsigned uVer;
	unsigned uIhl;
	size_t cbHdr;

	if (pHdr == NULL || len < 20u) {
		return 0;
	}

	uVer = (unsigned)(pHdr[0] >> 4);
	uIhl = (unsigned)(pHdr[0] & 0x0fu);
	if (uVer != 4u || uIhl < 5u) {
		return 0;
	}

	cbHdr = (size_t)uIhl * 4u;
	if (len < cbHdr) {
		return 0;
	}

	/*
	 * With the stored checksum included, a correct header yields a
	 * ones-complement sum of 0xffff, so the folded checksum is 0.
	 */
	if (gj_ip_checksum(pHdr, cbHdr) != 0u) {
		return 0;
	}
	return 1;
}

/* ---- underscored aliases ----------------------------------------------- */

uint16_t __gj_ip_checksum(const void *data, size_t len)
    __attribute__((alias("gj_ip_checksum")));
uint16_t __gj_ip_checksum_fold(uint32_t sum)
    __attribute__((alias("gj_ip_checksum_fold")));
int __gj_ipv4_header_ok(const void *hdr, size_t len)
    __attribute__((alias("gj_ipv4_header_ok")));
