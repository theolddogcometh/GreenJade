/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2653: gj_adler32_update_u - Adler-32 stream update.
 *
 * Surface (unique symbols):
 *   uint32_t gj_adler32_update_u(uint32_t state, const void *data, size_t n);
 *     - Fold n bytes into a running Adler-32 packed as
 *       (s2 << 16) | s1 with BASE 65521. Start with 1 for a new stream.
 *       data == NULL or n == 0 → state unchanged.
 *   uint32_t __gj_adler32_update_u  (alias)
 *   __libcgj_batch2653_marker = "libcgj-batch2653"
 *
 * Streaming checksum wave (2651-2660). Distinct from gj_adler32_update
 * (batch269) — unique _u surface; pure incremental no-op on NULL.
 * Companion finalizer: gj_adler32_final_u (batch2654).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2653_marker[] = "libcgj-batch2653";

/* Adler-32 prime modulus. */
#define B2653_BASE 65521u
/* Largest n where deferred sums fit in uint32_t (zlib NMAX). */
#define B2653_NMAX 5552u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2653_update(uint32_t u32State, const unsigned char *pData, size_t cbN)
{
	uint32_t u32S1;
	uint32_t u32S2;
	size_t cbLeft;
	size_t cbChunk;
	size_t iByte;

	u32S1 = u32State & 0xffffu;
	u32S2 = (u32State >> 16) & 0xffffu;
	cbLeft = cbN;

	while (cbLeft > 0u) {
		cbChunk = cbLeft;
		if (cbChunk > (size_t)B2653_NMAX) {
			cbChunk = (size_t)B2653_NMAX;
		}
		for (iByte = 0u; iByte < cbChunk; iByte++) {
			u32S1 += (uint32_t)pData[iByte];
			u32S2 += u32S1;
		}
		pData += cbChunk;
		cbLeft -= cbChunk;
		u32S1 %= B2653_BASE;
		u32S2 %= B2653_BASE;
	}

	return (u32S2 << 16) | u32S1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_adler32_update_u - fold n bytes into a running Adler-32 state.
 *
 * state: packed (s2 << 16) | s1 (init 1 for a new stream)
 * data:  input bytes (NULL is a no-op)
 * n:     byte count (0 is a no-op)
 *
 * Returns the updated packed state. Call gj_adler32_final_u when done
 * (identity finalize for Adler-32).
 */
uint32_t
gj_adler32_update_u(uint32_t u32State, const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	if (pData == NULL || cbN == 0u) {
		return u32State;
	}
	p = (const unsigned char *)pData;
	return b2653_update(u32State, p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_adler32_update_u(uint32_t u32State, const void *pData,
    size_t cbN)
    __attribute__((alias("gj_adler32_update_u")));
