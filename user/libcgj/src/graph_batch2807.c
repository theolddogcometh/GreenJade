/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2807: 16-bit XOR fold checksum (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_xor_fold16_u(const void *data, size_t n);
 *     - Fold n bytes into a 16-bit value by XOR. Bytes are paired as
 *       big-endian uint16 words; a trailing odd byte is XOR'd into the
 *       high byte lane (byte << 8). NULL data with n != 0 -> 0.
 *       Empty (n == 0) -> 0.
 *   uint16_t __gj_xor_fold16_u  (alias)
 *   __libcgj_batch2807_marker = "libcgj-batch2807"
 *
 * Post-2800 hash exclusive wave. Unique surface only — no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2807_marker[] = "libcgj-batch2807";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b2807_xor_fold16(const unsigned char *pData, size_t cbN)
{
	uint16_t u16Acc;
	size_t i;

	u16Acc = 0u;
	i = 0u;
	while (i + 1u < cbN) {
		u16Acc ^= (uint16_t)(((uint16_t)pData[i] << 8) |
		    (uint16_t)pData[i + 1u]);
		i += 2u;
	}
	if (i < cbN) {
		u16Acc ^= (uint16_t)((uint16_t)pData[i] << 8);
	}
	return u16Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xor_fold16_u - XOR-fold n bytes into a 16-bit checksum.
 *
 * data: input bytes (NULL only legal when n == 0)
 * n:    byte count
 *
 * Returns the 16-bit fold. NULL data with n != 0 yields 0 (defensive).
 * Empty input returns 0.
 */
uint16_t
gj_xor_fold16_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return 0u;
	}
	return b2807_xor_fold16(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_xor_fold16_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_xor_fold16_u")));
