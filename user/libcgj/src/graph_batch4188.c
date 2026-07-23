/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4188: one-shot Fletcher-32 checksum.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fletcher32_u(const uint8_t *p, size_t n);
 *     - Fletcher-32 over n bytes as little-endian 16-bit words
 *       (modulus 65535). Odd trailing byte is zero-padded as a high
 *       byte of zero (last word is just the low byte).
 *         sum1 = (sum1 + word) mod 65535
 *         sum2 = (sum2 + sum1) mod 65535
 *       Result = (sum2 << 16) | sum1. NULL or n == 0 → 0.
 *   uint32_t __gj_fletcher32_u  (alias)
 *   __libcgj_batch4188_marker = "libcgj-batch4188"
 *
 * Exclusive continuum CREATE-ONLY (4181-4190: u8_sum_checksum_u,
 * u16_sum_checksum_u, u32_sum_checksum_u, xor_reduce8_u, xor_reduce32_u,
 * ones_complement_sum16_u, fletcher16_u, fletcher32_u, luhn_u32_u,
 * batch_id_4190). Unique gj_fletcher32_u surface only; no multi-def.
 * Distinct from gj_fletcher32_fold / gj_fletcher32_update_u. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4188_marker[] = "libcgj-batch4188";

/*
 * Max 16-bit words between reductions:
 * sum1 grows by at most 0xffff per word. uint64_t holds many; 360 is the
 * classic bound for safe lazy reduction into 32-bit-scale intermediates.
 */
#define B4188_F32_BLOCK 360u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4188_mod65535_u64(uint64_t u64X)
{
	u64X = (u64X & 0xffffu) + (u64X >> 16);
	u64X = (u64X & 0xffffu) + (u64X >> 16);
	if (u64X >= 65535u) {
		u64X -= 65535u;
	}
	return (uint32_t)u64X;
}

static uint32_t
b4188_fletcher32(const uint8_t *pData, size_t cbN)
{
	uint64_t u64Sum1;
	uint64_t u64Sum2;
	size_t cWords;
	size_t cChunk;
	size_t i;
	uint32_t u32Word;
	const uint8_t *pBytes;

	pBytes = pData;
	u64Sum1 = 0u;
	u64Sum2 = 0u;
	cWords = cbN / 2u;

	while (cWords > 0u) {
		cChunk = cWords;
		if (cChunk > (size_t)B4188_F32_BLOCK) {
			cChunk = (size_t)B4188_F32_BLOCK;
		}
		for (i = 0u; i < cChunk; i++) {
			u32Word = (uint32_t)pBytes[0]
			    | ((uint32_t)pBytes[1] << 8);
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
	if ((cbN & 1u) != 0u) {
		u64Sum1 += (uint64_t)pBytes[0];
		u64Sum2 += u64Sum1;
	}

	return (b4188_mod65535_u64(u64Sum2) << 16)
	    | b4188_mod65535_u64(u64Sum1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fletcher32_u - one-shot Fletcher-32 over n bytes (LE 16-bit words).
 *
 * p: byte buffer (NULL only valid when n == 0)
 * n: length in bytes
 *
 * Returns (sum2 << 16) | sum1 with sums in 0..65534. NULL or empty → 0.
 * Self-contained; no parent wires.
 */
uint32_t
gj_fletcher32_u(const uint8_t *p, size_t n)
{
	(void)NULL;
	if (p == NULL || n == 0u) {
		return 0u;
	}
	return b4188_fletcher32(p, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fletcher32_u(const uint8_t *p, size_t n)
    __attribute__((alias("gj_fletcher32_u")));
