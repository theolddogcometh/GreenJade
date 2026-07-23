/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4369: gj_adler32_buf_u - Adler-32 buffer update.
 *
 * Surface (unique symbols):
 *   uint32_t gj_adler32_buf_u(const void *p, size_t n, uint32_t init);
 *     - Fold n bytes into a running Adler-32 starting from init, packed as
 *       (s2 << 16) | s1 with BASE 65521. p == NULL → return init.
 *       Init 1 for a new stream.
 *   uint32_t __gj_adler32_buf_u  (alias)
 *   __libcgj_batch4369_marker = "libcgj-batch4369"
 *
 * Exclusive continuum CREATE-ONLY (4361-4370). Companion single-byte
 * update is gj_adler32_update_byte_u (batch4368). Distinct from
 * gj_adler32_update_u (batch2653) / gj_adler32_n (batch783) — unique
 * gj_adler32_buf_u surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4369_marker[] = "libcgj-batch4369";

/* Adler-32 prime modulus. */
#define B4369_BASE 65521u
/* Largest n where deferred sums fit in uint32_t (zlib NMAX). */
#define B4369_NMAX 5552u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4369_buf(const unsigned char *pData, size_t cbN, uint32_t u32Init)
{
	uint32_t u32S1;
	uint32_t u32S2;
	size_t cbLeft;
	size_t cbChunk;
	size_t iByte;

	u32S1 = u32Init & 0xffffu;
	u32S2 = (u32Init >> 16) & 0xffffu;
	cbLeft = cbN;

	while (cbLeft > 0u) {
		cbChunk = cbLeft;
		if (cbChunk > (size_t)B4369_NMAX) {
			cbChunk = (size_t)B4369_NMAX;
		}
		for (iByte = 0u; iByte < cbChunk; iByte++) {
			u32S1 += (uint32_t)pData[iByte];
			u32S2 += u32S1;
		}
		pData += cbChunk;
		cbLeft -= cbChunk;
		u32S1 %= B4369_BASE;
		u32S2 %= B4369_BASE;
	}

	return (u32S2 << 16) | u32S1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_adler32_buf_u - fold n bytes into Adler-32 starting from init.
 *
 * p:    input bytes (NULL → return init)
 * n:    byte count (0 → return init)
 * init: packed (s2 << 16) | s1 (1 for a new stream)
 *
 * Returns the updated packed state. Self-contained; no parent wires.
 */
uint32_t
gj_adler32_buf_u(const void *pData, size_t cbN, uint32_t u32Init)
{
	const unsigned char *p;

	(void)NULL;
	if (pData == NULL) {
		return u32Init;
	}
	p = (const unsigned char *)pData;
	return b4369_buf(p, cbN, u32Init);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_adler32_buf_u(const void *pData, size_t cbN, uint32_t u32Init)
    __attribute__((alias("gj_adler32_buf_u")));
