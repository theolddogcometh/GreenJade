/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch348: IPv4 TCP/UDP pseudo-header checksum
 * (RFC 768 / RFC 793 / RFC 1071).
 *
 * Surface (unique symbols):
 *   uint16_t gj_tcp_v4_cksum(const uint8_t src[4], const uint8_t dst[4],
 *                            uint8_t proto, const void *seg, size_t seglen);
 *     — Ones-complement 16-bit transport checksum over the IPv4
 *       pseudo-header plus the TCP or UDP segment at seg[0..seglen).
 *       Pseudo-header fields (network octet order):
 *         source address (4), destination address (4),
 *         zero (1), protocol (1), segment length (2, low 16 bits of seglen).
 *       Segment words are taken in network (big-endian) octet order; a
 *       trailing odd byte is padded with a zero low-order octet. Returns
 *       the value to store in a zeroed checksum field (host integer form
 *       of the 16-bit ones-complement result). Empty contribution
 *       (NULL/zero addresses, seglen == 0, NULL seg) yields the fold of
 *       whatever partial sum remains; all-zero input → 0xffff.
 *   uint16_t __gj_tcp_v4_cksum  (alias)
 *   __libcgj_batch348_marker = "libcgj-batch348"
 *
 * Distinct from batch124 gj_ip_checksum and batch347 gj_inet_cksum
 * (those are bare-buffer Internet checksums without a pseudo-header).
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch348_marker[] = "libcgj-batch348";

/* ---- helpers ----------------------------------------------------------- */

/*
 * RFC 1071 §4.1: end-around carry fold of a multi-word sum into 16 bits,
 * then ones-complement. Zero partial sum → 0xffff (checksum of empty).
 */
static uint16_t
b348_fold(uint32_t uSum)
{
	while ((uSum >> 16) != 0u) {
		uSum = (uSum & 0xffffu) + (uSum >> 16);
	}
	return (uint16_t)~uSum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tcp_v4_cksum — IPv4 TCP/UDP pseudo-header + segment checksum.
 *
 * src:    IPv4 source address octets (network order); NULL → 0.0.0.0
 * dst:    IPv4 destination address octets (network order); NULL → 0.0.0.0
 * proto:  IP protocol number (e.g. 6 = TCP, 17 = UDP)
 * seg:    transport segment (header + payload); may be NULL if seglen == 0
 * seglen: byte length of the segment (also the pseudo-header length field,
 *         truncated to 16 bits)
 *
 * Caller must zero the checksum field inside seg when computing the value
 * to store. For verification, leave the field as received; a correct
 * packet yields 0 after fold (ones-complement sum of 0xffff).
 * NULL seg with seglen > 0 is treated as empty segment data (sum 0 for body).
 */
uint16_t
gj_tcp_v4_cksum(const uint8_t src[4], const uint8_t dst[4], uint8_t proto,
                const void *seg, size_t seglen)
{
	const unsigned char *pSeg;
	uint32_t uSum = 0u;
	uint32_t uLenField;
	size_t i;

	/* Pseudo-header: source address (2 × 16-bit BE words). */
	if (src != NULL) {
		uSum += ((uint32_t)src[0] << 8) | (uint32_t)src[1];
		uSum += ((uint32_t)src[2] << 8) | (uint32_t)src[3];
	}

	/* Pseudo-header: destination address. */
	if (dst != NULL) {
		uSum += ((uint32_t)dst[0] << 8) | (uint32_t)dst[1];
		uSum += ((uint32_t)dst[2] << 8) | (uint32_t)dst[3];
	}

	/* Pseudo-header: zero | protocol (high octet zero). */
	uSum += (uint32_t)proto;

	/* Pseudo-header: TCP/UDP length (16-bit; low bits of seglen). */
	uLenField = (uint32_t)(seglen & 0xffffu);
	uSum += uLenField;

	/* Segment body (header + data). */
	pSeg = (const unsigned char *)seg;
	if (pSeg != NULL && seglen > 0u) {
		i = 0u;
		while (i + 1u < seglen) {
			uSum += ((uint32_t)pSeg[i] << 8) | (uint32_t)pSeg[i + 1u];
			i += 2u;
		}
		if (i < seglen) {
			/* Odd trailing byte: pad with zero low-order octet. */
			uSum += (uint32_t)pSeg[i] << 8;
		}
	}

	return b348_fold(uSum);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_tcp_v4_cksum(const uint8_t src[4], const uint8_t dst[4],
                           uint8_t proto, const void *seg, size_t seglen)
    __attribute__((alias("gj_tcp_v4_cksum")));
