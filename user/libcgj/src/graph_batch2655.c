/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2655: gj_fletcher16_update_u - Fletcher-16 update.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fletcher16_update_u(uint32_t state, const void *data,
 *                                   size_t n);
 *     - Fold n bytes into a running Fletcher-16 state packed as
 *       (sum2 << 16) | sum1 with deferred modular reduction (mod 255).
 *       Start with 0 for a new stream. data == NULL or n == 0 → state
 *       unchanged.
 *   uint32_t __gj_fletcher16_update_u  (alias)
 *   __libcgj_batch2655_marker = "libcgj-batch2655"
 *
 * Streaming checksum wave (2651-2660). Distinct from one-shot
 * gj_fletcher16 (batch183). Companion: gj_fletcher16_final_u (batch2656).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2655_marker[] = "libcgj-batch2655";

/* Bytes between partial reductions (sums stay in uint32_t). */
#define B2655_BLOCK 256u

/* ---- freestanding helpers ---------------------------------------------- */

/* Fold x toward 0..254 via x % 255 == (x & 0xff) + (x >> 8). */
static uint32_t
b2655_mod255(uint32_t u32X)
{
	u32X = (u32X & 0xffu) + (u32X >> 8);
	u32X = (u32X & 0xffu) + (u32X >> 8);
	if (u32X >= 255u) {
		u32X -= 255u;
	}
	return u32X;
}

static uint32_t
b2655_update(uint32_t u32State, const unsigned char *pData, size_t cbN)
{
	uint32_t u32Sum1;
	uint32_t u32Sum2;
	size_t cbLeft;
	size_t cbChunk;
	size_t iByte;

	u32Sum1 = u32State & 0xffffu;
	u32Sum2 = (u32State >> 16) & 0xffffu;
	cbLeft = cbN;

	while (cbLeft > 0u) {
		cbChunk = cbLeft;
		if (cbChunk > (size_t)B2655_BLOCK) {
			cbChunk = (size_t)B2655_BLOCK;
		}
		for (iByte = 0u; iByte < cbChunk; iByte++) {
			u32Sum1 += (uint32_t)pData[iByte];
			u32Sum2 += u32Sum1;
		}
		pData += cbChunk;
		cbLeft -= cbChunk;
		u32Sum1 = (u32Sum1 & 0xffu) + (u32Sum1 >> 8);
		u32Sum2 = (u32Sum2 & 0xffu) + (u32Sum2 >> 8);
	}

	/* Keep packed halves tidy for the next update call. */
	u32Sum1 = b2655_mod255(u32Sum1);
	u32Sum2 = b2655_mod255(u32Sum2);
	return (u32Sum2 << 16) | u32Sum1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fletcher16_update_u - fold n bytes into a running Fletcher-16 state.
 *
 * state: packed (sum2 << 16) | sum1 (init 0 for a new stream)
 * data:  input bytes (NULL is a no-op)
 * n:     byte count (0 is a no-op)
 *
 * Returns the updated packed state. Call gj_fletcher16_final_u for the
 * 16-bit wire value.
 */
uint32_t
gj_fletcher16_update_u(uint32_t u32State, const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	if (pData == NULL || cbN == 0u) {
		return u32State;
	}
	p = (const unsigned char *)pData;
	return b2655_update(u32State, p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fletcher16_update_u(uint32_t u32State, const void *pData,
    size_t cbN)
    __attribute__((alias("gj_fletcher16_update_u")));
