/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2657: gj_fletcher32_update_u - Fletcher-32 update.
 *
 * Surface (unique symbols):
 *   uint64_t gj_fletcher32_update_u(uint64_t state, const void *data,
 *                                   size_t n);
 *     - Fold n bytes into a running Fletcher-32 over little-endian
 *       16-bit words (mod 65535). u64 state layout:
 *         bits  0..15  sum1 (reduced 0..65534 between calls)
 *         bits 16..31  sum2 (reduced 0..65534 between calls)
 *         bits 32..39  pending odd byte (if any)
 *         bit  40      has_pending flag
 *       Start with 0 for a new stream. data == NULL or n == 0 → state
 *       unchanged. Odd trailing bytes are held in-state across updates.
 *   uint64_t __gj_fletcher32_update_u  (alias)
 *   __libcgj_batch2657_marker = "libcgj-batch2657"
 *
 * Streaming checksum wave (2651-2660). Distinct from one-shot
 * gj_fletcher32 (batch183). Companion: gj_fletcher32_final_u (batch2658).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2657_marker[] = "libcgj-batch2657";

/* 16-bit words between partial reductions. */
#define B2657_BLOCK 360u

#define B2657_SUM1_MASK   0x000000000000ffffull
#define B2657_SUM2_SHIFT  16
#define B2657_PEND_SHIFT  32
#define B2657_FLAG_SHIFT  40
#define B2657_HAS_PEND    (1ull << B2657_FLAG_SHIFT)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2657_mod65535_u64(uint64_t u64X)
{
	u64X = (u64X & 0xffffu) + (u64X >> 16);
	u64X = (u64X & 0xffffu) + (u64X >> 16);
	if (u64X >= 65535u) {
		u64X -= 65535u;
	}
	return (uint32_t)u64X;
}

static uint64_t
b2657_pack(uint64_t u64Sum1, uint64_t u64Sum2, unsigned char u8Pend,
    int fHasPend)
{
	uint32_t u32Sum1;
	uint32_t u32Sum2;
	uint64_t u64Out;

	u32Sum1 = b2657_mod65535_u64(u64Sum1);
	u32Sum2 = b2657_mod65535_u64(u64Sum2);
	u64Out = (uint64_t)u32Sum1 |
	    ((uint64_t)u32Sum2 << B2657_SUM2_SHIFT);
	if (fHasPend != 0) {
		u64Out |= ((uint64_t)u8Pend << B2657_PEND_SHIFT) | B2657_HAS_PEND;
	}
	return u64Out;
}

static uint64_t
b2657_update(uint64_t u64State, const unsigned char *pData, size_t cbN)
{
	uint64_t u64Sum1;
	uint64_t u64Sum2;
	unsigned char u8Pend;
	int fHasPend;
	size_t iOff;
	size_t cWords;
	size_t cChunk;
	size_t iWord;
	uint32_t u32Word;

	u64Sum1 = u64State & B2657_SUM1_MASK;
	u64Sum2 = (u64State >> B2657_SUM2_SHIFT) & 0xffffu;
	fHasPend = ((u64State & B2657_HAS_PEND) != 0ull) ? 1 : 0;
	u8Pend = (unsigned char)((u64State >> B2657_PEND_SHIFT) & 0xffu);
	iOff = 0u;

	/* Complete a pending half-word with the first new byte. */
	if (fHasPend != 0 && cbN > 0u) {
		u32Word = (uint32_t)u8Pend | ((uint32_t)pData[0] << 8);
		u64Sum1 += (uint64_t)u32Word;
		u64Sum2 += u64Sum1;
		iOff = 1u;
		fHasPend = 0;
		u8Pend = 0u;
	}

	cWords = (cbN - iOff) / 2u;
	while (cWords > 0u) {
		cChunk = cWords;
		if (cChunk > (size_t)B2657_BLOCK) {
			cChunk = (size_t)B2657_BLOCK;
		}
		for (iWord = 0u; iWord < cChunk; iWord++) {
			u32Word = (uint32_t)pData[iOff] |
			    ((uint32_t)pData[iOff + 1u] << 8);
			iOff += 2u;
			u64Sum1 += (uint64_t)u32Word;
			u64Sum2 += u64Sum1;
		}
		cWords -= cChunk;
		u64Sum1 = (u64Sum1 & 0xffffu) + (u64Sum1 >> 16);
		u64Sum2 = (u64Sum2 & 0xffffu) + (u64Sum2 >> 16);
	}

	/* Hold a trailing odd byte for a later update or final. */
	if (iOff < cbN) {
		u8Pend = pData[iOff];
		fHasPend = 1;
	}

	return b2657_pack(u64Sum1, u64Sum2, u8Pend, fHasPend);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fletcher32_update_u - fold n bytes into a running Fletcher-32 state.
 *
 * state: packed u64 (see file header); init 0 for a new stream
 * data:  input bytes (NULL is a no-op)
 * n:     byte count (0 is a no-op)
 *
 * Returns the updated packed state. Call gj_fletcher32_final_u for the
 * 32-bit wire value (applies any pending odd byte as high-half zero).
 */
uint64_t
gj_fletcher32_update_u(uint64_t u64State, const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	if (pData == NULL || cbN == 0u) {
		return u64State;
	}
	p = (const unsigned char *)pData;
	return b2657_update(u64State, p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_fletcher32_update_u(uint64_t u64State, const void *pData,
    size_t cbN)
    __attribute__((alias("gj_fletcher32_update_u")));
