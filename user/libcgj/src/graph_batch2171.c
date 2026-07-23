/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2171: Internet checksum partial accumulation (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_inet_csum_partial_u(const void *data, size_t len, uint32_t sum);
 *     - Add network-order 16-bit words from data[0..len) into sum (RFC 1071
 *       partial). Odd trailing byte is a high-order octet with low zero.
 *       Returns a 32-bit partial sum (not folded, not ones-complemented).
 *       NULL data with len > 0 is treated as empty contribution.
 *   uint32_t __gj_inet_csum_partial_u  (alias)
 *   __libcgj_batch2171_marker = "libcgj-batch2171"
 *
 * Post-2170 net checksum exclusive wave (2171-2180). Distinct from
 * gj_inet_cksum (batch347) and gj_ip_checksum (batch124) which fully
 * fold and complement - partial surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2171_marker[] = "libcgj-batch2171";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Accumulate ones-complement 16-bit network-order words into a 32-bit
 * partial sum. Does not fold carries out of 16 bits and does not
 * ones-complement the result (caller uses fold/finish).
 */
static uint32_t
b2171_partial(const unsigned char *pDat, size_t cb, uint32_t u32Sum)
{
	size_t i;

	if (pDat == NULL || cb == 0u) {
		return u32Sum;
	}

	i = 0u;
	while (i + 1u < cb) {
		u32Sum += ((uint32_t)pDat[i] << 8) | (uint32_t)pDat[i + 1u];
		i += 2u;
	}
	if (i < cb) {
		/* Odd trailing byte: pad with zero low-order octet. */
		u32Sum += (uint32_t)pDat[i] << 8;
	}
	return u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_inet_csum_partial_u - RFC 1071 partial Internet checksum accumulation.
 *
 * data: buffer of len octets (may be NULL when len == 0, or empty contrib)
 * len:  number of bytes to cover
 * sum:  incoming partial sum (0 for a fresh accumulation)
 *
 * Returns the updated 32-bit partial sum. Pair with gj_inet_csum_fold_u
 * or gj_inet_csum_finish_u to obtain a store-form 16-bit checksum.
 */
uint32_t
gj_inet_csum_partial_u(const void *data, size_t len, uint32_t sum)
{
	const unsigned char *pDat = (const unsigned char *)data;

	(void)NULL;
	return b2171_partial(pDat, len, sum);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_inet_csum_partial_u(const void *data, size_t len, uint32_t sum)
    __attribute__((alias("gj_inet_csum_partial_u")));
