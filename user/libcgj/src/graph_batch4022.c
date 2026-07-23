/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4022: Adler-32 hash of a NUL-terminated string.
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_hash_adler_u(const char *s);
 *     - Adler-32 (ISO 3309 / zlib-shaped) over octets of s up to NUL.
 *       BASE 65521, start s1=1 s2=0 (packed init 1).
 *       s == NULL -> 0. Empty string "" -> 1 (init).
 *   uint32_t __gj_str_hash_adler_u  (alias)
 *   __libcgj_batch4022_marker = "libcgj-batch4022"
 *
 * Exclusive continuum CREATE-ONLY (4021-4030: str_hash_crc32c_u,
 * str_hash_adler_u, buf_hash_crc32c_u, buf_hash_adler_u, u32_mix_final_u,
 * u64_mix_final_u, u32_avalanche_u, u64_avalanche_u, u32_scramble_u,
 * batch_id_4030). Unique gj_str_hash_adler_u surface only; no multi-def.
 * Distinct from gj_adler32_* (batch269) and adler32 (batch39) — no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4022_marker[] = "libcgj-batch4022";

/* Adler-32 prime modulus; deferred-mod block bound (zlib NMAX). */
#define B4022_BASE 65521u
#define B4022_NMAX 5552u

/* ---- freestanding helpers ---------------------------------------------- */

/* One-shot Adler-32 over a NUL-terminated string (NUL not hashed). */
static uint32_t
b4022_str_adler(const char *sz)
{
	uint32_t uS1;
	uint32_t uS2;
	size_t i;
	size_t nChunk;

	uS1 = 1u;
	uS2 = 0u;
	i = 0u;
	while (sz[i] != '\0') {
		nChunk = 0u;
		while (sz[i] != '\0' && nChunk < (size_t)B4022_NMAX) {
			uS1 += (uint32_t)(unsigned char)sz[i];
			uS2 += uS1;
			i++;
			nChunk++;
		}
		uS1 %= B4022_BASE;
		uS2 %= B4022_BASE;
	}
	return (uS2 << 16) | uS1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_hash_adler_u - Adler-32 of a NUL-terminated string.
 *
 * s: NUL-terminated C string, or NULL
 *
 * Returns the packed Adler-32 checksum. NULL yields 0. Empty string
 * yields 1 (canonical init). Does not call libc. No parent wires.
 */
uint32_t
gj_str_hash_adler_u(const char *s)
{
	(void)NULL;
	if (s == NULL) {
		return 0u;
	}
	return b4022_str_adler(s);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_hash_adler_u(const char *s)
    __attribute__((alias("gj_str_hash_adler_u")));
