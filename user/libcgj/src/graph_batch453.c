/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch453: freestanding Fletcher-32 checksum.
 *
 * Surface (unique symbols vs Fletcher-16):
 *   uint32_t gj_fletcher32_fold(const void *data, size_t n);
 *     — Fletcher-32 over n bytes as little-endian 16-bit words
 *       (modulus 65535). Odd trailing byte is zero-padded as a high byte
 *       of zero (i.e. the last word is just the low byte).
 *         sum1 = (sum1 + word) mod 65535
 *         sum2 = (sum2 + sum1) mod 65535
 *       Result = (sum2 << 16) | sum1. Empty/NULL → 0.
 *   uint32_t __gj_fletcher32_fold  (alias)
 *   __libcgj_batch453_marker = "libcgj-batch453"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Does not define Fletcher-16.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch453_marker[] = "libcgj-batch453";

/*
 * Max 16-bit words between reductions:
 * sum1 grows by at most 0xffff per word. uint64_t holds many; 360 is the
 * classic bound for safe lazy reduction into 32-bit-scale intermediates.
 */
#define B453_F32_BLOCK 360u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fletcher32_fold — one-shot Fletcher-32 over little-endian 16-bit words
 * assembled from the byte stream. An odd final byte becomes a word with
 * the high byte zero.
 *
 * Edge cases:
 *   data == NULL || n == 0 → 0
 */
uint32_t
gj_fletcher32_fold(const void *data, size_t n)
{
	const unsigned char *pBytes;
	uint64_t u64Sum1;
	uint64_t u64Sum2;
	size_t cWords;
	size_t cChunk;
	size_t i;
	uint32_t u32Word;

	if (data == NULL || n == 0u) {
		return 0u;
	}

	pBytes = (const unsigned char *)data;
	u64Sum1 = 0u;
	u64Sum2 = 0u;
	cWords = n / 2u;

	while (cWords > 0u) {
		cChunk = cWords;
		if (cChunk > (size_t)B453_F32_BLOCK) {
			cChunk = (size_t)B453_F32_BLOCK;
		}
		for (i = 0u; i < cChunk; i++) {
			u32Word = (uint32_t)pBytes[0] |
			    ((uint32_t)pBytes[1] << 8);
			pBytes += 2u;
			u64Sum1 += (uint64_t)u32Word;
			u64Sum2 += u64Sum1;
		}
		cWords -= cChunk;
		/* Reduce mod 65535: x % 65535 == (x & 0xffff) + (x >> 16). */
		u64Sum1 = (u64Sum1 & 0xffffu) + (u64Sum1 >> 16);
		u64Sum2 = (u64Sum2 & 0xffffu) + (u64Sum2 >> 16);
	}

	/* Trailing odd byte (zero-padded high half). */
	if ((n & 1u) != 0u) {
		u64Sum1 += (uint64_t)pBytes[0];
		u64Sum2 += u64Sum1;
	}

	/* Final fold into 0..65534. */
	u64Sum1 = (u64Sum1 & 0xffffu) + (u64Sum1 >> 16);
	u64Sum2 = (u64Sum2 & 0xffffu) + (u64Sum2 >> 16);
	u64Sum1 = (u64Sum1 & 0xffffu) + (u64Sum1 >> 16);
	u64Sum2 = (u64Sum2 & 0xffffu) + (u64Sum2 >> 16);
	if (u64Sum1 >= 65535u) {
		u64Sum1 -= 65535u;
	}
	if (u64Sum2 >= 65535u) {
		u64Sum2 -= 65535u;
	}

	return (uint32_t)((u64Sum2 << 16) | u64Sum1);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fletcher32_fold(const void *data, size_t n)
    __attribute__((alias("gj_fletcher32_fold")));
