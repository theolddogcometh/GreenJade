/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1491: verify Internet checksum of a buffer.
 *
 * Surface (unique symbols):
 *   int gj_ip_hdr_checksum_ok(const void *data, size_t len);
 *     — Ones-complement 16-bit Internet checksum verification (RFC 1071)
 *       over len bytes at data. Words are taken in network (big-endian)
 *       order; a trailing odd byte is padded with a zero low-order octet.
 *       With the stored checksum field included, a correct buffer folds
 *       to ones-complement sum 0xffff, so the store-form checksum is 0.
 *       Returns 1 if ok, 0 otherwise (NULL, empty, or bad sum).
 *   int __gj_ip_hdr_checksum_ok  (alias)
 *   __libcgj_batch1491_marker = "libcgj-batch1491"
 *
 * Distinct from gj_ip_checksum / gj_ipv4_header_ok (batch124) and
 * gj_inet_cksum (batch347) — verification surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1491_marker[] = "libcgj-batch1491";

/* ---- helpers ----------------------------------------------------------- */

/*
 * RFC 1071 §4.1: end-around carry fold, then ones-complement.
 * Zero partial sum → 0xffff.
 */
static uint16_t
b1491_fold(uint32_t u32Sum)
{
	while ((u32Sum >> 16) != 0u) {
		u32Sum = (u32Sum & 0xffffu) + (u32Sum >> 16);
	}
	return (uint16_t)~u32Sum;
}

/*
 * Store-form Internet checksum over cb bytes (network-order words).
 * NULL / zero length → fold of 0 (0xffff).
 */
static uint16_t
b1491_cksum(const unsigned char *pDat, size_t cb)
{
	uint32_t u32Sum = 0u;
	size_t i;

	if (pDat == NULL || cb == 0u) {
		return b1491_fold(0u);
	}

	i = 0u;
	while (i + 1u < cb) {
		u32Sum += ((uint32_t)pDat[i] << 8) | (uint32_t)pDat[i + 1u];
		i += 2u;
	}
	if (i < cb) {
		u32Sum += (uint32_t)pDat[i] << 8;
	}
	return b1491_fold(u32Sum);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ip_hdr_checksum_ok — verify RFC 1071 Internet checksum of a buffer.
 *
 * data: buffer including the checksum field as stored on the wire
 * len:  byte length of the covered region
 *
 * Returns 1 if the ones-complement sum verifies (store-form checksum is
 * 0), else 0. Does not parse IPv4 version/IHL — pure buffer verify.
 */
int
gj_ip_hdr_checksum_ok(const void *data, size_t len)
{
	const unsigned char *pDat = (const unsigned char *)data;

	if (pDat == NULL || len == 0u) {
		return 0;
	}
	if (b1491_cksum(pDat, len) != 0u) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ip_hdr_checksum_ok(const void *data, size_t len)
    __attribute__((alias("gj_ip_hdr_checksum_ok")));
