/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4461: IPv4/IP header Internet checksum (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_ip_hdr_checksum_u(const uint8_t *hdr, size_t n);
 *     - Ones-complement sum of n octets of network-order 16-bit words,
 *       then ones-complement (~). Odd trailing byte is a high-order
 *       octet with low zero. NULL hdr returns 0 (empty not 0xffff).
 *   uint16_t __gj_ip_hdr_checksum_u  (alias)
 *   __libcgj_batch4461_marker = "libcgj-batch4461"
 *
 * Exclusive continuum CREATE-ONLY (4461-4470: ip_hdr_checksum_u,
 * udp_pseudo_fold_u, csum_add_u, csum_fold_u, csum_finish_u, htons_u,
 * ntohs_u, htonl_u, ntohl_u, batch_id_4470). Distinct from
 * gj_ip_checksum (batch124), gj_ip_hdr_checksum_ok (batch1491), and
 * gj_inet_checksum (batch728) — unique _u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4461_marker[] = "libcgj-batch4461";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * RFC 1071 ones-complement sum of network-order words, then invert.
 * NULL pointer short-circuits to 0 (caller contract for this surface).
 */
static uint16_t
b4461_hdr_csum(const uint8_t *pHdr, size_t cb)
{
	uint32_t u32Sum;
	size_t i;

	if (pHdr == NULL) {
		return 0u;
	}

	u32Sum = 0u;
	i = 0u;
	while (i + 1u < cb) {
		u32Sum += ((uint32_t)pHdr[i] << 8) | (uint32_t)pHdr[i + 1u];
		i += 2u;
	}
	if (i < cb) {
		u32Sum += (uint32_t)pHdr[i] << 8;
	}

	while ((u32Sum >> 16) != 0u) {
		u32Sum = (u32Sum & 0xffffu) + (u32Sum >> 16);
	}
	return (uint16_t)~u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ip_hdr_checksum_u - Internet checksum over an IP header buffer.
 *
 * hdr: pointer to header octets (NULL → 0)
 * n:   byte length to cover
 *
 * Returns store-form 16-bit checksum (ones-complement of folded sum).
 * Self-contained; no parent wires.
 */
uint16_t
gj_ip_hdr_checksum_u(const uint8_t *hdr, size_t n)
{
	(void)NULL;
	return b4461_hdr_csum(hdr, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_ip_hdr_checksum_u(const uint8_t *hdr, size_t n)
    __attribute__((alias("gj_ip_hdr_checksum_u")));
