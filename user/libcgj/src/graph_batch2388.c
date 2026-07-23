/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2388: 16-bit ones-complement sum unique surface
 * (post-2380 checksum exclusive wave).
 *
 * Surface (unique symbols):
 *   uint16_t gj_sum16_ones_comp(const void *data, size_t n);
 *     - Ones-complement 16-bit sum over data[0..n) as big-endian
 *       16-bit words (odd trailing byte treated as high byte with zero
 *       low). Carries folded back into the 16-bit field. Returns the
 *       folded sum (not inverted). NULL data with n != 0 -> 0.
 *       Empty (n == 0) -> 0.
 *   uint16_t __gj_sum16_ones_comp  (alias)
 *   __libcgj_batch2388_marker = "libcgj-batch2388"
 *
 * Post-2380 checksum exclusive wave (2381-2390). Internet-checksum
 * style fold without final invert - sum surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2388_marker[] = "libcgj-batch2388";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Fold high 16 bits of a running sum into the low 16 until the value
 * fits in 16 bits (ones-complement carry fold).
 */
static uint16_t
b2388_fold16(uint32_t uSum)
{
	while ((uSum >> 16) != 0u) {
		uSum = (uSum & 0xffffu) + (uSum >> 16);
	}
	return (uint16_t)uSum;
}

/* Ones-complement sum of cb bytes at p as big-endian 16-bit words. */
static uint16_t
b2388_sum16(const unsigned char *p, size_t cb)
{
	uint32_t uSum;
	size_t i;

	uSum = 0u;
	i = 0u;
	while (i + 1u < cb) {
		uSum += ((uint32_t)p[i] << 8) | (uint32_t)p[i + 1u];
		i += 2u;
	}
	if (i < cb) {
		uSum += (uint32_t)p[i] << 8;
	}
	return b2388_fold16(uSum);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sum16_ones_comp - ones-complement 16-bit sum over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0)
 * n:    byte count
 *
 * Returns the folded 16-bit ones-complement sum (not inverted).
 * NULL data with n != 0 yields 0. Empty input yields 0.
 * Does not call libc.
 */
uint16_t
gj_sum16_ones_comp(const void *pData, size_t cbN)
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
	return b2388_sum16(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_sum16_ones_comp(const void *pData, size_t cbN)
    __attribute__((alias("gj_sum16_ones_comp")));
