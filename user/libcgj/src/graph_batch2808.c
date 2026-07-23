/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2808: 32-bit XOR fold checksum (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_xor_fold32_u(const void *data, size_t n);
 *     - Fold n bytes into a 32-bit value by XOR. Bytes are packed as
 *       big-endian uint32 words (up to 4 bytes per word); a short tail
 *       is left-aligned in the high bytes. NULL data with n != 0 -> 0.
 *       Empty (n == 0) -> 0.
 *   uint32_t __gj_xor_fold32_u  (alias)
 *   __libcgj_batch2808_marker = "libcgj-batch2808"
 *
 * Post-2800 hash exclusive wave. Unique surface only — no multi-def.
 * Companion to gj_xor_fold16_u (batch2807).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2808_marker[] = "libcgj-batch2808";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2808_xor_fold32(const unsigned char *pData, size_t cbN)
{
	uint32_t u32Acc;
	size_t i;

	u32Acc = 0u;
	i = 0u;
	while (i + 3u < cbN) {
		u32Acc ^= ((uint32_t)pData[i] << 24) |
		    ((uint32_t)pData[i + 1u] << 16) |
		    ((uint32_t)pData[i + 2u] << 8) |
		    (uint32_t)pData[i + 3u];
		i += 4u;
	}
	if (i < cbN) {
		uint32_t u32Tail;
		unsigned uShift;

		u32Tail = 0u;
		uShift = 24u;
		while (i < cbN) {
			u32Tail |= (uint32_t)pData[i] << uShift;
			uShift -= 8u;
			i++;
		}
		u32Acc ^= u32Tail;
	}
	return u32Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xor_fold32_u - XOR-fold n bytes into a 32-bit checksum.
 *
 * data: input bytes (NULL only legal when n == 0)
 * n:    byte count
 *
 * Returns the 32-bit fold. NULL data with n != 0 yields 0 (defensive).
 * Empty input returns 0.
 */
uint32_t
gj_xor_fold32_u(const void *pData, size_t cbN)
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
	return b2808_xor_fold32(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xor_fold32_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_xor_fold32_u")));
