/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4024: Adler-32 hash of a byte buffer.
 *
 * Surface (unique symbols):
 *   uint32_t gj_buf_hash_adler_u(const void *data, size_t n);
 *     - Adler-32 (ISO 3309 / zlib-shaped) over n bytes at data.
 *       BASE 65521, start s1=1 s2=0 (packed init 1).
 *       NULL data with n != 0 -> 0. Empty (n == 0) -> 1 (init).
 *   uint32_t __gj_buf_hash_adler_u  (alias)
 *   __libcgj_batch4024_marker = "libcgj-batch4024"
 *
 * Exclusive continuum CREATE-ONLY (4021-4030: str_hash_crc32c_u,
 * str_hash_adler_u, buf_hash_crc32c_u, buf_hash_adler_u, u32_mix_final_u,
 * u64_mix_final_u, u32_avalanche_u, u64_avalanche_u, u32_scramble_u,
 * batch_id_4030). Unique gj_buf_hash_adler_u surface only; no multi-def.
 * Distinct from gj_adler32_* (batch269) and adler32 (batch39) — no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4024_marker[] = "libcgj-batch4024";

/* Adler-32 prime modulus; deferred-mod block bound (zlib NMAX). */
#define B4024_BASE 65521u
#define B4024_NMAX 5552u

/* ---- freestanding helpers ---------------------------------------------- */

/* One-shot Adler-32 over n bytes. */
static uint32_t
b4024_buf_adler(const unsigned char *pData, size_t cbN)
{
	uint32_t uS1;
	uint32_t uS2;
	size_t cbLeft;
	size_t cbChunk;
	size_t i;

	uS1 = 1u;
	uS2 = 0u;
	cbLeft = cbN;
	while (cbLeft > 0u) {
		cbChunk = cbLeft;
		if (cbChunk > (size_t)B4024_NMAX) {
			cbChunk = (size_t)B4024_NMAX;
		}
		for (i = 0u; i < cbChunk; i++) {
			uS1 += (uint32_t)pData[i];
			uS2 += uS1;
		}
		pData += cbChunk;
		cbLeft -= cbChunk;
		uS1 %= B4024_BASE;
		uS2 %= B4024_BASE;
	}
	return (uS2 << 16) | uS1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_hash_adler_u - Adler-32 of n bytes at data.
 *
 * data: byte buffer (may be NULL only when n == 0)
 * n:    length in bytes
 *
 * Returns the packed Adler-32 checksum. NULL data with n != 0 yields 0.
 * Empty input (n == 0) yields 1 (canonical init). Does not call libc.
 * No parent wires.
 */
uint32_t
gj_buf_hash_adler_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return 1u;
	}
	return b4024_buf_adler(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_buf_hash_adler_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_buf_hash_adler_u")));
